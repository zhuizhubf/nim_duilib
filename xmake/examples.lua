-- nim_duilib xmake 构建脚本：examples 目录下的示例程序

local exroot = path.join(os.projectdir(), "examples")

-- 所有平台通用的示例
local common_examples = {
    "basic", "chat", "ChildWindow", "ColorPicker", "ColorTheme", "controls",
    "DpiAware", "layout", "ListBox", "ListCtrl", "MoveControl", "MultiLang",
    "render", "RichEdit", "threads", "TreeView", "VirtualListBox", "XmlPreview"
}

-- 需要 CEF 的示例（--cef=y 时编译）
local cef_examples = {"cef", "CefBrowser"}

-- 需要 WebView2 的示例（Windows + --webview2=y 时编译）
local webview2_examples = {"WebView2", "WebView2Browser"}

-- CefBrowser 示例的额外源码子目录
local cef_extra_dirs = {"browser", "Windows", "Windows/taskbar"}

local examples = {}
for _, name in ipairs(common_examples) do
    table.insert(examples, name)
end
if get_config("cef") and duilib_is_windows() then
    for _, name in ipairs(cef_examples) do
        table.insert(examples, name)
    end
end
if duilib_webview2_enabled() then
    for _, name in ipairs(webview2_examples) do
        table.insert(examples, name)
    end
end
if duilib_scintilla_enabled() then
    table.insert(examples, "ScintillaDemo")
end

for _, name in ipairs(examples) do
    local exdir = path.join(exroot, name)
    local is_cef_example = (name == "cef") or (name == "CefBrowser")

    target(name)
        set_kind("binary")
        set_targetdir(DUILIB_BIN_DIR)
        set_rundir(DUILIB_BIN_DIR)
        duilib_target_settings()
        duilib_common_defines()

        add_files(path.join(exdir, "*.cpp"))
        if duilib_is_macos() then
            add_files(path.join(exdir, "*.mm"))
        end

        -- 头文件目录：仓库根目录（duilib/duilib.h 等）、示例自己的目录
        add_includedirs(DUILIB_SRC_DIR, DUILIB_ROOT, exdir)
        if name == "ScintillaDemo" then
            add_includedirs(duilib_scintilla_includedirs())
        end

        -- CefBrowser 示例的额外源码目录
        if name == "CefBrowser" then
            for _, subdir in ipairs(cef_extra_dirs) do
                add_files(path.join(exdir, subdir, "*.cpp"))
            end
        end

        add_deps("duilib")
        for _, dep in ipairs(duilib_module_targets()) do
            add_deps(dep)
            add_linkorders("duilib", dep)
        end

        if is_cef_example and get_config("cef") then
            add_deps(duilib_cef_wrapper_name())
            add_includedirs(path.join(DUILIB_THIRD_DIR, "prebuilt/libcef", duilib_cef_src_dir()))
        end

        if duilib_is_windows() then
            -- 资源文件（图标等）
            add_files(path.join(exdir, "*.rc"))
            -- 窗口程序：使用 Windows 子系统，并嵌入 manifest（与 VS 工程一致）
            local manifest = path.join(os.projectdir(), "xmake/manifest",
                                       duilib_arch_name() == "x86" and "duilib.x86.manifest" or "duilib.x64.manifest")
            add_ldflags("/subsystem:windows", {force = true})
            if os.isfile(manifest) then
                add_ldflags("/MANIFEST:EMBED", "/MANIFESTINPUT:" .. manifest, {force = true})
            end
            -- 系统库
            -- 说明：MSVC 链接时不会自动带上 gdi32/oleaut32 等系统库，这里按需显式列出
            add_syslinks("Comctl32", "Imm32", "Opengl32", "User32", "Gdi32", "shlwapi",
                         "Ole32", "OleAut32", "Uuid", "Advapi32", "Shell32", "WinInet", "Ws2_32",
                         "Comdlg32", "Winspool", "Odbc32")
            if duilib_render_gdi_enabled() then
                add_syslinks("Gdiplus", "Msimg32")
            end
            if not get_config("md") then
                -- 使用静态运行库(/MT)时，需要显式链接静态 UCRT（Skia 的数学函数依赖它）
                add_syslinks("libucrt")
            end
            if duilib_sdl_enabled() then
                add_syslinks("Version", "Winmm", "Setupapi")
            end
            if is_cef_example then
                add_ldflags("/DELAYLOAD:libcef.dll", {force = true})
            end
        elseif duilib_is_linux() then
            add_links("X11", "freetype", "fontconfig")
            add_syslinks("pthread", "dl")
        elseif duilib_is_macos() then
            add_frameworks("AppKit", "Foundation", "Metal", "Cocoa", "CoreText", "CoreGraphics", "CoreFoundation", "Accelerate")
            add_syslinks("pthread")
        elseif duilib_is_freebsd() then
            add_links("X11", "freetype", "fontconfig")
            add_syslinks("pthread", "dl")
        end
    target_end()
end
