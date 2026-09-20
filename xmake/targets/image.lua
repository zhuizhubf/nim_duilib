-- nim_duilib xmake 构建脚本：图片解码模块（SVG / Lottie）
-- ---------------------------------------------------------------------------
-- 解码模块由 svg_decoder / lottie_decoder 决定（auto 时跟随渲染后端）。

local env = DUILIB
local paths = env.paths
local core = paths.core_dir

-- SVG（NanoSVG 实现）----------------------------------------------------------
if env.svg_nanosvg_enabled() then
    target("duilib-image-svg-nanosvg")
        set_kind("static")
        set_targetdir(paths.lib_dir)
        add_rules("duilib.features")
        add_deps("duilib")
        add_files(path.join(core, "Image", "ImageDecoder_SVG_NanoSvg.cpp"))
        add_includedirs(paths.src_dir, paths.root, path.join(paths.third_dir, "svg"))
    target_end()
end

-- SVG（Skia 实现）------------------------------------------------------------
if env.svg_skia_enabled() then
    target("duilib-image-svg-skia")
        set_kind("static")
        set_targetdir(paths.lib_dir)
        add_rules("duilib.features", "duilib.skia")
        add_deps("duilib")
        if env.skia_base_enabled() then
            add_deps("duilib-skia-base")
        end
        add_files(
            path.join(core, "Image", "ImageDecoder_SVG.cpp"),
            path.join(core, "Image", "ImageDecoderModule_SvgSkia.cpp")
        )
        add_includedirs(paths.src_dir, paths.root)
    target_end()
end

-- Lottie（Skia 实现）----------------------------------------------------------
if env.lottie_skia_enabled() then
    target("duilib-image-lottie-skia")
        set_kind("static")
        set_targetdir(paths.lib_dir)
        add_rules("duilib.features", "duilib.skia")
        add_deps("duilib")
        if env.skia_base_enabled() then
            add_deps("duilib-skia-base")
        end
        add_files(
            path.join(core, "Image", "ImageDecoder_LOTTIE.cpp"),
            path.join(core, "Image", "Image_LOTTIE.cpp"),
            path.join(core, "Image", "ImageDecoderModule_LottieSkia.cpp")
        )
        add_includedirs(paths.src_dir, paths.root)
    target_end()
end
