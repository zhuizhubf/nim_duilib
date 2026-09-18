-- nim_duilib xmake 构建脚本：duilib 主库

local droot = path.join(os.projectdir(), "duilib")

target("duilib")
    set_kind("static")
    set_targetdir(DUILIB_LIB_DIR)
    duilib_target_settings()
    duilib_common_defines()

    -- duilib 自身的源码
    add_files(path.join(droot, "*.cpp"))
    local subdirs = {"Animation", "Box", "Layout", "Control", "Core", "Image", "Render", "RenderSkia", "Utils"}
    for _, dir in ipairs(subdirs) do
        add_files(path.join(droot, dir, "*.cpp"))
    end

    -- CEF 控件（FreeBSD 平台不支持 CEF）
    if not duilib_is_freebsd() then
        add_files(path.join(droot, "CEFControl", "*.cpp"))
        add_files(path.join(droot, "CEFControl/internal", "*.cpp"))
        if duilib_is_windows() then
            add_files(path.join(droot, "CEFControl/internal/Windows", "*.cc"))
        end
    end

    -- WebView2 控件（仅 Windows）
    if duilib_is_windows() then
        add_files(path.join(droot, "WebView2", "*.cpp"))
    end

    -- macOS 平台专用的 Objective-C++ 实现
    if duilib_is_macos() then
        add_files(path.join(droot, "Core", "*.mm"))
        add_files(path.join(droot, "Utils", "*.mm"))
        add_files(path.join(droot, "CEFControl", "*.mm"))
    end

    -- 内置的第三方源码（源码列表与原构建方式一致）
    add_files(path.join(droot, "third_party/giflib", "*.c"))
    add_files(path.join(droot, "third_party/zlib/contrib/minizip", "ioapi.c"))
    add_files(path.join(droot, "third_party/zlib/contrib/minizip", "unzip.c"))
    add_files(path.join(droot, "third_party/convert_utf", "*.cpp"))
    add_files(path.join(droot, "third_party/xml", "pugixml.cpp"))
    if duilib_is_windows() then
        add_files(path.join(droot, "third_party/libudis86", "*.c"))
    end

    -- 头文件目录：仓库根目录（duilib/third_party/... 头文件）、CEF、C++ 内置第三方库
    add_includedirs(
        DUILIB_ROOT,
        path.join(droot, "third_party/zlib"),
        path.join(droot, "third_party/giflib"),
        path.join(droot, "third_party/libpng"),
        path.join(droot, "third_party/convert_utf"),
        path.join(droot, "third_party/xml")
    )

    if not duilib_is_freebsd() then
        add_includedirs(path.join(droot, "third_party/libcef", duilib_cef_src_dir()))
    end

    -- 第三方静态库（链接关系会传递给示例程序）
    add_deps("duilib-zlib", "duilib-png", "duilib-cximage", "duilib-webp")

    -- SDL3（可选项，Windows 默认关闭，其他平台默认开启）
    if duilib_sdl_enabled() then
        add_packages("libsdl3")
    end

    -- Skia（本地包自动下载编译，或者使用 --skia_dir 指定的已有目录）
    local skia_dir = duilib_skia_dir()
    if skia_dir then
        -- 使用已有的 Skia 源码树：路径在解析阶段即可确定，直接添加源码和链接库
        add_includedirs(skia_dir)
        add_files(path.join(skia_dir, "tools/window/WindowContext.cpp"))
        add_files(path.join(skia_dir, "tools/window/GLWindowContext.cpp"))
        if os.isfile(path.join(skia_dir, "tools/ganesh/gl/win/SkWGL_win.cpp")) then
            add_files(path.join(skia_dir, "tools/ganesh/gl/win/SkWGL_win.cpp"))
        end
        local skia_libdir = duilib_skia_libdir()
        if skia_libdir then
            add_linkdirs(skia_libdir)
            add_links(DUILIB_SKIA_LIBS)
        end
    else
        add_packages("duilib-skia")
    end

    -- WebView2（Windows，可选项）
    if duilib_webview2_enabled() then
        add_includedirs(path.join(droot, "third_party/Microsoft.Web.WebView2/build/native/include"))
        add_linkdirs(path.join(droot, "third_party/Microsoft.Web.WebView2/build/native", duilib_arch_name()))
        add_links("WebView2LoaderStatic")
        add_syslinks("advapi32", "ole32", "shell32", "version", "wininet")
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

    on_load(function (target)
        -- libpag 检查：需要先编译好 libpag.lib
        if get_config("pag") then
            local arch = get_config("arch") or os.arch()
            local archdir = (arch == "x86" or arch == "i386" or arch == "i686") and "lib-vc-x86" or "lib-vc-x64"
            local libfile = path.join(os.projectdir(), "duilib/third_party/libpag/windows", archdir, "libpag.lib")
            if not os.isfile(libfile) then
                raise("未找到 " .. libfile .. "，请先按照 duilib/third_party/libpag/windows/libpag-build.md 编译 libpag")
            end
        end

        -- Skia：本地包安装完成后，才会知道 Skia 源码树的位置，
        -- 因此在这里把 Skia 的窗口相关源码（与 Skia 库的编译配置保持一致）加入编译列表
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

-- 输出当前的编译配置（--log=y 时）
if get_config("log") and get_config("plat") then
    print("duilib xmake build config:")
    print("    plat         : " .. duilib_plat() .. " / " .. duilib_arch_name())
    print("    mode         : " .. get_config("mode"))
    print("    sdl          : " .. tostring(duilib_sdl_enabled()))
    print("    cef          : " .. tostring(get_config("cef")) .. " (109: " .. tostring(get_config("cef109")) .. ")")
    print("    webview2     : " .. tostring(duilib_webview2_enabled()))
    print("    jpeg_turbo   : " .. tostring(get_config("jpeg_turbo")))
    print("    libpag       : " .. tostring(get_config("pag")))
    print("    skia_dir     : " .. (duilib_skia_dir() or "(本地包自动编译)"))
    print("    lib dir      : " .. DUILIB_LIB_DIR)
    print("    bin dir      : " .. DUILIB_BIN_DIR)
end
