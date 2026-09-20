#include "ScintillaDuiTextShaper.h"

#include "render/IRender.h"
#include "text/ITextShaper.h"

#if defined(DUILIB_RENDER_SKIA) && (DUILIB_RENDER_SKIA != 0)
#include "render-skia/Render_Skia.h"
#include "render-skia/SkiaTextShaper.h"
#endif

namespace duilib::ext::scintilla {

ui::ITextShaper *ScintillaDuiTextShaper::Get(
    ui::IRender *pRender, std::unique_ptr<ui::ITextShaper> &pOwnedShaper)
{
    pOwnedShaper.reset();
    if (pRender == nullptr) {
        return nullptr;
    }

    // GDI 后端：Render 自身实现了 ITextShaper 接口
    if (ui::ITextShaper *pShaper = dynamic_cast<ui::ITextShaper *>(pRender)) {
        return pShaper;
    }

#if defined(DUILIB_RENDER_SKIA) && (DUILIB_RENDER_SKIA != 0)
    // Skia 后端：SkiaTextShaper 依赖 Render_Skia 作为绘制目标
    if (ui::Render_Skia *pSkiaRender = dynamic_cast<ui::Render_Skia *>(pRender)) {
        pOwnedShaper = std::make_unique<ui::SkiaTextShaper>(pSkiaRender);
        return pOwnedShaper.get();
    }
#endif

    return nullptr;
}

} // namespace duilib::ext::scintilla
