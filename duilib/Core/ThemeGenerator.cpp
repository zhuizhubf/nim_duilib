#include "ThemeGenerator.h"
#include "duilib/Utils/StringConvert.h"

#include "duilib/third_party/xml/pugixml.hpp"

#include <algorithm>
#include <cmath>
#include <sstream>
#include <iomanip>
#include <fstream>
#include <cstdlib>
#include <set>

namespace ui
{

ThemeGenerator::ThemeGenerator()
    : m_hue(143.24)
    , m_base(0.0143)
    , m_isDark(false)
{
}

ThemeGenerator::~ThemeGenerator()
{
}

bool ThemeGenerator::ParseHexColor(const std::string& colorStr, uint8_t& alpha, uint8_t& r, uint8_t& g, uint8_t& b)
{
    if (colorStr.empty() || colorStr.length() != 9 || colorStr[0] != '#') {
        return false;
    }

    try {
        alpha = static_cast<uint8_t>(std::stoul(colorStr.substr(1, 2), nullptr, 16));
        r = static_cast<uint8_t>(std::stoul(colorStr.substr(3, 2), nullptr, 16));
        g = static_cast<uint8_t>(std::stoul(colorStr.substr(5, 2), nullptr, 16));
        b = static_cast<uint8_t>(std::stoul(colorStr.substr(7, 2), nullptr, 16));
        return true;
    }
    catch (...) {
        return false;
    }
}

std::string ThemeGenerator::RGBToHex(uint8_t alpha, uint8_t r, uint8_t g, uint8_t b)
{
    std::ostringstream oss;
    oss << "#" << std::uppercase << std::hex << std::setfill('0')
        << std::setw(2) << static_cast<int>(alpha)
        << std::setw(2) << static_cast<int>(r)
        << std::setw(2) << static_cast<int>(g)
        << std::setw(2) << static_cast<int>(b);
    return oss.str();
}

double ThemeGenerator::GetRelativeLuminance(uint8_t r, uint8_t g, uint8_t b)
{
    double rs = r / 255.0;
    double gs = g / 255.0;
    double bs = b / 255.0;

    rs = (rs > 0.03928) ? rs : rs / 12.92;
    gs = (gs > 0.03928) ? gs : gs / 12.92;
    bs = (bs > 0.03928) ? bs : bs / 12.92;

    return 0.2126 * rs + 0.7152 * gs + 0.0722 * bs;
}

double ThemeGenerator::CalculateContrastRatio(const std::string& color1, const std::string& color2)
{
    uint8_t a1, r1, g1, b1;
    uint8_t a2, r2, g2, b2;

    if (!ParseHexColor(color1, a1, r1, g1, b1) || !ParseHexColor(color2, a2, r2, g2, b2)) {
        return 0.0;
    }

    double l1 = GetRelativeLuminance(r1, g1, b1);
    double l2 = GetRelativeLuminance(r2, g2, b2);

    double lighter = std::max(l1, l2);
    double darker = std::min(l1, l2);

    return (lighter + 0.05) / (darker + 0.05);
}

bool ThemeGenerator::RGBToOKLCH(uint8_t r, uint8_t g, uint8_t b, double& L, double& C, double& H)
{
    double red = r / 255.0;
    double green = g / 255.0;
    double blue = b / 255.0;

    return ThemeGenerator::RGB2OKLCH(red, green, blue, &L, &C, &H) == 0;
}

std::string ThemeGenerator::OKLCHToARGB(double L, double C, double H, uint8_t alpha)
{
    uint8_t r, g, b;
    ThemeGenerator::OKLCH2RGB(L, C, H, r, g, b, alpha);
    return RGBToHex(alpha, r, g, b);
}

std::string ThemeGenerator::GetBaseColorFromHue(double hue, double base, bool isDark)
{
    // 增加baseChroma系数使背景色更明显地带有hue色彩
    // baseChroma = base * 0.15 ~ 0.25，使base变化时背景色能明显反映色相
    double baseChroma = base * 0.20;

    // 浅色模式：bg-L = 0.97 - base * 0.95
    // 深色模式：bg-L = 0.17 + base * 0.78
    double bgL;
    if (isDark) {
        bgL = 0.17 + base * 0.78;
    }
    else {
        bgL = 0.97 - base * 0.95;
    }

    return OKLCHToARGB(bgL, baseChroma, hue);
}

std::string ThemeGenerator::GetForegroundColor(double hue, double base, bool isDark)
{
    // 使用与背景相同的色相和色度
    double baseChroma = base * 0.20;

    // 浅色模式：fg-L = 0.22 + base * 0.72
    // 深色模式：fg-L = 0.92 - base * 0.75
    double fgL;
    if (isDark) {
        fgL = 0.92 - base * 0.75;
    }
    else {
        fgL = 0.22 + base * 0.72;
    }

    return OKLCHToARGB(fgL, baseChroma * 1.3, hue);
}

std::string ThemeGenerator::GetSurfaceColor(double hue, double base, bool isDark, int surfaceLevel)
{
    // sf-L = bg-L + offset
    // 浅色模式 offset = +0.025
    // 深色模式 offset = +0.07
    double baseChroma = base * 0.20;

    double bgL;
    if (isDark) {
        bgL = 0.17 + base * 0.78;
    }
    else {
        bgL = 0.97 - base * 0.95;
    }

    double offset = isDark ? 0.07 : 0.025;
    double sfL = bgL + offset * surfaceLevel;

    return OKLCHToARGB(sfL, baseChroma, hue);
}

std::string ThemeGenerator::GetNeutralColor(double hue, double base, bool isDark, int level)
{
    // 中性色使用与背景相同的色相
    // level范围0-1500，映射到0-15
    double baseChroma = base * 0.20;

    double bgL;
    if (isDark) {
        bgL = 0.17 + base * 0.78;
    }
    else {
        bgL = 0.97 - base * 0.95;
    }

    // 中性色的step与surface类似
    double step = isDark ? 0.07 : 0.025;
    double neutralL = bgL + step * (level / 100.0);

    return OKLCHToARGB(neutralL, baseChroma, hue);
}

std::string ThemeGenerator::GetStateColor(const std::string& baseColor, const std::string& state, bool isDark)
{
    if (baseColor.empty()) {
        return baseColor;
    }

    uint8_t alpha, r, g, b;
    if (!ParseHexColor(baseColor, alpha, r, g, b)) {
        return baseColor;
    }

    double L, C, H;
    if (!RGBToOKLCH(r, g, b, L, C, H)) {
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
    else if (state == "disabled") {
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

    return OKLCHToARGB(L, C, H, alpha);
}

std::string ThemeGenerator::ApplyAdjustments(const std::string& baseColor, const std::string& adjustStr)
{
    if (baseColor.empty() || adjustStr.empty()) {
        return baseColor;
    }

    uint8_t alpha, r, g, b;
    if (!ParseHexColor(baseColor, alpha, r, g, b)) {
        return baseColor;
    }

    double L, C, H;
    if (!RGBToOKLCH(r, g, b, L, C, H)) {
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

    return OKLCHToARGB(L, C, H, alpha);
}

std::pair<std::string, std::string> ThemeGenerator::DetectColorState(const std::string& colorName)
{
    std::vector<std::string> states = {"_disabled", "_pressed", "_hovered", "_selected"};
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

std::string ThemeGenerator::EnsureContrast(const std::string& textColor, const std::string& bgColor, double minContrast)
{
    uint8_t alpha, r, g, b;
    if (!ParseHexColor(textColor, alpha, r, g, b)) {
        return textColor;
    }

    double L, C, H;
    if (!RGBToOKLCH(r, g, b, L, C, H)) {
        return textColor;
    }

    uint8_t bgAlpha, bgR, bgG, bgB;
    if (!ParseHexColor(bgColor, bgAlpha, bgR, bgG, bgB)) {
        return textColor;
    }

    double bgLuminance = GetRelativeLuminance(bgR, bgG, bgB);

    double contrast = CalculateContrastRatio(textColor, bgColor);
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

        std::string testColor = OKLCHToARGB(L, C, H, alpha);
        contrast = CalculateContrastRatio(testColor, bgColor);

        if (contrast >= minContrast) {
            return testColor;
        }

        iteration++;
    }

    std::string finalColor = OKLCHToARGB(L, C, H, alpha);
    return finalColor;
}

void ThemeGenerator::GenerateThemeColors(double hue, double base, bool isDark)
{
    m_hue = hue;
    m_base = base;
    m_isDark = isDark;
    m_generatedColors.clear();

    std::string bgWindowMain = GetBaseColorFromHue(hue, base, isDark);

    uint8_t dummyA, dummyR, dummyG, dummyB;
    if (ParseHexColor(bgWindowMain, dummyA, dummyR, dummyG, dummyB)) {
        double luminance = GetRelativeLuminance(dummyR, dummyG, dummyB);
        if (isDark) {
            if (luminance > 0.5) {
                base = 1.0 - base;
            }
        }
        else {
            if (luminance < 0.5) {
                base = 1.0 - base;
            }
        }
    }

    bgWindowMain = GetBaseColorFromHue(hue, base, isDark);
    std::string fgWindowMain = GetForegroundColor(hue, base, isDark);

    std::string surface0 = GetSurfaceColor(hue, base, isDark, 0);
    std::string surface1 = GetSurfaceColor(hue, base, isDark, 1);
    std::string surface2 = GetSurfaceColor(hue, base, isDark, 2);
    std::string surface3 = GetSurfaceColor(hue, base, isDark, 3);

    // Accent固定参数
    // 浅色模式：accent-L = 0.6204, C = 0.195
    // 深色模式：accent-L = 0.68, C = 0.195
    double accentL = isDark ? 0.68 : 0.6204;
    double accentC = 0.195;
    std::string accentColor = OKLCHToARGB(accentL, accentC, hue);

    // accent-foreground：浅色模式接近白色，深色模式接近黑色
    double accentFgL = isDark ? 0.14 : 0.99;
    std::string accentForeground = OKLCHToARGB(accentFgL, 0, hue);

    std::string linkColor;
    std::string linkHoverColor;
    // 链接颜色使用accent
    if (isDark) {
        linkColor = accentColor;
        linkHoverColor = GetStateColor(accentColor, "hovered", isDark);
    }
    else {
        // 浅色模式下使用accent
        linkColor = accentColor;
        linkHoverColor = GetStateColor(accentColor, "hovered", isDark);
    }

    std::string bgBtnNormal = surface1;
    std::string bgBtnHovered = GetStateColor(bgBtnNormal, "hovered", isDark);
    std::string bgBtnPressed = GetStateColor(bgBtnNormal, "pressed", isDark);
    std::string bgBtnDisabled = GetStateColor(bgBtnNormal, "disabled", isDark);

    std::string bgBtnWindowNormal = surface1;
    std::string bgBtnWindowHovered = GetStateColor(bgBtnWindowNormal, "hovered", isDark);
    std::string bgBtnWindowPressed = GetStateColor(bgBtnWindowNormal, "pressed", isDark);

    std::string bgScrollbarBtnNormal = surface2;
    std::string bgScrollbarBtnHovered = GetStateColor(bgScrollbarBtnNormal, "hovered", isDark);
    std::string bgScrollbarBtnPressed = GetStateColor(bgScrollbarBtnNormal, "pressed", isDark);

    std::string bgMenuItemNormal = surface0;
    std::string bgMenuItemHovered = GetStateColor(bgMenuItemNormal, "hovered", isDark);
    std::string bgMenuItemPressed = GetStateColor(bgMenuItemNormal, "pressed", isDark);
    std::string bgMenuItemSelected = GetStateColor(bgMenuItemNormal, "selected", isDark);

    std::string bgMenuBarNormal = surface1;
    std::string bgMenuBarHovered = GetStateColor(bgMenuBarNormal, "hovered", isDark);
    std::string bgMenuBarPressed = GetStateColor(bgMenuBarNormal, "pressed", isDark);

    std::string bgListItemNormal = surface1;
    std::string bgListItemHovered = GetStateColor(bgListItemNormal, "hovered", isDark);
    std::string bgListItemPressed = GetStateColor(bgListItemNormal, "pressed", isDark);
    std::string bgListItemSelected = GetStateColor(bgListItemNormal, "selected", isDark);
    std::string bgListItemDisabled = GetStateColor(bgListItemNormal, "disabled", isDark);

    std::string bgTreeViewNodeNormal = surface1;
    std::string bgTreeViewNodeHovered = GetStateColor(bgTreeViewNodeNormal, "hovered", isDark);
    std::string bgTreeViewNodePressed = GetStateColor(bgTreeViewNodeNormal, "pressed", isDark);
    std::string bgTreeViewNodeSelected = GetStateColor(bgTreeViewNodeNormal, "selected", isDark);
    std::string bgTreeViewNodeDisabled = GetStateColor(bgTreeViewNodeNormal, "disabled", isDark);

    std::string bgComboNormal = surface0;
    std::string bgComboBtnNormal = surface1;
    std::string bgComboBtnHovered = GetStateColor(bgComboBtnNormal, "hovered", isDark);
    std::string bgComboBtnPressed = GetStateColor(bgComboBtnNormal, "pressed", isDark);
    std::string bgComboBtnDisabled = GetStateColor(bgComboBtnNormal, "disabled", isDark);

    std::string bgRichEditNormal = surface0;
    std::string bgRichEditBtnNormal = surface1;
    std::string bgRichEditBtnHovered = GetStateColor(bgRichEditBtnNormal, "hovered", isDark);
    std::string bgRichEditBtnPressed = GetStateColor(bgRichEditBtnNormal, "pressed", isDark);
    std::string bgRichEditBtnDisabled = GetStateColor(bgRichEditBtnNormal, "disabled", isDark);

    std::string bgTabCtrlItemCloseNormal = surface2;
    std::string bgTabCtrlItemCloseHovered = GetStateColor(bgTabCtrlItemCloseNormal, "hovered", isDark);
    std::string bgTabCtrlItemClosePressed = GetStateColor(bgTabCtrlItemCloseNormal, "pressed", isDark);

    std::string bgSliderNormal = surface2;
    std::string bgSliderThumbNormal = accentColor;

    std::string borderNormal = GetStateColor(surface2, "pressed", isDark);
    std::string borderControlNormal = surface2;
    std::string borderBtnNormal = surface2;
    std::string bgScrollbarNormal = surface2;
    std::string bgScrollbarThumbNormal = surface2;

    std::string bgListCtrlHeaderNormal = surface1;
    std::string bgListCtrlItemNormal = surface1;
    std::string borderComboNormal = surface2;
    std::string borderRicheditNormal = surface2;
    std::string bgTabCtrlItemNormal = surface0;
    std::string bgTabCtrlNormal = surface0;
    std::string borderTabCtrlItemNormal = surface2;
    std::string bgAddressBarNormal = surface1;
    std::string borderAddressBarNormal = surface2;
    std::string bgPropertyGridNormal = surface1;
    std::string textPropertyGridHeaderNormal = fgWindowMain;
    std::string bgAddressBarBtnNormal = surface1;
    std::string bgSplitNormal = surface2;
    std::string bgScrollbarBtnArrowNormal = GetStateColor(bgSliderThumbNormal, "pressed", isDark);

    for (int i = 0; i < 16; i++) {
        std::ostringstream name;
        name << "neutral_" << (i * 100);
        m_generatedColors[name.str()] = GetNeutralColor(hue, base, isDark, i * 100);
    }

    m_generatedColors["bg_window_main"] = bgWindowMain;
    m_generatedColors["foreground"] = fgWindowMain;
    m_generatedColors["surface_0"] = surface0;
    m_generatedColors["surface_1"] = surface1;
    m_generatedColors["surface_2"] = surface2;
    m_generatedColors["surface_3"] = surface3;
    m_generatedColors["accent"] = accentColor;
    m_generatedColors["accent_foreground"] = accentForeground;
    m_generatedColors["color_blue"] = accentColor;
    m_generatedColors["color_blue_dark"] = GetStateColor(accentColor, "pressed", isDark);
    m_generatedColors["color_blue_light"] = GetStateColor(accentColor, "hovered", isDark);
    m_generatedColors["text_default"] = fgWindowMain;
    m_generatedColors["text_muted"] = GetStateColor(fgWindowMain, "disabled", isDark);
    m_generatedColors["text_btn_normal"] = fgWindowMain;
    m_generatedColors["text_btn_disabled"] = GetStateColor(fgWindowMain, "disabled", isDark);
    m_generatedColors["text_primary_btn_normal"] = accentForeground;
    m_generatedColors["text_primary_btn_disabled"] = GetStateColor(accentForeground, "disabled", isDark);
    m_generatedColors["text_link_normal"] = linkColor;
    m_generatedColors["text_link_hovered"] = linkHoverColor;
    m_generatedColors["text_link_pressed"] = GetStateColor(linkColor, "pressed", isDark);
    m_generatedColors["bg_primary_btn_normal"] = accentColor;
    m_generatedColors["bg_primary_btn_hovered"] = GetStateColor(accentColor, "hovered", isDark);
    m_generatedColors["bg_primary_btn_pressed"] = GetStateColor(accentColor, "pressed", isDark);
    m_generatedColors["bg_primary_btn_disabled"] = GetStateColor(accentColor, "disabled", isDark);
    m_generatedColors["border_primary_btn_normal"] = GetStateColor(accentColor, "pressed", isDark);
    m_generatedColors["border_primary_btn_hovered"] = GetStateColor(accentColor, "hovered", isDark);
    m_generatedColors["border_primary_btn_pressed"] = GetStateColor(accentColor, "pressed", isDark);
    m_generatedColors["border_primary_btn_disabled"] = GetStateColor(accentColor, "disabled", isDark);
    m_generatedColors["border_control_focused"] = accentColor;
    m_generatedColors["border_richedit_focused"] = accentColor;
    m_generatedColors["border_richedit_bottom_focused"] = accentColor;
    m_generatedColors["border_list_ctrl_frame_selected"] = accentColor;
    m_generatedColors["bg_list_ctrl_frame_selected"] = GetStateColor(accentColor, "hovered", isDark);
    m_generatedColors["text_richedit_prompt"] = GetStateColor(fgWindowMain, "disabled", isDark);

    m_generatedColors["bg_btn_normal"] = bgBtnNormal;
    m_generatedColors["bg_btn_hovered"] = bgBtnHovered;
    m_generatedColors["bg_btn_pressed"] = bgBtnPressed;
    m_generatedColors["bg_btn_disabled"] = bgBtnDisabled;
    m_generatedColors["bg_btn_window_normal"] = bgBtnWindowNormal;
    m_generatedColors["bg_btn_window_hovered"] = bgBtnWindowHovered;
    m_generatedColors["bg_btn_window_pressed"] = bgBtnWindowPressed;
    m_generatedColors["bg_scrollbar_btn_normal"] = bgScrollbarBtnNormal;
    m_generatedColors["bg_scrollbar_btn_hovered"] = bgScrollbarBtnHovered;
    m_generatedColors["bg_scrollbar_btn_pressed"] = bgScrollbarBtnPressed;
    m_generatedColors["bg_menu_item_normal"] = bgMenuItemNormal;
    m_generatedColors["bg_menu_item_hovered"] = bgMenuItemHovered;
    m_generatedColors["bg_menu_item_pressed"] = bgMenuItemPressed;
    m_generatedColors["bg_menu_item_selected"] = bgMenuItemSelected;
    m_generatedColors["bg_menu_bar"] = bgMenuBarNormal;
    m_generatedColors["bg_menu_bar_hovered"] = bgMenuBarHovered;
    m_generatedColors["bg_menu_bar_pressed"] = bgMenuBarPressed;
    m_generatedColors["bg_list_item_normal"] = bgListItemNormal;
    m_generatedColors["bg_list_item_hovered"] = bgListItemHovered;
    m_generatedColors["bg_list_item_pressed"] = bgListItemPressed;
    m_generatedColors["bg_list_item_selected"] = bgListItemSelected;
    m_generatedColors["bg_list_item_disabled"] = bgListItemDisabled;
    m_generatedColors["bg_tree_view_node_normal"] = bgTreeViewNodeNormal;
    m_generatedColors["bg_tree_view_node_hovered"] = bgTreeViewNodeHovered;
    m_generatedColors["bg_tree_view_node_pressed"] = bgTreeViewNodePressed;
    m_generatedColors["bg_tree_view_node_selected"] = bgTreeViewNodeSelected;
    m_generatedColors["bg_tree_view_node_disabled"] = bgTreeViewNodeDisabled;
    m_generatedColors["bg_combo"] = bgComboNormal;
    m_generatedColors["bg_combo_btn_normal"] = bgComboBtnNormal;
    m_generatedColors["bg_combo_btn_hovered"] = bgComboBtnHovered;
    m_generatedColors["bg_combo_btn_pressed"] = bgComboBtnPressed;
    m_generatedColors["bg_combo_btn_disabled"] = bgComboBtnDisabled;
    m_generatedColors["bg_richedit"] = bgRichEditNormal;
    m_generatedColors["bg_richedit_btn_normal"] = bgRichEditBtnNormal;
    m_generatedColors["bg_richedit_btn_hovered"] = bgRichEditBtnHovered;
    m_generatedColors["bg_richedit_btn_pressed"] = bgRichEditBtnPressed;
    m_generatedColors["bg_richedit_btn_disabled"] = bgRichEditBtnDisabled;
    m_generatedColors["bg_tab_ctrl_item_close_normal"] = bgTabCtrlItemCloseNormal;
    m_generatedColors["bg_tab_ctrl_item_close_hovered"] = bgTabCtrlItemCloseHovered;
    m_generatedColors["bg_tab_ctrl_item_close_pressed"] = bgTabCtrlItemClosePressed;
    m_generatedColors["bg_slider"] = bgSliderNormal;
    m_generatedColors["bg_slider_thumb"] = bgSliderThumbNormal;
    m_generatedColors["border_normal"] = borderNormal;
    m_generatedColors["bg_list_ctrl_header"] = bgListCtrlHeaderNormal;
    m_generatedColors["bg_list_ctrl_item_normal"] = bgListCtrlItemNormal;
    m_generatedColors["border_combo_normal"] = borderComboNormal;
    m_generatedColors["border_richedit_normal"] = borderRicheditNormal;
    m_generatedColors["bg_tab_ctrl_item_normal"] = bgTabCtrlItemNormal;
    m_generatedColors["bg_tab_ctrl"] = bgTabCtrlNormal;
    m_generatedColors["border_tab_ctrl_item"] = borderTabCtrlItemNormal;
    m_generatedColors["bg_address_bar"] = bgAddressBarNormal;
    m_generatedColors["border_address_bar"] = borderAddressBarNormal;
    m_generatedColors["bg_property_grid"] = bgPropertyGridNormal;
    m_generatedColors["text_property_grid_header"] = textPropertyGridHeaderNormal;
    m_generatedColors["bg_address_bar_btn_normal"] = bgAddressBarBtnNormal;
    m_generatedColors["bg_split_normal"] = bgSplitNormal;
    m_generatedColors["bg_scrollbar_btn_arrow_normal"] = bgScrollbarBtnArrowNormal;
    m_generatedColors["border_control_normal"] = borderControlNormal;
    m_generatedColors["border_btn_normal"] = borderBtnNormal;
    m_generatedColors["bg_scrollbar_normal"] = bgScrollbarNormal;
    m_generatedColors["bg_scrollbar_thumb_normal"] = bgScrollbarThumbNormal;
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
    m_originalValues.clear();
    m_colorOrder.clear();

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

            if (!config.name.empty()) {
                m_loadedConfigs[config.name] = config;
                m_originalValues[config.name] = config.value;
                m_colorOrder.push_back(config.name);
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

    std::map<std::string, std::string> processedColors;

    // 第一步：处理所有颜色配置
    for (const auto& pair : m_loadedConfigs) {
        const std::string& colorName = pair.first;
        const ThemeColorConfig& attrs = pair.second;

        // 优先使用生成的核心颜色
        if (m_generatedColors.find(colorName) != m_generatedColors.end()) {
            processedColors[colorName] = m_generatedColors[colorName];
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
                baseColor = m_generatedColors[baseName];
            }
            else {
                std::string derivedFrom = attrs.derived_from;
                if (!derivedFrom.empty()) {
                    if (processedColors.find(derivedFrom) != processedColors.end()) {
                        baseColor = processedColors[derivedFrom];
                    }
                    else if (m_generatedColors.find(derivedFrom) != m_generatedColors.end()) {
                        baseColor = m_generatedColors[derivedFrom];
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
            if (ParseHexColor(originalValue, a, r, g, b)) {
                bool isSvgOrColor = (colorName.substr(0, 9) == "border_svg") || (colorName.substr(0, 6) == "color_");

                if (isSvgOrColor) {
                    std::string lowerName = colorName;
                    std::transform(lowerName.begin(), lowerName.end(), lowerName.begin(),
                        [](unsigned char c) { return static_cast<char>(std::tolower(c)); });

                    if (lowerName.find("white") != std::string::npos || lowerName.find("black") != std::string::npos) {
                        if (!isDark) {
                            if (lowerName.find("white") != std::string::npos) {
                                processedColors[colorName] = m_generatedColors["foreground"];
                            }
                            else {
                                processedColors[colorName] = m_generatedColors["bg_window_main"];
                            }
                        }
                        else {
                            if (lowerName.find("white") != std::string::npos) {
                                processedColors[colorName] = m_generatedColors["bg_window_main"];
                            }
                            else {
                                processedColors[colorName] = m_generatedColors["foreground"];
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
                        baseColor = m_generatedColors[derivedFrom];
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
            processedColors[colorName] = m_loadedConfigs[colorName].value;
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

std::string ThemeGenerator::GetGeneratedColor(const std::string& colorName) const
{
    auto it = m_generatedColors.find(colorName);
    if (it != m_generatedColors.end()) {
        return it->second;
    }
    return "";
}


static inline double srgb_transfer_function(double x) {
    if (x <= 0.0031308) {
        return 12.92 * x;
    }
    return 1.055 * pow(x, 1.0 / 2.4) - 0.055;
}

static inline double inverse_srgb_transfer_function(double x) {
    if (x <= 0.04045) {
        return x / 12.92;
    }
    return pow((x + 0.055) / 1.055, 2.4);
}

constexpr double PI = 3.141592653589793;

int ThemeGenerator::OKLCH2RGB(double L, double C, double H, double* red, double* green, double* blue)
{
    if ((red == nullptr) || (green == nullptr) || (blue == nullptr)) {
        return -1;
    }
    if (L < 0.0 || L > 1.0 || C < 0.0 || H < 0.0 || H > 360.0) {
        return -1;
    }

    double a = C * cos(H * PI / 180.0);
    double b = C * sin(H * PI / 180.0);

    double l_ = L + 0.3963377774 * a + 0.2158037573 * b;
    double m_ = L - 0.1055613458 * a - 0.0638541728 * b;
    double s_ = L - 0.0894841775 * a - 1.2914855480 * b;

    double l = l_ * l_ * l_;
    double m = m_ * m_ * m_;
    double s = s_ * s_ * s_;

    *red = +4.0767416621 * l - 3.3077115913 * m + 0.2309699292 * s;
    *green = -1.2684380046 * l + 2.6097574011 * m - 0.3413193965 * s;
    *blue = -0.0041960863 * l - 0.7034186147 * m + 1.7076147010 * s;

    *red = srgb_transfer_function(std::max(0.0, std::min(1.0, *red)));
    *green = srgb_transfer_function(std::max(0.0, std::min(1.0, *green)));
    *blue = srgb_transfer_function(std::max(0.0, std::min(1.0, *blue)));

    return 0;
}

int ThemeGenerator::OKLCH2RGB(double L, double C, double H, uint8_t& red, uint8_t& green, uint8_t& blue, uint8_t /*alpha*/)
{
    double r = 0, g = 0, b = 0;
    if (OKLCH2RGB(L, C, H, &r, &g, &b) != 0) {
        return -1;
    }
    red = static_cast<uint8_t>(255.0 * r);
    green = static_cast<uint8_t>(255.0 * g);
    blue = static_cast<uint8_t>(255.0 * b);
    return 0;
}

int ThemeGenerator::RGB2OKLCH(double red, double green, double blue, double* L, double* C, double* H)
{
    if ((L == nullptr) || (C == nullptr) || (H == nullptr)) {
        return -1;
    }
    if (red < 0.0 || red > 1.0 || green < 0.0 || green > 1.0 || blue < 0.0 || blue > 1.0) {
        return -1;
    }

    double r_lin = inverse_srgb_transfer_function(red);
    double g_lin = inverse_srgb_transfer_function(green);
    double b_lin = inverse_srgb_transfer_function(blue);

    double l_ = 0.4122214708 * r_lin + 0.5363325363 * g_lin + 0.0514459929 * b_lin;
    double m_ = 0.2119034982 * r_lin + 0.6806995451 * g_lin + 0.1073969566 * b_lin;
    double s_ = 0.0883024619 * r_lin + 0.2817188376 * g_lin + 0.6299787005 * b_lin;

    double l = cbrt(l_);
    double m = cbrt(m_);
    double s = cbrt(s_);

    *L = 0.2104542553 * l + 0.7936177850 * m - 0.0040720468 * s;
    double a = 1.9779984951 * l - 2.4285922050 * m + 0.4505937099 * s;
    double b_c = 0.0259040371 * l + 0.7827717662 * m - 0.8086757660 * s;

    *C = sqrt(a * a + b_c * b_c);
    *H = atan2(b_c, a) * 180.0 / PI;
    if (*H < 0.0) {
        *H += 360.0;
    }

    return 0;
}

}
