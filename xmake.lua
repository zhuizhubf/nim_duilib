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
        xmake f --examples=n                         # 只编译库，不编译示例
        xmake f --sdl=y                              # 启用 SDL3（Windows 默认关闭，其他平台默认开启）
        xmake f --cef=y                              # 启用 CEF（编译 cef / CefBrowser 示例）
        xmake f --cef109=y                           # 使用 CEF 109 版本（兼容 Win7）
        xmake f --webview2=n                         # 关闭 WebView2 控件（Windows，默认开启）
        xmake f --pag=y                              # 启用 libpag（需按文档自行编译 libpag.lib/libpag.dll）
        xmake f --jpeg_turbo=y                       # 启用 libjpeg-turbo 解码
        xmake f --md=y                               # MSVC 运行库使用 /MD（默认 /MT）
        xmake f --log=y                              # 输出详细的编译配置信息
        xmake f --skia_clang=y                       # Windows 下改用 LLVM/Clang 编译 Skia（默认用 MSVC，无需安装 LLVM）
        xmake f --skia_clang_dir=D:/LLVM             # 使用 clang 编译 Skia 时的 clang 目录（默认 C:/LLVM）
        xmake f --skia_dir=../skia                   # 直接使用已有的 Skia 源码树（跳过自动下载编译）

    说明：
        1. Skia 由项目内的本地包（xmake/repos 下的 duilib-skia）自动下载并编译，只编译当前配置；
           Windows 下默认使用 MSVC（cl.exe）编译，不需要安装 LLVM；如需改用 clang，配置 --skia_clang=y；
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

-- 公共变量（xmake/ 目录下的脚本可以直接使用）
DUILIB_ROOT      = os.projectdir()
-- 库源码根目录，同时作为包含根：duilib/、render/、render-skia/、render-gdi/、text/
DUILIB_SRC_DIR   = path.join(DUILIB_ROOT, "src")
-- 核心库源码目录（保持 duilib/ 名称，公开包含路径不变）
DUILIB_CORE_DIR  = path.join(DUILIB_SRC_DIR, "duilib")
-- 第三方依赖目录（含 prebuilt/ 预编译 SDK）
DUILIB_THIRD_DIR = path.join(DUILIB_ROOT, "third_party")
-- 扩展目录（可选控件扩展，拥有自己的源码、第三方依赖、文档与许可）
DUILIB_EXT_SCINTILLA_DIR = path.join(DUILIB_ROOT, "extensions", "scintilla")
DUILIB_EXT_CEF_DIR       = path.join(DUILIB_ROOT, "extensions", "cef")
DUILIB_EXT_WEBVIEW2_DIR  = path.join(DUILIB_ROOT, "extensions", "webview2")
DUILIB_LIB_DIR   = path.join(DUILIB_ROOT, "lib")
DUILIB_BIN_DIR   = path.join(DUILIB_ROOT, "bin")
DUILIB_SKIA_LIBS = {"svg", "skshaper", "skottie", "sksg", "jsonreader", "skia"}

-- 公共选项
option("sdl")
    set_default(false)
    set_showmenu(true)
    set_description("启用 SDL3 支持（非 Windows 平台始终启用）")
option_end()

option("cef")
    set_default(false)
    set_showmenu(true)
    set_description("启用 CEF：编译 libcef_dll_wrapper，并编译 cef/CefBrowser 示例")
option_end()

option("cef109")
    set_default(false)
    set_showmenu(true)
    set_description("使用 CEF 109 版本（兼容 Win7）")
option_end()

option("webview2")
    set_default(true)
    set_showmenu(true)
    set_description("启用 WebView2 控件（仅 Windows 有效，默认开启）")
option_end()

option("pag")
    set_default(false)
    set_showmenu(true)
    set_description("启用 libpag（需要先编译好 libpag.lib 和 libpag.dll）")
option_end()

option("jpeg_turbo")
    set_default(false)
    set_showmenu(true)
    set_description("启用 libjpeg-turbo 解码 JPEG 图片")
option_end()

option("md")
    set_default(false)
    set_showmenu(true)
    set_description("MSVC 运行库使用 /MD（默认使用 /MT）")
option_end()

option("log")
    set_default(false)
    set_showmenu(true)
    set_description("输出详细的编译配置信息")
option_end()

option("examples")
    set_default(true)
    set_showmenu(true)
    set_description("编译 examples 目录下的示例程序")
option_end()

option("scintilla")
    set_default(false)
    set_showmenu(true)
    set_description("启用 duilib-scintilla DUI 原生编辑器扩展")
option_end()

option("render")
    set_default("skia")
    set_showmenu(true)
    set_values("skia", "gdi", "both")
    set_description("渲染后端：skia、gdi 或 both（gdi/both 仅 Windows）")
option_end()

option("svg")
    set_default("auto")
    set_showmenu(true)
    set_values("auto", "nanosvg", "skia", "off")
    set_description("SVG 解码模块：auto、nanosvg、skia 或 off")
option_end()

option("lottie")
    set_default("auto")
    set_showmenu(true)
    set_values("auto", "off", "skia")
    set_description("Lottie 解码模块：auto、off 或 skia")
option_end()

option("common_text_layout")
    set_default(true)
    set_showmenu(true)
    set_description("Skia 后端使用 duilib-text 的公共文本布局（默认开启）")
option_end()

option("skia_dir")
    set_default("")
    set_showmenu(true)
    set_description("已有的 Skia 源码树目录（设置后不再自动下载/编译 Skia）")
option_end()

option("skia_clang_dir")
    set_default("")
    set_showmenu(true)
    set_description("使用 clang 编译 Skia 时的 clang 目录（默认 C:/LLVM）")
option_end()

option("skia_clang")
    set_default(false)
    set_showmenu(true)
    set_description("Windows 下使用 LLVM/Clang 编译 Skia（默认使用 MSVC，无需安装 LLVM）")
option_end()

-- 构建脚本
includes("xmake/common.lua")

-- 依赖包（xmake 要求在根作用域声明，target 中只使用 add_packages 引用）
if duilib_sdl_enabled() then
    add_requires("libsdl3", {configs = {shared = false}})
end
if duilib_skia_base_enabled() and not duilib_skia_dir() then
    add_requires("duilib-skia", {configs = {
        clang = (get_config("skia_clang") == true),
        clang_dir = duilib_skia_clang_dir(),
        runtime = (get_config("md") and "MD" or "MT")
    }})
end
if get_config("jpeg_turbo") and duilib_plat() ~= "windows" then
    add_requires("libjpeg-turbo")
end


task("format")
    on_run("format_apply")
    set_menu {
        usage = "xmake format",
        description = "按 .clang-format 就地格式化本地 C++ 源码（并行，跳过 3rd/tools）",
    }
task_end()

task("format-check")
    on_run("format_check")
    set_menu {
        usage = "xmake format-check",
        description = "检查本地 C++ 源码是否符合 .clang-format（不符则非零退出）",
    }
task_end()

task("attribute-gen")
    on_run("attribute_gen")
    set_menu {
        usage = "xmake attribute-gen",
        description = "由 attribute_defs.lua 生成 src/duilib/Utils/AttributeIds.g.h 与 .g.cpp",
    }
task_end()

task("attribute-check")
    on_run("attribute_check")
    set_menu {
        usage = "xmake attribute-check [options]",
        description = "检查属性名登记表：生成物同步、裸字面量残留、XML 语料覆盖（--baseline=<迁移前revision> 再比对名字集合）",
        options = {
            {'b', "baseline", "kv", nil, "与指定 revision（迁移前的提交）比对名字集合，例如 --baseline=6cfce0f4"},
        }
    }
task_end()

includes("xmake/third_party.lua")
includes("xmake/duilib.lua")

if get_config("examples") then
    includes("xmake/examples.lua")
end
