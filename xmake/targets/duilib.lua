-- nim_duilib xmake 构建脚本：duilib 核心库与后端无关的文本布局
-- ---------------------------------------------------------------------------
-- 源文件逐个列出（不使用通配符）：新增/删除源文件时同步修改本文件。

local env = DUILIB
local paths = env.paths
local droot = paths.core_dir
local thirdroot = paths.third_dir

-- duilib：核心静态库，不依赖 Skia/GDI+ ---------------------------------------
target("duilib")
    set_kind("static")
    set_targetdir(paths.lib_dir)
    add_rules("duilib.features")
    -- 只挂在主库上：--enable_log=y 时在配置阶段输出一次构建配置摘要
    add_rules("duilib.log")

    -- src/duilib
    add_files(
        path.join(droot, "duilib.cpp")
    )

    -- src/duilib/Animation
    add_files(
        path.join(droot, "Animation", "AnimationManager.cpp"),
        path.join(droot, "Animation", "AnimationPlayer.cpp"),
        path.join(droot, "Animation", "EasingFunctions.cpp")
    )

    -- src/duilib/Box
    add_files(
        path.join(droot, "Box", "ListBox.cpp"),
        path.join(droot, "Box", "ListBoxHelper.cpp"),
        path.join(droot, "Box", "ScrollBox.cpp"),
        path.join(droot, "Box", "TabBox.cpp"),
        path.join(droot, "Box", "VirtualListBox.cpp"),
        path.join(droot, "Box", "XmlBox.cpp")
    )

    -- src/duilib/Layout
    add_files(
        path.join(droot, "Layout", "GridLayout.cpp"),
        path.join(droot, "Layout", "HFlowLayout.cpp"),
        path.join(droot, "Layout", "HLayout.cpp"),
        path.join(droot, "Layout", "HTileLayout.cpp"),
        path.join(droot, "Layout", "Layout.cpp"),
        path.join(droot, "Layout", "VFlowLayout.cpp"),
        path.join(droot, "Layout", "VirtualHLayout.cpp"),
        path.join(droot, "Layout", "VirtualHTileLayout.cpp"),
        path.join(droot, "Layout", "VirtualVLayout.cpp"),
        path.join(droot, "Layout", "VirtualVTileLayout.cpp"),
        path.join(droot, "Layout", "VLayout.cpp"),
        path.join(droot, "Layout", "VTileLayout.cpp")
    )

    -- src/duilib/Control
    add_files(
        path.join(droot, "Control", "AddressBar.cpp"),
        path.join(droot, "Control", "BitmapControl.cpp"),
        path.join(droot, "Control", "CheckCombo.cpp"),
        path.join(droot, "Control", "ChildWindow.cpp"),
        path.join(droot, "Control", "ChildWindowImpl.cpp"),
        path.join(droot, "Control", "CircleProgress.cpp"),
        path.join(droot, "Control", "ColorControl.cpp"),
        path.join(droot, "Control", "ColorConvert.cpp"),
        path.join(droot, "Control", "ColorPicker.cpp"),
        path.join(droot, "Control", "ColorPickerCustom.cpp"),
        path.join(droot, "Control", "ColorPickerRegular.cpp"),
        path.join(droot, "Control", "ColorPickerStandard.cpp"),
        path.join(droot, "Control", "ColorPickerStandardGray.cpp"),
        path.join(droot, "Control", "ColorSlider.cpp"),
        path.join(droot, "Control", "Combo.cpp"),
        path.join(droot, "Control", "ComboButton.cpp"),
        path.join(droot, "Control", "DateTime.cpp"),
        path.join(droot, "Control", "DateTimeWnd_SDL.cpp"),
        path.join(droot, "Control", "DateTimeWnd_Windows.cpp"),
        path.join(droot, "Control", "DirectoryTree.cpp"),
        path.join(droot, "Control", "DirectoryTreeImpl_Linux.cpp"),
        path.join(droot, "Control", "DirectoryTreeImpl_MacOS.cpp"),
        path.join(droot, "Control", "DirectoryTreeImpl_Windows.cpp"),
        path.join(droot, "Control", "FilterCombo.cpp"),
        path.join(droot, "Control", "HotKey.cpp"),
        path.join(droot, "Control", "IconControl.cpp"),
        path.join(droot, "Control", "IPAddress.cpp"),
        path.join(droot, "Control", "LabelImpl.cpp"),
        path.join(droot, "Control", "Line.cpp"),
        path.join(droot, "Control", "ListCtrl.cpp"),
        path.join(droot, "Control", "ListCtrlData.cpp"),
        path.join(droot, "Control", "ListCtrlHeader.cpp"),
        path.join(droot, "Control", "ListCtrlHeaderItem.cpp"),
        path.join(droot, "Control", "ListCtrlIcon.cpp"),
        path.join(droot, "Control", "ListCtrlIconView.cpp"),
        path.join(droot, "Control", "ListCtrlItem.cpp"),
        path.join(droot, "Control", "ListCtrlLabel.cpp"),
        path.join(droot, "Control", "ListCtrlListView.cpp"),
        path.join(droot, "Control", "ListCtrlReportView.cpp"),
        path.join(droot, "Control", "ListCtrlSubItem.cpp"),
        path.join(droot, "Control", "ListCtrlView.cpp"),
        path.join(droot, "Control", "Menu.cpp"),
        path.join(droot, "Control", "MenuBar.cpp"),
        path.join(droot, "Control", "Progress.cpp"),
        path.join(droot, "Control", "PropertyGrid.cpp"),
        path.join(droot, "Control", "RichEdit_Windows.cpp"),
        path.join(droot, "Control", "RichEdit2.cpp"),
        path.join(droot, "Control", "RichEditData.cpp"),
        path.join(droot, "Control", "RichEditDragSource_Windows.cpp"),
        path.join(droot, "Control", "RichEditDropTarget_SDL.cpp"),
        path.join(droot, "Control", "RichEditDropTarget_Windows.cpp"),
        path.join(droot, "Control", "RichEditDropTargetHelper.cpp"),
        path.join(droot, "Control", "RichEditHost_Windows.cpp"),
        path.join(droot, "Control", "RichTextImpl.cpp"),
        path.join(droot, "Control", "Slider.cpp"),
        path.join(droot, "Control", "TabCtrl.cpp"),
        path.join(droot, "Control", "TextDrawer.cpp"),
        path.join(droot, "Control", "TreeView.cpp")
    )

    -- src/duilib/Core
    add_files(
        path.join(droot, "Core", "Box.cpp"),
        path.join(droot, "Core", "BoxShadow.cpp"),
        path.join(droot, "Core", "ClickThrough_Linux.cpp"),
        path.join(droot, "Core", "ClickThrough_Windows.cpp"),
        path.join(droot, "Core", "ColorConverter.cpp"),
        path.join(droot, "Core", "ColorManager.cpp"),
        path.join(droot, "Core", "ColorMap.cpp"),
        path.join(droot, "Core", "Control.cpp"),
        path.join(droot, "Core", "ControlDropTargetImpl_SDL.cpp"),
        path.join(droot, "Core", "ControlDropTargetImpl_Windows.cpp"),
        path.join(droot, "Core", "ControlDropTargetUtils.cpp"),
        path.join(droot, "Core", "ControlFinder.cpp"),
        path.join(droot, "Core", "ControlLoading.cpp"),
        path.join(droot, "Core", "CursorManager_SDL.cpp"),
        path.join(droot, "Core", "CursorManager_Windows.cpp"),
        path.join(droot, "Core", "DpiAwareness_SDL.cpp"),
        path.join(droot, "Core", "DpiAwareness_Windows.cpp"),
        path.join(droot, "Core", "DpiManager.cpp"),
        path.join(droot, "Core", "DragWindow.cpp"),
        path.join(droot, "Core", "DragWindowFilter_SDL.cpp"),
        path.join(droot, "Core", "EventArgs.cpp"),
        path.join(droot, "Core", "FontManager.cpp"),
        path.join(droot, "Core", "FrameworkThread.cpp"),
        path.join(droot, "Core", "FullscreenBox.cpp"),
        path.join(droot, "Core", "GlobalManager.cpp"),
        path.join(droot, "Core", "IconManager.cpp"),
        path.join(droot, "Core", "ImageList.cpp"),
        path.join(droot, "Core", "ImageManager.cpp"),
        path.join(droot, "Core", "Keyboard_SDL.cpp"),
        path.join(droot, "Core", "Keyboard_Windows.cpp"),
        path.join(droot, "Core", "Keycode_SDL.cpp"),
        path.join(droot, "Core", "LangManager.cpp"),
        path.join(droot, "Core", "MessageLoop_SDL.cpp"),
        path.join(droot, "Core", "MessageLoop_Windows.cpp"),
        path.join(droot, "Core", "NativeWindow_SDL.cpp"),
        path.join(droot, "Core", "NativeWindow_Windows.cpp"),
        path.join(droot, "Core", "PlaceHolder.cpp"),
        path.join(droot, "Core", "ScrollBar.cpp"),
        path.join(droot, "Core", "SDL_Linux.cpp"),
        path.join(droot, "Core", "Shadow.cpp"),
        path.join(droot, "Core", "StateColorMap.cpp"),
        path.join(droot, "Core", "StateColorMap2.cpp"),
        path.join(droot, "Core", "ThemeGenerator.cpp"),
        path.join(droot, "Core", "ThemeManager.cpp"),
        path.join(droot, "Core", "ThreadManager.cpp"),
        path.join(droot, "Core", "ThreadMessage_SDL.cpp"),
        path.join(droot, "Core", "ThreadMessage_Windows.cpp"),
        path.join(droot, "Core", "TimerManager.cpp"),
        path.join(droot, "Core", "ToolTip_SDL.cpp"),
        path.join(droot, "Core", "ToolTip_Windows.cpp"),
        path.join(droot, "Core", "UiColors.cpp"),
        path.join(droot, "Core", "Window.cpp"),
        path.join(droot, "Core", "WindowBase.cpp"),
        path.join(droot, "Core", "WindowBuilder.cpp"),
        path.join(droot, "Core", "WindowCreateParam.cpp"),
        path.join(droot, "Core", "WindowDropTarget_SDL.cpp"),
        path.join(droot, "Core", "WindowDropTarget_Windows.cpp"),
        path.join(droot, "Core", "WindowManager.cpp"),
        path.join(droot, "Core", "WindowRoot.cpp"),
        path.join(droot, "Core", "ZipManager.cpp"),
        path.join(droot, "Core", "ZipStreamIO.cpp")
    )

    -- src/duilib/Image（Skia 相关的解码器由独立模块提供，不编入核心库）
    add_files(
        path.join(droot, "Image", "APngDecoder.cpp"),
        path.join(droot, "Image", "FrameSequence_gif.cpp"),
        path.join(droot, "Image", "Image_Animation.cpp"),
        path.join(droot, "Image", "Image_Bitmap.cpp"),
        path.join(droot, "Image", "Image_GIF.cpp"),
        path.join(droot, "Image", "Image_ICO.cpp"),
        path.join(droot, "Image", "Image_JPEG.cpp"),
        path.join(droot, "Image", "Image_PAG.cpp"),
        path.join(droot, "Image", "Image_PNG.cpp"),
        path.join(droot, "Image", "Image_Svg.cpp"),
        path.join(droot, "Image", "Image_WEBP.cpp"),
        path.join(droot, "Image", "Image.cpp"),
        path.join(droot, "Image", "ImageAttribute.cpp"),
        path.join(droot, "Image", "ImageDecoder_Common.cpp"),
        path.join(droot, "Image", "ImageDecoder_GIF.cpp"),
        path.join(droot, "Image", "ImageDecoder_ICO.cpp"),
        path.join(droot, "Image", "ImageDecoder_Icon.cpp"),
        path.join(droot, "Image", "ImageDecoder_JPEG.cpp"),
        path.join(droot, "Image", "ImageDecoder_PAG.cpp"),
        path.join(droot, "Image", "ImageDecoder_PNG.cpp"),
        path.join(droot, "Image", "ImageDecoder_WEBP.cpp"),
        path.join(droot, "Image", "ImageDecoderFactory.cpp"),
        path.join(droot, "Image", "ImageDecoderUtil.cpp"),
        path.join(droot, "Image", "ImageInfo.cpp"),
        path.join(droot, "Image", "ImageLoadParam.cpp"),
        path.join(droot, "Image", "ImagePlayer.cpp"),
        path.join(droot, "Image", "ImageUtil.cpp"),
        path.join(droot, "Image", "StateImage.cpp"),
        path.join(droot, "Image", "StateImageMap.cpp")
    )

    -- src/duilib/Utils
    add_files(
        path.join(droot, "Utils", "ApiWrapper_Windows.cpp"),
        path.join(droot, "Utils", "AttributeIds.g.cpp"),
        path.join(droot, "Utils", "AttributeUtil.cpp"),
        path.join(droot, "Utils", "BitmapHepler_SDL.cpp"),
        path.join(droot, "Utils", "BitmapHepler_Windows.cpp"),
        path.join(droot, "Utils", "Clipboard_SDL.cpp"),
        path.join(droot, "Utils", "Clipboard_Windows.cpp"),
        path.join(droot, "Utils", "DiskUtils_Windows.cpp"),
        path.join(droot, "Utils", "DllManager_Windows.cpp"),
        path.join(droot, "Utils", "FileDialog_SDL.cpp"),
        path.join(droot, "Utils", "FileDialog_Windows.cpp"),
        path.join(droot, "Utils", "FilePath.cpp"),
        path.join(droot, "Utils", "FilePathUtil.cpp"),
        path.join(droot, "Utils", "FileTime.cpp"),
        path.join(droot, "Utils", "FileUtil.cpp"),
        path.join(droot, "Utils", "InlineHook_Windows.cpp"),
        path.join(droot, "Utils", "LogUtil.cpp"),
        path.join(droot, "Utils", "MonitorUtil_SDL.cpp"),
        path.join(droot, "Utils", "MonitorUtil_Windows.cpp"),
        path.join(droot, "Utils", "PerformanceUtil.cpp"),
        path.join(droot, "Utils", "ProcessSingleton.cpp"),
        path.join(droot, "Utils", "ScreenCapture_Linux.cpp"),
        path.join(droot, "Utils", "ScreenCapture_Wayland.cpp"),
        path.join(droot, "Utils", "ScreenCapture_Windows.cpp"),
        path.join(droot, "Utils", "ScreenCapture_X11.cpp"),
        path.join(droot, "Utils", "StringCharset.cpp"),
        path.join(droot, "Utils", "StringConvert.cpp"),
        path.join(droot, "Utils", "StringUtil.cpp"),
        path.join(droot, "Utils", "SystemUtil_SDL.cpp"),
        path.join(droot, "Utils", "SystemUtil_Windows.cpp"),
        path.join(droot, "Utils", "TrayIcon.cpp"),
        path.join(droot, "Utils", "TrayIcon_SDL.cpp"),
        path.join(droot, "Utils", "TrayIcon_Windows.cpp"),
        path.join(droot, "Utils", "WinImplBase.cpp")
    )

    -- macOS 平台专用的 Objective-C++ 实现
    if is_plat("macosx") then
        add_files(
            path.join(droot, "Core", "ClickThrough_MacOS.mm"),
            path.join(droot, "Core", "SDL_MacOS.mm"),
            path.join(droot, "Utils", "ScreenCapture_MacOS.mm")
        )
    end

    -- 渲染接口层（Render 已移出核心目录，但其源码仍编入核心库）
    add_files(
        path.join(paths.src_dir, "render", "AutoClip.cpp"),
        path.join(paths.src_dir, "render", "BitmapAlpha.cpp"),
        path.join(paths.src_dir, "render", "WindowRgn_Windows.cpp")
    )

    -- 内置的第三方源码
    add_files(
        path.join(thirdroot, "giflib", "dgif_lib.c"),
        path.join(thirdroot, "giflib", "egif_lib.c"),
        path.join(thirdroot, "giflib", "gif_err.c"),
        path.join(thirdroot, "giflib", "gif_font.c"),
        path.join(thirdroot, "giflib", "gif_hash.c"),
        path.join(thirdroot, "giflib", "gifalloc.c"),
        path.join(thirdroot, "giflib", "openbsd-reallocarray.c"),
        path.join(thirdroot, "giflib", "quantize.c"),
        path.join(thirdroot, "zlib", "contrib", "minizip", "ioapi.c"),
        path.join(thirdroot, "zlib", "contrib", "minizip", "unzip.c"),
        path.join(thirdroot, "convert_utf", "ConvertUTF.cpp"),
        path.join(thirdroot, "xml", "pugixml.cpp")
    )
    if is_plat("windows") then
        add_files(
            path.join(thirdroot, "libudis86", "decode.c"),
            path.join(thirdroot, "libudis86", "itab.c"),
            path.join(thirdroot, "libudis86", "syn-att.c"),
            path.join(thirdroot, "libudis86", "syn-intel.c"),
            path.join(thirdroot, "libudis86", "syn.c"),
            path.join(thirdroot, "libudis86", "udis86.c")
        )
    end

    -- 头文件目录
    add_includedirs(
        paths.src_dir,
        paths.root,
        path.join(thirdroot, "zlib"),
        path.join(thirdroot, "giflib"),
        path.join(thirdroot, "libpng"),
        path.join(thirdroot, "convert_utf"),
        path.join(thirdroot, "xml")
    )

    -- 第三方静态库
    add_deps("duilib-zlib", "duilib-png", "duilib-cximage", "duilib-webp")

    -- SDL3（可选项，Windows 默认关闭，其他平台默认开启）
    if env.sdl_enabled() then
        add_packages("libsdl3")
    end

    -- libjpeg-turbo（可选项）
    if has_config("enable_jpeg_turbo") then
        if is_plat("windows") then
            local jpegdir = path.join(thirdroot, "prebuilt/libjpeg-turbo",
                                      env.arch_name() == "x86" and "libjpeg-turbo6-win-vc-x86" or "libjpeg-turbo6-win-vc-x64")
            add_includedirs(path.join(jpegdir, "include"))
            add_linkdirs(path.join(jpegdir, "lib"))
            add_links("turbojpeg-static")
        else
            add_packages("libjpeg-turbo")
        end
    end

    -- libpag（可选项：需要自己编译 libpag.lib 和 libpag.dll）
    if has_config("enable_pag") then
        local pagroot = path.join(thirdroot, "prebuilt/libpag/windows")
        local pagdir = path.join(pagroot, env.arch_name() == "x86" and "lib-vc-x86" or "lib-vc-x64")
        add_includedirs(pagroot)
        add_linkdirs(pagdir)
        add_links("libpag")
    end

-- duilib-text：后端无关的文本布局 ---------------------------------------------
target("duilib-text")
    set_kind("static")
    set_targetdir(paths.lib_dir)
    add_rules("duilib.features")
    add_deps("duilib")
    add_files(
        path.join(paths.src_dir, "text", "TextLayout.cpp")
    )
    add_includedirs(paths.src_dir, paths.root)
