--!A cross-platform packaging helper for nim_duilib CI
--
-- 用法（由 .github/workflows/ci.yml 调用，也可在本地直接运行）：
--   xmake l .github/scripts/package.lua <版本> <平台> <架构> <变体> <最少示例数> [构建参数] [输出目录]
--   xmake l .github/scripts/package.lua v1.0.0 windows x64 default 20 "" dist
--
-- 说明：
--   * 只依赖 xmake 自带的 Lua 运行时，不需要 Python/Node；
--   * 归档使用系统自带的 tar（Windows/macOS 为 bsdtar，Linux 为 GNU tar）；
--   * 校验和使用 xmake 内置的 hash.sha256。

-- 全平台都会编译的示例程序
local COMMON_EXAMPLES = {
    "basic", "chat", "ChildWindow", "ColorPicker", "ColorTheme", "controls",
    "DpiAware", "layout", "ListBox", "ListCtrl", "MoveControl", "MultiLang",
    "render", "RichEdit", "threads", "TreeView", "VirtualListBox", "XmlPreview"
}

-- 仅 Windows 平台编译的示例程序
local WINDOWS_EXAMPLES = {"WebView2", "WebView2Browser"}

-- 运行时缓存目录，不能进入发布包
local EXCLUDED_DIRS = {
    ["webview2_cache"] = true,
    ["cef_cache"] = true,
    ["cef_temp"] = true,
    ["libcef_win"] = true,
    ["libcef_win_109"] = true,
    ["libcef_linux"] = true,
    ["libcef_macos"] = true
}

-- 调试信息与链接中间产物，只增加体积
local EXCLUDED_EXTENSIONS = {
    [".pdb"] = true, [".ilk"] = true, [".exp"] = true, [".iobj"] = true,
    [".ipdb"] = true, [".pgc"] = true, [".pgd"] = true
}

local LIBRARY_EXTENSIONS = {[".lib"] = true, [".a"] = true}

local function fail(message)
    raise(message)
end

local function sanitize(value)
    value = tostring(value or "")
    value = value:gsub("[^%w._-]", "-")
    value = value:gsub("^[%-%.]+", ""):gsub("[%-%.]+$", "")
    if value == "" then
        value = "unknown"
    end
    return value
end

local function to_unix_path(filepath)
    return (filepath:gsub("\\", "/"))
end

-- 递归复制目录，跳过缓存目录、调试信息和不需要的文件类型
local function copy_tree(srcdir, dstdir, keep_extensions)
    os.mkdir(dstdir)
    local copied = 0
    for _, entry in ipairs(os.filedirs(path.join(srcdir, "*"))) do
        local name = path.filename(entry)
        if os.isdir(entry) then
            if not EXCLUDED_DIRS[name:lower()] then
                copied = copied + copy_tree(entry, path.join(dstdir, name), keep_extensions)
            end
        else
            local extension = (path.extension(entry) or ""):lower()
            local wanted = not EXCLUDED_EXTENSIONS[extension] and
                           (keep_extensions == nil or keep_extensions[extension])
            if wanted then
                os.cp(entry, path.join(dstdir, name))
                copied = copied + 1
            end
        end
    end
    return copied
end

local function check_examples(bindir, platform, min_examples)
    local names = {}
    table.join2(names, COMMON_EXAMPLES)
    if platform == "windows" then
        table.join2(names, WINDOWS_EXAMPLES)
    end

    local missing = {}
    for _, name in ipairs(names) do
        local filename = (platform == "windows") and (name .. ".exe") or name
        if not os.isfile(path.join(bindir, filename)) then
            table.insert(missing, filename)
        end
    end
    if #missing > 0 then
        fail("missing example programs in bin/: " .. table.concat(missing, ", "))
    end

    -- 统计可执行文件数量，防止产物目录被意外清空或裁剪
    local count = 0
    for _, file in ipairs(os.files(path.join(bindir, "*"))) do
        if os.isfile(file) then
            local extension = (path.extension(file) or ""):lower()
            if platform == "windows" then
                if extension == ".exe" then
                    count = count + 1
                end
            elseif extension == "" then
                count = count + 1
            end
        end
    end
    if count < min_examples then
        fail(string.format("expected at least %d example programs in bin/, found %d",
                           min_examples, count))
    end
    return count
end

local function check_resources(bindir)
    local required = {
        "resources/themes/default/global.xml",
        "resources/lang/zh_CN.txt"
    }
    for _, relative in ipairs(required) do
        local filepath = path.join(bindir, relative)
        if not os.isfile(filepath) then
            fail("missing runtime resource: " .. relative)
        end
    end
end

local function collect_libraries(libdir)
    local libraries = {}
    for _, file in ipairs(os.files(path.join(libdir, "**"))) do
        if os.isfile(file) and LIBRARY_EXTENSIONS[(path.extension(file) or ""):lower()] then
            table.insert(libraries, file)
        end
    end
    if #libraries == 0 then
        fail("no static libraries (*.lib/*.a) found in lib/")
    end
    local has_duilib = false
    for _, file in ipairs(libraries) do
        if path.filename(file):lower():startswith("duilib") then
            has_duilib = true
            break
        end
    end
    if not has_duilib then
        fail("no duilib static library (*.lib/*.a) found in lib/")
    end
    return libraries
end

local function write_build_info(pkgroot, opt, example_count, library_count)
    local lines = {
        "nim_duilib CI build",
        "===================",
        "version     : " .. opt.version,
        "platform    : " .. opt.platform,
        "arch        : " .. opt.arch,
        "variant     : " .. opt.variant,
        "build flags : " .. (opt.build_flags ~= "" and opt.build_flags or "(none)"),
        "xmake       : " .. (os.getenv("XMAKE_VERSION") or "unknown"),
        "commit      : " .. (os.getenv("GITHUB_SHA") or "unknown"),
        "ref         : " .. (os.getenv("GITHUB_REF_NAME") or "unknown"),
        "runner      : " .. (os.getenv("RUNNER_OS") or "unknown") .. " / " ..
                          (os.getenv("RUNNER_ARCH") or "unknown"),
        "generated   : " .. os.date("!%Y-%m-%dT%H:%M:%SZ"),
        "examples    : " .. example_count,
        "libraries   : " .. library_count,
        "",
        "Layout:",
        "  bin/  example programs (run them from this directory)",
        "  lib/  nim_duilib static libraries",
        "",
        "The package does not contain C++ headers; use the source tree for development."
    }
    io.writefile(path.join(pkgroot, "BUILD-INFO.txt"), table.concat(lines, "\n") .. "\n")
end

-- 找到可用的 tar：Windows 必须使用系统自带的 bsdtar（Git 附带的 GNU tar 无法创建 zip）
local function find_tar()
    if os.host() == "windows" then
        local system_tar = path.join(os.getenv("SystemRoot") or "C:\\Windows", "System32/tar.exe")
        if os.isfile(system_tar) then
            return system_tar
        end
    end
    return "tar"
end

local function create_archive(archivefile, parentdir, rootname)
    local tar = find_tar()
    local argv
    if os.host() == "windows" then
        -- bsdtar：根据扩展名自动选择 zip 格式
        argv = {"-a", "-c", "-f", path.translate(archivefile),
                "-C", path.translate(parentdir), rootname}
    else
        argv = {"-czf", to_unix_path(archivefile),
                "-C", to_unix_path(parentdir), rootname}
    end
    os.vrunv(tar, argv)
end

function main(version, platform, arch, variant, min_examples, build_flags, out_dir)
    version = version or os.getenv("XMAKE_CI_VERSION")
    platform = platform or os.getenv("XMAKE_CI_PLATFORM")
    arch = arch or os.getenv("XMAKE_CI_ARCH")
    variant = variant or os.getenv("XMAKE_CI_VARIANT") or "default"
    min_examples = tonumber(min_examples or os.getenv("XMAKE_CI_MIN_EXAMPLES") or "0") or 0
    build_flags = build_flags or os.getenv("XMAKE_CI_BUILD_FLAGS") or ""
    out_dir = out_dir or os.getenv("XMAKE_CI_OUT") or "dist"

    if not version or version == "" then
        fail("missing package version: pass it as the first argument")
    end
    if platform ~= "windows" and platform ~= "linux" and platform ~= "macos" then
        fail("invalid platform '" .. tostring(platform) .. "', expected windows, linux or macos")
    end
    if not arch or arch == "" then
        fail("missing architecture: pass it as the third argument")
    end

    local root = os.curdir()
    local bindir = path.join(root, "bin")
    local libdir = path.join(root, "lib")
    if not os.isdir(bindir) then
        fail("missing bin directory: " .. bindir)
    end
    if not os.isdir(libdir) then
        fail("missing lib directory: " .. libdir)
    end

    local example_count = check_examples(bindir, platform, min_examples)
    check_resources(bindir)
    local libraries = collect_libraries(libdir)

    local parts = {"nim_duilib", sanitize(version), sanitize(platform), sanitize(arch)}
    if sanitize(variant):lower() ~= "default" then
        table.insert(parts, sanitize(variant))
    end
    local archive_root = table.concat(parts, "-")
    local archive_suffix = (platform == "windows") and ".zip" or ".tar.gz"

    local output_dir = path.is_absolute(out_dir) and out_dir or path.join(root, out_dir)
    os.mkdir(output_dir)

    -- 暂存目录放在系统临时目录，避免污染输出目录
    local stage_dir = os.tmpfile() .. "_stage"
    local pkgroot = path.join(stage_dir, archive_root)
    os.mkdir(pkgroot)

    copy_tree(bindir, path.join(pkgroot, "bin"))
    local library_count = copy_tree(libdir, path.join(pkgroot, "lib"), LIBRARY_EXTENSIONS)
    if library_count == 0 then
        fail("no static libraries copied from " .. libdir)
    end
    for _, name in ipairs({"LICENSE", "README.md"}) do
        local filepath = path.join(root, name)
        if os.isfile(filepath) then
            os.cp(filepath, path.join(pkgroot, name))
        end
    end
    write_build_info(pkgroot, {
        version = version,
        platform = platform,
        arch = arch,
        variant = variant,
        build_flags = build_flags
    }, example_count, library_count)

    local archivefile = path.join(output_dir, archive_root .. archive_suffix)
    os.tryrm(archivefile)
    create_archive(archivefile, stage_dir, archive_root)
    os.tryrm(stage_dir)
    if os.isdir(stage_dir) then
        os.rmdir(stage_dir)
    end

    local digest = hash.sha256(to_unix_path(archivefile))
    local checksumfile = archivefile .. ".sha256"
    io.writefile(checksumfile, digest .. "  " .. path.filename(archivefile) .. "\n")

    print("package   : " .. path.filename(archivefile) ..
          string.format(" (%.1f MiB)", os.filesize(archivefile) / (1024 * 1024)))
    print("examples  : " .. example_count)
    print("libraries : " .. library_count)
    print("sha256    : " .. digest)
end
