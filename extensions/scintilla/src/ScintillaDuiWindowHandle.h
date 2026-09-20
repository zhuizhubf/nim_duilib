#ifndef DUILIB_EXT_SCINTILLA_SCINTILLA_DUI_WINDOW_HANDLE_H_
#define DUILIB_EXT_SCINTILLA_SCINTILLA_DUI_WINDOW_HANDLE_H_

#include <memory>
#include <optional>
#include <stdexcept>
#include <string>
#include <string_view>
#include <vector>

#include "duilib/Core/UiPoint.h"

// clang-format off
//Scintilla 头文件有包含顺序要求，禁止自动排序（ScintillaTypes 必须最先包含）
#include "Sci_Position.h"
#include "ScintillaTypes.h"
#include "ScintillaStructures.h"
#include "ScintillaMessages.h"
#include "Scintilla.h"
#include "Geometry.h"
#include "Platform.h"
// clang-format on

namespace duilib::ext::scintilla {

/** Scintilla 平台层窗口句柄。
 *
 *  Scintilla 的 WindowID 是 void*，平台实现按照该接口解释：
 *    - 编辑器窗口（ScintillaDui）
 *    - CallTip 窗口（ScintillaDuiCallTip）
 *    - 自动补全列表窗口
 *
 *  这样同一套 Scintilla::Internal::Window 封装可以驱动不同的 DUI 窗口
 *  （与 Qt 平台层把 wid 当作 QWidget 的做法一致）。
 */
class ScintillaDuiWindowHandle
{
public:
    ScintillaDuiWindowHandle() = default;
    ScintillaDuiWindowHandle(const ScintillaDuiWindowHandle &) = delete;
    ScintillaDuiWindowHandle &operator=(const ScintillaDuiWindowHandle &) = delete;
    virtual ~ScintillaDuiWindowHandle() = default;

    /** 窗口矩形（屏幕坐标） */
    virtual Scintilla::Internal::PRectangle GetWindowPositionHandle() const = 0;
    /** 设置窗口矩形（屏幕坐标） */
    virtual void SetWindowPositionHandle(Scintilla::Internal::PRectangle rc) = 0;
    /** 显示/隐藏窗口 */
    virtual void ShowWindowHandle(bool bShow) = 0;
    /** 销毁窗口 */
    virtual void DestroyWindowHandle() noexcept = 0;
    /** 全部重绘 */
    virtual void InvalidateWindowAllHandle() = 0;
    /** 局部重绘 */
    virtual void InvalidateWindowRectHandle(Scintilla::Internal::PRectangle rc) = 0;
    /** 设置鼠标光标 */
    virtual void SetWindowCursorHandle(Scintilla::Internal::Window::Cursor cursor) = 0;
    /** 获取监视器区域 */
    virtual Scintilla::Internal::PRectangle GetMonitorRectHandle(Scintilla::Internal::Point pt) = 0;
    /** 把窗口客户区坐标转换为屏幕坐标（用于弹出窗口定位） */
    virtual ui::UiPoint ClientToScreenHandle(const ui::UiPoint &ptClient) const = 0;
};

} // namespace duilib::ext::scintilla

#endif // DUILIB_EXT_SCINTILLA_SCINTILLA_DUI_WINDOW_HANDLE_H_
