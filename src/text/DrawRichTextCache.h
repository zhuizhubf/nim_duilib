#ifndef UI_TEXT_DRAW_RICH_TEXT_CACHE_H_
#define UI_TEXT_DRAW_RICH_TEXT_CACHE_H_

#include "duilib/duilib_defs.h"

namespace ui {

/** RichText 绘制缓存基类
*
*  IRender 只持有该基类指针，具体缓存数据由各渲染后端实现。
*/
class DUILIB_API DrawRichTextCache
{
public:
    virtual ~DrawRichTextCache() = default;
};

} // namespace ui

#endif // UI_TEXT_DRAW_RICH_TEXT_CACHE_H_
