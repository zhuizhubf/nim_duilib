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
    fClipBox = true;
    fBox.setEmpty();
    fSpacingMul = SK_Scalar1;
    fSpacingAdd = 0;
    fLineMode = kWordBreak_Mode;
    fSpacingAlign = kStart_SpacingAlign;
    fTextAlign = kLeft_Align;

    fEndEllipsis = false;
    fPathEllipsis = false;
    fUnderline = false;
    fStrikeOut = false;

    fText = nullptr;
    fLen = 0;    
    fTextEncoding = SkTextEncoding::kUTF8;

    fPaint = nullptr;
    fFont = nullptr;
    fFallbackFontCreator = nullptr;
}

void SkTextBox::setLineMode(TextBoxLineMode mode)
{
    SkASSERT((unsigned)mode < kModeCount);
    fLineMode = mode;
}

void SkTextBox::setSpacingAlign(SpacingAlign align)
{
    SkASSERT((unsigned)align < kSpacingAlignCount);
    fSpacingAlign = SkToU8(align);
}

void SkTextBox::setTextAlign(TextAlign align)
{
    SkASSERT((unsigned)align < kAlignCount);
    fTextAlign = SkToU8(align);
}

void SkTextBox::setEndEllipsis(bool bEndEllipsis)
{
    fEndEllipsis = bEndEllipsis;
}

void SkTextBox::setPathEllipsis(bool bPathEllipsis)
{
    fPathEllipsis = bPathEllipsis;
}

void SkTextBox::setUnderline(bool bUnderline)
{
    fUnderline = bUnderline;
}

void SkTextBox::setStrikeOut(bool bStrikeOut)
{
    fStrikeOut = bStrikeOut;
}

void SkTextBox::getBox(SkRect* box) const
{
    if (box) {
        *box = fBox;
    }
}

void SkTextBox::setBox(const SkRect& box)
{
    fBox = box;
}

void SkTextBox::setBox(SkScalar left, SkScalar top, SkScalar right, SkScalar bottom)
{
    fBox.setLTRB(left, top, right, bottom);
}

void SkTextBox::setClipBox(bool bClipBox)
{
    fClipBox = bClipBox;
}

void SkTextBox::getSpacing(SkScalar* mul, SkScalar* add) const
{
    if (mul) {
        *mul = fSpacingMul;
    }
    if (add) {
        *add = fSpacingAdd;
    }
}

void SkTextBox::setSpacing(SkScalar mul, SkScalar add)
{
    fSpacingMul = mul;
    fSpacingAdd = add;
}

/////////////////////////////////////////////////////////////////////////////////////////////

SkScalar SkTextBox::visit(Visitor& visitor) const {
    const char* text = fText;
    size_t len = fLen;
    SkTextEncoding textEncoding = fTextEncoding;
    const SkFont& font = *fFont;
    const SkPaint& paint = *fPaint;
    TextBoxLineMode lineMode = fLineMode;
    SkRect boxRect = fBox;
    SkScalar spacingMul = fSpacingMul;
    SkScalar spacingAdd = fSpacingAdd;
    uint8_t spacingAlign = fSpacingAlign;
    uint8_t textAlign = fTextAlign;
    FallbackFontCreator fallbackFontCreator = fFallbackFontCreator;
    SkASSERT(fallbackFontCreator != nullptr);

    SkScalar marginWidth = boxRect.width();

    if (marginWidth <= 0 || len == 0) {
        return boxRect.top();
    }

    const char* textStop = text + len;

    SkScalar scaledSpacing = 0;
    SkScalar height = 0;
    SkScalar fontHeight = 0;
    SkFontMetrics metrics;

    SkScalar x = boxRect.fLeft;
    SkScalar y = boxRect.fTop;
    fontHeight = font.getMetrics(&metrics);
    scaledSpacing = fontHeight * spacingMul + spacingAdd;
    height = boxRect.height();

    //  compute Y position for first line
    {
        SkScalar textHeight = fontHeight;

        if (spacingAlign != kStart_SpacingAlign) {
            int32_t count = DrawSkiaText::CountLines(text, textStop - text, textEncoding,
                                                     font, fallbackFontCreator, paint, marginWidth, lineMode);
            SkASSERT(count > 0);
            textHeight += scaledSpacing * (count - 1);
        }

        switch (spacingAlign) {
        case kStart_SpacingAlign:
            y = 0;
            break;
        case kCenter_SpacingAlign:
            y = SkScalarHalf(height - textHeight);
            if (y < 0) {
                //如果居中对齐绘制区域不足，那么按照可显示的文字进行居中对齐
                y = static_cast<SkScalar>((((int)height - (int)fontHeight) / 2)) ;
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
        len = DrawSkiaText::Linebreak(text, textStop, textEncoding,
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
                SkScalar textWidth = DrawSkiaText::MeasureText(font, text,
                                                               len - trailing,
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
            bool hasMoreText = (text + len) < textStop;
            //当前是否为最后一行
            bool isLastLine = (y + scaledSpacing + metrics.fAscent / 2) >= boxRect.fBottom;

            visitor(text, len - trailing, textEncoding, x, y, font, paint, hasMoreText, isLastLine, fallbackFontCreator);
        }
        text += len;
        if (text >= textStop) {
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
static bool EllipsisTextUTF(const char text[], size_t length, SkTextEncoding textEncoding,
                            bool bEndEllipsis, bool bPathEllipsis,
                            const SkFont& font, FallbackFontCreator fallbackFontCreator,
                            const SkPaint& paint,
                            SkScalar destWidth,
                            const char** textOut, 
                            size_t& lengthOut,
                            T& string_utf,
                            const T& ellipsisStr /* "..." */,
                            const T& pathSep /* "/\\" */)
{
    if ((text == nullptr) || (length == 0)) {
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
    SkScalar ellipsisWidth = DrawSkiaText::MeasureText(font, ellipsisStr.c_str(), ellipsisStr.size()* charBytes, textEncoding, nullptr, &paint, fallbackFontCreator);
    SkScalar pathEndWidth = 0;    
    string.assign((const typename T::value_type*)text, length / charBytes);
    if (bPathEllipsis) {
        int pos = (int)string.find_last_of(pathSep);
        if (pos > 0) {
            pathEnd = string.substr(pos);
            pathEndWidth = DrawSkiaText::MeasureText(font, pathEnd.c_str(), pathEnd.size()* charBytes, textEncoding, nullptr, &paint, fallbackFontCreator);
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
        string_utf = string.c_str();
        if (textOut) {
            *textOut = (const char*)string_utf.c_str();
        }
        lengthOut = string_utf.size() * charBytes;
        return true;
    }
    return false;
}

static bool EllipsisText(const char text[], size_t length, SkTextEncoding textEncoding,
                         std::string& string_utf8,
                         std::u16string& string_utf16,
                         std::u32string& string_utf32,
                         bool bEndEllipsis, bool bPathEllipsis,
                         const SkFont& font, FallbackFontCreator fallbackFontCreator, const SkPaint& paint,
                         SkScalar destWidth,
                         const char** textOut, size_t& lengthOut)
{
    if (textEncoding == SkTextEncoding::kUTF8) {
        return EllipsisTextUTF<std::string>(text, length, SkTextEncoding::kUTF8,
                                            bEndEllipsis, bPathEllipsis,
                                            font, fallbackFontCreator, paint,
                                            destWidth,
                                            textOut, lengthOut,
                                            string_utf8,
                                            "...",
                                            "\\/");
    }
    else if (textEncoding == SkTextEncoding::kUTF16) {
        return EllipsisTextUTF<std::u16string>(text, length, SkTextEncoding::kUTF16,
                                            bEndEllipsis, bPathEllipsis,
                                            font, fallbackFontCreator, paint,
                                            destWidth,
                                            textOut, lengthOut,
                                            string_utf16,
                                            u"...",
                                            u"\\/");
    }
    else if (textEncoding == SkTextEncoding::kUTF32) {
        return EllipsisTextUTF<std::u32string>(text, length, SkTextEncoding::kUTF32,
                                            bEndEllipsis, bPathEllipsis,
                                            font, fallbackFontCreator, paint,
                                            destWidth,
                                            textOut, lengthOut,
                                            string_utf32,
                                            U"...",
                                            U"\\/");
    }
    return false;
}

static void TextBox_DrawText(SkTextBox* textBox, 
                             SkCanvas* canvas,
                             const char text[], size_t length, SkTextEncoding textEncoding, 
                             SkScalar x, SkScalar y,
                             const SkFont& font, const SkPaint& paint,
                             bool hasMoreText, bool isLastLine,
                             FallbackFontCreator fallbackFontCreator)
{

    //绘制一行文字
    SkRect boxRect;
    textBox->getBox(&boxRect);

    //绘制区域不足时，自动在末尾绘制省略号
    bool bEndEllipsis = textBox->getEndEllipsis();
    //绘制区域不足时，自动在绘制省略号代替文本
    //如果字符串包含反斜杠 (\\) 字符，在最后一个反斜杠之后保留尽可能多的文本。
    bool bPathEllipsis = textBox->getPathEllipsis();
    //字体属性：下划线
    bool bUnderline = textBox->getUnderline();
    //字体属性：删除线
    bool bStrikeOut = textBox->getStrikeOut();
    //单行模式
    bool isSingleLine = textBox->getLineMode() == TextBoxLineMode::kOneLine_Mode;

    if (!bEndEllipsis && !bPathEllipsis && !bUnderline && !bStrikeOut) {
        DrawSkiaText::DrawSimpleText(canvas, text, length, textEncoding, x, y, font, paint, fallbackFontCreator);
    }
    else {
        bool needEllipsis = false;
        if (bEndEllipsis || bPathEllipsis) {
            if (isSingleLine) {                
                //单行模式
                SkScalar textWidth = DrawSkiaText::MeasureText(font, text, length, textEncoding, nullptr, &paint, fallbackFontCreator);
                if ((x + textWidth) > boxRect.fRight) {
                    //文字超出边界，需要增加"..."替代无法显示的文字
                    needEllipsis = true;
                }
            }
            else {
                //多行模式
                if (bEndEllipsis && isLastLine && hasMoreText) {
                    //文字超出边界，需要增加"..."替代无法显示的文字
                    needEllipsis = true;
                }
            }
        }
        if(!needEllipsis && !bUnderline && !bStrikeOut) {
            DrawSkiaText::DrawSimpleText(canvas, text, length, textEncoding, x, y, font, paint, fallbackFontCreator);
        }
        else {
            std::string string_utf8;
            std::u16string string_utf16;
            std::u32string string_utf32;
            if (needEllipsis) {
                const char* textOut = nullptr;
                size_t lengthOut = 0;
                if (EllipsisText(text, length, textEncoding,
                                 string_utf8, string_utf16, string_utf32,
                                 bEndEllipsis, bPathEllipsis,
                                 font, fallbackFontCreator, paint,
                                 boxRect.fRight - x,
                                 &textOut, lengthOut)) {
                    //修改text和length的值，但不改变textEncoding
                    SkASSERT(textOut != nullptr);
                    SkASSERT(lengthOut != 0);
                    text = textOut;
                    length = lengthOut;
                }
            }
            //绘制文本
            DrawSkiaText::DrawSimpleText(canvas, text, length, textEncoding, x, y, font, paint, fallbackFontCreator);
            if (bUnderline || bStrikeOut) {
                SkScalar width = DrawSkiaText::MeasureText(font, text, length, textEncoding, nullptr, &paint, fallbackFontCreator);

                // Default fraction of the text size to use for a strike-through or underline.
                static constexpr SkScalar kLineThicknessFactor = (SK_Scalar1 / 18);
                // Fraction of the text size to raise the center of a strike-through line above
                // the baseline.
                const SkScalar kStrikeThroughOffset = (SK_Scalar1 * 65 / 252);
                // Fraction of the text size to lower an underline below the baseline.
                const SkScalar kUnderlineOffset = (SK_Scalar1 / 9);

                if (bStrikeOut) {
                    //绘制删除线
                    SkScalar thickness_factor = kLineThicknessFactor;
                    const SkScalar text_size = font.getSize();
                    const SkScalar height = text_size * thickness_factor;
                    const SkScalar top = y - text_size * kStrikeThroughOffset - height / 2;
                    SkScalar x_scalar = SkIntToScalar(x);
                    const SkRect r = SkRect::MakeLTRB(x_scalar, top, x_scalar + width, top + height);
                    canvas->drawRect(r, paint);
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
                    canvas->drawRect(r, paint);
                }
            }
        }
    }
}

///////////////////////////////////////////////////////////////////////////////

class CanvasVisitor : public SkTextBox::Visitor {
    SkCanvas* fCanvas;
    SkTextBox* fTextBox;
public:
    CanvasVisitor(SkCanvas* canvas, SkTextBox* textBox): 
         fCanvas(canvas)
        ,fTextBox(textBox) {
    }

    void operator()(const char text[], size_t length, SkTextEncoding textEncoding, 
                    SkScalar x, SkScalar y,
                    const SkFont& font, const SkPaint& paint,
                    bool hasMoreText, bool isLastLine,
                    FallbackFontCreator fallbackFontCreator) override
    {
        //调用单独封装的函数绘制文字，便于扩展
        TextBox_DrawText(fTextBox,
                         fCanvas,
                         text, length, textEncoding,
                         x, y,
                         font, paint,
                         hasMoreText, isLastLine,
                         fallbackFontCreator);
    }
};

void SkTextBox::setText(const char text[], size_t len, SkTextEncoding textEncoding, 
                        const SkFont& font, const SkPaint& paint,
                        FallbackFontCreator fallbackFontCreator) {
    fText = text;
    fLen = len;
    fTextEncoding = textEncoding;
    fPaint = &paint;
    fFont = &font;
    fFallbackFontCreator = fallbackFontCreator;
    SkASSERT(fallbackFontCreator != nullptr);

#ifdef _DEBUG
    //检查字符串序列是否正确
    if (textEncoding == SkTextEncoding::kUTF8) {
        SkASSERT(SkUTF8_CountUnichars(text, len) != -1);
    }
    else if (textEncoding == SkTextEncoding::kUTF16) {
        SkASSERT(SkUTF16_CountUnichars(text, len) != -1);
    }
    else if (textEncoding == SkTextEncoding::kUTF32) {
        SkASSERT(SkUTF32_CountUnichars(text, len) != -1);
    }
    else {
        SkASSERT(false);
    }
#endif
}

void SkTextBox::draw(SkCanvas* canvas, 
                     const char text[], size_t len, SkTextEncoding textEncoding, 
                     const SkFont& font, const SkPaint& paint,
                     FallbackFontCreator fallbackFontCreator) {
    setText(text, len, textEncoding, font, paint, fallbackFontCreator);
    draw(canvas);
}

void SkTextBox::draw(SkCanvas* canvas) {
    SkASSERT(canvas != nullptr);
    if (canvas == nullptr) {
        return;
    }

    SkASSERT(fText != nullptr);
    if (fText == nullptr) {
        return;
    }
    if (fLen == 0) {
        return;
    }
    SkASSERT((fTextEncoding == SkTextEncoding::kUTF8) ||
             (fTextEncoding == SkTextEncoding::kUTF16)||
             (fTextEncoding == SkTextEncoding::kUTF32));
    if ((fTextEncoding != SkTextEncoding::kUTF8)  &&
        (fTextEncoding != SkTextEncoding::kUTF16) &&
        (fTextEncoding != SkTextEncoding::kUTF32)) {
        return;
    }

    SkASSERT(fFont != nullptr);
    SkASSERT(fPaint != nullptr);
    if ((fPaint == nullptr) || (fFont == nullptr)) {
        return;
    }

#ifdef _DEBUG
    //检查字符串序列是否正确
    if (fTextEncoding == SkTextEncoding::kUTF8) {
        SkASSERT(SkUTF8_CountUnichars(fText, fLen) != -1);
    }
    else if (fTextEncoding == SkTextEncoding::kUTF16) {
        SkASSERT(SkUTF16_CountUnichars(fText, fLen) != -1);
    }
    else if (fTextEncoding == SkTextEncoding::kUTF32) {
        SkASSERT(SkUTF32_CountUnichars(fText, fLen) != -1);
    }
#endif
    int saveCount = 0;
    if (fClipBox) {
        saveCount = canvas->save();
        canvas->clipRect(fBox, true);
    }
    CanvasVisitor sink(canvas, this);
    this->visit(sink);
    if (fClipBox) {
        canvas->restoreToCount(saveCount);
    }
}

int32_t SkTextBox::countLines() const {
    return DrawSkiaText::CountLines(fText, fLen, fTextEncoding,
                                    *fFont, fFallbackFontCreator, *fPaint, fBox.width(),
                                     fLineMode);
}

SkScalar SkTextBox::getTextHeight() const {
    SkScalar spacing = fFont->getSize() * fSpacingMul + fSpacingAdd;
    return this->countLines() * spacing;
}

}//namespace ui
