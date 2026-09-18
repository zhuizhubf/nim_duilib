#include "duilib/RenderGDI/GdiTypes.h"

#ifdef DUILIB_BUILD_FOR_WIN

#include <gdiplus.h>

#include <algorithm>
#include <cmath>
#include <cstring>
#include <cwchar>

#pragma comment(lib, "gdiplus.lib")

namespace ui
{

namespace
{
Gdiplus::Color ToGdiplusColor(UiColor color, uint8_t alpha = 255)
{
    const uint8_t a = (uint8_t)((uint32_t)color.GetAlpha() * alpha / 255);
    return Gdiplus::Color(a, color.GetRed(), color.GetGreen(), color.GetBlue());
}

HBITMAP CreateDibSection(int32_t nWidth, int32_t nHeight, void** ppBits)
{
    if ((nWidth <= 0) || (nHeight <= 0)) {
        return nullptr;
    }
    BITMAPINFO bmi = {};
    bmi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
    bmi.bmiHeader.biWidth = nWidth;
    bmi.bmiHeader.biHeight = -nHeight;
    bmi.bmiHeader.biPlanes = 1;
    bmi.bmiHeader.biBitCount = 32;
    bmi.bmiHeader.biCompression = BI_RGB;
    bmi.bmiHeader.biSizeImage = (DWORD)(nWidth * nHeight * (int32_t)sizeof(uint32_t));

    HDC hdc = ::GetDC(nullptr);
    HBITMAP hBitmap = ::CreateDIBSection(hdc, &bmi, DIB_RGB_COLORS, ppBits, nullptr, 0);
    ::ReleaseDC(nullptr, hdc);
    return hBitmap;
}

void CopyPixelsToBitmap(uint8_t* pDest, uint32_t nWidth, uint32_t nHeight,
                        const void* pSrc, BitmapAlphaType alphaType)
{
    ASSERT(pDest != nullptr);
    if ((pDest == nullptr) || (nWidth == 0) || (nHeight == 0)) {
        return;
    }
    const size_t nCount = (size_t)nWidth * nHeight;
    if (pSrc == nullptr) {
        std::memset(pDest, 0, nCount * sizeof(uint32_t));
        return;
    }
    const uint32_t* pSrcPixels = (const uint32_t*)pSrc;
    uint32_t* pDestPixels = (uint32_t*)pDest;
    if (alphaType == BitmapAlphaType::kOpaque_SkAlphaType) {
        for (size_t i = 0; i < nCount; ++i) {
            pDestPixels[i] = (pSrcPixels[i] & 0x00FFFFFFu) | 0xFF000000u;
        }
    }
    else if (alphaType == BitmapAlphaType::kUnpremul_SkAlphaType) {
        for (size_t i = 0; i < nCount; ++i) {
            const uint32_t value = pSrcPixels[i];
            const uint32_t a = (value >> 24) & 0xFF;
            if (a == 0) {
                pDestPixels[i] = 0;
            }
            else if (a == 255) {
                pDestPixels[i] = value;
            }
            else {
                const uint32_t r = ((value >> 16) & 0xFF) * a / 255;
                const uint32_t g = ((value >> 8) & 0xFF) * a / 255;
                const uint32_t b = (value & 0xFF) * a / 255;
                pDestPixels[i] = (a << 24) | (r << 16) | (g << 8) | b;
            }
        }
    }
    else {
        std::memcpy(pDest, pSrc, nCount * sizeof(uint32_t));
    }
}
}

////////////////////////////////////////////////////////////////////////////////
// Bitmap_GDI
////////////////////////////////////////////////////////////////////////////////
Bitmap_GDI::Bitmap_GDI()
{
}

Bitmap_GDI::~Bitmap_GDI()
{
    DeleteBitmap();
}

void Bitmap_GDI::DeleteBitmap()
{
    if (m_hBitmap != nullptr) {
        ::DeleteObject(m_hBitmap);
        m_hBitmap = nullptr;
    }
    m_pPixelBits = nullptr;
    m_nWidth = 0;
    m_nHeight = 0;
}

bool Bitmap_GDI::Init(uint32_t nWidth, uint32_t nHeight,
                      const void* pPixelBits,
                      float /*fImageSizeScale*/,
                      BitmapAlphaType alphaType)
{
    DeleteBitmap();
    if ((nWidth == 0) || (nHeight == 0)) {
        return false;
    }
    m_hBitmap = CreateDibSection((int32_t)nWidth, (int32_t)nHeight, &m_pPixelBits);
    ASSERT(m_hBitmap != nullptr);
    if (m_hBitmap == nullptr) {
        return false;
    }
    m_nWidth = nWidth;
    m_nHeight = nHeight;
    CopyPixelsToBitmap((uint8_t*)m_pPixelBits, nWidth, nHeight, pPixelBits, alphaType);
    return true;
}

uint32_t Bitmap_GDI::GetWidth() const
{
    return m_nWidth;
}

uint32_t Bitmap_GDI::GetHeight() const
{
    return m_nHeight;
}

UiSize Bitmap_GDI::GetSize() const
{
    return UiSize((int32_t)m_nWidth, (int32_t)m_nHeight);
}

void* Bitmap_GDI::LockPixelBits()
{
    return m_pPixelBits;
}

void Bitmap_GDI::UnLockPixelBits()
{
}

IBitmap* Bitmap_GDI::Clone()
{
    Bitmap_GDI* pBitmap = new Bitmap_GDI;
    if (!pBitmap->Init(m_nWidth, m_nHeight, m_pPixelBits, 1.0f, BitmapAlphaType::kPremul_SkAlphaType)) {
        delete pBitmap;
        pBitmap = nullptr;
    }
    return pBitmap;
}

////////////////////////////////////////////////////////////////////////////////
// Pen_GDI
////////////////////////////////////////////////////////////////////////////////
Pen_GDI::Pen_GDI(UiColor color, float fWidth):
    m_color(color),
    m_fWidth(fWidth)
{
    if (m_fWidth <= 0.0f) {
        m_fWidth = 1.0f;
    }
}

void Pen_GDI::SetWidth(float fWidth)
{
    m_fWidth = (fWidth > 0.0f) ? fWidth : 1.0f;
}

float Pen_GDI::GetWidth() const
{
    return m_fWidth;
}

void Pen_GDI::SetColor(UiColor color)
{
    m_color = color;
}

UiColor Pen_GDI::GetColor() const
{
    return m_color;
}

void Pen_GDI::SetStartCap(LineCap cap)
{
    m_startCap = cap;
}

IPen::LineCap Pen_GDI::GetStartCap() const
{
    return m_startCap;
}

void Pen_GDI::SetEndCap(LineCap cap)
{
    m_endCap = cap;
}

IPen::LineCap Pen_GDI::GetEndCap() const
{
    return m_endCap;
}

void Pen_GDI::SetDashCap(LineCap cap)
{
    m_dashCap = cap;
}

IPen::LineCap Pen_GDI::GetDashCap() const
{
    return m_dashCap;
}

void Pen_GDI::SetLineJoin(LineJoin join)
{
    m_lineJoin = join;
}

IPen::LineJoin Pen_GDI::GetLineJoin() const
{
    return m_lineJoin;
}

void Pen_GDI::SetDashStyle(DashStyle style)
{
    m_dashStyle = style;
}

IPen::DashStyle Pen_GDI::GetDashStyle() const
{
    return m_dashStyle;
}

IPen* Pen_GDI::Clone() const
{
    Pen_GDI* pPen = new Pen_GDI(m_color, m_fWidth);
    pPen->m_startCap = m_startCap;
    pPen->m_endCap = m_endCap;
    pPen->m_dashCap = m_dashCap;
    pPen->m_lineJoin = m_lineJoin;
    pPen->m_dashStyle = m_dashStyle;
    return pPen;
}

////////////////////////////////////////////////////////////////////////////////
// Brush_GDI
////////////////////////////////////////////////////////////////////////////////
Brush_GDI::Brush_GDI(UiColor color):
    m_color(color)
{
}

IBrush* Brush_GDI::Clone()
{
    return new Brush_GDI(m_color);
}

UiColor Brush_GDI::GetColor() const
{
    return m_color;
}

////////////////////////////////////////////////////////////////////////////////
// Matrix_GDI
////////////////////////////////////////////////////////////////////////////////
Matrix_GDI::Matrix_GDI() = default;

void Matrix_GDI::Concat(const std::array<float, 6>& matrix)
{
    const float m11 = m_matrix[0];
    const float m12 = m_matrix[1];
    const float m21 = m_matrix[2];
    const float m22 = m_matrix[3];
    const float dx = m_matrix[4];
    const float dy = m_matrix[5];

    const float n11 = matrix[0];
    const float n12 = matrix[1];
    const float n21 = matrix[2];
    const float n22 = matrix[3];
    const float ndx = matrix[4];
    const float ndy = matrix[5];

    m_matrix[0] = m11 * n11 + m21 * n12;
    m_matrix[1] = m12 * n11 + m22 * n12;
    m_matrix[2] = m11 * n21 + m21 * n22;
    m_matrix[3] = m12 * n21 + m22 * n22;
    m_matrix[4] = m11 * ndx + m21 * ndy + dx;
    m_matrix[5] = m12 * ndx + m22 * ndy + dy;
}

void Matrix_GDI::Translate(float offsetX, float offsetY)
{
    Concat({ 1.0f, 0.0f, 0.0f, 1.0f, offsetX, offsetY });
}

void Matrix_GDI::Scale(float scaleX, float scaleY)
{
    Concat({ scaleX, 0.0f, 0.0f, scaleY, 0.0f, 0.0f });
}

void Matrix_GDI::Scale(float scaleX, float scaleY, float px, float py)
{
    Translate(-px, -py);
    Scale(scaleX, scaleY);
    Translate(px, py);
}

void Matrix_GDI::Rotate(float angle)
{
    const float radians = angle * 3.14159265358979323846f / 180.0f;
    const float cs = std::cos(radians);
    const float sn = std::sin(radians);
    Concat({ cs, sn, -sn, cs, 0.0f, 0.0f });
}

void Matrix_GDI::RotateAt(float angle, float px, float py)
{
    Translate(-px, -py);
    Rotate(angle);
    Translate(px, py);
}

void Matrix_GDI::Skew(float kx, float ky)
{
    const float tx = std::tan(kx * 3.14159265358979323846f / 180.0f);
    const float ty = std::tan(ky * 3.14159265358979323846f / 180.0f);
    Concat({ 1.0f, ty, tx, 1.0f, 0.0f, 0.0f });
}

void Matrix_GDI::Skew(float kx, float ky, float px, float py)
{
    Translate(-px, -py);
    Skew(kx, ky);
    Translate(px, py);
}

////////////////////////////////////////////////////////////////////////////////
// Path_GDI
////////////////////////////////////////////////////////////////////////////////
Path_GDI::Path_GDI():
    m_impl(std::make_unique<TImpl>())
{
}

Path_GDI::~Path_GDI() = default;

void Path_GDI::SetFillType(FillType mode)
{
    Gdiplus::FillMode fillMode = Gdiplus::FillModeAlternate;
    if ((mode == FillType::kWinding) || (mode == FillType::kInverseWinding)) {
        fillMode = Gdiplus::FillModeWinding;
    }
    m_impl->m_path.SetFillMode(fillMode);
}

IPath::FillType Path_GDI::GetFillType()
{
    return (m_impl->m_path.GetFillMode() == Gdiplus::FillModeWinding) ? FillType::kWinding : FillType::kEvenOdd;
}

void Path_GDI::AddLine(int32_t x1, int32_t y1, int32_t x2, int32_t y2)
{
    AddLine((float)x1, (float)y1, (float)x2, (float)y2);
}

void Path_GDI::AddLine(float x1, float y1, float x2, float y2)
{
    m_impl->m_path.AddLine(x1, y1, x2, y2);
}

void Path_GDI::AddLines(const UiPoint* points, int32_t count)
{
    if ((points == nullptr) || (count <= 0)) {
        return;
    }
    std::vector<Gdiplus::Point> pts;
    pts.reserve((size_t)count);
    for (int32_t i = 0; i < count; ++i) {
        pts.emplace_back(points[i].x, points[i].y);
    }
    m_impl->m_path.AddLines(pts.data(), count);
}

void Path_GDI::AddLines(const UiPointF* points, int32_t count)
{
    if ((points == nullptr) || (count <= 0)) {
        return;
    }
    std::vector<Gdiplus::PointF> pts;
    pts.reserve((size_t)count);
    for (int32_t i = 0; i < count; ++i) {
        pts.emplace_back(points[i].x, points[i].y);
    }
    m_impl->m_path.AddLines(pts.data(), count);
}

void Path_GDI::AddBezier(int32_t x1, int32_t y1, int32_t x2, int32_t y2, int32_t x3, int32_t y3, int32_t x4, int32_t y4)
{
    AddBezier((float)x1, (float)y1, (float)x2, (float)y2, (float)x3, (float)y3, (float)x4, (float)y4);
}

void Path_GDI::AddBezier(float x1, float y1, float x2, float y2, float x3, float y3, float x4, float y4)
{
    m_impl->m_path.AddBezier(x1, y1, x2, y2, x3, y3, x4, y4);
}

void Path_GDI::AddBeziers(const UiPoint* points, int32_t count)
{
    if ((points == nullptr) || (count <= 1)) {
        return;
    }
    std::vector<Gdiplus::Point> pts;
    pts.reserve((size_t)count);
    for (int32_t i = 0; i < count; ++i) {
        pts.emplace_back(points[i].x, points[i].y);
    }
    m_impl->m_path.AddBeziers(pts.data(), count);
}

void Path_GDI::AddBeziers(const UiPointF* points, int32_t count)
{
    if ((points == nullptr) || (count <= 1)) {
        return;
    }
    std::vector<Gdiplus::PointF> pts;
    pts.reserve((size_t)count);
    for (int32_t i = 0; i < count; ++i) {
        pts.emplace_back(points[i].x, points[i].y);
    }
    m_impl->m_path.AddBeziers(pts.data(), count);
}

void Path_GDI::AddRect(const UiRect& rect)
{
    m_impl->m_path.AddRectangle(Gdiplus::Rect(rect.left, rect.top, rect.Width(), rect.Height()));
}

void Path_GDI::AddRect(const UiRectF& rect)
{
    m_impl->m_path.AddRectangle(Gdiplus::RectF(rect.left, rect.top, rect.Width(), rect.Height()));
}

void Path_GDI::AddEllipse(const UiRect& rect)
{
    m_impl->m_path.AddEllipse(Gdiplus::Rect(rect.left, rect.top, rect.Width(), rect.Height()));
}

void Path_GDI::AddEllipse(const UiRectF& rect)
{
    m_impl->m_path.AddEllipse(Gdiplus::RectF(rect.left, rect.top, rect.Width(), rect.Height()));
}

void Path_GDI::AddArc(const UiRect& rect, float startAngle, float sweepAngle)
{
    m_impl->m_path.AddArc(Gdiplus::Rect(rect.left, rect.top, rect.Width(), rect.Height()), startAngle, sweepAngle);
}

void Path_GDI::AddArc(const UiRectF& rect, float startAngle, float sweepAngle)
{
    m_impl->m_path.AddArc(Gdiplus::RectF(rect.left, rect.top, rect.Width(), rect.Height()), startAngle, sweepAngle);
}

void Path_GDI::AddPolygon(const UiPoint* points, int32_t count)
{
    if ((points == nullptr) || (count <= 0)) {
        return;
    }
    std::vector<Gdiplus::Point> pts;
    pts.reserve((size_t)count);
    for (int32_t i = 0; i < count; ++i) {
        pts.emplace_back(points[i].x, points[i].y);
    }
    m_impl->m_path.AddPolygon(pts.data(), count);
}

void Path_GDI::AddPolygon(const UiPointF* points, int32_t count)
{
    if ((points == nullptr) || (count <= 0)) {
        return;
    }
    std::vector<Gdiplus::PointF> pts;
    pts.reserve((size_t)count);
    for (int32_t i = 0; i < count; ++i) {
        pts.emplace_back(points[i].x, points[i].y);
    }
    m_impl->m_path.AddPolygon(pts.data(), count);
}

void Path_GDI::Transform(IMatrix* pMatrix)
{
    Matrix_GDI* pGdiMatrix = dynamic_cast<Matrix_GDI*>(pMatrix);
    if (pGdiMatrix == nullptr) {
        return;
    }
    const std::array<float, 6>& values = pGdiMatrix->GetMatrix();
    Gdiplus::Matrix matrix(values[0], values[1], values[2], values[3], values[4], values[5]);
    m_impl->m_path.Transform(&matrix);
}

UiRect Path_GDI::GetBounds(const IPen* pen)
{
    Gdiplus::RectF bounds;
    if (m_impl->m_path.GetBounds(&bounds) != Gdiplus::Ok) {
        return UiRect();
    }
    if (pen != nullptr) {
        const float fInflate = pen->GetWidth() / 2.0f;
        bounds.Inflate(fInflate, fInflate);
    }
    return UiRect((int32_t)std::floor(bounds.X),
                  (int32_t)std::floor(bounds.Y),
                  (int32_t)std::ceil(bounds.GetRight()),
                  (int32_t)std::ceil(bounds.GetBottom()));
}

void Path_GDI::Close()
{
    m_impl->m_path.CloseFigure();
}

void Path_GDI::Reset()
{
    m_impl->m_path.Reset();
}

IPath* Path_GDI::Clone()
{
    Path_GDI* pPath = new Path_GDI;
    Gdiplus::GraphicsPath* pClone = m_impl->m_path.Clone();
    if (pClone != nullptr) {
        pPath->m_impl->m_path.Reset();
        pPath->m_impl->m_path.AddPath(pClone, TRUE);
        delete pClone;
    }
    return pPath;
}

////////////////////////////////////////////////////////////////////////////////
// Font_GDI
////////////////////////////////////////////////////////////////////////////////
Font_GDI::Font_GDI(GdiFontMgr* pFontMgr):
    m_pFontMgr(pFontMgr)
{
}

Font_GDI::~Font_GDI()
{
    DeleteFont();
}

void Font_GDI::DeleteFont()
{
    if (m_hFont != nullptr) {
        ::DeleteObject(m_hFont);
        m_hFont = nullptr;
    }
}

bool Font_GDI::InitFont(const UiFont& fontInfo)
{
    DeleteFont();
    m_fontName = fontInfo.m_fontName.c_str();
    if (m_fontName.empty() && (m_pFontMgr != nullptr)) {
        m_fontName = m_pFontMgr->GetDefaultFontName();
    }
    if (m_fontName.empty()) {
        m_fontName = _T("Microsoft YaHei");
    }
    const int32_t nFontSize = (fontInfo.m_fontSize > 0) ? fontInfo.m_fontSize : 12;
    m_fontSize = nFontSize;

    std::memset(&m_logFont, 0, sizeof(m_logFont));
    m_logFont.lfHeight = -nFontSize;
    m_logFont.lfWeight = fontInfo.m_bBold ? FW_BOLD : FW_NORMAL;
    m_logFont.lfItalic = fontInfo.m_bItalic ? TRUE : FALSE;
    m_logFont.lfUnderline = fontInfo.m_bUnderline ? TRUE : FALSE;
    m_logFont.lfStrikeOut = fontInfo.m_bStrikeOut ? TRUE : FALSE;
    m_logFont.lfCharSet = DEFAULT_CHARSET;
    m_logFont.lfOutPrecision = OUT_TT_PRECIS;
    m_logFont.lfQuality = CLEARTYPE_QUALITY;
    m_logFont.lfPitchAndFamily = DEFAULT_PITCH | FF_DONTCARE;
    wcsncpy_s(m_logFont.lfFaceName, LF_FACESIZE, m_fontName.c_str(), _TRUNCATE);

    m_hFont = ::CreateFontIndirectW(&m_logFont);
    if (m_hFont == nullptr) {
        m_logFont.lfFaceName[0] = 0;
        m_hFont = ::CreateFontIndirectW(&m_logFont);
    }
    return m_hFont != nullptr;
}

DString Font_GDI::FontName() const
{
    return m_fontName;
}

int32_t Font_GDI::FontSize() const
{
    return m_fontSize;
}

bool Font_GDI::IsBold() const
{
    return m_logFont.lfWeight >= FW_BOLD;
}

bool Font_GDI::IsUnderline() const
{
    return m_logFont.lfUnderline != FALSE;
}

bool Font_GDI::IsItalic() const
{
    return m_logFont.lfItalic != FALSE;
}

bool Font_GDI::IsStrikeOut() const
{
    return m_logFont.lfStrikeOut != FALSE;
}

bool Font_GDI::IsUnicodeCharSupported(uint32_t unicodeChar, uint16_t* glyphId)
{
    if ((m_hFont == nullptr) || (unicodeChar > 0xFFFF)) {
        return false;
    }
    HDC hdc = ::CreateCompatibleDC(nullptr);
    if (hdc == nullptr) {
        return false;
    }
    HGDIOBJ hOldFont = ::SelectObject(hdc, m_hFont);
    const wchar_t ch = (wchar_t)unicodeChar;
    WORD glyph = 0;
    const DWORD ret = ::GetGlyphIndicesW(hdc, &ch, 1, &glyph, GGI_MARK_NONEXISTING_GLYPHS);
    ::SelectObject(hdc, hOldFont);
    ::DeleteDC(hdc);
    if ((ret != GDI_ERROR) && (glyph != 0xFFFF)) {
        if (glyphId != nullptr) {
            *glyphId = glyph;
        }
        return true;
    }
    return false;
}

////////////////////////////////////////////////////////////////////////////////
// GdiFontMgr
////////////////////////////////////////////////////////////////////////////////
GdiFontMgr::GdiFontMgr()
{
}

GdiFontMgr::~GdiFontMgr()
{
    ClearFontFiles();
}

int CALLBACK GdiFontMgr::EnumFontCallback(const LOGFONTW* lpelfe, const TEXTMETRICW*, DWORD /*fontType*/, LPARAM lParam)
{
    GdiFontMgr* pFontMgr = (GdiFontMgr*)lParam;
    if ((pFontMgr != nullptr) && (lpelfe != nullptr) && (lpelfe->lfFaceName[0] != 0)) {
        pFontMgr->m_fontNames.insert(lpelfe->lfFaceName);
    }
    return 1;
}

void GdiFontMgr::EnsureFontNames() const
{
    if (m_bFontNamesLoaded) {
        return;
    }
    m_fontNames.clear();
    HDC hdc = ::GetDC(nullptr);
    if (hdc != nullptr) {
        LOGFONTW lf = {};
        lf.lfCharSet = DEFAULT_CHARSET;
        ::EnumFontFamiliesExW(hdc, &lf, (FONTENUMPROCW)EnumFontCallback, (LPARAM)this, 0);
        ::ReleaseDC(nullptr, hdc);
    }
    m_fontNameList.assign(m_fontNames.begin(), m_fontNames.end());
    m_bFontNamesLoaded = true;
}

uint32_t GdiFontMgr::GetFontCount() const
{
    EnsureFontNames();
    return (uint32_t)m_fontNameList.size();
}

bool GdiFontMgr::GetFontName(uint32_t nIndex, DString& fontName) const
{
    EnsureFontNames();
    if (nIndex >= m_fontNameList.size()) {
        return false;
    }
    fontName = m_fontNameList[nIndex];
    return true;
}

bool GdiFontMgr::HasFontName(const DString& fontName) const
{
    EnsureFontNames();
    for (const DString& name : m_fontNameList) {
        if (_wcsicmp(name.c_str(), fontName.c_str()) == 0) {
            return true;
        }
    }
    return false;
}

void GdiFontMgr::SetDefaultFontName(const DString& fontName)
{
    m_defaultFontName = fontName;
}

bool GdiFontMgr::LoadFontFile(const DString& fontFilePath)
{
    if (fontFilePath.empty()) {
        return false;
    }
    const int nRet = ::AddFontResourceExW(fontFilePath.c_str(), FR_PRIVATE, nullptr);
    if (nRet > 0) {
        m_fontFilePaths.push_back(fontFilePath);
        ClearFontCache();
        return true;
    }
    return false;
}

bool GdiFontMgr::LoadFontFileData(const void* data, size_t length)
{
    if ((data == nullptr) || (length == 0)) {
        return false;
    }
    m_fontDataBuffers.emplace_back((const uint8_t*)data, (const uint8_t*)data + length);
    std::vector<uint8_t>& fontData = m_fontDataBuffers.back();
    DWORD nFonts = 0;
    HANDLE hFont = ::AddFontMemResourceEx(fontData.data(), (DWORD)fontData.size(), nullptr, &nFonts);
    if ((hFont != nullptr) && (nFonts > 0)) {
        m_fontMemHandles.push_back(hFont);
        ClearFontCache();
        return true;
    }
    m_fontDataBuffers.pop_back();
    return false;
}

void GdiFontMgr::ClearFontFiles()
{
    for (const DString& fontFilePath : m_fontFilePaths) {
        ::RemoveFontResourceExW(fontFilePath.c_str(), FR_PRIVATE, nullptr);
    }
    m_fontFilePaths.clear();
    for (HANDLE hFont : m_fontMemHandles) {
        ::RemoveFontMemResourceEx(hFont);
    }
    m_fontMemHandles.clear();
    m_fontDataBuffers.clear();
    ClearFontCache();
}

void GdiFontMgr::ClearFontCache()
{
    m_bFontNamesLoaded = false;
    m_fontNames.clear();
    m_fontNameList.clear();
}

void GdiFontMgr::SetFallbackFontMgr(IFallbackFontMgr* pFallbackFontMgr)
{
    m_pFallbackFontMgr = pFallbackFontMgr;
}

IFallbackFontMgr* GdiFontMgr::GetFallbackFontMgr() const
{
    return m_pFallbackFontMgr;
}

} // namespace ui

#endif // DUILIB_BUILD_FOR_WIN
