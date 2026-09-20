#ifndef DUILIB_EXT_SCINTILLA_SCINTILLA_CONTROL_H_
#define DUILIB_EXT_SCINTILLA_SCINTILLA_CONTROL_H_

#include "duilib/Core/Box.h"
#include "duilib/Core/UiColor.h"
#include "duilib/Core/UiFont.h"
#include "duilib/Core/UiPoint.h"
#include "duilib/Core/UiRect.h"

#include <cstdint>
#include <functional>
#include <memory>
#include <string>
#include <utility>
#include <vector>

struct SCNotification;

namespace ui {
class ScrollBar;
} // namespace ui

namespace duilib::ext::scintilla {

class ScintillaDui;
class ScintillaDuiCallTip;

/** DUI 原生 Scintilla 编辑控件（对应 Qt 平台层的 ScintillaEditBase）。
 *
 * 控件不使用任何原生子窗口：
 *   - Scintilla 的绘制通过 ui::IRender 完成；
 *   - 内容区由 ui::Box 承载，右侧/底部是两个 ui::ScrollBar；
 *   - 输入法、剪贴板、定时器、菜单均使用 Duilib 的公共能力。
 *
 * XML 中的控件类型名为 "ScintillaControl"。
 */
class ScintillaControl final : public ui::Box
{
    typedef ui::Box BaseClass;

public:
    explicit ScintillaControl(ui::Window *pWindow);
    ScintillaControl(const ScintillaControl &) = delete;
    ScintillaControl &operator=(const ScintillaControl &) = delete;
    ~ScintillaControl() override;

public:
    virtual DString GetType() const override;
    virtual void OnInit() override;
    virtual void SetAttribute(const DString &strName, const DString &strValue) override;
    virtual void SetPos(ui::UiRect rc) override;
    virtual void Paint(ui::IRender *pRender, const ui::UiRect &rcPaint) override;
    virtual void PaintChild(ui::IRender *pRender, const ui::UiRect &rcPaint) override;
    virtual ui::Control *FindControl(
        ui::FINDCONTROLPROC Proc,
        void *pProcData,
        uint32_t uFlags,
        const ui::UiPoint &ptMouse = ui::UiPoint(),
        const ui::UiPoint &scrollPos = ui::UiPoint()) override;
    virtual void SetParent(ui::Box *pParent) override;
    virtual void SetWindow(ui::Window *pWindow) override;
    virtual void ChangeDpiScale(uint32_t nOldDpiScale, uint32_t nNewDpiScale) override;

public:
    using TextChangedCallback = std::function<void()>;
    using SavePointCallback = std::function<void(bool)>;
    using NotifyCallback = std::function<void(const SCNotification &)>;

    /** 获取/设置 UTF-8 文本 */
    std::string GetTextUTF8() const;
    void SetTextUTF8(const std::string &text);

    /** 便捷接口：使用 Duilib 字符串（内部转换为 UTF-8） */
    DString GetText() const;
    void SetText(const DString &text);

    void SetReadOnly(bool bReadOnly);
    bool IsReadOnly() const;

    /** 设置 Lexilla 词法器名称，例如 "cpp"、"python"、"json"、"xml"。 */
    void SetLexer(const std::string &lexerName);
    std::string GetLexer() const;

    void SetTabWidth(int32_t nTabWidth);
    int32_t GetTabWidth() const;
    void SetUseTabs(bool bUseTabs);
    bool IsUseTabs() const;
    void SetWordWrap(bool bWordWrap);
    bool IsWordWrap() const;
    void SetLineNumbersVisible(bool bVisible);
    bool IsLineNumbersVisible() const;
    void SetFoldingVisible(bool bVisible);
    bool IsFoldingVisible() const;

    /** 缩放：0 表示 100%，与 SCI_SETZOOM 的取值一致 */
    void SetZoom(int32_t nZoom);
    int32_t GetZoom() const;

    /** 默认样式（前景色、背景色、字体）。
     *  字体大小与 Duilib 的 UiFont 语义一致，为当前 DPI 下的设备像素值。
     */
    void SetDefaultStyle(const ui::UiColor &fore, const ui::UiColor &back, const ui::UiFont &font);

    /** 单个 Scintilla 样式的颜色/字形设置。 */
    void SetStyle(
        int32_t styleId,
        const ui::UiColor &fore,
        const ui::UiColor &back,
        bool bBold = false,
        bool bItalic = false);

    bool IsModified() const;
    void SetSavePoint();
    void Undo();
    void Redo();
    bool CanUndo() const;
    bool CanRedo() const;
    void Cut();
    void Copy();
    void Paste();
    void SelectAll();

    /** 自动补全列表（对应 SCI_AUTOCSHOW / SCI_AUTOCCANCEL） */
    void ShowAutoComplete(bool bShow, const std::string &itemList = std::string());

    /** 直接发送 Scintilla SCI_* 消息。 */
    intptr_t SendSciMessage(unsigned int message, uintptr_t wParam = 0, intptr_t lParam = 0) const;

    void SetTextChangedCallback(const TextChangedCallback &callback);
    void SetSavePointCallback(const SavePointCallback &callback);
    void SetNotifyCallback(const NotifyCallback &callback);

public:
    /** 供扩展内部平台层调用（下列接口与 ScintillaDui 配合工作） */
    void OnScintillaTextChanged();
    void OnScintillaSavePoint(bool bAtSavePoint);
    void OnScintillaNotify(const SCNotification &notification);
    void SetMouseCaptureFromScintilla(bool bOn);
    /** 把 Scintilla 的滚动状态同步到 ui::ScrollBar */
    void UpdateScrollBarsFromScintilla();
    /** 延迟同步滚动条状态（在 Scintilla 的绘制/通知过程中调用，避免重入） */
    void RequestScrollBarUpdate();
    /** 滚动条位置变化（由内部滚动条回调触发） */
    void OnScrollBarPosChanged(bool bVertical, int64_t nPos);
    /** CallTip 窗口创建请求（控件内容区局部坐标） */
    void CreateCallTipWindowDui(const ui::UiRect &rc);
    /** 创建（或复用）CallTip 窗口对象 */
    ScintillaDuiCallTip *GetOrCreateCallTip(ScintillaDui *pSci);
    /** 弹出右键菜单（屏幕坐标） */
    void ShowPopupMenuDui(const ui::UiPoint &ptScreen);
    /** 收集右键菜单项（由 ScintillaBase::AddToPopUp 调用） */
    void AddPopupMenuItem(const std::string &label, int cmd, bool enabled);
    /** 收集右键菜单项（const char* 版本，供平台层调用） */
    void AddToPopupMenu(const char *label, int cmd, bool enabled);
    /** 执行菜单命令 */
    void ExecutePopupCommand(int32_t nCommand);

protected:
    virtual bool ButtonDown(const ui::EventArgs &msg) override;
    virtual bool ButtonDoubleClick(const ui::EventArgs &msg) override;
    virtual bool ButtonUp(const ui::EventArgs &msg) override;
    virtual bool RButtonDown(const ui::EventArgs &msg) override;
    virtual bool RButtonUp(const ui::EventArgs &msg) override;
    virtual bool RButtonDoubleClick(const ui::EventArgs &msg) override;
    virtual bool MouseMove(const ui::EventArgs &msg) override;
    virtual bool MouseLeave(const ui::EventArgs &msg) override;
    virtual bool MouseWheel(const ui::EventArgs &msg) override;
    virtual bool OnChar(const ui::EventArgs &msg) override;
    virtual bool OnKeyDown(const ui::EventArgs &msg) override;
    virtual bool OnSetFocus(const ui::EventArgs &msg) override;
    virtual bool OnKillFocus(const ui::EventArgs &msg) override;
    virtual bool OnCaptureChanged(const ui::EventArgs &msg) override;
    virtual bool OnImeStartComposition(const ui::EventArgs &msg) override;
    virtual bool OnImeComposition(const ui::EventArgs &msg) override;
    virtual bool OnImeEndComposition(const ui::EventArgs &msg) override;
    virtual void OnSetVisible(bool bChanged) override;

private:
    friend class ScintillaDui;

    /** 鼠标事件坐标：窗口客户区坐标 -> 编辑器内容区局部坐标 */
    ui::UiPoint GetScintillaPoint(const ui::EventArgs &msg) const;
    /** 编辑器内容区（窗口客户区坐标，不含滚动条） */
    ui::UiRect GetTextAreaRect() const;
    /** 当前显示缩放百分比（100 表示无缩放；未关联窗口时返回 100） */
    int32_t GetDisplayScalePercent() const;
    /** 按当前显示缩放比例换算数值 */
    int32_t ScaledValue(int32_t nValue) const;
    /** 依据滚动条可见性计算内容区与滚动条位置 */
    void UpdateTextArea();
    /** 把 Scintilla 的滚动位置/范围同步到两个滚动条 */
    void SyncScrollBarState();
    void CreateScrollBars();
    /** 依据 line_numbers/folding 属性刷新边距设置 */
    void SetMarginVisible();
    /** 定义折叠标记的外观 */
    void DefineFoldingMarkers();
    void SetCaretLineVisible(bool bVisible);
    /** 把默认样式（字体、颜色）应用到 Scintilla */
    void ApplyDefaultStyleToScintilla();
    /** 弹出右键菜单（对应 Qt 的 contextMenuEvent） */
    void ShowContextMenu(const ui::EventArgs &msg);
    /** 更新输入法候选框位置 */
    void UpdateImePosition();
    void ClosePopupWindows();
    void DisposePopupWindows();

private:
    std::unique_ptr<ScintillaDui> m_pSci;
    std::unique_ptr<ui::ScrollBar> m_pVScrollBar;
    std::unique_ptr<ui::ScrollBar> m_pHScrollBar;
    std::unique_ptr<ScintillaDuiCallTip> m_pCallTip;
    std::vector<std::pair<std::string, int>> m_popupMenuItems;
    TextChangedCallback m_textChangedCallback;
    SavePointCallback m_savePointCallback;
    NotifyCallback m_notifyCallback;
    std::string m_lexerName;
    DString m_initialText;
    ui::UiFont m_fontInfo;
    ui::UiColor m_defaultFore;
    ui::UiColor m_defaultBack;
    int32_t m_baseFontSizeDip = 0;
    bool m_hasDefaultStyle = false;
    bool m_bMouseCaptured = false;
    bool m_bInitialized = false;
    bool m_bSyncingScrollBars = false;
    bool m_bScrollBarUpdatePending = false;
    bool m_bLineNumbersVisible = true;
    bool m_bFoldingVisible = true;
    ui::UiRect m_rcTextArea;
};

} // namespace duilib::ext::scintilla

#endif // DUILIB_EXT_SCINTILLA_SCINTILLA_CONTROL_H_
