#ifndef DUILIB_CORE_COLORCONVERTER_H_
#define DUILIB_CORE_COLORCONVERTER_H_

#include "duilib/Core/UiTypes.h"

namespace ui {

/** @class ColorConverter
 *  @brief 颜色空间转换工具类
 *  @details 提供OKLCH、RGB、Hex等颜色格式之间的相互转换功能
 */
class DUILIB_API ColorConverter
{
public:
    /** @brief 默认构造函数 */
    ColorConverter();

    /** @brief 析构函数 */
    ~ColorConverter();

    // ==================== Hex <-> RGB ====================

    /** @brief 解析Hex颜色字符串
     *  @param colorStr Hex颜色字符串，格式如"#FFAABBCC"
     *  @param alpha 输出alpha值(0-255)
     *  @param r 输出红色值(0-255)
     *  @param g 输出绿色值(0-255)
     *  @param b 输出蓝色值(0-255)
     *  @return 解析成功返回true，否则返回false
     */
    bool ParseHexColor(const std::string& colorStr, uint8_t& alpha, uint8_t& r, uint8_t& g, uint8_t& b);

    /** @brief 将RGB转换为Hex字符串
     *  @param alpha Alpha值(0-255)
     *  @param r 红色值(0-255)
     *  @param g 绿色值(0-255)
     *  @param b 蓝色值(0-255)
     *  @return Hex颜色字符串，如"#FFAABBCC"
     */
    std::string RGBToHex(uint8_t alpha, uint8_t r, uint8_t g, uint8_t b);

    // ==================== 相对亮度 ====================

    /** @brief 计算RGB颜色的相对亮度
     *  @param r 红色值(0-255)
     *  @param g 绿色值(0-255)
     *  @param b 蓝色值(0-255)
     *  @return 相对亮度值(0-1)
     */
    double GetRelativeLuminance(uint8_t r, uint8_t g, uint8_t b);

    /** @brief 计算两个颜色的对比度
     *  @param color1 颜色1的Hex字符串
     *  @param color2 颜色2的Hex字符串
     *  @return 对比度值，失败返回0.0
     */
    double CalculateContrastRatio(const std::string& color1, const std::string& color2);

    // ==================== RGB <-> OKLCH ====================

    /** @brief 将RGB转换为OKLCH
     *  @param r 红色值(0-255)
     *  @param g 绿色值(0-255)
     *  @param b 蓝色值(0-255)
     *  @param L 输出明度值(0-1)
     *  @param C 输出色度值(0~)
     *  @param H 输出色相值(0-360)
     *  @return 转换成功返回true，否则返回false
     */
    bool RGBToOKLCH(uint8_t r, uint8_t g, uint8_t b, double& L, double& C, double& H);

    /** @brief 将RGB(双精度)转换为OKLCH
     *  @param red 红色值(0.0-1.0)
     *  @param green 绿色值(0.0-1.0)
     *  @param blue 蓝色值(0.0-1.0)
     *  @param L 输出明度值(0-1)
     *  @param C 输出色度值(0~)
     *  @param H 输出色相值(0-360)
     *  @return 成功返回0，失败返回-1
     */
    int RGBToOKLCH(double red, double green, double blue, double* L, double* C, double* H);

    /** @brief 将OKLCH转换为ARGB十六进制字符串
     *  @param L 明度值(0-1)
     *  @param C 色度值
     *  @param H 色相值(0-360)
     *  @param alpha Alpha值(0-255)
     *  @return ARGB十六进制字符串，如"#FFAABBCC"
     */
    std::string OKLCHToARGB(double L, double C, double H, uint8_t alpha);

    /** @brief 将OKLCH转换为RGB
     *  @param L 明度值(0-1)
     *  @param C 色度值
     *  @param H 色相值(0-360)
     *  @param red 输出红色值(0.0-1.0)
     *  @param green 输出绿色值(0.0-1.0)
     *  @param blue 输出蓝色值(0.0-1.0)
     *  @return 成功返回0，失败返回-1
     */
    int OKLCHToRGB(double L, double C, double H, double* red, double* green, double* blue);

    /** @brief 将OKLCH转换为RGB(字节)
     *  @param L 明度值(0-1)
     *  @param C 色度值
     *  @param H 色相值(0-360)
     *  @param red 输出红色值(0-255)
     *  @param green 输出绿色值(0-255)
     *  @param blue 输出蓝色值(0-255)
     *  @return 成功返回0，失败返回-1
     */
    int OKLCHToRGB(double L, double C, double H, uint8_t& red, uint8_t& green, uint8_t& blue);

private:
    /** @brief sRGB传输函数(Gamma校正)
     *  @param x 输入值(0-1)
     *  @return 校正后的值(0-1)
     */
    double sRGBTransferFunction(double x);

    /** @brief 逆向sRGB传输函数
     *  @param x 输入值(0-1)
     *  @return 逆向校正后的值(0-1)
     */
    double InverseSRGBTransferFunction(double x);

    /** @brief 立方根函数
     *  @param x 输入值
     *  @return 立方根
     */
    double Cbrt(double x);

private:
    static constexpr double PI = 3.14159265358979323846;  ///< 圆周率常量
};

}  // namespace ui

#endif  // DUILIB_CORE_COLORCONVERTER_H_
