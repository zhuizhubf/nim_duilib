-- =============================================================================
-- xmake format-check：只检查、不修改文件；存在不符合 .clang-format 的文件则非零退出。
-- 适合作为 CI 门禁：`xmake format-check`
--
-- 与 `xmake format` 共用同一套引擎（format_apply.lua）：把这两个文件一起复制到任意
-- 项目的同一个脚本目录，并在 xmake.lua 中注册任务即可使用；扫描范围、跳过规则、
-- 并行度等配置项见 format_apply.lua 顶部说明。
--
-- 用法：
--     xmake format-check                              -- 检查整个项目
--     XMAKE_FORMAT_PATHS=src xmake format-check       -- 只检查指定目录/文件
--     xmake l xmake/scripts/format_check.lua src examples   -- 脚本直跑形式，可带路径参数
-- =============================================================================

import("format_apply", { rootdir = os.scriptdir() })

function main(...)
    return format_apply.run_clang_format("check", ...)
end
