-- rule("duilib.skia")：接入 Skia 渲染/解码所需的基础层
-- ---------------------------------------------------------------------------
-- 两种来源：
--   1. 默认使用项目内的本地包 duilib-skia（自动下载 + 编译，见 xmake/repos）；
--   2. 配置 --with_skia_dir 时直接使用已有的 Skia 源码树与编译产物。
-- 规则在 on_load 中执行：本地包需要等依赖解析完成后才能引用。

local env = DUILIB

rule("duilib.skia")
    on_load(function (target)
        local skia_dir = env.skia_dir()
        if skia_dir then
            target:add("includedirs", skia_dir)
            local libdir = env.skia_libdir()
            if libdir then
                target:add("linkdirs", libdir)
            end
            target:add("links", env.paths.skia_libs)
        else
            target:add("packages", "duilib-skia")
        end

        target:add("defines", "SK_GANESH", "SK_GL", "SK_RELEASE")

        if target:is_plat("linux", "freebsd") then
            -- Skia 的字体后端在这些平台依赖系统 freetype/fontconfig，
            -- 声明为 syslinks 会随静态库依赖传递到最终可执行文件并排在最后
            target:add("syslinks", "freetype", "fontconfig")
        end
    end)
