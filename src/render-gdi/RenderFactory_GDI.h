#ifndef UI_RENDER_GDI_FACTORY_H_
#define UI_RENDER_GDI_FACTORY_H_

#include "render-gdi/GdiTypes.h"

#ifdef DUILIB_BUILD_FOR_WIN

#include <memory>

namespace ui {

/** GDI/GDI+ 渲染工厂
*/
class RenderFactory_GDI : public IRenderFactory
{
public:
    RenderFactory_GDI();
    RenderFactory_GDI(const RenderFactory_GDI &) = delete;
    RenderFactory_GDI &operator=(const RenderFactory_GDI &) = delete;
    virtual ~RenderFactory_GDI() override;

    virtual IFont *CreateIFont() override;
    virtual IPen *CreatePen(UiColor color, float fWidth = 1.0f) override;
    virtual IBrush *CreateBrush(UiColor color) override;
    virtual IPath *CreatePath() override;
    virtual IMatrix *CreateMatrix() override;
    virtual IBitmap *CreateBitmap() override;
    virtual IRender *CreateRender(
        const IRenderDpiPtr &spRenderDpi,
        void *platformData = nullptr,
        RenderBackendType backendType = RenderBackendType::kRaster_BackendType) override;
    virtual IFontMgr *GetFontMgr() const override;

private:
    std::shared_ptr<GdiFontMgr> m_pFontMgr;
    ULONG_PTR m_gdiplusToken = 0;
};

} // namespace ui

#endif // DUILIB_BUILD_FOR_WIN

#endif // UI_RENDER_GDI_FACTORY_H_
