# duilib-cef

`duilib-cef` 是 nim_duilib 的 CEF 浏览器控件扩展（`CefControl`），把 libcef
（Chromium Embedded Framework）封装成 Duilib 的控件，用于在界面中嵌入网页。

## 特性

- 通过 `GlobalManager::AddCreateControlCallback` 自注册控件工厂，XML 中直接使用
  `<CefControl>` 节点，核心库的控件类名表不登记扩展控件类名。
- 提供窗口模式（`CefControlNative`）与离屏渲染模式（`CefControlOffScreen`）
  两种实现，支持网页导航、JavaScript 交互、右键菜单、下载、F11 全屏等。
- 支持 libcef 109 版本（兼容 Windows 7）与 libcef 142 版本（Windows 10 及以上），
  SDK 内含 Windows/Linux/macOS 的工程文件与预编译库。
- 核心 nim_duilib 库零改动：核心 `src/` 与核心 `third_party/` 不再包含 CEF
  的源码与 SDK，扩展使用核心库已经构建好的静态库与公开头文件。

## 目录结构

```text
extensions/cef/
├─ src/cef/                      # CEF 控件源码与公开头文件（含 internal/ 子目录）
├─ third_party/prebuilt/libcef/  # libcef SDK（libcef_dll_wrapper 源码 + 预编译库）
├─ docs/CEF.md                   # 扩展说明（目录结构/构建/接入/限制）
└─ licenses/                     # 许可文件
```

示例位于主工程的示例目录：`examples/cef`（窗口模式）、`examples/CefBrowser`
（离屏渲染的多标签浏览器），皮肤资源位于
`bin/resources/themes/default/cef` 与 `bin/resources/themes/default/cef_browser`。

## 构建

扩展通过主工程的构建入口启用（不提供扩展独立构建入口）：

```powershell
cd E:\LS\nim_duilib
xmake f -c -m release --cef=y                # 使用 libcef 142（默认）
# xmake f -c -m release --cef=y --cef109=y   # 使用 libcef 109（兼容 Win7）
xmake                                        # 核心库 + libcef_dll_wrapper + duilib-cef + 示例
```

不传 `--cef` 时（默认）不会构建任何 CEF 相关目标。CEF 目前只支持在 Windows
平台上通过 xmake 构建，其他平台配置 `--cef=y` 会在配置阶段给出明确错误提示。

## 应用接入

1. 链接扩展静态库 `duilib-cef`，以及 CEF 封装库
   `libcef_dll_wrapper`（109 版本为 `libcef_dll_wrapper_109`）。
2. 头文件包含目录需要添加：
   - `<repo>/src`：公开头 `duilib/...`（含聚合头 `duilib/duilib_cef.h`）
   - `<repo>/extensions/cef/src`：聚合头内部引用的 `cef/...`
   - `<repo>/extensions/cef/third_party/prebuilt/libcef/<版本目录>`：CEF 自身的 `include/cef_*.h`
3. 在加载包含 `CefControl` 的 XML 之前初始化 CEF（参数含义与完整流程见
   [examples/cef/TestApplication.cpp](../../examples/cef/TestApplication.cpp)）：

```cpp
#include "duilib/duilib_cef.h"

int32_t nExitCode = 0;
bool bEnableOffScreenRendering = false;   // 窗口模式传 false，离屏渲染模式传 true
if (!ui::CefManager::GetInstance()->Initialize(bEnableOffScreenRendering,
                                              _T("MyApp"), argc, argv, nullptr, nExitCode)) {
    return nExitCode;                     // 初始化失败，应退出进程
}
```

4. 在窗口 XML 中使用控件（类型名为 `CefControl`）：

```xml
<CefControl name="cef_control" url="cef.html" url_is_local_file="true"/>
```

5. 运行时把 libcef 的二进制与资源文件放到程序目录下的 `bin/libcef_win/x64`
   （Win32 为 `bin/libcef_win/Win32`，109 版本为 `bin/libcef_win_109/...`），
   这些文件需要自行从 CEF 官方构建站点下载，详见 [../../docs/CEF.md](../../docs/CEF.md)。

## SDK 版本与许可

| SDK | 版本 | 许可 |
| :--- | :--- | :--- |
| libcef 142 | cef_binary_142.0.10+g29548e2+chromium-142.0.7444.135 | [libcef.LICENSE.txt](licenses/libcef.LICENSE.txt)（BSD 3 条款） |
| libcef 109 | cef_binary_109.1.18+gf1c41e4+chromium-109.0.5414.120 | 同上 |

## SDK 更新说明

1. libcef 对 CMakeLists.txt 和 VC 工程文件有修改，更新时需要逐项对比：
   - `libcef_win/include/cef_config.h`（修改内容：`CEF_V8_ENABLE_SANDBOX` 宏只有 64 位系统定义）
   - `libcef_win/CMakeLists.txt`
   - `libcef_win/libcef_dll_wrapper.vcxproj`、`libcef_win/libcef_dll_wrapper.vcxproj.filters`
   - `libcef_win_109/CMakeLists.txt`
   - `libcef_win_109/libcef_dll_wrapper_109.vcxproj`、`libcef_win_109/libcef_dll_wrapper_109.vcxproj.filters`
   - `libcef_linux/CMakeLists.txt`
   - `libcef_macos/CMakeLists.txt`

   （以上路径均相对于 `extensions/cef/third_party/prebuilt/libcef/`）

2. 项目中其他需要同步修改的内容：
   - [../../third_party/README.md](../../third_party/README.md)：SDK 版本号与下载地址
   - [../../docs/CEF.md](../../docs/CEF.md)：CEF 版本号与运行时目录说明
   - [../../README.md](../../README.md) 与 `xmake/` 目录：CEF 版本号与编译配置

## 已知限制

- 只有 Windows 平台接入了 xmake 构建（Linux/macOS 的 SDK 工程文件随扩展保留）。
- 不实现操作系统无障碍桥接与跨进程“拖出”拖动。
- 扩展的目录结构、构建开关与接入要点见 [docs/CEF.md](docs/CEF.md)；
  接口、事件与 JS 交互的用法指南见 [../../docs/CEF.md](../../docs/CEF.md)。
