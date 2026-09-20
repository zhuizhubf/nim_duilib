# CEF 扩展说明（duilib-cef）

本文件说明 `extensions/cef` 扩展的目录结构、构建开关与接入要点；
CEF 控件的功能、事件、JS 交互等**用法指南**见仓库根文档
[docs/CEF.md](../../../docs/CEF.md)。

## 一、扩展与核心库的边界

- 核心库 `src/duilib` 不包含 CEF 的源码与头文件，也不依赖 libcef SDK；
  核心库只提供控件基类、窗口、布局、事件与渲染等公共能力。
- CEF 控件通过 `ui::GlobalManager::AddCreateControlCallback` 自注册工厂，
  XML 中直接书写 `<CefControl>` 节点即可创建，核心控件类名表不登记该名字。
- 核心库中的 `duilib/duilib_cef.h` 是对外聚合头，内容为
  `cef/CefControl.h`、`cef/CefControlEvent.h`、`cef/CefManager.h`；
  使用它的工程需要在包含目录中加入 `extensions/cef/src`。

## 二、目录结构

```text
extensions/cef/
├─ src/cef/                      # 控件源码与公开头文件
│   ├─ CefControl.h/.cpp         # 控件基类：窗口模式与离屏模式的公共逻辑
│   ├─ CefControlNative.*        # 窗口模式（WS_CHILD 子窗口）实现
│   ├─ CefControlOffScreen.*     # 离屏渲染（OSR）实现
│   ├─ CefControlEvent.h         # 网页事件回调接口
│   ├─ CefManager.h/.cpp         # CEF 模块初始化/反初始化、全局配置
│   ├─ CefManager_Windows/Linux/MacOS.*  # 各平台初始化细节
│   ├─ CefWindowUtils_*.*        # 各平台窗口工具
│   └─ internal/                 # CEF 客户端、JS 桥、浏览器处理器、Windows 拖放与 IME
├─ third_party/prebuilt/libcef/  # libcef SDK（含 libcef_dll_wrapper 源码与预编译库）
├─ docs/                         # 本说明
└─ licenses/                     # libcef 许可文件
```

## 三、构建

扩展通过仓库根目录的构建入口启用，不提供扩展目录内的独立构建入口：

```powershell
xmake f -c -m release --with_cef=latest    # libcef 142（默认）
xmake f -c -m release --with_cef=109       # libcef 109（兼容 Win7）
xmake                                      # 构建核心库 + libcef_dll_wrapper + duilib-cef + 示例
```

相关 target：

| target | 说明 |
| :--- | :--- |
| `duilib-cef` | CEF 控件静态库（`lib/duilib-cef.lib`） |
| `libcef_dll_wrapper` / `libcef_dll_wrapper_109` | CEF 官方 C++ 封装库，源码取自扩展内的 SDK |
| `cef` / `CefBrowser` | 示例程序（窗口模式 / 离屏渲染多标签浏览器） |

CEF 目前只在 Windows 平台接入 xmake 构建；其他平台配置 `--with_cef=latest` 会在配置阶段
给出明确错误提示，Linux/macOS 的 SDK 工程文件随扩展保留。

## 四、接入要点

1. 链接 `duilib-cef`（以及 `libcef_dll_wrapper`/`libcef_dll_wrapper_109`）。
2. 头文件包含目录：
   - `<repo>/src`（`duilib/...` 公开头）
   - `<repo>/extensions/cef/src`（`cef/...`）
   - `<repo>/extensions/cef/third_party/prebuilt/libcef/<版本目录>`（`include/cef_*.h`）
3. 在加载含 `CefControl` 的 XML 之前调用
   `ui::CefManager::GetInstance()->Initialize(...)`，参数与完整流程见
   [examples/cef/TestApplication.cpp](../../../examples/cef/TestApplication.cpp)。
4. 运行时把 libcef 的二进制与资源文件放到程序目录下的 `bin/libcef_win/x64`
   （Win32 为 `bin/libcef_win/Win32`，109 版本为 `bin/libcef_win_109/...`）。

## 五、已知限制

- 不实现操作系统无障碍桥接与跨进程“拖出”拖动（控件内部拖放与外部拖入仍可用）。
- 输入法使用系统候选框定位，不实现内联 IME 组合串。
- xmake 构建仅覆盖 Windows；Linux/macOS 需要自行按 SDK 工程文件构建。
