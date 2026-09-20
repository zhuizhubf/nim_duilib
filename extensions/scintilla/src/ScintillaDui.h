#ifndef DUILIB_EXT_SCINTILLA_SCINTILLA_DUI_H_
#define DUILIB_EXT_SCINTILLA_SCINTILLA_DUI_H_

#include "duilib/Core/Callback.h"

#include "ScintillaDuiWindowHandle.h"

#include <algorithm>
#include <array>
#include <cassert>
#include <cctype>
#include <cmath>
#include <cstddef>
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <ctime>
#include <map>
#include <memory>
#include <new>
#include <optional>
#include <set>
#include <stdexcept>
#include <string>
#include <string_view>
#include <vector>

// clang-format off
//Scintilla 头文件有包含顺序要求，禁止自动排序（ScintillaTypes 必须最先包含）
#include "ScintillaTypes.h"
#include "ScintillaMessages.h"
#include "ScintillaStructures.h"
#include "Scintilla.h"
#include "Debugging.h"
#include "Geometry.h"
#include "Platform.h"
#include "ILoader.h"
#include "ILexer.h"
#include "CharacterCategoryMap.h"
#include "Position.h"
#include "UniqueString.h"
#include "SplitVector.h"
#include "Partitioning.h"
#include "RunStyles.h"
#include "ContractionState.h"
#include "CellBuffer.h"
#include "AutoComplete.h"
#include "CallTip.h"
#include "KeyMap.h"
#include "Indicator.h"
#include "LineMarker.h"
#include "Style.h"
#include "ViewStyle.h"
#include "CharClassify.h"
#include "Decoration.h"
#include "CaseConvert.h"
#include "CaseFolder.h"
#include "Document.h"
#include "Selection.h"
#include "PositionCache.h"
#include "EditModel.h"
#include "MarginView.h"
#include "EditView.h"
#include "Editor.h"
#include "ScintillaBase.h"
// clang-format on

namespace ui {
class IRender;
class UiPoint;
class UiRect;
class UiFont;
class UiColor;
} // namespace ui

namespace duilib::ext::scintilla {

class ScintillaControl;

/** Scintilla core 的 DUI 平台子类（对应 Qt 平台层的 ScintillaQt）。
 *
 * 该类继承 ScintillaBase，把平台相关能力映射到 Duilib：
 *   ui::IRender / ui::ITextShaper / ui::TimerManager / ui::Clipboard / ui::Window。
 * 事件顺序、焦点状态、滚动同步均与 Qt 平台层 ScintillaQt 保持一致。
 */
class ScintillaDui final : public Scintilla::Internal::ScintillaBase,
                           public ui::SupportWeakCallback,
                           public ScintillaDuiWindowHandle
{
public:
    explicit ScintillaDui(ScintillaControl *pControl);
    ScintillaDui(const ScintillaDui &) = delete;
    ScintillaDui &operator=(const ScintillaDui &) = delete;
    ~ScintillaDui() override;

    /** 初始化/清理（对应 ScintillaQt::Init/Finalise） */
    void InitDui();
    void FinaliseDui();

    ScintillaControl *GetControl() const noexcept { return m_pControl; }

    //---- 绘制 ----
    /** 绘制编辑器。
     * @param [in] pRender 绘制目标
     * @param [in] rcLocalPaint 需要绘制的区域（编辑器内容区局部坐标）
     * @param [in] ptContentOrigin 编辑器内容区在窗口客户区中的原点
     */
    void PaintDui(
        ui::IRender *pRender, const ui::UiRect &rcLocalPaint, const ui::UiPoint &ptContentOrigin);
    /** 度量用的离屏 Render（对应 Qt 平台层的 QPaintDevice） */
    std::shared_ptr<ui::IRender> GetMeasurementRender();
    std::shared_ptr<ui::IRender> CreateOffscreenRender(int32_t width, int32_t height);

    //---- 客户区与重绘 ----
    void SetClientRectDui(const ui::UiRect &rcClient);
    ui::UiRect GetClientRectDui() const;
    void InvalidateAllDui();
    void InvalidateRectDui(const ui::UiRect &rcLocalRect);
    void SetCursorDui(Scintilla::Internal::Window::Cursor cursor);

    //---- 焦点 ----
    /** 焦点变化（对应 Qt 的 focusInEvent/focusOutEvent 中的 sqt->SetFocusState） */
    void SetFocusStateDui(bool bFocus);
    bool IsFocusedDui() const noexcept { return hasFocus; }

    //---- 鼠标捕获 ----
    void SetMouseCaptureDui(bool bOn);
    bool HaveMouseCaptureDui() const noexcept { return haveMouseCapture; }

    //---- 定时器与空闲任务 ----
    void OnFineTimer(Scintilla::Internal::Editor::TickReason reason);
    void OnIdleDui();

    //---- DPI ----
    /** 当前设备的每英寸像素数（已包含 DPI 缩放，对应 Qt 的 logicalDpiY） */
    int32_t LogPixelsYDui() const;
    /** 当前 DPI 缩放百分比（100 表示无缩放） */
    int32_t DpiScalePercentDui() const;

    //---- 滚动条同步 ----
    /** 垂直滚动条位置变化（单位为像素） */
    void OnVerticalScrollBarPosDui(int64_t nPosPixels);
    /** 水平滚动条位置变化（单位为像素） */
    void OnHorizontalScrollBarPosDui(int64_t nPosPixels);
    int32_t GetVerticalScrollRangeDui() const;
    int32_t GetHorizontalScrollRangeDui() const;
    int32_t GetVerticalScrollPosDui() const;
    int32_t GetHorizontalScrollPosDui() const;
    bool IsVerticalScrollBarVisibleDui() const noexcept { return verticalScrollBarVisible; }
    bool IsHorizontalScrollBarVisibleDui() const noexcept { return horizontalScrollBarVisible; }
    bool IsWrappingDui() const noexcept { return Wrapping(); }
    int32_t GetLineHeightDui() const noexcept
    {
        return static_cast<int32_t>(std::max<Scintilla::Internal::XYPOSITION>(1, vs.lineHeight));
    }
    int32_t GetLinesOnScreenDui() const noexcept
    {
        return static_cast<int32_t>(m_rcClient.Height() / (vs.lineHeight > 0 ? vs.lineHeight : 1));
    }
    int32_t GetTextAreaWidthDui() const noexcept
    {
        return static_cast<int32_t>(m_rcClient.Width());
    }

    //---- Scintilla 命令入口 ----
    intptr_t SendSciMessage(unsigned int message, uintptr_t wParam, intptr_t lParam);
    void RunScintillaCommand(int command);
    void AddTextDui(std::string_view text);
    void KeyDownDui(Scintilla::Keys key, Scintilla::KeyMod modifiers, bool *pConsumed);
    void ButtonDownDui(
        Scintilla::Internal::Point pt, unsigned int curTime, Scintilla::KeyMod modifiers);
    void RightButtonDownDui(
        Scintilla::Internal::Point pt, unsigned int curTime, Scintilla::KeyMod modifiers);
    void ButtonMoveDui(
        Scintilla::Internal::Point pt, unsigned int curTime, Scintilla::KeyMod modifiers);
    void ButtonUpDui(
        Scintilla::Internal::Point pt, unsigned int curTime, Scintilla::KeyMod modifiers);
    void MouseLeaveDui();
    void LineScrollDui(int lines);
    void EnsureCaretVisibleDui() { EnsureCaretVisible(); }

    /** 当前插入点位置（用于输入法候选框定位），返回控件局部坐标 */
    ui::UiPoint GetCaretLocationDui() const;

    /** CallTip 对象（用于在 DUI 弹出窗口中绘制） */
    Scintilla::Internal::CallTip *GetCallTipDui() noexcept { return &ct; }

    /** 样式缓存失效（字体/DPI 变化后强制重新实现字体） */
    void InvalidateStyleRedrawDui() { InvalidateStyleRedraw(); }

    //---- 自动补全与右键菜单（对应 Qt 的 ListBoxImpl / AddToPopUp） ----
    void CancelAutoCompleteDui();
    bool ShouldDisplayPopupDui(Scintilla::Internal::Point pt) const;
    bool PointInSelectionDui(Scintilla::Internal::Point pt);
    void SetEmptySelectionDui(Scintilla::Internal::Point pt);
    void ContextMenuDui(Scintilla::Internal::Point ptScreen);

    //---- 拖放 ----
    void DragEnterDui(Scintilla::Internal::Point pt);
    void DragMoveDui(Scintilla::Internal::Point pt);
    void DragLeaveDui();
    void DropTextDui(Scintilla::Internal::Point pt, std::string_view text, bool bMove);

    //---- ScintillaDuiWindowHandle（编辑器窗口的实现） ----
    virtual Scintilla::Internal::PRectangle GetWindowPositionHandle() const override;
    virtual void SetWindowPositionHandle(Scintilla::Internal::PRectangle rc) override;
    virtual void ShowWindowHandle(bool bShow) override;
    virtual void DestroyWindowHandle() noexcept override;
    virtual void InvalidateWindowAllHandle() override;
    virtual void InvalidateWindowRectHandle(Scintilla::Internal::PRectangle rc) override;
    virtual void SetWindowCursorHandle(Scintilla::Internal::Window::Cursor cursor) override;
    virtual Scintilla::Internal::PRectangle GetMonitorRectHandle(
        Scintilla::Internal::Point pt) override;
    virtual ui::UiPoint ClientToScreenHandle(const ui::UiPoint &ptClient) const override;

private:
    // Editor/ScintillaBase overrides
    void Finalise() override;
    Scintilla::Internal::PRectangle GetClientRectangle() const override;
    bool DragThreshold(Scintilla::Internal::Point ptStart, Scintilla::Internal::Point ptNow) override;
    bool ValidCodePage(int codePage) const override;
    std::string UTF8FromEncoded(std::string_view encoded) const override;
    std::string EncodedFromUTF8(std::string_view utf8) const override;

    void ScrollText(Sci::Line linesToMove) override;
    void SetVerticalScrollPos() override;
    void SetHorizontalScrollPos() override;
    bool ModifyScrollBars(Sci::Line nMax, Sci::Line nPage) override;
    void ReconfigureScrollBars() override;

    void Copy() override;
    void CopyToClipboard(const Scintilla::Internal::SelectionText &selectedText) override;
    void Paste() override;
    void ClaimSelection() override;

    void NotifyChange() override;
    void NotifyFocus(bool focus) override;
    void NotifyParent(Scintilla::NotificationData scn) override;

    bool FineTickerRunning(Scintilla::Internal::Editor::TickReason reason) override;
    void FineTickerStart(
        Scintilla::Internal::Editor::TickReason reason, int millis, int tolerance) override;
    void FineTickerCancel(Scintilla::Internal::Editor::TickReason reason) override;
    bool SetIdle(bool on) override;
    void QueueIdleWork(Scintilla::Internal::WorkItems items, ::Sci::Position upTo) override;
    void IdleWork() override;

    void SetMouseCapture(bool on) override;
    bool HaveMouseCapture() override;
    void StartDrag() override;

    void CreateCallTipWindow(Scintilla::Internal::PRectangle rc) override;
    void AddToPopUp(const char *label, int cmd, bool enabled) override;

    Scintilla::sptr_t WndProc(
        Scintilla::Message iMessage, Scintilla::uptr_t wParam, Scintilla::sptr_t lParam) override;
    Scintilla::sptr_t DefWndProc(
        Scintilla::Message iMessage, Scintilla::uptr_t wParam, Scintilla::sptr_t lParam) override;

    std::unique_ptr<Scintilla::Internal::CaseFolder> CaseFolderForEncoding() override;
    std::string CaseMapString(
        const std::string &text, Scintilla::Internal::Editor::CaseMapping caseMapping) override;

    static Scintilla::sptr_t DirectFunction(
        Scintilla::sptr_t ptr,
        unsigned int iMessage,
        Scintilla::uptr_t wParam,
        Scintilla::sptr_t lParam);
    static Scintilla::sptr_t DirectStatusFunction(
        Scintilla::sptr_t ptr,
        unsigned int iMessage,
        Scintilla::uptr_t wParam,
        Scintilla::sptr_t lParam,
        int *pStatus);

private:
    ScintillaControl *m_pControl = nullptr;
    ui::IRender *m_pActiveRender = nullptr;
    std::shared_ptr<ui::IRender> m_pMeasurementRender;
    bool haveMouseCapture = false;
    //滚动条的范围与页大小缓存（对应 ScintillaQt::vMax/vPage/hMax/hPage）
    int vMax = 0;
    int hMax = 0;
    int vPage = 0;
    int hPage = 0;
    int hCharWidth = 0;
    int m_rectangularSelectionModifier = SCMOD_ALT;
    std::array<size_t, static_cast<size_t>(Scintilla::Internal::Editor::TickReason::dwell) + 1>
        m_timerIds{};
    bool m_bIdlePosted = false;
    Scintilla::Internal::PRectangle m_rcClient;
};

} // namespace duilib::ext::scintilla

#endif // DUILIB_EXT_SCINTILLA_SCINTILLA_DUI_H_
