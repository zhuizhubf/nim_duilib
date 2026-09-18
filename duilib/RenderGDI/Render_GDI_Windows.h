#ifndef UI_RENDER_GDI_WINDOWS_H_
#define UI_RENDER_GDI_WINDOWS_H_

#include "duilib/RenderGDI/GdiTypes.h"

#ifdef DUILIB_BUILD_FOR_WIN

namespace ui
{

/** Windows GDI/GDI+ 渲染实现
*/
class Render_GDI_Windows: public IRender
{
public:
    explicit Render_GDI_Windows(HWND hWnd);
    Render_GDI_Windows(const Render_GDI_Windows&) = delete;
    Render_GDI_Windows& operator=(const Render_GDI_Windows&) = delete;
    virtual ~Render_GDI_Windows() override;

    virtual RenderType GetRenderType() const override;
    virtual RenderCapabilities GetCapabilities() const override;
    virtual RenderBackendType GetRenderBackendType() const override;

    virtual int32_t GetWidth() const override;
    virtual int32_t GetHeight() const override;
    virtual bool Resize(int32_t width, int32_t height) override;

    virtual UiPoint OffsetWindowOrg(UiPoint ptOffset) override;
    virtual UiPoint SetWindowOrg(UiPoint pt) override;
    virtual UiPoint GetWindowOrg() const override;

    virtual void SaveClip(int32_t& nState) override;
    virtual void RestoreClip(int32_t nState) override;
    virtual int32_t SetClip(const UiRect& rc, bool bIntersect = true) override;
    virtual int32_t SetRoundClip(const UiRect& rcItem, float rx, float ry, bool bIntersect = true) override;
    virtual void ClearClip(int32_t nState) override;

    virtual bool BitBlt(int32_t x, int32_t y, int32_t cx, int32_t cy,
                        IRender* pSrcRender, int32_t xSrc, int32_t ySrc,
                        RopMode rop) override;
    virtual bool StretchBlt(int32_t xDest, int32_t yDest, int32_t widthDest, int32_t heightDest,
                            IRender* pSrcRender, int32_t xSrc, int32_t ySrc,
                            int32_t widthSrc, int32_t heightSrc,
                            RopMode rop) override;
    virtual bool AlphaBlend(int32_t xDest, int32_t yDest, int32_t widthDest, int32_t heightDest,
                            IRender* pSrcRender, int32_t xSrc, int32_t ySrc,
                            int32_t widthSrc, int32_t heightSrc,
                            uint8_t alpha = 255) override;

    virtual void DrawImage(const UiRect& rcPaint, IBitmap* pBitmap,
                           const UiRect& rcDest, const UiRect& rcDestCorners,
                           const UiRect& rcSource, const UiRect& rcSourceCorners,
                           uint8_t uFade = 255,
                           const TiledDrawParam* pTiledDrawParam = nullptr,
                           bool bWindowShadowMode = false) override;
    virtual void DrawImage(const UiRect& rcPaint, IBitmap* pBitmap,
                           const UiRect& rcDest, const UiRect& rcSource,
                           uint8_t uFade = 255,
                           const TiledDrawParam* pTiledDrawParam = nullptr,
                           bool bWindowShadowMode = false) override;
    virtual void DrawImageRect(const UiRect& rcPaint, IBitmap* pBitmap,
                               const UiRect& rcDest, const UiRect& rcSource,
                               uint8_t uFade = 255, IMatrix* pMatrix = nullptr) override;

    virtual void DrawLine(const UiPointF& pt1, const UiPointF& pt2, UiColor penColor, float fWidth) override;
    virtual void DrawLine(const UiPointF& pt1, const UiPointF& pt2, IPen* pen) override;
    virtual void DrawRect(const UiRectF& rc, UiColor penColor, float fWidth, bool bLineInRect = false) override;
    virtual void DrawRect(const UiRectF& rc, IPen* pen, bool bLineInRect = false) override;
    virtual void FillRect(const UiRectF& rc, UiColor dwColor, uint8_t uFade = 255) override;
    virtual void FillRect(const UiRectF& rc, UiColor dwColor, UiColor dwColor2, int8_t nColor2Direction, uint8_t uFade = 255) override;

    virtual void DrawRoundRect(const UiRectF& rc, float rx, float ry, UiColor penColor, float fWidth) override;
    virtual void DrawRoundRect(const UiRectF& rc, float rx, float ry, IPen* pen) override;
    virtual void FillRoundRect(const UiRectF& rc, float rx, float ry, UiColor dwColor, uint8_t uFade = 255) override;
    virtual void FillRoundRect(const UiRectF& rc, float rx, float ry, UiColor dwColor, UiColor dwColor2, int8_t nColor2Direction, uint8_t uFade = 255) override;

    virtual void DrawArc(const UiRect& rc, float startAngle, float sweepAngle, bool useCenter,
                         const IPen* pen,
                         UiColor* gradientColor = nullptr, const UiRect* gradientRect = nullptr) override;
    virtual void DrawCircle(const UiPointF& centerPt, float radius, UiColor penColor, float fWidth) override;
    virtual void DrawCircle(const UiPointF& centerPt, float radius, IPen* pen) override;
    virtual void FillCircle(const UiPointF& centerPt, float radius, UiColor dwColor, uint8_t uFade = 255) override;

    virtual void DrawPath(const IPath* path, const IPen* pen) override;
    virtual void FillPath(const IPath* path, const IBrush* brush) override;
    virtual void FillPath(const IPath* path, const UiRectF& rc, UiColor dwColor, UiColor dwColor2, int8_t nColor2Direction) override;

    virtual UiRect MeasureString(const DString& strText, const MeasureStringParam& measureParam) override;
    virtual void DrawString(const DString& strText, const DrawStringParam& drawParam) override;

    virtual void MeasureRichText(const UiRect& textRect,
                                 const UiSize& szScrollOffset,
                                 IRenderFactory* pRenderFactory,
                                 const std::vector<RichTextData>& richTextData,
                                 std::vector<std::vector<UiRect>>* pRichTextRects) override;
    virtual void MeasureRichText2(const UiRect& textRect,
                                  const UiSize& szScrollOffset,
                                  IRenderFactory* pRenderFactory,
                                  const std::vector<RichTextData>& richTextData,
                                  RichTextLineInfoParam* pLineInfoParam,
                                  std::vector<std::vector<UiRect>>* pRichTextRects) override;
    virtual void MeasureRichText3(const UiRect& textRect,
                                  const UiSize& szScrollOffset,
                                  IRenderFactory* pRenderFactory,
                                  const std::vector<RichTextData>& richTextData,
                                  RichTextLineInfoParam* pLineInfoParam,
                                  std::shared_ptr<DrawRichTextCache>& spDrawRichTextCache,
                                  std::vector<std::vector<UiRect>>* pRichTextRects) override;
    virtual void DrawRichText(const UiRect& textRect,
                              const UiSize& szScrollOffset,
                              IRenderFactory* pRenderFactory,
                              const std::vector<RichTextData>& richTextData,
                              uint8_t uFade = 255,
                              std::vector<std::vector<UiRect>>* pRichTextRects = nullptr) override;
    virtual bool CreateDrawRichTextCache(const UiRect& textRect,
                                         const UiSize& szScrollOffset,
                                         IRenderFactory* pRenderFactory,
                                         const std::vector<RichTextData>& richTextData,
                                         std::shared_ptr<DrawRichTextCache>& spDrawRichTextCache) override;
    virtual bool IsValidDrawRichTextCache(const UiRect& textRect,
                                          const std::vector<RichTextData>& richTextData,
                                          const std::shared_ptr<DrawRichTextCache>& spDrawRichTextCache) override;
    virtual bool UpdateDrawRichTextCache(std::shared_ptr<DrawRichTextCache>& spOldDrawRichTextCache,
                                         const std::shared_ptr<DrawRichTextCache>& spUpdateDrawRichTextCache,
                                         std::vector<RichTextData>& richTextDataNew,
                                         size_t nStartLine,
                                         const std::vector<size_t>& modifiedLines,
                                         size_t nModifiedRows,
                                         const std::vector<size_t>& deletedLines,
                                         size_t nDeletedRows,
                                         const std::vector<int32_t>& rowRectTopList) override;
    virtual bool IsDrawRichTextCacheEqual(const DrawRichTextCache& first, const DrawRichTextCache& second) const override;
    virtual void DrawRichTextCacheData(const std::shared_ptr<DrawRichTextCache>& spDrawRichTextCache,
                                       const UiRect& rcNewTextRect,
                                       const UiSize& szNewScrollOffset,
                                       const std::vector<int32_t>& rowXOffset,
                                       uint8_t uFade,
                                       std::vector<std::vector<UiRect>>* pRichTextRects = nullptr) override;

    virtual void DrawBoxShadow(const UiRect& rc,
                               const UiSize& roundSize,
                               const UiPoint& cpOffset,
                               int32_t nBlurRadius,
                               int32_t nSpreadRadius,
                               UiColor dwColor,
                               uint8_t uAlpha) override;

    virtual IBitmap* MakeImageSnapshot() override;
    virtual void ClearAlpha(const UiRect& rcDirty, uint8_t alpha = 0) override;
    virtual void RestoreAlpha(const UiRect& rcDirty, const UiPadding& rcShadowPadding, uint8_t alpha) override;
    virtual void RestoreAlpha(const UiRect& rcDirty, const UiPadding& rcShadowPadding = UiPadding()) override;

    virtual void Clear(const UiColor& uiColor) override;
    virtual void ClearRect(const UiRect& rcDirty, const UiColor& uiColor) override;
    virtual std::unique_ptr<IRender> Clone() override;
    virtual bool ReadPixels(const UiRect& rc, void* dstPixels, size_t dstPixelsLen) override;
    virtual bool WritePixels(void* srcPixels, size_t srcPixelsLen, const UiRect& rc) override;
    virtual bool WritePixels(void* srcPixels, size_t srcPixelsLen, const UiRect& rc, const UiRect& rcPaint) override;
    virtual RenderClipType GetClipInfo(std::vector<UiRect>& clipRects) override;
    virtual bool IsClipEmpty() const override;
    virtual bool IsEmpty() const override;
    virtual void SetRenderDpi(const IRenderDpiPtr& spRenderDpi) override;
    virtual bool PaintAndSwapBuffers(IRenderPaint* pRenderPaint) override;
    virtual bool SetWindowRoundRectRgn(const UiRect& rcWnd, float rx, float ry, bool bRedraw) override;
    virtual bool SetWindowRectRgn(const UiRect& rcWnd, bool bRedraw) override;
    virtual void ClearWindowRgn(bool bRedraw) override;

    virtual HDC GetRenderDC(HWND hWnd) override;
    virtual void ReleaseRenderDC(HDC hdc) override;

private:
    bool CreateDib(int32_t nWidth, int32_t nHeight);
    void DeleteDib();
    void DrawBitmapRect(IBitmap* pBitmap, const UiRect& rcDest, const UiRect& rcSource, uint8_t uFade);
    void DrawRoundRectImpl(const UiRectF& rc, float rx, float ry, const IPen* pen, const IBrush* brush, bool bFill);

private:
    HWND m_hWnd = nullptr;
    HDC m_hMemDC = nullptr;
    HBITMAP m_hBitmap = nullptr;
    HGDIOBJ m_hOldBitmap = nullptr;
    void* m_pPixelBits = nullptr;
    int32_t m_nWidth = 0;
    int32_t m_nHeight = 0;
    UiPoint m_ptOrg = { 0, 0 };
    IRenderDpiPtr m_spRenderDpi;
};

} // namespace ui

#endif // DUILIB_BUILD_FOR_WIN

#endif // UI_RENDER_GDI_WINDOWS_H_
