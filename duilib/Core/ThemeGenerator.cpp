#include "ThemeGenerator.h"
#include "duilib/Utils/StringConvert.h"

#include "duilib/third_party/xml/pugixml.hpp"

#include <sstream>
#include <iomanip>
#include <fstream>

namespace ui
{

ThemeGenerator::ThemeGenerator()
    : m_hue(143.24)
    , m_base(0.0143)
    , m_isDark(false)
    // 背景色参数
    , m_bgLightL(0.97)
    , m_bgLightLScale(-0.95)
    , m_bgDarkL(0.17)
    , m_bgDarkLScale(0.78)
    , m_bgBaseChroma(1.0)
    // 前景色参数
    , m_fgLightL(0.22)
    , m_fgLightLScale(0.72)
    , m_fgDarkL(0.92)
    , m_fgDarkLScale(-0.75)
    , m_fgBaseChroma(0.15)
    // Surface层参数
    , m_surfaceLightOffset(0.025)
    , m_surfaceDarkOffset(0.07)
    , m_surfaceBaseChroma(1.5)
    // Accent参数
    , m_accentLightL(0.6204)
    , m_accentDarkL(0.68)
    , m_accentC(0.195)
{
}

ThemeGenerator::~ThemeGenerator()
{
}

void ThemeGenerator::SetBgParams(double bgLightL, double bgDarkL, double bgBaseChroma)
{
    m_bgLightL = bgLightL;
    m_bgDarkL = bgDarkL;
    m_bgBaseChroma = bgBaseChroma;
}

void ThemeGenerator::SetFgParams(double fgLightL, double fgDarkL, double fgBaseChroma)
{
    m_fgLightL = fgLightL;
    m_fgDarkL = fgDarkL;
    m_fgBaseChroma = fgBaseChroma;
}

void ThemeGenerator::SetSurfaceParams(double surfaceLightOffset, double surfaceDarkOffset, double surfaceBaseChroma)
{
    m_surfaceLightOffset = surfaceLightOffset;
    m_surfaceDarkOffset = surfaceDarkOffset;
    m_surfaceBaseChroma = surfaceBaseChroma;
}

void ThemeGenerator::SetAccentParams(double accentLightL, double accentDarkL, double accentC)
{
    m_accentLightL = accentLightL;
    m_accentDarkL = accentDarkL;
    m_accentC = accentC;
}

void ThemeGenerator::ResetParams()
{
    // 重置所有参数为默认值
    SetBgParams(0.97, 0.17, 1.5);
    SetFgParams(0.22, 0.92, 0.15);
    SetSurfaceParams(0.025, 0.07, 1.5);
    SetAccentParams(0.6204, 0.68, 0.195);
}

std::string ThemeGenerator::GetBackgroundColor(double hue, double base, bool isDark)
{
    // 使用成员变量计算背景色
    double baseChroma = base * m_bgBaseChroma;

    double bgL;
    if (isDark) {
        bgL = m_bgDarkL + base * m_bgDarkLScale;
    }
    else {
        bgL = m_bgLightL + base * m_bgLightLScale;
    }

    return ColorConverter::OKLCHToARGB(bgL, baseChroma, hue, 255);
}

std::string ThemeGenerator::GetForegroundColor(double hue, double base, bool isDark)
{
    // 使用成员变量计算前景色
    double fgChroma = base * m_fgBaseChroma;

    double fgL;
    if (isDark) {
        fgL = m_fgDarkL + base * m_fgDarkLScale;
    }
    else {
        fgL = m_fgLightL + base * m_fgLightLScale;
    }

    return ColorConverter::OKLCHToARGB(fgL, fgChroma, hue, 255);
}

std::string ThemeGenerator::GetSurfaceColor(double hue, double base, bool isDark, int surfaceLevel)
{
    // 使用成员变量计算Surface颜色
    double surfaceChroma = base * m_surfaceBaseChroma;

    double bgL;
    if (isDark) {
        bgL = m_bgDarkL + base * m_bgDarkLScale;
    }
    else {
        bgL = m_bgLightL + base * m_bgLightLScale;
    }

    // 浅色模式: offset为负，表面色随层级增加而变暗
    // 深色模式: offset为正，表面色随层级增加而变亮
    double offset = isDark ? m_surfaceDarkOffset : -m_surfaceLightOffset;
    double sfL = bgL + offset * surfaceLevel;

    return ColorConverter::OKLCHToARGB(sfL, surfaceChroma, hue, 255);
}

std::string ThemeGenerator::GetStateColor(const std::string& baseColor, const std::string& state, bool isDark) const
{
    if (baseColor.empty()) {
        return baseColor;
    }

    uint8_t alpha, r, g, b;
    if (!ColorConverter::ParseHexColor(baseColor, alpha, r, g, b)) {
        return baseColor;
    }

    double L, C, H;
    if (!ColorConverter::RGBToOKLCH(r, g, b, L, C, H)) {
        return baseColor;
    }

    if (state == "hovered") {
        if (!isDark) {
            L = std::max(0.0, std::min(1.0, L - 0.06));
        }
        else {
            L = std::max(0.0, std::min(1.0, L + 0.06));
        }
    }
    else if (state == "pressed") {
        if (!isDark) {
            L = std::max(0.0, std::min(1.0, L - 0.10));
        }
        else {
            L = std::max(0.0, std::min(1.0, L + 0.10));
        }
    }
    else if (state == "selected") {
        if (!isDark) {
            L = std::max(0.0, std::min(1.0, L - 0.09));
        }
        else {
            L = std::max(0.0, std::min(1.0, L + 0.09));
        }
    }
    else if ((state == "disabled") || (state == "prompt")) {
        if (!isDark) {
            L = std::max(0.0, std::min(1.0, L + 0.08));
            C = std::max(0.0, C - 0.4);
            alpha = 153;
        }
        else {
            L = std::max(0.0, std::min(1.0, L - 0.05));
            C = std::max(0.0, C - 0.2);
            alpha = 153;
        }
    }
    else if (state == "focused") {
        //强调色
        return GetGeneratedColor("--accent");
    }
    else {
        return baseColor;
    }

    return ColorConverter::OKLCHToARGB(L, C, H, alpha);
}

std::string ThemeGenerator::ApplyAdjustments(const std::string& baseColor, const std::string& adjustStr) const
{
    if (baseColor.empty() || adjustStr.empty()) {
        return baseColor;
    }

    uint8_t alpha, r, g, b;
    if (!ColorConverter::ParseHexColor(baseColor, alpha, r, g, b)) {
        return baseColor;
    }

    double L, C, H;
    if (!ColorConverter::RGBToOKLCH(r, g, b, L, C, H)) {
        return baseColor;
    }

    std::istringstream adjustStream(adjustStr);
    std::string adjustment;
    while (std::getline(adjustStream, adjustment, ',')) {
        std::string type;
        std::string valueStr;

        size_t colonPos = adjustment.find(':');
        if (colonPos != std::string::npos) {
            type = adjustment.substr(0, colonPos);
            valueStr = adjustment.substr(colonPos + 1);
        }
        else {
            type = adjustment;
            valueStr = "";
        }

        type.erase(0, type.find_first_not_of(" \t"));
        type.erase(type.find_last_not_of(" \t") + 1);
        valueStr.erase(0, valueStr.find_first_not_of(" \t"));
        valueStr.erase(valueStr.find_last_not_of(" \t") + 1);

        if (type == "invert") {
            L = 1.0 - L;
        }
        else if (type == "lightness") {
            double delta = std::stod(valueStr);
            L = std::max(0.0, std::min(1.0, L + delta / 100.0));
        }
        else if (type == "saturation") {
            double delta = std::stod(valueStr);
            C = std::max(0.0, C + delta / 100.0);
        }
        else if (type == "hue") {
            double delta = std::stod(valueStr);
            H = fmod(H + delta + 360.0, 360.0);
        }
        else if (type == "alpha") {
            if (valueStr[0] == '=') {
                alpha = static_cast<uint8_t>(std::stoul(valueStr.substr(1), nullptr, 10));
            }
            else {
                double delta = std::stod(valueStr);
                alpha = static_cast<uint8_t>(std::max(0, std::min(255, static_cast<int>(alpha + delta * 2.55))));
            }
        }
    }

    return ColorConverter::OKLCHToARGB(L, C, H, alpha);
}

std::pair<std::string, std::string> ThemeGenerator::DetectColorState(const std::string& colorName) const
{
    std::vector<std::string> states = {"_normal", "_disabled", "_pressed", "_hovered", "_selected", "_prompt" , "_focused" };
    for (const auto& state : states) {
        size_t pos = colorName.rfind(state);
        if (pos != std::string::npos) {
            std::string baseName = colorName.substr(0, pos);
            std::string actualState = state.substr(1);
            return std::make_pair(actualState, baseName);
        }
    }
    return std::make_pair("", colorName);
}

std::string ThemeGenerator::EnsureContrast(const std::string& textColor, const std::string& bgColor, double minContrast) const
{
    uint8_t alpha, r, g, b;
    if (!ColorConverter::ParseHexColor(textColor, alpha, r, g, b)) {
        return textColor;
    }

    double L, C, H;
    if (!ColorConverter::RGBToOKLCH(r, g, b, L, C, H)) {
        return textColor;
    }

    uint8_t bgAlpha, bgR, bgG, bgB;
    if (!ColorConverter::ParseHexColor(bgColor, bgAlpha, bgR, bgG, bgB)) {
        return textColor;
    }

    double bgLuminance = ColorConverter::GetRelativeLuminance(bgR, bgG, bgB);

    double contrast = ColorConverter::CalculateContrastRatio(textColor, bgColor);
    if (contrast < 0.01) {
        return textColor;
    }

    if (contrast >= minContrast) {
        return textColor;
    }

    double LStep = (bgLuminance > 0.5) ? 0.05 : -0.05;
    double LTarget = (bgLuminance > 0.5) ? 0.05 : 0.95;

    const int maxIterations = 50;
    int iteration = 0;

    while (iteration < maxIterations) {
        if ((bgLuminance > 0.5 && L <= LTarget) || (bgLuminance <= 0.5 && L >= LTarget)) {
            break;
        }

        L = std::max(LTarget, std::min((bgLuminance > 0.5) ? 0.95 : 0.05, L + LStep));

        std::string testColor = ColorConverter::OKLCHToARGB(L, C, H, alpha);
        contrast = ColorConverter::CalculateContrastRatio(testColor, bgColor);

        if (contrast >= minContrast) {
            return testColor;
        }

        iteration++;
    }

    std::string finalColor = ColorConverter::OKLCHToARGB(L, C, H, alpha);
    return finalColor;
}

void ThemeGenerator::GenerateThemeColors(double hue, double base, bool isDark)
{
    m_hue = hue;
    m_base = base;
    m_isDark = isDark;
    m_generatedColors.clear();

    // =========================================================================
    // 核心颜色计算
    // =========================================================================

    // 背景色和前景色
    std::string backgroundColor = GetBackgroundColor(hue, base, isDark);
    std::string foregroundColor = GetForegroundColor(hue, base, isDark);

    // Surface层（用于卡片、按钮等元素的背景）
    std::string surface0 = GetSurfaceColor(hue, base, isDark, -1);
    std::string surface1 = GetSurfaceColor(hue, base, isDark, 1);
    std::string surface2 = GetSurfaceColor(hue, base, isDark, 3);
    std::string surface3 = GetSurfaceColor(hue, base, isDark, 5);
    std::string surface4 = GetSurfaceColor(hue, base, isDark, 7);

    // Accent颜色（强调色）
    double accentL = isDark ? m_accentDarkL : m_accentLightL;
    std::string accentColor = ColorConverter::OKLCHToARGB(accentL, m_accentC, hue, 255);

    // Accent前景色：浅色模式接近白色，深色模式接近黑色
    double accentFgL = isDark ? 0.14 : 0.99;
    std::string accentForeground = ColorConverter::OKLCHToARGB(accentFgL, 0, hue, 255);

    // =========================================================================
    // 写入颜色到 m_generatedColors
    // =========================================================================

    // CSS变量（--开头）
    m_generatedColors["--background"] = backgroundColor;
    m_generatedColors["--foreground"] = foregroundColor;
    m_generatedColors["--surface_0"] = surface0;
    m_generatedColors["--surface_1"] = surface1;
    m_generatedColors["--surface_2"] = surface2;
    m_generatedColors["--surface_3"] = surface3;
    m_generatedColors["--surface_4"] = surface4;
    m_generatedColors["--accent"] = accentColor;
    m_generatedColors["--accent_foreground"] = accentForeground;
    m_generatedColors["--success"] = ColorConverter::OKLCHToARGB(0.65, 0.16, 152, 255);  // 成功色
    m_generatedColors["--warning"] = ColorConverter::OKLCHToARGB(0.68, 0.18, 80, 255);   // 警告色
    m_generatedColors["--error"] = ColorConverter::OKLCHToARGB(0.65, 0.18, 25, 255);     // 失败/错误/危险色

    // 窗口和基础文本颜色
    m_generatedColors["bg_window_main"] = backgroundColor;
    m_generatedColors["text_default"] = foregroundColor;
    m_generatedColors["text_muted"] = GetStateColor(foregroundColor, "disabled", isDark);
    m_generatedColors["text_btn_normal"] = foregroundColor;
    m_generatedColors["text_btn_disabled"] = GetStateColor(foregroundColor, "disabled", isDark);

    // 主按钮文本
    m_generatedColors["text_primary_btn_normal"] = accentForeground;
    m_generatedColors["text_primary_btn_disabled"] = GetStateColor(accentForeground, "disabled", isDark);

    // 链接文本
    m_generatedColors["text_link_normal"] = accentColor;
    m_generatedColors["text_link_hovered"] = GetStateColor(accentColor, "hovered", isDark);
    m_generatedColors["text_link_pressed"] = GetStateColor(accentColor, "pressed", isDark);

    // 富文本框提示文本
    m_generatedColors["text_richedit_prompt"] = GetStateColor(foregroundColor, "disabled", isDark);

    // 属性网格文本
    m_generatedColors["text_property_grid_header"] = foregroundColor;

    // 主按钮（蓝色按钮）
    m_generatedColors["color_blue"] = accentColor;
    m_generatedColors["color_blue_dark"] = GetStateColor(accentColor, "pressed", isDark);
    m_generatedColors["color_blue_light"] = GetStateColor(accentColor, "hovered", isDark);
    m_generatedColors["bg_primary_btn_normal"] = accentColor;
    m_generatedColors["bg_primary_btn_hovered"] = GetStateColor(accentColor, "hovered", isDark);
    m_generatedColors["bg_primary_btn_pressed"] = GetStateColor(accentColor, "pressed", isDark);
    m_generatedColors["bg_primary_btn_disabled"] = GetStateColor(accentColor, "disabled", isDark);
    m_generatedColors["border_primary_btn_normal"] = GetStateColor(accentColor, "pressed", isDark);
    m_generatedColors["border_primary_btn_hovered"] = GetStateColor(accentColor, "hovered", isDark);
    m_generatedColors["border_primary_btn_pressed"] = GetStateColor(accentColor, "pressed", isDark);
    m_generatedColors["border_primary_btn_disabled"] = GetStateColor(accentColor, "disabled", isDark);

    // 聚焦边框
    m_generatedColors["border_control_focused"] = accentColor;
    m_generatedColors["border_richedit_focused"] = accentColor;
    m_generatedColors["border_richedit_bottom_focused"] = accentColor;
    m_generatedColors["border_list_ctrl_frame_selected"] = accentColor;
    m_generatedColors["bg_list_ctrl_frame_selected"] = GetStateColor(accentColor, "hovered", isDark);

    // 普通按钮
    m_generatedColors["bg_btn_normal"] = surface1;
    m_generatedColors["bg_btn_hovered"] = GetStateColor(surface1, "hovered", isDark);
    m_generatedColors["bg_btn_pressed"] = GetStateColor(surface1, "pressed", isDark);
    m_generatedColors["bg_btn_disabled"] = GetStateColor(surface1, "disabled", isDark);

    // 窗口按钮
    m_generatedColors["bg_btn_window_normal"] = surface1;
    m_generatedColors["bg_btn_window_hovered"] = GetStateColor(surface1, "hovered", isDark);
    m_generatedColors["bg_btn_window_pressed"] = GetStateColor(surface1, "pressed", isDark);

    // 滚动条按钮
    m_generatedColors["bg_scrollbar_btn_normal"] = surface2;
    m_generatedColors["bg_scrollbar_btn_hovered"] = GetStateColor(surface2, "hovered", isDark);
    m_generatedColors["bg_scrollbar_btn_pressed"] = GetStateColor(surface2, "pressed", isDark);
    m_generatedColors["bg_scrollbar_btn_arrow_normal"] = GetStateColor(accentColor, "pressed", isDark);
    m_generatedColors["bg_slider"] = surface2;
    m_generatedColors["bg_slider_thumb"] = accentColor;
    m_generatedColors["bg_scrollbar_normal"] = surface2;
    m_generatedColors["bg_scrollbar_thumb_normal"] = surface2;

    // 菜单
    m_generatedColors["bg_menu_item_normal"] = surface0;
    m_generatedColors["bg_menu_item_hovered"] = GetStateColor(surface0, "hovered", isDark);
    m_generatedColors["bg_menu_item_pressed"] = GetStateColor(surface0, "pressed", isDark);
    m_generatedColors["bg_menu_item_selected"] = GetStateColor(surface0, "selected", isDark);
    m_generatedColors["bg_menu_bar"] = surface1;
    m_generatedColors["bg_menu_bar_hovered"] = GetStateColor(surface1, "hovered", isDark);
    m_generatedColors["bg_menu_bar_pressed"] = GetStateColor(surface1, "pressed", isDark);

    // 列表项
    m_generatedColors["bg_list_item_normal"] = surface1;
    m_generatedColors["bg_list_item_hovered"] = GetStateColor(surface1, "hovered", isDark);
    m_generatedColors["bg_list_item_pressed"] = GetStateColor(surface1, "pressed", isDark);
    m_generatedColors["bg_list_item_selected"] = GetStateColor(surface1, "selected", isDark);
    m_generatedColors["bg_list_item_disabled"] = GetStateColor(surface1, "disabled", isDark);
    m_generatedColors["bg_list_ctrl_header"] = surface1;
    m_generatedColors["bg_list_ctrl_item_normal"] = surface1;

    // 树形控件
    m_generatedColors["bg_tree_view_node_normal"] = surface1;
    m_generatedColors["bg_tree_view_node_hovered"] = GetStateColor(surface1, "hovered", isDark);
    m_generatedColors["bg_tree_view_node_pressed"] = GetStateColor(surface1, "pressed", isDark);
    m_generatedColors["bg_tree_view_node_selected"] = GetStateColor(surface1, "selected", isDark);
    m_generatedColors["bg_tree_view_node_disabled"] = GetStateColor(surface1, "disabled", isDark);

    // 组合框
    m_generatedColors["bg_combo"] = surface0;
    m_generatedColors["bg_combo_btn_normal"] = surface1;
    m_generatedColors["bg_combo_btn_hovered"] = GetStateColor(surface1, "hovered", isDark);
    m_generatedColors["bg_combo_btn_pressed"] = GetStateColor(surface1, "pressed", isDark);
    m_generatedColors["bg_combo_btn_disabled"] = GetStateColor(surface1, "disabled", isDark);
    m_generatedColors["border_combo_normal"] = surface2;

    // 富文本框
    m_generatedColors["bg_richedit"] = surface0;
    m_generatedColors["bg_richedit_btn_normal"] = surface1;
    m_generatedColors["bg_richedit_btn_hovered"] = GetStateColor(surface1, "hovered", isDark);
    m_generatedColors["bg_richedit_btn_pressed"] = GetStateColor(surface1, "pressed", isDark);
    m_generatedColors["bg_richedit_btn_disabled"] = GetStateColor(surface1, "disabled", isDark);
    m_generatedColors["border_richedit_normal"] = surface2;

    // Tab页
    m_generatedColors["bg_tab_ctrl_item_close_normal"] = surface2;
    m_generatedColors["bg_tab_ctrl_item_close_hovered"] = GetStateColor(surface2, "hovered", isDark);
    m_generatedColors["bg_tab_ctrl_item_close_pressed"] = GetStateColor(surface2, "pressed", isDark);
    m_generatedColors["bg_tab_ctrl_item_normal"] = surface0;
    m_generatedColors["bg_tab_ctrl"] = surface0;
    m_generatedColors["border_tab_ctrl_item"] = surface2;

    // 地址栏
    m_generatedColors["bg_address_bar"] = surface1;
    m_generatedColors["border_address_bar"] = surface2;
    m_generatedColors["bg_address_bar_btn_normal"] = surface1;

    // 属性网格和分隔条
    m_generatedColors["bg_property_grid"] = surface1;
    m_generatedColors["bg_split_normal"] = surface2;

    // 边框颜色
    m_generatedColors["border_normal"] = GetStateColor(surface2, "pressed", isDark);
    m_generatedColors["border_control_normal"] = surface2;
    m_generatedColors["border_btn_normal"] = surface2;
}

std::string ThemeGenerator::GetGeneratedColor(const std::string& colorName) const
{
    auto iter = m_generatedColors.find(colorName);
    if (iter != m_generatedColors.end()) {
        return iter->second;
    }
    return std::string();
}

bool ThemeGenerator::LoadConfigFromXml(const std::string& inputXml)
{
    ASSERT(!inputXml.empty());
    if (inputXml.empty()) {
        return false;
    }
    pugi::xml_document doc;
    pugi::xml_parse_result result;

    if (inputXml.front() == _T('<')) {
        //按文件数据加载
        m_originalXmlContent = inputXml; // 保存原始XML内容，用于后续精确替换value值
        result = doc.load_buffer(inputXml.c_str(), inputXml.size());
    }
    else {
        //按文件路径加载
        // 保存原始XML内容，用于后续精确替换value值
        std::ifstream file(inputXml, std::ios::binary);
        if (!file.is_open()) {
            return false;
        }

        std::stringstream buffer;
        buffer << file.rdbuf();
        m_originalXmlContent = buffer.str();
        file.close();

        result = doc.load_file(inputXml.c_str());
    }

    ASSERT(result);
    if (!result) {
        return false;
    }

    m_loadedConfigs.clear();
    m_themeMeta.properties.clear();

    pugi::xml_node root = doc.root().first_child();
    if (DString(root.name()) != _T("Global")) {
        return false;
    }

    int orderIndex = 0;
    for (pugi::xml_node child : root.children()) {
        DString nodeName = child.name();

        if (nodeName == _T("Theme")) {
            m_themeMeta.properties["theme_name"] = StringConvert::TToUTF8(child.attribute(_T("name")).as_string());
            m_themeMeta.properties["theme_type"] = StringConvert::TToUTF8(child.attribute(_T("type")).as_string());
            m_themeMeta.properties["theme_style"] = StringConvert::TToUTF8(child.attribute(_T("style")).as_string());
        }
        else if (nodeName == _T("ThemeMeta")) {
            for (pugi::xml_node prop : child.children()) {
                if (DString(prop.name()) == _T("Property")) {
                    std::string propName = StringConvert::TToUTF8(prop.attribute(_T("name")).as_string());
                    std::string propValue = StringConvert::TToUTF8(prop.attribute(_T("value")).as_string());
                    m_themeMeta.properties[propName] = propValue;
                }
            }
        }
        else if (nodeName == _T("ThemeColor")) {
            ThemeColorConfig config;
            config.name = StringConvert::TToUTF8(child.attribute(_T("name")).as_string());
            config.value = StringConvert::TToUTF8(child.attribute(_T("value")).as_string());
            config.type = StringConvert::TToUTF8(child.attribute(_T("type")).as_string());
            config.category = StringConvert::TToUTF8(child.attribute(_T("category")).as_string());
            config.role = StringConvert::TToUTF8(child.attribute(_T("role")).as_string());
            config.derived_from = StringConvert::TToUTF8(child.attribute(_T("derived_from")).as_string());
            config.adjust = StringConvert::TToUTF8(child.attribute(_T("adjust")).as_string());
            config.fixed = (DString(child.attribute(_T("fixed")).as_string(_T("false"))) == _T("true"));
            config.support_accent = (DString(child.attribute(_T("support_accent")).as_string(_T("false"))) == _T("true"));
            config.contrast_bg = StringConvert::TToUTF8(child.attribute(_T("contrast_bg")).as_string());
            config.comment_cn = StringConvert::TToUTF8(child.attribute(_T("comment_cn")).as_string());
            config.comment_en = StringConvert::TToUTF8(child.attribute(_T("comment_en")).as_string());
            config.node_order = orderIndex++;

            std::pair<std::string, std::string> statePair = DetectColorState(config.name);
            if (!statePair.first.empty()) {
                config.m_state = statePair.first;
                config.m_baseName = statePair.second;
            }

            if (!config.name.empty()) {
                m_loadedConfigs[config.name] = config;
            }
        }
    }
    return true;
}

std::string ThemeGenerator::GenerateTheme(double hue, double base, bool isDark)
{
    if (m_loadedConfigs.empty()) {
        return std::string();
    }
    GenerateThemeColors(hue, base, isDark);
    return GenerateThemeXml(isDark);
}

std::string ThemeGenerator::GenerateThemeXml(bool isDark) const
{
    std::map<std::string, std::string> processedColors;

    // 第一步：处理所有颜色配置
    for (const auto& pair : m_loadedConfigs) {
        const std::string& colorName = pair.first;
        const ThemeColorConfig& attrs = pair.second;

        // 优先使用生成的核心颜色
        auto iter = m_generatedColors.find(colorName);
        if (iter != m_generatedColors.end()) {
            processedColors[colorName] = iter->second;
            continue;
        }

        // 检测状态变体颜色
        auto [state, baseName] = DetectColorState(colorName);
        std::string adjust = attrs.adjust;

        if (!state.empty()) {
            std::string baseColor;
            if (processedColors.find(baseName) != processedColors.end()) {
                baseColor = processedColors[baseName];
            }
            else if (m_generatedColors.find(baseName) != m_generatedColors.end()) {
                baseColor = m_generatedColors.find(baseName)->second;
            }
            else {
                std::string derivedFrom = attrs.derived_from;
                if (!derivedFrom.empty()) {
                    if (processedColors.find(derivedFrom) != processedColors.end()) {
                        baseColor = processedColors[derivedFrom];
                    }
                    else if (m_generatedColors.find(derivedFrom) != m_generatedColors.end()) {
                        baseColor = m_generatedColors.find(derivedFrom)->second;
                    }
                }
            }

            if (!baseColor.empty()) {
                if (!adjust.empty()) {
                    std::string adjusted = ApplyAdjustments(baseColor, adjust);
                    processedColors[colorName] = GetStateColor(adjusted, state, isDark);
                }
                else {
                    processedColors[colorName] = GetStateColor(baseColor, state, isDark);
                }
                continue;
            }
        }

        // 处理派生颜色和调整
        std::string originalValue = attrs.value;
        if (!originalValue.empty()) {
            uint8_t a, r, g, b;
            if (ColorConverter::ParseHexColor(originalValue, a, r, g, b)) {
                bool isSvgOrColor = (colorName.substr(0, 9) == "border_svg") || (colorName.substr(0, 6) == "color_");

                if (isSvgOrColor) {
                    std::string lowerName = colorName;
                    std::transform(lowerName.begin(), lowerName.end(), lowerName.begin(),
                        [](unsigned char c) { return static_cast<char>(std::tolower(c)); });

                    if (lowerName.find("white") != std::string::npos || lowerName.find("black") != std::string::npos) {
                        if (!isDark) {
                            if (lowerName.find("white") != std::string::npos) {
                                processedColors[colorName] = GetGeneratedColor("--foreground");
                            }
                            else {
                                processedColors[colorName] = GetGeneratedColor("bg_window_main");
                            }
                        }
                        else {
                            if (lowerName.find("white") != std::string::npos) {
                                processedColors[colorName] = GetGeneratedColor("bg_window_main");
                            }
                            else {
                                processedColors[colorName] = GetGeneratedColor("--foreground");
                            }
                        }
                        continue;
                    }
                }

                std::string derivedFrom = attrs.derived_from;

                if (!derivedFrom.empty()) {
                    std::string baseColor;
                    if (processedColors.find(derivedFrom) != processedColors.end()) {
                        baseColor = processedColors[derivedFrom];
                    }
                    else if (m_generatedColors.find(derivedFrom) != m_generatedColors.end()) {
                        baseColor = GetGeneratedColor(derivedFrom);
                    }

                    if (!baseColor.empty()) {
                        if (!adjust.empty()) {
                            processedColors[colorName] = ApplyAdjustments(baseColor, adjust);
                        }
                        else {
                            processedColors[colorName] = baseColor;
                        }
                        continue;
                    }
                }

                if (!adjust.empty()) {
                    processedColors[colorName] = ApplyAdjustments(originalValue, adjust);
                }
                else {
                    processedColors[colorName] = originalValue;
                }
            }
            else {
                processedColors[colorName] = originalValue;
            }
        }
        else {
            processedColors[colorName] = originalValue;
        }
    }

    // 第二步：对比度检查和修正
    // 对所有processedColors中的颜色进行检查（只要在loadedConfigs中配置了contrast_bg）
    for (const auto& pair : processedColors) {
        const std::string& colorName = pair.first;

        auto itLoaded = m_loadedConfigs.find(colorName);
        if (itLoaded == m_loadedConfigs.end()) {
            continue;
        }

        const ThemeColorConfig& attrs = itLoaded->second;
        if (attrs.contrast_bg.empty()) {
            continue;
        }

        std::string contrastBgStr = attrs.contrast_bg;
        size_t commaPos = contrastBgStr.find(',');
        if (commaPos != std::string::npos) {
            contrastBgStr = contrastBgStr.substr(0, commaPos);
        }

        while (!contrastBgStr.empty() && (contrastBgStr[0] == ' ' || contrastBgStr[0] == '\t')) {
            contrastBgStr = contrastBgStr.substr(1);
        }
        while (!contrastBgStr.empty() && (contrastBgStr.back() == ' ' || contrastBgStr.back() == '\t')) {
            contrastBgStr.pop_back();
        }

        auto itBg = processedColors.find(contrastBgStr);
        auto itColor = processedColors.find(colorName);

        if (itBg != processedColors.end() && itColor != processedColors.end()) {
            std::string textColor = itColor->second;
            std::string bgColor = itBg->second;
            std::string corrected = EnsureContrast(textColor, bgColor, 4.5);
            if (!corrected.empty()) {
                processedColors[colorName] = corrected;
            }
        }
    }

    // 第三步：补充缺失的颜色
    for (const auto& pair : m_loadedConfigs) {
        const std::string& colorName = pair.first;
        if (processedColors.find(colorName) == processedColors.end()) {
            processedColors[colorName] = pair.second.value;
        }
    }

    // 第四步：精确替换原始XML中的value值（保持原始格式和顺序）
    std::string result = m_originalXmlContent;

    for (const auto& pair : processedColors) {
        const std::string& colorName = pair.first;
        const std::string& newValue = pair.second;

        if (colorName.empty() || newValue.empty()) {
            continue;
        }

        size_t namePos = result.find("name=\"" + colorName + "\"");
        if (namePos == std::string::npos) {
            continue;
        }

        size_t valuePos = result.find("value=\"", namePos);
        if (valuePos == std::string::npos) {
            continue;
        }

        size_t valueStart = valuePos + 7;
        size_t valueEnd = result.find("\"", valueStart);
        if (valueEnd == std::string::npos) {
            continue;
        }

        result = result.substr(0, valueStart) + newValue + result.substr(valueEnd);
    }

    return result;
}

}
