#include "render/IRenderBackend.h"
#include "duilib/Image/ImageDecoderFactory.h"
#include "duilib/Image/ImageDecoder_LOTTIE.h"

namespace ui
{

namespace
{
class ImageDecoderModule_LottieSkia final: public IImageDecoderModule
{
public:
    virtual void RegisterImageDecoders(ImageDecoderFactory& imageDecoderFactory) const override
    {
        imageDecoderFactory.AddImageDecoder(std::make_shared<ImageDecoder_LOTTIE>());
    }
};
}

const IImageDecoderModule* GetImageDecoderModule_LottieSkia()
{
    static const ImageDecoderModule_LottieSkia imageDecoderModule;
    return &imageDecoderModule;
}

} // namespace ui
