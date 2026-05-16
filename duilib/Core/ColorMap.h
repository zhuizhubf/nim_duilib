#ifndef UI_CORE_COLOR_MAP_H_
#define UI_CORE_COLOR_MAP_H_

#include "duilib/Core/UiColor.h"
#include <unordered_map>

namespace ui 
{
/** 颜色值的管理容器
*/
class DUILIB_API ColorMap
{
public:
    /** 添加一个颜色值
    * @param[in] strName 颜色名称（如 white）
    * @param[in] strValue 颜色具体数值（如 #FFFFFFFF）
    */
    void AddColor(const DString& strName, const DString& strValue);

    /** 添加一个颜色值
    * @param[in] strName 颜色名称（如 white）
    * @param[in] argb 颜色具体数值, 以ARGB格式表示
    */
    void AddColor(const DString& strName, UiColor argb);

    /** 根据名称获取一个颜色的具体数值
    * @param[in] strName 要获取的颜色名称
    * @return 返回 ARGB 格式的颜色描述值
    */
    UiColor GetColor(const DString& strName) const;

    /** 删除指定的颜色属性
    * @param [in] strName 要删除的颜色名称
    */
    void RemoveColor(const DString& strName);

    /** 删除所有颜色属性
    */
    void RemoveAllColors();

private:
    /** 颜色名称与颜色值的映射关系
    */
    std::unordered_map<DString, UiColor> m_colorMap;
};

} // namespace ui

#endif // UI_CORE_COLOR_MAP_H_
