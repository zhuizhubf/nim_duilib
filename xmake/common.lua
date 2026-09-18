-- nim_duilib xmake 构建脚本：公共设置与工具函数
-- 注意：这里的函数在 target 作用域内调用，直接作用在当前 target 上。

-- 目标平台
-- 说明：xmake 在配置的早期阶段（检测平台之前）会先解析一遍工程文件，此时 get_config("plat")
--       还是 nil，所以这里回退到主机平台，保证各个阶段的结果一致。
function duilib_plat()
    return get_config("plat") or os.host()
end

function duilib_is_windows()
    return duilib_plat() == "windows"
end

function duilib_is_macos()
    return duilib_plat() == "macosx"
end

function duilib_is_linux()
    return duilib_plat() == "linux"
end

function duilib_is_freebsd()
    return duilib_plat() == "freebsd"
end

-- 当前架构名称（与 libpag/WebView2/Skia 的目录命名规则保持一致）
function duilib_arch_name()
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
function duilib_build_mode()
    if get_config("mode") == "debug" then
        return "debug"
    end
    return "release"
end

-- Skia 本地包使用的 clang 目录（Windows）
function duilib_skia_clang_dir()
    local dir = get_config("skia_clang_dir")
    if dir and dir ~= "" then
        return dir
    end
    return "C:/LLVM"
end

-- 是否指定了已有的 Skia 源码树
function duilib_skia_dir()
    local dir = get_config("skia_dir")
    if dir and dir ~= "" then
        return path.absolute(dir, os.projectdir())
    end
    return nil
end

-- CEF 的源码目录名和封装库名称（与 CEF 官方的目录结构一致）
function duilib_cef_src_dir()
    if get_config("cef109") then
        return "libcef_win_109"
    end
    return "libcef_win"
end

function duilib_cef_wrapper_name()
    if duilib_is_windows() then
        if get_config("cef109") then
            return "libcef_dll_wrapper_109"
        end
        return "libcef_dll_wrapper"
    end
    return "cef_dll_wrapper"
end

-- 所有目标的基础设置
function duilib_target_settings()
    set_encodings("utf-8")
    if duilib_is_windows() then
        add_defines("UNICODE", "_UNICODE")
        set_runtimes(get_config("md") and "MD" or "MT")
    end
end

-- SDL3：Windows 下由 --sdl=y 控制（默认关闭），其他平台始终启用（SDL 是窗口系统的实现）
function duilib_sdl_enabled()
    if duilib_is_windows() then
        return get_config("sdl") == true
    end
    return true
end

-- 渲染后端：skia / gdi / both（gdi、both 仅 Windows）
function duilib_render_mode()
    local mode = get_config("render")
    if not mode or mode == "" then
        mode = "skia"
    end
    if (not duilib_is_windows()) and (mode ~= "skia") then
        raise("--render=" .. mode .. " 仅支持 Windows；当前平台请使用 --render=skia")
    end
    return mode
end

function duilib_render_skia_enabled()
    local mode = duilib_render_mode()
    return (mode == "skia") or (mode == "both")
end

function duilib_render_gdi_enabled()
    local mode = duilib_render_mode()
    return duilib_is_windows() and ((mode == "gdi") or (mode == "both"))
end

-- Lottie 解码模块：auto / off / skia
function duilib_lottie_mode()
    local mode = get_config("lottie")
    if not mode or mode == "" then
        mode = "auto"
    end
    if mode == "auto" then
        if duilib_render_skia_enabled() then
            return "skia"
        end
        return "off"
    end
    return mode
end

function duilib_lottie_skia_enabled()
    return duilib_lottie_mode() == "skia"
end

-- SVG 解码模块：auto / nanosvg / skia / off
function duilib_svg_mode()
    local mode = get_config("svg")
    if not mode or mode == "" then
        mode = "auto"
    end
    if mode == "auto" then
        if duilib_render_skia_enabled() or duilib_lottie_skia_enabled() then
            return "skia"
        end
        return "nanosvg"
    end
    return mode
end

function duilib_svg_nanosvg_enabled()
    return duilib_svg_mode() == "nanosvg"
end

function duilib_svg_skia_enabled()
    return duilib_svg_mode() == "skia"
end

-- 是否需要链接 Skia 基础层
function duilib_skia_base_enabled()
    return duilib_render_skia_enabled() or duilib_svg_skia_enabled() or duilib_lottie_skia_enabled()
end

-- 需要链接的渲染后端
function duilib_render_targets()
    local targets = {}
    if duilib_render_skia_enabled() then
        table.insert(targets, "duilib-render-skia")
    end
    if duilib_render_gdi_enabled() then
        table.insert(targets, "duilib-render-gdi")
    end
    return targets
end

-- 需要链接的图片解码模块
function duilib_image_targets()
    local targets = {}
    if duilib_svg_nanosvg_enabled() then
        table.insert(targets, "duilib-image-svg-nanosvg")
    elseif duilib_svg_skia_enabled() then
        table.insert(targets, "duilib-image-svg-skia")
    end
    if duilib_lottie_skia_enabled() then
        table.insert(targets, "duilib-image-lottie-skia")
    end
    return targets
end

-- Skia 相关宏定义
function duilib_skia_defines()
    add_defines("SK_GANESH", "SK_GL", "SK_RELEASE")
end

-- WebView2：仅 Windows 有效，默认开启
function duilib_webview2_enabled()
    if not duilib_is_windows() then
        return false
    end
    local enabled = get_config("webview2")
    if enabled == nil then
        return true
    end
    return enabled == true
end

-- 公共宏定义（duilib 功能开关，与 Skia/CEF/WebView2 的编译配置保持一致）
function duilib_common_defines()
    add_defines("DUILIB_SDL=" .. (duilib_sdl_enabled() and "1" or "0"))
    add_defines("DUILIB_CEF=" .. (get_config("cef") and "1" or "0"))
    add_defines("DUILIB_JPEG_TURBO=" .. (get_config("jpeg_turbo") and "1" or "0"))
    add_defines("DUILIB_LIB_PAG=" .. (get_config("pag") and "1" or "0"))
    add_defines("DUILIB_RENDER_SKIA=" .. (duilib_render_skia_enabled() and "1" or "0"))
    add_defines("DUILIB_RENDER_GDI=" .. (duilib_render_gdi_enabled() and "1" or "0"))
    add_defines("DUILIB_RENDER_DEFAULT_GDI=" .. ((duilib_render_mode() == "gdi") and "1" or "0"))
    add_defines("DUILIB_IMAGE_SVG_NANOSVG=" .. (duilib_svg_nanosvg_enabled() and "1" or "0"))
    add_defines("DUILIB_IMAGE_SVG_SKIA=" .. (duilib_svg_skia_enabled() and "1" or "0"))
    add_defines("DUILIB_IMAGE_LOTTIE_SKIA=" .. (duilib_lottie_skia_enabled() and "1" or "0"))
    add_defines("DUILIB_COMMON_TEXT_LAYOUT=" .. (get_config("common_text_layout") and "1" or "0"))
    if duilib_is_windows() then
        add_defines("DUILIB_WEBVIEW2=" .. (duilib_webview2_enabled() and "1" or "0"))
    end
end

-- --skia_dir 方式下，Skia 编译产物所在目录
function duilib_skia_libdir()
    local skia_root = duilib_skia_dir()
    if not skia_root then
        return nil
    end
    local outroot = path.join(skia_root, "out")
    local suffix = duilib_arch_name() .. "." .. duilib_build_mode()
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
