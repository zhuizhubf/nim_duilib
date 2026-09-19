#include "text/TextLayout.h"

#include <algorithm>
#include <cmath>
#include <cwctype>
#include <limits>
#include <vector>

namespace ui {

namespace {
struct LayoutGlyph
{
    size_t m_nRunIndex = 0;
    IFont *m_pFont = nullptr;
    UiColor m_textColor;
    TextGlyphInfo m_glyph;
    uint32_t m_unicodeChar = 0;
    bool m_bNewLine = false;
    float m_x = 0.0f;
    float m_y = 0.0f;
};

struct LayoutLine
{
    std::vector<LayoutGlyph> m_glyphs;
    float m_fWidth = 0.0f;
    float m_fHeight = 0.0f;
};

std::vector<uint32_t> ToUTF32(const DString &strText)
{
    std::vector<uint32_t> text;
    text.reserve(strText.size());
    using CharType = DString::value_type;
    if (sizeof(CharType) == 1) {
        const uint8_t *pData = (const uint8_t *) strText.c_str();
        const size_t nLen = strText.size();
        size_t i = 0;
        while (i < nLen) {
            uint32_t ch = pData[i++];
            if ((ch & 0x80) == 0) {
                text.push_back(ch);
            } else if ((ch & 0xE0) == 0xC0) {
                ch = ((ch & 0x1F) << 6) | (pData[i++] & 0x3F);
                text.push_back(ch);
            } else if ((ch & 0xF0) == 0xE0) {
                ch = ((ch & 0x0F) << 12) | ((pData[i++] & 0x3F) << 6) | (pData[i++] & 0x3F);
                text.push_back(ch);
            } else if ((ch & 0xF8) == 0xF0) {
                ch = ((ch & 0x07) << 18) | ((pData[i++] & 0x3F) << 12) | ((pData[i++] & 0x3F) << 6)
                     | (pData[i++] & 0x3F);
                text.push_back(ch);
            }
        }
    } else if (sizeof(CharType) == 2) {
        const uint16_t *pData = (const uint16_t *) strText.c_str();
        const size_t nLen = strText.size();
        for (size_t i = 0; i < nLen; ++i) {
            uint32_t ch = pData[i];
            if ((ch >= 0xD800) && (ch <= 0xDBFF) && ((i + 1) < nLen)) {
                const uint32_t low = pData[i + 1];
                if ((low >= 0xDC00) && (low <= 0xDFFF)) {
                    ch = 0x10000 + ((ch - 0xD800) << 10) + (low - 0xDC00);
                    ++i;
                }
            }
            text.push_back(ch);
        }
    } else {
        for (CharType ch : strText) {
            text.push_back((uint32_t) ch);
        }
    }
    return text;
}

bool IsLineBreakChar(uint32_t ch)
{
    return (ch == '\r') || (ch == '\n');
}

bool IsWhiteSpace(uint32_t ch)
{
    return (ch <= 32) || std::iswspace((wint_t) ch);
}

bool IsWordBreakChar(uint32_t ch)
{
    if (ch <= 255) {
        return !std::iswalnum((wint_t) ch);
    }
    return IsWhiteSpace(ch);
}

void ResolveLineHeight(
    ITextShaper &textShaper,
    const DrawStringParam &drawParam,
    float &fLineHeight,
    float &fFontHeight)
{
    TextFontMetrics metrics;
    if (textShaper.GetFontMetrics(drawParam.pFont, metrics)) {
        fFontHeight = metrics.m_fHeight;
    } else {
        fFontHeight = (float) drawParam.pFont->FontSize();
    }
    fLineHeight = fFontHeight * drawParam.fSpacingMul + drawParam.fSpacingAdd;
    if (fLineHeight <= 0.0f) {
        fLineHeight = fFontHeight;
    }
}

std::vector<LayoutGlyph> BuildGlyphs(
    ITextShaper &textShaper,
    size_t nRunIndex,
    IFont *pFont,
    UiColor textColor,
    const DString &strText,
    const DrawStringParam &drawParam)
{
    std::vector<LayoutGlyph> glyphs;
    const std::vector<uint32_t> text = ToUTF32(strText);
    glyphs.reserve(text.size());
    for (uint32_t ch : text) {
        if (IsLineBreakChar(ch)) {
            LayoutGlyph glyph;
            glyph.m_nRunIndex = nRunIndex;
            glyph.m_pFont = pFont;
            glyph.m_textColor = textColor;
            glyph.m_unicodeChar = ch;
            glyph.m_bNewLine = true;
            glyphs.push_back(glyph);
            continue;
        }
        TextGlyphInfo glyphInfo;
        if (!textShaper.ResolveGlyph(pFont, ch, glyphInfo, true)) {
            continue;
        }
        LayoutGlyph glyph;
        glyph.m_nRunIndex = nRunIndex;
        glyph.m_pFont = glyphInfo.m_pFont;
        glyph.m_textColor = textColor;
        glyph.m_glyph = glyphInfo;
        glyph.m_unicodeChar = ch;
        glyphs.push_back(glyph);
    }
    return glyphs;
}

std::vector<LayoutLine> WrapLines(
    const std::vector<LayoutGlyph> &glyphs,
    int32_t nMaxWidth,
    bool bSingleLine,
    float fLineHeight,
    float fWordSpacing)
{
    std::vector<LayoutLine> lines;
    LayoutLine currentLine;
    currentLine.m_fHeight = fLineHeight;
    float x = 0.0f;
    size_t nLastBreakIndex = (std::numeric_limits<size_t>::max)();
    for (const LayoutGlyph &glyph : glyphs) {
        if (glyph.m_bNewLine) {
            lines.push_back(currentLine);
            currentLine = LayoutLine();
            currentLine.m_fHeight = fLineHeight;
            x = 0.0f;
            nLastBreakIndex = (std::numeric_limits<size_t>::max)();
            if (bSingleLine) {
                break;
            }
            continue;
        }
        const float fAdvance = glyph.m_glyph.m_fAdvance + fWordSpacing;
        if (!bSingleLine && (nMaxWidth > 0) && (x > 0.0f) && ((x + fAdvance) > (float) nMaxWidth)) {
            if ((nLastBreakIndex != (std::numeric_limits<size_t>::max)())
                && (nLastBreakIndex + 1 < currentLine.m_glyphs.size())) {
                //优先按单词边界换行，将断点后的字符移到下一行
                std::vector<LayoutGlyph> carryGlyphs(
                    currentLine.m_glyphs.begin() + nLastBreakIndex + 1, currentLine.m_glyphs.end());
                currentLine.m_glyphs.resize(nLastBreakIndex + 1);
                currentLine.m_fWidth = x; // 当前行宽度稍后重算
                lines.push_back(currentLine);

                currentLine = LayoutLine();
                currentLine.m_fHeight = fLineHeight;
                x = 0.0f;
                for (LayoutGlyph carryGlyph : carryGlyphs) {
                    carryGlyph.m_x = x;
                    x += carryGlyph.m_glyph.m_fAdvance + fWordSpacing;
                    currentLine.m_glyphs.push_back(carryGlyph);
                }
                currentLine.m_fWidth = x;
            } else {
                lines.push_back(currentLine);
                currentLine = LayoutLine();
                currentLine.m_fHeight = fLineHeight;
                x = 0.0f;
            }
            nLastBreakIndex = (std::numeric_limits<size_t>::max)();
        }
        LayoutGlyph item = glyph;
        item.m_x = x;
        item.m_y = 0.0f;
        currentLine.m_glyphs.push_back(item);
        x += fAdvance;
        currentLine.m_fWidth = x;
        if (IsWordBreakChar(glyph.m_unicodeChar)) {
            nLastBreakIndex = currentLine.m_glyphs.size() - 1;
        }
    }
    if (!currentLine.m_glyphs.empty() || lines.empty()) {
        lines.push_back(currentLine);
    }
    return lines;
}

std::vector<LayoutLine> WrapVerticalLines(
    const std::vector<LayoutGlyph> &glyphs, int32_t nMaxHeight, float fColumnWidth)
{
    std::vector<LayoutLine> lines;
    LayoutLine currentLine;
    currentLine.m_fWidth = fColumnWidth;
    float y = 0.0f;
    for (const LayoutGlyph &glyph : glyphs) {
        if (glyph.m_bNewLine) {
            lines.push_back(currentLine);
            currentLine = LayoutLine();
            currentLine.m_fWidth = fColumnWidth;
            y = 0.0f;
            continue;
        }
        const float fAdvance = glyph.m_glyph.m_fAdvance;
        if ((nMaxHeight > 0) && (y > 0.0f) && ((y + fAdvance) > (float) nMaxHeight)) {
            lines.push_back(currentLine);
            currentLine = LayoutLine();
            currentLine.m_fWidth = fColumnWidth;
            y = 0.0f;
        }
        LayoutGlyph item = glyph;
        item.m_x = 0.0f;
        item.m_y = y;
        currentLine.m_glyphs.push_back(item);
        y += fAdvance;
        currentLine.m_fHeight = y;
    }
    if (!currentLine.m_glyphs.empty() || lines.empty()) {
        lines.push_back(currentLine);
    }
    return lines;
}

float CalculateTotalHeight(const std::vector<LayoutLine> &lines, float fLineHeight)
{
    if (lines.empty()) {
        return fLineHeight;
    }
    return fLineHeight * (float) lines.size();
}

void AppendEllipsis(
    ITextShaper &textShaper,
    std::vector<LayoutGlyph> &glyphs,
    IFont *pFont,
    UiColor textColor,
    float fWidth)
{
    const uint32_t dots[] = {'.', '.', '.'};
    float fDotsWidth = 0.0f;
    std::vector<LayoutGlyph> ellipsis;
    for (uint32_t ch : dots) {
        TextGlyphInfo glyphInfo;
        if (textShaper.ResolveGlyph(pFont, ch, glyphInfo, true)) {
            LayoutGlyph glyph;
            glyph.m_pFont = glyphInfo.m_pFont;
            glyph.m_textColor = textColor;
            glyph.m_glyph = glyphInfo;
            glyph.m_unicodeChar = ch;
            ellipsis.push_back(glyph);
            fDotsWidth += glyphInfo.m_fAdvance;
        }
    }
    float fTextWidth = 0.0f;
    size_t nKeepCount = glyphs.size();
    for (size_t i = 0; i < glyphs.size(); ++i) {
        fTextWidth += glyphs[i].m_glyph.m_fAdvance;
        if ((fTextWidth + fDotsWidth) > fWidth) {
            nKeepCount = i;
            break;
        }
    }
    glyphs.resize(nKeepCount);
    float x = fTextWidth - (nKeepCount > 0 ? glyphs.back().m_glyph.m_fAdvance : 0.0f);
    for (LayoutGlyph &glyph : ellipsis) {
        glyph.m_x = x;
        x += glyph.m_glyph.m_fAdvance;
        glyphs.push_back(glyph);
    }
}

void AppendPathEllipsis(
    ITextShaper &textShaper,
    std::vector<LayoutGlyph> &glyphs,
    IFont *pFont,
    UiColor textColor,
    float fWidth)
{
    float fTextWidth = 0.0f;
    for (const LayoutGlyph &glyph : glyphs) {
        fTextWidth += glyph.m_glyph.m_fAdvance;
    }
    if (fTextWidth <= fWidth) {
        return;
    }

    std::vector<LayoutGlyph> ellipsis;
    const uint32_t dots[] = {'.', '.', '.'};
    float fDotsWidth = 0.0f;
    for (uint32_t ch : dots) {
        TextGlyphInfo glyphInfo;
        if (textShaper.ResolveGlyph(pFont, ch, glyphInfo, true)) {
            LayoutGlyph glyph;
            glyph.m_pFont = glyphInfo.m_pFont;
            glyph.m_textColor = textColor;
            glyph.m_glyph = glyphInfo;
            glyph.m_unicodeChar = ch;
            ellipsis.push_back(glyph);
            fDotsWidth += glyphInfo.m_fAdvance;
        }
    }
    const float fRemainWidth = std::max(0.0f, fWidth - fDotsWidth);
    const float fHeadWidth = fRemainWidth / 2.0f;
    const float fTailWidth = fRemainWidth - fHeadWidth;

    size_t nHeadCount = 0;
    float fHeadActual = 0.0f;
    while ((nHeadCount < glyphs.size())
           && ((fHeadActual + glyphs[nHeadCount].m_glyph.m_fAdvance) <= fHeadWidth)) {
        fHeadActual += glyphs[nHeadCount].m_glyph.m_fAdvance;
        ++nHeadCount;
    }

    size_t nTailCount = 0;
    float fTailActual = 0.0f;
    while ((nTailCount < (glyphs.size() - nHeadCount))
           && ((fTailActual + glyphs[glyphs.size() - 1 - nTailCount].m_glyph.m_fAdvance)
               <= fTailWidth)) {
        fTailActual += glyphs[glyphs.size() - 1 - nTailCount].m_glyph.m_fAdvance;
        ++nTailCount;
    }

    std::vector<LayoutGlyph> newGlyphs;
    newGlyphs.reserve(nHeadCount + ellipsis.size() + nTailCount);
    float x = 0.0f;
    for (size_t i = 0; i < nHeadCount; ++i) {
        LayoutGlyph glyph = glyphs[i];
        glyph.m_x = x;
        x += glyph.m_glyph.m_fAdvance;
        newGlyphs.push_back(glyph);
    }
    for (LayoutGlyph glyph : ellipsis) {
        glyph.m_x = x;
        x += glyph.m_glyph.m_fAdvance;
        newGlyphs.push_back(glyph);
    }
    const size_t nTailStart = glyphs.size() - nTailCount;
    for (size_t i = nTailStart; i < glyphs.size(); ++i) {
        LayoutGlyph glyph = glyphs[i];
        glyph.m_x = x;
        x += glyph.m_glyph.m_fAdvance;
        newGlyphs.push_back(glyph);
    }
    glyphs.swap(newGlyphs);
}

void FillLineInfo(
    const std::vector<LayoutLine> &lines,
    const UiRect &textRect,
    float fLineHeight,
    RichTextLineInfoParam *pLineInfoParam)
{
    if (pLineInfoParam == nullptr) {
        return;
    }
    if (pLineInfoParam->m_pLineInfoList == nullptr) {
        return;
    }
    RichTextLineInfoList &lineList = *pLineInfoParam->m_pLineInfoList;
    lineList.clear();
    for (size_t i = 0; i < lines.size(); ++i) {
        RichTextLineInfoPtr pLineInfo(new RichTextLineInfo);
        RichTextRowInfoPtr pRowInfo(new RichTextRowInfo);
        DStringW lineText;
        pRowInfo->m_rowRect = UiRectF(
            (float) textRect.left,
            (float) textRect.top + fLineHeight * (float) i,
            (float) textRect.left + lines[i].m_fWidth,
            (float) textRect.top + fLineHeight * (float) (i + 1));
        pRowInfo->m_xOffset = 0;
        for (const LayoutGlyph &glyph : lines[i].m_glyphs) {
            RichTextCharInfo charInfo;
            charInfo.SetCharWidth(glyph.m_glyph.m_fAdvance);
            pRowInfo->m_charInfo.push_back(charInfo);
            if ((glyph.m_unicodeChar >= 0xD800) && (glyph.m_unicodeChar <= 0xDBFF)) {
                lineText.push_back((DStringW::value_type) glyph.m_unicodeChar);
            } else if (glyph.m_unicodeChar <= 0xFFFF) {
                lineText.push_back((DStringW::value_type) glyph.m_unicodeChar);
            }
        }
        pLineInfo->m_lineText = lineText;
        pLineInfo->m_nLineTextLen = (uint32_t) lineText.size();
        pLineInfo->m_rowInfo.push_back(pRowInfo);
        lineList.push_back(pLineInfo);
    }
}

struct RichLayoutResult
{
    std::vector<LayoutGlyph> m_glyphs;
    std::vector<LayoutLine> m_lines;
    float m_fLineHeight = 0.0f;
    std::vector<std::vector<UiRect>> m_runRects;
};

void BuildRichLayout(
    ITextShaper &textShaper,
    const UiRect &textRect,
    const std::vector<RichTextData> &richTextData,
    RichLayoutResult &result)
{
    result.m_runRects.clear();
    result.m_runRects.resize(richTextData.size());
    for (size_t i = 0; i < richTextData.size(); ++i) {
        const RichTextData &data = richTextData[i];
        if ((data.m_pFontInfo == nullptr) || data.m_textView.empty()) {
            continue;
        }
        IFont *pRunFont = textShaper.CreateFont(*data.m_pFontInfo);
        if (pRunFont == nullptr) {
            continue;
        }
        TextFontMetrics metrics;
        textShaper.GetFontMetrics(pRunFont, metrics);
        const float fRunLineHeight = metrics.m_fHeight * data.m_fRowSpacingMul
                                     + data.m_fRowSpacingAdd;
        result.m_fLineHeight = std::max(result.m_fLineHeight, fRunLineHeight);
        const std::vector<uint32_t> text = ToUTF32(
            DString(data.m_textView.data(), data.m_textView.size()));
        for (uint32_t ch : text) {
            if (IsLineBreakChar(ch)) {
                LayoutGlyph glyph;
                glyph.m_nRunIndex = i;
                glyph.m_bNewLine = true;
                result.m_glyphs.push_back(glyph);
                continue;
            }
            TextGlyphInfo glyphInfo;
            if (!textShaper.ResolveGlyph(pRunFont, ch, glyphInfo, true)) {
                continue;
            }
            LayoutGlyph glyph;
            glyph.m_nRunIndex = i;
            glyph.m_pFont = glyphInfo.m_pFont;
            glyph.m_textColor = data.m_textColor;
            glyph.m_glyph = glyphInfo;
            glyph.m_unicodeChar = ch;
            result.m_glyphs.push_back(glyph);
        }
    }
    if (result.m_fLineHeight <= 0.0f) {
        result.m_fLineHeight = (float) std::max(1, textRect.Height());
    }
    result.m_lines = WrapLines(result.m_glyphs, textRect.Width(), false, result.m_fLineHeight, 0.0f);

    //按行计算每个文本片段的矩形范围：
    //这里的top/bottom使用"行"的真实范围，不能直接使用textRect的top/bottom。
    //因为调用方在估算文本大小时，传入的textRect.bottom可能是INT_MAX（表示高度不限），
    //如果直接使用，会导致估算出的文本高度为INT_MAX，进而引起滚动条和布局反复变化。
    float fLineTop = (float) textRect.top;
    for (const LayoutLine &line : result.m_lines) {
        const float fLineBottom = fLineTop + result.m_fLineHeight;
        for (const LayoutGlyph &glyph : line.m_glyphs) {
            if (glyph.m_nRunIndex < result.m_runRects.size()) {
                const float fLeft = (float) textRect.left + glyph.m_x;
                const float fRight = fLeft + glyph.m_glyph.m_fAdvance;
                result.m_runRects[glyph.m_nRunIndex].push_back(UiRect(
                    (int32_t) fLeft, (int32_t) fLineTop, (int32_t) fRight, (int32_t) fLineBottom));
            }
        }
        fLineTop = fLineBottom;
    }
}
} // namespace

bool TextLayout::IsRichTextDataEqual(
    const std::vector<RichTextData> &first, const std::vector<RichTextData> &second)
{
    if (first.size() != second.size()) {
        return false;
    }
    for (size_t i = 0; i < first.size(); ++i) {
        const RichTextData &v1 = first[i];
        const RichTextData &v2 = second[i];
        if ((v1.m_textView.data() != v2.m_textView.data())
            || (v1.m_textView.size() != v2.m_textView.size())) {
            return false;
        }
        if ((v1.m_textColor != v2.m_textColor) || (v1.m_bgColor != v2.m_bgColor)) {
            return false;
        }
        if ((v1.m_pFontInfo == nullptr) || (v2.m_pFontInfo == nullptr)) {
            if (v1.m_pFontInfo != v2.m_pFontInfo) {
                return false;
            }
        } else if (*v1.m_pFontInfo != *v2.m_pFontInfo) {
            return false;
        }
        if ((v1.m_fRowSpacingMul != v2.m_fRowSpacingMul)
            || (v1.m_fRowSpacingAdd != v2.m_fRowSpacingAdd) || (v1.m_textStyle != v2.m_textStyle)) {
            return false;
        }
    }
    return true;
}

UiRect TextLayout::MeasureString(
    ITextShaper &textShaper, const DString &strText, const MeasureStringParam &measureParam)
{
    if ((measureParam.pFont == nullptr) || strText.empty()) {
        return UiRect();
    }
    DrawStringParam drawParam;
    drawParam.pFont = measureParam.pFont;
    drawParam.uFormat = measureParam.uFormat;
    drawParam.fSpacingMul = measureParam.fSpacingMul;
    drawParam.fSpacingAdd = measureParam.fSpacingAdd;
    drawParam.fWordSpacing = measureParam.fWordSpacing;
    const std::vector<LayoutGlyph> glyphs
        = BuildGlyphs(textShaper, 0, measureParam.pFont, UiColor(), strText, drawParam);
    float fLineHeight = 0.0f;
    float fFontHeight = 0.0f;
    ResolveLineHeight(textShaper, drawParam, fLineHeight, fFontHeight);
    const bool bSingleLine = ((measureParam.uFormat & DrawStringFormat::TEXT_SINGLELINE) != 0)
                             || ((measureParam.uFormat & DrawStringFormat::TEXT_VERTICAL) == 0
                                 && measureParam.rectSize <= 0);
    const bool bVertical = (measureParam.uFormat & DrawStringFormat::TEXT_VERTICAL) != 0;
    const std::vector<LayoutLine> lines
        = bVertical ? WrapVerticalLines(glyphs, measureParam.rectSize, fLineHeight)
                    : WrapLines(
                          glyphs,
                          measureParam.rectSize,
                          bSingleLine,
                          fLineHeight,
                          measureParam.fWordSpacing);
    float fWidth = 0.0f;
    for (const LayoutLine &line : lines) {
        fWidth = std::max(fWidth, line.m_fWidth);
    }
    float fHeight = 0.0f;
    if (bVertical) {
        for (const LayoutLine &line : lines) {
            fHeight = std::max(fHeight, line.m_fHeight);
        }
    } else {
        fHeight = CalculateTotalHeight(lines, fLineHeight);
    }
    return UiRect(0, 0, (int32_t) std::ceil(fWidth), (int32_t) std::ceil(fHeight));
}

void TextLayout::DrawString(
    ITextShaper &textShaper,
    IRender *pRender,
    const DString &strText,
    const DrawStringParam &drawParam)
{
    if ((pRender == nullptr) || (drawParam.pFont == nullptr) || strText.empty()) {
        return;
    }
    const std::vector<LayoutGlyph> glyphs
        = BuildGlyphs(textShaper, 0, drawParam.pFont, drawParam.dwTextColor, strText, drawParam);
    float fLineHeight = 0.0f;
    float fFontHeight = 0.0f;
    ResolveLineHeight(textShaper, drawParam, fLineHeight, fFontHeight);
    const bool bVertical = (drawParam.uFormat & DrawStringFormat::TEXT_VERTICAL) != 0;
    const bool bSingleLine = (drawParam.uFormat & DrawStringFormat::TEXT_SINGLELINE) != 0;
    std::vector<LayoutLine> lines
        = bVertical ? WrapVerticalLines(glyphs, drawParam.textRect.Height(), fLineHeight)
                    : WrapLines(
                          glyphs,
                          drawParam.textRect.Width(),
                          bSingleLine,
                          fLineHeight,
                          drawParam.fWordSpacing);
    const bool bEndEllipsis = (drawParam.uFormat & DrawStringFormat::TEXT_END_ELLIPSIS) != 0;
    const bool bPathEllipsis = (drawParam.uFormat & DrawStringFormat::TEXT_PATH_ELLIPSIS) != 0;
    if ((bEndEllipsis || bPathEllipsis) && (lines.size() == 1)
        && (lines[0].m_fWidth > (float) drawParam.textRect.Width())) {
        if (bPathEllipsis) {
            AppendPathEllipsis(
                textShaper,
                lines[0].m_glyphs,
                drawParam.pFont,
                drawParam.dwTextColor,
                (float) drawParam.textRect.Width());
        } else {
            AppendEllipsis(
                textShaper,
                lines[0].m_glyphs,
                drawParam.pFont,
                drawParam.dwTextColor,
                (float) drawParam.textRect.Width());
        }
        lines[0].m_fWidth = (float) drawParam.textRect.Width();
    }
    const int32_t nClipState = ((drawParam.uFormat & DrawStringFormat::TEXT_NOCLIP) != 0)
                                   ? -1
                                   : pRender->SetClip(drawParam.textRect, true);
    if (bVertical) {
        const float fTotalWidth = fLineHeight * (float) lines.size();
        float fStartX = (float) drawParam.textRect.right - fLineHeight;
        if ((drawParam.uFormat & DrawStringFormat::TEXT_LEFT) != 0) {
            fStartX = (float) drawParam.textRect.left;
        } else if ((drawParam.uFormat & DrawStringFormat::TEXT_HCENTER) != 0) {
            fStartX = (float) drawParam.textRect.left
                      + ((float) drawParam.textRect.Width() - fTotalWidth) / 2.0f;
        }
        for (size_t i = 0; i < lines.size(); ++i) {
            const LayoutLine &line = lines[i];
            const float fColumnX = ((drawParam.uFormat & DrawStringFormat::TEXT_LEFT) != 0)
                                       ? (fStartX + fLineHeight * (float) i)
                                       : (fStartX - fLineHeight * (float) i);
            float fStartY = (float) drawParam.textRect.top;
            if ((drawParam.uFormat & DrawStringFormat::TEXT_VCENTER) != 0) {
                fStartY += ((float) drawParam.textRect.Height() - line.m_fHeight) / 2.0f;
            } else if ((drawParam.uFormat & DrawStringFormat::TEXT_BOTTOM) != 0) {
                fStartY += (float) drawParam.textRect.Height() - line.m_fHeight;
            }
            for (const LayoutGlyph &glyph : line.m_glyphs) {
                textShaper.DrawGlyph(
                    glyph.m_glyph,
                    fColumnX + fLineHeight * 0.2f,
                    fStartY + glyph.m_y + fFontHeight * 0.8f,
                    drawParam.dwTextColor,
                    drawParam.uFade);
            }
        }
    } else {
        const float fTotalHeight = CalculateTotalHeight(lines, fLineHeight);
        float fStartY = (float) drawParam.textRect.top;
        if ((drawParam.uFormat & DrawStringFormat::TEXT_VCENTER) != 0) {
            fStartY += ((float) drawParam.textRect.Height() - fTotalHeight) / 2.0f;
        } else if ((drawParam.uFormat & DrawStringFormat::TEXT_BOTTOM) != 0) {
            fStartY += (float) drawParam.textRect.Height() - fTotalHeight;
        }
        for (size_t i = 0; i < lines.size(); ++i) {
            const LayoutLine &line = lines[i];
            float fStartX = (float) drawParam.textRect.left;
            if ((drawParam.uFormat & DrawStringFormat::TEXT_HCENTER) != 0) {
                fStartX += ((float) drawParam.textRect.Width() - line.m_fWidth) / 2.0f;
            } else if ((drawParam.uFormat & DrawStringFormat::TEXT_RIGHT) != 0) {
                fStartX += (float) drawParam.textRect.Width() - line.m_fWidth;
            }
            const float fY = fStartY + fLineHeight * (float) i + fFontHeight * 0.8f;
            for (const LayoutGlyph &glyph : line.m_glyphs) {
                textShaper.DrawGlyph(
                    glyph.m_glyph, fStartX + glyph.m_x, fY, drawParam.dwTextColor, drawParam.uFade);
            }
        }
    }
    if (nClipState >= 0) {
        pRender->ClearClip(nClipState);
    }
}

void TextLayout::MeasureRichText(
    ITextShaper &textShaper,
    const UiRect &textRect,
    const UiSize & /*szScrollOffset*/,
    const std::vector<RichTextData> &richTextData,
    std::vector<std::vector<UiRect>> *pRichTextRects)
{
    RichLayoutResult result;
    BuildRichLayout(textShaper, textRect, richTextData, result);
    if (pRichTextRects != nullptr) {
        *pRichTextRects = result.m_runRects;
    }
}

void TextLayout::MeasureRichText2(
    ITextShaper &textShaper,
    const UiRect &textRect,
    const UiSize &szScrollOffset,
    const std::vector<RichTextData> &richTextData,
    RichTextLineInfoParam *pLineInfoParam,
    std::vector<std::vector<UiRect>> *pRichTextRects)
{
    RichLayoutResult result;
    BuildRichLayout(textShaper, textRect, richTextData, result);
    if (pRichTextRects != nullptr) {
        *pRichTextRects = result.m_runRects;
    }
    if (pLineInfoParam != nullptr) {
        FillLineInfo(result.m_lines, textRect, result.m_fLineHeight, pLineInfoParam);
    }
}

void TextLayout::DrawRichText(
    ITextShaper &textShaper,
    IRender *pRender,
    const UiRect &textRect,
    const UiSize & /*szScrollOffset*/,
    const std::vector<RichTextData> &richTextData,
    uint8_t uFade,
    std::vector<std::vector<UiRect>> *pRichTextRects)
{
    RichLayoutResult result;
    BuildRichLayout(textShaper, textRect, richTextData, result);
    if (pRichTextRects != nullptr) {
        *pRichTextRects = result.m_runRects;
    }
    float y = (float) textRect.top;
    for (const LayoutLine &line : result.m_lines) {
        for (const LayoutGlyph &glyph : line.m_glyphs) {
            textShaper.DrawGlyph(
                glyph.m_glyph,
                (float) textRect.left + glyph.m_x,
                y + result.m_fLineHeight * 0.8f,
                glyph.m_textColor,
                uFade);
        }
        y += result.m_fLineHeight;
    }
}

} // namespace ui
