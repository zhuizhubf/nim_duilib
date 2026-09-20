-- nim_duilib xmake 构建脚本：渲染后端（Skia / GDI）
-- ---------------------------------------------------------------------------
-- 目标按配置启用：render_backend=skia|gdi|both（gdi/both 仅 Windows）。
-- 条件块内创建的 target 必须显式 target_end()，否则条件块之后的顶层语句会落到该 target 上。

local env = DUILIB
local paths = env.paths

-- duilib-skia-base：Skia 字体和公共基础封装 -----------------------------------
if env.skia_base_enabled() then
    target("duilib-skia-base")
        set_kind("static")
        set_targetdir(paths.lib_dir)
        add_rules("duilib.features", "duilib.skia")
        add_deps("duilib")
        add_files(
            path.join(paths.src_dir, "render-skia", "Font_Skia.cpp"),
            path.join(paths.src_dir, "render-skia", "FontMgr_Skia.cpp")
        )
        add_includedirs(paths.src_dir, paths.root)
    target_end()
end

-- duilib-render-skia：Skia 渲染后端 -------------------------------------------
if env.render_skia_enabled() then
    target("duilib-render-skia")
        set_kind("static")
        set_targetdir(paths.lib_dir)
        add_rules("duilib.features", "duilib.skia")
        add_deps("duilib", "duilib-text")
        if env.skia_base_enabled() then
            add_deps("duilib-skia-base")
        end
        -- src/render-skia（Font_Skia.cpp / FontMgr_Skia.cpp 属于 duilib-skia-base）
        add_files(
            path.join(paths.src_dir, "render-skia", "Bitmap_Skia.cpp"),
            path.join(paths.src_dir, "render-skia", "Brush_Skia.cpp"),
            path.join(paths.src_dir, "render-skia", "DrawRichText.cpp"),
            path.join(paths.src_dir, "render-skia", "DrawSkiaImage.cpp"),
            path.join(paths.src_dir, "render-skia", "DrawSkiaText.cpp"),
            path.join(paths.src_dir, "render-skia", "HorizontalDrawText.cpp"),
            path.join(paths.src_dir, "render-skia", "Matrix_Skia.cpp"),
            path.join(paths.src_dir, "render-skia", "Path_Skia.cpp"),
            path.join(paths.src_dir, "render-skia", "Pen_Skia.cpp"),
            path.join(paths.src_dir, "render-skia", "Render_Skia.cpp"),
            path.join(paths.src_dir, "render-skia", "Render_Skia_SDL.cpp"),
            path.join(paths.src_dir, "render-skia", "Render_Skia_Windows.cpp"),
            path.join(paths.src_dir, "render-skia", "RenderBackend_Skia.cpp"),
            path.join(paths.src_dir, "render-skia", "RenderFactory_Skia.cpp"),
            path.join(paths.src_dir, "render-skia", "SkGLWindowContext_Windows.cpp"),
            path.join(paths.src_dir, "render-skia", "SkRasterWindowContext_SDL.cpp"),
            path.join(paths.src_dir, "render-skia", "SkRasterWindowContext_Windows.cpp"),
            path.join(paths.src_dir, "render-skia", "SkTextBox.cpp"),
            path.join(paths.src_dir, "render-skia", "SkUTF.cpp"),
            path.join(paths.src_dir, "render-skia", "SkiaTextData.cpp"),
            path.join(paths.src_dir, "render-skia", "SkiaTextShaper.cpp"),
            path.join(paths.src_dir, "render-skia", "VerticalDrawText.cpp")
        )
        add_includedirs(paths.src_dir, paths.root)

        -- 渲染后端的 SDL 窗口实现（render-skia/Render_Skia_SDL.cpp、
        -- render-skia/SkRasterWindowContext_SDL.cpp）直接调用 SDL3 API 把绘制结果输出到窗口，
        -- 需要显式引用包才能拿到 SDL3 的头文件与链接库（非 Windows 平台默认启用 SDL）
        if env.sdl_enabled() then
            add_packages("libsdl3")
        end

        -- Skia 的窗口相关源码：默认来自本地包的安装目录（<installdir>/skia），
        -- 配置 --with_skia_dir 时来自指定的源码树；两种来源都要等依赖解析后才能确定，
        -- 因此在 on_load（脚本域）里加入，并按文件是否存在探测，避免报 add_files 匹配失败。
        on_load(function (target)
            local root = env.skia_dir()
            if not root then
                local pkg = target:pkg("duilib-skia")
                if pkg then
                    root = path.join(pkg:installdir(), "skia")
                end
            end
            if not root then
                return
            end
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
        end)
    target_end()
end

-- duilib-render-gdi：Windows GDI/GDI+ 渲染后端 ---------------------------------
if env.render_gdi_enabled() then
    target("duilib-render-gdi")
        set_kind("static")
        set_targetdir(paths.lib_dir)
        add_rules("duilib.features")
        add_deps("duilib", "duilib-text")
        add_files(
            path.join(paths.src_dir, "render-gdi", "GdiTypes.cpp"),
            path.join(paths.src_dir, "render-gdi", "RenderFactory_GDI.cpp"),
            path.join(paths.src_dir, "render-gdi", "Render_GDI_Windows.cpp")
        )
        add_includedirs(paths.src_dir, paths.root)
        add_syslinks("Gdi32", "Gdiplus", "Msimg32", "User32")
    target_end()
end
