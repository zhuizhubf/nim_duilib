# nim_duilib

[nim_duilib](https://github.com/rhett-lee/nim_duilib) 是一款基于C++开发的跨平台界面库，源于经典的 duilib 界面库并进行了深度优化与功能扩展，支持Windows/Linux/macOS/FreeBSD平台，支持的Linux系统包括OpenEuler、OpenKylin、UbuntuKylin、统信UOS、中科方德、Ubuntu、Fedora、Debian等，专注于简化桌面应用的高效开发。其设计融合了DirectUI理念，通过XML描述界面布局，实现视觉与逻辑的分离，显著提升开发灵活性与维护性。

![GitHub](https://img.shields.io/badge/license-MIT-green.svg)

[![CI](https://github.com/rhett-lee/nim_duilib/actions/workflows/ci.yml/badge.svg)](https://github.com/rhett-lee/nim_duilib/actions/workflows/ci.yml)

## 核心技术特性

 - XML布局驱动：采用 XML 文件定义界面结构，将界面布局与业务逻辑完全分离。开发者可通过修改 XML 快速调整控件位置、尺寸和样式，无需改动 C++ 核心代码，极大提升开发与迭代效率。
 - 丰富的控件体系：内置按钮、文本框、列表视图、虚表控件、树形控件、滑块、进度条、菜单、颜色选择、属性页、标签页等基础控件，同时支持自定义控件开发，满足多样化界面设计需求。
 - 高效的渲染机制：使用Skia作为渲染引擎，实现控件的无窗口绘制，减少系统资源占用，提升界面刷新速度。支持硬件加速渲染（后端绘制支持使用CPU绘制或者GPU绘制），确保复杂界面流畅运行。
 - 事件驱动：基于消息机制的事件处理，使得UI交互逻辑清晰，支持在XML文件中配置事件响应代码。
 - 多种图片格式：支持SVG/PNG/GIF/JPG/BMP/APNG/WEBP/ICO图片格式。
 - 支持动画格式：支持GIF、APNG、WEBP、Lottie JSON、PAG动画文件格式。
 - 多语言与国际化：支持动态多种语言切换，便于开发全球化的应用程序。
 - 支持动态换肤：通过XML文件定义皮肤结构，可以轻松改变界面风格，支持动态换肤。
 - 支持窗口阴影：支持窗口的圆角阴影、直角阴影，并可选择阴影大小，可实时更新；在Windows和macOS系统中，支持使用系统阴影。
 - 支持DPI感知：有Unaware、SystemAware、PerMonitorAware、PerMonitorAware_V2四种模式，支持独立设置DPI，支持高清DPI的适配（仅限Windows平台）。
 - 支持CEF控件：支持libcef 109 版本，以兼容Windows 7系统；支持libcef 142 版本，支持Windows 10及以上操作系统，支持Linux和MacOS平台。
 - 支持WebView2控件：支持使用WebView2控件用于显示网页，其接口封装简单，更易于使用（仅支持Windows平台）。
 - 支持SDL3：可使用SDL3作为窗口管理和输入输出等基本功能提供者，从而支持跨平台（目前已经适配了Windows/Linux/MacOS/FreeBSD平台）。
 - 支持主题切换，默认支持浅色主题和深色主题，支持设置强调色，并提供设计主题的辅助工具。

## 目录结构
| 目录          | 说明 |
| :---          | :--- |
| src           | 所有库的源代码：核心库 `src/duilib`，以及 `src/render`、`src/render-skia`、`src/render-gdi`、`src/text`、`src/cef`、`src/webview2` 等模块|
| docs          | 项目的说明文档，包括各个控件的功能介绍与属性列表说明文档|
| bin           | 各个示例程序输出目录，包含预设的皮肤和语言文件以及 CEF 依赖|
| licenses      | 引用的其他开源代码对应的licenses文件|
| xmake         | xmake编译脚本（第三方库、duilib主库、示例程序以及Skia的本地包定义）|
| build         | 编译的临时目录（build/build_temp，可清理）|
| examples      | 项目的示例程序源代码，涵盖所有控件的基本用法示例（示例程序，详见 [docs/Examples.md](docs/Examples.md)）|
| third_party   | 项目代码依赖的第三方库（`prebuilt/` 下为预编译 SDK），详细内容见后续文档|

## 基于NIM_Duilib_Framework源码做的主要修改
<table>
    <tr>
        <th>分类</th>
        <th>修改内容</th>
    </tr>
    <tr>
        <td rowspan="12">整体改进</td>
        <td align="left">1. 调整了代码的组织结构，按照功能模块划分，大文件按类拆分为多个小文件，有利于理解整个库的体系结构</td>
    </tr>
    <tr><td align="left">2. 梳理了代码的接口文件，补充各个接口的注释和功能注释，有利于阅读和理解代码</td></tr>
    <tr><td align="left">3. 对配置XML文件进行优化，调整了属性命名规则，控件的宽度和高度可以按百分比设置，图片属性增加了部分扩展，优化了图片加载流程</td></tr>
    <tr><td align="left">4. 扩展了图片资源的支持：新增加APNG/WEBP动画/Lottie JSON动画/PAG动画，新增对ICO的支持，并优化了图片加载引擎及代码实现逻辑</td></tr>
    <tr><td align="left">5. 重新实现各个布局的代码，并新增了常见的界面布局方案，总体归类为：浮动布局、水平布局、垂直布局、水平流式布局、垂直流式布局、网格布局、水平瓦片布局、垂直瓦片布局、虚表水平布局、虚表垂直布局、虚表水平瓦片布局、虚表垂直瓦片布局，使布局的概念更易理解，易于扩展，详情可参考文档(docs/Box.md)</td></tr>
    <tr><td align="left">6. XML文件解析引擎替换为pugixml parser，性能更优</td></tr>
    <tr><td align="left">7. 移除对base库的依赖，消息循环和线程通信相关功能改为自己实现</td></tr>
    <tr><td align="left">8. 集成了Skia引擎，并作为默认的渲染引擎</td></tr>
    <tr><td align="left">9. 支持SDL3，支持跨平台（已经适配了Windows平台、Linux平台、MacOS平台、FreeBSD平台）</td></tr>
    <tr><td align="left">10. CEF组件放到duilib工程，并对CEF的版本进行了升级（支持libcef 109 版本，以兼容Win7系统；支持libcef 142 版本，支持Win10及以上操作系统）</td></tr>
    <tr><td align="left">11. 重新设计图片管理的接口和加载流程（Image目录），支持多线程加载图片，以更好的扩展其他图片格式支持</td></tr>
    <tr><td align="left">12. 重新设计全局资源管理器的XML文件结构（global.xml），改进字体管理功能，支持字体回退功能，从而支持Emoji字体显示；支持浅色主题和深色主题，支持强调色</td></tr>
    <tr>
        <td rowspan="22">功能完善</td>
        <td align="left">1. 对窗口类（Window）增加了新的属性：的功能进行了完善，提高对DPI自适应、窗口消息的代码容错，代码结构做了调整</td>
    </tr>
    <tr><td align="left">2. 对窗口类（Window）增加了新的属性：use_system_caption，snap_layout_menu，sys_menu，sys_menu_rect, icon属性，提供使用操作系统默认标题栏的选项，自绘标题栏的功能与系统标题栏的功能相似</td></tr>
    <tr><td align="left">3. 对窗口（Window）引入WindowDropTarget辅助类，提供对基于窗口的拖放功能的支持</td></tr>
    <tr><td align="left">4. 对资源管理相关的部分做了梳理，使得字体、颜色、图片等资源管理更容易理解</td></tr>
    <tr><td align="left">5. 对ListBox控件进行了优化：细分为ListBox、VListBox、HListBox、VTileListBox、HTileListBox、VirtualListBox、VirtualVListBox、VirtualHListBox、VirtualVTileListBox、VirtualHTileListBox几种，功能更加完善，基于虚表的ListBox功能可用性提升</td></tr>
    <tr><td align="left">6. 对组合框（Combo）关联的控件（CheckCombo、FilterCombo）进行了优化，提升可用性</td></tr>
    <tr><td align="left">7. 对日期时间（DateTime）控件的编辑功能进行了完善</td></tr>
    <tr><td align="left">8. 对菜单（CMenuWnd）类的功能代码进行优化，按新的结构重新实现菜单，使得菜单内的控件完全兼容现有容器/控件体系，易于理解和维护</td></tr>
    <tr><td align="left">9. 对文本编辑控件（RichEdit）进行功能优化和扩展，丰富了较多常用功能</td></tr>
    <tr><td align="left">10. 对树控件（TreeView）进行功能优化和扩展，丰富了较多常用功能，提升可用性</td></tr>
    <tr><td align="left">11. 全局资源（GlobalManager）的接口进行了优化，使得所有资源全部通过此接口管理，易于理解和维护</td></tr>
    <tr><td align="left">12. 同一个窗口内的不同容器之间，容器内的控件可以通过属性设置支持拖出和拖入操作</td></tr>
    <tr><td align="left">13. 控件的背景色支持渐变色，新增前景色功能</td></tr>
    <tr><td align="left">14. 完善了多国语言的功能，能够更好的支持多语言动态切换，并提供示例程序examples/MultiLang</td></tr>
    <tr><td align="left">15. 完善了DPI感知功能，支持Unaware、SystemAware、PerMonitorAware、PerMonitorAware_V2四种模式，支持独立设置DPI，支持高清DPI的适配，提供了示例程序examples/DpiAware</td></tr>
    <tr><td align="left">16. 移除了ui_components工程，CEF组件代码重新梳理，继承到duilib工程中，其他内容删除</td></tr>
    <tr><td align="left">17. 优化窗口的阴影功能，窗口的阴影使用svg图片，增加了阴影类型属性（shadow_type），支持自绘阴影和系统阴影，详见`docs/Window.md`文档</td></tr>
    <tr><td align="left">18. 新增对APNG/SVG/WEBP/ICO/LOTTIE/PAG图片格式的支持</td></tr>
    <tr><td align="left">19. 重新设计控件的loading功能，使用Box容器展示loading功能，通过xml文件配置loading界面（包括动画图片），并支持与动画图片交互</td></tr>
    <tr><td align="left">20. Label文本显示控件的功能加强：对文本齐方式新增加"两端对齐"，新增对竖排文本的支持（文本绘制方向从上到下，从右到左），新增支持设置行间距和设置字间距</td></tr>
    <tr><td align="left">21. Control控件支持全屏显示（通过调用新增加的Window::SetFullscreenControl函数实现该功能），CEF控件和WebView2控件支持F11切换页面全屏</td></tr>
    <tr><td align="left">22. 完善控件动画的功能细节，并引入缓动函数，支持设置控件动画的属性，比如设置缓动函数类型，设置动画总时长和播放间隔等</td></tr>
    <tr>
        <td rowspan="24">新增控件/新增容器</td>
        <td align="left">1. GroupBox：分组容器</td>
    </tr>
    <tr><td align="left">2. HotKey：热键控件</td></tr>
    <tr><td align="left">3. HyperLink：带有超级链接的文字</td></tr>
    <tr><td align="left">4. IPAddress：IP地址控件</td></tr>
    <tr><td align="left">5. Line：画线控件</td></tr>
    <tr><td align="left">6. RichText：格式化文本（类HTML格式）</td></tr>
    <tr><td align="left">7. Split: 分割条控件/容器</td></tr>
    <tr><td align="left">8. TabCtrl：多标签控件（类似浏览器的多标签）</td></tr>
    <tr><td align="left">9. ListCtrl：列表控件（Report/Icon/List三个视图）</td></tr>
    <tr><td align="left">10. PropertyGrid: 属性表控件，支持文本、数字、复选框、字体、颜色、日期、IP地址、热键、文件路径、文件夹等属性</td></tr>
    <tr><td align="left">11. ColorPicker：拾色器，独立窗口，其中的子控件，可以单独作为颜色控件来使用</td></tr>
    <tr><td align="left">12. ComboButton：带有下拉组合框的按钮</td></tr>
    <tr><td align="left">13. DirectoryTree：目录树控件，用于显示文件系统中的目录</td></tr>
    <tr><td align="left">14. AddressBar：地址栏控件，用于显示本地文件系统的路径</td></tr>
    <tr><td align="left">15. WebView2Control：封装了WebView2控件的基本功能</td></tr>
    <tr><td align="left">16. GridBox/GridScrollBox：基于网格布局的控件</td></tr>
    <tr><td align="left">17. HFlowBox/VFlowBox/HFlowScrollBox/VFlowScrollBox：基于水平流式布局和垂直流式布局的控件</td></tr>
    <tr><td align="left">18. MenuBar：菜单栏控件</td></tr>
    <tr><td align="left">19. IconControl/BitmapControl：用户显示基于内存的小图标和位图数据</td></tr>
    <tr><td align="left">20. ChildWindow：子窗口控件，Windows平台的实现为系统原生子窗口（带有WS_CHILD属性）；其他平台为SDL的弹出式窗口，非原生子窗口，SDL不支持原生子窗口</td></tr>
    <tr><td align="left">21. ControlDragableT(模板类，包含以下四个标准控件：ControlDragable/BoxDragable/HBoxDragable/VBoxDragable)：支持相同Box内的子控件通过拖动来调整顺序，支持在不同的Box内通过拖动来调整控件所属容器</td></tr>
    <tr><td align="left">22. ControlMovableT(模板类，包含以下四个标准控件：ControlMovable/BoxMovable/HBoxMovable/VBoxMovable)：支持通过鼠标拖动来调整控件的位置，也支持通过鼠标拖动来调整父容器的位置</td></tr>
    <tr><td align="left">23. ControlResizableT(模板类，包含以下四个标准控件：ControlResizable/BoxResizable/HBoxResizable/VBoxResizable)：支持通过鼠标拖动来调整控件的大小，功能与调整窗口大小的功能相似</td></tr>
    <tr><td align="left">24. XmlBox：支持加载并预览界面库的XML文件的容器，可以用于预览XML文件中定义的控件显示效果</td></tr>
    <tr>
        <td rowspan="3">性能优化</td>
        <td align="left">1. 优化了Control及子控件的内存占用，在界面元素较多的时候，内存占有率有大幅降低</td>
    </tr>
    <tr><td align="left">2. 优化了动画绘制流程，合并定时器的触发事件，避免播放控件动画或者播放动画图片的过程中导致界面很卡的现象</td></tr>
    <tr><td align="left">3. 基于虚表的ListBox控件及关联控件：通过优化实现机制，使得可用性和性能有较大改善</td></tr>
    <tr>
        <td rowspan="15">示例程序完善</td>
        <td align="left">1. examples/ColorPicker: 新增加了拾色器示例程序</td>
    </tr>
    <tr><td align="left">2. examples/ListCtrl：新增加了列表的示例程序，演示列表的个性功能</td></tr>
    <tr><td align="left">3. examples/render：新增加了渲染引擎示例程序，演示了大部分容器、控件、资源管理的功能</td></tr>
    <tr><td align="left">4. examples/TreeView：新增加了树控件的示例程序，演示了树控件的各种功能</td></tr>
    <tr><td align="left">5. examples/RichEdit：新增加了富文本编辑控件的示例程序，演示了富文本编辑控件的各种功能</td></tr>
    <tr><td align="left">6. examples/MultiLang：提供多语言的动态切换功能演示</td></tr>
    <tr><td align="left">7. examples/DpiAware：提供了DPI感知功能的功能演示</td></tr>
    <tr><td align="left">8. examples/threads：提供了多线程的功能演示</td></tr>
    <tr><td align="left">9. examples/WebView2：提供了WebView2控件的功能演示</td></tr>
    <tr><td align="left">10. examples/WebView2Browser：提供了WebView2控件的功能演示（多标签）</td></tr>
    <tr><td align="left">11. examples/layout：所有布局和容器的功能演示</td></tr>
    <tr><td align="left">12. examples/ChildWindow：子窗口控件的功能演示</td></tr>
    <tr><td align="left">13. examples/XmlPreview：XML文件的界面效果预览功能测试（测试XmlBox容器）</td></tr>
    <tr><td align="left">14. examples/ColorTheme：颜色主题的功能预览与测试程序</td></tr>
    <tr><td align="left">15. 其他的示例程序：大部分进行了代码兼容性修改和优化，使得示例程序也可以当作测试程序使用</td></tr>
    <tr>
        <td rowspan="8">完善文档</td>
        <td align="left">1. README.md和docs子目录的文档重新进行了梳理，使得阅读者更容易理解界面库的功能、用法，更易上手</td>
    </tr>
    <tr><td align="left">2. 各个控件的接口没有单独整理成文档，因为可以直接阅读接口文件中的注释来达到目的，目前各个接口的注释是比较完善的</td></tr>
    <tr><td align="left">3. 各个平台的编译文档和依赖的编译脚本</td></tr>
    <tr><td align="left">4. 重新整理主体项目、依赖第三方源码的授权文件，统一放在licenses目录中管理</td></tr>
</table>

## 使用的第三方库说明
| 名称     |代码子目录                 | 用途                                 | license文件        | 协议分类          |
| :---     | :---                      |:---                                  |:---               |:---               |
|apng      |third_party/libpng  | 支持APNG图片格式                     |zlib/libpng License|zlib/libpng 许可协议，宽松型开源协议|
|libpng    |third_party/libpng  | 支持PNG图片格式                      |[libpng.LICENSE.txt](licenses/libpng.LICENSE.txt)|自定义的 BSD 风格宽松协议|
|zlib      |third_party/zlib    | 支持PNG/APNG图片格式<br>Zip文件解压  |[zlib.LICENSE.txt](licenses/zlib.LICENSE.txt)|zlib许可协议，宽松型开源协议|
|cximage   |third_party/cximage | 支持ICO图片格式                      |[cximage.LICENSE.txt](licenses/cximage.LICENSE.txt)|MIT 风格协议（非标准 MIT 协议）|
|giflib    |third_party/giflib  | 支持GIF图片格式                      |[giflib.LICENSE.txt](licenses/giflib.LICENSE.txt)|MIT 许可协议|
|libwebp   |third_party/libwebp | 支持WebP图片格式                     |[libWebP.LICENSE.txt](licenses/libwebp.LICENSE.txt)|BSD 3条款许可协议|
|stb_image |third_party/stb_image| 支持BMP图片格式<br>调整图片大小     |[stb_image.LICENSE.txt](licenses/stb_image.LICENSE.txt)|MIT 许可协议/公有领域协议|
|libjpeg-turbo|third_party/prebuilt/libjpeg-turbo| 支持JPEG图片格式             |[libjpeg-turbo.LICENSE.md](licenses/libjpeg-turbo.LICENSE.md)|IJG 许可证和修改后的BSD 3条款许可协议|
|nanosvg   |third_party/svg      | 支持SVG图片格式                     |[nanosvg.LICENSE.txt](licenses/nanosvg.LICENSE.txt)|zlib 许可协议|
|pugixml   |third_party/xml      | 支持资源描述XML的解析               |[pugixml.LICENSE.txt](licenses/pugixml.LICENSE.txt)|MIT 许可协议|
|ConvertUTF|third_party/convert_utf| 用于UTF-8/UTF-16编码转换         |[llvm.LICENSE.txt](licenses/llvm.LICENSE.txt)|Apache License Version 2.0 为主，<br>LLVM 例外条款补充，<br>历史版本兼容 legacy 协议|
|skia      |项目未包含skia源码          | 界面库渲染引擎<br>支持SVG图片格式<br>支持Lottie JSON动画|[skia.LICENSE.txt](licenses/skia.LICENSE.txt)|BSD 3条款许可协议|
|SDL       |项目未包含SDL源码           | 跨平台窗口管理                     |[SDL.LICENSE.txt](licenses/SDL.LICENSE.txt)|zlib 许可协议|
|duilib    |                            | NIM_Duilib_Framework<br>是基于duilib开发  |[duilib.LICENSE.txt](licenses/duilib.LICENSE.txt)|BSD 2条款许可协议|
|NIM_Duilib<br>Framework|               | 本项目是基于<br>NIM_Duilib_Framework开发   |[NIM_Duilib_Framework.LICENSE.txt](licenses/NIM_Duilib_Framework.LICENSE.txt)|MIT 许可协议|
|libcef    |third_party/prebuilt/libcef   | 用于加载CEF模块|[libcef.LICENSE.txt](licenses/libcef.LICENSE.txt)|BSD 3条款许可协议|
|udis86    |third_party/libudis86| 反汇编计算完整性指令最短长度         |[udis86.LICENSE.txt](licenses/udis86.LICENSE.txt)|BSD 2条款许可协议|
|WebView2  |third_party/<br>Microsoft.Web.WebView2| 支持WebView2控件 |[Microsoft.Web.WebView2.LICENSE.txt](licenses/Microsoft.Web.WebView2.LICENSE.txt)|BSD 3条款许可协议|
|libpag    |third_party/prebuilt/libpag   | 支持PAG动画文件<br>（该功能默认未启用，详见后续文档） |[libpag.LICENSE.txt](licenses/libpag.LICENSE.txt)|Apache License Version 2.0(主体)<br>libpag依赖的第三方组件的<br>授权协议很多，详见目录:<br>`third_party/prebuilt/libpag/licenses`<br>中的文件。如果介意libpag的授权协议<br>（包括主体协议/第三方组件协议），<br>可以不启用libpag。|

## 界面效果预览
使用该界面库编写的示例程序，该文档可以见到各个控件的展示效果：[docs/Examples.md](docs/Examples.md) 

## 关于PAG动画文件格式
* 目前PAG动画文件格式仅在Window平台支持，其他平台暂未支持
* 支持PAG动画文件格式功能默认关闭（因为需要自己编译libpag.lib和libpag.dll，放进项目才能正常编译运行）
* 支持PAG动画格式的开启方法：    
（1）编译前使用配置项开启：`xmake f --pag=y`（默认关闭）    
（2）参照以下文档编译libpag库：[`third_party/prebuilt/libpag/windows/libpag-build.md`](third_party/prebuilt/libpag/windows/libpag-build.md)     
* 编译nim_duilib的时候，直接使用xmake编译即可（`xmake`），编译产物在 bin 目录中。
* libpag库的主体授权协议为Apache License Version 2.0，其依赖的第三方组件的授权协议很多，<br>详见目录:`third_party/prebuilt/libpag/licenses`中的文件。<br>如果介意libpag的授权协议（包括主体协议/第三方组件协议），可以不启用libpag。

## 编程语言
- C/C++: 编译器需要支持C++20（`main`分支）
- C/C++: 编译器需要支持C++17（`develop-cpp17`分支，该分支仅用于支持Windows系统的VS2017/VS2019编译器，其他环境请使用主分支）

## 支持的操作系统
- Windows：7/10/11 及以上
- Linux：OpenEuler、OpenKylin（开放麒麟）、UbuntuKylin（优麒麟）、中科方德、统信UOS、Ubuntu、Debian、Fedora、OpenSuse等
- macOS：12+
- FreeBSD

## 支持的编译器
- Visual Studio 2022/2026（Windows，MSVC，默认用它编译Skia，无需安装LLVM）
- LLVM/Clang（Windows，可选：`xmake f --skia_clang=y` 时使用）
- gcc/g++（Linux、FreeBSD）
- clang/clang++（Linux、macOS、FreeBSD）

## 编译（xmake）
本项目使用 [xmake](https://xmake.io/) 构建：Skia 由项目内置的本地包自动下载并编译（默认使用 MSVC，**不需要安装 LLVM**），SDL3 等依赖由 xmake 包仓库自动获取，其余第三方库使用仓库内的源码或预编译库。

### 一、准备工作
1. 安装 [xmake](https://xmake.io/#/guide/installation)（v2.9 及以上版本）
2. Windows：安装 Visual Studio 2022/2026（勾选“使用 C++ 的桌面开发”工作负载）
3. Linux/macOS/FreeBSD：安装 gcc 或 clang，以及系统依赖（X11、freetype、fontconfig 等）
4. 可选：安装 LLVM/Clang（仅当需要改用 clang 编译 Skia 时使用，配置 `--skia_clang=y`）

> 编译 Skia 需要 Python：xmake 会自动获取并使用自带的 Python 包，无需手工安装或配置。

### 二、编译（Windows/Linux/macOS/FreeBSD 通用）
```
xmake f -o build/build_temp/xmake -c     # 配置（Release；首次配置会自动下载并编译 Skia，约 5 分钟）
xmake                                    # 编译：第三方库 + duilib + 全部示例程序
xmake run basic                          # 运行示例程序（可执行文件输出到 bin 目录）
```

### 三、常用配置项
| 配置项 | 说明 |
| :--- | :--- |
| `-m debug` / `-m release` | Debug/Release 编译（默认 Release）|
| `-a x86` / `-a x64` | 32 位/64 位（Windows 支持；Linux/macOS/FreeBSD 只支持 64 位）|
| `--examples=n` | 只编译库，不编译示例程序 |
| `--sdl=y` | 启用 SDL3（Windows 默认关闭，Linux/macOS/FreeBSD 始终启用）|
| `--cef=y` | 启用 CEF：编译 libcef_dll_wrapper 及 cef/CefBrowser 示例 |
| `--cef109=y` | 使用 CEF 109 版本（兼容 Win7）|
| `--webview2=n` | 关闭 WebView2 控件（Windows，默认开启）|
| `--pag=y` | 启用 libpag（需要先自行编译 libpag.lib 和 libpag.dll）|
| `--jpeg_turbo=y` | 启用 libjpeg-turbo 解码 JPEG 图片 |
| `--md=y` | MSVC 运行库使用 /MD（默认 /MT）|
| `--log=y` | 输出详细的编译配置信息 |
| `--skia_clang=y` | Windows 下改用 LLVM/Clang 编译 Skia（默认用 MSVC）|
| `--skia_clang_dir=D:/LLVM` | 使用 clang 编译 Skia 时的 clang 目录（默认 `C:/LLVM`）|
| `--skia_dir=../skia` | 直接使用已有的 Skia 源码树，跳过自动下载编译 |

### 四、编译产物
- `lib` 目录：duilib 主库与第三方静态库（duilib-zlib、duilib-png、duilib-webp、duilib-cximage）
- `bin` 目录：各个示例程序的可执行文件（依赖 bin/resources 目录中的皮肤、语言文件等资源）
- `build/build_temp`：编译的临时目录，可随时清理；清理后首次编译会重新编译 Skia

### 五、依赖的获取方式
| 依赖 | 获取方式 |
| :--- | :--- |
| Skia | 项目内置的本地包（`xmake/repos`）：自动下载指定快照 + 补丁并编译，仅编译当前配置；也可用 `--skia_dir` 复用已有的 Skia |
| SDL3 | xmake 官方包仓库自动获取 |
| zlib、libpng(含APNG)、giflib、libwebp、cximage | 使用仓库内源码编译 |
| libjpeg-turbo | 使用仓库内预编译库（Windows），其他平台使用 xmake 包 |
| CEF | 可选：需要自行下载 CEF 运行库，详见 [docs/CEF.md](docs/CEF.md) |
| WebView2 | 可选：使用仓库内的 WebView2 SDK（Windows 默认开启）|
| libpag | 可选：需要自行编译 libpag.lib/libpag.dll，详见 [third_party/prebuilt/libpag/windows/libpag-build.md](third_party/prebuilt/libpag/windows/libpag-build.md) |

### 六、平台说明
- Windows：使用 MSVC 编译；`--cef=y` 可启用 CEF（需自行准备 CEF 运行库）；WebView2 默认开启
- Linux/macOS/FreeBSD：SDL3 自动启用（窗口系统基于 SDL3）；CEF 暂不支持，配置 `--cef=y` 会给出明确错误提示
- 重新配置与重编：`xmake f -c` 清除配置重新配置；`xmake -r` 重新编译全部目标

### 七、持续集成（GitHub Actions）

仓库已配置 GitHub Actions 持续集成（[`.github/workflows/ci.yml`](.github/workflows/ci.yml)）：向 `main` 分支提交、提交 Pull Request、手动触发，以及推送 `v*` 标签时自动构建。

| 构建任务 | 运行环境 | 说明 |
| :--- | :--- | :--- |
| windows-x64 / windows-x86 | Windows Server 2022 + MSVC | 默认配置（Skia 渲染、/MT 运行库、WebView2） |
| windows-x64-sdl / windows-x64-md / windows-x64-gdi | Windows Server 2022 + MSVC | 可选配置：SDL3 窗口、/MD 运行库、GDI 渲染 |
| linux-x64 | Ubuntu 22.04 | SDL3 窗口；在虚拟显示（Xvfb）下运行示例做冒烟验证 |
| macos-arm64 | macOS 15（Apple Silicon） | SDL3 窗口 |

- 编译产物（全部示例程序 + 静态库）按平台打包为 `nim_duilib-<版本>-<平台>-<架构>[-<变体>].zip`（Windows）或 `.tar.gz`（Linux/macOS），并附带同名 `.sha256` 校验文件，可在 Actions 运行页面的 Artifacts 中下载。
- 推送 `v*` 标签时，同批压缩包会自动发布到对应版本的 Release 中。
- 首次构建需要从源码编译 Skia，耗时较长；CI 会缓存已编译的 xmake 依赖包（主要是 Skia），后续构建会明显加快。
- 打包与校验由 xmake 脚本完成（[`.github/scripts/package.lua`](.github/scripts/package.lua)），不需要 Python 等额外运行时。
- 本地复现 CI 构建：执行 `xmake f -o build/build_temp/xmake -c -m release`，然后执行 `xmake`；需要打包时执行 `xmake l .github/scripts/package.lua <版本> <平台> <架构> <变体> <最少示例数> "" dist`（其他可选配置见上文"常用配置项"）。


## 开发计划
 - 继续丰富界面库的控件，完善界面库的功能
 - 跨平台（Windows/Linux/macOS/FreeBSD）窗口引擎（基于[SDL3.0](https://www.libsdl.org/)）的不断测试与完善（目前X11/XWayland桌面环境下较稳定，但纯Wayland桌面环境下问题较多）
 - 测试界面库，发现缺陷并修复，不断完善代码

## AI 辅助开发（Claude Code 集成）

nim_duilib 提供了 AI 友好的文档和技能（Skills），可配合 [Claude Code](https://docs.anthropic.com/en/docs/claude-code) 实现 AI 辅助界面开发。

### 功能说明
注册后，Claude Code 在**任何项目**中都可以使用以下 nim_duilib 专属技能：

| 指令 / 技能 | 说明 |
| :--- | :--- |
| `/nim-init` | 为当前项目初始化 nim_duilib 开发环境（复制 LLM 参考文档、更新 CLAUDE.md） |
| `/nim-duilib-create-window` | 创建新窗口（自动生成 C++ 类 + XML 布局文件） |
| `/nim-duilib-xml-layout` | 设计 XML 界面布局（表单、分栏、工具栏、卡片网格等模板） |
| `/nim-duilib-add-control` | 添加控件（15+ 种控件的 XML 片段和 C++ 事件绑定代码） |
| `/nim-duilib-event-handler` | 事件处理（XML 内联事件和 C++ Attach 绑定） |
| `/nim-duilib-theme` | 主题定制（预定义颜色、字体、100+ 通用样式 Class 速查） |
| `/nim-duilib-resource-pack` | 资源打包与部署（ZIP 打包、嵌入 EXE 单文件发布） |

### 快速开始

**前提条件：** 已安装 [Claude Code](https://docs.anthropic.com/en/docs/claude-code)

**第一步：注册（只需执行一次）**

在 nim_duilib 根目录下执行：
```bash
# Windows (CMD / PowerShell)
.claude\register.bat

# Linux / macOS
bash .claude/register.sh
```
注册脚本会将所有技能安装到 `~/.claude/skills/`，全局生效。

**第二步：在你的应用项目中使用**

在任何需要使用 nim_duilib 的项目中打开 Claude Code，输入：
```
/nim-init
```
AI 会自动为该项目配置 LLM 参考文档和 CLAUDE.md。之后即可直接用自然语言指示 Claude 完成界面开发，例如：
- "创建一个设置窗口，包含用户名输入框和保存按钮"
- "设计一个左侧导航栏 + 右侧内容区的布局"
- "给这个按钮添加点击事件"
- "把资源打包成单个 EXE"

### 更新技能
当 nim_duilib 的 AI 技能文件（`.claude/skills/`）有更新时，重新执行注册脚本即可：
```bash
cd nim_duilib
.claude\register.bat   # Windows
# bash .claude/register.sh  # Linux / macOS
```

### 注销
如需移除所有全局技能：
```bash
bash nim_duilib/.claude/unregister.sh
```

### 文件结构
```
nim_duilib/.claude/
├── register.bat / register.ps1 / register.sh   # 全局注册脚本
├── unregister.sh                                # 注销脚本
├── docs/
│   └── nim-duilib-llm-reference.md              # LLM 完整参考手册
└── skills/                                      # AI 技能定义
    ├── nim-duilib-create-window.md
    ├── nim-duilib-xml-layout.md
    ├── nim-duilib-add-control.md
    ├── nim-duilib-event-handler.md
    ├── nim-duilib-theme.md
    └── nim-duilib-resource-pack.md
```

## 参考文档

 - [快速上手](docs/Getting-Started.md)
 - [示例程序](docs/Examples.md)
 - [全局样式：字体、颜色、图片等资源](docs/Global.md)
 - [窗口的样式](docs/Window.md)
 - [容器的样式](docs/Box.md)
 - [控件的样式](docs/Control.md)
 - [菜单的样式](docs/Menu.md)
 - [控件的事件/消息](docs/Events.md)
 - [XML文件中各控件的节点名称](docs/XmlNode.md)
 - [XML文件中直接响应控件的事件](docs/XmlEvents.md)
 - [CEF控件](docs/CEF.md)
 - [编译方式（xmake）](#编译xmake)
 - [参考文档](docs/Summary.md)

## 相关链接
1. Skia的编译文档库，点击访问：[skia_compile](https://github.com/rhett-lee/skia_compile) ：    
2. 本项目是直接在NIM_Duilib_Framework项目的基础上继续开发的，项目地址：[NIM_Duilib_Framework](https://github.com/netease-im/NIM_Duilib_Framework/)
3. NIM_Duilib_Framework项目是基于duilib开发的，项目地址：[duilib](https://github.com/duilib/duilib)
