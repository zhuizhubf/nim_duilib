-- =============================================================================
-- 通用 clang-format 封装（引擎 + `xmake format` 就地格式化任务）
--
-- 设计目标：与具体项目的目录布局无关，复制到任意项目都能直接用。
--   * 默认扫描整个项目，自动跳过隐藏目录、构建产物、第三方依赖与工具链目录；
--   * 优先使用项目自带的 clang-format（tools/、scripts/ 或项目根目录），否则回退 PATH；
--   * 并行执行、按批调用，并在应用后复查，只对残留文件再应用（最多 N 轮，默认 3）；
--   * 支持只处理指定路径（见下方"用法"）。
--
-- 在 xmake.lua 中注册任务（脚本与本文件放在同一目录，模块名按该目录解析）：
--     task("format")
--         on_run("scripts.format_apply")
--         set_menu { usage = "xmake format", description = "..." }
--     task_end()
--
-- 用法：
--     xmake format                       -- 格式化整个项目
--     XMAKE_FORMAT_PATHS=src,examples xmake format
--                                        -- 只处理指定目录/文件（xmake 任务不接受位置参数）
--     xmake l xmake/scripts/format_apply.lua src       -- 脚本直跑形式，可带路径参数
--
-- 环境变量（全部可选）：
--   XMAKE_FORMAT_BIN         clang-format 可执行文件路径
--   XMAKE_FORMAT_STYLE       --style 取值，默认 file（就近读取 .clang-format）
--   XMAKE_FORMAT_NJOB        并行进程数，默认 CPU 核心数
--   XMAKE_FORMAT_BATCH       单个进程一批处理几个文件，默认 128（批量调用可显著减少进程开销）
--   XMAKE_FORMAT_ROUNDS      应用 + 复查的最大轮数，默认 3
--   XMAKE_FORMAT_EXTENSIONS  参与格式化的扩展名，默认 C/C++/CUDA 常见扩展名
--                            （如需处理 Objective-C，可设为 "m,mm,h,cpp"，并要求 .clang-format 支持 ObjC）
--   XMAKE_FORMAT_SKIP        额外跳过的目录名（分号或逗号分隔，支持 `cmake-*` 前缀通配）
--   XMAKE_FORMAT_PATHS       默认扫描路径（分号或逗号分隔，默认项目根目录）
--   XMAKE_FORMAT_VERBOSE     非 0 时打印每个被处理/违规的文件
--   XMAKE_FORMAT_STRICT      非 0 时，"配置不支持的文件"也会让任务失败（默认只提示并跳过）
-- =============================================================================

import("async.runjobs", { alias = "runjobs" })

-- -----------------------------------------------------------------------------
-- 默认配置
-- -----------------------------------------------------------------------------

-- 默认参与格式化的扩展名（clang-format 支持的文本源码）
local DEFAULT_EXTENSIONS = {
    "c", "cc", "cpp", "cxx", "c++", "cppm",
    "h", "hh", "hpp", "hxx", "h++",
    "inl", "ipp", "ixx", "tcc", "tlh", "tli",
    "cu", "cuh",
}

-- 默认跳过的目录名（不区分大小写，`前缀*` 表示前缀匹配）
local DEFAULT_SKIP_DIRS = {
    "build", "build-*", "cmake-build-*", "out", "output", "dist", "obj", "objects",
    "third_party", "third-party", "thirdparty", "3rd", "3rdparty", "3rd-party", "3rd_party",
    "external", "extern", "vendor", "deps", "dependencies", "node_modules",
    -- 工具链/脚本目录（如随项目携带的 clang-format、clang-tidy、cppcheck 等）
    "tools",
}

-- clang-format 的违规诊断特征串（--dry-run --Werror 的输出）
local VIOLATION_HINT = "error: code should be clang-formatted"

-- -----------------------------------------------------------------------------
-- 基础工具
-- -----------------------------------------------------------------------------

local function _env(name, default)
    local value = os.getenv(name)
    if not value then
        return default
    end
    value = value:trim()
    if #value == 0 then
        return default
    end
    return value
end

local function _env_int(name, default)
    local value = tonumber(_env(name))
    if value and value > 0 then
        return math.floor(value)
    end
    return default
end

local function _env_list(name)
    local list = {}
    local value = _env(name)
    if not value then
        return list
    end
    for _, item in ipairs(value:split("[;,]")) do
        item = item:trim()
        if #item > 0 then
            table.insert(list, item)
        end
    end
    return list
end

-- 统一成正斜杠，便于跨平台比较与展示
local function _slash(p)
    return tostring(p):gsub("\\", "/")
end

-- 打印相对项目根目录的路径，便于阅读
local function _rel(file)
    local rel = path.relative(file, os.projectdir())
    if rel and #rel > 0 then
        return _slash(rel)
    end
    return _slash(file)
end

-- 归一化路径，用于诊断输出与文件列表的比对
local function _key(file)
    local p = _slash(path.absolute(file))
    if os.host() == "windows" then
        p = p:lower()
    end
    return p
end

-- 取路径最后一段（不能用 path.basename：它会把 ".git" 这类名字当成扩展名）
local function _name(p)
    return tostring(p):match("[^/\\]+$") or tostring(p)
end

-- 进度打印（最多约 10 行，避免刷屏）
local function _progress(label, total)
    local step = math.max(1, math.floor(total / 10))
    local nextat, done = step, 0
    return function (count)
        done = done + count
        if done >= nextat or done >= total then
            print(string.format("  %s：%d/%d", label, math.min(done, total), total))
            nextat = done + step
        end
    end
end

local function _chunks(files, size)
    local groups = {}
    for i = 1, #files, size do
        local group = {}
        for j = i, math.min(i + size - 1, #files) do
            table.insert(group, files[j])
        end
        table.insert(groups, group)
    end
    return groups
end

-- 默认并行度：优先使用 xmake 提供的 CPU 核数，并兼容旧版本
local function _default_njob()
    if type(os.default_njob) == "function" then
        local njob = os.default_njob()
        if type(njob) == "number" and njob > 0 then
            return njob
        end
    end
    if os.cpuinfo then
        local ncpu = os.cpuinfo("ncpu")
        if type(ncpu) == "number" and ncpu > 0 then
            return ncpu
        end
    end
    return 4
end

-- -----------------------------------------------------------------------------
-- clang-format 与风格配置定位
-- -----------------------------------------------------------------------------

local function _find_bin()
    local configured = _env("XMAKE_FORMAT_BIN")
    if configured then
        return configured
    end
    local names
    if os.host() == "windows" then
        names = { "clang-format.exe", "clang-format" }
    else
        names = { "clang-format", "clang-format.exe" }
    end
    local dirs = {
        path.join(os.projectdir(), "tools"),
        path.join(os.projectdir(), "scripts"),
        path.join(os.projectdir(), "scripts", "tools"),
        os.projectdir(),
    }
    for _, dir in ipairs(dirs) do
        for _, name in ipairs(names) do
            local bin = path.join(dir, name)
            if os.isfile(bin) then
                return bin
            end
        end
    end
    -- 回退到 PATH 上的 clang-format
    return "clang-format"
end

local function _version(bin)
    local version, message
    try{
        function ()
            version = (os.iorunv(bin, { "--version" }) or ""):trim()
        end,
        catch{
            function (errors)
                message = tostring(errors)
            end
        }
    }
    if not version then
        raise(string.format("无法执行 clang-format：%s\n%s\n" ..
            "可安装 clang-format，或用环境变量 XMAKE_FORMAT_BIN 指定可执行文件；" ..
            "也可以把 clang-format 放到项目的 tools/ 目录下。",
            bin, (message or ""):trim()))
    end
    return version
end

-- 从给定目录向上查找 .clang-format
local function _find_config(startdir)
    local dir = path.absolute(startdir)
    for _ = 1, 32 do
        for _, name in ipairs({ ".clang-format", "_clang-format" }) do
            local file = path.join(dir, name)
            if os.isfile(file) then
                return file
            end
        end
        local parent = path.directory(dir)
        if not parent or #parent == 0 or parent == dir then
            break
        end
        dir = parent
    end
    return nil
end

-- clang-format 10 才引入 --dry-run，老版本需要退化为"输出对比"
local function _supports_dry_run(bin)
    local supported = false
    try{
        function ()
            local help = os.iorunv(bin, { "--help" }) or ""
            supported = help:find("dry-run", 1, true) ~= nil
        end,
        catch{
            function ()
                supported = false
            end
        }
    }
    return supported
end

-- -----------------------------------------------------------------------------
-- 源码收集
-- -----------------------------------------------------------------------------

local function _extensions()
    local names = _env_list("XMAKE_FORMAT_EXTENSIONS")
    if #names == 0 then
        names = DEFAULT_EXTENSIONS
    end
    local exts = {}
    for _, name in ipairs(names) do
        name = name:lower()
        if name:startswith(".") then
            name = name:sub(2)
        end
        exts[name] = true
    end
    return exts
end

local function _skip_dirs()
    local names = {}
    for _, name in ipairs(DEFAULT_SKIP_DIRS) do
        table.insert(names, name:lower())
    end
    for _, name in ipairs(_env_list("XMAKE_FORMAT_SKIP")) do
        table.insert(names, name:lower())
    end
    return names
end

local function _is_skipped(name, skips)
    if name:startswith(".") then
        return true
    end
    local lname = name:lower()
    for _, pattern in ipairs(skips) do
        if pattern == lname then
            return true
        end
        if pattern:endswith("*") and lname:startswith(pattern:sub(1, #pattern - 1)) then
            return true
        end
    end
    return false
end

local function _walk(dir, ctx, depth)
    if depth > 64 then
        return
    end
    -- 防止软链接造成的目录环路
    local dirkey = _key(dir)
    if ctx.visited_dirs[dirkey] then
        return
    end
    ctx.visited_dirs[dirkey] = true

    for _, file in ipairs(os.files(path.join(dir, "*")) or {}) do
        local ext = path.extension(file)
        if #ext > 1 and ctx.exts[ext:sub(2):lower()] then
            local filekey = _key(file)
            if not ctx.visited_files[filekey] then
                ctx.visited_files[filekey] = true
                table.insert(ctx.files, file)
            end
        end
    end

    for _, sub in ipairs(os.dirs(path.join(dir, "*")) or {}) do
        if _is_skipped(_name(sub), ctx.skips) then
            ctx.skipped_dirs = ctx.skipped_dirs + 1
        else
            _walk(sub, ctx, depth + 1)
        end
    end
end

-- 扫描路径：命令行参数 > XMAKE_FORMAT_PATHS > 项目根目录
local function _roots(args)
    local roots = {}
    for _, arg in ipairs(args or {}) do
        if type(arg) == "string" then
            arg = arg:trim()
            if #arg > 0 and not arg:startswith("-") then
                table.insert(roots, arg)
            end
        end
    end
    if #roots == 0 then
        roots = _env_list("XMAKE_FORMAT_PATHS")
    end
    if #roots == 0 then
        roots = { os.projectdir() }
    end

    local result = {}
    for _, root in ipairs(roots) do
        local abs = path.absolute(root, os.projectdir())
        if os.isfile(abs) or os.isdir(abs) then
            table.insert(result, abs)
        else
            raise(string.format("路径不存在：%s", root))
        end
    end
    return result
end

local function _collect(roots)
    local ctx = {
        exts = _extensions(),
        skips = _skip_dirs(),
        files = {},
        visited_dirs = {},
        visited_files = {},
        skipped_dirs = 0,
    }
    for _, root in ipairs(roots) do
        if os.isfile(root) then
            -- 显式指定的文件：不再按扩展名过滤
            local filekey = _key(root)
            if not ctx.visited_files[filekey] then
                ctx.visited_files[filekey] = true
                table.insert(ctx.files, root)
            end
        else
            _walk(root, ctx, 0)
        end
    end
    table.sort(ctx.files, function (a, b) return _rel(a) < _rel(b) end)
    return ctx.files, ctx
end

-- -----------------------------------------------------------------------------
-- clang-format 调用
-- -----------------------------------------------------------------------------

-- 运行 clang-format 并捕获输出（退出码非 0 时 xmake 的 os.iorunv 会抛出，输出在异常对象里）
local function _capture(bin, argv)
    local ok, output = true, ""
    try{
        function ()
            local outdata, errdata = os.iorunv(bin, argv)
            output = (outdata or "") .. (errdata or "")
        end,
        catch{
            function (errors)
                ok = false
                if type(errors) == "table" then
                    output = (errors.stdout or "") .. (errors.stderr or "") .. (errors.errors or "")
                else
                    output = tostring(errors)
                end
            end
        }
    }
    return ok, output
end

-- 当前配置是否不支持该文件（clang-format 会按语言判定，例如 C++ 配置遇到含 #import 的 .h）
local function _unsupported(output)
    return output:find("not support", 1, true) ~= nil
end

-- 单文件检查：返回状态 ok / violated / unsupported / failed
local function _check_file(bin, style, file, dryrun)
    local argv
    if dryrun then
        argv = { "--dry-run", "--Werror", style, file }
    else
        argv = { style, file }
    end
    local ok, output = _capture(bin, argv)
    if ok then
        if dryrun then
            return "ok"
        end
        -- 老版本 clang-format：与格式化结果对比
        local origin = (io.readfile(file) or ""):gsub("\r\n", "\n")
        return origin == output:gsub("\r\n", "\n") and "ok" or "violated"
    end
    if output:find(VIOLATION_HINT, 1, true) then
        return "violated"
    end
    if _unsupported(output) then
        return "unsupported", output
    end
    return "failed", output
end

-- 就地格式化：返回（配置不支持的文件, 真正失败的文件及其信息）
local function _apply(bin, style, files, njob, batch)
    local groups = _chunks(files, batch)
    local retry = {}
    local report = _progress("已处理", #files)
    runjobs("clang-format", function (index)
        local group = groups[index]
        local argv = { "-i", style }
        for _, file in ipairs(group) do
            table.insert(argv, file)
        end
        local code = os.execv(bin, argv, { try = true })
        if code ~= 0 then
            -- 整批调用无法给出具体是哪个文件出错，稍后逐文件重试定位
            table.insert(retry, group)
        end
        report(#group)
    end, { total = #groups, comax = njob, waiting_indicator = true })

    local unsupported, failed = {}, {}
    for _, group in ipairs(retry) do
        for _, file in ipairs(group) do
            local ok, output = _capture(bin, { "-i", style, file })
            if not ok then
                if _unsupported(output) then
                    table.insert(unsupported, file)
                else
                    table.insert(failed, { file = file, message = output })
                end
            end
        end
    end
    return unsupported, failed
end

-- 检查文件列表：返回（不符合 .clang-format 的文件, 配置不支持的文件）
local function _scan(bin, style, files, njob, batch, dryrun)
    local violated, unsupported, failed = {}, {}, {}
    local seen_violated, seen_unsupported = {}, {}
    local report = _progress("已检查", #files)

    local function mark(status, file, message)
        if status == "violated" then
            if not seen_violated[_key(file)] then
                seen_violated[_key(file)] = true
                table.insert(violated, file)
            end
        elseif status == "unsupported" then
            if not seen_unsupported[_key(file)] then
                seen_unsupported[_key(file)] = true
                table.insert(unsupported, file)
            end
        elseif status == "failed" then
            table.insert(failed, { file = file, message = message })
        end
    end

    if not dryrun then
        -- 老版本 clang-format（无 --dry-run）：逐文件对比输出
        runjobs("clang-format-check", function (index)
            local status, message = _check_file(bin, style, files[index], false)
            mark(status, files[index], message)
            report(1)
        end, { total = #files, comax = njob, waiting_indicator = true })
    else
        -- 批量检查：解析 --dry-run --Werror 的诊断输出，快速得到违规文件
        local groups = _chunks(files, batch)
        local retry = {}
        runjobs("clang-format-check", function (index)
            local group = groups[index]
            local argv = { "--dry-run", "--Werror", style }
            for _, file in ipairs(group) do
                table.insert(argv, file)
            end
            local ok, output = _capture(bin, argv)
            local matched, count = {}, 0
            if not ok then
                for line in output:gmatch("[^\r\n]+") do
                    -- 命中判断用纯文本查找，避免把提示语里的 "-" 当成 Lua 模式字符
                    local file = line:match("^(.-):%d+:%d+: error:")
                    if file and line:find(VIOLATION_HINT, 1, true) then
                        matched[_key(file)] = true
                    end
                end
                for _, file in ipairs(group) do
                    if matched[_key(file)] then
                        mark("violated", file)
                        count = count + 1
                    end
                end
                -- 失败但不是"格式违规"（例如配置不支持该语言）的批次需要逐文件定位
                if count == 0 or _unsupported(output) then
                    table.insert(retry, group)
                end
            end
            report(#group)
        end, { total = #groups, comax = njob, waiting_indicator = true })

        for _, group in ipairs(retry) do
            for _, file in ipairs(group) do
                local status, message = _check_file(bin, style, file, true)
                mark(status, file, message)
            end
        end
    end

    if #failed > 0 then
        raise(string.format("clang-format 执行失败：%s\n%s",
            _rel(failed[1].file), (failed[1].message or "unknown error"):trim()))
    end
    table.sort(violated, function (a, b) return _rel(a) < _rel(b) end)
    table.sort(unsupported, function (a, b) return _rel(a) < _rel(b) end)
    return violated, unsupported
end

-- -----------------------------------------------------------------------------
-- 报告
-- -----------------------------------------------------------------------------

local function _report_files(title, files, limit)
    limit = limit or (_env_int("XMAKE_FORMAT_VERBOSE", 0) > 0 and #files or 50)
    print(title)
    for index, file in ipairs(files) do
        if index > limit then
            print(string.format("  ... 另有 %d 个文件（XMAKE_FORMAT_VERBOSE=1 可打印全部）", #files - limit))
            break
        end
        print("  " .. _rel(file))
    end
end

local function _elapsed(begin)
    local ms = os.mclock() - begin
    if ms >= 1000 then
        return string.format("%.1f s", ms / 1000)
    end
    return string.format("%d ms", ms)
end

-- -----------------------------------------------------------------------------
-- 主流程：mode = "apply" | "check"
-- -----------------------------------------------------------------------------

local function _run(mode, ...)
    local args = { ... }
    local bin = _find_bin()
    local version = _version(bin)
    local style = "--style=" .. _env("XMAKE_FORMAT_STYLE", "file")
    local config = _find_config(os.projectdir())
    local njob = _env_int("XMAKE_FORMAT_NJOB", _default_njob())
    local batch = _env_int("XMAKE_FORMAT_BATCH", 128)
    local rounds = _env_int("XMAKE_FORMAT_ROUNDS", 3)
    local dryrun = _supports_dry_run(bin)

    local begin = os.mclock()
    local roots = _roots(args)
    local files, ctx = _collect(roots)
    if #files == 0 then
        local scopes = {}
        for _, root in ipairs(roots) do
            table.insert(scopes, _rel(root))
        end
        raise(string.format("未找到需要处理的源码文件（扫描范围：%s）", table.concat(scopes, ", ")))
    end

    print(string.format("clang-format: %s", bin))
    print(string.format("  版本：%s", version))
    if config then
        print(string.format("  风格：%s（配置文件：%s）", style, _rel(config)))
    else
        print(string.format("  风格：%s（未找到 .clang-format，将使用 clang-format 内置回退风格）", style))
    end
    print(string.format("  范围：%d 个文件；跳过 %d 个目录（第三方/构建产物等）；并行 %d",
        #files, ctx.skipped_dirs, njob))

    -- clang-format 按语言判定文件（例如含 #import 的 .h 会被当作 Objective-C），
    -- 若当前 .clang-format 不支持该语言，则这些文件无法格式化：默认跳过并提示，
    -- 设置 XMAKE_FORMAT_STRICT=1 可改为直接失败。
    local skipped, strict = {}, _env_int("XMAKE_FORMAT_STRICT", 0) > 0
    local function add_skipped(list)
        for _, file in ipairs(list) do
            skipped[_key(file)] = file
        end
    end
    local function skipped_list()
        local list = {}
        for _, file in pairs(skipped) do
            table.insert(list, file)
        end
        table.sort(list, function (a, b) return _rel(a) < _rel(b) end)
        return list
    end

    if mode == "check" then
        local violated, unsupported = _scan(bin, style, files, njob, batch, dryrun)
        add_skipped(unsupported)
        local list = skipped_list()
        if #list > 0 then
            _report_files(string.format("提示：%d 个文件无法用当前配置格式化，已跳过：", #list), list, 10)
        end
        if #violated > 0 then
            _report_files(string.format("存在 %d 个未格式化文件（共 %d 个，耗时 %s）：",
                #violated, #files, _elapsed(begin)), violated)
            raise(string.format("format-check 失败：%d 个文件不符合 %s",
                #violated, config and _rel(config) or "clang-format 风格"))
        end
        if strict and #list > 0 then
            raise(string.format("format-check 失败：%d 个文件无法用当前配置格式化（XMAKE_FORMAT_STRICT=1）", #list))
        end
        print(string.format("通过：%d 个文件均符合规范（耗时 %s）", #files - #list, _elapsed(begin)))
        return
    end

    -- apply：应用 -> 复查 -> 只对残留文件再应用，直到收敛
    local remaining = files
    local applied, round = 0, 0
    while #remaining > 0 and round < rounds do
        round = round + 1
        print(string.format("第 %d 轮：应用 %d 个文件", round, #remaining))
        local unsupported, failed = _apply(bin, style, remaining, njob, batch)
        if #failed > 0 then
            raise(string.format("clang-format 执行失败：%s\n%s",
                _rel(failed[1].file), (failed[1].message or "unknown error"):trim()))
        end
        add_skipped(unsupported)
        applied = applied + #remaining
        local violated, unsupported2 = _scan(bin, style, remaining, njob, batch, dryrun)
        add_skipped(unsupported2)
        remaining = {}
        for _, file in ipairs(violated) do
            if not skipped[_key(file)] then
                table.insert(remaining, file)
            end
        end
        if #remaining > 0 then
            print(string.format("  仍有 %d 个文件需要再次应用", #remaining))
        end
    end

    local list = skipped_list()
    if #list > 0 then
        _report_files(string.format(
            "提示：%d 个文件无法用当前配置格式化（clang-format 按语言判定后不支持），已跳过：", #list), list, 10)
    end
    if #remaining > 0 then
        _report_files(string.format("以下 %d 个文件在 %d 轮后仍未收敛：", #remaining, rounds), remaining)
        raise(string.format("format 未完全收敛（%d 轮后仍剩 %d 个文件）", rounds, #remaining))
    end
    if strict and #list > 0 then
        raise(string.format("format 失败：%d 个文件无法用当前配置格式化（XMAKE_FORMAT_STRICT=1）", #list))
    end
    print(string.format("格式化完成：应用 %d 个文件 / %d 轮收敛，耗时 %s", applied, round, _elapsed(begin)))
end

-- `xmake format` 入口
function main(...)
    return _run("apply", ...)
end

-- 供 `format_check.lua` 复用（xmake 的本地模块机制只导出本文件新定义的全局函数）。
-- 这样两个任务共用同一套实现，配置项与行为完全一致。
function run_clang_format(mode, ...)
    return _run(mode, ...)
end

-- 直接 return 模块表：非沙箱加载本文件（import 的最后一层目录）时按返回值取用
return {
    run = _run,
    main = main,
}
