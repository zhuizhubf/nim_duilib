# duilib-scintilla

`duilib-scintilla` 是 nim_duilib 的 DUI 原生 Scintilla 编辑器扩展。

## 特性

- 不使用 Win32 子窗口、Cocoa 子视图或 GTK 控件；编辑器本身就是
  `ui::Box` 的子类，完全参与 Duilib 的布局、焦点、事件与绘制。
- 平台层以 Scintilla 上游的 Qt 移植层（`PlatQt.cpp`、`ScintillaQt.cpp`、
  `ScintillaEditBase.cpp`）为基准逐方法移植，事件顺序、焦点状态、
  滚动同步与 Qt 版本保持一致（不引入 Qt 依赖）。
- 文本度量与绘制使用 Duilib 的 `ui::ITextShaper`，按字形推进量绘制，
  光标、选区、语法高亮背景与文本像素严格对齐；中文等非等宽字体字符
  通过系统 CJK 字体自动回退。
- 内置 Scintilla 5.6.6 与 Lexilla 5.5.3 源码，支持语法高亮、行号、
  折叠边距、自动换行、多选区、矩形选择、撤销重做、剪贴板、
  自动补全列表、CallTip 与右键菜单。
- 核心 nim_duilib 库零改动：核心 `src/`、核心 `third_party/`、核心
  target 与核心属性登记表均未修改，扩展通过
  `GlobalManager::AddCreateControlCallback` 注册控件工厂。

## 目录结构

```text
extensions/scintilla/
├─ xmake.lua                 # 扩展独立构建入口（可选，用于单独构建扩展）
├─ src/                      # 移植层源码 + 对外头文件（同目录，无独立 include 目录）
│   ├─ duilib_scintilla.h    # 对外聚合头
│   ├─ ScintillaControl.h    # DUI 控件
│   ├─ ScintillaManager.h    # 控件工厂注册
│   ├─ ScintillaDui.*        # ScintillaBase 的平台移植（对应 ScintillaQt）
│   ├─ ScintillaDuiSurface.* # Surface 的 DUI 实现（对应 PlatQt 的 SurfaceImpl）
│   ├─ ScintillaDuiTextShaper.*  # GDI/Skia 文本塑形的唯一适配点
│   ├─ ScintillaDuiPopup.*   # CallTip 与自动补全列表的 DUI 弹窗
│   └─ ScintillaDuiPlatform.*# Window/ListBox/Menu/Platform 平台实现
├─ third_party/              # Scintilla 与 Lexilla 源码
├─ docs/Scintilla.md         # 设计说明与已知限制
└─ licenses/                 # 许可文件
```

示例位于主工程的示例目录：`examples/ScintillaDemo`，
皮肤资源位于 `bin/resources/themes/default/scintilla/scintilla.xml`。

## 构建

### 方式一：与主工程一起构建（推荐）

在 nim_duilib 根目录启用 `--scintilla=y`：

```powershell
cd E:\LS\nim_duilib
xmake f -c -m release --scintilla=y
xmake                       # 构建核心库 + duilib-scintilla + ScintillaDemo
```

默认（不传 `--scintilla`）不会构建任何扩展目标，核心构建保持不变。

### 方式二：扩展目录独立构建

先构建核心库，再进入扩展目录：

```powershell
cd E:\LS\nim_duilib
xmake f -c -m release
xmake

cd E:\LS\nim_duilib\extensions\scintilla
xmake f -c --duilib_dir=E:\LS\nim_duilib --duilib_render=skia --duilib_runtime=MT
xmake
```

也可以不切换目录，在仓库根目录用 `-P` 指定扩展工程：

```powershell
xmake f -P extensions/scintilla -c --duilib_dir=E:\LS\nim_duilib --duilib_render=skia --duilib_runtime=MT
xmake build -P extensions/scintilla
```

独立构建参数：

| 参数 | 取值 | 说明 |
| :--- | :--- | :--- |
| `--duilib_dir` | 路径 | 核心仓库根目录（默认 `../..`） |
| `--duilib_render` | `skia` / `gdi` / `both` | 必须与核心库的渲染后端一致 |
| `--duilib_runtime` | `MT` / `MD` | 必须与核心库的运行库一致 |
| `--duilib_sdl` | `y` / `n` | Windows 下核心库是否使用 SDL3 |
| `--examples` | `y` / `n` | 是否构建 `ScintillaDemo`（默认 y） |

独立构建时产物输出到 `extensions/scintilla/lib` 与 `extensions/scintilla/bin`，
不写入核心的 `lib/`、`bin/`。

## 应用接入

1. 应用额外链接 `duilib-scintilla` 静态库（使用主工程构建时，
   `--scintilla=y` 会增加该 target，示例会自动依赖它）。
2. 在加载包含 `ScintillaControl` 的 XML 之前注册控件工厂：

```cpp
#include "duilib_scintilla.h"

duilib::ext::scintilla::ScintillaManager::Instance().Initialize();
```

3. 在窗口 XML 中使用控件（类型名为 `ScintillaControl`）：

```xml
<ScintillaControl name="editor" lexer="cpp" line_numbers="true"
                  folding="true" tab_width="4" use_tabs="false"
                  word_wrap="false" width="stretch" height="stretch"/>
```

4. 通过 C++ 接口设置样式与文本：

```cpp
using duilib::ext::scintilla::ScintillaControl;

auto* pEditor = dynamic_cast<ScintillaControl*>(FindControl(_T("editor")));
ui::UiFont font;
font.m_fontName = _T("Consolas");
font.m_fontSize = pEditor->Dpi().GetScaleInt(14);
pEditor->SetDefaultStyle(ui::UiColor(0xFF, 0x1E, 0x1E, 0x1E),
                         ui::UiColor(0xFF, 0xFF, 0xFF, 0xFF),
                         font);
pEditor->SetLexer("cpp");
pEditor->SetTextUTF8("int main() { return 0; }");
```

XML 支持的扩展属性：`lexer`、`text`、`readonly`、`word_wrap`、`tab_width`、
`use_tabs`、`line_numbers`、`folding`、`zoom`。

## 公开接口

`duilib::ext::scintilla::ScintillaControl`：

- 文本：`GetTextUTF8` / `SetTextUTF8` / `GetText` / `SetText`
- 词法：`SetLexer` / `GetLexer`
- 排版：`SetTabWidth`、`SetUseTabs`、`SetWordWrap`、`SetLineNumbersVisible`、
  `SetFoldingVisible`、`SetZoom`
- 样式：`SetDefaultStyle`、`SetStyle`
- 编辑：`Undo`、`Redo`、`CanUndo`、`CanRedo`、`Cut`、`Copy`、`Paste`、
  `SelectAll`、`IsModified`、`SetSavePoint`
- 补全：`ShowAutoComplete`
- 完整 Scintilla 接口：`SendSciMessage(SCI_*, wParam, lParam)`
- 通知：`SetTextChangedCallback`、`SetSavePointCallback`、`SetNotifyCallback`

## 已知限制

- 不实现操作系统无障碍桥接（Windows UIA、macOS NSAccessibility、Linux AT-SPI）。
- 不实现跨进程/跨控件的“拖出”拖动（OLE `IDropSource`）；控件内部的文本
  拖动、以及从外部“拖入”文本仍可正常工作。
- 输入法使用系统候选框（`SetTextInputArea` 定位），不实现 Qt 那种
  内联 IME 组合字符串与预编辑指示器。
- 复杂排版（Arabic/Indic 整形、连字、双向排版）受核心 `ui::ITextShaper`
  能力限制。

详细设计说明见 [docs/Scintilla.md](docs/Scintilla.md)。
