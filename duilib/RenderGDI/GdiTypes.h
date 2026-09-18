#ifndef UI_RENDER_GDI_TYPES_H_
#define UI_RENDER_GDI_TYPES_H_

#include "duilib/Render/IRender.h"

#ifdef DUILIB_BUILD_FOR_WIN

#include <windows.h>
#include <objidl.h>
#include <propidl.h>
#include <gdiplus.h>

#include <array>
#include <memory>
#include <set>
#include <vector>

namespace ui
{
class GdiFontMgr;

/** GDI 位图实现
*/
class Bitmap_GDI: public IBitmap
{
public:
    Bitmap_GDI();
    Bitmap_GDI(const Bitmap_GDI&) = delete;
    Bitmap_GDI& operator=(const Bitmap_GDI&) = delete;
    virtual ~Bitmap_GDI() override;

    virtual bool Init(uint32_t nWidth, uint32_t nHeight,
                      const void* pPixelBits,
                      float fImageSizeScale = 1.0f,
                      BitmapAlphaType alphaType = BitmapAlphaType::kPremul_SkAlphaType) override;

    virtual uint32_t GetWidth() const override;
    virtual uint32_t GetHeight() const override;
    virtual UiSize GetSize() const override;
    virtual void* LockPixelBits() override;
    virtual void UnLockPixelBits() override;
    virtual IBitmap* Clone() override;

public:
    HBITMAP GetBitmap() const { return m_hBitmap; }
    uint32_t GetStride() const { return m_nWidth * sizeof(uint32_t); }

private:
    void DeleteBitmap();

private:
    HBITMAP m_hBitmap = nullptr;
    void* m_pPixelBits = nullptr;
    uint32_t m_nWidth = 0;
    uint32_t m_nHeight = 0;
};

/** GDI 画笔实现
*/
class Pen_GDI: public IPen
{
public:
    Pen_GDI(UiColor color, float fWidth);
    Pen_GDI(const Pen_GDI&) = delete;
    Pen_GDI& operator=(const Pen_GDI&) = delete;
    virtual ~Pen_GDI() override = default;

    virtual void SetWidth(float fWidth) override;
    virtual float GetWidth() const override;
    virtual void SetColor(UiColor color) override;
    virtual UiColor GetColor() const override;
    virtual void SetStartCap(LineCap cap) override;
    virtual LineCap GetStartCap() const override;
    virtual void SetEndCap(LineCap cap) override;
    virtual LineCap GetEndCap() const override;
    virtual void SetDashCap(LineCap cap) override;
    virtual LineCap GetDashCap() const override;
    virtual void SetLineJoin(LineJoin join) override;
    virtual LineJoin GetLineJoin() const override;
    virtual void SetDashStyle(DashStyle style) override;
    virtual DashStyle GetDashStyle() const override;
    virtual IPen* Clone() const override;

private:
    UiColor m_color;
    float m_fWidth = 1.0f;
    LineCap m_startCap = kButt_Cap;
    LineCap m_endCap = kButt_Cap;
    LineCap m_dashCap = kButt_Cap;
    LineJoin m_lineJoin = kMiter_Join;
    DashStyle m_dashStyle = kDashStyleSolid;
};

/** GDI 画刷实现
*/
class Brush_GDI: public IBrush
{
public:
    explicit Brush_GDI(UiColor color);
    Brush_GDI(const Brush_GDI&) = delete;
    Brush_GDI& operator=(const Brush_GDI&) = delete;
    virtual ~Brush_GDI() override = default;

    virtual IBrush* Clone() override;
    virtual UiColor GetColor() const override;

private:
    UiColor m_color;
};

/** GDI 矩阵实现
*/
class Matrix_GDI: public IMatrix
{
public:
    Matrix_GDI();
    Matrix_GDI(const Matrix_GDI&) = delete;
    Matrix_GDI& operator=(const Matrix_GDI&) = delete;
    virtual ~Matrix_GDI() override = default;

    virtual void Translate(float offsetX, float offsetY) override;
    virtual void Scale(float scaleX, float scaleY) override;
    virtual void Scale(float scaleX, float scaleY, float px, float py) override;
    virtual void Rotate(float angle) override;
    virtual void RotateAt(float angle, float px, float py) override;
    virtual void Skew(float kx, float ky) override;
    virtual void Skew(float kx, float ky, float px, float py) override;

public:
    /** 获取 GDI+ 使用的仿射矩阵（M11,M12,M21,M22,DX,DY）
    */
    const std::array<float, 6>& GetMatrix() const { return m_matrix; }

private:
    void Concat(const std::array<float, 6>& matrix);

private:
    // 与 GDI+ Matrix 的字段顺序保持一致
    std::array<float, 6> m_matrix = { 1.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f };
};

/** GDI 路径实现
*/
class Path_GDI: public IPath
{
public:
    Path_GDI();
    Path_GDI(const Path_GDI&) = delete;
    Path_GDI& operator=(const Path_GDI&) = delete;
    virtual ~Path_GDI() override;

    virtual void SetFillType(FillType mode) override;
    virtual FillType GetFillType() override;
    virtual void AddLine(int32_t x1, int32_t y1, int32_t x2, int32_t y2) override;
    virtual void AddLine(float x1, float y1, float x2, float y2) override;
    virtual void AddLines(const UiPoint* points, int32_t count) override;
    virtual void AddLines(const UiPointF* points, int32_t count) override;
    virtual void AddBezier(int32_t x1, int32_t y1, int32_t x2, int32_t y2, int32_t x3, int32_t y3, int32_t x4, int32_t y4) override;
    virtual void AddBezier(float x1, float y1, float x2, float y2, float x3, float y3, float x4, float y4) override;
    virtual void AddBeziers(const UiPoint* points, int32_t count) override;
    virtual void AddBeziers(const UiPointF* points, int32_t count) override;
    virtual void AddRect(const UiRect& rect) override;
    virtual void AddRect(const UiRectF& rect) override;
    virtual void AddEllipse(const UiRect& rect) override;
    virtual void AddEllipse(const UiRectF& rect) override;
    virtual void AddArc(const UiRect& rect, float startAngle, float sweepAngle) override;
    virtual void AddArc(const UiRectF& rect, float startAngle, float sweepAngle) override;
    virtual void AddPolygon(const UiPoint* points, int32_t count) override;
    virtual void AddPolygon(const UiPointF* points, int32_t count) override;
    virtual void Transform(IMatrix* pMatrix) override;
    virtual UiRect GetBounds(const IPen* pen) override;
    virtual void Close() override;
    virtual void Reset() override;
    virtual IPath* Clone() override;

public:
    class TImpl
    {
    public:
        TImpl(): m_path(Gdiplus::FillModeAlternate) { }
        Gdiplus::GraphicsPath m_path;
    };
    TImpl* GetImpl() const { return m_impl.get(); }

private:
    std::unique_ptr<TImpl> m_impl;
};

/** GDI 字体实现
*/
class Font_GDI: public IFont
{
public:
    explicit Font_GDI(GdiFontMgr* pFontMgr);
    Font_GDI(const Font_GDI&) = delete;
    Font_GDI& operator=(const Font_GDI&) = delete;
    virtual ~Font_GDI() override;

    virtual bool InitFont(const UiFont& fontInfo) override;
    virtual DString FontName() const override;
    virtual int32_t FontSize() const override;
    virtual bool IsBold() const override;
    virtual bool IsUnderline() const override;
    virtual bool IsItalic() const override;
    virtual bool IsStrikeOut() const override;
    virtual bool IsUnicodeCharSupported(uint32_t unicodeChar, uint16_t* glyphId) override;

public:
    HFONT GetFontHandle() const { return m_hFont; }
    const LOGFONTW& GetLogFont() const { return m_logFont; }
    GdiFontMgr* GetFontMgr() const { return m_pFontMgr; }

private:
    void DeleteFont();

private:
    GdiFontMgr* m_pFontMgr = nullptr;
    HFONT m_hFont = nullptr;
    LOGFONTW m_logFont = {};
    DString m_fontName;
    int32_t m_fontSize = 0;
};

/** GDI 字体管理器实现
*/
class GdiFontMgr: public IFontMgr
{
public:
    GdiFontMgr();
    GdiFontMgr(const GdiFontMgr&) = delete;
    GdiFontMgr& operator=(const GdiFontMgr&) = delete;
    virtual ~GdiFontMgr() override;

    virtual uint32_t GetFontCount() const override;
    virtual bool GetFontName(uint32_t nIndex, DString& fontName) const override;
    virtual bool HasFontName(const DString& fontName) const override;
    virtual void SetDefaultFontName(const DString& fontName) override;
    virtual bool LoadFontFile(const DString& fontFilePath) override;
    virtual bool LoadFontFileData(const void* data, size_t length) override;
    virtual void ClearFontFiles() override;
    virtual void ClearFontCache() override;
    virtual void SetFallbackFontMgr(IFallbackFontMgr* pFallbackFontMgr) override;
    virtual IFallbackFontMgr* GetFallbackFontMgr() const override;

    /** 获取默认字体名称
    */
    const DString& GetDefaultFontName() const { return m_defaultFontName; }

private:
    void EnsureFontNames() const;
    static int CALLBACK EnumFontCallback(const LOGFONTW* lpelfe, const TEXTMETRICW*, DWORD fontType, LPARAM lParam);

private:
    mutable bool m_bFontNamesLoaded = false;
    mutable std::set<DString> m_fontNames;
    mutable std::vector<DString> m_fontNameList;
    DString m_defaultFontName;
    IFallbackFontMgr* m_pFallbackFontMgr = nullptr;
    std::vector<HANDLE> m_fontMemHandles;
    std::vector<DString> m_fontFilePaths;
};

} // namespace ui

#endif // DUILIB_BUILD_FOR_WIN

#endif // UI_RENDER_GDI_TYPES_H_
