#include "DrawSkiaText.h"
#include "duilib/RenderSkia/Font_Skia.h"
#include "duilib/RenderSkia/SkUtils.h"
#include "duilib/Utils/StringConvert.h"
#include "duilib/Utils/PerformanceUtil.h"

namespace ui
{

IFallbackFontMgr* DrawSkiaText::GetFallbackFontMgr(const IFont* pFont)
{
    IFallbackFontMgr* pFallbackFontMgr = nullptr;
    if (pFont != nullptr) {
        IFontMgr* pFontMgr = nullptr;
        const Font_Skia* pSkiaFont = dynamic_cast<const Font_Skia*>(pFont);
        ASSERT(pSkiaFont != nullptr);
        if (pSkiaFont != nullptr) {
            pFontMgr = pSkiaFont->GetFontMgr();
        }
        if (pFontMgr != nullptr) {
            pFallbackFontMgr = pFontMgr->GetFallbackFontMgr();
        }
    }
    return pFallbackFontMgr;
}

const SkFont* DrawSkiaText::CreateFallbackFont(const IFont* pFont, SkUnichar unicodeChar, SkGlyphID* glyphId)
{
    IFont* pFallbackFont = nullptr;
    IFallbackFontMgr* pFallbackFontMgr = GetFallbackFontMgr(pFont);
    if (pFallbackFontMgr != nullptr) {
        ASSERT(sizeof(SkUnichar) == sizeof(uint32_t));
        pFallbackFont = pFallbackFontMgr->CreateFallbackFont(pFont, (uint32_t)unicodeChar, glyphId);
    }

    const SkFont* pFallbackSkFont = nullptr;
    if (pFallbackFont != nullptr) {
        Font_Skia* pSkiaFont = dynamic_cast<Font_Skia*>(pFallbackFont);
        ASSERT(pSkiaFont != nullptr);
        if (pSkiaFont != nullptr) {
            pFallbackSkFont = pSkiaFont->GetFontHandle();
            ASSERT(pFallbackSkFont != nullptr);
        }
    }
    return pFallbackSkFont;
}


UTF32String DrawSkiaText::GetDrawStringUTF32(const void* text, size_t byteLength, SkTextEncoding textEncoding)
{
    if ((text == nullptr) || (byteLength == 0)) {
        return UTF32String();
    }
    if (textEncoding == SkTextEncoding::kUTF8) {
        return StringConvert::UTF8ToUTF32((const DUTF8Char*)text, byteLength / sizeof(DUTF8Char));
    }
    else if (textEncoding == SkTextEncoding::kUTF16) {
        ASSERT(byteLength % sizeof(DUTF16Char) == 0);
        return StringConvert::UTF16ToUTF32((const DUTF16Char*)text, byteLength / sizeof(DUTF16Char));
    }
    else if (textEncoding == SkTextEncoding::kUTF32) {
        ASSERT(byteLength % sizeof(DUTF32Char) == 0);
        return UTF32String((const DUTF32Char*)text, byteLength / sizeof(DUTF32Char));
    }
    ASSERT(0);
    return UTF32String();
}

SkScalar DrawSkiaText::MeasureText(const SkFont& font, DUTF32Char ch,
                                   SkRect* bounds, const SkPaint* paint,
                                   const IFont* pFont)
{
    if (pFont != nullptr) {
        FallbackFontCreator fallbackFontCreator = [pFont](SkUnichar unicodeChar, SkGlyphID* glyphId) {
            return DrawSkiaText::CreateFallbackFont(pFont, unicodeChar, glyphId);
            };
        return MeasureText(font, ch, bounds, paint, fallbackFontCreator);
    }
    else if (font.unicharToGlyph((SkUnichar)ch) != 0) {
        return font.measureText(&ch, sizeof(ch), SkTextEncoding::kUTF32, bounds, paint);
    }
    return 0.0f;
}

SkScalar DrawSkiaText::MeasureText(const SkFont& font, DUTF32Char ch,
                                   SkRect* bounds, const SkPaint* paint,
                                   FallbackFontCreator fallbackFontCreator)
{
    SkGlyphID glyphId = font.unicharToGlyph((SkUnichar)ch);
    if (glyphId != 0) {
        return font.measureText(&glyphId, sizeof(SkGlyphID), SkTextEncoding::kGlyphID, bounds, paint);
    }
    else if (fallbackFontCreator != nullptr) {
        //当前设置的字体不支持这个字，需要使用回退字体
        glyphId = 0;
        const SkFont* pFallbackSkFont = fallbackFontCreator(ch, &glyphId);
        if (pFallbackSkFont != nullptr) {
            return pFallbackSkFont->measureText(&glyphId, sizeof(SkGlyphID), SkTextEncoding::kGlyphID, bounds, paint);
        }
    }
    return 0.0f;
}

SkScalar DrawSkiaText::MeasureText(const SkFont& font, const void* text, size_t byteLength, SkTextEncoding textEncoding,
                                   SkRect* bounds, const SkPaint* paint,
                                   const IFont* pFont,
                                   MeasureTextTempData& tempData)
{
    if (pFont != nullptr) {
        FallbackFontCreator fallbackFontCreator = [pFont](SkUnichar unicodeChar, SkGlyphID* glyphId) {
            return DrawSkiaText::CreateFallbackFont(pFont, unicodeChar, glyphId);
            };
        return MeasureText(font, text, byteLength, textEncoding, bounds, paint, fallbackFontCreator, tempData);
    }
    else {
        return font.measureText(text, byteLength, textEncoding, bounds, paint);
    }
}

/** 计算给定编码格式下一个字符的字节数
 * @param [in] textEncoding 文本编码格式
 * @return 字符字节数（UTF8:1, UTF16:2, UTF32:4）
 */
static inline size_t GetCharBytes(SkTextEncoding textEncoding)
{
    switch (textEncoding) {
    case SkTextEncoding::kUTF8:
        return 1;
    case SkTextEncoding::kUTF16:
        return 2;
    case SkTextEncoding::kUTF32:
        return 4;
    default:
        SkASSERT(false);
        break;
    }
    return 1;
}

SkScalar DrawSkiaText::MeasureText(const SkFont& font, const void* text, size_t byteLength, SkTextEncoding textEncoding,
                                   SkRect* bounds, const SkPaint* paint,
                                   FallbackFontCreator fallbackFontCreator,
                                   MeasureTextTempData& tempData)
{
    PerformanceStat statPerformance(_T("DrawSkiaText::MeasureText-str"));
    if ((text == nullptr) || (byteLength == 0)) {
        return 0.0f;
    }    
    if (fallbackFontCreator != nullptr) {
        //UTF32字符列表
        std::vector<SkUnichar>& unicharList = tempData.unicharList;
        unicharList.clear();
        unicharList.reserve(byteLength / GetCharBytes(textEncoding));

        EnumTextCallback enumTextCallback = [&](SkUnichar unicodeChar, size_t /*charByteLength*/) {
            unicharList.push_back(unicodeChar);
            return true;
            };
        SkiaTextData textData(text, byteLength, textEncoding);
        textData.EnumChars(enumTextCallback);
        if (unicharList.empty()) {
            return 0.0f;
        }

        //GlyphID列表
        std::vector<SkGlyphID>& glyphIDList = tempData.glyphIDList;
        glyphIDList.clear();
        glyphIDList.resize(unicharList.size());
        font.unicharsToGlyphs(SkSpan(unicharList.data(), unicharList.size()), SkSpan(glyphIDList.data(), glyphIDList.size()));

        bool bNeedFallback = false;
        for (SkGlyphID glyphID : glyphIDList) {
            if (glyphID == 0) {
                bNeedFallback = true;
                break;
            }
        }
        if (!bNeedFallback) {
            //不需要字体回退，直接计算
            return font.measureText(glyphIDList.data(), glyphIDList.size() * sizeof(SkGlyphID), SkTextEncoding::kGlyphID, bounds, paint);
        }

        //需要字体回退，拆分为两个部分（不需要字体回退的SkGlyphID列表，需要字体回退的UTF32字符列表）
        std::vector<SkGlyphID>& normalGlyphIDList = tempData.normalGlyphIDList;
        std::vector<SkUnichar>& fallbackUnicharList = tempData.fallbackUnicharList;
        normalGlyphIDList.clear();
        fallbackUnicharList.clear();
        normalGlyphIDList.reserve(unicharList.size());
        fallbackUnicharList.reserve(unicharList.size());
        const size_t nUnicharCount = unicharList.size();
        for (size_t nIndex = 0; nIndex < nUnicharCount; ++nIndex) {
            if (glyphIDList[nIndex] == 0) {
                fallbackUnicharList.push_back(unicharList[nIndex]);
            }
            else {
                normalGlyphIDList.push_back(glyphIDList[nIndex]);
            }
        }

        //先计算正常的字列表
        SkScalar fTotalWidth = 0;
        SkRect totalBounds;
        const bool bHasBounds = bounds != nullptr;
        SkRect* calcBounds = bHasBounds ? &totalBounds : nullptr;
        if (!normalGlyphIDList.empty()) {
            fTotalWidth = font.measureText(normalGlyphIDList.data(),
                                           normalGlyphIDList.size() * sizeof(SkGlyphID),
                                           SkTextEncoding::kGlyphID,
                                           calcBounds, paint);
        }

        //再计算需要字体回退的字符
        EnumTextCallback enumTextCallback2 = [&](SkUnichar unicodeChar, size_t /*charByteLength*/) {
            fTotalWidth += MeasureText(font, unicodeChar, calcBounds, paint, fallbackFontCreator);
            if (bHasBounds) {
                totalBounds.fTop = std::min(totalBounds.fTop, calcBounds->fTop);
                totalBounds.fBottom = std::max(totalBounds.fBottom, calcBounds->fBottom);
                totalBounds.fRight += std::max(0.0f, calcBounds->width());
            }
            return true;
            };

        SkiaTextData textData2(fallbackUnicharList.data(), fallbackUnicharList.size() * sizeof(SkUnichar), SkTextEncoding::kUTF32);
        textData2.EnumChars(enumTextCallback2);

        if (bounds != nullptr) {
            *bounds = totalBounds;
        }
        return fTotalWidth;
    }
    else {
        return font.measureText(text, byteLength, textEncoding, bounds, paint);
    }
}

SkScalar DrawSkiaText::DrawSimpleText(SkCanvas* skCanvas, DUTF32Char ch,
                                      SkScalar x, SkScalar y,
                                      const SkFont& font, const SkPaint& paint,
                                      const IFont* pFont)
{
    SkScalar fWidth = 0;
    if (pFont != nullptr) {
        FallbackFontCreator fallbackFontCreator = [pFont](SkUnichar unicodeChar, SkGlyphID* glyphId) {
            return DrawSkiaText::CreateFallbackFont(pFont, unicodeChar, glyphId);
            };
        fWidth = DrawSimpleText(skCanvas, ch, x, y, font, paint, fallbackFontCreator);
    }
    else {
        SkGlyphID glyphID = font.unicharToGlyph((SkUnichar)ch);
        if (glyphID != 0) {
            fWidth = font.getWidth(glyphID);
            skCanvas->drawSimpleText(&glyphID, sizeof(SkGlyphID), SkTextEncoding::kGlyphID, x, y, font, paint);
        }
    }
    return fWidth;
}

SkScalar DrawSkiaText::DrawSimpleText(SkCanvas* skCanvas, DUTF32Char ch, SkScalar x, SkScalar y,
                                      const SkFont& font, const SkPaint& paint,
                                      FallbackFontCreator fallbackFontCreator)
{
    SkScalar fWidth = 0;
    if (skCanvas == nullptr) {
        return fWidth;
    }
    SkGlyphID glyphID = font.unicharToGlyph((SkUnichar)ch);
    if (glyphID != 0) {
        fWidth = font.getWidth(glyphID);
        skCanvas->drawSimpleText(&glyphID, sizeof(SkGlyphID), SkTextEncoding::kGlyphID, x, y, font, paint);
    }
    else if (fallbackFontCreator != nullptr) {
        //该字体无法绘制，需要请求回退字体绘制
        glyphID = 0;
        const SkFont* pFallbackSkFont = fallbackFontCreator((uint32_t)ch, &glyphID);
        if (pFallbackSkFont != nullptr) {
            fWidth = pFallbackSkFont->getWidth(glyphID);
            skCanvas->drawSimpleText(&glyphID, sizeof(SkGlyphID), SkTextEncoding::kGlyphID, x, y, *pFallbackSkFont, paint);
        }
    }
    return fWidth;
}

void DrawSkiaText::DrawSimpleText(SkCanvas* skCanvas, const void* text, size_t byteLength, SkTextEncoding textEncoding,
                                      SkScalar x, SkScalar y,
                                      const SkFont& font, const SkPaint& paint,
                                      const IFont* pFont)
{
    if (pFont != nullptr) {
        FallbackFontCreator fallbackFontCreator = [pFont](SkUnichar unicodeChar, SkGlyphID* glyphId) {
            return DrawSkiaText::CreateFallbackFont(pFont, unicodeChar, glyphId);
            };
        return DrawSimpleText(skCanvas, text, byteLength, textEncoding, x, y, font, paint, fallbackFontCreator);
    }
    else {
        skCanvas->drawSimpleText(text, byteLength, textEncoding, x, y, font, paint);
    }
}

void DrawSkiaText::DrawSimpleText(SkCanvas* skCanvas, const void* text, size_t byteLength, SkTextEncoding textEncoding,
                                  SkScalar x, SkScalar y,
                                  const SkFont& font, const SkPaint& paint,
                                  FallbackFontCreator fallbackFontCreator)
{
    PerformanceStat statPerformance(_T("DrawSkiaText::DrawSimpleText"));
    if ((skCanvas == nullptr) || (text == nullptr) || (byteLength == 0)) {
        return;
    }
    if (fallbackFontCreator != nullptr) {
        UTF32String utf32 = GetDrawStringUTF32(text, byteLength, textEncoding);
        SkScalar fTotalWidth = 0;
        const size_t nCount = utf32.size();
        for (size_t nIndex = 0; nIndex < nCount; ++nIndex) {
            fTotalWidth += DrawSimpleText(skCanvas, utf32[nIndex], x + fTotalWidth, y, font, paint, fallbackFontCreator);
        }
    }
    else {
        skCanvas->drawSimpleText(text, byteLength, textEncoding, x, y, font, paint);
    }
}

size_t DrawSkiaText::BreakText(const void* text, size_t byteLength, SkTextEncoding textEncoding,
                               const SkFont& font, FallbackFontCreator fallbackFontCreator,
                               const SkPaint& paint, SkScalar maxWidth,
                               SkScalar* measuredWidth, SkScalar* measuredHeight,
                               MeasureTextTempData& tempData)
{
    std::vector<SkGlyphID> glyphs;
    std::vector<uint8_t> glyphChars;
    std::vector<SkScalar> glyphWidths;
    return BreakText(text, byteLength, textEncoding,
                     font, fallbackFontCreator, paint, maxWidth, measuredWidth, measuredHeight,
                     glyphs, glyphChars, glyphWidths, nullptr, nullptr, tempData);
}

size_t DrawSkiaText::BreakText(const void* text, size_t byteLength, SkTextEncoding textEncoding,
                               const SkFont& font, FallbackFontCreator fallbackFontCreator,
                               const SkPaint& paint, SkScalar maxWidth,
                               SkScalar* measuredWidth, SkScalar* measuredHeight,
                               std::vector<SkGlyphID>& glyphs,
                               std::vector<uint8_t>& glyphChars,
                               std::vector<SkScalar>& glyphWidths,
                               std::vector<uint8_t>* glyphCharList,
                               std::vector<SkScalar>* glyphWidthList,
                               MeasureTextTempData& tempData)
{
    PerformanceStat statPerformance(_T("DrawSkiaText::breakText"));
    if ((text == nullptr) || (maxWidth <= 0) || (byteLength == 0)){
        if (measuredWidth != nullptr) {
            *measuredWidth = 0;
        }
        return 0;
    }
    bool bWantGlyphData = (glyphCharList != nullptr) || (glyphWidthList != nullptr);
    SkRect bounds = SkRect::MakeEmpty();
    SkScalar width = DrawSkiaText::MeasureText(font, text, byteLength, textEncoding, &bounds, &paint, fallbackFontCreator, tempData);
    if (measuredHeight != nullptr) {
        *measuredHeight = bounds.height();
        SkASSERT(*measuredHeight > 0);
    }
    if (width <= maxWidth) {        
        if (measuredWidth != nullptr) {
            *measuredWidth = width;
        }        
        if (!bWantGlyphData) {
            return byteLength;
        }
    }

    //TODO：
    //后面的逻辑，合并到MeasureText中，提供新的函数

    glyphs.clear();     //保存每个glyphs字符
    glyphChars.clear(); //保存每个glyphs对应的字符个数
    //每个字符的字节数
    size_t charBytes = 1;

    if (!TextToGlyphs(text, byteLength, textEncoding, font, glyphs, charBytes)) {
        if (measuredWidth != nullptr) {
            *measuredWidth = width;
        }
        return byteLength;
    }

    if (!TextGlyphChars(text, byteLength, textEncoding, glyphChars)) {
        if (measuredWidth != nullptr) {
            *measuredWidth = width;
        }
        return byteLength;
    }
    SkASSERT(glyphChars.size() == glyphs.size());
    if (glyphChars.size() != glyphs.size()) {
        if (measuredWidth != nullptr) {
            *measuredWidth = width;
        }
        return byteLength;
    }

    glyphWidths.clear(); //保存每个glyphs字符的宽度
    glyphWidths.resize(glyphs.size(), 0);
    font.getWidthsBounds(SkSpan<const SkGlyphID>(glyphs.data(), glyphs.size()), //TODO：getWidthsBounds函数替换
                         SkSpan<SkScalar>(glyphWidths.data(), glyphWidths.size()),
                         SkSpan<SkRect>(), &paint);

    if (bWantGlyphData && (width <= maxWidth)) {
        if (glyphCharList != nullptr) {
            glyphCharList->swap(glyphChars);
        }
        if (glyphWidthList != nullptr) {
            glyphWidthList->swap(glyphWidths);
        }
        return byteLength;
    }

    size_t nGlyphCount = 0;    //符合要求的字形的数量
    size_t breakByteLength = 0;//单位是字节
    SkScalar totalWidth = 0;
    const size_t nGlyphWidthsCount = glyphWidths.size();
    for (size_t i = 0; i < nGlyphWidthsCount; ++i) {
        if ((totalWidth + glyphWidths[i]) > maxWidth) {
            nGlyphCount = i;
            for (size_t index = 0; index < i; ++index) {
                //计算字符个数
                breakByteLength += (glyphChars[index] * charBytes);
            }
            break;
        }
        totalWidth += glyphWidths[i];
    }
    if (measuredWidth != nullptr) {
        *measuredWidth = totalWidth;
    }
    SkASSERT(breakByteLength <= byteLength);
    if (breakByteLength > byteLength) {
        breakByteLength = byteLength;
    }
    if (bWantGlyphData) {
        SkASSERT(nGlyphCount > 0);
        if (glyphCharList != nullptr) {
            glyphCharList->swap(glyphChars);
            SkASSERT(nGlyphCount <= glyphCharList->size());
            glyphCharList->resize(nGlyphCount);
        }
        if (glyphWidthList != nullptr) {
            glyphWidthList->swap(glyphWidths);
            SkASSERT(nGlyphCount <= glyphWidthList->size());
            glyphWidthList->resize(nGlyphCount);
        }
    }
    return breakByteLength;
}


bool DrawSkiaText::TextToGlyphs(const void* text, size_t byteLength, SkTextEncoding textEncoding,
                                const SkFont& font, std::vector<SkGlyphID>& glyphs, size_t& charBytes)
{
    PerformanceStat statPerformance(_T("DrawSkiaText::TextToGlyphs"));
    glyphs.clear();
    glyphs.resize(byteLength, { 0, });
    SkSpan<SkGlyphID> glyphsSpan(glyphs.data(), glyphs.size());
    int glyphsCount = (int)font.textToGlyphs(text, byteLength, textEncoding, glyphsSpan);
    if (glyphsCount <= 0) {
        return false;
    }
    SkASSERT(glyphsCount <= (int)glyphs.size());
    glyphs.resize(glyphsCount);
    if (textEncoding == SkTextEncoding::kUTF8) {
        charBytes = 1;
    }
    else if (textEncoding == SkTextEncoding::kUTF16) {
        charBytes = 2;
    }
    else if (textEncoding == SkTextEncoding::kUTF32) {
        charBytes = 4;
    }
    else {
        SkASSERT(false);
        return false;
    }
    return true;
}

bool DrawSkiaText::TextGlyphChars(const void* text, size_t byteLength, SkTextEncoding textEncoding,
                                  std::vector<uint8_t>& glyphChars)
{
    glyphChars.clear();
    if (textEncoding == SkTextEncoding::kUTF8) {
        glyphChars.reserve(byteLength);
    }
    else if (textEncoding == SkTextEncoding::kUTF16) {
        glyphChars.reserve(byteLength / 2);
    }
    else if (textEncoding == SkTextEncoding::kUTF32) {
        glyphChars.resize(byteLength, 1);
    }
    else {
        SkASSERT(false);
        return false;
    }
    if ((text == nullptr) || (byteLength == 0)) {
        return false;
    }
    bool bHasError = false;
    if (textEncoding == SkTextEncoding::kUTF8) {
        const char* utf8 = static_cast<const char*>(text);
        const char* stop = utf8 + byteLength;
        while (utf8 < stop) {
            uint8_t numChars = 1;
            int type = SkUTF8_ByteType(*(const uint8_t*)utf8);
            SkASSERT(type >= -1 && type <= 4);
            if (!SkUTF8_TypeIsValidLeadingByte(type) || utf8 + type > stop) {
                // Sequence extends beyond end.
                bHasError = true;
                break;
            }
            while (type-- > 1) {
                ++numChars;
                ++utf8;
                if (!SkUTF8_ByteIsContinuation(*(const uint8_t*)utf8)) {
                    bHasError = true;
                    break;
                }
            }
            glyphChars.push_back(numChars);
            if (glyphChars.size() > byteLength) {
                bHasError = true;
                break;
            }
            ++utf8;
        }
    }
    else if (textEncoding == SkTextEncoding::kUTF16) {
        SkASSERT((byteLength % sizeof(uint16_t) == 0));//字符数必须是偶数
        //如果存在2个Unicode的字，检测具体哪个字符是双Unicode字节的，并做标记
        const uint16_t* src = static_cast<const uint16_t*>(text);
        const uint16_t* stop = src + (byteLength >> 1);
        while (src < stop) {
            uint8_t numChars = 1;
            unsigned c = *src++;
            SkASSERT(!SkUTF16_IsLowSurrogate(c));
            if (SkUTF16_IsHighSurrogate(c)) {
                if (src >= stop) {
                    glyphChars.clear();
                    SkASSERT(glyphChars.size() == glyphs.size());
                    break;
                }
                c = *src++;
                if (!SkUTF16_IsLowSurrogate(c)) {
                    glyphChars.clear();
                    SkASSERT(glyphChars.size() == glyphs.size());
                    break;
                }
                numChars = 2;
            }
            glyphChars.push_back(numChars);
            if (glyphChars.size() > byteLength) {
                bHasError = true;
                break;
            }
        }
    }
    if (bHasError) {
        glyphChars.clear();
    }
    SkASSERT(!bHasError);
    return !bHasError;
}

/** 判断是否为空格、不可见字符
* @param [in] c 是 Unicode 字符
* 返回true表示可以在当前字符进行换行
* 返回false表示不可以在当前字符换行
*/
static inline bool SkUTF_IsWhiteSpace(int c)
{
    //ASCII值（c）小于32的时候（含控制字符等不可见字符、空格），返回true；否则返回false
    return !((c - 1) >> 5);
}

/** 判断在当前字符处是否可以分行
* @param [in] c 是 Unicode 字符
*/
static inline bool SkUTF_IsLineBreaker(int c)
{
    //在数字和字母上不分行（返回false），尽量不换行，确保数字和英文单词的完整性
    //非字母数字（返回true）均可以分行
    if ((c >= -1) && (c <= 255)) {
        //ASCII值（c）
        if (::isalnum(c)) {
            return false;
        }
    }
    return true;
}

static inline SkUnichar SkUTF_NextUnichar(const void** ptr, SkTextEncoding textEncoding)
{
    if (textEncoding == SkTextEncoding::kUTF16) {
        return SkUTF16_NextUnichar((const uint16_t**)ptr);
    }
    else if (textEncoding == SkTextEncoding::kUTF32) {
        const uint32_t** srcPtr = (const uint32_t**)ptr;
        const uint32_t* src = *srcPtr;
        SkUnichar c = *src;
        *srcPtr = ++src;
        return c;
    }
    else {
        return SkUTF8_NextUnichar((const char**)ptr);
    }
}

static inline SkUnichar SkUTF_ToUnichar(const void* utf, SkTextEncoding textEncoding)
{
    if (textEncoding == SkTextEncoding::kUTF16) {
        const uint16_t* srcPtr = (const uint16_t*)utf;
        return SkUTF16_NextUnichar(&srcPtr);
    }
    else if (textEncoding == SkTextEncoding::kUTF32) {
        const uint32_t* srcPtr = (const uint32_t*)utf;
        SkUnichar c = *srcPtr;
        return c;
    }
    else {
        return SkUTF8_ToUnichar((const char*)utf);
    }
}

static inline int SkUTF_CountUTFBytes(const void* utf, SkTextEncoding textEncoding)
{
    if (textEncoding == SkTextEncoding::kUTF16) {
        // 2 or 4
        int numChars = 1;
        const uint16_t* src = static_cast<const uint16_t*>(utf);
        unsigned c = *src++;
        if (SkUTF16_IsHighSurrogate(c)) {
            c = *src++;
            if (!SkUTF16_IsLowSurrogate(c)) {
                SkASSERT(false);
            }
            numChars = 2;
        }
        return numChars * 2;
    }
    else if (textEncoding == SkTextEncoding::kUTF32) {
        //only 4
        return 4;
    }
    else {
        //1 or 2 or 3 or 4
        return SkUTF8_CountUTF8Bytes((const char*)utf);
    }
}

size_t DrawSkiaText::Linebreak(const char text[], const char stop[], SkTextEncoding textEncoding,
                               const SkFont& font, FallbackFontCreator fallbackFontCreator, const SkPaint& paint,
                               SkScalar margin, TextBoxLineMode lineMode, MeasureTextTempData& tempData,
                               size_t* trailing)
{
    size_t lengthBreak = stop - text;//单行模式
    if (lineMode != TextBoxLineMode::kOneLine_Mode) {
        //多行模式
        lengthBreak = DrawSkiaText::BreakText(text, stop - text, textEncoding, font, fallbackFontCreator,
                                              paint, margin, nullptr, nullptr, tempData);
    }

    //Check for white space or line breakers before the lengthBreak
    const char* start = text;
    const char* word_start = text;

    //标记是否可以分行
    bool prevIsLineBreaker = true;

    //如果设置trailing的值，这部分字符串在绘制的时候，会被忽略，不绘制
    if (trailing) {
        *trailing = 0;
    }

    while (text < stop) {
        const char* prevText = text;
        SkUnichar uni = SkUTF_NextUnichar((const void**)&text, textEncoding);

        //当前字符是否为空格（或非可见字符）
        bool currIsWhiteSpace = SkUTF_IsWhiteSpace(uni);

        //当前字符是否可以分行，分行条件：当前字符是空格（或非可见字符），或者不是字母/数字
        //Word分行逻辑：按Word分行，保证一个英文单词或者一个完整的数字不被分行显示
        bool currIsLineBreaker = SkUTF_IsLineBreaker(uni);
        if (lineMode == TextBoxLineMode::kCharBreak_Mode) {
            //按字符分行，每个字符都可以分行
            currIsLineBreaker = true;
        }
        if (prevIsLineBreaker) {
            //如果前面字符可以分行, 就执行前面一个字符
            word_start = prevText;
        }
        prevIsLineBreaker = currIsLineBreaker;

        if (text > start + lengthBreak) {
            if (currIsWhiteSpace) {
                // eat the rest of the whitespace
                while (text < stop && SkUTF_IsWhiteSpace(SkUTF_ToUnichar(text, textEncoding))) {
                    text += SkUTF_CountUTFBytes(text, textEncoding);
                }
                if (trailing) {
                    *trailing = text - prevText;
                }
            }
            else {
                // backup until a whitespace (or 1 char)
                if (word_start == start) {
                    if (prevText > start) {
                        text = prevText;
                    }
                }
                else {
                    text = word_start;
                }
            }
            break;
        }

        if ('\n' == uni) {
            size_t ret = text - start;
            size_t lineBreakSize = 1;
            if (text < stop) {
                uni = SkUTF_NextUnichar((const void**)&text, textEncoding);
                if ('\r' == uni) {
                    ret = text - start;
                    ++lineBreakSize;
                }
            }
            if (textEncoding == SkTextEncoding::kUTF16) {
                //每个字符串占2个字节
                lineBreakSize *= 2;
            }
            else if (textEncoding == SkTextEncoding::kUTF32) {
                //每个字符串占4个字节
                lineBreakSize *= 4;
            }
            if (trailing) {
                *trailing = lineBreakSize;
            }
            return ret;
        }

        if ('\r' == uni) {
            size_t ret = text - start;
            size_t lineBreakSize = 1;
            if (text < stop) {
                uni = SkUTF_NextUnichar((const void**)&text, textEncoding);
                if ('\n' == uni) {
                    ret = text - start;
                    ++lineBreakSize;
                }
            }
            if (textEncoding == SkTextEncoding::kUTF16) {
                //每个字符串占2个字节
                lineBreakSize *= 2;
            }
            else if (textEncoding == SkTextEncoding::kUTF32) {
                //每个字符串占4个字节
                lineBreakSize *= 4;
            }
            if (trailing) {
                *trailing = lineBreakSize;
            }
            return ret;
        }
    }

    return text - start;
}

int32_t DrawSkiaText::CountLines(const char text[], size_t len, SkTextEncoding textEncoding,
                                 const SkFont& font, FallbackFontCreator fallbackFontCreator, const SkPaint& paint,
                                 SkScalar width, TextBoxLineMode lineMode,
                                 std::vector<size_t>* lineLenList)
{
    MeasureTextTempData tempData;
    const char* stop = text + len;
    int32_t count = 0;
    if (width > 0) {
        do {
            count += 1;
            size_t lineLen = DrawSkiaText::Linebreak(text, stop, textEncoding, font, fallbackFontCreator, paint, width, lineMode, tempData);
            if (lineLenList != nullptr) {
                lineLenList->push_back(lineLen);
            }
            text += lineLen;
        } while (text < stop);
    }
    return count;
}

SkScalar DrawSkiaText::MeasureText(const SkFont& font, const SkiaTextData& textData,
                                   SkRect* bounds, const SkPaint* paint,
                                   const IFont* pFont,
                                   MeasureTextTempData& tempData)
{
    return MeasureText(font, textData.GetText(), textData.GetByteLength(), textData.GetTextEncoding(),
                       bounds, paint, pFont, tempData);
}

SkScalar DrawSkiaText::MeasureText(const SkFont& font, const SkiaTextData& textData,
                                   SkRect* bounds, const SkPaint* paint,
                                   FallbackFontCreator fallbackFontCreator,
                                   MeasureTextTempData& tempData)
{
    return MeasureText(font, textData.GetText(), textData.GetByteLength(), textData.GetTextEncoding(),
                       bounds, paint, fallbackFontCreator, tempData);
}

void DrawSkiaText::DrawSimpleText(SkCanvas* skCanvas, const SkiaTextData& textData,
                                   SkScalar x, SkScalar y,
                                   const SkFont& font, const SkPaint& paint,
                                   const IFont* pFont)
{
    DrawSimpleText(skCanvas, textData.GetText(), textData.GetByteLength(), textData.GetTextEncoding(),
                   x, y, font, paint, pFont);
}

void DrawSkiaText::DrawSimpleText(SkCanvas* skCanvas, const SkiaTextData& textData,
                                   SkScalar x, SkScalar y,
                                   const SkFont& font, const SkPaint& paint,
                                   FallbackFontCreator fallbackFontCreator)
{
    DrawSimpleText(skCanvas, textData.GetText(), textData.GetByteLength(), textData.GetTextEncoding(),
                   x, y, font, paint, fallbackFontCreator);
}

size_t DrawSkiaText::BreakText(const SkiaTextData& textData,
                               const SkFont& font, FallbackFontCreator fallbackFontCreator,
                               const SkPaint& paint, SkScalar maxWidth,
                               SkScalar* measuredWidth,
                               SkScalar* measuredHeight,
                               MeasureTextTempData& tempData)
{
    return BreakText(textData.GetText(), textData.GetByteLength(), textData.GetTextEncoding(),
                     font, fallbackFontCreator, paint, maxWidth,
                     measuredWidth, measuredHeight, tempData);
}

int32_t DrawSkiaText::CountLines(const SkiaTextData& textData,
                                  const SkFont& font, FallbackFontCreator fallbackFontCreator,
                                  const SkPaint& paint, SkScalar width, TextBoxLineMode lineMode,
                                  std::vector<size_t>* lineLenList)
{
    const char* text = static_cast<const char*>(textData.GetText());
    return CountLines(text, textData.GetByteLength(), textData.GetTextEncoding(),
                      font, fallbackFontCreator, paint, width, lineMode, lineLenList);
}

size_t DrawSkiaText::Linebreak(const SkiaTextData& textData, const char* stop,
                                const SkFont& font, FallbackFontCreator fallbackFontCreator, const SkPaint& paint,
                                SkScalar margin, TextBoxLineMode lineMode, MeasureTextTempData& tempData,
                                size_t* trailing)
{
    const char* text = static_cast<const char*>(textData.GetText());
    return Linebreak(text, stop, textData.GetTextEncoding(),
                     font, fallbackFontCreator, paint, margin, lineMode, tempData, trailing);
}

} // namespace ui
