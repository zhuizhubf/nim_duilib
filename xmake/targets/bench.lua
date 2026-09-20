-- nim_duilib xmake 构建脚本：属性名派发基准（可选，--enable_bench=y）
-- ---------------------------------------------------------------------------
-- bench.cpp 默认读取仓库内的 xmake/scripts/attribute_defs.lua，
-- 因此把运行目录设为仓库根目录，`xmake run bench_attribute_dispatch` 才能直接运行。

local env = DUILIB
local paths = env.paths

target("bench_attribute_dispatch")
    set_kind("binary")
    set_languages("c++17")
    set_optimize("fastest")
    set_targetdir(paths.bin_dir)
    set_rundir(paths.root)
    add_rules("duilib.config")
    add_files(path.join(paths.bench_dir, "bench.cpp"))
