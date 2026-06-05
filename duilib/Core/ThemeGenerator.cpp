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
    , m_surfaceBaseChroma(1.0)
    // Accent参数
    , m_accentLightL(0.6204)
    , m_accentDarkL(0.68)
    , m_accentC(0.195)
    // 带彩色选项（默认全部为true）
    , m_bgColored(true)
    , m_fgColored(true)
    , m_surfaceColored(true)
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

void ThemeGenerator::GetBgParams(double& bgLightL, double& bgDarkL, double& bgBaseChroma) const
{
    bgLightL = m_bgLightL;
    bgDarkL = m_bgDarkL;
    bgBaseChroma = m_bgBaseChroma;
}

void ThemeGenerator::GetFgParams(double& fgLightL, double& fgDarkL, double& fgBaseChroma) const
{
    fgLightL = m_fgLightL;
    fgDarkL = m_fgDarkL;
    fgBaseChroma = m_fgBaseChroma;
}

void ThemeGenerator::GetSurfaceParams(double& surfaceLightOffset, double& surfaceDarkOffset, double& surfaceBaseChroma) const
{
    surfaceLightOffset = m_surfaceLightOffset;
    surfaceDarkOffset = m_surfaceDarkOffset;
    surfaceBaseChroma = m_surfaceBaseChroma;
}

void ThemeGenerator::GetAccentParams(double& accentLightL, double& accentDarkL, double& accentC) const
{
    accentLightL = m_accentLightL;
    accentDarkL = m_accentDarkL;
    accentC = m_accentC;
}

void ThemeGenerator::SetBgColored(bool colored)
{
    m_bgColored = colored;
}

bool ThemeGenerator::IsBgColored() const
{
    return m_bgColored;
}

void ThemeGenerator::SetFgColored(bool colored)
{
    m_fgColored = colored;
}

bool ThemeGenerator::IsFgColored() const
{
    return m_fgColored;
}

void ThemeGenerator::SetSurfaceColored(bool colored)
{
    m_surfaceColored = colored;
}

bool ThemeGenerator::IsSurfaceColored() const
{
    return m_surfaceColored;
}

void ThemeGenerator::ResetParams()
{
    // 重置所有参数为默认值
    SetBgParams(0.97, 0.17, 1.0);
    SetFgParams(0.22, 0.92, 0.15);
    SetSurfaceParams(0.025, 0.07, 1.0);
    SetAccentParams(0.6204, 0.68, 0.195);
    // 重置带彩色选项为默认值
    m_bgColored = true;
    m_fgColored = true;
    m_surfaceColored = true;
}

std::string ThemeGenerator::GetBackgroundColor(double hue, double base, bool isDark)
{
    // 使用成员变量计算背景色
    double baseChroma = m_bgColored ? (base * m_bgBaseChroma) : 0.0;
    // 实际使用的色相值，无彩色时色相不影响结果
    double effectiveHue = m_bgColored ? hue : 0.0;

    double bgL;
    if (isDark) {
        bgL = m_bgDarkL + base * m_bgDarkLScale;
    }
    else {
        bgL = m_bgLightL + base * m_bgLightLScale;
    }

    return ColorConverter::OKLCHToARGB(bgL, baseChroma, effectiveHue, 255);
}

std::string ThemeGenerator::GetForegroundColor(double hue, double base, bool isDark)
{
    // 使用成员变量计算前景色
    double fgChroma = m_fgColored ? (base * m_fgBaseChroma) : 0.0;
    double effectiveHue = m_fgColored ? hue : 0.0;

    double fgL;
    if (isDark) {
        fgL = m_fgDarkL + base * m_fgDarkLScale;
    }
    else {
        fgL = m_fgLightL + base * m_fgLightLScale;
    }

    return ColorConverter::OKLCHToARGB(fgL, fgChroma, effectiveHue, 255);
}

std::string ThemeGenerator::GetSurfaceColor(double hue, double base, bool isDark, int surfaceLevel)
{
    // 使用成员变量计算Surface颜色
    double surfaceChroma = m_surfaceColored ? (base * m_surfaceBaseChroma) : 0.0;
    double effectiveHue = m_surfaceColored ? hue : 0.0;

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

    return ColorConverter::OKLCHToARGB(sfL, surfaceChroma, effectiveHue, 255);
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
    std::vector<std::string> states = {"_default", "_normal", "_disabled", "_pressed", "_hovered", "_selected", "_prompt" , "_focused" };
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

    // 根据背景明度确定调整方向：
    // 浅色背景(bgLuminance > 0.5)：需要让文字更暗(L减小)
    // 深色背景(bgLuminance <= 0.5)：需要让文字更亮(L增大)
    const bool bgIsLight = (bgLuminance > 0.5);
    const double LStep = bgIsLight ? -0.05 : 0.05;

    const int maxIterations = 50;
    int iteration = 0;

    while (iteration < maxIterations) {
        // 调整L值
        L = L + LStep;

        // 钳制到 [0, 1] 范围
        if (L < 0.0) L = 0.0;
        if (L > 1.0) L = 1.0;

        // 如果已经达到目标方向极端值，停止迭代
        if (bgIsLight && L <= 0.0) break;
        if (!bgIsLight && L >= 1.0) break;

        std::string testColor = ColorConverter::OKLCHToARGB(L, C, H, alpha);
        contrast = ColorConverter::CalculateContrastRatio(testColor, bgColor);

        if (contrast >= minContrast) {
            return testColor;
        }

        // 如果已无法再继续调整（到达极限），停止
        if ((bgIsLight && L <= 0.0) || (!bgIsLight && L >= 1.0)) {
            break;
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

    //筛选出需要处理的颜色
    std::map<std::string, ThemeColorConfig> generatedConfigs;
    for (const auto& pair : m_loadedConfigs) {
        const std::string& colorName = pair.first;
        const ThemeColorConfig& attrs = pair.second;
        if (attrs.fixed) {
            //固定属性，不需要生成颜色
            //continue;
        }
        if (attrs.category == "basic_color") {
            //基础色，不需要生成颜色
            continue;
        }
        if (!attrs.derived_from.empty()) {
            //继承自其他颜色的颜色，不需要生成颜色
            continue;
        }
        if (!attrs.m_state.empty()) {
            //有状态的颜色，按BaseName生成
            generatedConfigs[attrs.m_baseName] = attrs;
            if ((attrs.m_state == "normal") || (attrs.m_state == "default")) {                       
                generatedConfigs[colorName] = attrs; //仅保留正常状态的颜色值
            }
        }
        else {
            generatedConfigs[colorName] = attrs;
        }
    }

    //根据规则生成颜色
    for (const auto& pair : generatedConfigs) {
        const std::string& colorName = pair.first;
        const ThemeColorConfig& attrs = pair.second;
        std::string colorValue;
        if (attrs.support_accent) {
            //支持强调色的，使用强调色
            colorValue = accentColor;            
        }
        else if (attrs.category == "bg_color") {
            //背景色
            colorValue = backgroundColor;
        }
        else if (attrs.category == "border_color") {
            //边框色
            colorValue = surface1;
        }
        else if (attrs.category == "text_color") {
            //文本颜色
            colorValue = foregroundColor;
        }
        if (!colorValue.empty()) {
            if (!attrs.m_state.empty()) {
                m_generatedColors[attrs.m_baseName] = colorValue;
            }
            m_generatedColors[colorName] = colorValue;
        }
    }

    //固定色设置
    //
    //窗口关闭按钮
    m_generatedColors["bg_btn_window_close"] = m_generatedColors["--error"];

    //常用背景和边框
    m_generatedColors["bg_titlebar"] = surface2;
    m_generatedColors["bg_window_card"] = surface0;
    m_generatedColors["bg_container"] = surface0;
    m_generatedColors["bg_content"] = surface0;
    m_generatedColors["bg_overlay"] = surface0;
    m_generatedColors["bg_header"] = surface2;

    m_generatedColors["border_window"] = surface3;

    //编辑框背景
    m_generatedColors["bg_richedit"] = surface0;
    //地址栏背景
    m_generatedColors["bg_address_bar"] = surface0;
    //列表容器
    m_generatedColors["bg_list"] = surface0;

    m_generatedColors["bg_list_ctrl"] = surface0;
    m_generatedColors["bg_list_ctrl_view"] = surface0;

    m_generatedColors["bg_tree_view"] = surface0;

    //进度条背景
    m_generatedColors["bg_progress_track"] = surface2;

    //分割线颜色
    m_generatedColors["bg_split_normal"] = surface2;
    m_generatedColors["border_split_level2"] = surface2;
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

    // 优先使用生成的核心颜色
    for (const auto& pair : m_loadedConfigs) {
        const std::string& colorName = pair.first;
        auto iter = m_generatedColors.find(colorName);
        if (iter != m_generatedColors.end()) {
            processedColors[colorName] = iter->second;
        }
    }

    // 第1步：处理所有颜色配置的变体颜色
    for (const auto& pair : m_loadedConfigs) {
        const std::string& colorName = pair.first;

        //已经生成的，不重复计算
        auto iter = processedColors.find(colorName);
        if (iter != processedColors.end()) {
            continue;
        }

        //处理状态变体颜色
        auto [state, baseName] = DetectColorState(colorName);
        if (!state.empty()) {
            std::string baseColor;//基准色（用于计算状态变体颜色）
            if (processedColors.find(baseName) != processedColors.end()) {
                baseColor = processedColors[baseName];
            }
            else if (m_generatedColors.find(baseName) != m_generatedColors.end()) {
                baseColor = m_generatedColors.find(baseName)->second;
            }
            if (!baseColor.empty()) {
                processedColors[colorName] = GetStateColor(baseColor, state, isDark);
            }
        }
    }

    //第2步：处理派生颜色
    for (const auto& pair : m_loadedConfigs) {
        const std::string& colorName = pair.first;
        const ThemeColorConfig& attrs = pair.second;

        std::string derivedFrom = attrs.derived_from;
        if (derivedFrom.empty()) {
            //非派生颜色
            continue;
        }

        //处理派生颜色
        std::string baseColor;//基准色（用于计算派生颜色）
        if (processedColors.find(derivedFrom) != processedColors.end()) {
            baseColor = processedColors[derivedFrom];
        }
        else if (m_generatedColors.find(derivedFrom) != m_generatedColors.end()) {
            baseColor = m_generatedColors.find(derivedFrom)->second;
        }
        if (baseColor.empty()) {
            auto [derivedFromState, derivedFromBaseName] = DetectColorState(derivedFrom);
            if (!derivedFromState.empty()) {
                if (processedColors.find(derivedFromBaseName) != processedColors.end()) {
                    baseColor = processedColors[derivedFromBaseName];
                }
                else if (m_generatedColors.find(derivedFromBaseName) != m_generatedColors.end()) {
                    baseColor = m_generatedColors.find(derivedFromBaseName)->second;
                }
            }
        }

        if (!baseColor.empty()) {
            std::string adjust = attrs.adjust;
            if (!adjust.empty()) {
                processedColors[colorName] = ApplyAdjustments(baseColor, adjust);
            }
            else {
                processedColors[colorName] = baseColor;
            }
        }
    }

    // 第3步：对比度检查和修正
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

        std::list<std::string> contrastBgList = StringUtil::Split(attrs.contrast_bg, ",");
        for (std::string& contrastBg : contrastBgList) {
            StringUtil::Trim(contrastBg);
            if (!contrastBg.empty()) {
                auto itBg = processedColors.find(contrastBg);
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
        }
    }

    // 第4步：保留原颜色的透明度
    for (const auto& pair : m_loadedConfigs) {
        const std::string& colorName = pair.first;
        auto iter = processedColors.find(colorName);
        if (iter != processedColors.end()) {
            std::string oldColor = pair.second.value;
            std::string newColor = iter->second;
            uint8_t alpha, r, g, b;
            if (ColorConverter::ParseHexColor(newColor, alpha, r, g, b)) {
                if (alpha == 255) {
                    uint8_t alpha0, r0, g0, b0;
                    if (ColorConverter::ParseHexColor(oldColor, alpha0, r0, g0, b0)) {
                        if (alpha0 != alpha) {
                            iter->second = ColorConverter::RGBToHex(alpha0, r, g, b);
                        }
                    }
                }
            }
        }
    }

    // 第5步：补充缺失的颜色
    for (const auto& pair : m_loadedConfigs) {
        const std::string& colorName = pair.first;
        if (processedColors.find(colorName) == processedColors.end()) {
            processedColors[colorName] = pair.second.value;
        }
    }

    // 第6步：精确替换原始XML中的value值（保持原始格式和顺序）
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
