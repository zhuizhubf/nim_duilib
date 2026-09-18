#include "duilib/Render/WindowRgn_Windows.h"

#ifdef DUILIB_BUILD_FOR_WIN

#include <windows.h>

#include <cmath>

namespace ui
{

bool WindowRgn::SetWindowRoundRectRgn(HWND hWnd, const UiRect& rcWnd, float rx, float ry, bool bRedraw)
{
    if (!::IsWindow(hWnd)) {
        return false;
    }
    const int nRx = (int)std::max(1.0f, std::round(rx * 2.0f));
    const int nRy = (int)std::max(1.0f, std::round(ry * 2.0f));
    HRGN hRgn = ::CreateRoundRectRgn(rcWnd.left, rcWnd.top, rcWnd.right, rcWnd.bottom, nRx, nRy);
    if (hRgn == nullptr) {
        return false;
    }
    return ::SetWindowRgn(hWnd, hRgn, bRedraw ? TRUE : FALSE) != FALSE;
}

bool WindowRgn::SetWindowRectRgn(HWND hWnd, const UiRect& rcWnd, bool bRedraw)
{
    if (!::IsWindow(hWnd)) {
        return false;
    }
    HRGN hRgn = ::CreateRectRgn(rcWnd.left, rcWnd.top, rcWnd.right, rcWnd.bottom);
    if (hRgn == nullptr) {
        return false;
    }
    return ::SetWindowRgn(hWnd, hRgn, bRedraw ? TRUE : FALSE) != FALSE;
}

void WindowRgn::ClearWindowRgn(HWND hWnd, bool bRedraw)
{
    if (::IsWindow(hWnd)) {
        ::SetWindowRgn(hWnd, nullptr, bRedraw ? TRUE : FALSE);
    }
}

} // namespace ui

#endif // DUILIB_BUILD_FOR_WIN
