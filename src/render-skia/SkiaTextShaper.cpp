#include "render-skia/SkiaTextShaper.h"

#include "duilib/Core/GlobalManager.h"
#include "render-skia/DrawSkiaText.h"
#include "render-skia/Font_Skia.h"
#include "render-skia/Render_Skia.h"

#include "SkiaHeaderBegin.h"
#include "SkiaHeaderEnd.h"
#include "include/core/SkCanvas.h"
#include "include/core/SkFont.h"
#include "include/core/SkFontMetrics.h"
#include "include/core/SkPaint.h"

namespace ui {

SkiaTextShaper::SkiaTextShaper(Render_Skia *pRender)
    : m_pRender(pRender)
{}

IFont *SkiaTextShaper::CreateFont(const UiFont &fontInfo)
{
    for (const auto &item : m_fontCache) {
        if (item.first == fontInfo) {
            return item.second.get();
        }
    }
    IRenderFactory *pRenderFactory = GlobalManager::Instance().GetRenderFactory();
    if (pRenderFactory == nullptr) {
        return nullptr;
    }
    std::unique_ptr<IFont> pFont(pRenderFactory->CreateIFont());
    if ((pFont == nullptr) || !pFont->InitFont(fontInfo)) {
        return nullptr;
    }
    IFont *pFontPtr = pFont.get();
    m_fontCache.emplace_back(fontInfo, std::move(pFont));
    return pFontPtr;
}

bool SkiaTextShaper::GetFontMetrics(const IFont *pFont, TextFontMetrics &metrics)
{
    Font_Skia *pSkiaFont = dynamic_cast<Font_Skia *>(const_cast<IFont *>(pFont));
    if (pSkiaFont == nullptr) {
        return false;
    }
    const SkFont *pFontHandle = pSkiaFont->GetFontHandle();
    if (pFontHandle == nullptr) {
        return false;
    }
    SkFontMetrics skMetrics;
    pFontHandle->getMetrics(&skMetrics);
    metrics.m_fAscent = -skMetrics.fAscent;
    metrics.m_fDescent = skMetrics.fDescent;
    metrics.m_fHeight = skMetrics.fBottom - skMetrics.fTop;
    return true;
}

bool SkiaTextShaper::ResolveGlyph(
    const IFont *pFont, uint32_t unicodeChar, TextGlyphInfo &glyph, bool bUseDefaultCharWhenFailed)
{
    if ((pFont == nullptr) || (m_pRender == nullptr)) {
        return false;
    }
    Font_Skia *pSkiaFont = dynamic_cast<Font_Skia *>(const_cast<IFont *>(pFont));
    if (pSkiaFont == nullptr) {
        return false;
    }
    const SkFont *pFontHandle = pSkiaFont->GetFontHandle();
    if (pFontHandle == nullptr) {
        return false;
    }

    IFont *pResolvedFont = const_cast<IFont *>(pFont);
    uint32_t nResolvedChar = unicodeChar;
    SkGlyphID glyphId = pFontHandle->unicharToGlyph((SkUnichar) nResolvedChar);
    if (glyphId == 0) {
        IFallbackFontMgr *pFallbackFontMgr = DrawSkiaText::GetFallbackFontMgr(pFont);
        if (pFallbackFontMgr != nullptr) {
            uint16_t nFallbackGlyphId = 0;
            IFont *pFallbackFont
                = pFallbackFontMgr->CreateFallbackFont(pFont, nResolvedChar, &nFallbackGlyphId);
            if ((pFallbackFont != nullptr) && (nFallbackGlyphId != 0)) {
                pResolvedFont = pFallbackFont;
                glyphId = nFallbackGlyphId;
            }
        }
    }
    if ((glyphId == 0) && bUseDefaultCharWhenFailed) {
        nResolvedChar = (uint32_t) 'A';
        glyphId = pFontHandle->unicharToGlyph((SkUnichar) nResolvedChar);
    }
    if (glyphId == 0) {
        glyph.m_bMissing = true;
        return false;
    }

    SkRect bounds = SkRect::MakeEmpty();
    const float fAdvance = DrawSkiaText::MeasureTextChar(
        *pFontHandle,
        nResolvedChar,
        &bounds,
        m_pRender->m_pSkPaint.get(),
        pFont,
        bUseDefaultCharWhenFailed);
    glyph.m_pFont = pResolvedFont;
    glyph.m_glyphId = glyphId;
    glyph.m_unicodeChar = nResolvedChar;
    glyph.m_fAdvance = fAdvance;
    glyph.m_bounds = UiRectF(bounds.left(), bounds.top(), bounds.right(), bounds.bottom());
    glyph.m_bMissing = false;
    return true;
}

void SkiaTextShaper::DrawGlyph(
    const TextGlyphInfo &glyph, float x, float y, UiColor textColor, uint8_t uFade)
{
    if ((m_pRender == nullptr) || (glyph.m_pFont == nullptr) || (glyph.m_glyphId == 0)) {
        return;
    }
    Font_Skia *pSkiaFont = dynamic_cast<Font_Skia *>(glyph.m_pFont);
    if (pSkiaFont == nullptr) {
        return;
    }
    const SkFont *pFontHandle = pSkiaFont->GetFontHandle();
    SkCanvas *pCanvas = m_pRender->GetSkCanvas();
    if ((pFontHandle == nullptr) || (pCanvas == nullptr)) {
        return;
    }
    SkPaint skPaint = *m_pRender->m_pSkPaint;
    const uint8_t nAlpha = (uint8_t) ((uint32_t) textColor.GetAlpha() * uFade / 255);
    skPaint.setColor(
        SkColorSetARGB(nAlpha, textColor.GetRed(), textColor.GetGreen(), textColor.GetBlue()));
    const SkPoint &ptOrg = m_pRender->GetPointOrg();
    DrawSkiaText::DrawSimpleText(
        pCanvas,
        glyph.m_unicodeChar,
        x + ptOrg.x(),
        y + ptOrg.y(),
        *pFontHandle,
        skPaint,
        glyph.m_pFont);
}

} // namespace ui
