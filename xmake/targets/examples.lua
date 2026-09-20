-- nim_duilib xmake 构建脚本：examples 目录下的示例程序
-- ---------------------------------------------------------------------------
-- 示例清单由 DUILIB.example_names() 给出（公共示例 + 按配置追加的 CEF/WebView2/Scintilla 示例）；
-- 源文件逐个列出（不使用通配符）：新增示例源码时在本文件登记；
-- 平台链接配置（子系统、manifest、系统库、框架）由 duilib.app 规则负责。

local env = DUILIB
local paths = env.paths

-- 各示例的源文件（相对于 examples/<示例名>/），平台相关的 main_*.cpp 由各自的条件编译宏裁剪
local example_sources = {
    basic = {
        "main_freebsd.cpp", "main_linux.cpp", "main_macos.cpp", "main_windows.cpp",
        "MainForm.cpp", "MainThread.cpp", "TestApplication.cpp"
    },
    cef = {
        "main_linux.cpp", "main_windows.cpp",
        "CefForm.cpp", "MainThread.cpp", "TestApplication.cpp"
    },
    CefBrowser = {
        "main_linux.cpp", "main_windows.cpp",
        "MainThread.cpp", "TestApplication.cpp"
    },
    chat = {
        "main_freebsd.cpp", "main_linux.cpp", "main_macos.cpp", "main_windows.cpp",
        "ChatForm.cpp", "MainThread.cpp", "TestApplication.cpp"
    },
    ChildWindow = {
        "main_freebsd.cpp", "main_linux.cpp", "main_macos.cpp", "main_windows.cpp",
        "ChildWindowPaint_SDL.cpp", "ChildWindowPaint_Win.cpp",
        "MainForm.cpp", "MainThread.cpp", "MyChildWindowEvents.cpp", "TestApplication.cpp"
    },
    ColorPicker = {
        "main_freebsd.cpp", "main_linux.cpp", "main_macos.cpp", "main_windows.cpp",
        "MainThread.cpp", "TestApplication.cpp"
    },
    ColorTheme = {
        "main_freebsd.cpp", "main_linux.cpp", "main_macos.cpp", "main_windows.cpp",
        "MainForm.cpp", "MainThread.cpp", "TestApplication.cpp"
    },
    controls = {
        "main_freebsd.cpp", "main_linux.cpp", "main_macos.cpp", "main_windows.cpp",
        "AboutForm.cpp", "AnimationForm.cpp", "ControlForm.cpp",
        "MainThread.cpp", "TestApplication.cpp", "TestForm.cpp"
    },
    DpiAware = {
        "main_freebsd.cpp", "main_linux.cpp", "main_macos.cpp", "main_windows.cpp",
        "MainForm.cpp", "MainThread.cpp", "TestApplication.cpp"
    },
    layout = {
        "main_freebsd.cpp", "main_linux.cpp", "main_macos.cpp", "main_windows.cpp",
        "MainForm.cpp", "MainThread.cpp", "TestApplication.cpp"
    },
    ListBox = {
        "main_freebsd.cpp", "main_linux.cpp", "main_macos.cpp", "main_windows.cpp",
        "Item.cpp", "ListBoxForm.cpp", "MainThread.cpp", "TestApplication.cpp"
    },
    ListCtrl = {
        "main_freebsd.cpp", "main_linux.cpp", "main_macos.cpp", "main_windows.cpp",
        "MainForm.cpp", "MainThread.cpp", "TestApplication.cpp"
    },
    MoveControl = {
        "main_freebsd.cpp", "main_linux.cpp", "main_macos.cpp", "main_windows.cpp",
        "AppDb.cpp", "AppItemUi.cpp", "MainThread.cpp", "MoveControlForm.cpp", "TestApplication.cpp"
    },
    MultiLang = {
        "main_freebsd.cpp", "main_linux.cpp", "main_macos.cpp", "main_windows.cpp",
        "MainForm.cpp", "MainThread.cpp", "TestApplication.cpp"
    },
    render = {
        "main_freebsd.cpp", "main_linux.cpp", "main_macos.cpp", "main_windows.cpp",
        "MainThread.cpp", "RenderForm.cpp", "RenderTest1.cpp", "RenderTest2.cpp", "TestApplication.cpp"
    },
    RichEdit = {
        "main_freebsd.cpp", "main_linux.cpp", "main_macos.cpp", "main_windows.cpp",
        "FindForm.cpp", "MainForm.cpp", "MainThread.cpp",
        "ReplaceForm.cpp", "RichEditFindReplace.cpp", "TestApplication.cpp"
    },
    ScintillaDemo = {
        "main_freebsd.cpp", "main_linux.cpp", "main_macos.cpp", "main_windows.cpp",
        "MainForm.cpp", "MainThread.cpp", "TestApplication.cpp"
    },
    threads = {
        "main_freebsd.cpp", "main_linux.cpp", "main_macos.cpp", "main_windows.cpp",
        "MainForm.cpp", "MainThread.cpp", "TestApplication.cpp", "WorkerThread.cpp"
    },
    TreeView = {
        "main_freebsd.cpp", "main_linux.cpp", "main_macos.cpp", "main_windows.cpp",
        "ComputerView.cpp", "ExplorerView.cpp", "MainForm.cpp", "MainThread.cpp",
        "SimpleFileView.cpp", "TestApplication.cpp"
    },
    VirtualListBox = {
        "main_freebsd.cpp", "main_linux.cpp", "main_macos.cpp", "main_windows.cpp",
        "DataProvider.cpp", "Item.cpp", "MainForm.cpp", "MainThread.cpp", "TestApplication.cpp"
    },
    WebView2 = {
        "MainForm.cpp", "MainThread.cpp", "WebView2.cpp"
    },
    WebView2Browser = {
        "main_windows.cpp",
        "BrowserBox.cpp", "BrowserForm.cpp", "BrowserManager.cpp",
        "DragDropManager.cpp", "DragForm.cpp", "MainThread.cpp", "TestApplication.cpp"
    },
    XmlPreview = {
        "main_freebsd.cpp", "main_linux.cpp", "main_macos.cpp", "main_windows.cpp",
        "MainForm.cpp", "MainThread.cpp", "TestApplication.cpp"
    }
}

-- macOS 专用源文件（仅 cef / CefBrowser 示例提供）
local example_macos_sources = {
    cef = {"main_macos.mm"},
    CefBrowser = {"main_macos.mm"}
}

-- Windows 资源文件（图标等）
local example_windows_resources = {
    basic = {"basic.rc"},
    cef = {"cef.rc"},
    CefBrowser = {"CefBrowser.rc"},
    chat = {"chat.rc"},
    ChildWindow = {"ChildWindow.rc"},
    ColorPicker = {"ColorPicker.rc"},
    ColorTheme = {"ColorTheme.rc"},
    controls = {"controls.rc"},
    DpiAware = {"DpiAware.rc"},
    layout = {"layout.rc"},
    ListBox = {"ListBox.rc"},
    ListCtrl = {"ListCtrl.rc"},
    MoveControl = {"MoveControl.rc"},
    MultiLang = {"MultiLang.rc"},
    render = {"render.rc"},
    RichEdit = {"RichEdit.rc"},
    ScintillaDemo = {"ScintillaDemo.rc"},
    threads = {"threads.rc"},
    TreeView = {"TreeView.rc"},
    VirtualListBox = {"VirtualListBox.rc"},
    WebView2 = {"WebView2.rc"},
    WebView2Browser = {"WebView2Browser.rc"},
    XmlPreview = {"XmlPreview.rc"}
}

-- CefBrowser 示例的额外源文件（相对于 examples/CefBrowser/）
local cefbrowser_sources = {
    "browser/BrowserBox.cpp",
    "browser/BrowserForm.cpp",
    "browser/BrowserManager.cpp",
    "browser/DragDropManager.cpp",
    "browser/DragForm.cpp",
    "Windows/BrowserBox_Windows.cpp",
    "Windows/BrowserForm_Windows.cpp",
    "Windows/taskbar/DwmUtil.cpp",
    "Windows/taskbar/TaskbarManager.cpp"
}

for _, name in ipairs(env.example_names()) do
    local exdir = path.join(paths.examples_dir, name)

    target(name)
        set_kind("binary")
        set_targetdir(paths.bin_dir)
        set_rundir(paths.bin_dir)
        add_rules("duilib.app")

        for _, file in ipairs(example_sources[name] or {}) do
            add_files(path.join(exdir, file))
        end
        if is_plat("macosx") then
            for _, file in ipairs(example_macos_sources[name] or {}) do
                add_files(path.join(exdir, file))
            end
        end
        if name == "CefBrowser" then
            for _, file in ipairs(cefbrowser_sources) do
                add_files(path.join(exdir, file))
            end
        end

        -- 头文件目录：仓库根目录（duilib/duilib.h 等）、示例自己的目录
        add_includedirs(paths.src_dir, paths.root, exdir)
        if env.is_cef_example(name) then
            add_includedirs(env.cef_includedirs())
        end
        if env.is_webview2_example(name) then
            add_includedirs(env.webview2_includedirs())
        end
        if name == "ScintillaDemo" then
            add_includedirs(env.scintilla_includedirs())
        end

        add_deps("duilib")
        if env.skia_base_enabled() then
            add_deps("duilib-skia-base")
        end
        for _, dep in ipairs(env.module_targets()) do
            add_deps(dep)
            -- 静态库链接顺序：渲染/图片/扩展模块引用了核心库（duilib）与文本布局（duilib-text）的符号，
            -- GNU ld 是单遍扫描（Linux/FreeBSD），必须"引用方在前、被引用方在后"，否则会报
            -- BitmapAlpha/Image_Svg 等未定义符号；Windows(MSVC)/macOS(ld64) 对顺序不敏感。
            add_linkorders(dep, "duilib-text", "duilib")
        end

        -- Linux/FreeBSD 的 GNU ld 单遍扫描下还存在反向引用：核心库 duilib（GlobalManager）调用模块入口
        -- GetRenderBackend_Skia / GetImageDecoderModule_*Skia，而上面的顺序把核心库排在模块之后，
        -- 线性顺序无法同时满足两个方向。链接组（--start-group/--end-group）让 ld 反复扫描这组静态库，
        -- 从而解析循环依赖。
        -- 说明：xmake 只为 GNU 工具链（Linux/FreeBSD）生成组参数，MSVC/macOS/MinGW 平台不生成
        -- （这些平台自身能处理循环引用），因此这里只在 Linux/FreeBSD 上启用链接组；
        -- skia-base 是渲染/图片模块的公共依赖、同样处在循环里，用 add_deps 声明为直接依赖参与链接。
        if is_plat("linux", "freebsd") then
            local grouped = {}
            for _, dep in ipairs(env.module_targets()) do
                table.insert(grouped, dep)
            end
            if env.skia_base_enabled() then
                table.insert(grouped, "duilib-skia-base")
            end
            table.insert(grouped, "duilib-text")
            table.insert(grouped, "duilib")
            add_linkgroups(table.unpack(grouped), {group = true})
        end

        -- SDL3：部分示例直接调用 SDL API（如 ChildWindow 的 SDL 绘制），
        -- 需要显式引用包，才能拿到 SDL3 的头文件目录和链接库
        if env.sdl_enabled() then
            add_packages("libsdl3")
        end

        if env.is_cef_example(name) and env.cef_enabled() then
            add_deps(env.cef_wrapper_name())
        end

        if is_plat("windows") then
            -- 窗口子系统与 manifest 使用 xmake 内置规则：
            --   platform.windows.subsystem 按工具链生成正确的 /SUBSYSTEM 链接参数
            --   platform.windows.manifest  识别 .manifest 源文件并生成 /MANIFEST:EMBED 等参数
            add_rules("platform.windows.subsystem", "platform.windows.manifest")
            set_values("windows.subsystem", "windows")
            add_files(path.join(paths.manifest_dir,
                                env.arch_name() == "x86" and "duilib.x86.manifest" or "duilib.x64.manifest"))
            for _, file in ipairs(example_windows_resources[name] or {}) do
                add_files(path.join(exdir, file))
            end
        end
end
