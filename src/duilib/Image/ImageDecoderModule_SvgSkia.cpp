#include "render/IRenderBackend.h"
#include "duilib/Image/ImageDecoderFactory.h"
#include "duilib/Image/ImageDecoder_SVG.h"

namespace ui
{

namespace
{
class ImageDecoderModule_SvgSkia final: public IImageDecoderModule
{
public:
    virtual void RegisterImageDecoders(ImageDecoderFactory& imageDecoderFactory) const override
    {
        imageDecoderFactory.AddImageDecoder(std::make_shared<ImageDecoder_SVG>());
    }
};
}

const IImageDecoderModule* GetImageDecoderModule_SvgSkia()
{
    static const ImageDecoderModule_SvgSkia imageDecoderModule;
    return &imageDecoderModule;
}

} // namespace ui
