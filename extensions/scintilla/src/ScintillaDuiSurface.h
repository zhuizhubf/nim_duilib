#ifndef DUILIB_EXT_SCINTILLA_SCINTILLA_DUI_SURFACE_H_
#define DUILIB_EXT_SCINTILLA_SCINTILLA_DUI_SURFACE_H_

#include <cstddef>
#include <cstdint>
#include <memory>
#include <optional>
#include <stdexcept>
#include <string>
#include <string_view>
#include <utility>
#include <vector>

#include "duilib/Core/UiPoint.h"

// clang-format off
//Scintilla 头文件有包含顺序要求，禁止自动排序（ScintillaTypes 必须最先包含）
#include "Sci_Position.h"
#include "ScintillaTypes.h"
#include "ScintillaStructures.h"
#include "ScintillaMessages.h"
#include "Scintilla.h"
#include "Geometry.h"
#include "Platform.h"
// clang-format on

namespace ui {
class IFont;
class IRender;
class ITextShaper;
class UiColor;
struct TextGlyphInfo;
} // namespace ui

namespace duilib::ext::scintilla {

class ScintillaDui;
class ScintillaDuiFont;

/** Scintilla Surface 的 DUI 实现（对应 Qt 平台层的 SurfaceImpl）。
 *
 *  绘制映射：ui::IRender；
 *  字形度量与字形绘制映射：ui::ITextShaper（按基线逐个字形）。
 *
 *  度量与绘制使用完全相同的字形推进量，因此光标位置、选区矩形、
 *  语法高亮背景与文本像素严格对齐（Qt 平台层通过 QTextLayout 的
 *  字形运行实现同样的效果）。
 */
class ScintillaDuiSurface final : public Scintilla::Internal::Surface
{
public:
    ScintillaDuiSurface();
    explicit ScintillaDuiSurface(ScintillaDui *pOwner);
    ScintillaDuiSurface(ScintillaDui *pOwner, ui::IRender *pRender);
    ScintillaDuiSurface(const ScintillaDuiSurface &) = delete;
    ScintillaDuiSurface &operator=(const ScintillaDuiSurface &) = delete;
    ~ScintillaDuiSurface() override;

public:
    virtual void Init(Scintilla::Internal::WindowID wid) override;
    virtual void Init(Scintilla::Internal::SurfaceID sid, Scintilla::Internal::WindowID wid) override;
    virtual std::unique_ptr<Scintilla::Internal::Surface> AllocatePixMap(
        int width, int height) override;
    virtual void SetMode(Scintilla::Internal::SurfaceMode mode) override;
    virtual void Release() noexcept override;
    virtual int SupportsFeature(Scintilla::Supports feature) noexcept override;
    virtual bool Initialised() override;
    virtual int LogPixelsY() override;
    virtual int PixelDivisions() override;
    virtual int DeviceHeightFont(int points) override;
    virtual void LineDraw(
        Scintilla::Internal::Point start,
        Scintilla::Internal::Point end,
        Scintilla::Internal::Stroke stroke) override;
    virtual void PolyLine(
        const Scintilla::Internal::Point *pts,
        size_t npts,
        Scintilla::Internal::Stroke stroke) override;
    virtual void Polygon(
        const Scintilla::Internal::Point *pts,
        size_t npts,
        Scintilla::Internal::FillStroke fillStroke) override;
    virtual void RectangleDraw(
        Scintilla::Internal::PRectangle rc, Scintilla::Internal::FillStroke fillStroke) override;
    virtual void RectangleFrame(
        Scintilla::Internal::PRectangle rc, Scintilla::Internal::Stroke stroke) override;
    virtual void FillRectangle(
        Scintilla::Internal::PRectangle rc, Scintilla::Internal::Fill fill) override;
    virtual void FillRectangleAligned(
        Scintilla::Internal::PRectangle rc, Scintilla::Internal::Fill fill) override;
    virtual void FillRectangle(
        Scintilla::Internal::PRectangle rc, Scintilla::Internal::Surface &surfacePattern) override;
    virtual void RoundedRectangle(
        Scintilla::Internal::PRectangle rc, Scintilla::Internal::FillStroke fillStroke) override;
    virtual void AlphaRectangle(
        Scintilla::Internal::PRectangle rc,
        Scintilla::Internal::XYPOSITION cornerSize,
        Scintilla::Internal::FillStroke fillStroke) override;
    virtual void GradientRectangle(
        Scintilla::Internal::PRectangle rc,
        const std::vector<Scintilla::Internal::ColourStop> &stops,
        GradientOptions options) override;
    virtual void DrawRGBAImage(
        Scintilla::Internal::PRectangle rc,
        int width,
        int height,
        const unsigned char *pixelsImage) override;
    virtual void Ellipse(
        Scintilla::Internal::PRectangle rc, Scintilla::Internal::FillStroke fillStroke) override;
    virtual void Stadium(
        Scintilla::Internal::PRectangle rc,
        Scintilla::Internal::FillStroke fillStroke,
        Ends ends) override;
    virtual void Copy(
        Scintilla::Internal::PRectangle rc,
        Scintilla::Internal::Point from,
        Scintilla::Internal::Surface &surfaceSource) override;
    virtual std::unique_ptr<Scintilla::Internal::IScreenLineLayout> Layout(
        const Scintilla::Internal::IScreenLine *screenLine) override;

    virtual void DrawTextNoClip(
        Scintilla::Internal::PRectangle rc,
        const Scintilla::Internal::Font *font,
        Scintilla::Internal::XYPOSITION ybase,
        std::string_view text,
        Scintilla::Internal::ColourRGBA fore,
        Scintilla::Internal::ColourRGBA back) override;
    virtual void DrawTextClipped(
        Scintilla::Internal::PRectangle rc,
        const Scintilla::Internal::Font *font,
        Scintilla::Internal::XYPOSITION ybase,
        std::string_view text,
        Scintilla::Internal::ColourRGBA fore,
        Scintilla::Internal::ColourRGBA back) override;
    virtual void DrawTextTransparent(
        Scintilla::Internal::PRectangle rc,
        const Scintilla::Internal::Font *font,
        Scintilla::Internal::XYPOSITION ybase,
        std::string_view text,
        Scintilla::Internal::ColourRGBA fore) override;
    virtual void MeasureWidths(
        const Scintilla::Internal::Font *font,
        std::string_view text,
        Scintilla::Internal::XYPOSITION *positions) override;
    virtual Scintilla::Internal::XYPOSITION WidthText(
        const Scintilla::Internal::Font *font, std::string_view text) override;

    virtual void DrawTextNoClipUTF8(
        Scintilla::Internal::PRectangle rc,
        const Scintilla::Internal::Font *font,
        Scintilla::Internal::XYPOSITION ybase,
        std::string_view text,
        Scintilla::Internal::ColourRGBA fore,
        Scintilla::Internal::ColourRGBA back) override;
    virtual void DrawTextClippedUTF8(
        Scintilla::Internal::PRectangle rc,
        const Scintilla::Internal::Font *font,
        Scintilla::Internal::XYPOSITION ybase,
        std::string_view text,
        Scintilla::Internal::ColourRGBA fore,
        Scintilla::Internal::ColourRGBA back) override;
    virtual void DrawTextTransparentUTF8(
        Scintilla::Internal::PRectangle rc,
        const Scintilla::Internal::Font *font,
        Scintilla::Internal::XYPOSITION ybase,
        std::string_view text,
        Scintilla::Internal::ColourRGBA fore) override;
    virtual void MeasureWidthsUTF8(
        const Scintilla::Internal::Font *font,
        std::string_view text,
        Scintilla::Internal::XYPOSITION *positions) override;
    virtual Scintilla::Internal::XYPOSITION WidthTextUTF8(
        const Scintilla::Internal::Font *font, std::string_view text) override;

    virtual Scintilla::Internal::XYPOSITION Ascent(const Scintilla::Internal::Font *font) override;
    virtual Scintilla::Internal::XYPOSITION Descent(const Scintilla::Internal::Font *font) override;
    virtual Scintilla::Internal::XYPOSITION InternalLeading(
        const Scintilla::Internal::Font *font) override;
    virtual Scintilla::Internal::XYPOSITION Height(const Scintilla::Internal::Font *font) override;
    virtual Scintilla::Internal::XYPOSITION AverageCharWidth(
        const Scintilla::Internal::Font *font) override;

    virtual void SetClip(Scintilla::Internal::PRectangle rc) override;
    virtual void PopClip() override;
    virtual void FlushCachedState() override;
    virtual void FlushDrawing() override;

    /** 设置内容区在窗口客户区中的原点。
     *
     *  Scintilla 使用内容区的局部坐标绘制，而 ui::IRender 使用窗口客户区坐标。
     *  这里把偏移量直接叠加到传给 Render 的坐标上，而不使用 OffsetWindowOrg：
     *  核心库的 Skia 与 GDI 后端对 OffsetWindowOrg 的符号约定相反，直接依赖会导致
     *  绘制原点偏移（历史问题：光标/文本整体上移、残影）。
     */
    void SetPositionOffset(const ui::UiPoint &ptOrigin) { m_ptOrigin = ptOrigin; }
    const ui::UiPoint &GetPositionOffset() const noexcept { return m_ptOrigin; }

private:
    /** 获取当前绘制目标关联的文本塑形接口（随绘制目标变化自动切换） */
    ui::ITextShaper *GetTextShaper();
    void EnsureRender();
    void FillRectImpl(Scintilla::Internal::PRectangle rc, Scintilla::Internal::ColourRGBA colour);
    void StrokeRect(
        Scintilla::Internal::PRectangle rc,
        Scintilla::Internal::ColourRGBA colour,
        Scintilla::Internal::XYPOSITION width);
    void DrawTextImpl(
        Scintilla::Internal::PRectangle rc,
        const Scintilla::Internal::Font *font,
        Scintilla::Internal::XYPOSITION ybase,
        std::string_view text,
        Scintilla::Internal::ColourRGBA fore,
        Scintilla::Internal::ColourRGBA back,
        bool bDrawBackground);
    /** 按字形逐个度量文本宽度，返回总宽度 */
    Scintilla::Internal::XYPOSITION MeasureWithShaper(
        const Scintilla::Internal::Font *font,
        std::string_view text,
        Scintilla::Internal::XYPOSITION *positions);
    /** 无法获取文本塑形接口时的兼容度量方式 */
    Scintilla::Internal::XYPOSITION MeasureWithRender(
        const Scintilla::Internal::Font *font,
        std::string_view text,
        Scintilla::Internal::XYPOSITION *positions);
    bool GetFontMetrics(const Scintilla::Internal::Font *font, float &fAscent, float &fDescent);
    /** 解析字形：当前字体不支持时，使用系统 CJK 字体作为回退 */
    bool ResolveGlyphWithFallback(
        const Scintilla::Internal::Font *font, uint32_t unicodeChar, ui::TextGlyphInfo &glyph);
    /** 获取与指定字体匹配的 CJK 回退字体 */
    const Scintilla::Internal::Font *GetCjkFallbackFont(const Scintilla::Internal::Font *font);

private:
    ScintillaDui *m_pOwner = nullptr;
    ui::IRender *m_pRender = nullptr;
    /** 内容区在窗口客户区中的原点（局部坐标 -> 客户区坐标） */
    ui::UiPoint m_ptOrigin;
    std::shared_ptr<ui::IRender> m_ownedRender;
    ui::ITextShaper *m_pTextShaper = nullptr;
    ui::IRender *m_pTextShaperRender = nullptr;
    std::unique_ptr<ui::ITextShaper> m_ownedTextShaper;
    /** CJK 回退字体缓存（键为原始字体） */
    std::vector<
        std::pair<const Scintilla::Internal::Font *, std::shared_ptr<Scintilla::Internal::Font>>>
        m_cjkFallbackFonts;
    std::vector<int32_t> m_clipStates;
    Scintilla::Internal::SurfaceMode m_mode;
};

} // namespace duilib::ext::scintilla

#endif // DUILIB_EXT_SCINTILLA_SCINTILLA_DUI_SURFACE_H_
