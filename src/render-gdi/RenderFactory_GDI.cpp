#include "render-gdi/RenderFactory_GDI.h"

#ifdef DUILIB_BUILD_FOR_WIN

#include "render/IRenderBackend.h"
#include "render-gdi/Render_GDI_Windows.h"

#include <gdiplus.h>

#pragma comment(lib, "gdiplus.lib")

namespace ui
{

RenderFactory_GDI::RenderFactory_GDI()
{
    Gdiplus::GdiplusStartupInput startupInput;
    const Gdiplus::Status status = Gdiplus::GdiplusStartup(&m_gdiplusToken, &startupInput, nullptr);
    ASSERT(status == Gdiplus::Ok);
    if (status != Gdiplus::Ok) {
        m_gdiplusToken = 0;
    }
    m_pFontMgr = std::make_shared<GdiFontMgr>();
}

RenderFactory_GDI::~RenderFactory_GDI()
{
    m_pFontMgr.reset();
    if (m_gdiplusToken != 0) {
        Gdiplus::GdiplusShutdown(m_gdiplusToken);
        m_gdiplusToken = 0;
    }
}

IFont* RenderFactory_GDI::CreateIFont()
{
    return new Font_GDI(m_pFontMgr.get());
}

IPen* RenderFactory_GDI::CreatePen(UiColor color, float fWidth)
{
    return new Pen_GDI(color, fWidth);
}

IBrush* RenderFactory_GDI::CreateBrush(UiColor color)
{
    return new Brush_GDI(color);
}

IPath* RenderFactory_GDI::CreatePath()
{
    return new Path_GDI;
}

IMatrix* RenderFactory_GDI::CreateMatrix()
{
    return new Matrix_GDI;
}

IBitmap* RenderFactory_GDI::CreateBitmap()
{
    return new Bitmap_GDI;
}

IRender* RenderFactory_GDI::CreateRender(const IRenderDpiPtr& spRenderDpi, void* platformData, RenderBackendType /*backendType*/)
{
    IRender* pRender = new Render_GDI_Windows((HWND)platformData);
    if (pRender != nullptr) {
        pRender->SetRenderDpi(spRenderDpi);
    }
    return pRender;
}

IFontMgr* RenderFactory_GDI::GetFontMgr() const
{
    return m_pFontMgr.get();
}

namespace
{
class RenderBackend_GDI final: public IRenderBackend
{
public:
    virtual RenderType GetRenderType() const override
    {
        return RenderType::kRenderType_GDI;
    }

    virtual const char* GetName() const override
    {
        return "GDI";
    }

    virtual IRenderFactory* CreateRenderFactory() const override
    {
        return new RenderFactory_GDI;
    }
};
}

const IRenderBackend* GetRenderBackend_GDI()
{
    static const RenderBackend_GDI backend;
    return &backend;
}

} // namespace ui

#endif // DUILIB_BUILD_FOR_WIN
