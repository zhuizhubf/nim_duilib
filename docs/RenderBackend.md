# 渲染后端与模块拆分

## 目标

渲染实现已从 `duilib` 核心库中拆分出来，核心库只依赖 `IRender` 等公共接口，不直接包含 Skia 或 GDI+ 的渲染实现。

保留 `duilib` 作为核心库名称。使用库的工程需要额外链接所需的渲染后端和图片解码模块。

所有库源码位于 `src/` 下（核心库为 `src/duilib`），包含根为 `<repo>/src` 与仓库根 `<repo>`（后者用于 `third_party/...`）。核心库的公开包含路径 `duilib/...` 保持不变。

## 静态库

| 库 | 说明 |
| :--- | :--- |
| `duilib` | 核心库：控件、窗口、资源管理、字体管理、公共 `IRender` 接口和后端注册表 |
| `duilib-text` | 后端无关的文本布局、换行、对齐、富文本基础和 `ITextShaper` 接口 |
| `duilib-skia-base` | Skia 字体管理和公共基础封装 |
| `duilib-render-skia` | Skia 渲染后端，链接 `duilib`、`duilib-text`、`duilib-skia-base` 和 Skia |
| `duilib-render-gdi` | Windows GDI/GDI+ 渲染后端，链接 `duilib`、`duilib-text`、`gdiplus` 和 `msimg32` |
| `duilib-image-svg-nanosvg` | 基于 nanosvg 的独立 SVG 解码模块，不依赖渲染后端 |
| `duilib-image-svg-skia` | 基于 Skia 的 SVG 解码模块 |
| `duilib-image-lottie-skia` | 基于 Skia 的 Lottie 解码模块，不依赖 `duilib-render-skia` |
| `duilib-cef` | CEF 控件模块（可选，`--cef=y` 时编译并链接；源码位于 `src/cef`） |
| `duilib-webview2` | WebView2 控件模块（可选，Windows + `--webview2=y` 时编译并链接；源码位于 `src/webview2`） |

图片解码模块通过 `IRenderFactory::CreateBitmap` 创建 `IBitmap`，因此可以配合任意当前渲染后端使用。

CEF 与 WebView2 控件通过 `GlobalManager::AddCreateControlCallback` 自注册，核心库不包含这两个模块的源码与头文件。**使用这两个控件的工程需要额外链接 `duilib-cef` / `duilib-webview2`**，并照常调用 `CefManager::Initialize` / `WebView2Manager::Initialize` 注册控件。

## 构建配置

使用 xmake 配置：

```sh
# 默认：Skia 渲染，SVG/Lottie 使用 Skia 解码
xmake f -c --render=skia

# Windows：GDI/GDI+ 渲染，默认使用 nanosvg 解码 SVG，关闭 Lottie
xmake f -c --render=gdi

# Windows：同时链接 Skia 和 GDI 后端，运行时可在 Startup 前选择
xmake f -c --render=both
```

图片解码模块选项：

```sh
--svg=auto       # 默认；有 Skia 基础层时优先 Skia，否则使用 nanosvg
--svg=nanosvg    # 强制使用 nanosvg
--svg=skia       # 使用 Skia SVG 解码模块
--svg=off        # 不注册 SVG 解码器

--lottie=auto    # Skia/双后端默认 skia；GDI-only 默认 off
--lottie=off     # 不注册 Lottie 解码器
--lottie=skia    # 链接 Skia Lottie 解码模块（可配合 GDI 渲染使用）
```

Skia 文本迁移过渡选项：

```sh
--common_text_layout=y   # 默认开启：Skia 文本走 duilib-text 公共布局
--common_text_layout=n   # 回退到 RenderSkia 旧的文本布局实现
```

公共文本布局覆盖普通横排/竖排字符串和 RichText 绘制/测量/缓存接口。若遇到视觉回归，可通过 `--common_text_layout=n` 暂时回退。

`--render=gdi` 和 `--render=both` 仅支持 Windows。

## 运行期选择

`--render=both` 时会同时链接两个渲染后端。可以在 `GlobalManager::Startup` 之前选择：

```cpp
ui::GlobalManager::Instance().SetRenderType(ui::RenderType::kRenderType_GDI);
ui::GlobalManager::Instance().Startup(resParam);
```

规则如下：

- `SetRenderType` 在首次 `Startup` 前，以及 `Shutdown` 后生效。
- 已启动时调用返回 `false`。
- 请求未链接的后端时返回 `false`。
- 不支持运行中热切换，也不支持同一进程内按窗口混用后端。

## GDI 后端能力

GDI 后端使用 32bpp DIB + 内存 DC 绘制，普通窗口通过 `BitBlt` 提交，分层窗口通过 `UpdateLayeredWindow` 提交。

- 基础图元、裁剪和坐标原点：GDI。
- 路径、渐变、半透明、图像变换、抗锯齿和阴影近似：GDI+。
- 文本布局：`duilib-text` + GDI shaper；支持横排、基本富文本、换行、对齐和字体回退。
- 当前不支持的 GDI 能力：彩色 Emoji、Skia 等价的高斯模糊阴影、完整竖排排版。
- `render_backend_type="GL"` 在 GDI 模式下会回退为 CPU 绘制。

## 兼容性说明

- 默认 `--render=skia` 行为保持不变。
- 外部工程升级后需要显式链接所选渲染后端和图片解码模块。
- GDI 配置下不再注册 SVG-Skia 和 Lottie-Skia；需要这些格式时可使用 `--svg=skia`、`--lottie=skia`，或使用 `--svg=nanosvg`。
