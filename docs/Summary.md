## 项目关联内容链接

* [项目简介](../README.md)
* [快速上手](Getting-Started.md)
* [示例程序](Examples.md)
* [全局样式：字体、颜色、图片等资源](Global.md)
* [窗口的样式](Window.md)
* [窗口阴影参考文档](WindowShadow.md)
* [容器的样式](Box.md)
* [控件的样式](Control.md)
* [菜单的样式](Menu.md)
* [控件的事件/消息](Events.md)
* [XML文件中各控件的节点名称](XmlNode.md)
* [XML文件中直接响应控件的事件](XmlEvents.md)
* [主题参考文档](Theme.md)
* [CEF控件](CEF.md)
* [编译方式（xmake）](../README.md#编译xmake)

## 项目中主要类的简表
各个类的详细接口说明请参考关联头文件，有较详细的注释。    
* 基本数据类型

| 类名称 | 关联头文件| 用途 |
| :--- | :--- | :--- |
| UiSize | [duilib/Core/UiSize.h](../src/duilib/Core/UiSize.h) | 32位Size类型的封装|
| UiSize64 | [duilib/Core/UiSize64.h](../src/duilib/Core/UiSize64.h) | 64位Size类型的封装|
| UiPoint | [duilib/Core/UiPoint.h](../src/duilib/Core/UiPoint.h) | 整型Point类型的封装|
| UiPointF | [duilib/Core/UiPointF.h](../src/duilib/Core/UiPointF.h) | 浮点型Point类型的封装|
| UiFixedInt | [duilib/Core/UiFixedInt.h](../src/duilib/Core/UiFixedInt.h) | 控件设置长度（或宽度）数值类型的封装|
| UiEstInt | [duilib/Core/UiEstInt.h](../src/duilib/Core/UiEstInt.h) | 控件估算长度（或宽度）数值类型的封装|
| UiFixedSize | [duilib/Core/UiTypes.h](../src/duilib/Core/UiTypes.h) | 设置的控件大小|
| UiEstSize | [duilib/Core/UiTypes.h](../src/duilib/Core/UiTypes.h) | 估算的控件大小（相比UiFixedSize，没有Auto类型）|
| UiEstResult | [duilib/Core/UiTypes.h](../src/duilib/Core/UiTypes.h) | 估算控件大小的结果|
| UiPadding | [duilib/Core/UiPadding.h](../src/duilib/Core/UiPadding.h) | 内边距类型的封装|
| UiMargin | [duilib/Core/UiMargin.h](../src/duilib/Core/UiMargin.h) | 外边距类型的封装|
| UiString | [duilib/Core/UiString.h](../src/duilib/Core/UiString.h) | 控件使用的字符串，用于替代std::wstring，以减少控件的内存占用，该类适合用于较低的内存空间来存储字符串，性能方面由于字符串复制偏多，性能偏弱|
| UiRect | [duilib/Core/UiRect.h](../src/duilib/Core/UiRect.h) | 32位Rect类型的封装|
| UiFont | [duilib/Core/UiFont.h](../src/duilib/Core/UiFont.h) | 字体类型的封装|
| UiColor | [duilib/Core/UiColor.h](../src/duilib/Core/UiColor.h) | 颜色类型的封装|
| UiColors | [duilib/Core/UiColors.h](../src/duilib/Core/UiColors.h) | 常见颜色值常量(ARGB格式)|

* 窗口相关

| 类名称/说明 | 关联头文件| 用途 |
| :--- | :--- | :--- |
| Window | [duilib/Core/Window.h](../src/duilib/Core/Window.h) | duilib核心窗口的封装|
| WindowBuilder | [duilib/Core/WindowBuilder.h](../src/duilib/Core/WindowBuilder.h) | 解析XML文件，并创建窗口、容器布局和控件等|
| Shadow | [duilib/Core/Shadow.h](../src/duilib/Core/Shadow.h) | 窗口阴影属性|
| WindowImplBase | [duilib/Utils/WinImplBase.h](../src/duilib/Utils/WinImplBase.h) | 提供给应用层的窗口基类，实现一个带有标题栏的窗体，带有最大化、最小化、还原按钮的支持，提供带有 WS_EX_LAYERED 属性窗口阴影|
| EventType | [duilib/duilib_defs.h](../src/duilib/duilib_defs.h) | 定义所有消息类型|
| EventArgs | [duilib/Core/EventArgs.h](../src/duilib/Core/EventArgs.h) | 窗口消息与控件事件通知的参数|
| WindowDropTarget | [duilib/Core/WindowDropTarget.h](../src/duilib/Core/WindowDropTarget.h) | 窗口中控件的拖放支持|
| Windows版本兼容性 | [duilib/duilib_config.h](../src/duilib/duilib_config.h) | Windows版本兼容性定义，默认配置兼容Win7以上操作系统|

* 控件与容器的基类

| 类名称 | 基类名称 |关联头文件| 用途 |
| :--- | :--- |:--- | :--- |
| PlaceHolder |nbase::SupportWeakCallback| [duilib/Core/PlaceHolder.h](../src/duilib/Core/PlaceHolder.h) | 所有控件和容器的顶层基类，封装了控件的位置、大小、外观形状等基本数据|
| Control | PlaceHolder | [duilib/Core/Control.h](../src/duilib/Core/Control.h) | 一个基本的控件，也是所有控件和容器的基类，包含了控件的位置、大小、状态、颜色、图片资源、动画、绘制、光标、鼠标、键盘、焦点、快捷键操作等基本功能|
| Box | Control| [duilib/Core/Box.h](../src/duilib/Core/Box.h) | 所有容器的基类，封装了容器的位置、大小、外观形状、子控件管理（添加、删除、修改、绘制、操作、拖放、布局等）|

* 布局

| 类名称 | 基类名称 |关联头文件| 用途 |
| :--- | :--- |:--- | :--- |
| Layout | |[duilib/Layout/Layout.h](../src/duilib/Layout/Layout.h) | 浮动布局|
| HLayout |Layout |[duilib/Layout/HLayout.h](../src/duilib/Layout/HLayout.h) | 水平布局|
| VLayout |Layout |[duilib/Layout/VLayout.h](../src/duilib/Layout/VLayout.h) | 垂直布局|
| HFlowLayout |Layout |[duilib/Layout/HFlowLayout.h](../src/duilib/Layout/HFlowLayout.h) | 水平流式布局|
| VFlowLayout |Layout |[duilib/Layout/VFlowLayout.h](../src/duilib/Layout/VFlowLayout.h) | 垂直流式布局|
| HTileLayout |Layout| [duilib/Layout/HTileLayout.h](../src/duilib/Layout/HTileLayout.h) | 水平瓦片布局|
| VTileLayout |Layout| [duilib/Layout/VTileLayout.h](../src/duilib/Layout/VTileLayout.h) | 垂直瓦片布局|
| VirtualHLayout |HLayout| [duilib/Layout/VirtualHLayout.h](../src/duilib/Layout/VirtualHLayout.h) | 虚表水平布局|
| VirtualVLayout |VLayout| [duilib/Layout/VirtualVLayout.h](../src/duilib/Layout/VirtualVLayout.h) | 虚表垂直布局|
| VirtualHTileLayout |HTileLayout| [duilib/Layout/VirtualHTileLayout.h](../src/duilib/Layout/VirtualHTileLayout.h) | 虚表水平瓦片布局|
| VirtualVTileLayout |VTileLayout| [duilib/Layout/VirtualVTileLayout.h](../src/duilib/Layout/VirtualVTileLayout.h) | 虚表垂直瓦片布局|
| GridLayout |Layout |[duilib/Layout/GridLayout.h](../src/duilib/Layout/GridLayout.h) | 网格布局|

* 容器

| 类名称 | 基类名称 |布局类型|关联头文件| 名称/用途 |
| :--- | :--- |:--- |:--- | :--- |
| Box | Control|[Layout](../src/duilib/Layout/Layout.h)|[duilib/Core/Box.h](../src/duilib/Core/Box.h) | 自由布局容器，所有容器的基类，封装了容器的位置、大小、外观形状、子控件管理（添加、删除、修改、绘制、操作、拖放、布局等）|
| VBox | Box|[VLayout](../src/duilib/Layout/VLayout.h)| [duilib/Box/VBox.h](../src/duilib/Box/VBox.h) | 垂直布局容器 |
| HBox | Box|[HLayout](../src/duilib/Layout/HLayout.h)|[duilib/Box/HBox.h](../src/duilib/Box/HBox.h) | 水平布局容器 |
| VFlowBox | Box|[VFlowLayout](../src/duilib/Layout/VFlowLayout.h)| [duilib/Box/VBox.h](../src/duilib/Box/VBox.h) | 垂直流式布局容器 |
| HFlowBox | Box|[HFlowLayout](../src/duilib/Layout/HFlowLayout.h)|[duilib/Box/HBox.h](../src/duilib/Box/HBox.h) | 水平流式布局容器 |
| TabBox | Box|[Layout](../src/duilib/Layout/Layout.h)|[duilib/Box/TabBox.h](../src/duilib/Box/TabBox.h) | 多标签布局容器 |
| VTileBox | Box|[VTileLayout](../src/duilib/Layout/VTileLayout.h)|[duilib/Box/TileBox.h](../src/duilib/Box/TileBox.h) | 垂直瓦片布局容器|
| HTileBox | Box|[HTileLayout](../src/duilib/Layout/HTileLayout.h)|[duilib/Box/TileBox.h](../src/duilib/Box/TileBox.h) | 水平瓦片布局容器|
| ScrollBox | Box|[Layout](../src/duilib/Layout/Layout.h)|[duilib/Box/ScrollBox.h](../src/duilib/Box/ScrollBox.h) | 自由布局可滚动容器，带有垂直或水平滚动条|
| VScrollBox | ScrollBox|[VLayout](../src/duilib/Layout/VLayout.h)|[duilib/Box/ScrollBox.h](../src/duilib/Box/ScrollBox.h) | 垂直布局可滚动容器，带有垂直或水平滚动条|
| HScrollBox | ScrollBox|[HLayout](../src/duilib/Layout/HLayout.h)|[duilib/Box/ScrollBox.h](../src/duilib/Box/ScrollBox.h) | 水平布局可滚动容器，带有垂直或水平滚动条|
| VTileScrollBox |ScrollBox|[VTileLayout](../src/duilib/Layout/VTileLayout.h)|[duilib/Box/ScrollBox.h](../src/duilib/Box/ScrollBox.h) | 垂直瓦片布局可滚动容器，带有垂直或水平滚动条|
| HTileScrollBox |ScrollBox|[HTileLayout](../src/duilib/Layout/HTileLayout.h)|[duilib/Box/ScrollBox.h](../src/duilib/Box/ScrollBox.h) | 水平瓦片布局可滚动容器，带有垂直或水平滚动条|
| ListBox | ScrollBox|[Layout](../src/duilib/Layout/Layout.h)|[duilib/Box/ListBox.h](../src/duilib/Box/ListBox.h) | 自由布局的列表容器|
| VListBox | ListBox|[VLayout](../src/duilib/Layout/VLayout.h)|[duilib/Box/ListBox.h](../src/duilib/Box/ListBox.h) | 垂直布局的列表容器|
| HListBox | ListBox|[HLayout](../src/duilib/Layout/HLayout.h)|[duilib/Box/ListBox.h](../src/duilib/Box/ListBox.h) | 水平布局的列表容器|
| VTileListBox | ListBox|[VTileLayout](../src/duilib/Layout/VTileLayout.h)|[duilib/Box/ListBox.h](../src/duilib/Box/ListBox.h) | 垂直瓦片布局的列表容器|
| HTileListBox | ListBox|[HTileLayout](../src/duilib/Layout/HTileLayout.h)|[duilib/Box/ListBox.h](../src/duilib/Box/ListBox.h) | 水平布局的列表容器|
| VirtualListBox | ListBox|[Layout](../src/duilib/Layout/Layout.h)|[duilib/Box/VirtualListBox.h](../src/duilib/Box/VirtualListBox.h) | 虚表实现的自由布局的列表容器|
| VirtualVListBox | VirtualListBox|[VirtualVLayout](../src/duilib/Layout/VirtualVLayout.h)|[duilib/Box/VirtualListBox.h](../src/duilib/Box/VirtualListBox.h) | 虚表实现的垂直布局的列表容器|
| VirtualHListBox | VirtualListBox|[VirtualHLayout](../src/duilib/Layout/VirtualHLayout.h)|[duilib/Box/VirtualListBox.h](../src/duilib/Box/VirtualListBox.h) | 虚表实现的水平布局的列表容器|
| VirtualVTileListBox | VirtualListBox|[VirtualVTileLayout](../src/duilib/Layout/VirtualVTileLayout.h)|[duilib/Box/VirtualListBox.h](../src/duilib/Box/VirtualListBox.h) | 虚表实现的垂直瓦片布局的列表容器|
| VirtualHTileListBox | VirtualListBox|[VirtualHTileLayout](../src/duilib/Layout/VirtualHTileLayout.h)|[duilib/Box/VirtualListBox.h](../src/duilib/Box/VirtualListBox.h) | 虚表实现的水平布局的列表容器|
| GridBox | Box|[GridLayout](../src/duilib/Layout/GridLayout.h)|[duilib/Box/GridBox.h](../src/duilib/Box/GridBox.h) | 网格布局容器 |
| GridScrollBox | ScrollBox|[GridLayout](../src/duilib/Layout/GridLayout.h)|[duilib/Box/GridBox.h](../src/duilib/Box/GridBox.h) | 网格布局容器（支持滚动条） |

* 图片

| 类名称/说明 | 关联头文件| 用途 |
| :--- | :--- | :--- |
| Image | [duilib/Image/Image.h](../src/duilib/Image/Image.h) | 图片相关封装，支持的文件格式：SVG/PNG/GIF/JPG/BMP/APNG/WEBP/ICO/Lottie-JSON/PAG|
| ImageAttribute | [duilib/Image/ImageAttribute.h](../src/duilib/Image/ImageAttribute.h) | 图片属性|
| ImageLoadParam | [duilib/Image/ImageLoadParam.h](../src/duilib/Image/ImageLoadParam.h) | 图片加载属性，用于加载一个图片|
| ImageInfo | [duilib/Image/ImageInfo.h](../src/duilib/Image/ImageInfo.h) | 图片信息|
| ImageDecoder | [duilib/Image/ImageDecoder.h](../src/duilib/Image/ImageDecoder.h) | 支持多线程解码的接口（底层解码使用，支持延迟解码，可以在多线程中解码，避免在UI线程解码图片导致卡顿）|
| ImagePlayer | [duilib/Image/ImagePlayer.h](../src/duilib/Image/ImagePlayer.h) | 控件图片动画播放的逻辑封装（支持GIF/WebP/APNG/Lottie-JSON/PAG动画）|
| StateImage | [duilib/Image/StateImage.h](../src/duilib/Image/StateImage.h) | 控件状态与图片的映射|
| StateImageMap | [duilib/Image/StateImageMap.h](../src/duilib/Image/StateImageMap.h) | 控件图片类型与状态图片的映射|
| ImageManager | [duilib/Core/ImageManager.h](../src/duilib/Core/ImageManager.h) | 图片资源管理器|
| IconManager | [duilib/Core/IconManager.h](../src/duilib/Core/IconManager.h) | 图标资源管理器（线程安全，适合图标类的小图片资源），支持Windows平台的HICON句柄|
| ImageList | [duilib/Core/ImageList.h](../src/duilib/Core/ImageList.h) | 图片列表|

* 动画

| 类名称/说明 | 关联头文件| 用途 |
| :--- | :--- | :--- |
| AnimationManager | [duilib/Animation/AnimationManager.h](../src/duilib/Animation/AnimationManager.h) | 图片动画管理器|
| AnimationPlayer | [duilib/Animation/AnimationPlayer.h](../src/duilib/Animation/AnimationPlayer.h) | 图片动画播放状态管理|

* 颜色

| 类名称/说明 | 关联头文件| 用途 |
| :--- | :--- | :--- |
| UiColor | [duilib/Core/UiColor.h](../src/duilib/Core/UiColor.h) | 颜色类型的封装|
| UiColors | [duilib/Core/UiColors.h](../src/duilib/Core/UiColors.h) | 常见颜色值常量(ARGB格式)|
| StateColorMap | [duilib/Core/StateColorMap.h](../src/duilib/Core/UiColors.h) | 控件状态与颜色值的映射|

* 字体

| 类名称/说明 | 关联头文件| 用途 |
| :--- | :--- | :--- |
| UiFont | [duilib/Core/UiFont.h](../src/duilib/Core/UiFont.h) | 字体类型的封装|
| FontManager | [duilib/Core/FontManager.h](../src/duilib/Core/FontManager.h) | 字体管理器|

* 渲染引擎接口

| 类名称/说明 | 关联头文件| 用途 |
| :--- | :--- | :--- |
| IRenderFactory | [render/IRender.h](../src/render/IRender.h) | 渲染工厂接口，用于创建Font、Pen、Brush、Path、Matrix、Bitmap、Render等渲染实现对象 |
| IFont | [render/IRender.h](../src/render/IRender.h) | 字体接口 |
| IBitmap | [render/IRender.h](../src/render/IRender.h) | 位图接口 |
| IPen | [render/IRender.h](../src/render/IRender.h) | 画笔接口 |
| IBrush | [render/IRender.h](../src/render/IRender.h) | 画刷接口 |
| IPath | [render/IRender.h](../src/render/IRender.h) | 路径接口 |
| IMatrix | [render/IRender.h](../src/render/IRender.h) | 矩阵接口 |
| IRender | [render/IRender.h](../src/render/IRender.h) | 渲染接口，用于画图、绘制文字等 |

* 渲染后端与静态库拆分

渲染后端、文本布局和图片解码模块的拆分方式、构建选项及运行期切换方式，参见 [RenderBackend.md](RenderBackend.md)。

* Skia渲染引擎

| 类名称/说明 | 关联头文件| 用途 |
| :--- | :--- | :--- |
| RenderFactory_Skia | [render-skia/RenderFactory_Skia.h](../src/render-skia/RenderFactory_Skia.h) | 渲染工厂接口的实现 |
| Font_Skia | [render-skia/Font_Skia.h](../src/render-skia/Font_Skia.h) | 字体接口的实现 |
| Bitmap_Skia | [render-skia/Bitmap_Skia.h](../src/render-skia/Bitmap_Skia.h) | 位图接口的实现 |
| Pen_Skia | [render-skia/Pen_Skia.h](../src/render-skia/Pen_Skia.h) | 画笔接口的实现 |
| Brush_Skia | [render-skia/Brush_Skia.h](../src/render-skia/Brush_Skia.h) | 画刷接口的实现 |
| Path_Skia | [render-skia/Path_Skia.h](../src/render-skia/Path_Skia.h) | 路径接口的实现 |
| Matrix_Skia | [render-skia/Matrix_Skia.h](../src/render-skia/Matrix_Skia.h) | 矩阵接口的实现 |
| FontMgr_Skia | [render-skia/FontMgr_Skia.h](../src/render-skia/FontMgr_Skia.h) | 字体管理器的接口实现 |
| Render_Skia | [render-skia/Render_Skia.h](../src/render-skia/Render_Skia.h) | 渲染接口的实现，用于画图、绘制文字等 |
| Render_Skia_Windows | [render-skia/Render_Skia_Windows.h](../src/render-skia/Render_Skia_Windows.h) | 渲染接口的Windows相关功能的实现 |
| Render_Skia_SDL | [render-skia/Render_Skia_SDL.h](../src/render-skia/Render_Skia_SDL.h) | 渲染接口的SDL相关功能的实现，主要用于Linux，Windows也支持 |

* 控件/功能组件

| 类名称/功能组件 | 基类 | 关联头文件| 用途 |
| :--- | :--- | :---| :--- |
| ScrollBar | Control| [duilib/Core/ScrollBar.h](../src/duilib/Core/ScrollBar.h) | 滚动条控件 |
| Label | Control| [duilib/Control/Label.h](../src/duilib/Control/Label.h) | 标签控件（模板），用于显示文本 |
| LabelBox | Box| [duilib/Control/Label.h](../src/duilib/Control/Label.h) | 标签容器（模板），用于显示文本 |
| Button | Control| [duilib/Control/Button.h](../src/duilib/Control/Button.h) | 按钮控件（模板实现） |
| ButtonBox | Box| [duilib/Control/Button.h](../src/duilib/Control/Button.h) | 按钮容器控件（模板实现） |
| CheckBox | Control| [duilib/Control/CheckBox.h](../src/duilib/Control/CheckBox.h) | 复选框控件（模板实现） |
| CheckBoxBox | Box| [duilib/Control/CheckBox.h](../src/duilib/Control/CheckBox.h) | 复选框容器（模板实现） |
| Option | Control| [duilib/Control/Option.h](../src/duilib/Control/Option.h) | 单选按钮控件|
| OptionBox | Box| [duilib/Control/Option.h](../src/duilib/Control/Option.h) | 单选按钮容器|
| GroupBox | Box| [duilib/Control/GroupBox.h](../src/duilib/Control/GroupBox.h) | 分组容器（模板） |
| GroupVBox | VBox| [duilib/Control/GroupBox.h](../src/duilib/Control/GroupBox.h) | 垂直分组容器（模板） |
| GroupHBox | HBox| [duilib/Control/GroupBox.h](../src/duilib/Control/GroupBox.h) | 水平分组容器（模板） |
| Combo | Box| [duilib/Control/Combo.h](../src/duilib/Control/Combo.h) | 组合框 |
| ComboButton | Box| [duilib/Control/ComboButton.h](../src/duilib/Control/ComboButton.h) | 带有下拉组合框的按钮 |
| CheckCombo | Control| [duilib/Control/CheckCombo.h](../src/duilib/Control/CheckCombo.h) | 带复选框的组合框 |
| FilterCombo | Combo| [duilib/Control/FilterCombo.h](../src/duilib/Control/FilterCombo.h) | 带有过滤功能的组合框 |
| DateTime | Label| [duilib/Control/DateTime.h](../src/duilib/Control/DateTime.h) | 日期时间选择控件 |
| HotKey | HBox| [duilib/Control/HotKey.h](../src/duilib/Control/HotKey.h) | 热键控件 |
| HyperLink | Label| [duilib/Control/HyperLink.h](../src/duilib/Control/HyperLink.h) | 带有超级链接的文字, 如果URL为空的话，可以当作普通的文字按钮使用 |
| IPAddress | HBox| [duilib/Control/IPAddress.h](../src/duilib/Control/IPAddress.h) | IP地址控件 |
| Line | Control| [duilib/Control/Line.h](../src/duilib/Control/Line.h) | 画线控件 |
| Menu | WindowImplBase| [duilib/Control/Menu.h](../src/duilib/Control/Menu.h) | 菜单，独立窗口 |
| Progress | Label| [duilib/Control/Progress.h](../src/duilib/Control/Progress.h) | 进度条控件 |
| Slider | Progress| [duilib/Control/Slider.h](../src/duilib/Control/Slider.h) | 滑块控件 |
| CircleProgress | Control| [duilib/Control/CircleProgress.h](../src/duilib/Control/CircleProgress.h) | 环形进度条 |
| RichEdit | ScrollBox| [duilib/Control/RichEdit.h](../src/duilib/Control/RichEdit.h) | 富文本编辑框控件 |
| RichEdit实现类 | | [duilib/Control/RichEditCtrl_Windows.h](../src/duilib/Control/RichEditCtrl_Windows.h) | 富文本编辑框的主要功能封装（Windows） |
| RichEdit实现类 | | [duilib/Control/RichEditHost_Windows.h](../src/duilib/Control/RichEditHost_Windows.h) | 富文本编辑框的主要功能实现（Windows） |
| RichEdit实现类 | | [duilib/Control/RichEdit_SDL.h](../src/duilib/Control/RichEdit_SDL.h) | 富文本编辑框的主要功能封装（SDL） |
| RichText | Control| [duilib/Control/RichText.h](../src/duilib/Control/RichText.h) | 格式化文本（类HTML格式） |
| Split | Control| [duilib/Control/Split.h](../src/duilib/Control/Split.h) | 分割条控件 |
| SplitBox | Box| [duilib/Control/Split.h](../src/duilib/Control/Split.h) | 分割条容器 |
| TabCtrl | ListBox| [duilib/Control/TabCtrl.h](../src/duilib/Control/TabCtrl.h) | 多标签控件（类似浏览器的多标签） |
| TreeView | ListBox| [duilib/Control/TreeView.h](../src/duilib/Control/TreeView.h) | 树控件 |
| TreeNode | ListBoxItem| [duilib/Control/TreeView.h](../src/duilib/Control/TreeView.h) | 树控件的节点 |
| DirectoryTree | TreeView| [duilib/Control/DirectoryTree.h](../src/duilib/Control/DirectoryTree.h) | 目录树控件，用于显示文件系统的目录结构 |
| ListCtrl | VBox| [duilib/Control/ListCtrl.h](../src/duilib/Control/ListCtrl.h) | 列表控件 |
| ListCtrl实现类 | | [duilib/Control/ListCtrlDefs.h](../src/duilib/Control/ListCtrlDefs.h) | 列表控件的基本类型定义 |
| ListCtrl实现类 | | [duilib/Control/ListCtrlHeader.h](../src/duilib/Control/ListCtrlHeader.h) | 列表控件的表头 |
| ListCtrl实现类 | | [duilib/Control/ListCtrlHeaderItem.h](../src/duilib/Control/ListCtrlHeaderItem.h) | 列表控件的表头子项 |
| ListCtrl实现类 | | [duilib/Control/ListCtrlItem.h](../src/duilib/Control/ListCtrlItem.h) | 列表控件数据项 |
| ListCtrl实现类 | | [duilib/Control/ListCtrlSubItem.h](../src/duilib/Control/ListCtrlSubItem.h) | 列表控件数据项的子项 |
| ListCtrl实现类 | | [duilib/Control/ListCtrlView.h](../src/duilib/Control/ListCtrlView.h) | 列表控件视图基类 |
| ListCtrl实现类 | | [duilib/Control/ListCtrlReportView.h](../src/duilib/Control/ListCtrlReportView.h) | 列表控件Report视图 |
| ListCtrl实现类 | | [duilib/Control/ListCtrlIconView.h](../src/duilib/Control/ListCtrlIconView.h) | 列表控件Icon/List视图 |
| ListCtrl实现类 | | [duilib/Control/ListCtrlData.h](../src/duilib/Control/ListCtrlData.h) | 列表控件的数据管理器 |
| PropertyGrid | VBox| [duilib/Control/PropertyGrid.h](../src/duilib/Control/PropertyGrid.h) | 属性表控件，支持文本、数字、复选框、字体、颜色、日期、IP地址、热键、文件路径、文件夹等属性 |
| ColorPicker | WindowImplBase| [duilib/Control/ColorPicker.h](../src/duilib/Control/ColorPicker.h) | 拾色器，独立窗口 |
| 拾色器实现类 | | [duilib/Control/ColorControl.h](../src/duilib/Control/ColorControl.h) | ColorPicker的实现类，自定义颜色控件 |
| 拾色器实现类 | | [duilib/Control/ColorConvert.h](../src/duilib/Control/ColorConvert.h) | ColorPicker的实现类，颜色类型（RGB/HSV/HSL）转换类 |
| 拾色器实现类 | | [duilib/Control/ColorPickerCustom.h](../src/duilib/Control/ColorPickerCustom.h) | ColorPicker的实现类，自定义颜色 |
| 拾色器实现类 | | [duilib/Control/ColorPickerRegular.h](../src/duilib/Control/ColorPickerRegular.h) | ColorPicker的实现类，常用颜色 |
| 拾色器实现类 | | [duilib/Control/ColorPickerStandard.h](../src/duilib/Control/ColorPickerStandard.h) | ColorPicker的实现类，标准颜色 |
| 拾色器实现类 | | [duilib/Control/ColorPickerStandardGray.h](../src/duilib/Control/ColorPickerStandardGray.h) | ColorPicker的实现类，标准颜色，灰色 |
| 拾色器实现类 | | [duilib/Control/ColorSlider.h](../src/duilib/Control/ColorSlider.h) | ColorPicker的实现类 |
| ControlDragable | Control | [duilib/Control/ControlDragable.h](../src/duilib/Control/ControlDragable.h) | 支持相同Box内的子控件通过拖动来调整顺序 |
| BoxDragable | Box | [duilib/Control/ControlDragable.h](../src/duilib/Control/ControlDragable.h) | 支持相同Box内的子控件通过拖动来调整顺序 |
| HBoxDragable | HBox | [duilib/Control/ControlDragable.h](../src/duilib/Control/ControlDragable.h) | 支持相同Box内的子控件通过拖动来调整顺序 |
| VBoxDragable | VBoxDragable | [duilib/Control/ControlDragable.h](../src/duilib/Control/ControlDragable.h) | 支持相同Box内的子控件通过拖动来调整顺序 |
| IconControl | Control | [duilib/Control/IconControl.h](../src/duilib/Control/IconControl.h) | 用于显示图标的控件，如果不设置图标数据，则兼容基类Control的所有功能|
| AddressBar | HBox | [duilib/Control/AddressBar.h](../src/duilib/Control/AddressBar.h) | 地址栏控件，用于显示本地文件系统的路径 |

* 全局资源

| 类名称 | 关联头文件| 用途 |
| :--- | :--- | :--- |
| GlobalManager | [duilib/Core/GlobalManager.h](../src/duilib/Core/GlobalManager.h) | 全局属性管理工具类，用于管理一些全局属性的工具类，包含全局样式（global.xml）和语言设置等 |
| IRenderFactory | [render/IRender.h](../src/render/IRender.h) | 渲染接口的管理类，渲染接口管理，用于创建Font、Pen、Brush、Path、Matrix、Bitmap、Render等渲染实现对象 |
| FontManager | [duilib/Core/FontManager.h](../src/duilib/Core/FontManager.h) | 字体的管理类 |
| ImageManager | [duilib/Core/ImageManager.h](../src/duilib/Core/ImageManager.h) | 图片的管理类 |
| IconManager | [duilib/Core/IconManager.h](../src/duilib/Core/IconManager.h) | 图标资源管理器（线程安全，适合图标类的小图片资源），支持Windows平台的HICON句柄 |
| ZipManager | [duilib/Core/ZipManager.h](../src/duilib/Core/ZipManager.h) | ZIP压缩包管理器 |
| DpiManager | [duilib/Core/DpiManager.h](../src/duilib/Core/DpiManager.h) | DPI管理器，用于支持DPI自适应等功能 |
| TimerManager | [duilib/Core/TimerManager.h](../src/duilib/Core/TimerManager.h) | 定时器管理器 |
| LangManager | [duilib/Core/LangManager.h](../src/duilib/Core/LangManager.h) | 多语言支持管理器 |
| CursorManager | [duilib/Core/CursorManager.h](../src/duilib/Core/CursorManager.h) | 光标管理器 |
| ThreadManager | [duilib/Core/ThreadManager.h](../src/duilib/Core/ThreadManager.h) | 线程管理器 |
| ColorManager | [duilib/Core/ColorManager.h](../src/duilib/Core/ColorManager.h) | 颜色管理器 |
| ThemeManager | [duilib/Core/ThemeManager.h](../src/duilib/Core/ThemeManager.h) | 主题管理器 |
| WindowManager | [duilib/Core/WindowManager.h](../src/duilib/Core/WindowManager.h) | 窗口管理器 |
| ImageDecoderFactory | [duilib/Image/ImageDecoderFactory.h](../src/duilib/Image/ImageDecoderFactory.h) | 图片解码器的管理 |

* libcef控件封装相关

| 类名称 | 关联头文件| 用途 |
| :--- | :--- | :--- |
| CefManager | [cef/CefManager.h](../src/cef/CefManager.h) | CEF控件管理器，负责CEF模块的初始化和反初始化相关工作 |
| CefControl | [cef/CefControl.h](../src/cef/CefControl.h) | CEF控件接口，提供网页浏览相关的基本功能及事件的接受等功能 |
| CefControlEvent | [cef/CefControlEvent.h](../src/cef/CefControlEvent.h) | CEF控件的网页浏览相关事件接收接口 |
| CefControlNative | [cef/CefControlNative.h](../src/cef/CefControlNative.h) | CEF控件窗口模式的封装 |
| CefControlOffScreen | [cef/CefControlOffScreen.h](../src/cef/CefControlOffScreen.h) | CEF控件离屏渲染模式的封装 |
