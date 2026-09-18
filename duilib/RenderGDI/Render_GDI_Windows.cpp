#include "duilib/RenderGDI/Render_GDI_Windows.h"

#ifdef DUILIB_BUILD_FOR_WIN

#include "duilib/Render/BitmapAlpha.h"
#include "duilib/RenderGDI/GdiTypes.h"
#include "duilib/Utils/PerformanceUtil.h"

#include <gdiplus.h>

#include <algorithm>
#include <cmath>
#include <cstring>
#include <memory>

#pragma comment(lib, "gdiplus.lib")
#pragma comment(lib, "msimg32.lib")

namespace ui
{

namespace
{
Gdiplus::Color ToGdiplusColor(UiColor color, uint8_t alpha = 255)
{
    const uint8_t a = (uint8_t)((uint32_t)color.GetAlpha() * alpha / 255);
    return Gdiplus::Color(a, color.GetRed(), color.GetGreen(), color.GetBlue());
}

std::unique_ptr<Gdiplus::Graphics> CreateGdiplusGraphics(HDC hdc, UiPoint ptOrg)
{
    std::unique_ptr<Gdiplus::Graphics> graphics = std::make_unique<Gdiplus::Graphics>(hdc);
    if (graphics != nullptr) {
        graphics->SetSmoothingMode(Gdiplus::SmoothingModeAntiAlias);
        graphics->SetPixelOffsetMode(Gdiplus::PixelOffsetModeHalf);
        graphics->SetTextRenderingHint(Gdiplus::TextRenderingHintAntiAlias);
        graphics->TranslateTransform((float)ptOrg.x, (float)ptOrg.y);
    }
    return graphics;
}

void SetFadeColorMatrix(Gdiplus::ImageAttributes& imageAttributes, uint8_t uFade)
{
    if (uFade >= 255) {
        return;
    }
    const float fScale = (float)uFade / 255.0f;
    Gdiplus::ColorMatrix colorMatrix = { {
        { 1.0f, 0.0f, 0.0f, 0.0f, 0.0f },
        { 0.0f, 1.0f, 0.0f, 0.0f, 0.0f },
        { 0.0f, 0.0f, 1.0f, 0.0f, 0.0f },
        { 0.0f, 0.0f, 0.0f, fScale, 0.0f },
        { 0.0f, 0.0f, 0.0f, 0.0f, 1.0f }
    } };
    imageAttributes.SetColorMatrix(&colorMatrix, Gdiplus::ColorMatrixFlagsDefault, Gdiplus::ColorAdjustTypeBitmap);
}

UINT ToDrawTextFormat(uint32_t uFormat, bool bSingleLine)
{
    UINT nFormat = DT_NOPREFIX;
    if ((uFormat & DrawStringFormat::TEXT_HCENTER) != 0) {
        nFormat |= DT_CENTER;
    }
    else if ((uFormat & DrawStringFormat::TEXT_RIGHT) != 0) {
        nFormat |= DT_RIGHT;
    }
    else {
        nFormat |= DT_LEFT;
    }

    if ((uFormat & DrawStringFormat::TEXT_VCENTER) != 0) {
        nFormat |= DT_VCENTER;
    }
    else if ((uFormat & DrawStringFormat::TEXT_BOTTOM) != 0) {
        nFormat |= DT_BOTTOM;
    }

    if (bSingleLine || ((uFormat & DrawStringFormat::TEXT_SINGLELINE) != 0)) {
        nFormat |= DT_SINGLELINE;
    }
    else if (((uFormat & DrawStringFormat::TEXT_WORD_WRAP) != 0) ||
             ((uFormat & DrawStringFormat::TEXT_END_ELLIPSIS) != 0) ||
             ((uFormat & DrawStringFormat::TEXT_PATH_ELLIPSIS) != 0)) {
        nFormat |= DT_WORDBREAK;
    }

    if ((uFormat & DrawStringFormat::TEXT_END_ELLIPSIS) != 0) {
        nFormat |= DT_END_ELLIPSIS;
    }
    else if ((uFormat & DrawStringFormat::TEXT_PATH_ELLIPSIS) != 0) {
        nFormat |= DT_PATH_ELLIPSIS;
    }
    return nFormat;
}

Gdiplus::StringAlignment ToStringAlignment(UINT align)
{
    if (align == DT_CENTER) {
        return Gdiplus::StringAlignmentCenter;
    }
    if (align == DT_RIGHT) {
        return Gdiplus::StringAlignmentFar;
    }
    return Gdiplus::StringAlignmentNear;
}

std::unique_ptr<Gdiplus::StringFormat> CreateStringFormat(uint32_t uFormat)
{
    std::unique_ptr<Gdiplus::StringFormat> pFormat = std::make_unique<Gdiplus::StringFormat>();
    if (pFormat == nullptr) {
        return nullptr;
    }
    const UINT nFormat = ToDrawTextFormat(uFormat, false);
    pFormat->SetAlignment(ToStringAlignment(nFormat & (DT_CENTER | DT_RIGHT)));
    if ((nFormat & DT_VCENTER) != 0) {
        pFormat->SetLineAlignment(Gdiplus::StringAlignmentCenter);
    }
    else if ((nFormat & DT_BOTTOM) != 0) {
        pFormat->SetLineAlignment(Gdiplus::StringAlignmentFar);
    }
    if ((nFormat & DT_SINGLELINE) != 0) {
        pFormat->SetFormatFlags(pFormat->GetFormatFlags() | Gdiplus::StringFormatFlagsNoWrap);
    }
    if ((nFormat & DT_END_ELLIPSIS) != 0) {
        pFormat->SetTrimming(Gdiplus::StringTrimmingEllipsisCharacter);
    }
    else if ((nFormat & DT_PATH_ELLIPSIS) != 0) {
        pFormat->SetTrimming(Gdiplus::StringTrimmingEllipsisPath);
    }
    return pFormat;
}

Gdiplus::Rect ToGdiplusRect(const UiRectF& rc)
{
    return Gdiplus::Rect((INT)std::floor(rc.left), (INT)std::floor(rc.top),
                         (INT)std::ceil(rc.Width()), (INT)std::ceil(rc.Height()));
}

void AddRoundRectPath(Gdiplus::GraphicsPath& path, const UiRectF& rc, float rx, float ry)
{
    const float left = rc.left;
    const float top = rc.top;
    const float right = rc.right;
    const float bottom = rc.bottom;
    const float d = std::max(0.0f, std::min(rx, ry));
    if ((d <= 0.0f) || (right <= left) || (bottom <= top)) {
        path.AddRectangle(Gdiplus::RectF(left, top, right - left, bottom - top));
        return;
    }
    path.AddArc(left, top, d * 2.0f, d * 2.0f, 180.0f, 90.0f);
    path.AddArc(right - d * 2.0f, top, d * 2.0f, d * 2.0f, 270.0f, 90.0f);
    path.AddArc(right - d * 2.0f, bottom - d * 2.0f, d * 2.0f, d * 2.0f, 0.0f, 90.0f);
    path.AddArc(left, bottom - d * 2.0f, d * 2.0f, d * 2.0f, 90.0f, 90.0f);
    path.CloseFigure();
}

std::unique_ptr<Gdiplus::Pen> CreateGdiplusPen(const IPen* pPen)
{
    if (pPen == nullptr) {
        return nullptr;
    }
    std::unique_ptr<Gdiplus::Pen> pen = std::make_unique<Gdiplus::Pen>(ToGdiplusColor(pPen->GetColor()), pPen->GetWidth());
    if (pen == nullptr) {
        return nullptr;
    }
    switch (pPen->GetStartCap()) {
    case IPen::kRound_Cap: pen->SetStartCap(Gdiplus::LineCapRound); break;
    case IPen::kSquare_Cap: pen->SetStartCap(Gdiplus::LineCapSquare); break;
    default: pen->SetStartCap(Gdiplus::LineCapFlat); break;
    }
    switch (pPen->GetEndCap()) {
    case IPen::kRound_Cap: pen->SetEndCap(Gdiplus::LineCapRound); break;
    case IPen::kSquare_Cap: pen->SetEndCap(Gdiplus::LineCapSquare); break;
    default: pen->SetEndCap(Gdiplus::LineCapFlat); break;
    }
    switch (pPen->GetLineJoin()) {
    case IPen::kBevel_Join: pen->SetLineJoin(Gdiplus::LineJoinBevel); break;
    case IPen::kRound_Join: pen->SetLineJoin(Gdiplus::LineJoinRound); break;
    default: pen->SetLineJoin(Gdiplus::LineJoinMiter); break;
    }
    Gdiplus::DashStyle dashStyle = Gdiplus::DashStyleSolid;
    switch (pPen->GetDashStyle()) {
    case IPen::kDashStyleDash: dashStyle = Gdiplus::DashStyleDash; break;
    case IPen::kDashStyleDot: dashStyle = Gdiplus::DashStyleDot; break;
    case IPen::kDashStyleDashDot: dashStyle = Gdiplus::DashStyleDashDot; break;
    case IPen::kDashStyleDashDotDot: dashStyle = Gdiplus::DashStyleDashDotDot; break;
    default: break;
    }
    pen->SetDashStyle(dashStyle);
    return pen;
}
}

Render_GDI_Windows::Render_GDI_Windows(HWND hWnd):
    m_hWnd(hWnd)
{
    m_hMemDC = ::CreateCompatibleDC(nullptr);
    ASSERT(m_hMemDC != nullptr);
    if (::IsWindow(hWnd)) {
        RECT rc = {};
        ::GetClientRect(hWnd, &rc);
        const int32_t nWidth = rc.right - rc.left;
        const int32_t nHeight = rc.bottom - rc.top;
        if ((nWidth > 0) && (nHeight > 0)) {
            CreateDib(nWidth, nHeight);
        }
    }
}

Render_GDI_Windows::~Render_GDI_Windows()
{
    DeleteDib();
    if (m_hMemDC != nullptr) {
        ::DeleteDC(m_hMemDC);
        m_hMemDC = nullptr;
    }
}

bool Render_GDI_Windows::CreateDib(int32_t nWidth, int32_t nHeight)
{
    if ((nWidth <= 0) || (nHeight <= 0)) {
        return false;
    }
    DeleteDib();
    BITMAPINFO bmi = {};
    bmi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
    bmi.bmiHeader.biWidth = nWidth;
    bmi.bmiHeader.biHeight = -nHeight;
    bmi.bmiHeader.biPlanes = 1;
    bmi.bmiHeader.biBitCount = 32;
    bmi.bmiHeader.biCompression = BI_RGB;
    bmi.bmiHeader.biSizeImage = (DWORD)(nWidth * nHeight * (int32_t)sizeof(uint32_t));
    m_hBitmap = ::CreateDIBSection(m_hMemDC, &bmi, DIB_RGB_COLORS, &m_pPixelBits, nullptr, 0);
    ASSERT(m_hBitmap != nullptr);
    if (m_hBitmap == nullptr) {
        return false;
    }
    m_hOldBitmap = ::SelectObject(m_hMemDC, m_hBitmap);
    m_nWidth = nWidth;
    m_nHeight = nHeight;
    std::memset(m_pPixelBits, 0, (size_t)nWidth * nHeight * sizeof(uint32_t));
    return true;
}

void Render_GDI_Windows::DeleteDib()
{
    if (m_hMemDC != nullptr) {
        ::SetViewportOrgEx(m_hMemDC, 0, 0, nullptr);
    }
    if (m_hOldBitmap != nullptr) {
        ::SelectObject(m_hMemDC, m_hOldBitmap);
        m_hOldBitmap = nullptr;
    }
    if (m_hBitmap != nullptr) {
        ::DeleteObject(m_hBitmap);
        m_hBitmap = nullptr;
    }
    m_pPixelBits = nullptr;
    m_nWidth = 0;
    m_nHeight = 0;
}

RenderType Render_GDI_Windows::GetRenderType() const
{
    return RenderType::kRenderType_GDI;
}

RenderCapabilities Render_GDI_Windows::GetCapabilities() const
{
    RenderCapabilities capabilities;
    capabilities.Add(RenderCapabilities::kPath);
    capabilities.Add(RenderCapabilities::kGradientFill);
    capabilities.Add(RenderCapabilities::kAlphaBlend);
    capabilities.Add(RenderCapabilities::kLayeredWindow);
    capabilities.Add(RenderCapabilities::kImageTransform);
    capabilities.Add(RenderCapabilities::kBoxShadow);
    capabilities.Add(RenderCapabilities::kTextPathEllipsis);
    capabilities.Add(RenderCapabilities::kAntiAlias);
    return capabilities;
}

RenderBackendType Render_GDI_Windows::GetRenderBackendType() const
{
    return RenderBackendType::kRaster_BackendType;
}

int32_t Render_GDI_Windows::GetWidth() const
{
    return m_nWidth;
}

int32_t Render_GDI_Windows::GetHeight() const
{
    return m_nHeight;
}

bool Render_GDI_Windows::Resize(int32_t width, int32_t height)
{
    if ((width == m_nWidth) && (height == m_nHeight)) {
        return true;
    }
    return CreateDib(width, height);
}

UiPoint Render_GDI_Windows::OffsetWindowOrg(UiPoint ptOffset)
{
    const UiPoint ptOld = m_ptOrg;
    m_ptOrg.x += ptOffset.x;
    m_ptOrg.y += ptOffset.y;
    return ptOld;
}

UiPoint Render_GDI_Windows::SetWindowOrg(UiPoint pt)
{
    const UiPoint ptOld = m_ptOrg;
    m_ptOrg = pt;
    return ptOld;
}

UiPoint Render_GDI_Windows::GetWindowOrg() const
{
    return m_ptOrg;
}

void Render_GDI_Windows::SaveClip(int32_t& nState)
{
    nState = ::SaveDC(m_hMemDC);
}

void Render_GDI_Windows::RestoreClip(int32_t nState)
{
    if (nState >= 0) {
        ::RestoreDC(m_hMemDC, nState);
    }
}

int32_t Render_GDI_Windows::SetClip(const UiRect& rc, bool bIntersect)
{
    if (rc.IsEmpty()) {
        return -1;
    }
    const int32_t nState = ::SaveDC(m_hMemDC);
    RECT rect = { rc.left + m_ptOrg.x, rc.top + m_ptOrg.y, rc.right + m_ptOrg.x, rc.bottom + m_ptOrg.y };
    HRGN hRgn = ::CreateRectRgnIndirect(&rect);
    ::ExtSelectClipRgn(m_hMemDC, hRgn, bIntersect ? RGN_AND : RGN_DIFF);
    ::DeleteObject(hRgn);
    return nState;
}

int32_t Render_GDI_Windows::SetRoundClip(const UiRect& rcItem, float rx, float ry, bool bIntersect)
{
    if (rcItem.IsEmpty()) {
        return -1;
    }
    const int32_t nState = ::SaveDC(m_hMemDC);
    const int32_t nRx = std::max(1, (int32_t)std::round(rx * 2.0f));
    const int32_t nRy = std::max(1, (int32_t)std::round(ry * 2.0f));
    HRGN hRgn = ::CreateRoundRectRgn(rcItem.left + m_ptOrg.x, rcItem.top + m_ptOrg.y,
                                     rcItem.right + m_ptOrg.x, rcItem.bottom + m_ptOrg.y,
                                     nRx, nRy);
    ::ExtSelectClipRgn(m_hMemDC, hRgn, bIntersect ? RGN_AND : RGN_DIFF);
    ::DeleteObject(hRgn);
    return nState;
}

void Render_GDI_Windows::ClearClip(int32_t nState)
{
    if (nState >= 0) {
        ::RestoreDC(m_hMemDC, nState);
    }
}

bool Render_GDI_Windows::BitBlt(int32_t x, int32_t y, int32_t cx, int32_t cy,
                                IRender* pSrcRender, int32_t xSrc, int32_t ySrc,
                                RopMode rop)
{
    Render_GDI_Windows* pSrc = dynamic_cast<Render_GDI_Windows*>(pSrcRender);
    if ((pSrc == nullptr) || (pSrc->m_hMemDC == nullptr)) {
        return false;
    }
    DWORD dwRop = SRCCOPY;
    switch (rop) {
    case RopMode::kDstInvert: dwRop = DSTINVERT; break;
    case RopMode::kSrcInvert: dwRop = SRCINVERT; break;
    case RopMode::kSrcAnd: dwRop = SRCAND; break;
    default: break;
    }
    return ::BitBlt(m_hMemDC, x + m_ptOrg.x, y + m_ptOrg.y, cx, cy,
                    pSrc->m_hMemDC, xSrc + pSrc->m_ptOrg.x, ySrc + pSrc->m_ptOrg.y, dwRop) != FALSE;
}

bool Render_GDI_Windows::StretchBlt(int32_t xDest, int32_t yDest, int32_t widthDest, int32_t heightDest,
                                    IRender* pSrcRender, int32_t xSrc, int32_t ySrc,
                                    int32_t widthSrc, int32_t heightSrc,
                                    RopMode rop)
{
    Render_GDI_Windows* pSrc = dynamic_cast<Render_GDI_Windows*>(pSrcRender);
    if ((pSrc == nullptr) || (pSrc->m_hMemDC == nullptr)) {
        return false;
    }
    DWORD dwRop = SRCCOPY;
    switch (rop) {
    case RopMode::kDstInvert: dwRop = DSTINVERT; break;
    case RopMode::kSrcInvert: dwRop = SRCINVERT; break;
    case RopMode::kSrcAnd: dwRop = SRCAND; break;
    default: break;
    }
    if (dwRop == SRCCOPY) {
        ::SetStretchBltMode(m_hMemDC, HALFTONE);
    }
    return ::StretchBlt(m_hMemDC, xDest + m_ptOrg.x, yDest + m_ptOrg.y, widthDest, heightDest,
                        pSrc->m_hMemDC, xSrc + pSrc->m_ptOrg.x, ySrc + pSrc->m_ptOrg.y,
                        widthSrc, heightSrc, dwRop) != FALSE;
}

bool Render_GDI_Windows::AlphaBlend(int32_t xDest, int32_t yDest, int32_t widthDest, int32_t heightDest,
                                    IRender* pSrcRender, int32_t xSrc, int32_t ySrc,
                                    int32_t widthSrc, int32_t heightSrc,
                                    uint8_t alpha)
{
    Render_GDI_Windows* pSrc = dynamic_cast<Render_GDI_Windows*>(pSrcRender);
    if ((pSrc == nullptr) || (pSrc->m_hMemDC == nullptr)) {
        return false;
    }
    BLENDFUNCTION bf = { AC_SRC_OVER, 0, alpha, AC_SRC_ALPHA };
    return ::AlphaBlend(m_hMemDC, xDest + m_ptOrg.x, yDest + m_ptOrg.y, widthDest, heightDest,
                        pSrc->m_hMemDC, xSrc + pSrc->m_ptOrg.x, ySrc + pSrc->m_ptOrg.y,
                        widthSrc, heightSrc, bf) != FALSE;
}

void Render_GDI_Windows::DrawBitmapRect(IBitmap* pBitmap, const UiRect& rcDest, const UiRect& rcSource, uint8_t uFade)
{
    Bitmap_GDI* pGdiBitmap = dynamic_cast<Bitmap_GDI*>(pBitmap);
    if (pGdiBitmap == nullptr) {
        return;
    }
    HDC hSrcDC = ::CreateCompatibleDC(m_hMemDC);
    if (hSrcDC == nullptr) {
        return;
    }
    HGDIOBJ hOldBitmap = ::SelectObject(hSrcDC, pGdiBitmap->GetBitmap());
    BLENDFUNCTION bf = { AC_SRC_OVER, 0, uFade, AC_SRC_ALPHA };
    ::AlphaBlend(m_hMemDC, rcDest.left + m_ptOrg.x, rcDest.top + m_ptOrg.y,
                 rcDest.Width(), rcDest.Height(),
                 hSrcDC, rcSource.left, rcSource.top, rcSource.Width(), rcSource.Height(), bf);
    ::SelectObject(hSrcDC, hOldBitmap);
    ::DeleteDC(hSrcDC);
}

void Render_GDI_Windows::DrawImage(const UiRect& /*rcPaint*/, IBitmap* pBitmap,
                                   const UiRect& rcDest, const UiRect& rcDestCorners,
                                   const UiRect& rcSource, const UiRect& rcSourceCorners,
                                   uint8_t uFade,
                                   const TiledDrawParam* pTiledDrawParam,
                                   bool /*bWindowShadowMode*/)
{
    if ((pBitmap == nullptr) || rcDest.IsEmpty() || rcSource.IsEmpty()) {
        return;
    }
    if (rcDestCorners.IsEmpty() || rcSourceCorners.IsEmpty()) {
        DrawBitmapRect(pBitmap, rcDest, rcSource, uFade);
        return;
    }
    const int32_t xDest[4] = { rcDest.left, rcDest.left + rcDestCorners.left, rcDest.right - rcDestCorners.right, rcDest.right };
    const int32_t xSrc[4] = { rcSource.left, rcSource.left + rcSourceCorners.left, rcSource.right - rcSourceCorners.right, rcSource.right };
    const int32_t yDest[4] = { rcDest.top, rcDest.top + rcDestCorners.top, rcDest.bottom - rcDestCorners.bottom, rcDest.bottom };
    const int32_t ySrc[4] = { rcSource.top, rcSource.top + rcSourceCorners.top, rcSource.bottom - rcSourceCorners.bottom, rcSource.bottom };

    for (int y = 0; y < 3; ++y) {
        for (int x = 0; x < 3; ++x) {
            if ((xSrc[x + 1] <= xSrc[x]) || (ySrc[y + 1] <= ySrc[y])) {
                continue;
            }
            UiRect rcSrc(xSrc[x], ySrc[y], xSrc[x + 1], ySrc[y + 1]);
            UiRect rcDst(xDest[x], yDest[y], xDest[x + 1], yDest[y + 1]);
            if ((x == 1) && (y == 1)) {
                if (pTiledDrawParam != nullptr) {
                    for (int32_t nY = rcDst.top; nY < rcDst.bottom; nY += rcSrc.Height()) {
                        for (int32_t nX = rcDst.left; nX < rcDst.right; nX += rcSrc.Width()) {
                            UiRect rcTile(nX, nY, std::min(nX + rcSrc.Width(), rcDst.right), std::min(nY + rcSrc.Height(), rcDst.bottom));
                            DrawBitmapRect(pBitmap, rcTile, rcSrc, uFade);
                        }
                    }
                }
                else {
                    DrawBitmapRect(pBitmap, rcDst, rcSrc, uFade);
                }
            }
            else {
                DrawBitmapRect(pBitmap, rcDst, rcSrc, uFade);
            }
        }
    }
}

void Render_GDI_Windows::DrawImage(const UiRect& rcPaint, IBitmap* pBitmap,
                                   const UiRect& rcDest, const UiRect& rcSource,
                                   uint8_t uFade,
                                   const TiledDrawParam* pTiledDrawParam,
                                   bool bWindowShadowMode)
{
    DrawImage(rcPaint, pBitmap, rcDest, UiRect(), rcSource, UiRect(), uFade, pTiledDrawParam, bWindowShadowMode);
}

void Render_GDI_Windows::DrawImageRect(const UiRect& /*rcPaint*/, IBitmap* pBitmap,
                                       const UiRect& rcDest, const UiRect& rcSource,
                                       uint8_t uFade, IMatrix* pMatrix)
{
    if (pMatrix == nullptr) {
        DrawBitmapRect(pBitmap, rcDest, rcSource, uFade);
        return;
    }
    Bitmap_GDI* pGdiBitmap = dynamic_cast<Bitmap_GDI*>(pBitmap);
    Matrix_GDI* pGdiMatrix = dynamic_cast<Matrix_GDI*>(pMatrix);
    if ((pGdiBitmap == nullptr) || (pGdiMatrix == nullptr)) {
        return;
    }
    Gdiplus::Bitmap bitmap(pGdiBitmap->GetBitmap(), nullptr);
    std::unique_ptr<Gdiplus::Graphics> graphics = CreateGdiplusGraphics(m_hMemDC, m_ptOrg);
    if (graphics == nullptr) {
        return;
    }
    const std::array<float, 6>& values = pGdiMatrix->GetMatrix();
    Gdiplus::Matrix matrix(values[0], values[1], values[2], values[3], values[4], values[5]);
    graphics->SetTransform(&matrix);
    Gdiplus::ImageAttributes imageAttributes;
    SetFadeColorMatrix(imageAttributes, uFade);
    graphics->DrawImage(&bitmap, Gdiplus::Rect(rcDest.left, rcDest.top, rcDest.Width(), rcDest.Height()),
                        rcSource.left, rcSource.top, rcSource.Width(), rcSource.Height(),
                        Gdiplus::UnitPixel, &imageAttributes);
}

void Render_GDI_Windows::DrawLine(const UiPointF& pt1, const UiPointF& pt2, UiColor penColor, float fWidth)
{
    Pen_GDI pen(penColor, fWidth);
    DrawLine(pt1, pt2, &pen);
}

void Render_GDI_Windows::DrawLine(const UiPointF& pt1, const UiPointF& pt2, IPen* pen)
{
    std::unique_ptr<Gdiplus::Graphics> graphics = CreateGdiplusGraphics(m_hMemDC, m_ptOrg);
    std::unique_ptr<Gdiplus::Pen> gdiplusPen = CreateGdiplusPen(pen);
    if ((graphics == nullptr) || (gdiplusPen == nullptr)) {
        return;
    }
    graphics->DrawLine(gdiplusPen.get(), pt1.x, pt1.y, pt2.x, pt2.y);
}

void Render_GDI_Windows::DrawRect(const UiRectF& rc, UiColor penColor, float fWidth, bool bLineInRect)
{
    Pen_GDI pen(penColor, fWidth);
    DrawRect(rc, &pen, bLineInRect);
}

void Render_GDI_Windows::DrawRect(const UiRectF& rc, IPen* pen, bool bLineInRect)
{
    std::unique_ptr<Gdiplus::Graphics> graphics = CreateGdiplusGraphics(m_hMemDC, m_ptOrg);
    std::unique_ptr<Gdiplus::Pen> gdiplusPen = CreateGdiplusPen(pen);
    if ((graphics == nullptr) || (gdiplusPen == nullptr)) {
        return;
    }
    Gdiplus::RectF rect = Gdiplus::RectF(rc.left, rc.top, rc.Width(), rc.Height());
    if (bLineInRect && (pen != nullptr)) {
        const float fHalf = pen->GetWidth() / 2.0f;
        rect.Inflate(-fHalf, -fHalf);
    }
    graphics->DrawRectangle(gdiplusPen.get(), rect);
}

void Render_GDI_Windows::FillRect(const UiRectF& rc, UiColor dwColor, uint8_t uFade)
{
    std::unique_ptr<Gdiplus::Graphics> graphics = CreateGdiplusGraphics(m_hMemDC, m_ptOrg);
    if (graphics == nullptr) {
        return;
    }
    const UiColor color = UiColor((uint8_t)((uint32_t)dwColor.GetAlpha() * uFade / 255),
                                  dwColor.GetRed(), dwColor.GetGreen(), dwColor.GetBlue());
    Gdiplus::SolidBrush brush(ToGdiplusColor(color));
    graphics->FillRectangle(&brush, Gdiplus::RectF(rc.left, rc.top, rc.Width(), rc.Height()));
}

void Render_GDI_Windows::FillRect(const UiRectF& rc, UiColor dwColor, UiColor dwColor2, int8_t nColor2Direction, uint8_t uFade)
{
    std::unique_ptr<Gdiplus::Graphics> graphics = CreateGdiplusGraphics(m_hMemDC, m_ptOrg);
    if (graphics == nullptr) {
        return;
    }
    const Gdiplus::RectF rect(rc.left, rc.top, rc.Width(), rc.Height());
    float angle = 0.0f;
    switch (nColor2Direction) {
    case 2: angle = 90.0f; break;
    case 3: angle = 45.0f; break;
    case 4: angle = 135.0f; break;
    default: angle = 0.0f; break;
    }
    Gdiplus::LinearGradientBrush brush(rect,
                                       ToGdiplusColor(dwColor, uFade),
                                       ToGdiplusColor(dwColor2, uFade),
                                       angle);
    graphics->FillRectangle(&brush, rect);
}

void Render_GDI_Windows::DrawRoundRectImpl(const UiRectF& rc, float rx, float ry, const IPen* pen, const IBrush* brush, bool bFill)
{
    std::unique_ptr<Gdiplus::Graphics> graphics = CreateGdiplusGraphics(m_hMemDC, m_ptOrg);
    if (graphics == nullptr) {
        return;
    }
    Gdiplus::GraphicsPath path;
    AddRoundRectPath(path, rc, rx, ry);
    if (bFill && (brush != nullptr)) {
        Gdiplus::SolidBrush gdiBrush(ToGdiplusColor(brush->GetColor()));
        graphics->FillPath(&gdiBrush, &path);
    }
    else if (pen != nullptr) {
        std::unique_ptr<Gdiplus::Pen> gdiPen = CreateGdiplusPen(pen);
        if (gdiPen != nullptr) {
            graphics->DrawPath(gdiPen.get(), &path);
        }
    }
}

void Render_GDI_Windows::DrawRoundRect(const UiRectF& rc, float rx, float ry, UiColor penColor, float fWidth)
{
    Pen_GDI pen(penColor, fWidth);
    DrawRoundRect(rc, rx, ry, &pen);
}

void Render_GDI_Windows::DrawRoundRect(const UiRectF& rc, float rx, float ry, IPen* pen)
{
    DrawRoundRectImpl(rc, rx, ry, pen, nullptr, false);
}

void Render_GDI_Windows::FillRoundRect(const UiRectF& rc, float rx, float ry, UiColor dwColor, uint8_t uFade)
{
    Brush_GDI brush(UiColor((uint8_t)((uint32_t)dwColor.GetAlpha() * uFade / 255), dwColor.GetRed(), dwColor.GetGreen(), dwColor.GetBlue()));
    DrawRoundRectImpl(rc, rx, ry, nullptr, &brush, true);
}

void Render_GDI_Windows::FillRoundRect(const UiRectF& rc, float rx, float ry, UiColor dwColor, UiColor dwColor2, int8_t nColor2Direction, uint8_t uFade)
{
    std::unique_ptr<Gdiplus::Graphics> graphics = CreateGdiplusGraphics(m_hMemDC, m_ptOrg);
    if (graphics == nullptr) {
        return;
    }
    Gdiplus::GraphicsPath path;
    AddRoundRectPath(path, rc, rx, ry);
    float angle = 0.0f;
    switch (nColor2Direction) {
    case 2: angle = 90.0f; break;
    case 3: angle = 45.0f; break;
    case 4: angle = 135.0f; break;
    default: angle = 0.0f; break;
    }
    Gdiplus::LinearGradientBrush brush(Gdiplus::RectF(rc.left, rc.top, rc.Width(), rc.Height()),
                                       ToGdiplusColor(dwColor, uFade),
                                       ToGdiplusColor(dwColor2, uFade),
                                       angle);
    graphics->FillPath(&brush, &path);
}

void Render_GDI_Windows::DrawArc(const UiRect& rc, float startAngle, float sweepAngle, bool useCenter,
                                 const IPen* pen,
                                 UiColor* /*gradientColor*/, const UiRect* /*gradientRect*/)
{
    std::unique_ptr<Gdiplus::Graphics> graphics = CreateGdiplusGraphics(m_hMemDC, m_ptOrg);
    std::unique_ptr<Gdiplus::Pen> gdiPen = CreateGdiplusPen(pen);
    if ((graphics == nullptr) || (gdiPen == nullptr)) {
        return;
    }
    Gdiplus::GraphicsPath path;
    path.AddArc(Gdiplus::Rect(rc.left, rc.top, rc.Width(), rc.Height()), startAngle, sweepAngle);
    if (useCenter) {
        path.AddLine(Gdiplus::PointF((float)(rc.left + rc.right) / 2.0f, (float)(rc.top + rc.bottom) / 2.0f),
                     Gdiplus::PointF((float)rc.left, (float)rc.top));
        path.CloseFigure();
    }
    graphics->DrawPath(gdiPen.get(), &path);
}

void Render_GDI_Windows::DrawCircle(const UiPointF& centerPt, float radius, UiColor penColor, float fWidth)
{
    Pen_GDI pen(penColor, fWidth);
    DrawCircle(centerPt, radius, &pen);
}

void Render_GDI_Windows::DrawCircle(const UiPointF& centerPt, float radius, IPen* pen)
{
    std::unique_ptr<Gdiplus::Graphics> graphics = CreateGdiplusGraphics(m_hMemDC, m_ptOrg);
    std::unique_ptr<Gdiplus::Pen> gdiPen = CreateGdiplusPen(pen);
    if ((graphics == nullptr) || (gdiPen == nullptr)) {
        return;
    }
    graphics->DrawEllipse(gdiPen.get(), centerPt.x - radius, centerPt.y - radius, radius * 2.0f, radius * 2.0f);
}

void Render_GDI_Windows::FillCircle(const UiPointF& centerPt, float radius, UiColor dwColor, uint8_t uFade)
{
    std::unique_ptr<Gdiplus::Graphics> graphics = CreateGdiplusGraphics(m_hMemDC, m_ptOrg);
    if (graphics == nullptr) {
        return;
    }
    Gdiplus::SolidBrush brush(ToGdiplusColor(dwColor, uFade));
    graphics->FillEllipse(&brush, centerPt.x - radius, centerPt.y - radius, radius * 2.0f, radius * 2.0f);
}

void Render_GDI_Windows::DrawPath(const IPath* path, const IPen* pen)
{
    const Path_GDI* pGdiPath = dynamic_cast<const Path_GDI*>(path);
    if ((pGdiPath == nullptr) || (pGdiPath->GetImpl() == nullptr)) {
        return;
    }
    std::unique_ptr<Gdiplus::Graphics> graphics = CreateGdiplusGraphics(m_hMemDC, m_ptOrg);
    std::unique_ptr<Gdiplus::Pen> gdiPen = CreateGdiplusPen(pen);
    if ((graphics == nullptr) || (gdiPen == nullptr)) {
        return;
    }
    graphics->DrawPath(gdiPen.get(), &pGdiPath->GetImpl()->m_path);
}

void Render_GDI_Windows::FillPath(const IPath* path, const IBrush* brush)
{
    const Path_GDI* pGdiPath = dynamic_cast<const Path_GDI*>(path);
    if ((pGdiPath == nullptr) || (pGdiPath->GetImpl() == nullptr) || (brush == nullptr)) {
        return;
    }
    std::unique_ptr<Gdiplus::Graphics> graphics = CreateGdiplusGraphics(m_hMemDC, m_ptOrg);
    if (graphics == nullptr) {
        return;
    }
    Gdiplus::SolidBrush gdiBrush(ToGdiplusColor(brush->GetColor()));
    graphics->FillPath(&gdiBrush, &pGdiPath->GetImpl()->m_path);
}

void Render_GDI_Windows::FillPath(const IPath* path, const UiRectF& rc, UiColor dwColor, UiColor dwColor2, int8_t nColor2Direction)
{
    const Path_GDI* pGdiPath = dynamic_cast<const Path_GDI*>(path);
    if ((pGdiPath == nullptr) || (pGdiPath->GetImpl() == nullptr)) {
        return;
    }
    std::unique_ptr<Gdiplus::Graphics> graphics = CreateGdiplusGraphics(m_hMemDC, m_ptOrg);
    if (graphics == nullptr) {
        return;
    }
    float angle = 0.0f;
    switch (nColor2Direction) {
    case 2: angle = 90.0f; break;
    case 3: angle = 45.0f; break;
    case 4: angle = 135.0f; break;
    default: angle = 0.0f; break;
    }
    Gdiplus::LinearGradientBrush brush(Gdiplus::RectF(rc.left, rc.top, rc.Width(), rc.Height()),
                                       ToGdiplusColor(dwColor), ToGdiplusColor(dwColor2), angle);
    graphics->FillPath(&brush, &pGdiPath->GetImpl()->m_path);
}

UiRect Render_GDI_Windows::MeasureString(const DString& strText, const MeasureStringParam& measureParam)
{
    if ((m_hMemDC == nullptr) || (measureParam.pFont == nullptr)) {
        return UiRect();
    }
    Font_GDI* pFont = dynamic_cast<Font_GDI*>(measureParam.pFont);
    if ((pFont == nullptr) || (pFont->GetFontHandle() == nullptr)) {
        return UiRect();
    }
    HGDIOBJ hOldFont = ::SelectObject(m_hMemDC, pFont->GetFontHandle());
    RECT rc = { 0, 0, measureParam.rectSize > 0 ? measureParam.rectSize : 0, 0 };
    if (measureParam.rectSize <= 0) {
        rc.right = 0x7FFFFFFF;
    }
    UINT nFormat = ToDrawTextFormat(measureParam.uFormat, false) | DT_CALCRECT;
    ::DrawTextW(m_hMemDC, strText.c_str(), (int)strText.size(), &rc, nFormat);
    ::SelectObject(m_hMemDC, hOldFont);
    return UiRect(rc.left, rc.top, rc.right, rc.bottom);
}

void Render_GDI_Windows::DrawString(const DString& strText, const DrawStringParam& drawParam)
{
    if (strText.empty() || (drawParam.pFont == nullptr)) {
        return;
    }
    Font_GDI* pFont = dynamic_cast<Font_GDI*>(drawParam.pFont);
    if ((pFont == nullptr) || (pFont->GetFontHandle() == nullptr)) {
        return;
    }
    std::unique_ptr<Gdiplus::Graphics> graphics = CreateGdiplusGraphics(m_hMemDC, m_ptOrg);
    if (graphics == nullptr) {
        return;
    }
    Gdiplus::Font gdiplusFont(m_hMemDC, pFont->GetFontHandle());
    std::unique_ptr<Gdiplus::StringFormat> pFormat = CreateStringFormat(drawParam.uFormat);
    if (pFormat == nullptr) {
        return;
    }
    Gdiplus::SolidBrush brush(ToGdiplusColor(drawParam.dwTextColor, drawParam.uFade));
    const Gdiplus::RectF rc((float)drawParam.textRect.left, (float)drawParam.textRect.top,
                            (float)drawParam.textRect.Width(), (float)drawParam.textRect.Height());
    graphics->DrawString(strText.c_str(), (INT)strText.size(), &gdiplusFont, rc, pFormat.get(), &brush);
}

void Render_GDI_Windows::MeasureRichText(const UiRect& textRect,
                                         const UiSize& /*szScrollOffset*/,
                                         IRenderFactory* /*pRenderFactory*/,
                                         const std::vector<RichTextData>& richTextData,
                                         std::vector<std::vector<UiRect>>* pRichTextRects)
{
    if (pRichTextRects != nullptr) {
        pRichTextRects->clear();
    }
    int32_t x = textRect.left;
    for (const RichTextData& data : richTextData) {
        if (data.m_pFontInfo == nullptr) {
            continue;
        }
        Font_GDI font(nullptr);
        font.InitFont(*data.m_pFontInfo);
        MeasureStringParam measureParam;
        measureParam.pFont = &font;
        measureParam.uFormat = data.m_textStyle;
        MeasureString(DString(data.m_textView.data(), data.m_textView.size()), measureParam);
    }
}

void Render_GDI_Windows::MeasureRichText2(const UiRect& textRect,
                                          const UiSize& szScrollOffset,
                                          IRenderFactory* pRenderFactory,
                                          const std::vector<RichTextData>& richTextData,
                                          RichTextLineInfoParam* pLineInfoParam,
                                          std::vector<std::vector<UiRect>>* pRichTextRects)
{
    MeasureRichText(textRect, szScrollOffset, pRenderFactory, richTextData, pRichTextRects);
    if (pLineInfoParam != nullptr) {
        pLineInfoParam->m_pLineInfoList->clear();
    }
}

void Render_GDI_Windows::MeasureRichText3(const UiRect& textRect,
                                          const UiSize& szScrollOffset,
                                          IRenderFactory* pRenderFactory,
                                          const std::vector<RichTextData>& richTextData,
                                          RichTextLineInfoParam* pLineInfoParam,
                                          std::shared_ptr<DrawRichTextCache>& spDrawRichTextCache,
                                          std::vector<std::vector<UiRect>>* pRichTextRects)
{
    spDrawRichTextCache.reset();
    MeasureRichText2(textRect, szScrollOffset, pRenderFactory, richTextData, pLineInfoParam, pRichTextRects);
}

void Render_GDI_Windows::DrawRichText(const UiRect& textRect,
                                      const UiSize& /*szScrollOffset*/,
                                      IRenderFactory* /*pRenderFactory*/,
                                      const std::vector<RichTextData>& richTextData,
                                      uint8_t uFade,
                                      std::vector<std::vector<UiRect>>* pRichTextRects)
{
    if (pRichTextRects != nullptr) {
        pRichTextRects->clear();
        pRichTextRects->resize(richTextData.size());
    }
    int32_t x = textRect.left;
    for (size_t i = 0; i < richTextData.size(); ++i) {
        const RichTextData& data = richTextData[i];
        if ((data.m_pFontInfo == nullptr) || data.m_textView.empty()) {
            continue;
        }
        Font_GDI font(nullptr);
        font.InitFont(*data.m_pFontInfo);
        DrawStringParam drawParam;
        drawParam.textRect = UiRect(x, textRect.top, textRect.right, textRect.bottom);
        drawParam.dwTextColor = data.m_textColor;
        drawParam.uFade = uFade;
        drawParam.pFont = &font;
        drawParam.uFormat = data.m_textStyle;
        DrawString(DString(data.m_textView.data(), data.m_textView.size()), drawParam);
        MeasureStringParam measureParam;
        measureParam.pFont = &font;
        measureParam.uFormat = data.m_textStyle;
        UiRect rc = MeasureString(DString(data.m_textView.data(), data.m_textView.size()), measureParam);
        if (pRichTextRects != nullptr) {
            (*pRichTextRects)[i].push_back(UiRect(x, textRect.top, x + rc.Width(), textRect.bottom));
        }
        x += rc.Width();
    }
}

bool Render_GDI_Windows::CreateDrawRichTextCache(const UiRect& /*textRect*/,
                                                 const UiSize& /*szScrollOffset*/,
                                                 IRenderFactory* /*pRenderFactory*/,
                                                 const std::vector<RichTextData>& /*richTextData*/,
                                                 std::shared_ptr<DrawRichTextCache>& spDrawRichTextCache)
{
    spDrawRichTextCache.reset();
    return false;
}

bool Render_GDI_Windows::IsValidDrawRichTextCache(const UiRect& /*textRect*/,
                                                  const std::vector<RichTextData>& /*richTextData*/,
                                                  const std::shared_ptr<DrawRichTextCache>& /*spDrawRichTextCache*/)
{
    return false;
}

bool Render_GDI_Windows::UpdateDrawRichTextCache(std::shared_ptr<DrawRichTextCache>& /*spOldDrawRichTextCache*/,
                                                 const std::shared_ptr<DrawRichTextCache>& /*spUpdateDrawRichTextCache*/,
                                                 std::vector<RichTextData>& /*richTextDataNew*/,
                                                 size_t /*nStartLine*/,
                                                 const std::vector<size_t>& /*modifiedLines*/,
                                                 size_t /*nModifiedRows*/,
                                                 const std::vector<size_t>& /*deletedLines*/,
                                                 size_t /*nDeletedRows*/,
                                                 const std::vector<int32_t>& /*rowRectTopList*/)
{
    return false;
}

bool Render_GDI_Windows::IsDrawRichTextCacheEqual(const DrawRichTextCache& /*first*/, const DrawRichTextCache& /*second*/) const
{
    return false;
}

void Render_GDI_Windows::DrawRichTextCacheData(const std::shared_ptr<DrawRichTextCache>& /*spDrawRichTextCache*/,
                                               const UiRect& /*rcNewTextRect*/,
                                               const UiSize& /*szNewScrollOffset*/,
                                               const std::vector<int32_t>& /*rowXOffset*/,
                                               uint8_t /*uFade*/,
                                               std::vector<std::vector<UiRect>>* /*pRichTextRects*/)
{
}

void Render_GDI_Windows::DrawBoxShadow(const UiRect& rc, const UiSize& /*roundSize*/, const UiPoint& cpOffset,
                                       int32_t nBlurRadius, int32_t /*nSpreadRadius*/, UiColor dwColor, uint8_t uAlpha)
{
    std::unique_ptr<Gdiplus::Graphics> graphics = CreateGdiplusGraphics(m_hMemDC, m_ptOrg);
    if (graphics == nullptr) {
        return;
    }
    const int32_t nSteps = std::max(1, std::min(16, nBlurRadius / 2 + 1));
    for (int32_t i = nSteps; i >= 1; --i) {
        const int32_t nOffset = (int32_t)std::round((float)nBlurRadius * i / nSteps);
        UiRectF rcShadow((float)(rc.left + cpOffset.x - nOffset),
                         (float)(rc.top + cpOffset.y - nOffset),
                         (float)(rc.right + cpOffset.x + nOffset),
                         (float)(rc.bottom + cpOffset.y + nOffset));
        const uint8_t nFade = (uint8_t)((uint32_t)uAlpha * (nSteps - i + 1) / (nSteps * 2));
        Gdiplus::SolidBrush brush(ToGdiplusColor(dwColor, nFade));
        graphics->FillRectangle(&brush, Gdiplus::RectF(rcShadow.left, rcShadow.top, rcShadow.Width(), rcShadow.Height()));
    }
}

IBitmap* Render_GDI_Windows::MakeImageSnapshot()
{
    if ((m_pPixelBits == nullptr) || (m_nWidth <= 0) || (m_nHeight <= 0)) {
        return nullptr;
    }
    Bitmap_GDI* pBitmap = new Bitmap_GDI;
    if (!pBitmap->Init((uint32_t)m_nWidth, (uint32_t)m_nHeight, m_pPixelBits, 1.0f, BitmapAlphaType::kPremul_SkAlphaType)) {
        delete pBitmap;
        pBitmap = nullptr;
    }
    return pBitmap;
}

void Render_GDI_Windows::ClearAlpha(const UiRect& rcDirty, uint8_t alpha)
{
    if (m_pPixelBits != nullptr) {
        BitmapAlpha bitmapAlpha((uint8_t*)m_pPixelBits, m_nWidth, m_nHeight, sizeof(uint32_t));
        bitmapAlpha.ClearAlpha(rcDirty, alpha);
    }
}

void Render_GDI_Windows::RestoreAlpha(const UiRect& rcDirty, const UiPadding& rcShadowPadding, uint8_t alpha)
{
    if (m_pPixelBits != nullptr) {
        BitmapAlpha bitmapAlpha((uint8_t*)m_pPixelBits, m_nWidth, m_nHeight, sizeof(uint32_t));
        bitmapAlpha.RestoreAlpha(rcDirty, rcShadowPadding, alpha);
    }
}

void Render_GDI_Windows::RestoreAlpha(const UiRect& rcDirty, const UiPadding& rcShadowPadding)
{
    if (m_pPixelBits != nullptr) {
        BitmapAlpha bitmapAlpha((uint8_t*)m_pPixelBits, m_nWidth, m_nHeight, sizeof(uint32_t));
        bitmapAlpha.RestoreAlpha(rcDirty, rcShadowPadding);
    }
}

void Render_GDI_Windows::Clear(const UiColor& uiColor)
{
    if (m_pPixelBits == nullptr) {
        return;
    }
    const uint32_t value = uiColor.GetARGB();
    uint32_t* pPixel = (uint32_t*)m_pPixelBits;
    const size_t nCount = (size_t)m_nWidth * m_nHeight;
    for (size_t i = 0; i < nCount; ++i) {
        pPixel[i] = value;
    }
}

void Render_GDI_Windows::ClearRect(const UiRect& rcDirty, const UiColor& uiColor)
{
    if (m_pPixelBits == nullptr) {
        return;
    }
    UiRect rc = rcDirty;
    rc.Intersect(UiRect(0, 0, m_nWidth, m_nHeight));
    if (rc.IsEmpty()) {
        return;
    }
    const uint32_t value = uiColor.GetARGB();
    for (int32_t y = rc.top; y < rc.bottom; ++y) {
        uint32_t* pPixel = (uint32_t*)m_pPixelBits + (size_t)y * m_nWidth + rc.left;
        for (int32_t x = rc.left; x < rc.right; ++x) {
            *pPixel++ = value;
        }
    }
}

std::unique_ptr<IRender> Render_GDI_Windows::Clone()
{
    std::unique_ptr<Render_GDI_Windows> pRender = std::make_unique<Render_GDI_Windows>(nullptr);
    if (pRender->Resize(m_nWidth, m_nHeight)) {
        ::BitBlt(pRender->m_hMemDC, 0, 0, m_nWidth, m_nHeight, m_hMemDC, 0, 0, SRCCOPY);
    }
    pRender->m_ptOrg = m_ptOrg;
    pRender->SetRenderDpi(m_spRenderDpi);
    return pRender;
}

bool Render_GDI_Windows::ReadPixels(const UiRect& rc, void* dstPixels, size_t dstPixelsLen)
{
    if ((m_pPixelBits == nullptr) || (dstPixels == nullptr) || rc.IsEmpty()) {
        return false;
    }
    const size_t nNeedLen = (size_t)rc.Width() * rc.Height() * sizeof(uint32_t);
    if (dstPixelsLen < nNeedLen) {
        return false;
    }
    for (int32_t y = 0; y < rc.Height(); ++y) {
        const uint32_t* pSrc = (const uint32_t*)m_pPixelBits + (size_t)(rc.top + y) * m_nWidth + rc.left;
        uint32_t* pDst = (uint32_t*)dstPixels + (size_t)y * rc.Width();
        std::memcpy(pDst, pSrc, rc.Width() * sizeof(uint32_t));
    }
    return true;
}

bool Render_GDI_Windows::WritePixels(void* srcPixels, size_t srcPixelsLen, const UiRect& rc)
{
    if ((m_pPixelBits == nullptr) || (srcPixels == nullptr) || rc.IsEmpty()) {
        return false;
    }
    if (srcPixelsLen != (size_t)rc.Width() * rc.Height() * sizeof(uint32_t)) {
        return false;
    }
    for (int32_t y = 0; y < rc.Height(); ++y) {
        uint32_t* pDst = (uint32_t*)m_pPixelBits + (size_t)(rc.top + y) * m_nWidth + rc.left;
        const uint32_t* pSrc = (const uint32_t*)srcPixels + (size_t)y * rc.Width();
        std::memcpy(pDst, pSrc, rc.Width() * sizeof(uint32_t));
    }
    return true;
}

bool Render_GDI_Windows::WritePixels(void* srcPixels, size_t srcPixelsLen, const UiRect& rc, const UiRect& rcPaint)
{
    UiRect rcUpdate = rc;
    rcUpdate.Intersect(rcPaint);
    if (rcUpdate.IsEmpty()) {
        return false;
    }
    const int32_t nOffsetX = rcUpdate.left - rc.left;
    const int32_t nOffsetY = rcUpdate.top - rc.top;
    const uint32_t* pSrc = (const uint32_t*)srcPixels;
    for (int32_t y = 0; y < rcUpdate.Height(); ++y) {
        const uint32_t* pSrcLine = pSrc + (size_t)(nOffsetY + y) * rc.Width() + nOffsetX;
        uint32_t* pDstLine = (uint32_t*)m_pPixelBits + (size_t)(rcUpdate.top + y) * m_nWidth + rcUpdate.left;
        std::memcpy(pDstLine, pSrcLine, rcUpdate.Width() * sizeof(uint32_t));
    }
    return true;
}

RenderClipType Render_GDI_Windows::GetClipInfo(std::vector<UiRect>& clipRects)
{
    clipRects.clear();
    RECT rc = {};
    const int nRet = ::GetClipBox(m_hMemDC, &rc);
    if ((nRet == NULLREGION) || (nRet == ERROR)) {
        return RenderClipType::kEmpty;
    }
    clipRects.push_back(UiRect(rc.left - m_ptOrg.x, rc.top - m_ptOrg.y, rc.right - m_ptOrg.x, rc.bottom - m_ptOrg.y));
    return RenderClipType::kRect;
}

bool Render_GDI_Windows::IsClipEmpty() const
{
    RECT rc = {};
    return ::GetClipBox(m_hMemDC, &rc) == NULLREGION;
}

bool Render_GDI_Windows::IsEmpty() const
{
    return (m_nWidth <= 0) || (m_nHeight <= 0);
}

void Render_GDI_Windows::SetRenderDpi(const IRenderDpiPtr& spRenderDpi)
{
    m_spRenderDpi = spRenderDpi;
}

bool Render_GDI_Windows::PaintAndSwapBuffers(IRenderPaint* pRenderPaint)
{
    ASSERT(pRenderPaint != nullptr);
    if ((pRenderPaint == nullptr) || !::IsWindow(m_hWnd)) {
        return false;
    }
    RECT rcUpdate = {};
    if (!::GetUpdateRect(m_hWnd, &rcUpdate, FALSE)) {
        return false;
    }
    PAINTSTRUCT ps = {};
    HDC hPaintDC = ::BeginPaint(m_hWnd, &ps);
    UiRect rcPaint(ps.rcPaint.left, ps.rcPaint.top, ps.rcPaint.right, ps.rcPaint.bottom);
    if (rcPaint.IsEmpty()) {
        rcPaint = UiRect(rcUpdate.left, rcUpdate.top, rcUpdate.right, rcUpdate.bottom);
    }
    bool bRet = false;
    if (!rcPaint.IsEmpty() && (hPaintDC != nullptr)) {
        bRet = pRenderPaint->DoPaint(rcPaint);
        if (::GetWindowLong(m_hWnd, GWL_EXSTYLE) & WS_EX_LAYERED) {
            RECT rcWindow = {};
            RECT rcClient = {};
            ::GetWindowRect(m_hWnd, &rcWindow);
            ::GetClientRect(m_hWnd, &rcClient);
            POINT ptDst = { rcWindow.left, rcWindow.top };
            SIZE sz = { rcClient.right - rcClient.left, rcClient.bottom - rcClient.top };
            POINT ptSrc = { 0, 0 };
            BLENDFUNCTION bf = { AC_SRC_OVER, 0, pRenderPaint->GetLayeredWindowAlpha(), AC_SRC_ALPHA };
            ::UpdateLayeredWindow(m_hWnd, nullptr, &ptDst, &sz, m_hMemDC, &ptSrc, 0, &bf, ULW_ALPHA);
        }
        else {
            ::BitBlt(hPaintDC, rcPaint.left, rcPaint.top, rcPaint.Width(), rcPaint.Height(),
                     m_hMemDC, rcPaint.left, rcPaint.top, SRCCOPY);
        }
    }
    if (hPaintDC != nullptr) {
        ::EndPaint(m_hWnd, &ps);
    }
    return bRet;
}

bool Render_GDI_Windows::SetWindowRoundRectRgn(const UiRect& rcWnd, float rx, float ry, bool bRedraw)
{
    if (!::IsWindow(m_hWnd)) {
        return false;
    }
    HRGN hRgn = ::CreateRoundRectRgn(rcWnd.left, rcWnd.top, rcWnd.right, rcWnd.bottom,
                                     (int)std::round(rx * 2.0f), (int)std::round(ry * 2.0f));
    ::SetWindowRgn(m_hWnd, hRgn, bRedraw ? TRUE : FALSE);
    return true;
}

bool Render_GDI_Windows::SetWindowRectRgn(const UiRect& rcWnd, bool bRedraw)
{
    if (!::IsWindow(m_hWnd)) {
        return false;
    }
    HRGN hRgn = ::CreateRectRgn(rcWnd.left, rcWnd.top, rcWnd.right, rcWnd.bottom);
    ::SetWindowRgn(m_hWnd, hRgn, bRedraw ? TRUE : FALSE);
    return true;
}

void Render_GDI_Windows::ClearWindowRgn(bool bRedraw)
{
    if (::IsWindow(m_hWnd)) {
        ::SetWindowRgn(m_hWnd, nullptr, bRedraw ? TRUE : FALSE);
    }
}

HDC Render_GDI_Windows::GetRenderDC(HWND /*hWnd*/)
{
    ::SetViewportOrgEx(m_hMemDC, m_ptOrg.x, m_ptOrg.y, nullptr);
    return m_hMemDC;
}

void Render_GDI_Windows::ReleaseRenderDC(HDC hdc)
{
    if (hdc == m_hMemDC) {
        ::SetViewportOrgEx(m_hMemDC, 0, 0, nullptr);
    }
}

} // namespace ui

#endif // DUILIB_BUILD_FOR_WIN
