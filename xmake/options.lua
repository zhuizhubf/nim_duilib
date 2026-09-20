-- nim_duilib xmake 构建脚本：命令行选项
-- ---------------------------------------------------------------------------
-- 说明：对外配置项集中在本文件声明，target 与规则通过 get_config/has_config 读取；
--       分类（set_category）用于 xmake f --menu 的分组显示：
--         feature    功能开关（enable_*）
--         backend    渲染/解码后端（*_backend、*_decoder）
--         dependency 可选依赖与外部路径（with_*）
--         develop    开发辅助（enable_examples、enable_log、enable_bench）

option("enable_sdl")
    set_default(false)
    set_showmenu(true)
    set_category("feature")
    set_description("启用 SDL3 窗口实现（Windows 默认关闭，其他平台始终启用）")
option_end()

option("enable_webview2")
    set_default(true)
    set_showmenu(true)
    set_category("feature")
    set_description("启用 WebView2 控件扩展（仅 Windows 有效）")
option_end()

option("enable_pag")
    set_default(false)
    set_showmenu(true)
    set_category("feature")
    set_description("启用 libpag（需要先编译好 libpag.lib 和 libpag.dll）")
option_end()

option("enable_jpeg_turbo")
    set_default(false)
    set_showmenu(true)
    set_category("feature")
    set_description("启用 libjpeg-turbo 解码 JPEG 图片")
option_end()

option("enable_scintilla")
    set_default(false)
    set_showmenu(true)
    set_category("feature")
    set_description("启用 duilib-scintilla DUI 原生编辑器扩展")
option_end()

option("enable_common_text_layout")
    set_default(true)
    set_showmenu(true)
    set_category("feature")
    set_description("Skia 后端使用 duilib-text 的公共文本布局")
option_end()

option("render_backend")
    set_default("skia")
    set_showmenu(true)
    set_category("backend")
    set_values("skia", "gdi", "both")
    set_description("渲染后端：skia、gdi 或 both（gdi/both 仅 Windows）")
option_end()

option("svg_decoder")
    set_default("auto")
    set_showmenu(true)
    set_category("backend")
    set_values("auto", "nanosvg", "skia", "off")
    set_description("SVG 解码模块：auto、nanosvg、skia 或 off")
option_end()

option("lottie_decoder")
    set_default("auto")
    set_showmenu(true)
    set_category("backend")
    set_values("auto", "off", "skia")
    set_description("Lottie 解码模块：auto、off 或 skia")
option_end()

option("with_cef")
    set_default("off")
    set_showmenu(true)
    set_category("dependency")
    set_values("off", "latest", "109")
    set_description("CEF 控件扩展：off（默认）、latest（最新版）或 109（兼容 Win7）")
option_end()

option("with_skia_clang")
    set_default(false)
    set_showmenu(true)
    set_category("dependency")
    set_description("Windows 下使用 LLVM/Clang 编译 Skia（默认使用 MSVC，无需安装 LLVM）")
option_end()

option("with_skia_clang_dir")
    set_default("")
    set_showmenu(true)
    set_category("dependency")
    set_description("使用 clang 编译 Skia 时的 clang 目录（默认 C:/LLVM）")
option_end()

option("with_skia_dir")
    set_default("")
    set_showmenu(true)
    set_category("dependency")
    set_description("已有的 Skia 源码树目录（设置后不再自动下载/编译 Skia）")
option_end()

option("enable_examples")
    set_default(true)
    set_showmenu(true)
    set_category("develop")
    set_description("编译 examples 目录下的示例程序")
option_end()

option("enable_log")
    set_default(false)
    set_showmenu(true)
    set_category("develop")
    set_description("配置阶段输出详细的编译配置信息")
option_end()

option("enable_bench")
    set_default(false)
    set_showmenu(true)
    set_category("develop")
    set_description("编译 tools/bench_attribute_dispatch 属性名派发基准")
option_end()
