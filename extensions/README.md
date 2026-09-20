# nim_duilib extensions

本目录用于放置 **不属于 nim_duilib 核心库** 的可选扩展。

当前扩展：

- `scintilla/`：DUI 原生 Scintilla 编辑控件扩展，通过 `--scintilla=y` 启用，
  并额外提供扩展目录内的独立构建入口。
- `cef/`：CEF 浏览器控件扩展（`CefControl`），通过 `--cef=y` 启用
  （`--cef109=y` 使用兼容 Win7 的 libcef 109）。
- `webview2/`：WebView2 浏览器控件扩展（`WebView2Control`），Windows 默认启用，
  可用 `--webview2=n` 关闭。

扩展拥有自己的源码、第三方依赖、示例、文档和许可证，使用核心库已经构建好的
静态库与公开头文件；核心 `src/` 与核心 `third_party/` 不包含扩展的源码与 SDK，
核心构建 target 也不依赖扩展。

构建方式：`cef/` 与 `webview2/` 只通过仓库根目录的构建入口启用
（`xmake f --cef=y`、`--webview2=n` 等），不提供扩展目录内的独立构建入口；
`scintilla/` 额外提供 `extensions/scintilla/xmake.lua`，可在扩展目录内独立构建。
