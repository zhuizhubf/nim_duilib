#ifndef UI_RENDER_SKIA_TEXT_SHAPER_H_
#define UI_RENDER_SKIA_TEXT_SHAPER_H_

#include "text/ITextShaper.h"

#include <memory>
#include <vector>

namespace ui {
class Render_Skia;

/** Skia 后端的 ITextShaper 实现
*/
class SkiaTextShaper : public ITextShaper
{
public:
    explicit SkiaTextShaper(Render_Skia *pRender);
    SkiaTextShaper(const SkiaTextShaper &) = delete;
    SkiaTextShaper &operator=(const SkiaTextShaper &) = delete;
    virtual ~SkiaTextShaper() override = default;

    virtual IFont *CreateFont(const UiFont &fontInfo) override;
    virtual bool GetFontMetrics(const IFont *pFont, TextFontMetrics &metrics) override;
    virtual bool ResolveGlyph(
        const IFont *pFont,
        uint32_t unicodeChar,
        TextGlyphInfo &glyph,
        bool bUseDefaultCharWhenFailed) override;
    virtual void DrawGlyph(
        const TextGlyphInfo &glyph, float x, float y, UiColor textColor, uint8_t uFade) override;

private:
    Render_Skia *m_pRender = nullptr;
    std::vector<std::pair<UiFont, std::unique_ptr<IFont>>> m_fontCache;
};

} // namespace ui

#endif // UI_RENDER_SKIA_TEXT_SHAPER_H_
