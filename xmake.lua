--[[

    nim_duilib 的 xmake 构建脚本
    ---------------------------------------------------------------------------
    本文件是项目唯一的构建入口（原 CMake 工程、VS 解决方案和 bat/sh 脚本已移除）。

    快速开始（Windows + MSVC x64）：
        xmake f -o build/build_temp/xmake -c         # 配置（Release，当前平台/架构）
        xmake                                        # 编译全部（第三方库 + duilib + 全部示例）
        xmake build basic                            # 只编译 basic 示例
        xmake run basic                              # 运行 basic 示例

    常用配置（执行 xmake f 配置后生效）：
        xmake f -m debug                             # Debug 编译
        xmake f -a x86                               # 32 位编译
        xmake f --enable_examples=n                  # 只编译库，不编译示例
        xmake f --enable_sdl=y                       # 启用 SDL3（Windows 默认关闭，其他平台默认开启）
        xmake f --with_cef=latest                    # 启用 CEF（编译 cef / CefBrowser 示例）
        xmake f --with_cef=109                       # 使用 CEF 109 版本（兼容 Win7）
        xmake f --enable_webview2=n                  # 关闭 WebView2 控件（Windows，默认开启）
        xmake f --enable_pag=y                       # 启用 libpag（需按文档自行编译 libpag.lib/libpag.dll）
        xmake f --enable_jpeg_turbo=y                # 启用 libjpeg-turbo 解码
        xmake f --runtimes=MD                        # MSVC 运行库使用 /MD（xmake 内置选项，默认 MT）
        xmake f --enable_log=y                       # 输出详细的编译配置信息
        xmake f --enable_bench=y                     # 额外编译属性名派发基准（tools/bench_attribute_dispatch）
        xmake f --with_skia_clang=y                  # Windows 下改用 LLVM/Clang 编译 Skia（默认用 MSVC，无需安装 LLVM）
        xmake f --with_skia_clang_dir=D:/LLVM        # 使用 clang 编译 Skia 时的 clang 目录（默认 C:/LLVM）
        xmake f --with_skia_dir=../skia              # 直接使用已有的 Skia 源码树（跳过自动下载编译）

    脚本结构（仓库根目录只保留本文件，其余脚本按职责分目录）：
        xmake/options.lua           选项声明（feature / backend / dependency / develop）
        xmake/env.lua               公共路径与配置判定（描述域全局表 DUILIB）
        xmake/rules/                项目规则（duilib.config / duilib.features / duilib.app /
                                    duilib.skia / duilib.log）
        xmake/targets/              目标定义（第三方库、duilib、渲染、图片、扩展、示例、基准）
        xmake/tasks/register.lua    任务注册（任务脚本与数据表在 xmake/scripts/）
        xmake/manifest/             Windows 清单文件
        xmake/repos/                本地包仓库（duilib-skia 自动下载并编译）

    说明：
        1. Skia 由项目内的本地包（xmake/repos 下的 duilib-skia）自动下载并编译，只编译当前配置；
           Windows 下默认使用 MSVC（cl.exe）编译，不需要安装 LLVM；如需改用 clang，配置 --with_skia_clang=y；
        2. SDL3 由 xmake 官方包仓库自动获取；
        3. 其他第三方库使用仓库内的源码/预编译库，编译方式与原构建方式保持一致；
        4. 库文件输出到 lib/，可执行文件输出到 bin/，与现有脚本一致。

]]

set_project("nim_duilib")
set_version("1.0.0")
set_xmakever("2.9.0")
set_languages("c++20")

-- 挂载 xmake 的模式规则，把 `-m release` / `-m debug` 翻译成对应的编译参数：
--   release：/O2 + -DNDEBUG（无调试符号）
--   debug  ：/Od + /Zi（生成调试符号）
-- 说明：xmake 不会自动挂载这两条规则，缺少它们时 `xmake f -m release` 会退化为"无优化"编译，
--       体积显著偏大且运行性能下降，因此必须在项目根目录显式挂载（作用于全部 target）。
add_rules("mode.debug", "mode.release")

-- 项目内的本地包仓库（Skia 自动下载并编译）
add_repositories("duilib-repo xmake/repos")

-- 选项与公共配置：选项必须先声明，xmake/env.lua 提供的 DUILIB 表供后续脚本使用
includes("xmake/options.lua")
includes("xmake/env.lua")

-- 运行库设置：项目默认使用静态运行库 MT，用 xmake 内置选项 --runtimes 覆盖
-- （MT/MTd/MD/MDd）；在根作用域设置会应用到本工程的全部 target。
-- 说明：依赖包各自的运行库由 add_requires 的 configs 决定（例如 duilib-skia 的 runtime 参数），
-- 与本工程的 /MT 不一致时会在链接期报 __imp_itoa/__imp_lround 之类的未解析符号。
if is_plat("windows") then
    set_runtimes(get_config("runtimes") or "MT")
end

-- 依赖包（xmake 要求在根作用域声明，target 中只使用 add_packages 引用）
if DUILIB.sdl_enabled() then
    add_requires("libsdl3", {configs = {shared = false}})
end
if DUILIB.skia_base_enabled() and not DUILIB.skia_dir() then
    add_requires("duilib-skia", {configs = {
        clang = DUILIB.skia_clang_enabled(),
        clang_dir = DUILIB.skia_clang_dir(),
        runtime = DUILIB.runtime()
    }})
end
if has_config("enable_jpeg_turbo") and not is_plat("windows") then
    add_requires("libjpeg-turbo")
end

-- 构建辅助任务（xmake format / format-check / attribute-gen / attribute-check）
includes("xmake/tasks/register.lua")

-- 项目规则（必须在 target 之前注册）
includes("xmake/rules/config.lua")
includes("xmake/rules/features.lua")
includes("xmake/rules/app.lua")
includes("xmake/rules/skia.lua")
includes("xmake/rules/log.lua")

-- 目标
includes("xmake/targets/third_party.lua")
includes("xmake/targets/duilib.lua")
includes("xmake/targets/render.lua")
includes("xmake/targets/image.lua")
includes("xmake/targets/extensions.lua")
if DUILIB.examples_enabled() then
    includes("xmake/targets/examples.lua")
end
if has_config("enable_bench") then
    includes("xmake/targets/bench.lua")
end
