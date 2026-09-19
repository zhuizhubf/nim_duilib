#ifndef UI_TEXT_TEXT_LAYOUT_H_
#define UI_TEXT_TEXT_LAYOUT_H_

#include "text/ITextShaper.h"

namespace ui {

/** 后端无关的文本布局辅助类
*/
class DUILIB_API TextLayout
{
public:
    /** 判断两组 RichText 数据是否一致
    */
    static bool IsRichTextDataEqual(
        const std::vector<RichTextData> &first, const std::vector<RichTextData> &second);

    /** 计算普通字符串的绘制区域
    */
    static UiRect MeasureString(
        ITextShaper &textShaper, const DString &strText, const MeasureStringParam &measureParam);

    /** 绘制普通字符串
    */
    static void DrawString(
        ITextShaper &textShaper,
        IRender *pRender,
        const DString &strText,
        const DrawStringParam &drawParam);

    /** 计算富文本的绘制区域
    */
    static void MeasureRichText(
        ITextShaper &textShaper,
        const UiRect &textRect,
        const UiSize &szScrollOffset,
        const std::vector<RichTextData> &richTextData,
        std::vector<std::vector<UiRect>> *pRichTextRects);

    /** 计算富文本并输出行/字符信息
    */
    static void MeasureRichText2(
        ITextShaper &textShaper,
        const UiRect &textRect,
        const UiSize &szScrollOffset,
        const std::vector<RichTextData> &richTextData,
        RichTextLineInfoParam *pLineInfoParam,
        std::vector<std::vector<UiRect>> *pRichTextRects);

    /** 绘制富文本
    */
    static void DrawRichText(
        ITextShaper &textShaper,
        IRender *pRender,
        const UiRect &textRect,
        const UiSize &szScrollOffset,
        const std::vector<RichTextData> &richTextData,
        uint8_t uFade,
        std::vector<std::vector<UiRect>> *pRichTextRects);
};

} // namespace ui

#endif // UI_TEXT_TEXT_LAYOUT_H_
