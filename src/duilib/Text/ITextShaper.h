#ifndef UI_TEXT_ITEXT_SHAPER_H_
#define UI_TEXT_ITEXT_SHAPER_H_

#include "duilib/Render/IRender.h"

namespace ui
{

/** 字体度量信息
*/
struct TextFontMetrics
{
    float m_fAscent = 0.0f;
    float m_fDescent = 0.0f;
    float m_fHeight = 0.0f;
};

/** 单个字形信息
*/
struct TextGlyphInfo
{
    /** 实际使用的字体（可能是回退字体）
    */
    IFont* m_pFont = nullptr;

    /** 字形索引
    */
    uint16_t m_glyphId = 0;

    /** 对应的 Unicode 字符
    */
    uint32_t m_unicodeChar = 0;

    /** 字符宽度
    */
    float m_fAdvance = 0.0f;

    /** 字符边界
    */
    UiRectF m_bounds;

    /** 当前字体是否不包含该字符
    */
    bool m_bMissing = false;
};

/** 文本字形适配接口
*/
class DUILIB_API ITextShaper
{
public:
    virtual ~ITextShaper() = default;

    /** 根据字体描述创建或获取缓存的字体对象
    */
    virtual IFont* CreateFont(const UiFont& fontInfo) = 0;

    /** 获取字体度量
    */
    virtual bool GetFontMetrics(const IFont* pFont, TextFontMetrics& metrics) = 0;

    /** 解析一个 Unicode 字符对应的字形（包含字体回退）
    */
    virtual bool ResolveGlyph(const IFont* pFont, uint32_t unicodeChar, TextGlyphInfo& glyph, bool bUseDefaultCharWhenFailed) = 0;

    /** 绘制一个已经解析好的字形
    */
    virtual void DrawGlyph(const TextGlyphInfo& glyph, float x, float y, UiColor textColor, uint8_t uFade) = 0;
};

} // namespace ui

#endif // UI_TEXT_ITEXT_SHAPER_H_
