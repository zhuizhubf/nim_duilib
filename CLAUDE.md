# nim_duilib - 跨平台 C++ UI 库

## 项目概述
nim_duilib 是基于 Skia 渲染引擎的跨平台 C++ UI 框架，采用 XML 描述界面布局 + C++ 控制逻辑的开发模式。
- **支持平台**: Windows (7/10/11+), Linux, macOS (12+), FreeBSD
- **渲染引擎**: Skia (CPU/OpenGL)
- **构建工具**: xmake（Skia 由项目内置本地包自动下载并编译）
- **C++ 标准**: C++17+

## 项目结构
```
nim_duilib/
├── src/                 # 所有库源码
│   ├── duilib/          # 核心库（含 Core/ Control/ Box/ Layout/ Animation/ Image/ Utils/）
│   ├── text/            # 后端无关的文本布局（duilib-text）
│   ├── render/          # 渲染接口
│   ├── render-skia/     # Skia渲染实现
│   └── render-gdi/      # Windows GDI/GDI+ 渲染实现
├── extensions/          # 可选控件扩展（cef / webview2 / scintilla）
│   ├── cef/             # CEF浏览器控件扩展（--with_cef=latest）
│   ├── webview2/        # WebView2控件扩展（Windows默认启用）
│   └── scintilla/       # DUI 原生 Scintilla 编辑控件扩展（--enable_scintilla=y）
├── examples/            # 示例程序
├── docs/                # 完整文档
├── bin/resources/       # 主题资源（XML布局、图片、字体）
├── xmake/               # xmake构建脚本
│   ├── options.lua      # 命令行选项（enable_* / with_* / render_backend 等）
│   ├── env.lua          # 公共路径与配置判定（描述域全局表 DUILIB）
│   ├── rules/           # 项目规则（duilib.config / features / app / skia / log）
│   ├── targets/         # 目标定义（third_party / duilib / render / image / extensions / examples / bench）
│   ├── tasks/           # xmake任务注册（format / format-check / attribute-gen / attribute-check）
│   ├── scripts/         # 任务脚本与属性名数据表（attribute_defs.lua 等）
│   ├── manifest/        # Windows清单文件
│   └── repos/           # 本地包仓库（Skia自动下载并编译）
└── build/               # 编译临时目录（build/build_temp，可清理）
```

## 开发模式（XML + C++）

### XML 布局文件
- 位置: `bin/resources/themes/default/<skin_folder>/<skin_file>.xml`
- 全局资源: `bin/resources/themes/default/global.xml`（字体、颜色、通用样式）
- 编码: UTF-8

### C++ 代码三件套
每个窗口通常需要三个文件:
1. **MainThread** - 继承 `ui::FrameworkThread`，负责初始化和创建窗口
2. **MainForm.h/cpp** - 继承 `ui::WindowImplBase`，实现窗口逻辑
3. **XML布局文件** - 描述界面结构和样式

### 关键代码模式

**初始化全局资源:**
```cpp
ui::FilePath resourcePath = ui::FilePathUtil::GetCurrentModuleDirectory();
resourcePath += _T("resources\\");
ui::GlobalManager::Instance().Startup(ui::LocalFilesResParam(resourcePath));
```

**创建窗口:**
```cpp
MainForm* window = new MainForm();
window->CreateWnd(nullptr, ui::WindowCreateParam(_T("WindowTitle"), true));
window->PostQuitMsgWhenClosed(true);
window->ShowWindow(ui::kSW_SHOW_NORMAL);
```

**查找控件:**
```cpp
ui::Button* btn = dynamic_cast<ui::Button*>(FindControl(_T("btn_name")));
```

**事件绑定:**
```cpp
btn->AttachClick([this](const ui::EventArgs& args) {
    // 处理点击事件
    return true;
});
```

## 文档参考
- 完整文档: `docs/Summary.md`（文档索引）
- 控件属性: `docs/Control.md`
- 容器/布局: `docs/Box.md`
- 全局资源: `docs/Global.md`
- 窗口属性: `docs/Window.md`
- 事件系统: `docs/Events.md`
- XML事件: `docs/XmlEvents.md`
- XML节点名: `docs/XmlNode.md`
- LLM详细参考: `.claude/docs/nim-duilib-llm-reference.md`

## 编码规范
- 字符串使用 `DString` 类型，字面量用 `_T("...")` 宏包裹
- 控件查找用 `FindControl(_T("name"))`，需 `dynamic_cast` 到具体类型
- 事件回调返回 `true` 表示已处理
- XML属性值中内嵌引号用单引号`'`或花括号`{}`代替双引号
- 控件类支持模板变体: `Label`(Control基)、`LabelBox`(Box基)、`LabelHBox`(HBox基)、`LabelVBox`(VBox基)
- 窗口析构由框架管理，使用 `new` 创建，不需要手动 `delete`
- **属性名统一登记表**：新增/修改 XML 属性只在 `xmake/scripts/attribute_defs.lua` 对应域列表**末尾**追加名字，然后运行 `xmake attribute-gen`；历史书写变体写进 `alias`（如 `{ name = "scroll_select", alias = { "scrollselect" } }`），别名在 `IdOf` 中归一到规范名，链上每个属性只需一个 `case`
- `src/duilib/Utils/AttributeIds.g.h`、`AttributeIds.g.cpp`、`CtrlDefs.g.h` 是生成文件，**禁止手改**；CI 用 `xmake attribute-check` 校验生成物同步与 XML 语料覆盖
- 属性派发一律用枚举：链上写 `switch (id)` + `case ui::attr::control::kXxx`，禁止再引入字符串比较；链上签名是 `SetAttributeById(attr::control::Id id, const DString &strValue)`，**不传名字**
- 代码内设置属性用 `SetAttributeById(ui::attr::control::kXxx, _T("xxx"), value)`，避免重复的字符串→枚举转换
- **控件类名域**：`ui::attr::ctrl` 覆盖全部控件类名（115 个，含宏名），`WindowBuilder::CreateControlByClass` 用 `switch (ui::attr::ctrl::IdOf(name))` 跳表派发；`DUI_CTR_*` 宏由 `xmake/scripts/attribute_defs.lua` 的 `ctrl` 域生成到 `CtrlDefs.g.h`（不再手写），类名比较不再直接用宏比较
- **脚本位置约定**：仓库根目录只保留 `xmake.lua` 入口；选项在 `xmake/options.lua`，公共配置在 `xmake/env.lua`，规则在 `xmake/rules/`，目标在 `xmake/targets/`，任务注册在 `xmake/tasks/register.lua`，任务脚本与数据表统一放 `xmake/scripts/`
- **源文件清单约定**：所有 `add_files` 逐个列出具体文件，**不使用通配符**（如 `*.cpp`）；新增/删除源文件时同步修改 `xmake/targets/` 下对应文件，避免构建结果随目录内容变化

## 构建
- 配置: `xmake f -o build/build_temp/xmake -c`（首次会自动下载并编译 Skia，默认用 MSVC，无需 LLVM）
- 编译: `xmake`（库 + 全部示例）；只编库用 `xmake f --enable_examples=n` 后再 `xmake`
- 运行: `xmake run basic`，或直接运行 `bin` 目录下的示例程序
- 可选: `--with_cef=latest`、`--enable_pag=y`、`--enable_jpeg_turbo=y`、`--enable_sdl=y`(Windows)、`--enable_scintilla=y`、`--with_skia_clang=y`、`--render_backend=gdi`、`--runtimes=MD`（xmake 内置，默认 MT）等
