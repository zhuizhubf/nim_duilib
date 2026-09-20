#ifndef DUILIB_EXT_SCINTILLA_SCINTILLA_DUI_POPUP_H_
#define DUILIB_EXT_SCINTILLA_SCINTILLA_DUI_POPUP_H_

#include "ScintillaDuiWindowHandle.h"

#include "duilib/Core/UiRect.h"
#include "duilib/Utils/WinImplBase.h"

#include <functional>
#include <memory>
#include <string>
#include <vector>

namespace ui {
class Control;
class IRender;
class ListBox;
class Window;
} // namespace ui

namespace Scintilla::Internal {
class ListBox;
} // namespace Scintilla::Internal

namespace duilib::ext::scintilla {

class ScintillaControl;
class ScintillaDui;
class ScintillaDuiPopupWindow;

/** 弹出窗口内容控件：把绘制回调转交给上层类（CallTip 使用）。
 *
 *  对应 Qt 平台层的 QWidget 子类 CallTipImpl（paintEvent 中调用 ct.PaintCT）。
 */
class ScintillaDuiPopupContent final : public ui::Control
{
    typedef ui::Control BaseClass;

public:
    explicit ScintillaDuiPopupContent(ui::Window *pWindow);
    ScintillaDuiPopupContent(const ScintillaDuiPopupContent &) = delete;
    ScintillaDuiPopupContent &operator=(const ScintillaDuiPopupContent &) = delete;
    ~ScintillaDuiPopupContent() override;

public:
    virtual DString GetType() const override;
    virtual void Paint(ui::IRender *pRender, const ui::UiRect &rcPaint) override;

    /** 设置绘制回调 */
    void SetPaintCallback(std::function<void(ui::IRender *, const ui::UiRect &)> callback);

private:
    std::function<void(ui::IRender *, const ui::UiRect &)> m_paintCallback;
};

/** 无焦点弹出窗口：承载 CallTip 或自动补全列表（对应 Qt 的 QWidget 弹出窗口） */
class ScintillaDuiPopupWindow final : public ui::WindowImplBase
{
    typedef ui::WindowImplBase BaseClass;

public:
    /** 弹出窗口的用途 */
    enum class PopupType {
        callTip, //CallTip：自定义绘制
        list,    //列表：内部创建一个 ui::ListBox
    };

    ScintillaDuiPopupWindow(PopupType type, ui::Window *pParentWindow);
    ScintillaDuiPopupWindow(const ScintillaDuiPopupWindow &) = delete;
    ScintillaDuiPopupWindow &operator=(const ScintillaDuiPopupWindow &) = delete;
    ~ScintillaDuiPopupWindow() override;

public:
    /** 创建窗口（内部完成，重复调用无效） */
    bool EnsureCreated();
    /** 按屏幕矩形定位并显示（不激活窗口） */
    void ShowAt(const ui::UiRect &rcScreen);
    void Hide();
    bool IsPopupVisible() const;

    ScintillaDuiPopupContent *GetContent() const noexcept { return m_pContent; }
    ui::ListBox *GetListBox() const noexcept { return m_pListBox; }

    virtual DString GetSkinFolder() override;
    virtual DString GetSkinFile() override;
    virtual ui::Control *CreateControl(const DString &strClass) override;

protected:
    virtual void OnInitWindow() override;
    virtual void OnCloseWindow() override;

private:
    PopupType m_type = PopupType::callTip;
    ui::Window *m_pParentWindow = nullptr;
    ScintillaDuiPopupContent *m_pContent = nullptr;
    ui::ListBox *m_pListBox = nullptr;
    ui::UiRect m_rcScreen;
    bool m_bCreated = false;
    bool m_bClosing = false;
};

/** CallTip 窗口（对应 Qt 平台层的 CallTipImpl）。
 *
 *  该对象同时是 Scintilla 平台层的窗口句柄（ct.wCallTip），
 *  实现 Window 的定位/显示/重绘请求。
 */
class ScintillaDuiCallTip final : public ScintillaDuiWindowHandle
{
public:
    ScintillaDuiCallTip(ScintillaControl *pControl, ScintillaDui *pSci);
    ScintillaDuiCallTip(const ScintillaDuiCallTip &) = delete;
    ScintillaDuiCallTip &operator=(const ScintillaDuiCallTip &) = delete;
    ~ScintillaDuiCallTip() override;

    /** 按屏幕矩形显示 */
    void Show(const ui::UiRect &rcScreen);
    void Hide();
    bool IsVisible() const;

    //ScintillaDuiWindowHandle
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
    void PaintContent(ui::IRender *pRender, const ui::UiRect &rcPaint);

private:
    ScintillaControl *m_pControl = nullptr;
    ScintillaDui *m_pSci = nullptr;
    std::unique_ptr<ScintillaDuiPopupWindow> m_pWindow;
    ui::UiRect m_rcScreen;
};

/** 创建自动补全列表的 Scintilla ListBox 实现（对应 Qt 的 ListBoxImpl） */
std::unique_ptr<Scintilla::Internal::ListBox> CreateDuiListBox();

} // namespace duilib::ext::scintilla

#endif // DUILIB_EXT_SCINTILLA_SCINTILLA_DUI_POPUP_H_
