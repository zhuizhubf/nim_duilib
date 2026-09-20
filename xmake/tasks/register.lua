--[[

    nim_duilib 的 xmake 任务注册
    ---------------------------------------------------------------------------
    本文件由根目录的 xmake.lua 通过 `includes("xmake/tasks/register.lua")` 引入，
    只负责注册任务的菜单与入口，实际逻辑在 xmake/scripts/ 目录下：

        xmake format              -> xmake/scripts/format_apply.lua
        xmake format-check        -> xmake/scripts/format_check.lua
        xmake attribute-gen       -> xmake/scripts/attribute_gen.lua
        xmake attribute-check     -> xmake/scripts/attribute_check.lua

    说明：任务入口用显式 rootdir 导入脚本（xmake/scripts），不依赖 on_run 的模块搜索路径；
          on_run 收到的参数原样转交给脚本的 main(...)，因此脚本直跑形式
          （`xmake l xmake/scripts/xxx.lua <参数>`）与任务形式的参数处理保持一致。

]]--

local scripts_dir = path.join(os.projectdir(), "xmake")

local function task_entry(module)
    return function (...)
        import(module, {rootdir = scripts_dir}).main(...)
    end
end

task("format")
    on_run(task_entry("scripts.format_apply"))
    set_menu {
        usage = "xmake format",
        description = "按 .clang-format 就地格式化本地 C++ 源码（并行，跳过 3rd/tools）",
    }
task_end()

task("format-check")
    on_run(task_entry("scripts.format_check"))
    set_menu {
        usage = "xmake format-check",
        description = "检查本地 C++ 源码是否符合 .clang-format（不符则非零退出）",
    }
task_end()

task("attribute-gen")
    on_run(task_entry("scripts.attribute_gen"))
    set_menu {
        usage = "xmake attribute-gen",
        description = "由 attribute_defs.lua 生成 src/duilib/Utils/AttributeIds.g.h 与 .g.cpp",
    }
task_end()

task("attribute-check")
    on_run(task_entry("scripts.attribute_check"))
    set_menu {
        usage = "xmake attribute-check [options]",
        description = "检查属性名登记表：生成物同步、裸字面量残留、XML 语料覆盖（--baseline=<迁移前revision> 再比对名字集合）",
        options = {
            {'b', "baseline", "kv", nil, "与指定 revision（迁移前的提交）比对名字集合，例如 --baseline=6cfce0f4"},
        }
    }
task_end()
