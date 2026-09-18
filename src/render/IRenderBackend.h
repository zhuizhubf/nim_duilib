#ifndef UI_RENDER_IRENDER_BACKEND_H_
#define UI_RENDER_IRENDER_BACKEND_H_

#include "render/IRender.h"

namespace ui
{
class ImageDecoderFactory;

/** 渲染后端描述符
*
*  核心库只依赖该接口，不直接包含任何具体渲染后端的头文件。
*  静态后端库负责实现对应的描述符导出函数。
*/
class DUILIB_API IRenderBackend
{
public:
    virtual ~IRenderBackend() = default;

    /** 获取后端类型
    */
    virtual RenderType GetRenderType() const = 0;

    /** 获取后端名称（用于日志和错误提示）
    */
    virtual const char* GetName() const = 0;

    /** 创建一个新的渲染工厂
    * @return 返回由调用方管理的工厂对象
    */
    virtual IRenderFactory* CreateRenderFactory() const = 0;
};

/** 图片解码模块描述符
*/
class DUILIB_API IImageDecoderModule
{
public:
    virtual ~IImageDecoderModule() = default;

    /** 注册该模块提供的图片解码器
    */
    virtual void RegisterImageDecoders(ImageDecoderFactory& imageDecoderFactory) const = 0;
};

/** Skia 渲染后端描述符（仅当定义了 DUILIB_RENDER_SKIA=1 时由后端库提供）
*/
DUILIB_API const IRenderBackend* GetRenderBackend_Skia();

/** GDI/GDI+ 渲染后端描述符（仅 Windows，且定义了 DUILIB_RENDER_GDI=1 时由后端库提供）
*/
DUILIB_API const IRenderBackend* GetRenderBackend_GDI();

/** nanosvg 图片解码模块描述符
*/
DUILIB_API const IImageDecoderModule* GetImageDecoderModule_SvgNanoSvg();

/** Skia SVG 图片解码模块描述符
*/
DUILIB_API const IImageDecoderModule* GetImageDecoderModule_SvgSkia();

/** Skia Lottie 图片解码模块描述符
*/
DUILIB_API const IImageDecoderModule* GetImageDecoderModule_LottieSkia();

} // namespace ui

#endif // UI_RENDER_IRENDER_BACKEND_H_
