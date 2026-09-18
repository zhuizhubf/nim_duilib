#include "duilib/Image/ImageDecoder_SVG.h"

#include "duilib/Image/Image_Svg.h"
#include "duilib/Render/IRenderBackend.h"
#include "duilib/Core/GlobalManager.h"
#include "duilib/Image/ImageDecoderFactory.h"
#include "duilib/Utils/FileUtil.h"

#include <cmath>
#include <cstring>
#include <cwctype>
#include <memory>
#include <vector>

#pragma warning (push)
#pragma warning (disable: 4456 4244 4702)
    #define NANOSVG_IMPLEMENTATION
    #define NANOSVG_ALL_COLOR_KEYWORDS
    #include "duilib/third_party/svg/nanosvg.h"
    #define NANOSVGRAST_IMPLEMENTATION
    #include "duilib/third_party/svg/nanosvgrast.h"
#pragma warning (pop)

namespace ui
{

namespace
{
class SvgImage_NanoSvg final: public ISvgImage
{
public:
    SvgImage_NanoSvg(NSVGimage* pSvgImage, float fImageSizeScale):
        m_pSvgImage(pSvgImage),
        m_fImageSizeScale(fImageSizeScale)
    {
        if (m_pSvgImage != nullptr) {
            m_nWidth = (uint32_t)std::ceil(m_pSvgImage->width);
            m_nHeight = (uint32_t)std::ceil(m_pSvgImage->height);
        }
    }

    virtual ~SvgImage_NanoSvg() override
    {
        if (m_pSvgImage != nullptr) {
            nsvgDelete(m_pSvgImage);
            m_pSvgImage = nullptr;
        }
    }

    virtual uint32_t GetWidth() const override
    {
        return m_nWidth;
    }

    virtual uint32_t GetHeight() const override
    {
        return m_nHeight;
    }

    virtual float GetImageSizeScale() const override
    {
        return m_fImageSizeScale;
    }

    virtual std::shared_ptr<IBitmap> GetBitmap(const UiSize& szImageSize,
                                               SvgReplaceColorCallbackFunction /*svgReplaceColorCallback*/) override
    {
        if (m_pSvgImage == nullptr) {
            return nullptr;
        }
        const uint32_t nWidth = (szImageSize.cx > 0) ? (uint32_t)szImageSize.cx : m_nWidth;
        const uint32_t nHeight = (szImageSize.cy > 0) ? (uint32_t)szImageSize.cy : m_nHeight;
        if ((nWidth == 0) || (nHeight == 0)) {
            return nullptr;
        }
        if ((m_pCachedBitmap != nullptr) &&
            (m_pCachedBitmap->GetWidth() == nWidth) &&
            (m_pCachedBitmap->GetHeight() == nHeight)) {
            return m_pCachedBitmap;
        }
        IRenderFactory* pRenderFactory = GlobalManager::Instance().GetRenderFactory();
        if (pRenderFactory == nullptr) {
            return nullptr;
        }
        std::shared_ptr<IBitmap> pBitmap(pRenderFactory->CreateBitmap());
        if (pBitmap == nullptr) {
            return nullptr;
        }
        std::vector<uint8_t> pixels((size_t)nWidth * nHeight * 4);
        const float fScaleX = (float)nWidth / m_pSvgImage->width;
        const float fScaleY = (float)nHeight / m_pSvgImage->height;
        NSVGrasterizer* pRasterizer = nsvgCreateRasterizer();
        if (pRasterizer == nullptr) {
            return nullptr;
        }
        nsvgRasterize(pRasterizer, m_pSvgImage, 0, 0, fScaleX, pixels.data(),
                      (int)nWidth, (int)nHeight, (int)(nWidth * 4));
        nsvgDeleteRasterizer(pRasterizer);
        if (!pBitmap->Init(nWidth, nHeight, pixels.data(), 1.0f, BitmapAlphaType::kUnpremul_SkAlphaType)) {
            return nullptr;
        }
        m_pCachedBitmap = pBitmap;
        return pBitmap;
    }

private:
    NSVGimage* m_pSvgImage = nullptr;
    uint32_t m_nWidth = 0;
    uint32_t m_nHeight = 0;
    float m_fImageSizeScale = 1.0f;
    std::shared_ptr<IBitmap> m_pCachedBitmap;
};
}

ImageDecoder_SVG::ImageDecoder_SVG() = default;
ImageDecoder_SVG::~ImageDecoder_SVG() = default;

DString ImageDecoder_SVG::GetFormatName() const
{
    return _T("SVG");
}

bool ImageDecoder_SVG::CanDecode(const DString& imageFilePath) const
{
    if (imageFilePath.size() < 4) {
        return false;
    }
    DString ext = imageFilePath.substr(imageFilePath.size() - 4);
    for (wchar_t& ch : ext) {
        ch = (wchar_t)::towlower(ch);
    }
    return ext == _T(".svg");
}

bool ImageDecoder_SVG::CanDecode(const uint8_t* data, size_t dataLen) const
{
    if ((data == nullptr) || (dataLen < 4)) {
        return false;
    }
    const char* pData = (const char*)data;
    if ((dataLen >= 5) && (std::strncmp(pData, "<?xml", 5) == 0)) {
        return true;
    }
    return (std::strncmp(pData, "<svg", 4) == 0);
}

std::unique_ptr<IImage> ImageDecoder_SVG::LoadImageData(const ImageDecodeParam& decodeParam)
{
    std::vector<uint8_t> fileData;
    if ((decodeParam.m_pFileData != nullptr) && !decodeParam.m_pFileData->empty()) {
        fileData = *decodeParam.m_pFileData;
    }
    else if (!decodeParam.m_imageFilePath.IsEmpty()) {
        FileUtil::ReadFileData(decodeParam.m_imageFilePath, fileData);
    }
    if (fileData.empty()) {
        return nullptr;
    }
    if (fileData.back() != '\0') {
        fileData.push_back('\0');
    }
    NSVGimage* pSvgImage = nsvgParse((char*)fileData.data(), "px", 96.0f);
    if (pSvgImage == nullptr) {
        return nullptr;
    }
    std::shared_ptr<ISvgImage> pSvg(std::make_shared<SvgImage_NanoSvg>(pSvgImage, decodeParam.m_fImageSizeScale));
    return Image_Svg::MakeImage(pSvg);
}

namespace
{
class ImageDecoderModule_SvgNanoSvg final: public IImageDecoderModule
{
public:
    virtual void RegisterImageDecoders(ImageDecoderFactory& imageDecoderFactory) const override
    {
        imageDecoderFactory.AddImageDecoder(std::make_shared<ImageDecoder_SVG>());
    }
};
}

const IImageDecoderModule* GetImageDecoderModule_SvgNanoSvg()
{
    static const ImageDecoderModule_SvgNanoSvg imageDecoderModule;
    return &imageDecoderModule;
}

} // namespace ui
