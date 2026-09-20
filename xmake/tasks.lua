--[[

    nim_duilib 的 xmake 任务定义
    ---------------------------------------------------------------------------
    本文件由根目录的 xmake.lua 通过 `includes("xmake/tasks.lua")` 引入，
    只负责注册任务的菜单与入口，实际逻辑在 xmake/scripts/ 目录下：

        xmake format              -> xmake/scripts/format_apply.lua
        xmake format-check        -> xmake/scripts/format_check.lua
        xmake attribute-gen       -> xmake/scripts/attribute_gen.lua
        xmake attribute-check     -> xmake/scripts/attribute_check.lua

    说明：`on_run("名字")` 中的模块名按"当前脚本所在目录"解析，本文件位于 xmake/，
          因此这里写成 `scripts.xxx` 即指向 xmake/scripts/xxx.lua。

]]--

task("format")
    on_run("scripts.format_apply")
    set_menu {
        usage = "xmake format",
        description = "按 .clang-format 就地格式化本地 C++ 源码（并行，跳过 3rd/tools）",
    }
task_end()

task("format-check")
    on_run("scripts.format_check")
    set_menu {
        usage = "xmake format-check",
        description = "检查本地 C++ 源码是否符合 .clang-format（不符则非零退出）",
    }
task_end()

task("attribute-gen")
    on_run("scripts.attribute_gen")
    set_menu {
        usage = "xmake attribute-gen",
        description = "由 attribute_defs.lua 生成 src/duilib/Utils/AttributeIds.g.h 与 .g.cpp",
    }
task_end()

task("attribute-check")
    on_run("scripts.attribute_check")
    set_menu {
        usage = "xmake attribute-check [options]",
        description = "检查属性名登记表：生成物同步、裸字面量残留、XML 语料覆盖（--baseline=<迁移前revision> 再比对名字集合）",
        options = {
            {'b', "baseline", "kv", nil, "与指定 revision（迁移前的提交）比对名字集合，例如 --baseline=6cfce0f4"},
        }
    }
task_end()
