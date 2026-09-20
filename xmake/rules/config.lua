-- rule("duilib.config")：所有参与编译的 target 的基础配置
-- ---------------------------------------------------------------------------
-- 源码编码与 Windows 平台宏。第三方库与 duilib 自身都使用本规则；
-- MSVC 运行库由根作用域的 set_runtimes 统一设置（已应用到全部 target），此处不再重复设置。

local env = DUILIB

rule("duilib.config")
    on_load(function (target)
        target:set("encodings", "utf-8")
        if target:is_plat("windows") then
            target:add("defines", "UNICODE", "_UNICODE")
        end
    end)
