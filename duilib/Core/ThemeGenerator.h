#ifndef UI_CORE_THEME_GENERATOR_H_
#define UI_CORE_THEME_GENERATOR_H_

#include "duilib/Core/UiTypes.h"
#include <string>
#include <map>
#include <vector>

namespace ui
{

/** @struct ThemeColorConfig
 *  @brief 主题颜色配置结构体
 *  @details 存储单个颜色项的所有配置信息，包括名称、值、类型、分类等属性
 */
struct ThemeColorConfig
{
    std::string name;            ///< 颜色名称，如"bg_window_main"
    std::string value;           ///< 颜色值，格式为"#AARRGGBB"
    std::string type;            ///< 颜色类型，如"bg_color"、"border_color"等
    std::string category;        ///< 颜色分类，如"bg_color"、"text_color"等
    std::string role;           ///< 颜色角色描述
    std::string derived_from;    ///< 派生来源，指定基于哪个颜色派生
    std::string adjust;         ///< 调整参数，如"lightness:-10,saturation:20"
    bool fixed;                 ///< 是否为固定颜色，固定颜色不会被主题生成器修改
    bool support_accent;        ///< 是否支持强调色
    std::string contrast_bg;    ///< 对比度检查的背景色名称
    std::string comment_cn;     ///< 中文注释
    std::string comment_en;     ///< 英文注释
    std::string original_xml;   ///< 原始XML片段（保留原始格式）
    int node_order;             ///< 节点顺序索引（用于保持输出顺序与输入一致）
};

/** @struct ThemeMetaInfo
 *  @brief 主题元信息结构体
 *  @details 存储主题的基本元数据信息
 */
struct ThemeMetaInfo
{
    std::map<std::string, std::string> properties;  ///< 元数据属性键值对
};

/** @class ThemeGenerator
 *  @brief 主题颜色生成器
 *  @details 基于OKLCH色彩空间的主题颜色生成器，支持：
 *          - 从输入XML动态读取颜色配置
 *          - 生成符合WCAG 2.1对比度标准的主题颜色
 *          - 支持Light/Dark两种模式
 *          - 自动处理颜色状态变体（hovered、pressed、selected、disabled）
 *          - 保持输出XML与输入XML的顺序和注释一致
 */
class DUILIB_API ThemeGenerator
{
public:
    /** @brief 构造函数 */
    ThemeGenerator();

    /** @brief 析构函数 */
    ~ThemeGenerator();

    /** @brief 从XML文件加载颜色配置
     *  @param inputXml 输入XML文件路径
     *  @return 加载成功返回true，否则返回false
     *  @details 解析XML文件，提取所有ThemeColor节点配置，并保存原始XML内容
     */
    bool LoadConfigFromXml(const std::string& inputXml);

    /** @brief 生成主题颜色
     *  @param hue 色调值(0-360)
     *  @param base 基础亮度值(0-1)
     *  @param isDark 是否为暗色主题
     *  @return 生成的主题XML字符串
     *  @details 根据指定的色相和亮度生成完整的主题颜色配置，
     *          只修改颜色值，保留原始XML的格式、顺序和注释
     */
    std::string GenerateTheme(double hue, double base, bool isDark);

    /** @brief 获取生成的指定颜色
     *  @param colorName 颜色名称
     *  @return 颜色的ARGB字符串，如"#FF123456"
     */
    std::string GetGeneratedColor(const std::string& colorName) const;

    /** @brief 获取已加载的颜色配置
     *  @return 颜色配置映射表
     */
    std::map<std::string, ThemeColorConfig> GetLoadedConfigs() const { return m_loadedConfigs; }

private:
    /** @brief 解析十六进制颜色字符串
     *  @param colorStr 颜色字符串，格式为"#AARRGGBB"
     *  @param alpha 输出：透明度分量(0-255)
     *  @param r 输出：红色分量(0-255)
     *  @param g 输出：绿色分量(0-255)
     *  @param b 输出：蓝色分量(0-255)
     *  @return 解析成功返回true，否则返回false
     */
    bool ParseHexColor(const std::string& colorStr, uint8_t& alpha, uint8_t& r, uint8_t& g, uint8_t& b);

    /** @brief 将RGBA分量转换为十六进制颜色字符串
     *  @param alpha 透明度(0-255)
     *  @param r 红色分量(0-255)
     *  @param g 绿色分量(0-255)
     *  @param b 蓝色分量(0-255)
     *  @return 格式化的十六进制颜色字符串"#AARRGGBB"
     */
    std::string RGBToHex(uint8_t alpha, uint8_t r, uint8_t g, uint8_t b);

    /** @brief 计算RGB颜色的相对亮度
     *  @param r 红色分量(0-255)
     *  @param g 绿色分量(0-255)
     *  @param b 蓝色分量(0-255)
     *  @return 相对亮度值(0-1)
     *  @details 使用ITU-R BT.709系数计算sRGB空间的相对亮度
     */
    double GetRelativeLuminance(uint8_t r, uint8_t g, uint8_t b);

    /** @brief 计算两个颜色之间的对比度比率
     *  @param color1 第一个颜色（ARGB格式）
     *  @param color2 第二个颜色（ARGB格式）
     *  @return 对比度比率值
     *  @details 根据WCAG 2.1公式计算：(L1 + 0.05) / (L2 + 0.05)
     */
    double CalculateContrastRatio(const std::string& color1, const std::string& color2);

    /** @brief 将RGB颜色转换为OKLCH色彩空间
     *  @param r 红色分量(0-255)
     *  @param g 绿色分量(0-255)
     *  @param b 蓝色分量(0-255)
     *  @param L 输出：亮度分量(0-1)
     *  @param C 输出：色度分量(0-1)
     *  @param H 输出：色相分量(0-360)
     *  @return 转换成功返回true，否则返回false
     */
    bool RGBToOKLCH(uint8_t r, uint8_t g, uint8_t b, double& L, double& C, double& H);

    /** @brief 将OKLCH颜色转换为ARGB格式
     *  @param L 亮度分量(0-1)
     *  @param C 色度分量(0-1)
     *  @param H 色相分量(0-360)
     *  @param alpha 透明度(0-255)，默认为255
     *  @return ARGB格式的颜色字符串
     */
    std::string OKLCHToARGB(double L, double C, double H, uint8_t alpha = 255);

    /** @brief 根据色相获取基础背景颜色
     *  @param hue 色调值(0-360)
     *  @param base 基础亮度值(0-1)
     *  @param isDark 是否为暗色主题
     *  @return 基础背景颜色的ARGB字符串
     */
    std::string GetBaseColorFromHue(double hue, double base, bool isDark);

    /** @brief 获取前景色（与背景色互补）
     *  @param hue 色调值(0-360)
     *  @param base 基础亮度值(0-1)
     *  @param isDark 是否为暗色主题
     *  @return 前景色的ARGB字符串
     */
    std::string GetForegroundColor(double hue, double base, bool isDark);

    /** @brief 获取表面层颜色
     *  @param hue 色调值(0-360)
     *  @param base 基础亮度值(0-1)
     *  @param isDark 是否为暗色主题
     *  @param surfaceLevel 表面层级(0-3)
     *  @return 表面层颜色的ARGB字符串
     *  @details 根据层级递推颜色明暗，浅色主题逐层变暗，深色主题逐层变亮
     */
    std::string GetSurfaceColor(double hue, double base, bool isDark, int surfaceLevel);

    /** @brief 获取中性色
     *  @param hue 色调值(0-360)
     *  @param base 基础亮度值(0-1)
     *  @param isDark 是否为暗色主题
     *  @param level 层级(0-1500)
     *  @return 中性色的ARGB字符串
     */
    std::string GetNeutralColor(double hue, double base, bool isDark, int level);

    /** @brief 获取状态变体颜色
     *  @param baseColor 基础颜色
     *  @param state 状态类型："hovered"、"pressed"、"selected"、"disabled"
     *  @param isDark 是否为暗色主题
     *  @return 状态变体颜色的ARGB字符串
     *  @details 根据状态调整亮度和饱和度：
     *          - hovered: 浅色变暗(+0.06L)，深色变亮(-0.06L)
     *          - pressed: 浅色变暗(+0.10L)，深色变亮(-0.10L)
     *          - selected: 浅色变暗(+0.09L)，深色变亮(-0.09L)
     *          - disabled: 降低饱和度并设置半透明
     */
    std::string GetStateColor(const std::string& baseColor, const std::string& state, bool isDark);

    /** @brief 应用颜色调整参数
     *  @param baseColor 基础颜色
     *  @param adjustStr 调整参数字符串，格式："lightness:10,saturation:-5,hue:30"
     *  @return 调整后的颜色
     *  @details 支持的调整类型：
     *          - lightness: 亮度调整（百分比）
     *          - saturation: 饱和度调整（百分比）
     *          - hue: 色相调整（度数）
     *          - alpha: 透明度调整（绝对值或相对值）
     */
    std::string ApplyAdjustments(const std::string& baseColor, const std::string& adjustStr);

    /** @brief 检测颜色名称中的状态标识
     *  @param colorName 颜色名称
     *  @return pair<状态, 基础名称>
     *  @details 从颜色名称中提取状态后缀，如"bg_btn_hovered"返回("hovered", "bg_btn")
     */
    std::pair<std::string, std::string> DetectColorState(const std::string& colorName);

    /** @brief 生成主题核心颜色集
     *  @param hue 色调值(0-360)
     *  @param base 基础亮度值(0-1)
     *  @param isDark 是否为暗色主题
     *  @details 生成所有核心主题颜色，包括：
     *          - 背景色、前景色、表面层颜色
     *          - 强调色及其前景色
     *          - 链接颜色及其状态变体
     *          - 中性色渐变
     */
    void GenerateThemeColors(double hue, double base, bool isDark);

    /** @brief 确保文本颜色与背景色的对比度达标
     *  @param textColor 文本颜色
     *  @param bgColor 背景颜色
     *  @param minContrast 最低对比度要求，默认4.5（WCAG AA标准）
     *  @return 调整后的文本颜色
     *  @details 如果对比度不足，通过迭代调整亮度分量直到达到目标对比度
     */
    std::string EnsureContrast(const std::string& textColor, const std::string& bgColor, double minContrast = 4.5);

    /** @brief 写入输出XML文件（保留中）
     *  @param outputXml 输出文件路径
     *  @return 写入成功返回true
     */
    bool WriteOutputXml(const std::string& outputXml);

    /** @brief OKLCH到RGB转换（双精度版本）
     *  @param L 亮度分量(0-1)
     *  @param C 色度分量(0-1)
     *  @param H 色相分量(0-360)
     *  @param red 输出：红色分量(0-1)
     *  @param green 输出：绿色分量(0-1)
     *  @param blue 输出：蓝色分量(0-1)
     *  @return 转换成功返回0
     */
    static int OKLCH2RGB(double L, double C, double H, double* red, double* green, double* blue);

    /** @brief OKLCH到RGB转换（字节版本）
     *  @param L 亮度分量(0-1)
     *  @param C 色度分量(0-1)
     *  @param H 色相分量(0-360)
     *  @param red 输出：红色分量(0-255)
     *  @param green 输出：绿色分量(0-255)
     *  @param blue 输出：蓝色分量(0-255)
     *  @param alpha 透明度(0-255)
     *  @return 转换成功返回0
     */
    static int OKLCH2RGB(double L, double C, double H, uint8_t& red, uint8_t& green, uint8_t& blue, uint8_t alpha = 255);

    /** @brief RGB到OKLCH转换
     *  @param red 红色分量(0-1)
     *  @param green 绿色分量(0-1)
     *  @param blue 蓝色分量(0-1)
     *  @param L 输出：亮度分量(0-1)
     *  @param C 输出：色度分量(0-1)
     *  @param H 输出：色相分量(0-360)
     *  @return 转换成功返回0
     */
    static int RGB2OKLCH(double red, double green, double blue, double* L, double* C, double* H);

private:
    double m_hue;                                            ///< 当前色调值
    double m_base;                                           ///< 当前基础亮度值
    bool m_isDark;                                           ///< 当前是否为暗色主题
    std::map<std::string, std::string> m_generatedColors;   ///< 生成的核心颜色集
    std::map<std::string, ThemeColorConfig> m_loadedConfigs; ///< 从XML加载的颜色配置
    std::vector<std::string> m_colorOrder;                  ///< 颜色节点原始顺序
    std::string m_originalXmlContent;                       ///< 原始XML完整内容
    ThemeMetaInfo m_themeMeta;                              ///< 主题元信息
    std::map<std::string, std::string> m_originalValues;    ///< 原始颜色值（用于对比）
};

}

#endif //UI_CORE_THEME_GENERATOR_H_
