#include "render/IRenderBackend.h"
#include "render-skia/RenderFactory_Skia.h"

namespace ui
{

namespace
{
class RenderBackend_Skia final: public IRenderBackend
{
public:
    virtual RenderType GetRenderType() const override
    {
        return RenderType::kRenderType_Skia;
    }

    virtual const char* GetName() const override
    {
        return "Skia";
    }

    virtual IRenderFactory* CreateRenderFactory() const override
    {
        return new RenderFactory_Skia;
    }
};
}

const IRenderBackend* GetRenderBackend_Skia()
{
    static const RenderBackend_Skia backend;
    return &backend;
}

} // namespace ui
