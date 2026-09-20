-- rule("duilib.features")：duilib 公共特性宏
-- ---------------------------------------------------------------------------
-- DUILIB_SDL / DUILIB_RENDER_SKIA 等宏由 src/duilib/duilib_config.h 判断，
-- 声明为 {public = true} 后会经 add_deps 传递给依赖方（示例、基准程序等），
-- 因此各 target 不需要各自重复一遍宏定义。

local env = DUILIB

rule("duilib.features")
    add_deps("duilib.config")
    on_load(function (target)
        for _, define in ipairs(env.feature_defines()) do
            target:add("defines", define, {public = true})
        end
    end)
