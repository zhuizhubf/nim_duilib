#include "ScintillaDuiPopup.h"

#include "ScintillaControl.h"
#include "ScintillaDui.h"
#include "ScintillaDuiSurface.h"

#include "duilib/Box/ListBox.h"
#include "duilib/Box/ListBoxItem.h"
#include "duilib/Core/UiPoint.h"
#include "duilib/Core/Window.h"
#include "duilib/Utils/StringConvert.h"
#include "duilib/duilib_defs.h"

#include "Debugging.h"
#include "Platform.h"

#include <algorithm>
#include <cstring>
#include <string>
#include <vector>

namespace duilib::ext::scintilla {

using namespace Scintilla::Internal;

namespace {

/** 内部弹出窗口的控件名 */
constexpr const char *kPopupContentName = "scintilla_popup_content";
constexpr const char *kPopupListName = "scintilla_popup_list";

/** CallTip 弹窗的 XML（对应 Qt 的 CallTipImpl 绘制窗口） */
const char *CallTipWindowXml()
{
    return "<Window shadow_type=\"menu_round\" shadow_border_size=\"1\" "
           "shadow_border_color=\"border_window\">"
           "<Box name=\"scintilla_popup_root\">"
           "<ScintillaPopupContent name=\"scintilla_popup_content\" width=\"stretch\" "
           "height=\"stretch\"/>"
           "</Box>"
           "</Window>";
}

/** 自动补全列表弹窗的 XML（对应 Qt 的 ListWidget） */
const char *ListWindowXml()
{
    return "<Window shadow_type=\"menu_round\" shadow_border_size=\"1\" "
           "shadow_border_color=\"border_window\">"
           "<Box name=\"scintilla_popup_root\">"
           "<ListBox class=\"list\" name=\"scintilla_popup_list\" vscrollbar=\"true\" "
           "width=\"stretch\" height=\"stretch\"/>"
           "</Box>"
           "</Window>";
}

/** 取字符串中第一个分隔符之前的内容 */
std::string TextBeforeSeparator(const char *text, char separator)
{
    if (text == nullptr) {
        return {};
    }
    const std::string value(text);
    const size_t nPos = value.find(separator);
    return (nPos == std::string::npos) ? value : value.substr(0, nPos);
}

} // namespace

//------------------------------------------------------------------------------
// ScintillaDuiPopupContent
//------------------------------------------------------------------------------

ScintillaDuiPopupContent::ScintillaDuiPopupContent(ui::Window *pWindow)
    : BaseClass(pWindow)
{}

ScintillaDuiPopupContent::~ScintillaDuiPopupContent() = default;

DString ScintillaDuiPopupContent::GetType() const
{
    return _T("ScintillaPopupContent");
}

void ScintillaDuiPopupContent::Paint(ui::IRender *pRender, const ui::UiRect &rcPaint)
{
    BaseClass::Paint(pRender, rcPaint);
    if (m_paintCallback && (pRender != nullptr)) {
        m_paintCallback(pRender, rcPaint);
    }
}

void ScintillaDuiPopupContent::SetPaintCallback(
    std::function<void(ui::IRender *, const ui::UiRect &)> callback)
{
    m_paintCallback = std::move(callback);
}

//------------------------------------------------------------------------------
// ScintillaDuiPopupWindow
//------------------------------------------------------------------------------

ScintillaDuiPopupWindow::ScintillaDuiPopupWindow(PopupType type, ui::Window *pParentWindow)
    : m_type(type)
    , m_pParentWindow(pParentWindow)
{}

ScintillaDuiPopupWindow::~ScintillaDuiPopupWindow() = default;

DString ScintillaDuiPopupWindow::GetSkinFolder()
{
    return _T("");
}

DString ScintillaDuiPopupWindow::GetSkinFile()
{
    //直接返回 XML 内容，无需额外的资源文件
    return ui::StringConvert::UTF8ToT(
        (m_type == PopupType::list) ? ListWindowXml() : CallTipWindowXml());
}

ui::Control *ScintillaDuiPopupWindow::CreateControl(const DString &strClass)
{
    if (strClass == _T("ScintillaPopupContent")) {
        m_pContent = new ScintillaDuiPopupContent(this);
        return m_pContent;
    }
    return BaseClass::CreateControl(strClass);
}

void ScintillaDuiPopupWindow::OnInitWindow()
{
    BaseClass::OnInitWindow();
    m_pContent = dynamic_cast<ScintillaDuiPopupContent *>(
        FindControl(ui::StringConvert::UTF8ToT(kPopupContentName)));
    m_pListBox = dynamic_cast<ui::ListBox *>(
        FindControl(ui::StringConvert::UTF8ToT(kPopupListName)));
}

void ScintillaDuiPopupWindow::OnCloseWindow()
{
    m_pContent = nullptr;
    m_pListBox = nullptr;
    m_bCreated = false;
    BaseClass::OnCloseWindow();
}

bool ScintillaDuiPopupWindow::EnsureCreated()
{
    if (m_bCreated) {
        return true;
    }
    if (m_pParentWindow == nullptr) {
        return false;
    }
    ui::WindowCreateParam createParam(_T("scintilla_popup"));
    createParam.m_dwStyle = ui::kWS_POPUP;
    createParam.m_dwExStyle = ui::kWS_EX_TOPMOST | ui::kWS_EX_TOOLWINDOW | ui::kWS_EX_LAYERED;
    createParam.m_bCenterWindow = false;
    createParam.m_nX = m_rcScreen.left;
    createParam.m_nY = m_rcScreen.top;
    createParam.m_nWidth = std::max<int32_t>(1, m_rcScreen.Width());
    createParam.m_nHeight = std::max<int32_t>(1, m_rcScreen.Height());
    if (!CreateWnd(m_pParentWindow, createParam)) {
        return false;
    }
    m_bCreated = true;
    return true;
}

void ScintillaDuiPopupWindow::ShowAt(const ui::UiRect &rcScreen)
{
    m_rcScreen = rcScreen;
    if (!EnsureCreated()) {
        return;
    }
    const int32_t nWidth = std::max<int32_t>(1, rcScreen.Width());
    const int32_t nHeight = std::max<int32_t>(1, rcScreen.Height());
    MoveWindow(rcScreen.left, rcScreen.top, nWidth, nHeight, true);
    ShowWindow(ui::kSW_SHOW_NA);
    if (m_pContent != nullptr) {
        m_pContent->Invalidate();
    }
}

void ScintillaDuiPopupWindow::Hide()
{
    if (m_bCreated && IsWindowVisible()) {
        ShowWindow(ui::kSW_HIDE);
    }
}

bool ScintillaDuiPopupWindow::IsPopupVisible() const
{
    return m_bCreated && IsWindowVisible();
}

//------------------------------------------------------------------------------
// ScintillaDuiCallTip
//------------------------------------------------------------------------------

ScintillaDuiCallTip::ScintillaDuiCallTip(ScintillaControl *pControl, ScintillaDui *pSci)
    : m_pControl(pControl)
    , m_pSci(pSci)
{}

ScintillaDuiCallTip::~ScintillaDuiCallTip()
{
    m_pWindow.reset();
}

void ScintillaDuiCallTip::Show(const ui::UiRect &rcScreen)
{
    if (m_pControl == nullptr) {
        return;
    }
    if (m_pWindow == nullptr) {
        m_pWindow = std::make_unique<ScintillaDuiPopupWindow>(
            ScintillaDuiPopupWindow::PopupType::callTip, m_pControl->GetWindow());
        if (!m_pWindow->EnsureCreated()) {
            m_pWindow.reset();
            return;
        }
        if (ScintillaDuiPopupContent *pContent = m_pWindow->GetContent()) {
            ScintillaDuiCallTip *pThis = this;
            pContent->SetPaintCallback([pThis](ui::IRender *pRender, const ui::UiRect &rcPaint) {
                pThis->PaintContent(pRender, rcPaint);
            });
        }
    }
    m_rcScreen = rcScreen;
    m_pWindow->ShowAt(rcScreen);
}

void ScintillaDuiCallTip::Hide()
{
    if (m_pWindow != nullptr) {
        m_pWindow->Hide();
    }
}

bool ScintillaDuiCallTip::IsVisible() const
{
    return (m_pWindow != nullptr) && m_pWindow->IsPopupVisible();
}

void ScintillaDuiCallTip::PaintContent(ui::IRender *pRender, const ui::UiRect & /*rcPaint*/)
{
    if ((pRender == nullptr) || (m_pSci == nullptr)) {
        return;
    }
    //对应 Qt 的 CallTipImpl::paintEvent -> pct->PaintCT(surface)
    Scintilla::Internal::CallTip *pCallTip = m_pSci->GetCallTipDui();
    if (pCallTip == nullptr) {
        return;
    }
    ScintillaDuiSurface surface(m_pSci, pRender);
    surface.SetMode(Scintilla::Internal::SurfaceMode(SC_CP_UTF8, false));
    //弹出窗口的内容控件可能不在窗口左上角，这里叠加其客户区原点
    if ((m_pWindow != nullptr) && (m_pWindow->GetContent() != nullptr)) {
        const ui::UiRect rcContent = m_pWindow->GetContent()->GetRect();
        surface.SetPositionOffset(ui::UiPoint(rcContent.left, rcContent.top));
    }
    pCallTip->PaintCT(&surface);
    surface.Release();
}

PRectangle ScintillaDuiCallTip::GetWindowPositionHandle() const
{
    return PRectangle::FromInts(m_rcScreen.left, m_rcScreen.top, m_rcScreen.right, m_rcScreen.bottom);
}

void ScintillaDuiCallTip::SetWindowPositionHandle(PRectangle rc)
{
    ui::UiRect rcScreen(
        static_cast<int32_t>(rc.left),
        static_cast<int32_t>(rc.top),
        static_cast<int32_t>(rc.right),
        static_cast<int32_t>(rc.bottom));
    m_rcScreen = rcScreen;
    if (m_pWindow != nullptr) {
        m_pWindow->ShowAt(rcScreen);
    }
}

void ScintillaDuiCallTip::ShowWindowHandle(bool bShow)
{
    if (bShow) {
        Show(m_rcScreen);
    } else {
        Hide();
    }
}

void ScintillaDuiCallTip::DestroyWindowHandle() noexcept
{
    if (m_pWindow != nullptr) {
        m_pWindow->Hide();
    }
}

void ScintillaDuiCallTip::InvalidateWindowAllHandle()
{
    if ((m_pWindow != nullptr) && m_pWindow->IsPopupVisible()) {
        m_pWindow->ShowAt(m_rcScreen);
    }
}

void ScintillaDuiCallTip::InvalidateWindowRectHandle(PRectangle /*rc*/)
{
    InvalidateWindowAllHandle();
}

void ScintillaDuiCallTip::SetWindowCursorHandle(Scintilla::Internal::Window::Cursor /*cursor*/)
{
    //CallTip 使用默认光标
}

PRectangle ScintillaDuiCallTip::GetMonitorRectHandle(Scintilla::Internal::Point /*pt*/)
{
    return PRectangle(0, 0, 4096, 4096);
}

ui::UiPoint ScintillaDuiCallTip::ClientToScreenHandle(const ui::UiPoint &ptClient) const
{
    return ui::UiPoint(ptClient.x + m_rcScreen.left, ptClient.y + m_rcScreen.top);
}

//------------------------------------------------------------------------------
// 自动补全列表（对应 Qt 的 ListBoxImpl）
//------------------------------------------------------------------------------

namespace {

class DuiListBox final : public Scintilla::Internal::ListBox, public ScintillaDuiWindowHandle
{
public:
    DuiListBox() = default;
    DuiListBox(const DuiListBox &) = delete;
    DuiListBox &operator=(const DuiListBox &) = delete;
    ~DuiListBox() override = default;

    //---- Scintilla::Internal::ListBox ----
    void SetFont(const Scintilla::Internal::Font * /*font*/) override {}

    void Create(
        Scintilla::Internal::Window &parent,
        int ctrlID,
        Scintilla::Internal::Point /*location*/,
        int lineHeight,
        bool /*unicodeMode*/,
        Scintilla::Technology /*technology*/) override
    {
        m_nCtrlId = ctrlID;
        m_nLineHeight = std::max(1, lineHeight);
        m_pParentHandle = static_cast<ScintillaDuiWindowHandle *>(parent.GetID());
        wid = static_cast<ScintillaDuiWindowHandle *>(this);

        ui::Window *pOwnerWindow = nullptr;
        if (ScintillaDui *pDui = dynamic_cast<ScintillaDui *>(m_pParentHandle)) {
            if ((pDui->GetControl() != nullptr) && (pDui->GetControl()->GetWindow() != nullptr)) {
                pOwnerWindow = pDui->GetControl()->GetWindow();
            }
        }
        if (pOwnerWindow == nullptr) {
            return;
        }
        if (m_pWindow == nullptr) {
            m_pWindow = std::make_unique<ScintillaDuiPopupWindow>(
                ScintillaDuiPopupWindow::PopupType::list, pOwnerWindow);
            m_pWindow->EnsureCreated();
            if (ui::ListBox *pListBox = m_pWindow->GetListBox()) {
                DuiListBox *pThis = this;
                //鼠标点击列表项：接受补全（对应 Qt 的 itemClicked -> doubleClick 事件）
                pListBox->AttachClick([pThis](const ui::EventArgs & /*msg*/) {
                    pThis->NotifyDelegate(true);
                    return true;
                });
                //键盘选择变化
                pListBox->AttachSelChanged([pThis](const ui::EventArgs & /*msg*/) {
                    pThis->NotifyDelegate(false);
                    return true;
                });
            } else {
                m_pWindow.reset();
            }
        }
    }

    void SetAverageCharWidth(int width) override { m_nAveCharWidth = std::max(1, width); }
    void SetVisibleRows(int rows) override { m_nVisibleRows = std::max(1, rows); }
    int GetVisibleRows() const override { return m_nVisibleRows; }

    Scintilla::Internal::PRectangle GetDesiredRect() override
    {
        //估算列表所需的宽高（对应 Qt 的 ListBoxImpl::GetDesiredRect）
        size_t nMaxChars = 8;
        for (const std::string &item : m_items) {
            nMaxChars = std::max(nMaxChars, item.size());
        }
        const int32_t nWidth = static_cast<int32_t>(nMaxChars) * m_nAveCharWidth
                               + 2 * CaretFromEdge();
        const int32_t nRows = static_cast<int32_t>(
            std::min<size_t>(m_items.size(), static_cast<size_t>(m_nVisibleRows)));
        const int32_t nHeight = std::max(1, nRows) * m_nLineHeight + 2;
        return Scintilla::Internal::PRectangle(0, 0, nWidth, nHeight);
    }

    int CaretFromEdge() override { return 2; }

    void Clear() noexcept override
    {
        m_items.clear();
        m_nSelection = -1;
        if ((m_pWindow != nullptr) && (m_pWindow->GetListBox() != nullptr)) {
            m_pWindow->GetListBox()->RemoveAllItems();
        }
    }

    void Append(char *text, int /*type*/) override
    {
        //Scintilla 的列表项可能带有 "#类型" 后缀
        const std::string item = TextBeforeSeparator(text, '#');
        m_items.push_back(item);
        AppendToPopupList(item);
        if (m_nSelection < 0) {
            m_nSelection = 0;
            Select(m_nSelection);
        }
    }

    int Length() override { return static_cast<int>(m_items.size()); }

    void Select(int n) override
    {
        if ((n < 0) || (n >= Length())) {
            return;
        }
        m_nSelection = n;
        if ((m_pWindow != nullptr) && (m_pWindow->GetListBox() != nullptr)) {
            ui::ListBox *pListBox = m_pWindow->GetListBox();
            pListBox->SetCurSel(static_cast<size_t>(n));
            pListBox->EnsureVisible(static_cast<size_t>(n));
        }
    }

    int GetSelection() override { return m_nSelection; }

    int Find(const char *prefix) override
    {
        if (prefix == nullptr) {
            return -1;
        }
        const std::string prefixText(prefix);
        for (size_t i = 0; i < m_items.size(); ++i) {
            if (m_items[i].compare(0, prefixText.size(), prefixText) == 0) {
                return static_cast<int>(i);
            }
        }
        return -1;
    }

    std::string GetValue(int n) override
    {
        if ((n >= 0) && (n < Length())) {
            return m_items[static_cast<size_t>(n)];
        }
        return {};
    }

    void RegisterImage(int /*type*/, const char * /*xpm_data*/) override {}
    void RegisterRGBAImage(
        int /*type*/, int /*width*/, int /*height*/, const unsigned char * /*pixelsImage*/) override
    {}
    void ClearRegisteredImages() override {}
    void SetDelegate(Scintilla::Internal::IListBoxDelegate *lbDelegate) override
    {
        m_pDelegate = lbDelegate;
    }

    void SetList(const char *list, char separator, char typesep) override
    {
        Clear();
        if (list == nullptr) {
            return;
        }
        std::string current;
        for (const char *p = list; *p != '\0'; ++p) {
            if (*p == separator) {
                if (typesep != '\0') {
                    const size_t typePos = current.find(typesep);
                    if (typePos != std::string::npos) {
                        current.resize(typePos);
                    }
                }
                m_items.push_back(current);
                AppendToPopupList(current);
                current.clear();
            } else {
                current.push_back(*p);
            }
        }
        if (!current.empty()) {
            if (typesep != '\0') {
                const size_t typePos = current.find(typesep);
                if (typePos != std::string::npos) {
                    current.resize(typePos);
                }
            }
            m_items.push_back(current);
            AppendToPopupList(current);
        }
        if (!m_items.empty()) {
            m_nSelection = 0;
            Select(0);
        }
    }

    void SetOptions(Scintilla::Internal::ListOptions /*options_*/) override {}

    //---- ScintillaDuiWindowHandle ----
    virtual Scintilla::Internal::PRectangle GetWindowPositionHandle() const override
    {
        return Scintilla::Internal::PRectangle::FromInts(
            m_rcScreen.left, m_rcScreen.top, m_rcScreen.right, m_rcScreen.bottom);
    }

    virtual void SetWindowPositionHandle(Scintilla::Internal::PRectangle rc) override
    {
        m_rcScreen = ui::UiRect(
            static_cast<int32_t>(rc.left),
            static_cast<int32_t>(rc.top),
            static_cast<int32_t>(rc.right),
            static_cast<int32_t>(rc.bottom));
        if (m_pWindow != nullptr) {
            m_pWindow->ShowAt(m_rcScreen);
        }
    }

    virtual void ShowWindowHandle(bool bShow) override
    {
        if (m_pWindow == nullptr) {
            return;
        }
        if (bShow) {
            m_pWindow->ShowAt(m_rcScreen);
        } else {
            m_pWindow->Hide();
        }
    }

    virtual void DestroyWindowHandle() noexcept override
    {
        if (m_pWindow != nullptr) {
            m_pWindow->Hide();
        }
    }

    virtual void InvalidateWindowAllHandle() override {}
    virtual void InvalidateWindowRectHandle(Scintilla::Internal::PRectangle /*rc*/) override {}
    virtual void SetWindowCursorHandle(Scintilla::Internal::Window::Cursor /*cursor*/) override {}

    virtual Scintilla::Internal::PRectangle GetMonitorRectHandle(
        Scintilla::Internal::Point /*pt*/) override
    {
        return Scintilla::Internal::PRectangle(0, 0, 4096, 4096);
    }

    virtual ui::UiPoint ClientToScreenHandle(const ui::UiPoint &ptClient) const override
    {
        return ui::UiPoint(ptClient.x + m_rcScreen.left, ptClient.y + m_rcScreen.top);
    }

private:
    void AppendToPopupList(const std::string &text)
    {
        if ((m_pWindow == nullptr) || (m_pWindow->GetListBox() == nullptr)) {
            return;
        }
        ui::ListBox *pListBox = m_pWindow->GetListBox();
        auto *pItem = new ui::ListBoxItem(m_pWindow.get());
        pItem->SetClass(_T("list_item"));
        pItem->SetText(ui::StringConvert::UTF8ToT(text));
        if (!pListBox->AddItem(pItem)) {
            delete pItem;
        }
    }

    void NotifyDelegate(bool bAccept)
    {
        if (m_pDelegate == nullptr) {
            return;
        }
        if ((m_pWindow != nullptr) && (m_pWindow->GetListBox() != nullptr)) {
            const size_t nSel = m_pWindow->GetListBox()->GetCurSel();
            if (nSel != ui::Box::InvalidIndex) {
                m_nSelection = static_cast<int>(nSel);
            }
        }
        Scintilla::Internal::ListBoxEvent event(
            bAccept ? Scintilla::Internal::ListBoxEvent::EventType::doubleClick
                    : Scintilla::Internal::ListBoxEvent::EventType::selectionChange);
        m_pDelegate->ListNotify(&event);
    }

private:
    ScintillaDuiWindowHandle *m_pParentHandle = nullptr;
    std::unique_ptr<ScintillaDuiPopupWindow> m_pWindow;
    std::vector<std::string> m_items;
    Scintilla::Internal::IListBoxDelegate *m_pDelegate = nullptr;
    ui::UiRect m_rcScreen;
    int m_nCtrlId = 0;
    int m_nSelection = -1;
    int m_nVisibleRows = 5;
    int m_nLineHeight = 16;
    int m_nAveCharWidth = 8;
};

} // namespace

std::unique_ptr<Scintilla::Internal::ListBox> CreateDuiListBox()
{
    return std::make_unique<DuiListBox>();
}

} // namespace duilib::ext::scintilla
