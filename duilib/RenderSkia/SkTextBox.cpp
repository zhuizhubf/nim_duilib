/*
 * Copyright 2006 The Android Open Source Project
 *
 * Use of this source code is governed by a BSD-style license that can be
 * found in the LICENSE file.
 */

#include "SkTextBox.h"
#include "SkUtils.h"

#include "SkiaHeaderBegin.h"
#include "include/core/SkFont.h"
#include "include/core/SkFontMetrics.h"
#include "include/core/SkTextBlob.h"
#include "include/core/SkSpan.h"
#include "SkiaHeaderEnd.h"

#include "duilib/RenderSkia/DrawSkiaText.h"

//该文件原始文件的出处：skia/chrome_67/src/utils/SkTextBox.cpp
//基于原始文件，有修改，以兼容最新版本的skia代码（2023-06-25）
//原始文件从chrome 68以后就删除了。

namespace ui
{

SkTextBox::SkTextBox()
{
    m_clipBox = true;
    m_box.setEmpty();
    m_spacingMul = SK_Scalar1;
    m_spacingAdd = 0;
    m_lineMode = kWordBreak_Mode;
    m_spacingAlign = kStart_SpacingAlign;
    m_textAlign = kLeft_Align;

    m_endEllipsis = false;
    m_pathEllipsis = false;
    m_underline = false;
    m_strikeOut = false;

    m_pText = nullptr;
    m_len = 0;
    m_textEncoding = SkTextEncoding::kUTF8;

    m_pPaint = nullptr;
    m_pFont = nullptr;
    m_fallbackFontCreator = nullptr;
}

void SkTextBox::SetLineMode(TextBoxLineMode lineMode)
{
    SkASSERT((unsigned)lineMode < kModeCount);
    m_lineMode = lineMode;
}

void SkTextBox::SetSpacingAlign(SpacingAlign align)
{
    SkASSERT((unsigned)align < kSpacingAlignCount);
    m_spacingAlign = SkToU8(align);
}

void SkTextBox::SetTextAlign(TextAlign align)
{
    SkASSERT((unsigned)align < kAlignCount);
    m_textAlign = SkToU8(align);
}

void SkTextBox::SetEndEllipsis(bool bEndEllipsis)
{
    m_endEllipsis = bEndEllipsis;
}

void SkTextBox::SetPathEllipsis(bool bPathEllipsis)
{
    m_pathEllipsis = bPathEllipsis;
}

void SkTextBox::SetUnderline(bool bUnderline)
{
    m_underline = bUnderline;
}

void SkTextBox::SetStrikeOut(bool bStrikeOut)
{
    m_strikeOut = bStrikeOut;
}

void SkTextBox::GetBox(SkRect* pBox) const
{
    if (pBox) {
        *pBox = m_box;
    }
}

void SkTextBox::SetBox(const SkRect& box)
{
    m_box = box;
}

void SkTextBox::SetBox(SkScalar left, SkScalar top, SkScalar right, SkScalar bottom)
{
    m_box.setLTRB(left, top, right, bottom);
}

void SkTextBox::SetClipBox(bool bClipBox)
{
    m_clipBox = bClipBox;
}

void SkTextBox::GetSpacing(SkScalar* pMul, SkScalar* pAdd) const
{
    if (pMul) {
        *pMul = m_spacingMul;
    }
    if (pAdd) {
        *pAdd = m_spacingAdd;
    }
}

void SkTextBox::SetSpacing(SkScalar mul, SkScalar add)
{
    m_spacingMul = mul;
    m_spacingAdd = add;
}

SkScalar SkTextBox::Visit(Visitor& visitor) const
{
    const char* pText = m_pText;
    size_t nLen = m_len;
    SkTextEncoding textEncoding = m_textEncoding;
    const SkFont& font = *m_pFont;
    const SkPaint& paint = *m_pPaint;
    TextBoxLineMode lineMode = m_lineMode;
    SkRect boxRect = m_box;
    SkScalar spacingMul = m_spacingMul;
    SkScalar spacingAdd = m_spacingAdd;
    uint8_t spacingAlign = m_spacingAlign;
    uint8_t textAlign = m_textAlign;
    FallbackFontCreator fallbackFontCreator = m_fallbackFontCreator;
    SkASSERT(fallbackFontCreator != nullptr);

    SkScalar marginWidth = boxRect.width();

    if (marginWidth <= 0 || nLen == 0) {
        return boxRect.fTop;
    }

    const char* textStop = pText + nLen;

    SkScalar scaledSpacing = 0;
    SkScalar height = 0;
    SkScalar fontHeight = 0;
    SkFontMetrics metrics;

    SkScalar x = boxRect.fLeft;
    SkScalar y = boxRect.fTop;
    fontHeight = font.getMetrics(&metrics);
    scaledSpacing = fontHeight * spacingMul + spacingAdd;
    height = boxRect.height();

    //计算第一行文字的Y坐标位置
    {
        SkScalar textHeight = fontHeight;

        if (spacingAlign != kStart_SpacingAlign) {
            int32_t nCount = DrawSkiaText::CountLines(pText, textStop - pText, textEncoding,
                                                     font, fallbackFontCreator, paint, marginWidth, lineMode);
            SkASSERT(nCount > 0);
            textHeight += scaledSpacing * (nCount - 1);
        }

        switch (spacingAlign) {
        case kStart_SpacingAlign:
            y = 0;
            break;
        case kCenter_SpacingAlign:
            y = SkScalarHalf(height - textHeight);
            if (y < 0) {
                //如果居中对齐绘制区域不足，那么按照可显示的文字进行居中对齐
                y = static_cast<SkScalar>((((int)height - (int)fontHeight) / 2));
            }
            break;
        default:
            SkASSERT(spacingAlign == kEnd_SpacingAlign);
            y = height - textHeight;
            break;
        }
        y += boxRect.fTop - metrics.fAscent;
    }

    for (;;) {
        size_t trailing = 0;
        nLen = DrawSkiaText::Linebreak(pText, textStop, textEncoding,
                                      font, fallbackFontCreator, paint,
                                      marginWidth, lineMode,
                                      &trailing);
        if (y + metrics.fDescent + metrics.fLeading > 0) {

            if (textAlign == kLeft_Align) {
                //横向：左对齐
                x = boxRect.fLeft;
            }
            else {
                //右对齐或者中对齐
                SkScalar textWidth = DrawSkiaText::MeasureText(font, pText,
                                                               nLen - trailing,
                                                               textEncoding,
                                                               nullptr, &paint, fallbackFontCreator);
                if (textAlign == kCenter_Align) {
                    //横向：中对齐
                    x = boxRect.fLeft + (marginWidth / 2) - textWidth / 2;
                    if (x < boxRect.fLeft) {
                        x = boxRect.fLeft;
                    }
                }
                else {
                    //横向，右对齐
                    x = boxRect.fRight - textWidth;
                    if (x < boxRect.fLeft) {
                        x = boxRect.fLeft;
                    }
                }
            }
            //后续还有没有待绘制文本
            bool bHasMoreText = (pText + nLen) < textStop;
            //当前是否为最后一行
            bool bIsLastLine = (y + scaledSpacing + metrics.fAscent / 2) >= boxRect.fBottom;

            visitor(pText, nLen - trailing, textEncoding, x, y, font, paint, bHasMoreText, bIsLastLine, fallbackFontCreator);
        }
        pText += nLen;
        if (pText >= textStop) {
            break;
        }
        y += scaledSpacing;
        if ((y + metrics.fAscent / 2) >= boxRect.fBottom) {
            break;
        }
    }
    return y + metrics.fDescent + metrics.fLeading;
}

template<typename T>
static bool EllipsisTextUTF(const char pText[], size_t nLength, SkTextEncoding textEncoding,
                            bool bEndEllipsis, bool bPathEllipsis,
                            const SkFont& font, FallbackFontCreator fallbackFontCreator,
                            const SkPaint& paint,
                            SkScalar destWidth,
                            const char** pTextOut,
                            size_t& nLengthOut,
                            T& stringUtf,
                            const T& ellipsisStr,
                            const T& pathSep)
{
    if ((pText == nullptr) || (nLength == 0)) {
        return false;
    }
    T pathEnd;
    T string;

    size_t charBytes = 1;
    if (textEncoding == SkTextEncoding::kUTF8) {
        charBytes = 1;
    }
    if (textEncoding == SkTextEncoding::kUTF16) {
        charBytes = 2;
    }
    if (textEncoding == SkTextEncoding::kUTF32) {
        charBytes = 4;
    }
    SkScalar ellipsisWidth = DrawSkiaText::MeasureText(font, ellipsisStr.c_str(), ellipsisStr.size() * charBytes, textEncoding, nullptr, &paint, fallbackFontCreator);
    SkScalar pathEndWidth = 0;
    string.assign((const typename T::value_type*)pText, nLength / charBytes);
    if (bPathEllipsis) {
        int pos = (int)string.find_last_of(pathSep);
        if (pos > 0) {
            pathEnd = string.substr(pos);
            pathEndWidth = DrawSkiaText::MeasureText(font, pathEnd.c_str(), pathEnd.size() * charBytes, textEncoding, nullptr, &paint, fallbackFontCreator);
            if ((pathEndWidth + ellipsisWidth) > destWidth) {
                //宽度不足以显示路径的最后一段文字
                pathEnd.clear();
            }
        }
        if (pathEnd.empty()) {
            bEndEllipsis = true;
            pathEndWidth = 0;
        }
        else {
            string = string.substr(0, pos);
        }
    }

    if (!bEndEllipsis && pathEnd.empty()) {
        return false;
    }

    SkScalar leftWidth = destWidth - ellipsisWidth - pathEndWidth;
    if (leftWidth <= 1) {
        return false;
    }

    size_t textLen = DrawSkiaText::breakText(string.c_str(), string.size() * charBytes, textEncoding,
                                          font, fallbackFontCreator, paint, leftWidth);
    textLen /= charBytes;
    if ((textLen > 0) && (textLen <= (string.size()))) {
        string = string.substr(0, textLen);
        string += ellipsisStr;
        string += pathEnd;
        stringUtf = string.c_str();
        if (pTextOut) {
            *pTextOut = (const char*)stringUtf.c_str();
        }
        nLengthOut = stringUtf.size() * charBytes;
        return true;
    }
    return false;
}

static bool EllipsisText(const char pText[], size_t nLength, SkTextEncoding textEncoding,
                         std::string& stringUtf8,
                         std::u16string& stringUtf16,
                         std::u32string& stringUtf32,
                         bool bEndEllipsis, bool bPathEllipsis,
                         const SkFont& font, FallbackFontCreator fallbackFontCreator, const SkPaint& paint,
                         SkScalar destWidth,
                         const char** pTextOut, size_t& nLengthOut)
{
    if (textEncoding == SkTextEncoding::kUTF8) {
        return EllipsisTextUTF<std::string>(pText, nLength, SkTextEncoding::kUTF8,
                                            bEndEllipsis, bPathEllipsis,
                                            font, fallbackFontCreator, paint,
                                            destWidth,
                                            pTextOut, nLengthOut,
                                            stringUtf8,
                                            "...",
                                            "\\/");
    }
    else if (textEncoding == SkTextEncoding::kUTF16) {
        return EllipsisTextUTF<std::u16string>(pText, nLength, SkTextEncoding::kUTF16,
                                            bEndEllipsis, bPathEllipsis,
                                            font, fallbackFontCreator, paint,
                                            destWidth,
                                            pTextOut, nLengthOut,
                                            stringUtf16,
                                            u"...",
                                            u"\\/");
    }
    else if (textEncoding == SkTextEncoding::kUTF32) {
        return EllipsisTextUTF<std::u32string>(pText, nLength, SkTextEncoding::kUTF32,
                                            bEndEllipsis, bPathEllipsis,
                                            font, fallbackFontCreator, paint,
                                            destWidth,
                                            pTextOut, nLengthOut,
                                            stringUtf32,
                                            U"...",
                                            U"\\/");
    }
    return false;
}

static void TextBox_DrawText(SkTextBox* pTextBox,
                             SkCanvas* pSkCanvas,
                             const char pText[], size_t nLength, SkTextEncoding textEncoding,
                             SkScalar x, SkScalar y,
                             const SkFont& font, const SkPaint& paint,
                             bool bHasMoreText, bool bIsLastLine,
                             FallbackFontCreator fallbackFontCreator)
{
    //绘制一行文字
    SkRect boxRect;
    pTextBox->GetBox(&boxRect);

    //绘制区域不足时，自动在末尾绘制省略号
    bool bEndEllipsis = pTextBox->GetEndEllipsis();
    //绘制区域不足时，自动在绘制省略号代替文本
    //如果字符串包含反斜杠 (\\) 字符，在最后一个反斜杠之后保留尽可能多的文本。
    bool bPathEllipsis = pTextBox->GetPathEllipsis();
    //字体属性：下划线
    bool bUnderline = pTextBox->GetUnderline();
    //字体属性：删除线
    bool bStrikeOut = pTextBox->GetStrikeOut();
    //单行模式
    bool isSingleLine = pTextBox->GetLineMode() == TextBoxLineMode::kOneLine_Mode;

    if (!bEndEllipsis && !bPathEllipsis && !bUnderline && !bStrikeOut) {
        DrawSkiaText::DrawSimpleText(pSkCanvas, pText, nLength, textEncoding, x, y, font, paint, fallbackFontCreator);
    }
    else {
        bool needEllipsis = false;
        if (bEndEllipsis || bPathEllipsis) {
            if (isSingleLine) {
                //单行模式
                SkScalar textWidth = DrawSkiaText::MeasureText(font, pText, nLength, textEncoding, nullptr, &paint, fallbackFontCreator);
                if ((x + textWidth) > boxRect.fRight) {
                    //文字超出边界，需要增加"..."替代无法显示的文字
                    needEllipsis = true;
                }
            }
            else {
                //多行模式
                if (bEndEllipsis && bIsLastLine && bHasMoreText) {
                    //文字超出边界，需要增加"..."替代无法显示的文字
                    needEllipsis = true;
                }
            }
        }
        if (!needEllipsis && !bUnderline && !bStrikeOut) {
            DrawSkiaText::DrawSimpleText(pSkCanvas, pText, nLength, textEncoding, x, y, font, paint, fallbackFontCreator);
        }
        else {
            std::string stringUtf8;
            std::u16string stringUtf16;
            std::u32string stringUtf32;
            if (needEllipsis) {
                const char* pTextOut = nullptr;
                size_t nLengthOut = 0;
                if (EllipsisText(pText, nLength, textEncoding,
                                 stringUtf8, stringUtf16, stringUtf32,
                                 bEndEllipsis, bPathEllipsis,
                                 font, fallbackFontCreator, paint,
                                 boxRect.fRight - x,
                                 &pTextOut, nLengthOut)) {
                    //修改text和length的值，但不改变textEncoding
                    SkASSERT(pTextOut != nullptr);
                    SkASSERT(nLengthOut != 0);
                    pText = pTextOut;
                    nLength = nLengthOut;
                }
            }
            //绘制文本
            DrawSkiaText::DrawSimpleText(pSkCanvas, pText, nLength, textEncoding, x, y, font, paint, fallbackFontCreator);
            if (bUnderline || bStrikeOut) {
                SkScalar width = DrawSkiaText::MeasureText(font, pText, nLength, textEncoding, nullptr, &paint, fallbackFontCreator);

                //绘制删除线/下划线的厚度因子（相对于字体大小）
                static constexpr SkScalar kLineThicknessFactor = (SK_Scalar1 / 18);
                //删除线高于基线的偏移量因子
                const SkScalar kStrikeThroughOffset = (SK_Scalar1 * 65 / 252);
                //下划线低于基线的偏移量因子
                const SkScalar kUnderlineOffset = (SK_Scalar1 / 9);

                if (bStrikeOut) {
                    //绘制删除线
                    SkScalar thickness_factor = kLineThicknessFactor;
                    const SkScalar text_size = font.getSize();
                    const SkScalar height = text_size * thickness_factor;
                    const SkScalar top = y - text_size * kStrikeThroughOffset - height / 2;
                    SkScalar x_scalar = SkIntToScalar(x);
                    const SkRect r = SkRect::MakeLTRB(x_scalar, top, x_scalar + width, top + height);
                    pSkCanvas->drawRect(r, paint);
                }
                if (bUnderline) {
                    //绘制下划线
                    SkScalar thickness_factor = 1.5;
                    SkScalar x_scalar = SkIntToScalar(x);
                    const SkScalar text_size = font.getSize();
                    SkRect r = SkRect::MakeLTRB(
                                                x_scalar, y + text_size * kUnderlineOffset, x_scalar + width,
                                                y + (text_size *
                                                    (kUnderlineOffset +
                                                    (thickness_factor * kLineThicknessFactor))));
                    pSkCanvas->drawRect(r, paint);
                }
            }
        }
    }
}

///////////////////////////////////////////////////////////////////////////////

class CanvasVisitor : public SkTextBox::Visitor {
    SkCanvas* m_pCanvas;
    SkTextBox* m_pTextBox;
public:
    CanvasVisitor(SkCanvas* pCanvas, SkTextBox* pTextBox):
         m_pCanvas(pCanvas)
        ,m_pTextBox(pTextBox) {
    }

    void operator()(const char pText[], size_t nLength, SkTextEncoding textEncoding,
                    SkScalar x, SkScalar y,
                    const SkFont& font, const SkPaint& paint,
                    bool bHasMoreText, bool bIsLastLine,
                    FallbackFontCreator fallbackFontCreator) override
    {
        //调用单独封装的函数绘制文字，便于扩展
        TextBox_DrawText(m_pTextBox,
                         m_pCanvas,
                         pText, nLength, textEncoding,
                         x, y,
                         font, paint,
                         bHasMoreText, bIsLastLine,
                         fallbackFontCreator);
    }
};

void SkTextBox::SetText(const char pText[], size_t nLen, SkTextEncoding textEncoding,
                        const SkFont& font, const SkPaint& paint,
                        FallbackFontCreator fallbackFontCreator)
{
    m_pText = pText;
    m_len = nLen;
    m_textEncoding = textEncoding;
    m_pPaint = &paint;
    m_pFont = &font;
    m_fallbackFontCreator = fallbackFontCreator;
    SkASSERT(fallbackFontCreator != nullptr);

#ifdef _DEBUG
    //检查字符串序列是否正确
    if (textEncoding == SkTextEncoding::kUTF8) {
        SkASSERT(SkUTF8_CountUnichars(pText, nLen) != -1);
    }
    else if (textEncoding == SkTextEncoding::kUTF16) {
        SkASSERT(SkUTF16_CountUnichars(pText, nLen) != -1);
    }
    else if (textEncoding == SkTextEncoding::kUTF32) {
        SkASSERT(SkUTF32_CountUnichars(pText, nLen) != -1);
    }
    else {
        SkASSERT(false);
    }
#endif
}

void SkTextBox::Draw(SkCanvas* pSkCanvas,
                     const char pText[], size_t nLen, SkTextEncoding textEncoding,
                     const SkFont& font, const SkPaint& paint,
                     FallbackFontCreator fallbackFontCreator)
{
    SetText(pText, nLen, textEncoding, font, paint, fallbackFontCreator);
    Draw(pSkCanvas);
}

void SkTextBox::Draw(SkCanvas* pSkCanvas)
{
    SkASSERT(pSkCanvas != nullptr);
    if (pSkCanvas == nullptr) {
        return;
    }

    SkASSERT(m_pText != nullptr);
    if (m_pText == nullptr) {
        return;
    }
    if (m_len == 0) {
        return;
    }
    SkASSERT((m_textEncoding == SkTextEncoding::kUTF8) ||
             (m_textEncoding == SkTextEncoding::kUTF16)||
             (m_textEncoding == SkTextEncoding::kUTF32));
    if ((m_textEncoding != SkTextEncoding::kUTF8)  &&
        (m_textEncoding != SkTextEncoding::kUTF16) &&
        (m_textEncoding != SkTextEncoding::kUTF32)) {
        return;
    }

    SkASSERT(m_pFont != nullptr);
    SkASSERT(m_pPaint != nullptr);
    if ((m_pPaint == nullptr) || (m_pFont == nullptr)) {
        return;
    }

#ifdef _DEBUG
    //检查字符串序列是否正确
    if (m_textEncoding == SkTextEncoding::kUTF8) {
        SkASSERT(SkUTF8_CountUnichars(m_pText, m_len) != -1);
    }
    else if (m_textEncoding == SkTextEncoding::kUTF16) {
        SkASSERT(SkUTF16_CountUnichars(m_pText, m_len) != -1);
    }
    else if (m_textEncoding == SkTextEncoding::kUTF32) {
        SkASSERT(SkUTF32_CountUnichars(m_pText, m_len) != -1);
    }
#endif

    int saveCount = 0;
    if (m_clipBox) {
        saveCount = pSkCanvas->save();
        pSkCanvas->clipRect(m_box, true);
    }
    CanvasVisitor sink(pSkCanvas, this);
    this->Visit(sink);
    if (m_clipBox) {
        pSkCanvas->restoreToCount(saveCount);
    }
}

int32_t SkTextBox::CountLines() const
{
    return DrawSkiaText::CountLines(m_pText, m_len, m_textEncoding,
                                    *m_pFont, m_fallbackFontCreator, *m_pPaint, m_box.width(),
                                     m_lineMode);
}

SkScalar SkTextBox::GetTextHeight() const
{
    SkScalar spacing = m_pFont->getSize() * m_spacingMul + m_spacingAdd;
    return this->CountLines() * spacing;
}

}//namespace ui
