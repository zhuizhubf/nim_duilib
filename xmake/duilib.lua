-- nim_duilib xmake 构建脚本：核心库与独立渲染/图片模块

local droot = DUILIB_CORE_DIR

-- 给当前 target 应用 Skia 包（包含头文件、链接目录和系统依赖）
local function duilib_apply_skia_package()
    local skia_dir = duilib_skia_dir()
    if skia_dir then
        add_includedirs(skia_dir)
        local skia_libdir = duilib_skia_libdir()
        if skia_libdir then
            add_linkdirs(skia_libdir)
        end
        add_links(DUILIB_SKIA_LIBS)
    else
        add_packages("duilib-skia")
    end
    duilib_skia_defines()
end

-- 添加 Skia 窗口层源码（仅 render-skia 需要）
local function duilib_add_skia_window_sources()
    local skia_dir = duilib_skia_dir()
    if skia_dir then
        add_files(path.join(skia_dir, "tools/window/WindowContext.cpp"))
        add_files(path.join(skia_dir, "tools/window/GLWindowContext.cpp"))
        if os.isfile(path.join(skia_dir, "tools/ganesh/gl/win/SkWGL_win.cpp")) then
            add_files(path.join(skia_dir, "tools/ganesh/gl/win/SkWGL_win.cpp"))
        end
    end
end

-- -----------------------------------------------------------------------------
-- duilib：核心静态库，不依赖 Skia/GDI+
-- -----------------------------------------------------------------------------
target("duilib")
    set_kind("static")
    set_targetdir(DUILIB_LIB_DIR)
    duilib_target_settings()
    duilib_common_defines()

    add_files(path.join(droot, "*.cpp"))
    local subdirs = {"Animation", "Box", "Layout", "Control", "Core", "Image", "Utils"}
    for _, dir in ipairs(subdirs) do
        add_files(path.join(droot, dir, "*.cpp"))
    end
    -- 渲染接口层（Render 已移出核心目录，但其源码仍编入核心库）
    add_files(path.join(DUILIB_SRC_DIR, "render", "*.cpp"))

    -- Skia 相关解码器不属于核心库
    remove_files(
        path.join(droot, "Image", "ImageDecoder_SVG.cpp"),
        path.join(droot, "Image", "ImageDecoder_LOTTIE.cpp"),
        path.join(droot, "Image", "Image_LOTTIE.cpp"),
        path.join(droot, "Image", "ImageDecoderModule_SvgSkia.cpp"),
        path.join(droot, "Image", "ImageDecoderModule_LottieSkia.cpp"),
        path.join(droot, "Image", "ImageDecoder_SVG_NanoSvg.cpp")
    )

    -- macOS 平台专用的 Objective-C++ 实现
    if duilib_is_macos() then
        add_files(path.join(droot, "Core", "*.mm"))
        add_files(path.join(droot, "Utils", "*.mm"))
    end

    -- 内置的第三方源码
    add_files(path.join(droot, "third_party/giflib", "*.c"))
    add_files(path.join(droot, "third_party/zlib/contrib/minizip", "ioapi.c"))
    add_files(path.join(droot, "third_party/zlib/contrib/minizip", "unzip.c"))
    add_files(path.join(droot, "third_party/convert_utf", "*.cpp"))
    add_files(path.join(droot, "third_party/xml", "pugixml.cpp"))
    if duilib_is_windows() then
        add_files(path.join(droot, "third_party/libudis86", "*.c"))
    end

    -- 头文件目录
    add_includedirs(
        DUILIB_SRC_DIR,
        DUILIB_ROOT,
        path.join(droot, "third_party/zlib"),
        path.join(droot, "third_party/giflib"),
        path.join(droot, "third_party/libpng"),
        path.join(droot, "third_party/convert_utf"),
        path.join(droot, "third_party/xml")
    )

    -- 第三方静态库
    add_deps("duilib-zlib", "duilib-png", "duilib-cximage", "duilib-webp")

    -- SDL3（可选项，Windows 默认关闭，其他平台默认开启）
    if duilib_sdl_enabled() then
        add_packages("libsdl3")
    end

    -- libjpeg-turbo（可选项）
    if get_config("jpeg_turbo") then
        if duilib_is_windows() then
            local jpegdir = path.join(droot, "third_party/libjpeg-turbo",
                                      duilib_arch_name() == "x86" and "libjpeg-turbo6-win-vc-x86" or "libjpeg-turbo6-win-vc-x64")
            add_includedirs(path.join(jpegdir, "include"))
            add_linkdirs(path.join(jpegdir, "lib"))
            add_links("turbojpeg-static")
        else
            add_packages("libjpeg-turbo")
        end
    end

    -- libpag（可选项：需要自己编译 libpag.lib 和 libpag.dll）
    if get_config("pag") then
        local pagroot = path.join(droot, "third_party/libpag/windows")
        local pagdir = path.join(pagroot, duilib_arch_name() == "x86" and "lib-vc-x86" or "lib-vc-x64")
        add_includedirs(pagroot)
        add_linkdirs(pagdir)
        add_links("libpag")
    end
target_end()

-- -----------------------------------------------------------------------------
-- duilib-text：后端无关的文本布局
-- -----------------------------------------------------------------------------
target("duilib-text")
    set_kind("static")
    set_targetdir(DUILIB_LIB_DIR)
    duilib_target_settings()
    duilib_common_defines()
    add_deps("duilib")
    add_files(path.join(DUILIB_SRC_DIR, "text", "*.cpp"))
    add_includedirs(DUILIB_SRC_DIR, DUILIB_ROOT)
target_end()

-- -----------------------------------------------------------------------------
-- duilib-skia-base：Skia 字体和公共基础封装
-- -----------------------------------------------------------------------------
if duilib_skia_base_enabled() then
    target("duilib-skia-base")
        set_kind("static")
        set_targetdir(DUILIB_LIB_DIR)
        duilib_target_settings()
        duilib_common_defines()
        duilib_apply_skia_package()
        add_deps("duilib")
        add_files(
            path.join(DUILIB_SRC_DIR, "render-skia", "Font_Skia.cpp"),
            path.join(DUILIB_SRC_DIR, "render-skia", "FontMgr_Skia.cpp")
        )
        add_includedirs(DUILIB_SRC_DIR, DUILIB_ROOT)
    target_end()
end

-- -----------------------------------------------------------------------------
-- duilib-render-skia：Skia 渲染后端
-- -----------------------------------------------------------------------------
if duilib_render_skia_enabled() then
    target("duilib-render-skia")
        set_kind("static")
        set_targetdir(DUILIB_LIB_DIR)
        duilib_target_settings()
        duilib_common_defines()
        duilib_apply_skia_package()
        add_deps("duilib", "duilib-text")
        if duilib_skia_base_enabled() then
            add_deps("duilib-skia-base")
        end
        add_files(path.join(DUILIB_SRC_DIR, "render-skia", "*.cpp"))
        remove_files(
            path.join(DUILIB_SRC_DIR, "render-skia", "Font_Skia.cpp"),
            path.join(DUILIB_SRC_DIR, "render-skia", "FontMgr_Skia.cpp")
        )
        add_includedirs(DUILIB_SRC_DIR, DUILIB_ROOT)
        duilib_add_skia_window_sources()

        on_load(function (target)
            local pkg = target:pkg("duilib-skia")
            if pkg then
                local root = path.join(pkg:installdir(), "skia")
                local sources = {
                    "tools/window/WindowContext.cpp",
                    "tools/window/GLWindowContext.cpp",
                    "tools/ganesh/gl/win/SkWGL_win.cpp"
                }
                for _, name in ipairs(sources) do
                    local file = path.join(root, name)
                    if os.isfile(file) then
                        target:add("files", file)
                    end
                end
            end
        end)
    target_end()
end

-- -----------------------------------------------------------------------------
-- duilib-render-gdi：Windows GDI/GDI+ 渲染后端
-- -----------------------------------------------------------------------------
if duilib_render_gdi_enabled() then
    target("duilib-render-gdi")
        set_kind("static")
        set_targetdir(DUILIB_LIB_DIR)
        duilib_target_settings()
        duilib_common_defines()
        add_deps("duilib", "duilib-text")
        add_files(path.join(DUILIB_SRC_DIR, "render-gdi", "*.cpp"))
        add_includedirs(DUILIB_SRC_DIR, DUILIB_ROOT)
        add_syslinks("Gdi32", "Gdiplus", "Msimg32", "User32")
    target_end()
end

-- -----------------------------------------------------------------------------
-- duilib-cef：CEF 控件模块（可选，--cef=y，仅 Windows 支持）
-- -----------------------------------------------------------------------------
if get_config("cef") and not duilib_is_freebsd() then
    target("duilib-cef")
        set_kind("static")
        set_targetdir(DUILIB_LIB_DIR)
        duilib_target_settings()
        duilib_common_defines()
        set_languages("c++17")
        add_deps("duilib")
        add_files(
            path.join(DUILIB_SRC_DIR, "cef", "*.cpp"),
            path.join(DUILIB_SRC_DIR, "cef", "internal", "*.cpp")
        )
        if duilib_is_windows() then
            add_files(path.join(DUILIB_SRC_DIR, "cef", "internal/Windows", "*.cc"))
        end
        if duilib_is_macos() then
            add_files(path.join(DUILIB_SRC_DIR, "cef", "*.mm"))
        end
        add_includedirs(DUILIB_SRC_DIR, DUILIB_ROOT,
                        path.join(DUILIB_THIRD_DIR, "libcef", duilib_cef_src_dir()))
    target_end()
end

-- -----------------------------------------------------------------------------
-- duilib-webview2：WebView2 控件模块（可选，Windows 且 --webview2=y）
-- -----------------------------------------------------------------------------
if duilib_webview2_enabled() then
    target("duilib-webview2")
        set_kind("static")
        set_targetdir(DUILIB_LIB_DIR)
        duilib_target_settings()
        duilib_common_defines()
        add_deps("duilib")
        add_files(path.join(DUILIB_SRC_DIR, "webview2", "*.cpp"))
        add_includedirs(DUILIB_SRC_DIR, DUILIB_ROOT,
                        path.join(DUILIB_THIRD_DIR, "Microsoft.Web.WebView2/build/native/include"))
        add_linkdirs(path.join(DUILIB_THIRD_DIR, "Microsoft.Web.WebView2/build/native", duilib_arch_name()))
        add_links("WebView2LoaderStatic")
        add_syslinks("advapi32", "ole32", "shell32", "version", "wininet")
    target_end()
end

-- -----------------------------------------------------------------------------
-- 图片解码模块
-- -----------------------------------------------------------------------------
if duilib_svg_nanosvg_enabled() then
    target("duilib-image-svg-nanosvg")
        set_kind("static")
        set_targetdir(DUILIB_LIB_DIR)
        duilib_target_settings()
        duilib_common_defines()
        add_deps("duilib")
        add_files(path.join(droot, "Image", "ImageDecoder_SVG_NanoSvg.cpp"))
        add_includedirs(DUILIB_SRC_DIR, DUILIB_ROOT, path.join(droot, "third_party/svg"))
    target_end()
end

if duilib_svg_skia_enabled() then
    target("duilib-image-svg-skia")
        set_kind("static")
        set_targetdir(DUILIB_LIB_DIR)
        duilib_target_settings()
        duilib_common_defines()
        duilib_apply_skia_package()
        add_deps("duilib")
        if duilib_skia_base_enabled() then
            add_deps("duilib-skia-base")
        end
        add_files(
            path.join(droot, "Image", "ImageDecoder_SVG.cpp"),
            path.join(droot, "Image", "ImageDecoderModule_SvgSkia.cpp")
        )
        add_includedirs(DUILIB_SRC_DIR, DUILIB_ROOT)
    target_end()
end

if duilib_lottie_skia_enabled() then
    target("duilib-image-lottie-skia")
        set_kind("static")
        set_targetdir(DUILIB_LIB_DIR)
        duilib_target_settings()
        duilib_common_defines()
        duilib_apply_skia_package()
        add_deps("duilib")
        if duilib_skia_base_enabled() then
            add_deps("duilib-skia-base")
        end
        add_files(
            path.join(droot, "Image", "ImageDecoder_LOTTIE.cpp"),
            path.join(droot, "Image", "Image_LOTTIE.cpp"),
            path.join(droot, "Image", "ImageDecoderModule_LottieSkia.cpp")
        )
        add_includedirs(DUILIB_SRC_DIR, DUILIB_ROOT)
    target_end()
end

-- 输出当前的编译配置（--log=y 时）
if get_config("log") and get_config("plat") then
    print("duilib xmake build config:")
    print("    plat          : " .. duilib_plat() .. " / " .. duilib_arch_name())
    print("    mode          : " .. duilib_build_mode())
    print("    render        : " .. duilib_render_mode())
    print("    svg           : " .. duilib_svg_mode())
    print("    lottie        : " .. duilib_lottie_mode())
    print("    sdl           : " .. tostring(duilib_sdl_enabled()))
    print("    cef           : " .. tostring(get_config("cef")) .. " (109: " .. tostring(get_config("cef109")) .. ")")
    print("    webview2      : " .. tostring(duilib_webview2_enabled()))
    print("    jpeg_turbo    : " .. tostring(get_config("jpeg_turbo")))
    print("    libpag        : " .. tostring(get_config("pag")))
    print("    skia_dir      : " .. (duilib_skia_dir() or "(本地包自动编译)"))
    print("    lib dir       : " .. DUILIB_LIB_DIR)
    print("    bin dir       : " .. DUILIB_BIN_DIR)
end
