#include "duilib/Image/ImageDecoder_SVG.h"

#include "duilib/Image/Image_Svg.h"
#include "render/IRenderBackend.h"
#include "duilib/Core/GlobalManager.h"
#include "duilib/Image/ImageDecoderFactory.h"
#include "duilib/Utils/FileUtil.h"
#include "duilib/Utils/StringConvert.h"
#include "duilib/Utils/StringUtil.h"

#include <cmath>
#include <cstring>
#include <cwctype>
#include <list>
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
struct SvgReplaceText
{
    DStringA m_srcText;
    DStringA m_destText;
    bool m_bColor = false;
    DString m_colorName;
    UiColor m_colorValue;
};

void CreateSvgReplaceTextList(SvgReplaceColorCallbackFunction svgReplaceColorCallback,
                              const DString& strSvgReplaceColor,
                              std::vector<SvgReplaceText>& svgReplaceTextList)
{
    svgReplaceTextList.clear();
    if (strSvgReplaceColor.empty() || (svgReplaceColorCallback == nullptr)) {
        return;
    }
    DStringA svgReplaceColor = StringConvert::TToUTF8(strSvgReplaceColor);
    std::list<DStringA> colorsList = StringUtil::Split(svgReplaceColor, ";");
    for (DStringA colorPair : colorsList) {
        StringUtil::Trim(colorPair);
        std::list<DStringA> colors = StringUtil::Split(colorPair, "|");
        if (colors.size() != 2) {
            continue;
        }
        DStringA srcColor = *colors.begin();
        DStringA destColor = *colors.rbegin();
        StringUtil::Trim(srcColor);
        StringUtil::Trim(destColor);
        if (srcColor.empty() || destColor.empty()) {
            continue;
        }
        const DString colorName = StringConvert::UTF8ToT(destColor);
        const UiColor colorValue = svgReplaceColorCallback(colorName);
        SvgReplaceText replaceText;
        replaceText.m_srcText = srcColor;
        replaceText.m_destText = destColor;
        replaceText.m_bColor = !colorValue.IsEmpty();
        replaceText.m_colorName = colorName;
        replaceText.m_colorValue = colorValue;
        svgReplaceTextList.emplace_back(std::move(replaceText));
    }
}

DStringA GetReplacedSvgText(const DStringA& svgText,
                            const std::vector<SvgReplaceText>& svgReplaceTextList,
                            bool* pExecReplaced)
{
    DStringA newSvgText = svgText;
    if (pExecReplaced != nullptr) {
        *pExecReplaced = false;
    }
    for (const SvgReplaceText& replaceText : svgReplaceTextList) {
        if (replaceText.m_bColor) {
            const DStringA rgbaColor = StringUtil::Printf("rgba(%d,%d,%d,%.02f)",
                                                          (int32_t)replaceText.m_colorValue.GetR(),
                                                          (int32_t)replaceText.m_colorValue.GetG(),
                                                          (int32_t)replaceText.m_colorValue.GetB(),
                                                          (float)replaceText.m_colorValue.GetA() / 255.0f);
            if (replaceText.m_srcText != rgbaColor) {
                StringUtil::ReplaceAll(replaceText.m_srcText, rgbaColor, newSvgText);
                if (pExecReplaced != nullptr) {
                    *pExecReplaced = true;
                }
            }
        }
        else if (replaceText.m_srcText != replaceText.m_destText) {
            StringUtil::ReplaceAll(replaceText.m_srcText, replaceText.m_destText, newSvgText);
            if (pExecReplaced != nullptr) {
                *pExecReplaced = true;
            }
        }
    }
    return newSvgText;
}

bool CheckReplacedSvgColorChanged(SvgReplaceColorCallbackFunction svgReplaceColorCallback,
                                  std::vector<SvgReplaceText>& svgReplaceTextList)
{
    if (svgReplaceColorCallback == nullptr) {
        return false;
    }
    for (SvgReplaceText& replaceText : svgReplaceTextList) {
        if (!replaceText.m_bColor) {
            continue;
        }
        const UiColor colorValue = svgReplaceColorCallback(replaceText.m_colorName);
        if (!colorValue.IsEmpty() && (colorValue != replaceText.m_colorValue)) {
            replaceText.m_colorValue = colorValue;
            return true;
        }
    }
    return false;
}

NSVGimage* ParseSvgText(const DStringA& svgText)
{
    if (svgText.empty()) {
        return nullptr;
    }
    return nsvgParse(const_cast<char*>(svgText.c_str()), "px", 96.0f);
}

class SvgImage_NanoSvg final: public ISvgImage
{
public:
    SvgImage_NanoSvg(NSVGimage* pSvgImage,
                     float fImageSizeScale,
                     DStringA svgText,
                     std::vector<SvgReplaceText> svgReplaceTextList,
                     SvgReplaceColorCallbackFunction svgReplaceColorCallback):
        m_pSvgImage(pSvgImage),
        m_fImageSizeScale(fImageSizeScale),
        m_svgText(std::move(svgText)),
        m_svgReplaceTextList(std::move(svgReplaceTextList)),
        m_svgReplaceColorCallback(std::move(svgReplaceColorCallback))
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
                                               SvgReplaceColorCallbackFunction svgReplaceColorCallback) override
    {
        if (m_pSvgImage == nullptr) {
            return nullptr;
        }
        SvgReplaceColorCallbackFunction replaceColorCallback = svgReplaceColorCallback;
        if (replaceColorCallback == nullptr) {
            replaceColorCallback = m_svgReplaceColorCallback;
        }
        if (!m_svgReplaceTextList.empty() && CheckReplacedSvgColorChanged(replaceColorCallback, m_svgReplaceTextList)) {
            const DStringA newSvgText = GetReplacedSvgText(m_svgText, m_svgReplaceTextList, nullptr);
            NSVGimage* pNewSvgImage = ParseSvgText(newSvgText);
            if (pNewSvgImage != nullptr) {
                nsvgDelete(m_pSvgImage);
                m_pSvgImage = pNewSvgImage;
                m_nWidth = (uint32_t)std::ceil(m_pSvgImage->width);
                m_nHeight = (uint32_t)std::ceil(m_pSvgImage->height);
                m_pCachedBitmap.reset();
            }
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
    DStringA m_svgText;
    std::vector<SvgReplaceText> m_svgReplaceTextList;
    SvgReplaceColorCallbackFunction m_svgReplaceColorCallback;
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
    const DStringA svgText((const DStringA::value_type*)fileData.data(), fileData.size());
    std::vector<SvgReplaceText> svgReplaceTextList;
    CreateSvgReplaceTextList(decodeParam.m_svgReplaceColorCallback,
                             decodeParam.m_svgReplaceColors,
                             svgReplaceTextList);
    DStringA parsedSvgText = svgText;
    if (!svgReplaceTextList.empty()) {
        parsedSvgText = GetReplacedSvgText(svgText, svgReplaceTextList, nullptr);
    }
    NSVGimage* pSvgImage = ParseSvgText(parsedSvgText);
    if (pSvgImage == nullptr) {
        return nullptr;
    }
    std::shared_ptr<ISvgImage> pSvg(std::make_shared<SvgImage_NanoSvg>(pSvgImage,
                                                                      decodeParam.m_fImageSizeScale,
                                                                      svgText,
                                                                      std::move(svgReplaceTextList),
                                                                      decodeParam.m_svgReplaceColorCallback));
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
