-- rule("duilib.log")：--enable_log=y 时输出构建配置摘要
-- ---------------------------------------------------------------------------
-- 只在 on_config（脚本域、每个 target 一次）里输出：描述域被解析多次，不能在其中 print。
-- 本规则只挂在 duilib 主库上，保证每次配置只打印一次。

local env = DUILIB

rule("duilib.log")
    on_config(function (target)
        if not (has_config("enable_log") and get_config("plat")) then
            return
        end
        print("duilib xmake build config:")
        print("    plat          : " .. get_config("plat") .. " / " .. env.arch_name())
        print("    mode          : " .. env.build_mode())
        print("    render        : " .. env.render_backend())
        print("    svg           : " .. env.svg_decoder())
        print("    lottie        : " .. env.lottie_decoder())
        print("    sdl           : " .. tostring(env.sdl_enabled()))
        print("    cef           : " .. env.cef_version())
        print("    webview2      : " .. tostring(env.webview2_enabled()))
        print("    jpeg_turbo    : " .. tostring(has_config("enable_jpeg_turbo")))
        print("    libpag        : " .. tostring(has_config("enable_pag")))
        print("    skia_dir      : " .. (env.skia_dir() or "(本地包自动编译)"))
        print("    lib dir       : " .. env.paths.lib_dir)
        print("    bin dir       : " .. env.paths.bin_dir)
    end)
