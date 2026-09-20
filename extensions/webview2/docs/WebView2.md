# WebView2 扩展说明（duilib-webview2）

本文件说明 `extensions/webview2` 扩展的目录结构、构建开关、接入方式与公开接口；
XML 属性列表见 [docs/Control.md](../../../docs/Control.md)，
示例截图与说明见 [docs/Examples.md](../../../docs/Examples.md)。

## 一、扩展与核心库的边界

- 核心库 `src/duilib` 不包含 WebView2 的源码与头文件，也不依赖 WebView2 SDK；
  WebView2 控件通过 `ui::GlobalManager::AddCreateControlCallback` 自注册工厂，
  XML 中直接书写 `<WebView2Control>` 节点即可创建。
- 核心库中的 `duilib/duilib_webview2.h` 是对外聚合头，内容为
  `webview2/ComCallback.h`、`webview2/ComPtr.h`、`webview2/WebView2Control.h`、
  `webview2/WebView2Manager.h`；使用它的工程需要加入 `extensions/webview2/src`。
- WebView2 仅支持 Windows 平台。

## 二、目录结构

```text
extensions/webview2/
├─ src/webview2/
│   ├─ WebView2Control.h/.cpp        # 控件对外接口（继承 ui::Control）
│   ├─ WebView2ControlImpl.*         # ICoreWebView2 事件与控制器实现
│   ├─ WebView2EnvironmentOptions.*  # ICoreWebView2EnvironmentOptions 实现
│   ├─ WebView2Manager.h/.cpp        # 全局初始化（缓存目录、语言、UserAgent 等）
│   ├─ ComPtr.h / ComCallback.h      # COM 智能指针与回调模板
├─ third_party/prebuilt/Microsoft.Web.WebView2/  # WebView2 SDK（头文件 + 预编译库）
├─ docs/                             # 本说明
└─ licenses/                         # WebView2 许可文件
```

## 三、构建

扩展通过仓库根目录的构建入口启用，不提供扩展目录内的独立构建入口；
Windows 平台默认启用，可用 `--webview2=n` 关闭：

```powershell
xmake f -c -m release                  # Windows：同时构建 duilib-webview2 与示例
xmake f -c -m release --webview2=n     # 关闭 WebView2 扩展
xmake                                  # 核心库 + duilib-webview2 + 全部示例
```

产物为 `lib/duilib-webview2.lib`；示例为 `bin/WebView2.exe` 与
`bin/WebView2Browser.exe`。

## 四、接入要点

1. 链接 `duilib-webview2` 与 SDK 的 `WebView2LoaderStatic`。
2. 头文件包含目录：
   - `<repo>/src`（`duilib/...` 公开头）
   - `<repo>/extensions/webview2/src`（`webview2/...`）
   - `<repo>/extensions/webview2`（SDK 头的 `third_party/prebuilt/...` 路径）
   - 链接目录：`<repo>/extensions/webview2/third_party/prebuilt/Microsoft.Web.WebView2/build/native/<x64|x86>`
3. 初始化（在加载含 `WebView2Control` 的 XML 之前）：

```cpp
#include "duilib/duilib_webview2.h"
#include "webview2/WebView2Manager.h"

DString userDataFolder = ui::WebView2Manager::GetInstance().GetDefaultUserDataFolder(_T("MyApp"));
ui::WebView2Manager::GetInstance().Initialize(userDataFolder);
```

4. XML 中使用控件（类型名 `WebView2Control`）：

```xml
<WebView2Control name="webview2" url="https://www.example.com" devtools_enabled="true"/>
```

5. 运行时依赖用户机器上的 Edge WebView2 Runtime（Windows 10/11 默认已安装）；
   网页缓存目录默认是 `${程序目录}/webview2_cache/${程序名称}`。

## 五、公开接口摘要

`ui::WebView2Control`：

- 初始化与状态：`InitializeAsync`、`IsInitializing`、`IsInitialized`
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

## 六、已知限制

- 仅支持 Windows 平台；Windows 7/8 需要自行部署 WebView2 Runtime。
- 与 CEF 控件同时使用时，两者的离屏渲染叠加顺序受控件层级限制。
