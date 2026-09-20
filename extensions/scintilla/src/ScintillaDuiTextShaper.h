#ifndef DUILIB_EXT_SCINTILLA_SCINTILLA_DUI_TEXT_SHAPER_H_
#define DUILIB_EXT_SCINTILLA_SCINTILLA_DUI_TEXT_SHAPER_H_

#include <memory>

namespace ui {
class IRender;
class ITextShaper;
} // namespace ui

namespace duilib::ext::scintilla {

/** 文本塑形适配层（Scintilla Surface 与 Duilib 文本能力的唯一耦合点）。
 *
 * 对应 Qt 平台层中的 QFontMetricsF / QTextLayout：Scintilla 需要
 * "按字形逐步度量 + 按基线绘制字形"，Duilib 通过 ui::ITextShaper 提供该能力。
 *
 * 后端差异集中在此处：
 *   - GDI 后端：ui::Render_GDI_Windows 自身实现了 ui::ITextShaper；
 *   - Skia 后端：ui::Render_Skia 内部持有 SkiaTextShaper，此处按需创建。
 *
 * 如果核心库未来把 ui::IRender::GetTextShaper() 作为公开接口，只需修改本文件。
 */
class ScintillaDuiTextShaper final
{
public:
    ScintillaDuiTextShaper() = delete;
    ~ScintillaDuiTextShaper() = delete;

    /** 获取当前 Render 关联的文本塑形接口。
     * @param [in] pRender 当前绘制目标（可以为 nullptr）
     * @param [out] pOwnedShaper 当接口由本函数创建时，返回其所有权
     * @return 文本塑形接口，返回 nullptr 表示当前 Render 不支持文本塑形
     */
    static ui::ITextShaper *Get(ui::IRender *pRender, std::unique_ptr<ui::ITextShaper> &pOwnedShaper);
};

} // namespace duilib::ext::scintilla

#endif // DUILIB_EXT_SCINTILLA_SCINTILLA_DUI_TEXT_SHAPER_H_
