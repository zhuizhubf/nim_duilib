# duilib-webview2

`duilib-webview2` 是 nim_duilib 的 WebView2 浏览器控件扩展
（`WebView2Control`），基于微软 Edge WebView2（Chromium 内核）实现网页显示，
仅支持 Windows 平台。

## 特性

- 通过 `GlobalManager::AddCreateControlCallback` 自注册控件工厂，XML 中直接使用
  `<WebView2Control>` 节点，核心库的控件类名表不登记扩展控件类名。
- 封装了 WebView2 的常用功能：导航、前进后退、刷新、JavaScript 执行与回调、
  开发者工具、Cookie 管理、页面缩放、全屏（F11）等。
- 使用仓库内的 WebView2 SDK（`WebView2LoaderStatic` 静态链接），无需用户安装
  NuGet 包。
- 核心 nim_duilib 库零改动：核心 `src/` 与核心 `third_party/` 不再包含 WebView2
  的源码与 SDK，扩展使用核心库已经构建好的静态库与公开头文件。

## 目录结构

```text
extensions/webview2/
├─ src/webview2/                              # WebView2 控件源码与公开头文件
├─ third_party/prebuilt/Microsoft.Web.WebView2/ # WebView2 SDK（头文件 + 预编译库）
├─ docs/WebView2.md                           # 扩展说明（目录结构/构建/接入/接口）
└─ licenses/                                  # 许可文件
```

示例位于主工程的示例目录：`examples/WebView2`、`examples/WebView2Browser`
（多标签浏览器），皮肤资源位于 `bin/resources/themes/default/webview2` 与
`bin/resources/themes/default/webview2_browser`。

## 构建

扩展通过主工程的构建入口启用（不提供扩展独立构建入口）。Windows 平台上
WebView2 扩展默认开启，如需关闭：

```powershell
cd E:\LS\nim_duilib
xmake f -c -m release                  # Windows 默认构建 WebView2 扩展
# xmake f -c -m release --enable_webview2=n   # 关闭 WebView2 扩展
xmake                                  # 核心库 + duilib-webview2 + 全部示例
```

非 Windows 平台不会构建该扩展。

## 应用接入

1. 链接扩展静态库 `duilib-webview2` 与 SDK 的 `WebView2LoaderStatic`。
2. 头文件包含目录需要添加：
   - `<repo>/src`：公开头 `duilib/...`（含聚合头 `duilib/duilib_webview2.h`）
   - `<repo>/extensions/webview2/src`：`webview2/...` 头文件
   - `<repo>/extensions/webview2`：SDK 头 `third_party/prebuilt/Microsoft.Web.WebView2/build/native/include/WebView2.h`
   - `<repo>/extensions/webview2/third_party/prebuilt/Microsoft.Web.WebView2/build/native/<x64|x86>`（链接目录）
3. 在加载包含 `WebView2Control` 的 XML 之前初始化（完整流程见
   [examples/WebView2/MainThread.cpp](../../examples/WebView2/MainThread.cpp)）：

```cpp
#include "duilib/duilib_webview2.h"
#include "webview2/WebView2Manager.h"

DString userDataFolder = ui::WebView2Manager::GetInstance().GetDefaultUserDataFolder(_T("MyApp"));
ui::WebView2Manager::GetInstance().Initialize(userDataFolder);
```

4. 在窗口 XML 中使用控件（类型名为 `WebView2Control`）：

```xml
<WebView2Control name="webview2" url="https://www.example.com" devtools_enabled="true"/>
```

5. 运行时会自动使用用户机器上的 Edge WebView2 Runtime（Windows 10/11 默认已安装）；
   SDK 中的 `WebView2Loader.dll` 在需要动态加载时可放到程序目录。

## 公开接口

`ui::WebView2Control`（[src/webview2/WebView2Control.h](src/webview2/WebView2Control.h)）：

- 初始化：`InitializeAsync`、`IsInitializing`、`IsInitialized`
- 导航：`Navigate`、`NavigateBack`、`NavigateForward`、`Refresh`、`Stop`、
  `GetUrl`、`GetTitle`、`IsNavigating`、`CanGoBack`、`CanGoForward`
- 脚本与消息：`ExecuteScript`、`SetScriptEnabled`、`SetWebMessageEnabled`、
  `PostWebMessageAsJson`、`PostWebMessageAsString`
- 页面设置：`SetUserAgent`、`SetZoomFactor`、`SetAreDefaultContextMenusEnabled`、
  `SetAreDefaultScriptDialogsEnabled`、`CapturePreview`
- 开发者工具：`SetAreDevToolsEnabled`、`OpenDevToolsWindow`
- 事件回调：`SetWebMessageReceivedCallback`、`SetNavigationStateChangedCallback`、
  `SetDocumentTitleChangedCallback`、`SetSourceChangedCallback`、
  `SetNewWindowRequestedCallback`、`SetHistoryChangedCallback`、
  `SetZoomFactorChangedCallback`、`SetFavIconChangedCallback`

XML 支持的扩展属性、示例与截图见 [../../docs/Control.md](../../docs/Control.md) 与
[../../docs/Examples.md](../../docs/Examples.md)；扩展的目录结构、构建开关与
接入要点见 [docs/WebView2.md](docs/WebView2.md)。

## SDK 版本与许可

| SDK | 版本 | 许可 |
| :--- | :--- | :--- |
| Microsoft.Web.WebView2 | 1.0.3296.44 | [Microsoft.Web.WebView2.LICENSE.txt](licenses/Microsoft.Web.WebView2.LICENSE.txt)（BSD 3 条款） |

## 已知限制

- 仅支持 Windows 平台（Windows 7/8 需要自行部署 WebView2 Runtime）。
- 不支持与 CEF 控件混用同一窗口的 GPU 合成场景（两者都使用离屏渲染时的叠加顺序受控件层级限制）。
