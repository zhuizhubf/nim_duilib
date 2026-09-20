-- nim_duilib xmake 构建脚本：公共路径与配置判定
-- ---------------------------------------------------------------------------
-- 约定：
--   1. 本文件由根目录的 xmake.lua 通过 includes() 引入，运行在“描述域”；这里定义的
--      DUILIB 全局表只在描述域可见，xmake 不会把描述域的全局变量带进 on_load 等“脚本域”。
--   2. 规则文件在加载时用 `local env = DUILIB` 捕获局部引用，闭包才能把这些函数带进
--      on_load；因此表内函数只引用本文件内的局部变量，不引用 DUILIB 这个全局名字。
--   3. 表内只放路径常量与纯函数；函数内部实时读取 get_config，不缓存配置快照，
--      保证配置阶段早期（plat 尚未确定）与后续阶段结果一致。

local env = {}

-- 公共路径 -------------------------------------------------------------------
local paths = {}
paths.root           = os.projectdir()
paths.src_dir        = path.join(paths.root, "src")
paths.core_dir       = path.join(paths.src_dir, "duilib")
paths.third_dir      = path.join(paths.root, "third_party")
paths.extensions_dir = path.join(paths.root, "extensions")
paths.examples_dir   = path.join(paths.root, "examples")
paths.manifest_dir   = path.join(paths.root, "xmake", "manifest")
paths.lib_dir        = path.join(paths.root, "lib")
paths.bin_dir        = path.join(paths.root, "bin")
paths.bench_dir      = path.join(paths.root, "tools", "bench_attribute_dispatch")
paths.scintilla_dir  = path.join(paths.extensions_dir, "scintilla")
paths.cef_dir        = path.join(paths.extensions_dir, "cef")
paths.webview2_dir   = path.join(paths.extensions_dir, "webview2")
-- duilib 链接 Skia 时需要的静态库（顺序即链接顺序）
paths.skia_libs      = {"svg", "skshaper", "skottie", "sksg", "jsonreader", "skia"}
env.paths = paths

-- 目标平台与架构 -------------------------------------------------------------
-- 平台判断一律使用 xmake 内置的 is_plat(...)（脚本域内同样可用），不再自行封装；
-- 配置早期（检测平台之前）会有一遍 plat 为 nil 的解析，此时 is_plat 返回 false，
-- 但该遍只是预解析，最终生效的是平台确定后的那一遍，结果一致。

-- 当前架构名称（与 libpag/WebView2/Skia 的目录命名规则保持一致）
function env.arch_name()
    local arch = get_config("arch") or os.arch()
    if arch == "x86" or arch == "i386" or arch == "i686" then
        return "x86"
    elseif arch == "x64" or arch == "x86_64" or arch == "amd64" then
        return "x64"
    elseif arch == "arm64" or arch == "aarch64" then
        return "arm64"
    elseif arch == "arm" then
        return "arm"
    end
    return "x64"
end

-- 当前编译模式：debug/release
function env.build_mode()
    if is_mode("debug") then
        return "debug"
    end
    return "release"
end

-- MSVC 运行库：取 xmake 内置 --runtimes 的值（默认 MT），并去掉 debug 后缀（MTd/MDd -> MT/MD），
-- 供 duilib-skia 包的 runtime 配置使用
function env.runtime()
    local runtime = get_config("runtimes")
    if type(runtime) ~= "string" or runtime == "" then
        runtime = "MT"
    end
    return (runtime:gsub("d$", ""))
end

-- 渲染后端：skia / gdi / both（gdi、both 仅 Windows） ---------------------------
function env.render_backend()
    local backend = get_config("render_backend")
    if not backend or backend == "" then
        backend = "skia"
    end
    if (not is_plat("windows")) and (backend ~= "skia") then
        raise("--render_backend=" .. backend .. " 仅支持 Windows；当前平台请使用 --render_backend=skia")
    end
    return backend
end

function env.render_skia_enabled()
    local backend = env.render_backend()
    return (backend == "skia") or (backend == "both")
end

function env.render_gdi_enabled()
    return is_plat("windows") and ((env.render_backend() == "gdi") or (env.render_backend() == "both"))
end

-- Lottie 解码模块：auto / off / skia -----------------------------------------
function env.lottie_decoder()
    local decoder = get_config("lottie_decoder")
    if not decoder or decoder == "" then
        decoder = "auto"
    end
    if decoder == "auto" then
        if env.render_skia_enabled() then
            return "skia"
        end
        return "off"
    end
    return decoder
end

function env.lottie_skia_enabled()
    return env.lottie_decoder() == "skia"
end

-- SVG 解码模块：auto / nanosvg / skia / off -----------------------------------
function env.svg_decoder()
    local decoder = get_config("svg_decoder")
    if not decoder or decoder == "" then
        decoder = "auto"
    end
    if decoder == "auto" then
        if env.render_skia_enabled() or env.lottie_skia_enabled() then
            return "skia"
        end
        return "nanosvg"
    end
    return decoder
end

function env.svg_nanosvg_enabled()
    return env.svg_decoder() == "nanosvg"
end

function env.svg_skia_enabled()
    return env.svg_decoder() == "skia"
end

-- 是否需要链接 Skia 基础层
function env.skia_base_enabled()
    return env.render_skia_enabled() or env.svg_skia_enabled() or env.lottie_skia_enabled()
end

-- SDL3：Windows 下由 --enable_sdl=y 控制（默认关闭），其他平台始终启用（窗口系统实现）
function env.sdl_enabled()
    if is_plat("windows") then
        return has_config("enable_sdl")
    end
    return true
end

-- CEF 扩展：off / latest / 109 ------------------------------------------------
function env.cef_version()
    local version = get_config("with_cef")
    if not version or version == "" then
        return "off"
    end
    return version
end

function env.cef_enabled()
    return env.cef_version() ~= "off"
end

-- CEF 的源码目录名和封装库名称（与 CEF 官方的目录结构一致）
function env.cef_src_dir()
    if env.cef_version() == "109" then
        return "libcef_win_109"
    end
    return "libcef_win"
end

function env.cef_wrapper_name()
    if not is_plat("windows") then
        return "cef_dll_wrapper"
    end
    if env.cef_version() == "109" then
        return "libcef_dll_wrapper_109"
    end
    return "libcef_dll_wrapper"
end

-- CEF 扩展：源码/头文件目录与预编译 SDK 目录
function env.cef_sdk_dir()
    return path.join(paths.cef_dir, "third_party", "prebuilt", "libcef", env.cef_src_dir())
end

-- 扩展 src 根解析 "cef/..."，扩展根解析 "third_party/prebuilt/..."，
-- SDK 根解析 CEF 自身的 "include/cef_*.h"
function env.cef_includedirs()
    return {
        path.join(paths.cef_dir, "src"),
        paths.cef_dir,
        env.cef_sdk_dir()
    }
end

-- WebView2 扩展：仅 Windows 有效，默认开启 -------------------------------------
function env.webview2_enabled()
    if not is_plat("windows") then
        return false
    end
    return has_config("enable_webview2")
end

function env.webview2_sdk_dir()
    return path.join(paths.webview2_dir, "third_party", "prebuilt", "Microsoft.Web.WebView2")
end

-- 扩展 src 根解析 "webview2/..."，扩展根解析 "third_party/prebuilt/..."
function env.webview2_includedirs()
    return {
        path.join(paths.webview2_dir, "src"),
        paths.webview2_dir
    }
end

-- Scintilla 扩展（可选，默认关闭）---------------------------------------------
function env.scintilla_enabled()
    return has_config("enable_scintilla")
end

-- Scintilla 扩展的源码头文件目录（扩展自身 + 上游 Scintilla/Lexilla）
function env.scintilla_includedirs()
    local extroot = paths.scintilla_dir
    local scroot = path.join(extroot, "third_party", "scintilla")
    local lxroot = path.join(extroot, "third_party", "lexilla")
    return {
        path.join(extroot, "src"),
        path.join(scroot, "include"),
        path.join(scroot, "src"),
        path.join(lxroot, "include"),
        path.join(lxroot, "lexlib")
    }
end

function env.examples_enabled()
    return has_config("enable_examples")
end

-- 可选模块（渲染后端 + 图片解码 + 可选控件）------------------------------------
function env.render_targets()
    local targets = {}
    if env.render_skia_enabled() then
        table.insert(targets, "duilib-render-skia")
    end
    if env.render_gdi_enabled() then
        table.insert(targets, "duilib-render-gdi")
    end
    return targets
end

function env.image_targets()
    local targets = {}
    if env.svg_nanosvg_enabled() then
        table.insert(targets, "duilib-image-svg-nanosvg")
    elseif env.svg_skia_enabled() then
        table.insert(targets, "duilib-image-svg-skia")
    end
    if env.lottie_skia_enabled() then
        table.insert(targets, "duilib-image-lottie-skia")
    end
    return targets
end

function env.cef_targets()
    local targets = {}
    if env.cef_enabled() and not is_plat("freebsd") then
        table.insert(targets, "duilib-cef")
    end
    return targets
end

function env.webview2_targets()
    local targets = {}
    if env.webview2_enabled() then
        table.insert(targets, "duilib-webview2")
    end
    return targets
end

function env.scintilla_targets()
    local targets = {}
    if env.scintilla_enabled() then
        table.insert(targets, "duilib-scintilla")
    end
    return targets
end

-- 全部可选模块：渲染 + 图片解码 + 可选控件
function env.module_targets()
    local targets = {}
    for _, group in ipairs({env.render_targets(), env.image_targets(),
                            env.cef_targets(), env.webview2_targets(),
                            env.scintilla_targets()}) do
        for _, name in ipairs(group) do
            table.insert(targets, name)
        end
    end
    return targets
end

-- 公共特性宏（duilib 功能开关，与 Skia/CEF/WebView2 的编译配置保持一致）---------
function env.feature_defines()
    local defines = {
        "DUILIB_SDL=" .. (env.sdl_enabled() and "1" or "0"),
        "DUILIB_CEF=" .. (env.cef_enabled() and "1" or "0"),
        "DUILIB_JPEG_TURBO=" .. (has_config("enable_jpeg_turbo") and "1" or "0"),
        "DUILIB_LIB_PAG=" .. (has_config("enable_pag") and "1" or "0"),
        "DUILIB_RENDER_SKIA=" .. (env.render_skia_enabled() and "1" or "0"),
        "DUILIB_RENDER_GDI=" .. (env.render_gdi_enabled() and "1" or "0"),
        "DUILIB_RENDER_DEFAULT_GDI=" .. ((env.render_backend() == "gdi") and "1" or "0"),
        "DUILIB_IMAGE_SVG_NANOSVG=" .. (env.svg_nanosvg_enabled() and "1" or "0"),
        "DUILIB_IMAGE_SVG_SKIA=" .. (env.svg_skia_enabled() and "1" or "0"),
        "DUILIB_IMAGE_LOTTIE_SKIA=" .. (env.lottie_skia_enabled() and "1" or "0"),
        "DUILIB_COMMON_TEXT_LAYOUT=" .. (has_config("enable_common_text_layout") and "1" or "0"),
        "DUILIB_SCINTILLA=" .. (env.scintilla_enabled() and "1" or "0")
    }
    if is_plat("windows") then
        table.insert(defines, "DUILIB_WEBVIEW2=" .. (env.webview2_enabled() and "1" or "0"))
    end
    return defines
end

-- Skia：包配置与源码树 --------------------------------------------------------
-- 使用 clang 编译 Skia 时的 clang 目录（Windows）
function env.skia_clang_dir()
    local dir = get_config("with_skia_clang_dir")
    if dir and dir ~= "" then
        return dir
    end
    return "C:/LLVM"
end

function env.skia_clang_enabled()
    return has_config("with_skia_clang")
end

-- 是否指定了已有的 Skia 源码树（指定后不再使用本地包）
function env.skia_dir()
    local dir = get_config("with_skia_dir")
    if dir and dir ~= "" then
        return path.absolute(dir, paths.root)
    end
    return nil
end

-- --with_skia_dir 方式下，Skia 编译产物所在目录
function env.skia_libdir()
    local skia_root = env.skia_dir()
    if not skia_root then
        return nil
    end
    local outroot = path.join(skia_root, "out")
    local suffix = env.arch_name() .. "." .. env.build_mode()
    -- 现有脚本使用 llvm 或 gcc 子目录，本项目的本地包默认使用 msvc 子目录
    local candidates = {"llvm." .. suffix, "msvc." .. suffix, "gcc." .. suffix}
    for _, name in ipairs(candidates) do
        local dir = path.join(outroot, name)
        if os.isfile(path.join(dir, "skia.lib")) or os.isfile(path.join(dir, "libskia.a")) then
            return dir
        end
    end
    -- 其他命名：在 out 目录中自动搜索包含 Skia 静态库的目录
    for _, dir in ipairs(os.dirs(path.join(outroot, "*"))) do
        if os.isfile(path.join(dir, "skia.lib")) or os.isfile(path.join(dir, "libskia.a")) then
            return dir
        end
    end
    return path.join(outroot, candidates[1])
end

-- 示例程序 -------------------------------------------------------------------
-- 所有平台通用的示例
local common_examples = {
    "basic", "chat", "ChildWindow", "ColorPicker", "ColorTheme", "controls",
    "DpiAware", "layout", "ListBox", "ListCtrl", "MoveControl", "MultiLang",
    "render", "RichEdit", "threads", "TreeView", "VirtualListBox", "XmlPreview"
}

-- 需要 CEF / WebView2 的示例
local cef_examples = {"cef", "CefBrowser"}
local webview2_examples = {"WebView2", "WebView2Browser"}

function env.example_names()
    local names = {}
    for _, name in ipairs(common_examples) do
        table.insert(names, name)
    end
    if env.cef_enabled() and is_plat("windows") then
        for _, name in ipairs(cef_examples) do
            table.insert(names, name)
        end
    end
    if env.webview2_enabled() then
        for _, name in ipairs(webview2_examples) do
            table.insert(names, name)
        end
    end
    if env.scintilla_enabled() then
        table.insert(names, "ScintillaDemo")
    end
    return names
end

function env.is_cef_example(name)
    for _, item in ipairs(cef_examples) do
        if item == name then
            return true
        end
    end
    return false
end

function env.is_webview2_example(name)
    for _, item in ipairs(webview2_examples) do
        if item == name then
            return true
        end
    end
    return false
end

DUILIB = env
