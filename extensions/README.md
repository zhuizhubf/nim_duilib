# nim_duilib extensions

本目录用于放置 **不属于 nim_duilib 核心库** 的可选扩展。

当前扩展：

- `scintilla/`：DUI 原生 Scintilla 编辑控件扩展，不修改核心库源码、核心
  `third_party` 和核心构建 target。

扩展拥有自己的 `xmake.lua`、第三方依赖、示例、文档和许可证，使用核心库
已经构建好的静态库与公开头文件。
