# Scintilla 扩展设计说明

## 目标与边界

在 nim_duilib 中提供不需要原生子窗口的 Scintilla 编辑器控件，
并且不侵入核心库：

1. 控件继承 `ui::Box`，参与 Duilib 的布局、事件、焦点与绘制。
2. 不修改核心 `src/`、核心 `third_party/`、核心 target 与核心属性登记表。
3. Scintilla/Lexilla 只存在于 `extensions/scintilla/third_party`。
4. 通过 `GlobalManager::AddCreateControlCallback` 注册控件工厂。

## 与 Qt 平台层的对应关系

移植以 Scintilla 上游的 Qt 平台层为基准，逐方法翻译为 Duilib 能力：

| Qt 平台层 | DUI 实现 |
| :--- | :--- |
| `QPainter` / `QPaintDevice` | `ui::IRender` |
| `QFont` / `QFontMetricsF` | `ui::IFont` + `ui::ITextShaper::GetFontMetrics` |
| `QTextLayout` 的字形运行 | `ui::ITextShaper::ResolveGlyph` 逐字形度量与绘制 |
| `QAbstractScrollArea` / `QScrollBar` | `ui::Box` + 两个 `ui::ScrollBar` |
| `QClipboard` | `ui::Clipboard` |
| `QTimer` | `ui::TimerManager` |
| `QWidget` 弹窗 | `ui::Window`（`WindowImplBase` 派生，无焦点弹出窗口） |
| `QMenu` | `ui::Menu`（按 `AddToPopUp` 结果动态生成菜单 XML） |
| `QListWidget` | `ui::ListBox` 弹窗（`Scintilla::ListBox` 的实现） |
| `QInputMethodEvent` | Duilib 的 IME 事件 + `SetTextInputArea` 候选框定位 |
| `QDropEvent` | Duilib 的拖放事件 |
| `QElapsedTimer` | `std::chrono::steady_clock` |

## 层次

```text
duilib::ext::scintilla::ScintillaControl   (ui::Box)
    -> ScintillaDui            (ScintillaBase + 平台窗口句柄)
        -> ScintillaDuiSurface (Surface) -> ui::IRender / ui::ITextShaper
        -> ScintillaDuiPopup   (CallTip / 自动补全弹窗)
        -> ScintillaDuiPlatform(Window / ListBox / Menu / Platform)
```

## 平台窗口句柄

`Scintilla::Internal::Window` 的成员函数不是虚函数，平台层通过 `WindowID`
（`void*`）区分窗口。本移植定义 `ScintillaDuiWindowHandle` 接口：

- 编辑器窗口：`ScintillaDui`
- CallTip 窗口：`ScintillaDuiCallTip`
- 自动补全列表：`DuiListBox`

`ScintillaDuiPlatform.cpp` 中的 `Window::*` 全部按该接口分发。

注意：`wMain`（以及所有 `WindowID`）保存的是**基类子对象指针**
（`static_cast<ScintillaDuiWindowHandle*>`），多继承下带偏移，
反向转换必须使用 `dynamic_cast`。这里曾经是光标不显示、选区错乱的根源之一。

## Surface 实现

`ScintillaDuiSurface` 把 Scintilla 绘制调用映射到 Duilib：

- 线条、矩形、圆角、路径、图片映射到 `ui::IRender`；
- 文本按字形度量与绘制：`ResolveGlyph` 得到推进量，`DrawGlyph` 按基线绘制，
  保证 `MeasureWidthsUTF8` 与 `DrawText*` 使用完全一致的推进量；
- 当前样式字体缺少字形时，使用系统 CJK 字体作为回退（同字号/粗体/斜体），
  最后才使用占位字符；
- 无绘制目标的度量调用（Scintilla 的 `AutoSurface`）使用缓存的离屏
  Raster Render（`IRenderFactory::CreateRender(dpi, nullptr, kRaster)`）；
- 行缓存、选择边距、缩进参考线等 pixmap 通过 `AllocatePixMap` 创建离屏 Render，
  并用 `AlphaBlend` 回贴；
- 使用 `SetClip` / `ClearClip` 维护裁剪栈。

坐标映射：Scintilla 使用内容区的局部坐标，`ui::IRender` 使用窗口客户区坐标。
扩展**不使用** `IRender::OffsetWindowOrg` 来实现该平移，而是把内容区原点
（`ScintillaControl::m_rcTextArea` 的左上角）交给 `ScintillaDuiSurface`，
由 Surface 把偏移直接叠加到传给 Render/ITextShaper 的每个坐标上。
原因：核心库的 Skia 后端与 GDI 后端对 `OffsetWindowOrg` 的符号约定相反
（Skia 内部取反，GDI 直接累加），依赖它会得到错误的绘制原点，表现为
整体上移若干行、光标与文本行不对应、以及上一帧内容未被覆盖的残影。

DPI：`LogPixelsY()` 返回 `96 * 显示缩放百分比 / 100`，Scintilla 通过
`DeviceHeightFont()` 把磅值换算为设备像素，因此字体大小随 DPI 自动缩放，
`ChangeDpiScale` 只需使样式缓存失效。

## 输入、焦点与事件顺序

`ScintillaControl` 按 Qt 版本的事件顺序处理：

- 鼠标按下/双击 → `ButtonDownWithModifiers`，抬起 → `ButtonUpWithModifiers`，
  移动 → `ButtonMoveWithModifiers`，右键按下 → `RightButtonDownWithModifiers`；
  鼠标时间戳取自单调时钟（`steady_clock`），保证双击选词、三击选行、
  拖动选择与自动滚动判定正确。
- 按键 → `KeyTranslate`（移植自 `ScintillaWin::KeyTranslate`）→
  `KeyDownWithModifiers`；未被消费的按键交回 Duilib（如 Tab 焦点切换）。
- 字符输入 → `InsertCharacter`（Windows 使用 `kEventChar` 的 `wParam`，
  SDL 使用 `kEventTextInput`）。
- 滚轮 → Ctrl 缩放 / Shift 水平滚动 / 普通按行滚动。
- 焦点 → `SetFocusState(true/false)`（不仅是发送通知），插入符因此才会
  激活并闪烁；`OnSetFocus` 同时打开输入法并把候选框定位到插入符。
- 键盘焦点、鼠标捕获由 `ui::Window` 统一管理（与 Qt 平台层一致，
  Scintilla 只记录 `haveMouseCapture` 状态）。

## 滚动条与内容区

控件是 `ui::Box`，内部持有两个 `ui::ScrollBar`（主题类 `vscrollbar` /
`hscrollbar`），编辑内容区为去掉滚动条后的剩余区域：

- 垂直滚动条使用像素范围：`range = MaxScrollPos() * lineHeight`，
  `pos = topLine * lineHeight`；
- 水平滚动条：`range = scrollWidth - clientWidth`，`pos = xOffset`，
  自动换行时隐藏；
- 滚动条位置变化 → `ScrollTo` / `HorizontalScrollTo`；
- Scintilla 在绘制/通知过程中请求的滚动条更新会被**延迟**到当前消息
  处理结束后执行，避免在绘制中重新布局造成重入。

## 弹窗

- CallTip：`ScintillaDuiCallTip` 是无焦点弹出窗口，内部用自定义控件回调
  `CallTip::PaintCT` 绘制，窗口尺寸来自 `SetPositionRelative` 的矩形。
- 右键菜单：`AddToPopUp` 收集菜单项，按 `ui::Menu` 的主题类动态生成 XML
  （`MenuListBox` + `MenuItem`），弹出位置使用屏幕坐标。
- 自动补全：`Scintilla::ListBox` 由 `DuiListBox` 实现，使用
  `ui::ListBox` 弹窗显示候选，选中变化/点击通过 `IListBoxDelegate`
  回传给 Scintilla。

## 构建与回归

- 主工程：`xmake f --scintilla=y` → `xmake`，同时构建扩展与 `ScintillaDemo`；
  默认关闭时核心构建完全不变。
- 构建入口：扩展只通过仓库根目录的构建入口启用，与 `extensions/cef`、
  `extensions/webview2` 一致；扩展目录内不提供独立构建入口。
- 回归检查：核心 `xmake`、现有示例、`xmake attribute-check`、
  `xmake format-check` 不因扩展而变化；核心 `lib/` 不新增 Scintilla 相关库。

## 已知限制

- 无操作系统无障碍桥接（UIA / NSAccessibility / AT-SPI）。
- 不支持跨进程“拖出”（需要实现 OLE `IDropSource`/`IDataObject`）；
  控件内部拖动与外部“拖入”可用。
- 输入法使用系统候选框，不实现内联预编辑字符串与预编辑指示器。
- 复杂排版（Arabic/Indic 整形、连字、双向排版）受核心 `ui::ITextShaper` 限制。
