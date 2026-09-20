#include "ScintillaDuiPlatform.h"

#include "ScintillaControl.h"
#include "ScintillaDui.h"
#include "ScintillaDuiPopup.h"
#include "ScintillaDuiWindowHandle.h"

#include "duilib/Core/UiRect.h"

#include "Debugging.h"
#include "Platform.h"

#include <algorithm>
#include <cassert>
#include <cstdarg>
#include <cstdio>
#include <cstdlib>
#include <string>
#include <vector>

#ifdef _WIN32
#include <windows.h>
#endif

namespace duilib::ext::scintilla {

bool InitializePlatform()
{
    return true;
}

void UninitializePlatform() {}

} // namespace duilib::ext::scintilla

namespace Scintilla::Internal {

using duilib::ext::scintilla::ScintillaDui;
using duilib::ext::scintilla::ScintillaDuiWindowHandle;

namespace {

/** 把 Scintilla 的平台窗口句柄解释为本移植层的窗口接口 */
ScintillaDuiWindowHandle *ToWindowHandle(WindowID wid) noexcept
{
    return static_cast<ScintillaDuiWindowHandle *>(wid);
}

} // namespace

//------------------------------------------------------------------------------
// Window：所有窗口操作都通过 ScintillaDuiWindowHandle 接口分发
//------------------------------------------------------------------------------

Window::~Window() noexcept = default;

void Window::Destroy() noexcept
{
    if (ScintillaDuiWindowHandle *pHandle = ToWindowHandle(wid)) {
        pHandle->DestroyWindowHandle();
    }
    wid = nullptr;
    cursorLast = Cursor::invalid;
}

PRectangle Window::GetPosition() const
{
    if (ScintillaDuiWindowHandle *pHandle = ToWindowHandle(wid)) {
        return pHandle->GetWindowPositionHandle();
    }
    return PRectangle();
}

void Window::SetPosition(PRectangle rc)
{
    if (ScintillaDuiWindowHandle *pHandle = ToWindowHandle(wid)) {
        pHandle->SetWindowPositionHandle(rc);
    }
}

void Window::SetPositionRelative(PRectangle rc, const Window *relativeTo)
{
    ScintillaDuiWindowHandle *pHandle = ToWindowHandle(wid);
    if (pHandle == nullptr) {
        return;
    }
    ui::UiPoint pt(static_cast<int32_t>(rc.left), static_cast<int32_t>(rc.top));
    const ScintillaDuiWindowHandle *pRelative = (relativeTo != nullptr)
                                                    ? ToWindowHandle(relativeTo->GetID())
                                                    : nullptr;
    if (pRelative != nullptr) {
        //rc 是相对于 relativeTo 的客户区坐标，需要转换为屏幕坐标
        pt = pRelative->ClientToScreenHandle(pt);
    }
    const int32_t nWidth = std::max<int32_t>(1, static_cast<int32_t>(rc.Width()));
    const int32_t nHeight = std::max<int32_t>(1, static_cast<int32_t>(rc.Height()));
    pHandle->SetWindowPositionHandle(
        PRectangle::FromInts(pt.x, pt.y, pt.x + nWidth, pt.y + nHeight));
}

PRectangle Window::GetClientPosition() const
{
    return GetPosition();
}

void Window::Show(bool show)
{
    if (ScintillaDuiWindowHandle *pHandle = ToWindowHandle(wid)) {
        pHandle->ShowWindowHandle(show);
    }
}

void Window::InvalidateAll()
{
    if (ScintillaDuiWindowHandle *pHandle = ToWindowHandle(wid)) {
        pHandle->InvalidateWindowAllHandle();
    }
}

void Window::InvalidateRectangle(PRectangle rc)
{
    if (ScintillaDuiWindowHandle *pHandle = ToWindowHandle(wid)) {
        pHandle->InvalidateWindowRectHandle(rc);
    }
}

void Window::SetCursor(Cursor curs)
{
    if (ScintillaDuiWindowHandle *pHandle = ToWindowHandle(wid)) {
        pHandle->SetWindowCursorHandle(curs);
    }
}

PRectangle Window::GetMonitorRect(Point pt)
{
    if (ScintillaDuiWindowHandle *pHandle = ToWindowHandle(wid)) {
        return pHandle->GetMonitorRectHandle(pt);
    }
    return PRectangle();
}

//------------------------------------------------------------------------------
// ListBox：自动补全列表（由 ScintillaDuiPopup.cpp 中的实现提供）
//------------------------------------------------------------------------------

ListBox::ListBox() noexcept = default;
ListBox::~ListBox() noexcept = default;

std::unique_ptr<ListBox> ListBox::Allocate()
{
    return duilib::ext::scintilla::CreateDuiListBox();
}

//------------------------------------------------------------------------------
// Menu：右键菜单（由 ui::Menu 实现，对应 Qt 的 QMenu）
//------------------------------------------------------------------------------

Menu::Menu() noexcept
    : mid(nullptr)
{}

void Menu::CreatePopUp() {}

void Menu::Destroy() noexcept
{
    mid = nullptr;
}

void Menu::Show(Point pt, const Window &w)
{
    ScintillaDuiWindowHandle *pHandle = ToWindowHandle(w.GetID());
    ScintillaDui *pDui = dynamic_cast<ScintillaDui *>(pHandle);
    if ((pDui == nullptr) || (pDui->GetControl() == nullptr)) {
        return;
    }
    pDui->GetControl()->ShowPopupMenuDui(
        ui::UiPoint(static_cast<int32_t>(pt.x), static_cast<int32_t>(pt.y)));
}

//------------------------------------------------------------------------------
// Platform：系统相关参数
//------------------------------------------------------------------------------

namespace Platform {

ColourRGBA Chrome()
{
    return ColourRGBA(0xf0, 0xf0, 0xf0);
}

ColourRGBA ChromeHighlight()
{
    return ColourRGBA(0xff, 0xff, 0xff);
}

const char *DefaultFont()
{
#ifdef _WIN32
    return "Consolas";
#else
    return "Monospace";
#endif
}

int DefaultFontSize()
{
    return 11;
}

unsigned int DoubleClickTime()
{
#ifdef _WIN32
    return ::GetDoubleClickTime();
#else
    return 500;
#endif
}

void DebugDisplay(const char *s) noexcept
{
#ifdef _WIN32
    ::OutputDebugStringA(s != nullptr ? s : "");
#else
    std::fputs(s != nullptr ? s : "", stderr);
#endif
}

void DebugPrintf(const char *format, ...) noexcept
{
    if (format == nullptr) {
        return;
    }
    char buffer[2048] = {};
    va_list args;
    va_start(args, format);
    std::vsnprintf(buffer, sizeof(buffer), format, args);
    va_end(args);
    DebugDisplay(buffer);
}

bool ShowAssertionPopUps(bool /*assertionPopUps*/) noexcept
{
    return false;
}

void Assert(const char *c, const char *file, int line) noexcept
{
    std::fprintf(stderr, "Scintilla assertion failed: %s (%s:%d)\n", c, file, line);
    std::abort();
}

} // namespace Platform

} // namespace Scintilla::Internal
