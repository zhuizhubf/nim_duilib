#ifndef UI_RENDER_SKIA_DRAW_SKIA_TEXT_H_
#define UI_RENDER_SKIA_DRAW_SKIA_TEXT_H_

#include "duilib/Render/IRender.h"

#include "SkiaHeaderBegin.h"
#include "include/core/SkCanvas.h"
#include "include/core/SkPaint.h"
#include "include/core/SkFont.h"
#include "SkiaHeaderEnd.h"

#include <vector>
#include <functional>

class SkFont;

namespace ui 
{
/** 回退字体管理器
*/
typedef std::function<const SkFont* (uint32_t ch)> FallbackFontCreator;

/** 换行模式
*/
enum TextBoxLineMode {
    kOneLine_Mode,   //单行模式
    kWordBreak_Mode, //Word Break模式，多行模式，按单词分行（默认）
    kCharBreak_Mode, //Char Break模式，多行模式，按字符分行（英文单词和数字都会被换行切分）
    kModeCount       //有效值总个数
};

/** 调用Canvas绘制Skia的文本，二次封装Skia文本相关函数以支持回退字体的功能
 *  该功能的目标是支持emoji文字和扩展汉字（Unicode扩展B/C区等其他区域的字符）的显示
 */
class DrawSkiaText
{
public:
    /** 获取回退字体管理器
    */
    static IFallbackFontMgr* GetFallbackFontMgr(const IFont* pFont);

    /** 创建指定字体的回退字体接口
    * @param [in] pFont 当前字体接口
    * @param [in] unicodeChar UTF32字符，如果为0表示不支持字符检测
    * @return 返回对应的回退字体接口
    */
    static const SkFont* CreateFallbackFont(const IFont* pFont, uint32_t unicodeChar);

public:
    /** 评估文字的宽度和绘制区域，支持字体回退
    * @return 返回字符宽度，如果不支持绘制该字符，返回0
    */
    static SkScalar MeasureText(const SkFont& font, DUTF32Char ch,
                                SkRect* bounds, const SkPaint* paint,
                                const IFont* pFont);

    /** 评估文字的宽度和绘制区域，支持字体回退
    * @return 返回字符宽度，如果不支持绘制该字符，返回0
    */
    static SkScalar MeasureText(const SkFont& font, DUTF32Char ch,
                                SkRect* bounds, const SkPaint* paint,
                                FallbackFontCreator fallbackFontCreator);

    /** 评估文字的宽度和绘制区域，支持字体回退
    * @return 返回字符宽度
    */
    static SkScalar MeasureText(const SkFont& font, const void* text, size_t byteLength, SkTextEncoding textEncoding,
                                SkRect* bounds, const SkPaint* paint,
                                const IFont* pFont);

    /** 评估文字的宽度和绘制区域，支持字体回退
    * @return 返回字符宽度
    */
    static SkScalar MeasureText(const SkFont& font, const void* text, size_t byteLength, SkTextEncoding textEncoding,
                                SkRect* bounds, const SkPaint* paint,
                                FallbackFontCreator fallbackFontCreator);

public:
    /** 绘制文本，支持字体回退
    */
    static SkScalar DrawSimpleText(SkCanvas* skCanvas, DUTF32Char ch, SkScalar x, SkScalar y,
                                   const SkFont& font, const SkPaint& paint,
                                   const IFont* pFont);

    /** 绘制文本，支持字体回退
    */
    static SkScalar DrawSimpleText(SkCanvas* skCanvas, DUTF32Char ch, SkScalar x, SkScalar y,
                                   const SkFont& font, const SkPaint& paint,
                                   FallbackFontCreator fallbackFontCreator);

    /** 绘制文本，支持字体回退
    */
    static void DrawSimpleText(SkCanvas* skCanvas, const void* text, size_t byteLength, SkTextEncoding textEncoding,
                               SkScalar x, SkScalar y,
                               const SkFont& font, const SkPaint& paint,
                               const IFont* pFont);

    /** 绘制文本，支持字体回退
    */
    static void DrawSimpleText(SkCanvas* skCanvas, const void* text, size_t byteLength, SkTextEncoding textEncoding,
                               SkScalar x, SkScalar y,
                               const SkFont& font, const SkPaint& paint,
                               FallbackFontCreator fallbackFontCreator);

public:
    //原始函数：SkPaint::breakText 这个函数在最新的Skia代码里面已经删除了，这个是自己实现的版本。
    /** 返回适合最大宽度的文本字节
     * @param [in] text 文本字符串的起始地址
     * @param [in] len 文本字符串的长度（字节）
     * @param [in] textEncoding 文本的编码, SkTextEncoding::kUTF8 or kUTF16 or kUTF32
     * @param [in] font 字体
     * @param [in] fallbackFontCreator 回退字体管理器
     * @param [in] paint 绘制属性
     * @param [in] maxWidth 绘制的最大宽度
     * @param [out] measuredWidth  返回估算的绘制宽度，小于或等于 maxWidth
     * @param [out] measuredHeight 返回估算的绘制高度
     * @return 返回本行可绘制的字节数，总是小于或者等于byteLength
     */
    static size_t breakText(const void* text, size_t byteLength, SkTextEncoding textEncoding,
                            const SkFont& font, FallbackFontCreator fallbackFontCreator,
                            const SkPaint& paint, SkScalar maxWidth,
                            SkScalar* measuredWidth = nullptr,
                            SkScalar* measuredHeight = nullptr);

    /** 特殊版本，临时变量（glyphs，glyphChars，glyphWidths）由外部提供，以改进执行性能
    * @param [out] glyphs 绘制了多少个Glyph字符(临时变量)
    * @param [out] glyphChars 返回每个Glyph字符占几个输入字符(临时变量)
    * @param [out] glyphWidths 返回每个Glyph字符绘制的宽度(临时变量)
    * @param [out] glyphCharList 返回每个glyph字符由几个输入字符构成的(返回给应用层)
    * @param [out] glyphWidthList 返回每个glyph字符的输出宽度值(返回给应用层)
    */
    static size_t breakText(const void* text, size_t byteLength, SkTextEncoding textEncoding,
                            const SkFont& font, FallbackFontCreator fallbackFontCreator,
                            const SkPaint& paint, SkScalar maxWidth,
                            SkScalar* measuredWidth, SkScalar* measuredHeight,
                            std::vector<SkGlyphID>& glyphs,
                            std::vector<uint8_t>& glyphChars,
                            std::vector<SkScalar>& glyphWidths,
                            std::vector<uint8_t>* glyphCharList,
                            std::vector<SkScalar>* glyphWidthList);

public:
    /** 统计绘制需要多少行
     * @param [in] text 文本字符串的起始地址
     * @param [in] len 文本字符串的长度（字节）
     * @param [in] textEncoding 文本的编码, SkTextEncoding::kUTF8 or kUTF16 or kUTF32
     * @param [in] font 字体
     * @param [in] fallbackFontCreator 回退字体管理器
     * @param [in] paint 绘制属性
     * @param [in] width 绘制区域的宽度
     * @param [in] lineMode 换行模式
     * @param [out] lineLenList 返回每行文本数据的长度（字节）
     */
    static int32_t CountLines(const char text[], size_t len, SkTextEncoding textEncoding,
                              const SkFont& font, FallbackFontCreator fallbackFontCreator,
                              const SkPaint& paint, SkScalar width, TextBoxLineMode lineMode,
                              std::vector<size_t>* lineLenList = nullptr);


    /** 返回适合最大宽度的文本字节
     * @param [in] text 文本字符串的起始地址
     * @param [in] len 文本字符串的长度（字节）
     * @param [in] textEncoding 文本的编码, SkTextEncoding::kUTF8 or kUTF16 or kUTF32
     * @param [in] font 字体
     * @param [in] fallbackFontCreator 回退字体管理器
     * @param [in] paint 绘制属性
     * @param [in] margin 绘制区域的宽度
     * @param [in] lineMode 换行模式
     * @param [out] trailing 返回尾部的字符占几个字节（字节）
     */
    static size_t Linebreak(const char text[], const char stop[], SkTextEncoding textEncoding,
                            const SkFont& font, FallbackFontCreator fallbackFontCreator, const SkPaint& paint,
                            SkScalar margin, TextBoxLineMode lineMode,
                            size_t* trailing = nullptr);

private:
    /** 获取UTF32字符串，内部操作时按UTF32编码处理
    */
    static UTF32String GetDrawStringUTF32(const void* text, size_t byteLength, SkTextEncoding textEncoding);

    /** 将文本转换为Glyphs
    * @param [in] text 文本的起始地址
    * @param [in] byteLength 文本长度（字节数）
    * @param [in] textEncoding 文本编码SkTextEncoding::kUTF8 or kUTF16 or kUTF32
    * @param [in] font 字体
    * @param [out] glyphs 转换结果Glyphs
    * @param [out] 每个字符占的字节数
    */
    static bool TextToGlyphs(const void* text, size_t byteLength, SkTextEncoding textEncoding, 
                             const SkFont& font, std::vector<SkGlyphID>& glyphs, size_t& charBytes);

    /** 计算每个文本的每个字符所占的字节数
    * @param [in] text 文本的起始地址
    * @param [in] byteLength 文本长度（字节数）
    * @param [in] textEncoding 文本编码SkTextEncoding::kUTF8 or kUTF16 or kUTF32
    * @param [out] glyphs 转换结果Glyphs
    */
    static bool TextGlyphChars(const void* text, size_t byteLength, SkTextEncoding textEncoding,
                               std::vector<uint8_t>& glyphChars);
};

} // namespace ui

#endif // UI_RENDER_SKIA_DRAW_SKIA_TEXT_H_
