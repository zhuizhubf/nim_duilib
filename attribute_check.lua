-- =============================================================================
-- 属性名登记表一致性检查 / 初版数据表导出（xmake 内建 Lua）
--
-- 用法（仓库根目录）：
--     xmake attribute-check                     -- 常规：裸字面量残留 + XML 语料覆盖 + 生成物同步
--     xmake attribute-check --baseline          -- 追加：数据表名字集合 与 git HEAD 逐域完全相等
--     xmake l attribute_check.lua --dump <path> -- 按源码抽取顺序导出 attribute_defs.lua（仅初版用）
--
-- 说明：xmake 内建 Lua 不含 pcall，且 | 交替模式不生效、** 需要至少一层目录，
--       因此本脚本统一采用"变量集合过滤 + 多段 glob"的写法。
-- =============================================================================

local g_repoRoot = os.curdir()

-- XML 节点名白名单：不属于"控件类名 / 已登记节点名"但确实合法的节点，逐条注明原因。
-- 节点名若来自运行时注册的自定义控件（AddCreateControlCallback）或窗口自定义工厂，静态无法判定。
local g_nodeAllowlist = {
    ["ColorPreviewLabel"]   = "ColorPicker 窗口自定义控件类（ColorPicker::CreateControlClass 创建）",
    ["ScreenColorPicker"]   = "ColorPicker 窗口自定义控件类（ColorPicker::CreateControlClass 创建）",
    ["ScreenColorPreview"]  = "ColorPicker 窗口自定义控件类（ColorPicker::CreateControlClass 创建）",
    ["RenderTest1"]         = "示例自定义控件类（examples/render）",
    ["RenderTest2"]         = "示例自定义控件类（examples/render）",
    ["ChildWindowTemplate"] = "模板片段根节点，由代码显式加载，非控件类名",
    ["Loading"]             = "模板片段根节点，由代码显式加载，非控件类名",
    ["Property"]            = "ThemeMeta 子节点，属全局/主题解析范围，不在本登记表的 7 个名字域内",
}
local g_dataDir = nil

local g_domains = {
    { name = "control", roots = { "src/duilib", "src/cef", "src/webview2" },
      excludeFiles = { "Window.cpp", "WindowBuilder.cpp", "ImageAttribute.cpp", "Shadow.cpp", "ControlLoading.cpp", "ThemeGenerator.cpp" },
      excludeFragments = { "\\Layout\\" },
      vars = { "strName", "srName" } },
    { name = "window", roots = { "src/duilib/Core/Window.cpp", "src/duilib/Core/WindowBuilder.cpp" }, vars = { "strName" } },
    { name = "layout", roots = { "src/duilib/Layout" }, vars = { "strName" } },
    { name = "node", roots = { "src/duilib/Core/WindowBuilder.cpp", "src/duilib/Core/ThemeGenerator.cpp" }, vars = { "nodeName", "strClass" } },
    { name = "image", roots = { "src/duilib/Image/ImageAttribute.cpp" }, vars = { "name" } },
    { name = "shadow", roots = { "src/duilib/Core/Shadow.cpp" }, vars = { "typeString" } },
    { name = "loading", roots = { "src/duilib/Core/ControlLoading.cpp" }, vars = { "name" } },
}

local function norm(p)
    return (p:gsub("/", "\\"))
end

local function basename(p)
    return (norm(p):match("([^\\]+)$")) or p
end

local function var_set(domain)
    local set = {}
    for _, v in ipairs(domain.vars) do
        set[v] = true
    end
    return set
end

local function collect_files(domain)
    local files, seen = {}, {}
    local function add(f)
        if not seen[f] then
            seen[f] = true
            files[#files + 1] = f
        end
    end
    for _, root in ipairs(domain.roots) do
        if os.isdir(root) then
            for _, pat in ipairs({ "/**/*.cpp", "/**/*.h", "/*.cpp", "/*.h" }) do
                for _, f in ipairs(os.files(root .. pat) or {}) do
                    add(f)
                end
            end
        elseif os.isfile(root) then
            add(root)
        end
    end
    local result = {}
    for _, f in ipairs(files) do
        local skip = false
        for _, ex in ipairs(domain.excludeFiles or {}) do
            if basename(f) == ex then
                skip = true
                break
            end
        end
        if not skip then
            for _, ex in ipairs(domain.excludeFragments or {}) do
                if norm(f):find(ex, 1, true) ~= nil then
                    skip = true
                    break
                end
            end
        end
        if not skip then
            result[#result + 1] = f
        end
    end
    return result
end

-- 按首次出现顺序抽取"名字位置"的字面量：<域内变量> == _T("...")
local function collect_ordered(domain, text)
    local vars = var_set(domain)
    local order, set = {}, {}
    for var, name in text:gmatch("%f[%a_]([%a_][%w_]*)%s*==%s*_T%(\"([^\"]+)\"%)") do
        if vars[var] and (not set[name]) then
            set[name] = true
            order[#order + 1] = name
        end
    end
    return order
end

local function collect_domain_names(domain, fromHead)
    local order, set = {}, {}
    for _, f in ipairs(collect_files(domain)) do
        local text = nil
        if fromHead then
            try{
                function ()
                    text = os.iorunv("git", { "-C", g_repoRoot, "show", "HEAD:" .. (norm(f):gsub("\\", "/")) })
                end,
                catch{
                    function (errors)
                        text = nil
                    end
                }
            }
        else
            text = io.readfile(f)
        end
        if text then
            for _, name in ipairs(collect_ordered(domain, text)) do
                if not set[name] then
                    set[name] = true
                    order[#order + 1] = name
                end
            end
        end
    end
    return order
end

local function sorted_keys(set)
    local list = {}
    for k in pairs(set) do
        list[#list + 1] = k
    end
    table.sort(list)
    return list
end

local function check_generated()
    local genScript = path.join(g_repoRoot, "attribute_gen.lua")
    if not os.isfile(genScript) then
        return true, "跳过（attribute_gen.lua 未落地仓库）"
    end
    local outDir = path.join(os.tmpdir(), "attrgen_sync_check")
    if not os.isdir(outDir) then
        os.mkdir(outDir)
    end
    os.execv("xmake", { "l", genScript, outDir }, { try = true })
    local problems = {}
    for _, name in ipairs({ "AttributeIds.g.h", "AttributeIds.g.cpp" }) do
        local a = path.join(g_repoRoot, "src", "duilib", "Utils", name)
        local b = path.join(outDir, name)
        if not os.isfile(a) then
            problems[#problems + 1] = name .. " 未落地"
        else
            local ta = (io.readfile(a) or ""):gsub("\r\n", "\n")
            local tb = (io.readfile(b) or ""):gsub("\r\n", "\n")
            if ta ~= tb then
                problems[#problems + 1] = name .. " 与生成器输出不一致（请运行 xmake attribute-gen）"
            end
        end
    end
    if #problems == 0 then
        return true, "通过"
    end
    return false, table.concat(problems, "; ")
end

local function check_residue()
    local lines = {}
    for _, domain in ipairs(g_domains) do
        local vars, count, samples = var_set(domain), 0, {}
        for _, f in ipairs(collect_files(domain)) do
            local text = io.readfile(f) or ""
            for var, name in text:gmatch("%f[%a_]([%a_][%w_]*)%s*==%s*_T%(\"([^\"]+)\"%)") do
                if vars[var] then
                    count = count + 1
                    if #samples < 3 then
                        samples[#samples + 1] = norm(f) .. ":" .. name
                    end
                end
            end
        end
        lines[#lines + 1] = string.format("  %-8s 剩余 %4d 处  %s", domain.name, count, table.concat(samples, " | "))
    end
    return table.concat(lines, "\n")
end

local function get_defs()
    import("attribute_defs", { rootdir = (g_dataDir or g_repoRoot) })
    return attribute_defs.get_defs()
end

local function check_baseline()
    local defs = get_defs()
    local allOk, lines = true, {}
    for _, domain in ipairs(g_domains) do
        local expected = {}
        for _, name in ipairs(defs[domain.name] or {}) do
            expected[name] = true
        end
        local actual = {}
        for _, name in ipairs(collect_domain_names(domain, true)) do
            actual[name] = true
        end
        local missing, extra = {}, {}
        for name in pairs(actual) do
            if not expected[name] then
                missing[#missing + 1] = name
            end
        end
        for name in pairs(expected) do
            if not actual[name] then
                extra[#extra + 1] = name
            end
        end
        if (#missing > 0) or (#extra > 0) then
            allOk = false
        end
        lines[#lines + 1] = string.format("  %-8s HEAD=%4d 表=%4d 缺失=%d 多余=%d %s", domain.name,
            #sorted_keys(actual), #sorted_keys(expected), #missing, #extra,
            (#missing + #extra > 0) and ("[缺失]" .. table.concat(missing, ",") .. " [多余]" .. table.concat(extra, ",")) or "")
    end
    return allOk, table.concat(lines, "\n")
end

local function check_xml()
    local defs = get_defs()
    local known, nodeNames, ctrlClasses = {}, {}, {}
    for _, domain in ipairs({ "control", "window", "layout", "image" }) do
        for _, name in ipairs(defs[domain] or {}) do
            known[name] = domain
        end
    end
    for _, name in ipairs(defs["node"] or {}) do
        nodeNames[name] = true
    end
    local defsText = io.readfile(path.join(g_repoRoot, "src", "duilib", "duilib_defs.h")) or ""
    for name in defsText:gmatch("DUI_CTR_%u[%w_]*%s+%(_T%(\"([^\"]+)\"%)%)") do
        ctrlClasses[name] = true
    end
    local attrMiss, nodeMiss, xmlFiles = {}, {}, os.files(path.join(g_repoRoot, "bin", "resources", "**", "*.xml")) or {}
    for _, f in ipairs(xmlFiles) do
        local text = (io.readfile(f) or ""):gsub('"[^"]*"', '""')
        for nodeName in text:gmatch("<([%a_][%w_]*)") do
            if (not nodeNames[nodeName]) and (not ctrlClasses[nodeName]) and (not g_nodeAllowlist[nodeName]) then
                nodeMiss[nodeName] = true
            end
        end
        for nodeName, attrText in text:gmatch("<([%a_][%w_]*)([^>]*)>") do
            if ctrlClasses[nodeName] or (nodeName == "Class") or (nodeName == "Window") then
                for attrName in attrText:gmatch("([%a_][%w_]*)%s*=") do
                    if not known[attrName] then
                        attrMiss[attrName] = true
                    end
                end
            end
        end
    end
    local lines = {}
    lines[#lines + 1] = "  XML 文件数: " .. #xmlFiles .. "，已知控件类名: " .. #sorted_keys(ctrlClasses) .. "，已登记节点名: " .. #sorted_keys(nodeNames)
    lines[#lines + 1] = "  未登记的属性名（" .. #sorted_keys(attrMiss) .. "）: " .. table.concat(sorted_keys(attrMiss), ", ")
    lines[#lines + 1] = "  未登记的节点名（" .. #sorted_keys(nodeMiss) .. "）: " .. table.concat(sorted_keys(nodeMiss), ", ")
    return (#sorted_keys(attrMiss) == 0) and (#sorted_keys(nodeMiss) == 0), table.concat(lines, "\n")
end

local function dump_data(pathOut)
    local lines = {}
    lines[#lines + 1] = "-- 属性名登记表：唯一手写输入。"
    lines[#lines + 1] = "--"
    lines[#lines + 1] = "-- 新增属性：在对应域的列表末尾追加一行属性名，然后运行 `xmake attribute-gen`。"
    lines[#lines + 1] = "-- 每个名字（含历史拼写）各占一个 ID；代码里两个名字共用一个处理分支时，switch 里写两个 case 标签。"
    lines[#lines + 1] = "-- 本文件是 AttributeIds.g.h / AttributeIds.g.cpp 的唯一数据来源。"
    lines[#lines + 1] = ""
    lines[#lines + 1] = "function get_defs()"
    lines[#lines + 1] = "    return {"
    local total = 0
    for _, domain in ipairs(g_domains) do
        local names = collect_domain_names(domain, false)
        lines[#lines + 1] = "        " .. domain.name .. " = {"
        for _, name in ipairs(names) do
            lines[#lines + 1] = "            \"" .. name .. "\","
        end
        lines[#lines + 1] = "        },"
        total = total + #names
        print(string.format("  %-8s names=%d", domain.name, #names))
    end
    lines[#lines + 1] = "    }"
    lines[#lines + 1] = "end"
    io.writefile(pathOut, table.concat(lines, "\n") .. "\n")
    print("导出色数: " .. total .. " -> " .. pathOut)
end

function main(...)
    local args = { ... }
    local baseline, dumpPath = false, nil
    for i, a in ipairs(args) do
        if a == "--baseline" then
            baseline = true
        elseif a == "--dump" then
            dumpPath = args[i + 1]
        end
    end
    -- 通过 xmake 任务运行时，--baseline 由任务选项传入
    try{
        function ()
            import("core.base.option", { alias = "_attr_opt" })
            if _attr_opt.get("baseline") then
                baseline = true
            end
        end,
        catch{
            function (errors)
            end
        }
    }

    print("检查仓库: " .. g_repoRoot)
    if os.isfile(path.join(g_repoRoot, "attribute_defs.lua")) then
        g_dataDir = g_repoRoot
    else
        g_dataDir = os.scriptdir()
        print("  （attribute_defs.lua 未落地仓库，数据来源: " .. g_dataDir .. "）")
    end

    if dumpPath then
        dump_data(dumpPath)
        return
    end

    local failed = false
    print("[A] 生成物同步")
    local okA, msgA = check_generated()
    print("  " .. msgA)
    if not okA then
        failed = true
    end

    print("[B] 裸字面量残留")
    print(check_residue())

    if baseline then
        print("[C] 与 git HEAD 等价性（--baseline）")
        local ok, detail = check_baseline()
        print(detail)
        if not ok then
            failed = true
        end
    else
        print("[C] 与 git HEAD 等价性（未启用，加 --baseline）")
    end

    print("[D] XML 语料覆盖")
    local okD, detailD = check_xml()
    print(detailD)
    if not okD then
        failed = true
    end

    if failed then
        print("结果: 存在问题")
        os.exit(1)
    end
    print("结果: 通过")
end