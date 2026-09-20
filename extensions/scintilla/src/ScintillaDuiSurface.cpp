#include "ScintillaDuiSurface.h"

#include "ScintillaDui.h"
#include "ScintillaDuiTextShaper.h"

#include "duilib/Core/GlobalManager.h"
#include "duilib/Core/UiColor.h"
#include "duilib/Core/UiFont.h"
#include "duilib/Core/UiPoint.h"
#include "duilib/Core/UiRect.h"
#include "duilib/Utils/StringConvert.h"
#include "render/IRender.h"
#include "text/ITextShaper.h"

#include <algorithm>
#include <cmath>
#include <cstring>

namespace duilib::ext::scintilla {

using namespace Scintilla::Internal;

namespace {

ui::UiColor ToUiColor(ColourRGBA colour)
{
    return ui::UiColor(colour.GetAlpha(), colour.GetRed(), colour.GetGreen(), colour.GetBlue());
}

ui::UiRect ToUiRect(PRectangle rc, const ui::UiPoint &ptOrigin)
{
    return ui::UiRect(
        static_cast<int32_t>(rc.left) + ptOrigin.x,
        static_cast<int32_t>(rc.top) + ptOrigin.y,
        static_cast<int32_t>(rc.right) + ptOrigin.x,
        static_cast<int32_t>(rc.bottom) + ptOrigin.y);
}

ui::UiRectF ToUiRectF(PRectangle rc, const ui::UiPoint &ptOrigin)
{
    return ui::UiRectF(
        static_cast<float>(rc.left) + static_cast<float>(ptOrigin.x),
        static_cast<float>(rc.top) + static_cast<float>(ptOrigin.y),
        static_cast<float>(rc.right) + static_cast<float>(ptOrigin.x),
        static_cast<float>(rc.bottom) + static_cast<float>(ptOrigin.y));
}

/** 一个 UTF-8 序列的起始字节对应的字节数 */
size_t Utf8CodePointLength(unsigned char lead) noexcept
{
    if ((lead & 0x80) == 0) {
        return 1;
    }
    if ((lead & 0xE0) == 0xC0) {
        return 2;
    }
    if ((lead & 0xF0) == 0xE0) {
        return 3;
    }
    if ((lead & 0xF8) == 0xF0) {
        return 4;
    }
    return 1; //非法起始字节，按单字节处理
}

/** 解析一个 UTF-8 字符，返回 Unicode 码点，consumed 返回实际消耗的字节数 */
uint32_t DecodeUtf8(const char *text, size_t length, size_t &consumed) noexcept
{
    const unsigned char *bytes = reinterpret_cast<const unsigned char *>(text);
    size_t codePointLength = std::min(Utf8CodePointLength(bytes[0]), length);
    if (codePointLength < 2) {
        consumed = 1;
        return bytes[0];
    }
    //校验续字节，避免解析出非法码点
    for (size_t i = 1; i < codePointLength; ++i) {
        if ((bytes[i] & 0xC0) != 0x80) {
            consumed = 1;
            return bytes[0];
        }
    }
    uint32_t codePoint = 0;
    if (codePointLength == 2) {
        codePoint = ((bytes[0] & 0x1F) << 6) | (bytes[1] & 0x3F);
    } else if (codePointLength == 3) {
        codePoint = ((bytes[0] & 0x0F) << 12) | ((bytes[1] & 0x3F) << 6) | (bytes[2] & 0x3F);
    } else {
        codePoint = ((bytes[0] & 0x07) << 18) | ((bytes[1] & 0x3F) << 12) | ((bytes[2] & 0x3F) << 6)
                    | (bytes[3] & 0x3F);
    }
    consumed = codePointLength;
    return codePoint;
}

} // namespace

} // namespace duilib::ext::scintilla

// -----------------------------------------------------------------------------
// Scintilla 平台接口：字体
// -----------------------------------------------------------------------------

namespace duilib::ext::scintilla {

/** Scintilla 字体对象：封装 Duilib 的字体接口（对应 Qt 的 QFont） */
class ScintillaDuiFont final : public Scintilla::Internal::Font
{
public:
    ScintillaDuiFont(std::unique_ptr<ui::IFont> pFont, const ui::UiFont &fontInfo)
        : m_pFont(std::move(pFont))
        , m_fontInfo(fontInfo)
    {}

    ui::IFont *GetFont() const noexcept { return m_pFont.get(); }
    const ui::UiFont &GetFontInfo() const noexcept { return m_fontInfo; }

private:
    std::unique_ptr<ui::IFont> m_pFont;
    ui::UiFont m_fontInfo;
};

/** 把 Scintilla 的 Font 接口转换为内部的字体对象 */
const ScintillaDuiFont *ToDuiFont(const Scintilla::Internal::Font *font) noexcept
{
    return dynamic_cast<const ScintillaDuiFont *>(font);
}

} // namespace duilib::ext::scintilla

namespace Scintilla::Internal {

std::shared_ptr<Font> Font::Allocate(const FontParameters &fp)
{
    ui::IRenderFactory *pFactory = ui::GlobalManager::Instance().GetRenderFactory();
    if (pFactory == nullptr) {
        return nullptr;
    }
    std::unique_ptr<ui::IFont> font(pFactory->CreateIFont());
    if (font == nullptr) {
        return nullptr;
    }

    //Scintilla 传入的 size 是设备像素高度（已在 DeviceHeightFont 中做完 DPI 换算）
    ui::UiFont fontInfo;
    fontInfo.m_fontName = ui::StringConvert::UTF8ToT(fp.faceName != nullptr ? fp.faceName : "");
    fontInfo.m_fontSize = std::max<int32_t>(1, static_cast<int32_t>(std::lround(fp.size)));
    fontInfo.m_bBold
        = (static_cast<int>(fp.weight) >= static_cast<int>(Scintilla::FontWeight::Bold));
    fontInfo.m_bItalic = fp.italic;
    if (!font->InitFont(fontInfo)) {
        return nullptr;
    }
    return std::make_shared<duilib::ext::scintilla::ScintillaDuiFont>(std::move(font), fontInfo);
}

std::unique_ptr<Surface> Surface::Allocate(Scintilla::Technology /*technology*/)
{
    return std::make_unique<duilib::ext::scintilla::ScintillaDuiSurface>();
}

} // namespace Scintilla::Internal

namespace duilib::ext::scintilla {

ScintillaDuiSurface::ScintillaDuiSurface() = default;

ScintillaDuiSurface::ScintillaDuiSurface(ScintillaDui *pOwner)
    : m_pOwner(pOwner)
{}

ScintillaDuiSurface::ScintillaDuiSurface(ScintillaDui *pOwner, ui::IRender *pRender)
    : m_pOwner(pOwner)
    , m_pRender(pRender)
{}

ScintillaDuiSurface::~ScintillaDuiSurface() = default;

void ScintillaDuiSurface::Init(WindowID wid)
{
    //wid 是 ScintillaDuiWindowHandle* 基类指针（多继承下有偏移），必须用 dynamic_cast 还原
    m_pOwner = dynamic_cast<ScintillaDui *>(static_cast<ScintillaDuiWindowHandle *>(wid));
}

void ScintillaDuiSurface::Init(SurfaceID sid, WindowID wid)
{
    m_pOwner = dynamic_cast<ScintillaDui *>(static_cast<ScintillaDuiWindowHandle *>(wid));
    m_pRender = static_cast<ui::IRender *>(sid);
    m_pTextShaper = nullptr;
    m_pTextShaperRender = nullptr;
    m_ownedTextShaper.reset();
}

std::unique_ptr<Surface> ScintillaDuiSurface::AllocatePixMap(int width, int height)
{
    std::unique_ptr<ScintillaDuiSurface> surface = std::make_unique<ScintillaDuiSurface>(m_pOwner);
    if (m_pOwner != nullptr) {
        surface->m_ownedRender = m_pOwner->CreateOffscreenRender(width, height);
        surface->m_pRender = surface->m_ownedRender.get();
    }
    return surface;
}

void ScintillaDuiSurface::SetMode(SurfaceMode mode)
{
    m_mode = mode;
}

void ScintillaDuiSurface::Release() noexcept
{
    m_pRender = nullptr;
    m_ownedRender.reset();
    m_pTextShaper = nullptr;
    m_pTextShaperRender = nullptr;
    m_ownedTextShaper.reset();
    m_clipStates.clear();
}

int ScintillaDuiSurface::SupportsFeature(Scintilla::Supports /*feature*/) noexcept
{
    return 0;
}

bool ScintillaDuiSurface::Initialised()
{
    EnsureRender();
    return m_pRender != nullptr;
}

int ScintillaDuiSurface::LogPixelsY()
{
    if (m_pOwner != nullptr) {
        return m_pOwner->LogPixelsYDui();
    }
    return 96;
}

int ScintillaDuiSurface::PixelDivisions()
{
    return 1;
}

int ScintillaDuiSurface::DeviceHeightFont(int points)
{
    const int logPixelsY = LogPixelsY();
    const int fontHeight = static_cast<int>(
        std::ceil(static_cast<double>(points) * logPixelsY / 72.0));
    return std::max(fontHeight, 1);
}

void ScintillaDuiSurface::LineDraw(Point start, Point end, Stroke stroke)
{
    EnsureRender();
    if (m_pRender == nullptr) {
        return;
    }
    m_pRender->DrawLine(
        ui::UiPointF(static_cast<float>(start.x), static_cast<float>(start.y)),
        ui::UiPointF(static_cast<float>(end.x), static_cast<float>(end.y)),
        ToUiColor(stroke.colour),
        static_cast<float>(std::max<XYPOSITION>(1.0, stroke.width)));
}

void ScintillaDuiSurface::PolyLine(const Point *pts, size_t npts, Stroke stroke)
{
    if ((pts == nullptr) || (npts < 2)) {
        return;
    }
    for (size_t i = 1; i < npts; ++i) {
        LineDraw(pts[i - 1], pts[i], stroke);
    }
}

void ScintillaDuiSurface::Polygon(const Point *pts, size_t npts, FillStroke fillStroke)
{
    EnsureRender();
    if ((m_pRender == nullptr) || (pts == nullptr) || (npts < 3)) {
        return;
    }
    ui::IRenderFactory *pFactory = ui::GlobalManager::Instance().GetRenderFactory();
    if (pFactory == nullptr) {
        return;
    }
    std::unique_ptr<ui::IPath> path(pFactory->CreatePath());
    if (path == nullptr) {
        return;
    }
    std::vector<ui::UiPointF> points;
    points.reserve(npts);
    for (size_t i = 0; i < npts; ++i) {
        points.emplace_back(
            static_cast<float>(pts[i].x) + static_cast<float>(m_ptOrigin.x),
            static_cast<float>(pts[i].y) + static_cast<float>(m_ptOrigin.y));
    }
    path->AddPolygon(points.data(), static_cast<int32_t>(points.size()));
    if (fillStroke.fill.colour.GetAlpha() != 0) {
        std::unique_ptr<ui::IBrush> brush(pFactory->CreateBrush(ToUiColor(fillStroke.fill.colour)));
        if (brush != nullptr) {
            m_pRender->FillPath(path.get(), brush.get());
        }
    }
    if (fillStroke.stroke.colour.GetAlpha() != 0) {
        std::unique_ptr<ui::IPen> pen(pFactory->CreatePen(
            ToUiColor(fillStroke.stroke.colour), static_cast<float>(fillStroke.stroke.width)));
        if (pen != nullptr) {
            m_pRender->DrawPath(path.get(), pen.get());
        }
    }
}

void ScintillaDuiSurface::RectangleDraw(PRectangle rc, FillStroke fillStroke)
{
    FillRectImpl(rc, fillStroke.fill.colour);
    StrokeRect(rc, fillStroke.stroke.colour, fillStroke.stroke.width);
}

void ScintillaDuiSurface::RectangleFrame(PRectangle rc, Stroke stroke)
{
    StrokeRect(rc, stroke.colour, stroke.width);
}

void ScintillaDuiSurface::FillRectangle(PRectangle rc, Fill fill)
{
    FillRectImpl(rc, fill.colour);
}

void ScintillaDuiSurface::FillRectangleAligned(PRectangle rc, Fill fill)
{
    FillRectImpl(rc, fill.colour);
}

void ScintillaDuiSurface::FillRectangle(PRectangle rc, Surface &surfacePattern)
{
    ScintillaDuiSurface *pSource = dynamic_cast<ScintillaDuiSurface *>(&surfacePattern);
    EnsureRender();
    if ((m_pRender == nullptr) || (pSource == nullptr) || (pSource->m_pRender == nullptr)) {
        return;
    }
    if (rc.Empty()) {
        return;
    }
    const int32_t nWidth = std::max<int32_t>(1, static_cast<int32_t>(rc.Width()));
    const int32_t nHeight = std::max<int32_t>(1, static_cast<int32_t>(rc.Height()));
    //平铺图案：直接按源位图重复绘制
    const int32_t nTileWidth = std::max<int32_t>(1, pSource->m_pRender->GetWidth());
    const int32_t nTileHeight = std::max<int32_t>(1, pSource->m_pRender->GetHeight());
    for (int32_t y = 0; y < nHeight; y += nTileHeight) {
        for (int32_t x = 0; x < nWidth; x += nTileWidth) {
            const int32_t nCopyWidth = std::min(nTileWidth, nWidth - x);
            const int32_t nCopyHeight = std::min(nTileHeight, nHeight - y);
            m_pRender->AlphaBlend(
                static_cast<int32_t>(rc.left) + x + m_ptOrigin.x,
                static_cast<int32_t>(rc.top) + y + m_ptOrigin.y,
                nCopyWidth,
                nCopyHeight,
                pSource->m_pRender,
                0,
                0,
                nCopyWidth,
                nCopyHeight,
                255);
        }
    }
}

void ScintillaDuiSurface::RoundedRectangle(PRectangle rc, FillStroke fillStroke)
{
    EnsureRender();
    if (m_pRender == nullptr) {
        return;
    }
    const float radius = std::max(1.0f, static_cast<float>(rc.Height()) / 2.0f);
    if (fillStroke.fill.colour.GetAlpha() != 0) {
        m_pRender->FillRoundRect(
            ToUiRectF(rc, m_ptOrigin), radius, radius, ToUiColor(fillStroke.fill.colour), 255);
    }
    if (fillStroke.stroke.colour.GetAlpha() != 0) {
        ui::IRenderFactory *pFactory = ui::GlobalManager::Instance().GetRenderFactory();
        if (pFactory != nullptr) {
            std::unique_ptr<ui::IPen> pen(pFactory->CreatePen(
                ToUiColor(fillStroke.stroke.colour), static_cast<float>(fillStroke.stroke.width)));
            if (pen != nullptr) {
                m_pRender->DrawRoundRect(ToUiRectF(rc, m_ptOrigin), radius, radius, pen.get());
            }
        }
    }
}

void ScintillaDuiSurface::AlphaRectangle(
    PRectangle rc, XYPOSITION /*cornerSize*/, FillStroke fillStroke)
{
    //半透明矩形：颜色的 Alpha 通道参与合成（Scintilla 用于选区、括号匹配等）
    FillRectImpl(rc, fillStroke.fill.colour);
    if (fillStroke.stroke.colour.GetAlpha() != 0) {
        StrokeRect(rc, fillStroke.stroke.colour, fillStroke.stroke.width);
    }
}

void ScintillaDuiSurface::GradientRectangle(
    PRectangle rc, const std::vector<ColourStop> &stops, GradientOptions options)
{
    if (stops.empty()) {
        return;
    }
    if (stops.size() == 1) {
        FillRectImpl(rc, stops[0].colour);
        return;
    }
    EnsureRender();
    if (m_pRender == nullptr) {
        return;
    }
    const int8_t direction = (options == GradientOptions::leftToRight) ? 1 : 2;
    m_pRender->FillRect(
        ToUiRectF(rc, m_ptOrigin),
        ToUiColor(stops.front().colour),
        ToUiColor(stops.back().colour),
        direction,
        255);
}

void ScintillaDuiSurface::DrawRGBAImage(
    PRectangle rc, int width, int height, const unsigned char *pixelsImage)
{
    if ((width <= 0) || (height <= 0) || (pixelsImage == nullptr)) {
        return;
    }
    EnsureRender();
    ui::IRenderFactory *pFactory = ui::GlobalManager::Instance().GetRenderFactory();
    if ((m_pRender == nullptr) || (pFactory == nullptr)) {
        return;
    }
    std::unique_ptr<ui::IBitmap> bitmap(pFactory->CreateBitmap());
    if ((bitmap == nullptr)
        || !bitmap->Init(static_cast<uint32_t>(width), static_cast<uint32_t>(height), pixelsImage)) {
        return;
    }
    const ui::UiRect rcDest = ToUiRect(rc, m_ptOrigin);
    const ui::UiRect rcSource(0, 0, width, height);
    m_pRender->DrawImageRect(rcDest, bitmap.get(), rcDest, rcSource, 255, nullptr);
}

void ScintillaDuiSurface::Ellipse(PRectangle rc, FillStroke fillStroke)
{
    EnsureRender();
    ui::IRenderFactory *pFactory = ui::GlobalManager::Instance().GetRenderFactory();
    if ((m_pRender == nullptr) || (pFactory == nullptr)) {
        return;
    }
    const float cx = static_cast<float>(rc.Centre().x) + static_cast<float>(m_ptOrigin.x);
    const float cy = static_cast<float>(rc.Centre().y) + static_cast<float>(m_ptOrigin.y);
    const float rx = static_cast<float>(rc.Width() / 2.0);
    const float ry = static_cast<float>(rc.Height() / 2.0);
    if ((rx <= 0.0f) || (ry <= 0.0f)) {
        return;
    }
    const float radius = std::max(rx, ry);
    if (fillStroke.fill.colour.GetAlpha() != 0) {
        m_pRender->FillCircle(ui::UiPointF(cx, cy), radius, ToUiColor(fillStroke.fill.colour), 255);
    }
    if (fillStroke.stroke.colour.GetAlpha() != 0) {
        std::unique_ptr<ui::IPen> pen(pFactory->CreatePen(
            ToUiColor(fillStroke.stroke.colour), static_cast<float>(fillStroke.stroke.width)));
        if (pen != nullptr) {
            m_pRender->DrawCircle(ui::UiPointF(cx, cy), radius, pen.get());
        }
    }
}

void ScintillaDuiSurface::Stadium(PRectangle rc, FillStroke fillStroke, Ends /*ends*/)
{
    RoundedRectangle(rc, fillStroke);
}

void ScintillaDuiSurface::Copy(PRectangle rc, Point from, Surface &surfaceSource)
{
    ScintillaDuiSurface *pSource = dynamic_cast<ScintillaDuiSurface *>(&surfaceSource);
    EnsureRender();
    if ((m_pRender == nullptr) || (pSource == nullptr) || (pSource->m_pRender == nullptr)) {
        return;
    }
    const int32_t nWidth = static_cast<int32_t>(rc.Width());
    const int32_t nHeight = static_cast<int32_t>(rc.Height());
    if ((nWidth <= 0) || (nHeight <= 0)) {
        return;
    }
    m_pRender->AlphaBlend(
        static_cast<int32_t>(rc.left) + m_ptOrigin.x,
        static_cast<int32_t>(rc.top) + m_ptOrigin.y,
        nWidth,
        nHeight,
        pSource->m_pRender,
        static_cast<int32_t>(from.x),
        static_cast<int32_t>(from.y),
        nWidth,
        nHeight,
        255);
}

std::unique_ptr<IScreenLineLayout> ScintillaDuiSurface::Layout(const IScreenLine * /*screenLine*/)
{
    //DUI 移植层不支持双向排版（与 Qt 平台层的行为一致）
    return nullptr;
}

void ScintillaDuiSurface::DrawTextNoClip(
    PRectangle rc,
    const Font *font,
    XYPOSITION ybase,
    std::string_view text,
    ColourRGBA fore,
    ColourRGBA back)
{
    DrawTextImpl(rc, font, ybase, text, fore, back, true);
}

void ScintillaDuiSurface::DrawTextClipped(
    PRectangle rc,
    const Font *font,
    XYPOSITION ybase,
    std::string_view text,
    ColourRGBA fore,
    ColourRGBA back)
{
    SetClip(rc);
    DrawTextImpl(rc, font, ybase, text, fore, back, true);
    PopClip();
}

void ScintillaDuiSurface::DrawTextTransparent(
    PRectangle rc, const Font *font, XYPOSITION ybase, std::string_view text, ColourRGBA fore)
{
    DrawTextImpl(rc, font, ybase, text, fore, ColourRGBA(0, 0, 0, 0), false);
}

void ScintillaDuiSurface::MeasureWidths(
    const Font *font, std::string_view text, XYPOSITION *positions)
{
    MeasureWithShaper(font, text, positions);
}

XYPOSITION ScintillaDuiSurface::WidthText(const Font *font, std::string_view text)
{
    return MeasureWithShaper(font, text, nullptr);
}

void ScintillaDuiSurface::DrawTextNoClipUTF8(
    PRectangle rc,
    const Font *font,
    XYPOSITION ybase,
    std::string_view text,
    ColourRGBA fore,
    ColourRGBA back)
{
    DrawTextNoClip(rc, font, ybase, text, fore, back);
}

void ScintillaDuiSurface::DrawTextClippedUTF8(
    PRectangle rc,
    const Font *font,
    XYPOSITION ybase,
    std::string_view text,
    ColourRGBA fore,
    ColourRGBA back)
{
    DrawTextClipped(rc, font, ybase, text, fore, back);
}

void ScintillaDuiSurface::DrawTextTransparentUTF8(
    PRectangle rc, const Font *font, XYPOSITION ybase, std::string_view text, ColourRGBA fore)
{
    DrawTextTransparent(rc, font, ybase, text, fore);
}

void ScintillaDuiSurface::MeasureWidthsUTF8(
    const Font *font, std::string_view text, XYPOSITION *positions)
{
    MeasureWithShaper(font, text, positions);
}

XYPOSITION ScintillaDuiSurface::WidthTextUTF8(const Font *font, std::string_view text)
{
    return MeasureWithShaper(font, text, nullptr);
}

bool ScintillaDuiSurface::GetFontMetrics(const Font *font, float &fAscent, float &fDescent)
{
    fAscent = 0.0f;
    fDescent = 0.0f;
    const ScintillaDuiFont *pFont = ToDuiFont(font);
    if (pFont == nullptr) {
        return false;
    }
    ui::ITextShaper *pShaper = GetTextShaper();
    if (pShaper == nullptr) {
        //没有文本塑形接口时，退化为按字号估算
        fAscent = static_cast<float>(pFont->GetFontInfo().m_fontSize) * 0.8f;
        fDescent = static_cast<float>(pFont->GetFontInfo().m_fontSize) * 0.2f;
        return true;
    }
    ui::TextFontMetrics metrics;
    if (!pShaper->GetFontMetrics(pFont->GetFont(), metrics)) {
        fAscent = static_cast<float>(pFont->GetFontInfo().m_fontSize) * 0.8f;
        fDescent = static_cast<float>(pFont->GetFontInfo().m_fontSize) * 0.2f;
        return true;
    }
    fAscent = metrics.m_fAscent;
    fDescent = metrics.m_fDescent;
    return true;
}

XYPOSITION ScintillaDuiSurface::Ascent(const Font *font)
{
    float fAscent = 0.0f;
    float fDescent = 0.0f;
    GetFontMetrics(font, fAscent, fDescent);
    return static_cast<XYPOSITION>(fAscent);
}

XYPOSITION ScintillaDuiSurface::Descent(const Font *font)
{
    float fAscent = 0.0f;
    float fDescent = 0.0f;
    GetFontMetrics(font, fAscent, fDescent);
    return static_cast<XYPOSITION>(fDescent);
}

XYPOSITION ScintillaDuiSurface::InternalLeading(const Font * /*font*/)
{
    return 0;
}

XYPOSITION ScintillaDuiSurface::Height(const Font *font)
{
    float fAscent = 0.0f;
    float fDescent = 0.0f;
    GetFontMetrics(font, fAscent, fDescent);
    return static_cast<XYPOSITION>(fAscent + fDescent);
}

XYPOSITION ScintillaDuiSurface::AverageCharWidth(const Font *font)
{
    const XYPOSITION width = MeasureWithShaper(font, "x", nullptr);
    return std::max<XYPOSITION>(1, width);
}

void ScintillaDuiSurface::SetClip(PRectangle rc)
{
    EnsureRender();
    if (m_pRender == nullptr) {
        return;
    }
    const int32_t state = m_pRender->SetClip(ToUiRect(rc, m_ptOrigin), true);
    m_clipStates.push_back(state);
}

void ScintillaDuiSurface::PopClip()
{
    if ((m_pRender == nullptr) || m_clipStates.empty()) {
        return;
    }
    const int32_t state = m_clipStates.back();
    m_clipStates.pop_back();
    m_pRender->ClearClip(state);
}

void ScintillaDuiSurface::FlushCachedState()
{
    //DUI 的绘制调用即时生效，无需刷新缓存状态
}

void ScintillaDuiSurface::FlushDrawing()
{
    //DUI 的绘制调用即时生效，无需刷新绘制指令
}

ui::ITextShaper *ScintillaDuiSurface::GetTextShaper()
{
    EnsureRender();
    if (m_pRender == nullptr) {
        return nullptr;
    }
    if ((m_pTextShaper != nullptr) && (m_pTextShaperRender == m_pRender)) {
        return m_pTextShaper;
    }
    m_pTextShaperRender = m_pRender;
    m_pTextShaper = ScintillaDuiTextShaper::Get(m_pRender, m_ownedTextShaper);
    return m_pTextShaper;
}

const Scintilla::Internal::Font *ScintillaDuiSurface::GetCjkFallbackFont(
    const Scintilla::Internal::Font *font)
{
    if (font == nullptr) {
        return nullptr;
    }
    for (const auto &item : m_cjkFallbackFonts) {
        if (item.first == font) {
            return item.second.get();
        }
    }

    const ScintillaDuiFont *pFont = ToDuiFont(font);
    ui::ITextShaper *pShaper = GetTextShaper();
    if ((pFont == nullptr) || (pShaper == nullptr)) {
        return nullptr;
    }
    //CJK 字体候选：与 Duilib 的默认字体保持一致（核心的字体回退列表只覆盖 Emoji 与扩展汉字）
#if defined(_WIN32)
    const char *const fontFamilyNames[] = {"Microsoft YaHei", "SimSun", "Segoe UI"};
#elif defined(__APPLE__)
    const char *const fontFamilyNames[] = {"PingFang SC", "Songti SC", "Hiragino Sans GB"};
#else
    const char *const fontFamilyNames[]
        = {"Noto Sans CJK SC", "Source Han Sans CN", "WenQuanYi Micro Hei"};
#endif
    //用于判断回退字体是否包含中文字形
    constexpr uint32_t probeChar = 0x6EDA; //“滚”

    const ui::UiFont &fontInfo = pFont->GetFontInfo();
    std::shared_ptr<Scintilla::Internal::Font> pFallbackFont;
    for (const char *pFamilyName : fontFamilyNames) {
        ui::UiFont fallbackInfo = fontInfo;
        fallbackInfo.m_fontName = ui::StringConvert::UTF8ToT(pFamilyName);
        std::shared_ptr<Scintilla::Internal::Font> pFontCandidate
            = Scintilla::Internal::Font::Allocate(
                Scintilla::Internal::FontParameters(
                    pFamilyName,
                    static_cast<Scintilla::Internal::XYPOSITION>(fontInfo.m_fontSize),
                    fontInfo.m_bBold ? Scintilla::FontWeight::Bold : Scintilla::FontWeight::Normal,
                    fontInfo.m_bItalic));
        const ScintillaDuiFont *pCandidate = ToDuiFont(pFontCandidate.get());
        if (pCandidate == nullptr) {
            continue;
        }
        ui::TextGlyphInfo glyph;
        //探测：该字体是否包含中文字形（不使用默认字符替换）
        if (pShaper->ResolveGlyph(pCandidate->GetFont(), probeChar, glyph, false)
            && (glyph.m_glyphId != 0)) {
            pFallbackFont = pFontCandidate;
            break;
        }
    }
    m_cjkFallbackFonts.emplace_back(font, pFallbackFont);
    return pFallbackFont.get();
}

bool ScintillaDuiSurface::ResolveGlyphWithFallback(
    const Scintilla::Internal::Font *font, uint32_t unicodeChar, ui::TextGlyphInfo &glyph)
{
    const ScintillaDuiFont *pFont = ToDuiFont(font);
    ui::ITextShaper *pShaper = GetTextShaper();
    if ((pFont == nullptr) || (pShaper == nullptr)) {
        return false;
    }
    //1) 优先使用当前样式字体（不替换为默认字符）
    if (pShaper->ResolveGlyph(pFont->GetFont(), unicodeChar, glyph, false)
        && (glyph.m_glyphId != 0)) {
        return true;
    }
    //2) 使用系统 CJK 回退字体
    const ScintillaDuiFont *pFallback = ToDuiFont(GetCjkFallbackFont(font));
    if ((pFallback != nullptr)
        && pShaper->ResolveGlyph(pFallback->GetFont(), unicodeChar, glyph, false)
        && (glyph.m_glyphId != 0)) {
        return true;
    }
    //3) 最后使用默认字符占位，保证字形宽度有效
    return pShaper->ResolveGlyph(pFont->GetFont(), unicodeChar, glyph, true);
}

void ScintillaDuiSurface::EnsureRender()
{
    //度量用的 Surface 没有关联绘制目标，使用离屏 Render 完成文本度量
    if ((m_pRender == nullptr) && (m_pOwner != nullptr)) {
        m_ownedRender = m_pOwner->GetMeasurementRender();
        m_pRender = m_ownedRender.get();
    }
}

void ScintillaDuiSurface::FillRectImpl(PRectangle rc, ColourRGBA colour)
{
    if ((colour.GetAlpha() == 0) || rc.Empty()) {
        return;
    }
    EnsureRender();
    if (m_pRender == nullptr) {
        return;
    }
    //颜色的 Alpha 通道已经包含透明度，这里 uFade 传 255（避免双后端语义差异）
    m_pRender->FillRect(ToUiRectF(rc, m_ptOrigin), ToUiColor(colour), 255);
}

void ScintillaDuiSurface::StrokeRect(PRectangle rc, ColourRGBA colour, XYPOSITION width)
{
    if ((colour.GetAlpha() == 0) || rc.Empty()) {
        return;
    }
    EnsureRender();
    if (m_pRender == nullptr) {
        return;
    }
    m_pRender->DrawRect(
        ToUiRectF(rc, m_ptOrigin),
        ToUiColor(colour),
        static_cast<float>(std::max<XYPOSITION>(1.0, width)),
        true);
}

XYPOSITION ScintillaDuiSurface::MeasureWithShaper(
    const Font *font, std::string_view text, XYPOSITION *positions)
{
    if (text.empty()) {
        return 0;
    }
    const ScintillaDuiFont *pFont = ToDuiFont(font);
    ui::ITextShaper *pShaper = GetTextShaper();
    if ((pFont == nullptr) || (pShaper == nullptr)) {
        return MeasureWithRender(font, text, positions);
    }

    XYPOSITION x = 0;
    size_t offset = 0;
    const size_t nLength = text.size();
    while (offset < nLength) {
        size_t consumed = 1;
        const uint32_t unicodeChar = DecodeUtf8(text.data() + offset, nLength - offset, consumed);
        ui::TextGlyphInfo glyph;
        if (ResolveGlyphWithFallback(font, unicodeChar, glyph)) {
            x += glyph.m_fAdvance;
        } else {
            //无法解析的字形：按空格宽度占位，保持与 Qt 平台层类似的对齐行为
            ui::TextGlyphInfo spaceGlyph;
            if (ResolveGlyphWithFallback(font, static_cast<uint32_t>(' '), spaceGlyph)) {
                x += spaceGlyph.m_fAdvance;
            }
        }
        if (positions != nullptr) {
            //与 Qt 平台层一致：多字节字符的每个字节都记录该字符结束位置
            for (size_t i = 0; i < consumed; ++i) {
                positions[offset + i] = x;
            }
        }
        offset += consumed;
    }
    return x;
}

XYPOSITION ScintillaDuiSurface::MeasureWithRender(
    const Font *font, std::string_view text, XYPOSITION *positions)
{
    if (text.empty()) {
        return 0;
    }
    EnsureRender();
    const ScintillaDuiFont *pFont = ToDuiFont(font);
    if ((m_pRender == nullptr) || (pFont == nullptr) || (pFont->GetFont() == nullptr)) {
        const XYPOSITION fallback = static_cast<XYPOSITION>(text.size()) * 8.0;
        if (positions != nullptr) {
            for (size_t i = 0; i < text.size(); ++i) {
                positions[i] = static_cast<XYPOSITION>(i + 1) * 8.0;
            }
        }
        return fallback;
    }

    ui::MeasureStringParam measureParam;
    measureParam.pFont = pFont->GetFont();
    measureParam.uFormat = ui::DrawStringFormat::TEXT_SINGLELINE;

    XYPOSITION x = 0;
    size_t offset = 0;
    const size_t nLength = text.size();
    while (offset < nLength) {
        size_t consumed = 1;
        (void) DecodeUtf8(text.data() + offset, nLength - offset, consumed);
        std::string oneChar(text.substr(offset, consumed));
        const ui::UiRect rc
            = m_pRender->MeasureString(ui::StringConvert::UTF8ToT(oneChar), measureParam);
        x += std::max<XYPOSITION>(0, rc.Width());
        if (positions != nullptr) {
            for (size_t i = 0; i < consumed; ++i) {
                positions[offset + i] = x;
            }
        }
        offset += consumed;
    }
    return x;
}

void ScintillaDuiSurface::DrawTextImpl(
    PRectangle rc,
    const Font *font,
    XYPOSITION ybase,
    std::string_view text,
    ColourRGBA fore,
    ColourRGBA back,
    bool bDrawBackground)
{
    if (text.empty()) {
        return;
    }
    EnsureRender();
    const ScintillaDuiFont *pFont = ToDuiFont(font);
    if ((m_pRender == nullptr) || (pFont == nullptr) || (pFont->GetFont() == nullptr)) {
        return;
    }
    if (bDrawBackground && (back.GetAlpha() != 0)) {
        FillRectImpl(rc, back);
    }
    if (fore.GetAlpha() == 0) {
        return;
    }

    ui::ITextShaper *pShaper = GetTextShaper();
    if (pShaper == nullptr) {
        //没有文本塑形接口时，退化为整串绘制（度量与绘制可能略有偏差）
        ui::DrawStringParam drawParam;
        drawParam.pFont = pFont->GetFont();
        drawParam.uFormat = ui::DrawStringFormat::TEXT_LEFT | ui::DrawStringFormat::TEXT_TOP
                            | ui::DrawStringFormat::TEXT_SINGLELINE
                            | ui::DrawStringFormat::TEXT_NOCLIP;
        drawParam.dwTextColor = ToUiColor(fore);
        drawParam.uFade = fore.GetAlpha();
        const int32_t nAscent = static_cast<int32_t>(Ascent(font) + 0.5);
        drawParam.textRect = ui::UiRect(
            static_cast<int32_t>(rc.left) + m_ptOrigin.x,
            static_cast<int32_t>(ybase) - nAscent + m_ptOrigin.y,
            static_cast<int32_t>(rc.right) + m_ptOrigin.x,
            static_cast<int32_t>(ybase) - nAscent + static_cast<int32_t>(Height(font) + 2)
                + m_ptOrigin.y);
        m_pRender->DrawString(ui::StringConvert::UTF8ToT(std::string(text)), drawParam);
        return;
    }

    //按字形逐个绘制，推进量与 MeasureWidthsUTF8 完全一致
    const float fOriginX = static_cast<float>(m_ptOrigin.x);
    const float fOriginY = static_cast<float>(m_ptOrigin.y);
    XYPOSITION x = rc.left;
    size_t offset = 0;
    const size_t nLength = text.size();
    while (offset < nLength) {
        size_t consumed = 1;
        const uint32_t unicodeChar = DecodeUtf8(text.data() + offset, nLength - offset, consumed);
        ui::TextGlyphInfo glyph;
        if (ResolveGlyphWithFallback(font, unicodeChar, glyph) && (glyph.m_pFont != nullptr)
            && (glyph.m_glyphId != 0)) {
            pShaper->DrawGlyph(
                glyph,
                static_cast<float>(x) + fOriginX,
                static_cast<float>(ybase) + fOriginY,
                ToUiColor(fore),
                255);
            x += glyph.m_fAdvance;
        }
        offset += consumed;
    }
}

} // namespace duilib::ext::scintilla
