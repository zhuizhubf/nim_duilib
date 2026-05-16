#include "ColorMap.h"
#include "duilib/Core/GlobalManager.h"

namespace ui 
{
void ColorMap::AddColor(const DString& strName, const DString& strValue)
{
    ASSERT(!strName.empty() && !strValue.empty());
    if (strName.empty() || strValue.empty()) {
        return;
    }
    UiColor color = ColorManager::ConvertToUiColor(strValue);
    AddColor(strName, color);
}

void ColorMap::AddColor(const DString& strName, UiColor argb)
{
    ASSERT(!strName.empty() && (argb.GetARGB() != 0));
    if (strName.empty() || (argb.GetARGB() == 0)) {
        return;
    }
#ifdef _DEBUG
    if (!GlobalManager::Instance().Theme().IsSwitchingTheme()) {
        //在切换主题的时候，允许覆盖，其他情况下覆盖时断言
        auto iter = m_colorMap.find(strName);
        if (iter != m_colorMap.end()) {
            ASSERT(iter->second == argb);
        }
    }
#endif
    m_colorMap[strName] = argb;
}

UiColor ColorMap::GetColor(const DString& strName) const
{
    if (!strName.empty()) {
        auto it = m_colorMap.find(strName);
        if (it != m_colorMap.end()) {
            return it->second;
        }
    }    
    return UiColor();
}

void ColorMap::RemoveColor(const DString& strName)
{
    auto it = m_colorMap.find(strName);
    if (it != m_colorMap.end()) {
        m_colorMap.erase(it);
    }
}

void ColorMap::RemoveAllColors()
{
    m_colorMap.clear();
}

} // namespace ui

