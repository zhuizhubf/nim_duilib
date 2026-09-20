#include "ScintillaDui.h"

#include "ScintillaControl.h"
#include "ScintillaDuiPopup.h"
#include "ScintillaDuiSurface.h"

#include "duilib/Core/GlobalManager.h"
#include "duilib/Core/TimerManager.h"
#include "duilib/Core/UiPoint.h"
#include "duilib/Core/UiRect.h"
#include "duilib/Core/Window.h"
#include "duilib/Utils/Clipboard.h"

#include <algorithm>
#include <cmath>

namespace duilib::ext::scintilla {

using namespace Scintilla;
using namespace Scintilla::Internal;

namespace {

/** 离屏度量 Render 的最小尺寸（文本度量与尺寸无关，仅需一个有效的绘制目标） */
constexpr int32_t kMeasurementRenderSize = 8;

} // namespace

ScintillaDui::ScintillaDui(ScintillaControl *pControl)
    : m_pControl(pControl)
{
    //wMain 是本平台层的窗口句柄（对应 Qt 平台层的 scrollArea/widget）
    //注意：必须转换为 ScintillaDuiWindowHandle* 的基类指针（多继承下基类子对象有偏移），
    //否则平台层用该指针调用虚函数时会取到错误的虚表。
    wMain = static_cast<ScintillaDuiWindowHandle *>(this);
    //由控件负责整体绘制，不需要 Scintilla 的双缓冲
    view.bufferedDraw = false;
}

ScintillaDui::~ScintillaDui()
{
    FinaliseDui();
}

void ScintillaDui::InitDui()
{
    m_rectangularSelectionModifier = SCMOD_ALT;
    //文档统一使用 UTF-8，简化跨平台文本处理
    WndProc(Message::SetCodePage, SC_CP_UTF8, 0);
    WndProc(Message::SetEOLMode, SC_EOL_CRLF, 0);
    WndProc(Message::SetTechnology, static_cast<uptr_t>(Technology::Default), 0);
    SetCtrlID(1);
}

void ScintillaDui::FinaliseDui()
{
    for (size_t &timerId : m_timerIds) {
        if (timerId != 0) {
            ui::GlobalManager::Instance().Timer().RemoveTimer(timerId);
            timerId = 0;
        }
    }
    haveMouseCapture = false;
    m_pActiveRender = nullptr;
    m_pMeasurementRender.reset();
}

void ScintillaDui::PaintDui(
    ui::IRender *pRender, const ui::UiRect &rcLocalPaint, const ui::UiPoint &ptContentOrigin)
{
    if ((pRender == nullptr) || rcLocalPaint.IsEmpty()) {
        return;
    }
    m_pActiveRender = pRender;

    const PRectangle rcArea(
        static_cast<XYPOSITION>(rcLocalPaint.left),
        static_cast<XYPOSITION>(rcLocalPaint.top),
        static_cast<XYPOSITION>(rcLocalPaint.right),
        static_cast<XYPOSITION>(rcLocalPaint.bottom));

    const PaintState oldPaintState = paintState;
    paintState = PaintState::painting;
    this->rcPaint = rcArea;
    paintingAllText = rcArea.Contains(GetClientRectangle());

    ScintillaDuiSurface surface(this, pRender);
    surface.SetMode(CurrentSurfaceMode());
    //内容区局部坐标 -> 窗口客户区坐标的偏移由 Surface 自行叠加
    surface.SetPositionOffset(ptContentOrigin);
    Paint(&surface, rcArea);
    surface.Release();

    //Scintilla 在绘制过程中可能要求放弃本次绘制并请求整体重绘
    const bool bNeedFullRepaint = (paintState == PaintState::abandoned);
    paintState = oldPaintState;
    m_pActiveRender = nullptr;

    if (bNeedFullRepaint) {
        InvalidateAllDui();
    }
}

std::shared_ptr<ui::IRender> ScintillaDui::GetMeasurementRender()
{
    if (m_pMeasurementRender != nullptr) {
        return m_pMeasurementRender;
    }
    m_pMeasurementRender = CreateOffscreenRender(kMeasurementRenderSize, kMeasurementRenderSize);
    return m_pMeasurementRender;
}

std::shared_ptr<ui::IRender> ScintillaDui::CreateOffscreenRender(int32_t width, int32_t height)
{
    if ((width <= 0) || (height <= 0) || (m_pControl == nullptr)
        || (m_pControl->GetWindow() == nullptr)) {
        return nullptr;
    }
    ui::IRenderFactory *pFactory = ui::GlobalManager::Instance().GetRenderFactory();
    if (pFactory == nullptr) {
        return nullptr;
    }
    std::shared_ptr<ui::IRender> render(pFactory->CreateRender(
        m_pControl->GetWindow()->GetRenderDpi(),
        nullptr,
        ui::RenderBackendType::kRaster_BackendType));
    if ((render != nullptr) && !render->Resize(width, height)) {
        render.reset();
    }
    return render;
}

void ScintillaDui::SetClientRectDui(const ui::UiRect &rcClient)
{
    const PRectangle
        rc(static_cast<XYPOSITION>(rcClient.left),
           static_cast<XYPOSITION>(rcClient.top),
           static_cast<XYPOSITION>(rcClient.right),
           static_cast<XYPOSITION>(rcClient.bottom));
    if (m_rcClient == rc) {
        return;
    }
    m_rcClient = rc;
    //对应 Qt 的 resizeEvent -> sqt->ChangeSize()
    ChangeSize();
    if (m_pControl != nullptr) {
        m_pControl->RequestScrollBarUpdate();
    }
    InvalidateAllDui();
}

ui::UiRect ScintillaDui::GetClientRectDui() const
{
    return ui::UiRect(
        static_cast<int32_t>(m_rcClient.left),
        static_cast<int32_t>(m_rcClient.top),
        static_cast<int32_t>(m_rcClient.right),
        static_cast<int32_t>(m_rcClient.bottom));
}

void ScintillaDui::InvalidateAllDui()
{
    if (m_pControl != nullptr) {
        m_pControl->Invalidate();
    }
}

void ScintillaDui::InvalidateRectDui(const ui::UiRect &rcLocalRect)
{
    if (m_pControl == nullptr) {
        return;
    }
    //Scintilla 传入的是控件局部坐标，Duilib 需要窗口客户区坐标
    const ui::UiRect rcControl = m_pControl->GetRect();
    ui::UiRect rcWindow = rcLocalRect;
    rcWindow.Offset(rcControl.left, rcControl.top);
    m_pControl->InvalidateRect(rcWindow);
}

void ScintillaDui::SetCursorDui(Window::Cursor cursor)
{
    if (m_pControl == nullptr) {
        return;
    }
    switch (cursor) {
    case Window::Cursor::text:
        m_pControl->SetCursorType(ui::CursorType::kCursorIBeam);
        break;
    case Window::Cursor::wait:
        m_pControl->SetCursorType(ui::CursorType::kCursorWait);
        break;
    case Window::Cursor::hand:
        m_pControl->SetCursorType(ui::CursorType::kCursorHand);
        break;
    case Window::Cursor::horizontal:
        m_pControl->SetCursorType(ui::CursorType::kCursorSizeWE);
        break;
    case Window::Cursor::vertical:
        m_pControl->SetCursorType(ui::CursorType::kCursorSizeNS);
        break;
    case Window::Cursor::arrow:
    case Window::Cursor::up:
    case Window::Cursor::reverseArrow:
    case Window::Cursor::invalid:
    default:
        m_pControl->SetCursorType(ui::CursorType::kCursorArrow);
        break;
    }
}

void ScintillaDui::SetFocusStateDui(bool bFocus)
{
    //关键修复：焦点状态必须进入 Scintilla 内部（hasFocus/caret.active），
    //否则插入符永远不会显示，也无法闪烁（对应 Qt focusInEvent/focusOutEvent）。
    SetFocusState(bFocus);
}

void ScintillaDui::SetMouseCaptureDui(bool bOn)
{
    haveMouseCapture = bOn;
}

void ScintillaDui::OnFineTimer(TickReason reason)
{
    TickFor(reason);
}

void ScintillaDui::OnIdleDui()
{
    m_bIdlePosted = false;
    if (!Idle()) {
        SetIdle(false);
    }
}

int32_t ScintillaDui::LogPixelsYDui() const
{
    //对应 Qt 的 QPaintDevice::logicalDpiY()：包含 DPI 缩放，
    //Scintilla 通过 DeviceHeightFont() 把磅值换算为设备像素，字体随 DPI 自动缩放
    const int32_t nScale = DpiScalePercentDui();
    return (96 * nScale + 50) / 100;
}

int32_t ScintillaDui::DpiScalePercentDui() const
{
    if ((m_pControl != nullptr) && (m_pControl->GetWindow() != nullptr)) {
        const uint32_t nScale = m_pControl->Dpi().GetDisplayScaleFactor();
        if (nScale > 0) {
            return static_cast<int32_t>(nScale);
        }
    }
    return 100;
}

void ScintillaDui::OnVerticalScrollBarPosDui(int64_t nPosPixels)
{
    const int32_t nLineHeight = GetLineHeightDui();
    const Sci::Line nNewTopLine = static_cast<Sci::Line>(
        nPosPixels / std::max<int32_t>(1, nLineHeight));
    if (nNewTopLine == topLine) {
        return;
    }
    ScrollTo(nNewTopLine);
}

void ScintillaDui::OnHorizontalScrollBarPosDui(int64_t nPosPixels)
{
    const int32_t nNewOffset = static_cast<int32_t>(std::max<int64_t>(0, nPosPixels));
    if (nNewOffset == xOffset) {
        return;
    }
    HorizontalScrollTo(nNewOffset);
}

int32_t ScintillaDui::GetVerticalScrollRangeDui() const
{
    const int32_t nLineHeight = GetLineHeightDui();
    const Sci::Line nMaxScroll = MaxScrollPos();
    return static_cast<int32_t>(std::max<Sci::Line>(0, nMaxScroll) * nLineHeight);
}

int32_t ScintillaDui::GetHorizontalScrollRangeDui() const
{
    const int32_t nPageWidth = GetTextAreaWidthDui();
    return std::max<int32_t>(0, static_cast<int32_t>(scrollWidth) - nPageWidth);
}

int32_t ScintillaDui::GetVerticalScrollPosDui() const
{
    return static_cast<int32_t>(topLine * GetLineHeightDui());
}

int32_t ScintillaDui::GetHorizontalScrollPosDui() const
{
    return xOffset;
}

intptr_t ScintillaDui::SendSciMessage(unsigned int message, uintptr_t wParam, intptr_t lParam)
{
    return WndProc(static_cast<Message>(message), wParam, lParam);
}

void ScintillaDui::RunScintillaCommand(int command)
{
    Command(command);
}

void ScintillaDui::AddTextDui(std::string_view text)
{
    if (!text.empty()) {
        InsertCharacter(text, CharacterSource::DirectInput);
    }
}

void ScintillaDui::KeyDownDui(Scintilla::Keys key, Scintilla::KeyMod modifiers, bool *pConsumed)
{
    bool consumed = false;
    const intptr_t nRet = KeyDownWithModifiers(key, modifiers, &consumed);
    if (!consumed && (nRet != 0)) {
        consumed = true;
    }
    if (pConsumed != nullptr) {
        *pConsumed = consumed;
    }
}

void ScintillaDui::ButtonDownDui(Point pt, unsigned int curTime, Scintilla::KeyMod modifiers)
{
    ButtonDownWithModifiers(pt, curTime, modifiers);
}

void ScintillaDui::RightButtonDownDui(Point pt, unsigned int curTime, Scintilla::KeyMod modifiers)
{
    RightButtonDownWithModifiers(pt, curTime, modifiers);
}

void ScintillaDui::ButtonMoveDui(Point pt, unsigned int curTime, Scintilla::KeyMod modifiers)
{
    ButtonMoveWithModifiers(pt, curTime, modifiers);
}

void ScintillaDui::ButtonUpDui(Point pt, unsigned int curTime, Scintilla::KeyMod modifiers)
{
    ButtonUpWithModifiers(pt, curTime, modifiers);
}

void ScintillaDui::MouseLeaveDui()
{
    MouseLeave();
}

void ScintillaDui::LineScrollDui(int lines)
{
    if (lines == 0) {
        return;
    }
    ScrollTo(topLine + lines);
}

ui::UiPoint ScintillaDui::GetCaretLocationDui() const
{
    if (pdoc == nullptr) {
        return ui::UiPoint(0, 0);
    }
    const Sci::Position pos = CurrentPosition();
    const Point pt = const_cast<ScintillaDui *>(this)->LocationFromPosition(pos);
    return ui::UiPoint(static_cast<int32_t>(pt.x), static_cast<int32_t>(pt.y));
}

void ScintillaDui::CancelAutoCompleteDui()
{
    if (ac.Active()) {
        AutoCompleteCancel();
    }
}

bool ScintillaDui::ShouldDisplayPopupDui(Point pt) const
{
    return ShouldDisplayPopup(pt);
}

bool ScintillaDui::PointInSelectionDui(Point pt)
{
    return PointInSelection(pt);
}

void ScintillaDui::SetEmptySelectionDui(Point pt)
{
    SetEmptySelection(PositionFromLocation(pt));
}

void ScintillaDui::ContextMenuDui(Point ptScreen)
{
    //ScintillaBase::ContextMenu 会依次调用 popup.CreatePopUp()、
    //AddToPopUp()（由 ScintillaDui::AddToPopUp 收集菜单项）和 popup.Show()
    ContextMenu(ptScreen);
}

void ScintillaDui::DragEnterDui(Point pt)
{
    SetDragPosition(SPositionFromLocation(pt, false, false, UserVirtualSpace()));
}

void ScintillaDui::DragMoveDui(Point pt)
{
    SetDragPosition(SPositionFromLocation(pt, false, false, UserVirtualSpace()));
}

void ScintillaDui::DragLeaveDui()
{
    SetDragPosition(SelectionPosition(Sci::invalidPosition));
}

void ScintillaDui::DropTextDui(Point pt, std::string_view text, bool bMove)
{
    const SelectionPosition movePos = SPositionFromLocation(pt, false, false, UserVirtualSpace());
    DropAt(movePos, text, bMove, false);
}

PRectangle ScintillaDui::GetWindowPositionHandle() const
{
    if ((m_pControl == nullptr) || (m_pControl->GetWindow() == nullptr)) {
        return PRectangle();
    }
    const ui::UiRect rcClient = m_pControl->GetRect();
    ui::UiPoint ptTopLeft(rcClient.left, rcClient.top);
    ui::UiPoint ptBottomRight(rcClient.right, rcClient.bottom);
    ui::Window *pWindow = m_pControl->GetWindow();
    pWindow->ClientToScreen(ptTopLeft);
    pWindow->ClientToScreen(ptBottomRight);
    return PRectangle::FromInts(ptTopLeft.x, ptTopLeft.y, ptBottomRight.x, ptBottomRight.y);
}

void ScintillaDui::SetWindowPositionHandle(PRectangle /*rc*/)
{
    //编辑器窗口的位置由 Duilib 布局管理，不接受 Scintilla 的移动请求
}

void ScintillaDui::ShowWindowHandle(bool /*bShow*/)
{
    //编辑器窗口的显示状态由 Duilib 管理
}

void ScintillaDui::DestroyWindowHandle() noexcept
{
    //编辑器窗口的生命周期由 Duilib 管理
}

void ScintillaDui::InvalidateWindowAllHandle()
{
    InvalidateAllDui();
}

void ScintillaDui::InvalidateWindowRectHandle(PRectangle rc)
{
    InvalidateRectDui(
        ui::UiRect(
            static_cast<int32_t>(rc.left),
            static_cast<int32_t>(rc.top),
            static_cast<int32_t>(rc.right),
            static_cast<int32_t>(rc.bottom)));
}

void ScintillaDui::SetWindowCursorHandle(Window::Cursor cursor)
{
    SetCursorDui(cursor);
}

PRectangle ScintillaDui::GetMonitorRectHandle(Point /*pt*/)
{
    //返回屏幕的可用范围（DUI 控件不直接管理监视器信息）
    return PRectangle(0, 0, 4096, 4096);
}

ui::UiPoint ScintillaDui::ClientToScreenHandle(const ui::UiPoint &ptClient) const
{
    ui::UiPoint ptScreen(ptClient);
    if ((m_pControl == nullptr) || (m_pControl->GetWindow() == nullptr)) {
        return ptScreen;
    }
    //内容区局部坐标 -> 窗口客户区坐标 -> 屏幕坐标
    const ui::UiRect rcArea = m_pControl->GetTextAreaRect();
    ptScreen.x += rcArea.left;
    ptScreen.y += rcArea.top;
    m_pControl->GetWindow()->ClientToScreen(ptScreen);
    return ptScreen;
}

void ScintillaDui::Finalise()
{
    ScintillaBase::Finalise();
}

PRectangle ScintillaDui::GetClientRectangle() const
{
    return m_rcClient;
}

bool ScintillaDui::DragThreshold(Point ptStart, Point ptNow)
{
    //对应 Qt 的 QApplication::startDragDistance()
    const XYPOSITION dx = std::abs(ptStart.x - ptNow.x);
    const XYPOSITION dy = std::abs(ptStart.y - ptNow.y);
    return (dx > 4.0) || (dy > 4.0);
}

bool ScintillaDui::ValidCodePage(int codePage) const
{
    return (codePage == 0) || (codePage == SC_CP_UTF8);
}

std::string ScintillaDui::UTF8FromEncoded(std::string_view encoded) const
{
    //只支持 UTF-8 文档编码，无需转码
    return std::string(encoded);
}

std::string ScintillaDui::EncodedFromUTF8(std::string_view utf8) const
{
    return std::string(utf8);
}

void ScintillaDui::ScrollText(Sci::Line /*linesToMove*/)
{
    //DUI 不做像素级滚动拷贝，直接整体重绘（与 Editor::ScrollText 的默认行为一致）
    Redraw();
}

void ScintillaDui::SetVerticalScrollPos()
{
    Editor::SetVerticalScrollPos();
    if (m_pControl != nullptr) {
        m_pControl->RequestScrollBarUpdate();
    }
}

void ScintillaDui::SetHorizontalScrollPos()
{
    if (m_pControl != nullptr) {
        m_pControl->RequestScrollBarUpdate();
    }
}

bool ScintillaDui::ModifyScrollBars(Sci::Line nMax, Sci::Line nPage)
{
    //对应 Qt 的 ScintillaQt::ModifyScrollBars：把范围与页大小同步给滚动条
    bool modified = false;

    const int vNewPage = static_cast<int>(nPage);
    const int vNewMax = static_cast<int>(std::max<Sci::Line>(0, nMax - nPage + 1));
    if ((vMax != vNewMax) || (vPage != vNewPage)) {
        vMax = vNewMax;
        vPage = vNewPage;
        modified = true;
    }

    const int hNewPage = static_cast<int>(m_rcClient.Width());
    const int hNewMax = std::max<int>(0, static_cast<int>(scrollWidth) - hNewPage);
    const int charWidth = static_cast<int>(vs.styles[STYLE_DEFAULT].aveCharWidth);
    if ((hMax != hNewMax) || (hPage != hNewPage) || (hCharWidth != charWidth)) {
        hMax = hNewMax;
        hPage = hNewPage;
        hCharWidth = charWidth;
        modified = true;
    }

    if (m_pControl != nullptr) {
        m_pControl->RequestScrollBarUpdate();
    }
    return modified;
}

void ScintillaDui::ReconfigureScrollBars()
{
    if (m_pControl != nullptr) {
        m_pControl->RequestScrollBarUpdate();
    }
}

void ScintillaDui::Copy()
{
    if (!sel.Empty()) {
        SelectionText st;
        CopySelectionRange(&st);
        CopyToClipboard(st);
    }
}

void ScintillaDui::CopyToClipboard(const SelectionText &selectedText)
{
    const DStringA text(selectedText.Data(), selectedText.Length());
    ui::Clipboard::SetClipboardText(text);
}

void ScintillaDui::Paste()
{
    DStringA text;
    if (!ui::Clipboard::GetClipboardText(text) || text.empty()) {
        return;
    }
    SelectionText selectedText;
    selectedText.Copy(text, SC_CP_UTF8, CharacterSet::Ansi, false, false);
    UndoGroup ug(pdoc);
    ClearSelection(multiPasteMode == MultiPaste::Each);
    InsertPasteShape(selectedText.AsView(), PasteShape::stream);
    EnsureCaretVisible();
}

void ScintillaDui::ClaimSelection()
{
    //DUI 平台没有 X11 的 primary selection，无需处理
}

void ScintillaDui::NotifyChange()
{
    if (m_pControl != nullptr) {
        m_pControl->OnScintillaTextChanged();
    }
}

void ScintillaDui::NotifyFocus(bool focus)
{
    //Editor::NotifyFocus 会向应用发送 FocusIn/FocusOut 通知
    Editor::NotifyFocus(focus);
}

void ScintillaDui::NotifyParent(NotificationData scn)
{
    scn.nmhdr.hwndFrom = wMain.GetID();
    scn.nmhdr.idFrom = GetCtrlID();
    if (m_pControl != nullptr) {
        m_pControl->OnScintillaNotify(*reinterpret_cast<const SCNotification *>(&scn));
    }
}

bool ScintillaDui::FineTickerRunning(TickReason reason)
{
    const size_t index = static_cast<size_t>(reason);
    return (index < m_timerIds.size()) && (m_timerIds[index] != 0);
}

void ScintillaDui::FineTickerStart(TickReason reason, int millis, int /*tolerance*/)
{
    FineTickerCancel(reason);
    const size_t index = static_cast<size_t>(reason);
    if ((index >= m_timerIds.size()) || (m_pControl == nullptr)) {
        return;
    }
    std::weak_ptr<ui::WeakFlag> weakFlag = m_pControl->GetWeakFlag();
    const size_t timerId = ui::GlobalManager::Instance().Timer().AddTimer(
        weakFlag,
        [this, reason]() { OnFineTimer(reason); },
        static_cast<uint32_t>(std::max(1, millis)));
    m_timerIds[index] = timerId;
}

void ScintillaDui::FineTickerCancel(TickReason reason)
{
    const size_t index = static_cast<size_t>(reason);
    if ((index < m_timerIds.size()) && (m_timerIds[index] != 0)) {
        ui::GlobalManager::Instance().Timer().RemoveTimer(m_timerIds[index]);
        m_timerIds[index] = 0;
    }
}

bool ScintillaDui::SetIdle(bool on)
{
    if (!on) {
        m_bIdlePosted = false;
        return true;
    }
    if (m_bIdlePosted || (m_pControl == nullptr)) {
        return true;
    }
    m_bIdlePosted = true;
    std::weak_ptr<ui::WeakFlag> weakFlag = m_pControl->GetWeakFlag();
    ui::GlobalManager::Instance().Thread().PostTask(ui::kThreadUI, [weakFlag, this]() {
        if (weakFlag.expired()) {
            //控件已销毁，不再访问 this
            return;
        }
        OnIdleDui();
    });
    return true;
}

void ScintillaDui::QueueIdleWork(WorkItems items, Sci::Position upTo)
{
    Editor::QueueIdleWork(items, upTo);
    SetIdle(true);
}

void ScintillaDui::IdleWork()
{
    Editor::IdleWork();
}

void ScintillaDui::SetMouseCapture(bool on)
{
    if (!mouseDownCaptures) {
        return;
    }
    haveMouseCapture = on;
    if (m_pControl != nullptr) {
        m_pControl->SetMouseCaptureFromScintilla(on);
    }
}

bool ScintillaDui::HaveMouseCapture()
{
    return haveMouseCapture;
}

void ScintillaDui::StartDrag()
{
    //DUI 平台不实现跨进程拖出（需要额外的 IDropSource/IDataObject 实现）；
    //控件内部的文本拖动（选中后拖动改变位置）仍由 Scintilla 内部逻辑完成。
    inDragDrop = DragDrop::dragging;
    SetDragPosition(SelectionPosition(Sci::invalidPosition));
}

void ScintillaDui::CreateCallTipWindow(PRectangle rc)
{
    if (m_pControl == nullptr) {
        return;
    }
    //创建（或复用）CallTip 的 DUI 弹出窗口，并把它作为 Scintilla 的平台窗口
    ScintillaDuiCallTip *pCallTip = m_pControl->GetOrCreateCallTip(this);
    if (pCallTip == nullptr) {
        return;
    }
    ct.wCallTip = static_cast<ScintillaDuiWindowHandle *>(pCallTip);
    //rc 是编辑器内容区的局部坐标，交给控件转换为屏幕坐标后显示
    m_pControl->CreateCallTipWindowDui(
        ui::UiRect(
            static_cast<int32_t>(rc.left),
            static_cast<int32_t>(rc.top),
            static_cast<int32_t>(rc.right),
            static_cast<int32_t>(rc.bottom)));
}

void ScintillaDui::AddToPopUp(const char *label, int cmd, bool enabled)
{
    if (m_pControl != nullptr) {
        m_pControl->AddToPopupMenu(label, cmd, enabled);
    }
}

sptr_t ScintillaDui::WndProc(Message iMessage, uptr_t wParam, sptr_t lParam)
{
    try {
        switch (iMessage) {
        case Message::GrabFocus:
            if (m_pControl != nullptr) {
                m_pControl->SetFocus();
            }
            return 0;
        case Message::GetDirectFunction:
            return reinterpret_cast<sptr_t>(DirectFunction);
        case Message::GetDirectStatusFunction:
            return reinterpret_cast<sptr_t>(DirectStatusFunction);
        case Message::GetDirectPointer:
            return reinterpret_cast<sptr_t>(this);
        case Message::SetRectangularSelectionModifier:
            m_rectangularSelectionModifier = static_cast<int>(wParam);
            return 0;
        case Message::GetRectangularSelectionModifier:
            return m_rectangularSelectionModifier;
        case Message::SetIMEInteraction:
            //DUI 使用系统输入法窗口（不支持 Qt 那种 inline IME）
            return 0;
        default:
            return ScintillaBase::WndProc(iMessage, wParam, lParam);
        }
    } catch (std::bad_alloc &) {
        errorStatus = Status::BadAlloc;
    } catch (Failure &failure) {
        errorStatus = failure.status;
    } catch (...) {
        errorStatus = Status::Failure;
    }
    return 0;
}

sptr_t ScintillaDui::DefWndProc(Message /*iMessage*/, uptr_t /*wParam*/, sptr_t /*lParam*/)
{
    return 0;
}

std::unique_ptr<CaseFolder> ScintillaDui::CaseFolderForEncoding()
{
    if (pdoc->dbcsCodePage == SC_CP_UTF8) {
        return std::make_unique<CaseFolderUnicode>();
    }
    return std::make_unique<CaseFolderTable>();
}

std::string ScintillaDui::CaseMapString(const std::string &text, CaseMapping caseMapping)
{
    if (text.empty() || (caseMapping == CaseMapping::same)) {
        return text;
    }
    std::string result(text.length() * maxExpansionCaseConversion, 0);
    const size_t length = CaseConvertString(
        &result[0],
        result.length(),
        text.c_str(),
        text.length(),
        (caseMapping == CaseMapping::upper) ? CaseConversion::upper : CaseConversion::lower);
    result.resize(length);
    return result;
}

sptr_t ScintillaDui::DirectFunction(sptr_t ptr, unsigned int iMessage, uptr_t wParam, sptr_t lParam)
{
    ScintillaDui *pSci = reinterpret_cast<ScintillaDui *>(ptr);
    return pSci != nullptr ? pSci->WndProc(static_cast<Message>(iMessage), wParam, lParam) : 0;
}

sptr_t ScintillaDui::DirectStatusFunction(
    sptr_t ptr, unsigned int iMessage, uptr_t wParam, sptr_t lParam, int *pStatus)
{
    ScintillaDui *pSci = reinterpret_cast<ScintillaDui *>(ptr);
    const sptr_t result = pSci != nullptr
                              ? pSci->WndProc(static_cast<Message>(iMessage), wParam, lParam)
                              : 0;
    if (pStatus != nullptr) {
        *pStatus = (pSci != nullptr) ? static_cast<int>(pSci->errorStatus) : 0;
    }
    return result;
}

} // namespace duilib::ext::scintilla
