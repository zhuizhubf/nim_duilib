#include "ScintillaControl.h"

#include "ScintillaDui.h"
#include "ScintillaDuiPopup.h"

#include "duilib/Control/Menu.h"
#include "duilib/Core/GlobalManager.h"
#include "duilib/Core/Keyboard.h"
#include "duilib/Core/ResourceParam.h"
#include "duilib/Core/ScrollBar.h"
#include "duilib/Core/Window.h"
#include "duilib/Utils/StringConvert.h"
#include "duilib/Utils/StringUtil.h"
#include "duilib/duilib_defs.h"

#include "Lexilla.h"
#include "Scintilla.h"

#include <algorithm>
#include <chrono>
#include <cmath>

#ifdef DUILIB_BUILD_FOR_SDL
#include <SDL3/SDL_events.h>
#endif

namespace duilib::ext::scintilla {

namespace {

/** 鼠标事件时间戳（毫秒），对应 Qt 的 QElapsedTimer::elapsed()。
 *  Scintilla 依赖该时间戳判定双击、三击和拖动选择，必须单调递增。
 */
uint32_t CurrentMouseTimeMs() noexcept
{
    using namespace std::chrono;
    //每 20 亿毫秒（约 24 天）回绕一次，与 Qt 平台层保持一致
    constexpr int64_t kMaxTime = 2000000000;
    const int64_t nMs = duration_cast<milliseconds>(steady_clock::now().time_since_epoch()).count();
    return static_cast<uint32_t>(nMs % kMaxTime);
}

/** 虚拟键码 -> Scintilla 按键（移植自 ScintillaWin::KeyTranslate） */
Scintilla::Keys KeyTranslate(uint32_t vkCode) noexcept
{
    switch (vkCode) {
    case ui::kVK_DOWN:
        return Scintilla::Keys::Down;
    case ui::kVK_UP:
        return Scintilla::Keys::Up;
    case ui::kVK_LEFT:
        return Scintilla::Keys::Left;
    case ui::kVK_RIGHT:
        return Scintilla::Keys::Right;
    case ui::kVK_HOME:
        return Scintilla::Keys::Home;
    case ui::kVK_END:
        return Scintilla::Keys::End;
    case ui::kVK_PRIOR:
        return Scintilla::Keys::Prior;
    case ui::kVK_NEXT:
        return Scintilla::Keys::Next;
    case ui::kVK_DELETE:
        return Scintilla::Keys::Delete;
    case ui::kVK_INSERT:
        return Scintilla::Keys::Insert;
    case ui::kVK_ESCAPE:
        return Scintilla::Keys::Escape;
    case ui::kVK_BACK:
        return Scintilla::Keys::Back;
    case ui::kVK_TAB:
        return Scintilla::Keys::Tab;
    case ui::kVK_RETURN:
        return Scintilla::Keys::Return;
    case ui::kVK_ADD:
        return Scintilla::Keys::Add;
    case ui::kVK_SUBTRACT:
        return Scintilla::Keys::Subtract;
    case ui::kVK_DIVIDE:
        return Scintilla::Keys::Divide;
    case ui::kVK_LWIN:
        return Scintilla::Keys::Win;
    case ui::kVK_RWIN:
        return Scintilla::Keys::RWin;
    case ui::kVK_APPS:
        return Scintilla::Keys::Menu;
    case ui::kVK_OEM_2:
        return static_cast<Scintilla::Keys>('/');
    case ui::kVK_OEM_3:
        return static_cast<Scintilla::Keys>('`');
    case ui::kVK_OEM_4:
        return static_cast<Scintilla::Keys>('[');
    case ui::kVK_OEM_5:
        return static_cast<Scintilla::Keys>('\\');
    case ui::kVK_OEM_6:
        return static_cast<Scintilla::Keys>(']');
    default:
        return static_cast<Scintilla::Keys>(vkCode);
    }
}

Scintilla::KeyMod ToScintillaModifiers(uint32_t modifierKey) noexcept
{
    Scintilla::KeyMod modifiers = Scintilla::KeyMod::Norm;
    if ((modifierKey & ui::ModifierKey::kShift) != 0) {
        modifiers = static_cast<Scintilla::KeyMod>(static_cast<int>(modifiers) | SCMOD_SHIFT);
    }
    if ((modifierKey & ui::ModifierKey::kControl) != 0) {
        modifiers = static_cast<Scintilla::KeyMod>(static_cast<int>(modifiers) | SCMOD_CTRL);
    }
    if ((modifierKey & ui::ModifierKey::kAlt) != 0) {
        modifiers = static_cast<Scintilla::KeyMod>(static_cast<int>(modifiers) | SCMOD_ALT);
    }
    if ((modifierKey & ui::ModifierKey::kWin) != 0) {
        modifiers = static_cast<Scintilla::KeyMod>(static_cast<int>(modifiers) | SCMOD_SUPER);
    }
    return modifiers;
}

/** 把文本输入事件转换为 UTF-8 文本 */
std::string ToScintillaUTF8(const ui::EventArgs &msg)
{
#ifdef DUILIB_BUILD_FOR_SDL
    //SDL 文本输入：wParam 指向 UTF-16 字符串，lParam 为长度
    if ((msg.eventData == SDL_EVENT_TEXT_INPUT) && (msg.wParam != 0) && (msg.lParam > 0)) {
        const DStringW text(
            reinterpret_cast<const DStringW::value_type *>(msg.wParam),
            static_cast<size_t>(msg.lParam));
        return ui::StringConvert::TToUTF8(text);
    }
#endif
    //Windows WM_CHAR：wParam 为一个 UTF-16 码元
    if (msg.wParam != 0) {
        DStringW text;
        text.push_back(static_cast<DStringW::value_type>(msg.wParam));
        return ui::StringConvert::TToUTF8(text);
    }
    return {};
}

/** COLORREF（0x00BBGGRR） */
constexpr uint32_t ColorRefRGB(uint32_t r, uint32_t g, uint32_t b) noexcept
{
    return (r & 0xFF) | ((g & 0xFF) << 8) | ((b & 0xFF) << 16);
}

/** 生成 XML 文本时使用的转义 */
DString EscapeXmlText(const DString &text)
{
    DString result;
    result.reserve(text.size() + 8);
    for (const DString::value_type ch : text) {
        switch (ch) {
        case _T('&'):
            result += _T("&amp;");
            break;
        case _T('<'):
            result += _T("&lt;");
            break;
        case _T('>'):
            result += _T("&gt;");
            break;
        case _T('"'):
            result += _T("&quot;");
            break;
        case _T('\''):
            result += _T("&apos;");
            break;
        case _T('\t'):
            result += _T("    ");
            break;
        default:
            result += ch;
            break;
        }
    }
    return result;
}

} // namespace

//------------------------------------------------------------------------------
// 内部滚动条：位置变化时通知编辑控件（对应 Qt 的 QScrollBar::valueChanged）
//------------------------------------------------------------------------------
namespace {

class ScintillaScrollBar final : public ui::ScrollBar
{
    typedef ui::ScrollBar BaseClass;

public:
    ScintillaScrollBar(ui::Window *pWindow, ScintillaControl *pEditor, bool bVertical)
        : BaseClass(pWindow)
        , m_pEditor(pEditor)
        , m_bVertical(bVertical)
    {}

    ScintillaScrollBar(const ScintillaScrollBar &) = delete;
    ScintillaScrollBar &operator=(const ScintillaScrollBar &) = delete;
    ~ScintillaScrollBar() override = default;

    virtual void SetPos(ui::UiRect rc) override
    {
        BaseClass::SetPos(rc);
        //所有滚动位置的变化最终都会通过 SetPos 反映出来（SetScrollPos 内部会调用 SetPos），
        //这里统一拦截，避免依赖 ui::ScrollBar 的内部实现细节
        const int64_t nPos = GetScrollPos();
        if (nPos != m_nLastPos) {
            m_nLastPos = nPos;
            if (m_pEditor != nullptr) {
                m_pEditor->OnScrollBarPosChanged(m_bVertical, nPos);
            }
        }
    }

private:
    ScintillaControl *m_pEditor = nullptr;
    bool m_bVertical = true;
    int64_t m_nLastPos = 0;
};

} // namespace

ScintillaControl::ScintillaControl(ui::Window *pWindow)
    : BaseClass(pWindow)
{}

ScintillaControl::~ScintillaControl()
{
    DisposePopupWindows();
    if (m_pSci != nullptr) {
        m_pSci->FinaliseDui();
        m_pSci.reset();
    }
    m_pVScrollBar.reset();
    m_pHScrollBar.reset();
}

DString ScintillaControl::GetType() const
{
    return _T("ScintillaControl");
}

int32_t ScintillaControl::GetDisplayScalePercent() const
{
    if (GetWindow() == nullptr) {
        //控件尚未关联窗口（例如 XML 构建过程中），使用默认缩放比例
        return 100;
    }
    const uint32_t nScale = Dpi().GetDisplayScaleFactor();
    return (nScale > 0) ? static_cast<int32_t>(nScale) : 100;
}

int32_t ScintillaControl::ScaledValue(int32_t nValue) const
{
    const int32_t nScale = GetDisplayScalePercent();
    return (nValue * nScale + 50) / 100;
}

void ScintillaControl::OnInit()
{
    BaseClass::OnInit();
    if (m_bInitialized) {
        return;
    }

    CreateScrollBars();

    m_pSci = std::make_unique<ScintillaDui>(this);
    m_pSci->InitDui();
    m_bInitialized = true;

    SendSciMessage(SCI_SETCODEPAGE, SC_CP_UTF8, 0);
    SendSciMessage(SCI_SETMULTIPLESELECTION, 1, 0);
    SendSciMessage(SCI_SETADDITIONALSELECTIONTYPING, 1, 0);
    SendSciMessage(SCI_SETMULTIPASTE, SC_MULTIPASTE_EACH, 0);
    SendSciMessage(SCI_SETVIRTUALSPACEOPTIONS, SCVS_RECTANGULARSELECTION, 0);
    SendSciMessage(SCI_SETMARGINOPTIONS, SC_IV_LOOKBOTH, 0);
    SetMarginVisible();
    SetTabWidth(4);
    SetUseTabs(false);
    SetWordWrap(false);
    SetCaretLineVisible(true);

    if (!m_hasDefaultStyle) {
        //没有显式设置默认样式时，使用平台默认的等宽字体，保证文本可见
        ui::UiFont font;
        font.m_fontName = ui::StringConvert::UTF8ToT(Scintilla::Internal::Platform::DefaultFont());
        font.m_fontSize = std::max<int32_t>(1, Scintilla::Internal::Platform::DefaultFontSize());
        font.m_fontSize = ScaledValue(font.m_fontSize);
        SetDefaultStyle(
            ui::UiColor(0xFF, 0x1E, 0x1E, 0x1E), ui::UiColor(0xFF, 0xFF, 0xFF, 0xFF), font);
    } else {
        ApplyDefaultStyleToScintilla();
    }

    if (!m_initialText.empty()) {
        SetText(m_initialText);
    }
    if (!m_lexerName.empty()) {
        SetLexer(m_lexerName);
    }

    UpdateScrollBarsFromScintilla();
}

void ScintillaControl::SetAttribute(const DString &strName, const DString &strValue)
{
    if (strName == _T("lexer")) {
        m_lexerName = ui::StringConvert::TToUTF8(strValue);
        if (m_bInitialized) {
            SetLexer(m_lexerName);
        }
        return;
    }
    if (strName == _T("text")) {
        m_initialText = strValue;
        if (m_bInitialized) {
            SetText(strValue);
        }
        return;
    }
    if (strName == _T("readonly")) {
        SetReadOnly(ui::StringUtil::IsValueTrue(strValue));
        return;
    }
    if (strName == _T("word_wrap")) {
        SetWordWrap(ui::StringUtil::IsValueTrue(strValue));
        return;
    }
    if (strName == _T("tab_width")) {
        SetTabWidth(ui::StringUtil::StringToInt32(strValue));
        return;
    }
    if (strName == _T("use_tabs")) {
        SetUseTabs(ui::StringUtil::IsValueTrue(strValue));
        return;
    }
    if (strName == _T("line_numbers")) {
        SetLineNumbersVisible(ui::StringUtil::IsValueTrue(strValue));
        return;
    }
    if (strName == _T("folding")) {
        SetFoldingVisible(ui::StringUtil::IsValueTrue(strValue));
        return;
    }
    if (strName == _T("zoom")) {
        SetZoom(ui::StringUtil::StringToInt32(strValue));
        return;
    }
    BaseClass::SetAttribute(strName, strValue);
}

void ScintillaControl::SetPos(ui::UiRect rc)
{
    BaseClass::SetPos(rc);
    if (m_pSci == nullptr) {
        return;
    }
    //对应 Qt 的 resizeEvent -> sqt->ChangeSize()
    UpdateScrollBarsFromScintilla();
}

void ScintillaControl::Paint(ui::IRender *pRender, const ui::UiRect &rcPaint)
{
    if ((pRender == nullptr) || (m_pSci == nullptr)) {
        return;
    }
    BaseClass::Paint(pRender, rcPaint);

    const ui::UiRect rcArea = m_rcTextArea;
    if (rcArea.IsEmpty()) {
        return;
    }
    ui::UiRect rcDirty;
    if (!ui::UiRect::Intersect(rcDirty, rcPaint, rcArea)) {
        return;
    }

    //Scintilla 使用编辑器内容区的局部坐标绘制；
    //这里不使用 OffsetWindowOrg（核心库各后端对其符号约定不一致），
    //而是把内容区原点交给 Surface，由其叠加到所有绘制坐标上。
    const int32_t nClipState = pRender->SetClip(rcArea, true);
    m_pSci->PaintDui(
        pRender,
        ui::UiRect(
            rcDirty.left - rcArea.left,
            rcDirty.top - rcArea.top,
            rcDirty.right - rcArea.left,
            rcDirty.bottom - rcArea.top),
        ui::UiPoint(rcArea.left, rcArea.top));
    pRender->ClearClip(nClipState);
}

void ScintillaControl::PaintChild(ui::IRender *pRender, const ui::UiRect &rcPaint)
{
    if ((pRender == nullptr) || (m_pSci == nullptr)) {
        return;
    }
    if ((m_pVScrollBar != nullptr) && m_pVScrollBar->IsVisible()) {
        m_pVScrollBar->AlphaPaint(pRender, rcPaint);
    }
    if ((m_pHScrollBar != nullptr) && m_pHScrollBar->IsVisible()) {
        m_pHScrollBar->AlphaPaint(pRender, rcPaint);
    }
}

ui::Control *ScintillaControl::FindControl(
    ui::FINDCONTROLPROC Proc,
    void *pProcData,
    uint32_t uFlags,
    const ui::UiPoint &ptMouse,
    const ui::UiPoint &scrollPos)
{
    if (((uFlags & UIFIND_VISIBLE) != 0) && !IsVisible()) {
        return nullptr;
    }
    if (((uFlags & UIFIND_ENABLED) != 0) && !IsEnabled()) {
        return nullptr;
    }

    ui::UiPoint pt(ptMouse);
    pt.Offset(scrollPos);
    if (((uFlags & UIFIND_HITTEST) != 0) && !GetRect().ContainsPt(pt)) {
        return nullptr;
    }

    //滚动条优先命中（与 ui::ScrollBox 的处理方式一致）
    if ((m_pVScrollBar != nullptr) && m_pVScrollBar->IsVisible()) {
        if (ui::Control *pResult = m_pVScrollBar->FindControl(Proc, pProcData, uFlags, pt)) {
            return pResult;
        }
    }
    if ((m_pHScrollBar != nullptr) && m_pHScrollBar->IsVisible()) {
        if (ui::Control *pResult = m_pHScrollBar->FindControl(Proc, pProcData, uFlags, pt)) {
            return pResult;
        }
    }
    return BaseClass::FindControl(Proc, pProcData, uFlags, pt);
}

void ScintillaControl::SetParent(ui::Box *pParent)
{
    if (m_pVScrollBar != nullptr) {
        m_pVScrollBar->SetParent(this);
    }
    if (m_pHScrollBar != nullptr) {
        m_pHScrollBar->SetParent(this);
    }
    BaseClass::SetParent(pParent);
}

void ScintillaControl::SetWindow(ui::Window *pWindow)
{
    if (m_pVScrollBar != nullptr) {
        m_pVScrollBar->SetWindow(pWindow);
    }
    if (m_pHScrollBar != nullptr) {
        m_pHScrollBar->SetWindow(pWindow);
    }
    BaseClass::SetWindow(pWindow);
}

void ScintillaControl::ChangeDpiScale(uint32_t nOldDpiScale, uint32_t nNewDpiScale)
{
    BaseClass::ChangeDpiScale(nOldDpiScale, nNewDpiScale);
    if (m_pVScrollBar != nullptr) {
        m_pVScrollBar->ChangeDpiScale(nOldDpiScale, nNewDpiScale);
    }
    if (m_pHScrollBar != nullptr) {
        m_pHScrollBar->ChangeDpiScale(nOldDpiScale, nNewDpiScale);
    }
    if (m_pSci == nullptr) {
        return;
    }
    //DPI 变化后 LogPixelsY 随之变化，字体需要重新实现
    //（对应 Qt 平台层的 DevicePixelRatioChange -> InvalidateStyleRedraw）
    ApplyDefaultStyleToScintilla();
    m_pSci->InvalidateStyleRedrawDui();
    UpdateScrollBarsFromScintilla();
    Invalidate();
}

std::string ScintillaControl::GetTextUTF8() const
{
    if (m_pSci == nullptr) {
        return {};
    }
    const intptr_t length = m_pSci->SendSciMessage(SCI_GETLENGTH, 0, 0);
    if (length <= 0) {
        return {};
    }
    std::string text(static_cast<size_t>(length), '\0');
    m_pSci->SendSciMessage(
        SCI_GETTEXT, static_cast<uintptr_t>(length + 1), reinterpret_cast<intptr_t>(text.data()));
    return text;
}

void ScintillaControl::SetTextUTF8(const std::string &text)
{
    if (m_pSci == nullptr) {
        m_initialText = ui::StringConvert::UTF8ToT(text);
        return;
    }
    m_pSci->SendSciMessage(SCI_SETTEXT, 0, reinterpret_cast<intptr_t>(text.c_str()));
}

DString ScintillaControl::GetText() const
{
    return ui::StringConvert::UTF8ToT(GetTextUTF8());
}

void ScintillaControl::SetText(const DString &text)
{
    SetTextUTF8(ui::StringConvert::TToUTF8(text));
}

void ScintillaControl::SetReadOnly(bool bReadOnly)
{
    SendSciMessage(SCI_SETREADONLY, bReadOnly ? 1 : 0, 0);
}

bool ScintillaControl::IsReadOnly() const
{
    return SendSciMessage(SCI_GETREADONLY, 0, 0) != 0;
}

void ScintillaControl::SetLexer(const std::string &lexerName)
{
    m_lexerName = lexerName;
    if (m_pSci == nullptr) {
        return;
    }
    if (lexerName.empty()) {
        SendSciMessage(SCI_SETILEXER, 0, 0);
        return;
    }
    Scintilla::ILexer5 *pLexer = CreateLexer(lexerName.c_str());
    if (pLexer != nullptr) {
        SendSciMessage(SCI_SETILEXER, 0, reinterpret_cast<intptr_t>(pLexer));
    }
}

std::string ScintillaControl::GetLexer() const
{
    return m_lexerName;
}

void ScintillaControl::SetTabWidth(int32_t nTabWidth)
{
    SendSciMessage(SCI_SETTABWIDTH, static_cast<uintptr_t>(std::max(1, nTabWidth)), 0);
}

int32_t ScintillaControl::GetTabWidth() const
{
    return static_cast<int32_t>(SendSciMessage(SCI_GETTABWIDTH, 0, 0));
}

void ScintillaControl::SetUseTabs(bool bUseTabs)
{
    SendSciMessage(SCI_SETUSETABS, bUseTabs ? 1 : 0, 0);
}

bool ScintillaControl::IsUseTabs() const
{
    return SendSciMessage(SCI_GETUSETABS, 0, 0) != 0;
}

void ScintillaControl::SetWordWrap(bool bWordWrap)
{
    SendSciMessage(SCI_SETWRAPMODE, bWordWrap ? SC_WRAP_WORD : SC_WRAP_NONE, 0);
    UpdateScrollBarsFromScintilla();
}

bool ScintillaControl::IsWordWrap() const
{
    return SendSciMessage(SCI_GETWRAPMODE, 0, 0) != SC_WRAP_NONE;
}

void ScintillaControl::SetLineNumbersVisible(bool bVisible)
{
    m_bLineNumbersVisible = bVisible;
    SetMarginVisible();
}

bool ScintillaControl::IsLineNumbersVisible() const
{
    return m_bLineNumbersVisible;
}

void ScintillaControl::SetFoldingVisible(bool bVisible)
{
    m_bFoldingVisible = bVisible;
    SetMarginVisible();
}

bool ScintillaControl::IsFoldingVisible() const
{
    return m_bFoldingVisible;
}

void ScintillaControl::SetMarginVisible()
{
    if (m_pSci == nullptr) {
        return;
    }
    //标记边距区：0 = 行号（或折叠符号），其余边距关闭
    constexpr int32_t kMarginCount = 5;
    int32_t nMargin = 0;
    if (m_bLineNumbersVisible) {
        SendSciMessage(SCI_SETMARGINTYPEN, static_cast<uintptr_t>(nMargin), SC_MARGIN_NUMBER);
        SendSciMessage(
            SCI_SETMARGINWIDTHN,
            static_cast<uintptr_t>(nMargin),
            static_cast<uintptr_t>(ScaledValue(44)));
        ++nMargin;
    }
    if (m_bFoldingVisible) {
        SendSciMessage(SCI_SETMARGINTYPEN, static_cast<uintptr_t>(nMargin), SC_MARGIN_SYMBOL);
        SendSciMessage(SCI_SETMARGINMASKN, static_cast<uintptr_t>(nMargin), SC_MASK_FOLDERS);
        SendSciMessage(SCI_SETMARGINSENSITIVEN, static_cast<uintptr_t>(nMargin), 1);
        SendSciMessage(
            SCI_SETMARGINWIDTHN,
            static_cast<uintptr_t>(nMargin),
            static_cast<uintptr_t>(ScaledValue(16)));
        DefineFoldingMarkers();
        ++nMargin;
    }
    for (; nMargin < kMarginCount; ++nMargin) {
        SendSciMessage(SCI_SETMARGINWIDTHN, static_cast<uintptr_t>(nMargin), 0);
    }
}

void ScintillaControl::DefineFoldingMarkers()
{
    SendSciMessage(SCI_MARKERDEFINE, SC_MARKNUM_FOLDEROPEN, SC_MARK_BOXMINUS);
    SendSciMessage(SCI_MARKERDEFINE, SC_MARKNUM_FOLDER, SC_MARK_BOXPLUS);
    SendSciMessage(SCI_MARKERDEFINE, SC_MARKNUM_FOLDERSUB, SC_MARK_VLINE);
    SendSciMessage(SCI_MARKERDEFINE, SC_MARKNUM_FOLDERTAIL, SC_MARK_LCORNER);
    SendSciMessage(SCI_MARKERDEFINE, SC_MARKNUM_FOLDEREND, SC_MARK_BOXPLUSCONNECTED);
    SendSciMessage(SCI_MARKERDEFINE, SC_MARKNUM_FOLDEROPENMID, SC_MARK_BOXMINUSCONNECTED);
    SendSciMessage(SCI_MARKERDEFINE, SC_MARKNUM_FOLDERMIDTAIL, SC_MARK_TCORNER);
    for (int32_t nMarker = SC_MARKNUM_FOLDEREND; nMarker <= SC_MARKNUM_FOLDEROPEN; ++nMarker) {
        SendSciMessage(
            SCI_MARKERSETBACK, static_cast<uintptr_t>(nMarker), ColorRefRGB(0x80, 0x80, 0x80));
        SendSciMessage(
            SCI_MARKERSETFORE, static_cast<uintptr_t>(nMarker), ColorRefRGB(0xFF, 0xFF, 0xFF));
    }
}

void ScintillaControl::SetCaretLineVisible(bool bVisible)
{
    SendSciMessage(SCI_SETCARETLINEVISIBLE, bVisible ? 1 : 0, 0);
    SendSciMessage(SCI_SETCARETLINEBACK, ColorRefRGB(0xF2, 0xF2, 0xF2), 0);
    SendSciMessage(SCI_SETCARETLINEBACKALPHA, 128, 0);
}

void ScintillaControl::SetZoom(int32_t nZoom)
{
    SendSciMessage(SCI_SETZOOM, static_cast<uintptr_t>(nZoom), 0);
}

int32_t ScintillaControl::GetZoom() const
{
    return static_cast<int32_t>(SendSciMessage(SCI_GETZOOM, 0, 0));
}

void ScintillaControl::SetDefaultStyle(
    const ui::UiColor &fore, const ui::UiColor &back, const ui::UiFont &font)
{
    m_defaultFore = fore;
    m_defaultBack = back;
    m_fontInfo = font;
    //Duilib 的字体大小是设备像素值；这里换算为不随 DPI 变化的 DIP 值保存
    const int32_t nScale = std::max<int32_t>(1, GetDisplayScalePercent());
    m_baseFontSizeDip = std::max<int32_t>(1, font.m_fontSize * 100 / nScale);
    m_hasDefaultStyle = true;
    ApplyDefaultStyleToScintilla();
}

void ScintillaControl::ApplyDefaultStyleToScintilla()
{
    if ((m_pSci == nullptr) || !m_hasDefaultStyle) {
        return;
    }
    //Scintilla 的样式大小单位为磅（1 磅 = 1/72 英寸）；LogPixelsY() 已包含 DPI 缩放，
    //因此这里用 DIP 值换算，字体大小会随 DPI 自动缩放（对应 Qt 的 QFont 磅值语义）
    const int32_t nSizeInPoints100 = static_cast<int32_t>(std::lround(m_baseFontSizeDip * 75.0));
    SendSciMessage(
        SCI_STYLESETSIZEFRACTIONAL,
        STYLE_DEFAULT,
        static_cast<uintptr_t>(std::max<int32_t>(1, nSizeInPoints100)));
    SendSciMessage(SCI_STYLESETFORE, STYLE_DEFAULT, m_defaultFore.GetARGB());
    SendSciMessage(SCI_STYLESETBACK, STYLE_DEFAULT, m_defaultBack.GetARGB());
    SendSciMessage(SCI_STYLESETBOLD, STYLE_DEFAULT, m_fontInfo.m_bBold ? 1 : 0);
    SendSciMessage(SCI_STYLESETITALIC, STYLE_DEFAULT, m_fontInfo.m_bItalic ? 1 : 0);
    SendSciMessage(SCI_STYLESETUNDERLINE, STYLE_DEFAULT, m_fontInfo.m_bUnderline ? 1 : 0);

    const std::string fontName = ui::StringConvert::TToUTF8(DString(m_fontInfo.m_fontName.c_str()));
    if (!fontName.empty()) {
        SendSciMessage(SCI_STYLESETFONT, STYLE_DEFAULT, reinterpret_cast<intptr_t>(fontName.c_str()));
    }
    //所有样式继承默认样式的字形设置
    SendSciMessage(SCI_STYLECLEARALL, 0, 0);
}

void ScintillaControl::SetStyle(
    int32_t styleId, const ui::UiColor &fore, const ui::UiColor &back, bool bBold, bool bItalic)
{
    SendSciMessage(SCI_STYLESETFORE, static_cast<uintptr_t>(styleId), fore.GetARGB());
    SendSciMessage(SCI_STYLESETBACK, static_cast<uintptr_t>(styleId), back.GetARGB());
    SendSciMessage(SCI_STYLESETBOLD, static_cast<uintptr_t>(styleId), bBold ? 1 : 0);
    SendSciMessage(SCI_STYLESETITALIC, static_cast<uintptr_t>(styleId), bItalic ? 1 : 0);
}

bool ScintillaControl::IsModified() const
{
    return SendSciMessage(SCI_GETMODIFY, 0, 0) != 0;
}

void ScintillaControl::SetSavePoint()
{
    SendSciMessage(SCI_SETSAVEPOINT, 0, 0);
}

void ScintillaControl::Undo()
{
    SendSciMessage(SCI_UNDO, 0, 0);
}

void ScintillaControl::Redo()
{
    SendSciMessage(SCI_REDO, 0, 0);
}

bool ScintillaControl::CanUndo() const
{
    return SendSciMessage(SCI_CANUNDO, 0, 0) != 0;
}

bool ScintillaControl::CanRedo() const
{
    return SendSciMessage(SCI_CANREDO, 0, 0) != 0;
}

void ScintillaControl::Cut()
{
    SendSciMessage(SCI_CUT, 0, 0);
}

void ScintillaControl::Copy()
{
    SendSciMessage(SCI_COPY, 0, 0);
}

void ScintillaControl::Paste()
{
    SendSciMessage(SCI_PASTE, 0, 0);
}

void ScintillaControl::SelectAll()
{
    SendSciMessage(SCI_SELECTALL, 0, 0);
}

void ScintillaControl::ShowAutoComplete(bool bShow, const std::string &itemList)
{
    if (!bShow) {
        SendSciMessage(SCI_AUTOCCANCEL, 0, 0);
        return;
    }
    if (itemList.empty()) {
        return;
    }
    SendSciMessage(SCI_AUTOCSETSEPARATOR, static_cast<uintptr_t>(' '), 0);
    SendSciMessage(SCI_AUTOCSHOW, 0, reinterpret_cast<intptr_t>(itemList.c_str()));
}

intptr_t ScintillaControl::SendSciMessage(
    unsigned int message, uintptr_t wParam, intptr_t lParam) const
{
    if (m_pSci == nullptr) {
        return 0;
    }
    return const_cast<ScintillaDui *>(m_pSci.get())->SendSciMessage(message, wParam, lParam);
}

void ScintillaControl::SetTextChangedCallback(const TextChangedCallback &callback)
{
    m_textChangedCallback = callback;
}

void ScintillaControl::SetSavePointCallback(const SavePointCallback &callback)
{
    m_savePointCallback = callback;
}

void ScintillaControl::SetNotifyCallback(const NotifyCallback &callback)
{
    m_notifyCallback = callback;
}

void ScintillaControl::OnScintillaTextChanged()
{
    if (m_textChangedCallback) {
        m_textChangedCallback();
    }
    SendEvent(ui::kEventTextChanged);
}

void ScintillaControl::OnScintillaSavePoint(bool bAtSavePoint)
{
    if (m_savePointCallback) {
        m_savePointCallback(bAtSavePoint);
    }
}

void ScintillaControl::OnScintillaNotify(const SCNotification &notification)
{
    switch (notification.nmhdr.code) {
    case SCN_SAVEPOINTREACHED:
        OnScintillaSavePoint(true);
        break;
    case SCN_SAVEPOINTLEFT:
        OnScintillaSavePoint(false);
        break;
    case SCN_UPDATEUI:
    case SCN_ZOOM:
    case SCN_PAINTED:
        //绘制/通知过程中不能重新布局，延迟更新
        RequestScrollBarUpdate();
        break;
    default:
        break;
    }

    if (m_notifyCallback) {
        m_notifyCallback(notification);
    }
}

void ScintillaControl::SetMouseCaptureFromScintilla(bool bOn)
{
    //鼠标捕获由 ui::Window 在鼠标按下/弹起时自动完成（与 Qt 平台层一致），
    //这里只记录 Scintilla 需要的状态，避免破坏 Duilib 自身的捕获逻辑
    m_bMouseCaptured = bOn;
}

void ScintillaControl::UpdateScrollBarsFromScintilla()
{
    if ((m_pSci == nullptr) || (m_pVScrollBar == nullptr) || (m_pHScrollBar == nullptr)) {
        return;
    }
    if (m_bSyncingScrollBars) {
        //避免内容区更新与滚动条同步相互递归
        return;
    }
    m_bSyncingScrollBars = true;

    UpdateTextArea();
    SyncScrollBarState();
    //滚动条可见性可能刚刚发生变化，重新计算一次内容区
    UpdateTextArea();

    m_bSyncingScrollBars = false;
}

void ScintillaControl::RequestScrollBarUpdate()
{
    if (m_bScrollBarUpdatePending || (GetWindow() == nullptr)) {
        return;
    }
    m_bScrollBarUpdatePending = true;
    std::weak_ptr<ui::WeakFlag> weakFlag = GetWeakFlag();
    ScintillaControl *pThis = this;
    //Scintilla 会在绘制过程中请求更新滚动条；延迟到当前消息处理完成后再执行，
    //避免在绘制/事件派发过程中重新布局（与 Qt 平台层的事件队列语义一致）。
    ui::GlobalManager::Instance().Thread().PostTask(ui::kThreadUI, [pThis, weakFlag]() {
        if (weakFlag.expired()) {
            return;
        }
        pThis->m_bScrollBarUpdatePending = false;
        pThis->UpdateScrollBarsFromScintilla();
    });
}

void ScintillaControl::SyncScrollBarState()
{
    if ((m_pSci == nullptr) || (m_pVScrollBar == nullptr) || (m_pHScrollBar == nullptr)) {
        return;
    }
    const int32_t nLineHeight = std::max<int32_t>(1, m_pSci->GetLineHeightDui());

    //垂直滚动条：范围为像素值（与 ui::ScrollBar 的滑块比例计算保持一致）
    const int32_t nVMax = m_pSci->GetVerticalScrollRangeDui();
    m_pVScrollBar->SetScrollRange(nVMax);
    m_pVScrollBar->SetScrollPos(m_pSci->GetVerticalScrollPosDui());
    m_pVScrollBar->SetLineSize(nLineHeight, false);
    m_pVScrollBar->SetThumbMinLength(ScaledValue(24), false);
    m_pVScrollBar->SetVisible(m_pSci->IsVerticalScrollBarVisibleDui() && (nVMax > 0));

    //水平滚动条：自动换行模式下不显示
    const int32_t nHMax = m_pSci->GetHorizontalScrollRangeDui();
    m_pHScrollBar->SetScrollRange(nHMax);
    m_pHScrollBar->SetScrollPos(m_pSci->GetHorizontalScrollPosDui());
    m_pHScrollBar->SetLineSize(std::max<int32_t>(1, nLineHeight / 2), false);
    m_pHScrollBar->SetThumbMinLength(ScaledValue(24), false);
    m_pHScrollBar->SetVisible(
        m_pSci->IsHorizontalScrollBarVisibleDui() && !m_pSci->IsWrappingDui() && (nHMax > 0));
}

void ScintillaControl::OnScrollBarPosChanged(bool bVertical, int64_t nPos)
{
    if ((m_pSci == nullptr) || m_bSyncingScrollBars) {
        return;
    }
    if (bVertical) {
        m_pSci->OnVerticalScrollBarPosDui(nPos);
    } else {
        m_pSci->OnHorizontalScrollBarPosDui(nPos);
    }
}

void ScintillaControl::CreateCallTipWindowDui(const ui::UiRect &rc)
{
    if (m_pSci == nullptr) {
        return;
    }
    ui::UiPoint ptScreen(rc.left, rc.top);
    ui::Window *pWindow = GetWindow();
    const ui::UiRect rcArea = m_rcTextArea;
    ptScreen.x += rcArea.left;
    ptScreen.y += rcArea.top;
    if (pWindow == nullptr) {
        return;
    }
    pWindow->ClientToScreen(ptScreen);
    GetOrCreateCallTip(m_pSci.get());
    if (m_pCallTip == nullptr) {
        return;
    }
    m_pCallTip->Show(
        ui::UiRect(
            ptScreen.x,
            ptScreen.y,
            ptScreen.x + std::max<int32_t>(1, rc.Width()),
            ptScreen.y + std::max<int32_t>(1, rc.Height())));
}

ScintillaDuiCallTip *ScintillaControl::GetOrCreateCallTip(ScintillaDui *pSci)
{
    if (m_pCallTip == nullptr) {
        m_pCallTip = std::make_unique<ScintillaDuiCallTip>(this, pSci);
    }
    return m_pCallTip.get();
}

void ScintillaControl::ShowPopupMenuDui(const ui::UiPoint &ptScreen)
{
    if (m_popupMenuItems.empty() || (GetWindow() == nullptr)) {
        m_popupMenuItems.clear();
        return;
    }
    //动态生成菜单 XML（对应 Qt 的 QMenu::addAction），使用 DUI 主题中的菜单样式
    DString xml;
    xml += _T("<Window shadow_type=\"menu_round\" shadow_border_size=\"1\" ")
           _T("shadow_border_color=\"border_window\">\n");
    xml += _T("  <MenuListBox class=\"menu\" name=\"scintilla_menu\" padding=\"0,4,0,4\">\n");
    for (const auto &item : m_popupMenuItems) {
        if (item.first.empty()) {
            xml += _T("    <Box class=\"menu_split_box\"><Control ")
                   _T("class=\"menu_split_line\"/></Box>\n");
            continue;
        }
        xml += _T("    <MenuItem class=\"menu_element\" name=\"sci_cmd_");
        xml += ui::StringUtil::Int32ToString(item.second);
        xml += _T("\">\n      <Label class=\"menu_text\" text=\"");
        xml += EscapeXmlText(ui::StringConvert::UTF8ToT(item.first));
        xml += _T("\"/>\n    </MenuItem>\n");
    }
    xml += _T("  </MenuListBox>\n</Window>\n");
    m_popupMenuItems.clear();

    ui::Menu *pMenu = new ui::Menu(GetWindow());
    pMenu->SetSkinFolder(DString(DUILIB_PUBLIC_RES_DIR) + _T("/menu/"));
    std::weak_ptr<ui::WeakFlag> editorFlag = GetWeakFlag();
    ScintillaControl *pThis = this;
    pMenu->AttachMenuItemActivated([pThis, editorFlag](
                                       const DString & /*menuName*/,
                                       int32_t /*nMenuLevel*/,
                                       const DString &itemName,
                                       size_t /*nItemIndex*/) {
        if (editorFlag.expired()) {
            return;
        }
        //菜单项名称中携带 Scintilla 命令ID
        constexpr const DString::value_type *kPrefix = _T("sci_cmd_");
        constexpr size_t nPrefixLen = 8;
        if ((itemName.size() > nPrefixLen) && (itemName.compare(0, nPrefixLen, kPrefix) == 0)) {
            pThis->ExecutePopupCommand(ui::StringUtil::StringToInt32(itemName.substr(nPrefixLen)));
        }
    });
    pMenu->ShowMenu(xml, ptScreen, ui::MenuPopupPosType::RIGHT_BOTTOM);
}

void ScintillaControl::AddPopupMenuItem(const std::string &label, int cmd, bool /*enabled*/)
{
    m_popupMenuItems.emplace_back(label, cmd);
}

void ScintillaControl::AddToPopupMenu(const char *label, int cmd, bool enabled)
{
    AddPopupMenuItem(label != nullptr ? std::string(label) : std::string(), cmd, enabled);
}

ui::UiPoint ScintillaControl::GetScintillaPoint(const ui::EventArgs &msg) const
{
    const ui::UiRect rcArea = m_rcTextArea;
    return ui::UiPoint(msg.ptMouse.x - rcArea.left, msg.ptMouse.y - rcArea.top);
}

ui::UiRect ScintillaControl::GetTextAreaRect() const
{
    return m_rcTextArea;
}

void ScintillaControl::UpdateTextArea()
{
    if (m_pSci == nullptr) {
        return;
    }
    const ui::UiRect rc = GetRect();
    const bool bVScroll = (m_pVScrollBar != nullptr) && m_pVScrollBar->IsVisible();
    const bool bHScroll = (m_pHScrollBar != nullptr) && m_pHScrollBar->IsVisible();
    const int32_t nVWidth = bVScroll
                                ? std::max<int32_t>(0, m_pVScrollBar->GetFixedWidth().GetInt32())
                                : 0;
    const int32_t nHHeight = bHScroll
                                 ? std::max<int32_t>(0, m_pHScrollBar->GetFixedHeight().GetInt32())
                                 : 0;

    ui::UiRect rcArea(rc.left, rc.top, rc.right - nVWidth, rc.bottom - nHHeight);
    rcArea.Validate();
    m_rcTextArea = rcArea;

    if (bVScroll) {
        m_pVScrollBar->SetPos(
            ui::UiRect(rc.right - nVWidth, rc.top, rc.right, rc.bottom - nHHeight));
    }
    if (bHScroll) {
        m_pHScrollBar->SetPos(
            ui::UiRect(rc.left, rc.bottom - nHHeight, rc.right - nVWidth, rc.bottom));
    }
    //Scintilla 使用内容区局部坐标
    m_pSci->SetClientRectDui(ui::UiRect(0, 0, rcArea.Width(), rcArea.Height()));
}

void ScintillaControl::CreateScrollBars()
{
    if ((m_pVScrollBar != nullptr) || (GetWindow() == nullptr)) {
        return;
    }
    m_pVScrollBar.reset(new ScintillaScrollBar(GetWindow(), this, true));
    m_pVScrollBar->SetClass(_T("vscrollbar"));
    m_pVScrollBar->SetHorizontal(false);
    m_pVScrollBar->SetVisible(false);
    m_pVScrollBar->SetScrollRange(0);
    m_pVScrollBar->SetParent(this);
    m_pVScrollBar->SetWindow(GetWindow());
    m_pVScrollBar->Init();

    m_pHScrollBar.reset(new ScintillaScrollBar(GetWindow(), this, false));
    m_pHScrollBar->SetClass(_T("hscrollbar"));
    m_pHScrollBar->SetHorizontal(true);
    m_pHScrollBar->SetVisible(false);
    m_pHScrollBar->SetScrollRange(0);
    m_pHScrollBar->SetParent(this);
    m_pHScrollBar->SetWindow(GetWindow());
    m_pHScrollBar->Init();
}

void ScintillaControl::ClosePopupWindows()
{
    if (m_pCallTip != nullptr) {
        m_pCallTip->Hide();
    }
    if (m_pSci != nullptr) {
        m_pSci->CancelAutoCompleteDui();
    }
}

void ScintillaControl::DisposePopupWindows()
{
    m_pCallTip.reset();
}

void ScintillaControl::UpdateImePosition()
{
    if ((m_pSci == nullptr) || (GetWindow() == nullptr)) {
        return;
    }
    const ui::UiPoint ptCaret = m_pSci->GetCaretLocationDui();
    const ui::UiRect rcArea = m_rcTextArea;
    const int32_t nCaretHeight = std::max<int32_t>(1, m_pSci->GetLineHeightDui());
    ui::UiRect rcInput(
        rcArea.left + ptCaret.x,
        rcArea.top + ptCaret.y,
        rcArea.left + ptCaret.x + ScaledValue(2),
        rcArea.top + ptCaret.y + nCaretHeight);
    GetWindow()->NativeWnd()->SetTextInputArea(&rcInput, 0);
}

void ScintillaControl::ExecutePopupCommand(int32_t nCommand)
{
    if (m_pSci != nullptr) {
        m_pSci->RunScintillaCommand(nCommand);
    }
}

void ScintillaControl::ShowContextMenu(const ui::EventArgs &msg)
{
    if (m_pSci == nullptr) {
        return;
    }
    const ui::UiPoint pt = GetScintillaPoint(msg);
    const Scintilla::Internal::Point sciPt(
        static_cast<Scintilla::Internal::XYPOSITION>(pt.x),
        static_cast<Scintilla::Internal::XYPOSITION>(pt.y));
    //与 Qt 的 contextMenuEvent 一致：右键点击在选区之外时先移动插入点
    if (!m_pSci->ShouldDisplayPopupDui(sciPt)) {
        return;
    }
    if (!m_pSci->PointInSelectionDui(sciPt)) {
        m_pSci->SetEmptySelectionDui(sciPt);
    }
    //菜单坐标使用屏幕坐标
    ui::UiPoint ptScreen(msg.ptMouse.x, msg.ptMouse.y);
    ui::Window *pWindow = GetWindow();
    if (pWindow == nullptr) {
        return;
    }
    pWindow->ClientToScreen(ptScreen);
    m_popupMenuItems.clear();
    m_pSci->ContextMenuDui(
        Scintilla::Internal::Point(
            static_cast<Scintilla::Internal::XYPOSITION>(ptScreen.x),
            static_cast<Scintilla::Internal::XYPOSITION>(ptScreen.y)));
}

bool ScintillaControl::ButtonDown(const ui::EventArgs &msg)
{
    //点击编辑器时关闭 CallTip/自动补全列表
    ClosePopupWindows();
    if (m_pSci != nullptr) {
        const ui::UiPoint pt = GetScintillaPoint(msg);
        m_pSci->ButtonDownDui(
            Scintilla::Internal::Point(pt.x, pt.y),
            CurrentMouseTimeMs(),
            ToScintillaModifiers(msg.modifierKey));
    }
    return true;
}

bool ScintillaControl::ButtonDoubleClick(const ui::EventArgs &msg)
{
    //Scintilla 自己完成双击/三击判定，这里与按下事件走同一条路径（对应 Qt 的 mouseDoubleClickEvent）
    return ButtonDown(msg);
}

bool ScintillaControl::ButtonUp(const ui::EventArgs &msg)
{
    if (m_pSci != nullptr) {
        const ui::UiPoint pt = GetScintillaPoint(msg);
        m_pSci->ButtonUpDui(
            Scintilla::Internal::Point(pt.x, pt.y),
            CurrentMouseTimeMs(),
            ToScintillaModifiers(msg.modifierKey));
    }
    return true;
}

bool ScintillaControl::RButtonDown(const ui::EventArgs &msg)
{
    ClosePopupWindows();
    if (m_pSci != nullptr) {
        const ui::UiPoint pt = GetScintillaPoint(msg);
        m_pSci->RightButtonDownDui(
            Scintilla::Internal::Point(pt.x, pt.y),
            CurrentMouseTimeMs(),
            ToScintillaModifiers(msg.modifierKey));
    }
    return true;
}

bool ScintillaControl::RButtonUp(const ui::EventArgs &msg)
{
    ShowContextMenu(msg);
    return true;
}

bool ScintillaControl::RButtonDoubleClick(const ui::EventArgs &msg)
{
    return RButtonDown(msg);
}

bool ScintillaControl::MouseMove(const ui::EventArgs &msg)
{
    if (m_pSci != nullptr) {
        const ui::UiPoint pt = GetScintillaPoint(msg);
        m_pSci->ButtonMoveDui(
            Scintilla::Internal::Point(pt.x, pt.y),
            CurrentMouseTimeMs(),
            ToScintillaModifiers(msg.modifierKey));
    }
    return true;
}

bool ScintillaControl::MouseLeave(const ui::EventArgs & /*msg*/)
{
    if (m_pSci != nullptr) {
        m_pSci->MouseLeaveDui();
    }
    return true;
}

bool ScintillaControl::MouseWheel(const ui::EventArgs &msg)
{
    if (m_pSci == nullptr) {
        return true;
    }
    //对应 ScintillaWin 的 WM_MOUSEWHEEL：Ctrl+滚轮缩放，Shift+滚轮水平滚动，否则按行滚动
    const int32_t nDelta = static_cast<int32_t>(msg.eventData);
    constexpr int32_t kWheelDelta = 120;
    const int32_t nActions = nDelta / kWheelDelta;
    if (nActions == 0) {
        return true;
    }
    const int32_t nLinesPerScroll = std::max<int32_t>(1, m_pSci->GetLinesOnScreenDui() / 5);
    if ((msg.modifierKey & ui::ModifierKey::kControl) != 0) {
        if (nActions > 0) {
            m_pSci->RunScintillaCommand(static_cast<int>(Scintilla::Message::ZoomIn));
        } else {
            m_pSci->RunScintillaCommand(static_cast<int>(Scintilla::Message::ZoomOut));
        }
    } else if ((msg.modifierKey & ui::ModifierKey::kShift) != 0) {
        const int32_t nCharWidth = std::max<int32_t>(
            1,
            static_cast<int32_t>(m_pSci->SendSciMessage(
                SCI_TEXTWIDTH, STYLE_DEFAULT, reinterpret_cast<intptr_t>(" "))));
        m_pSci->OnHorizontalScrollBarPosDui(
            m_pSci->GetHorizontalScrollPosDui() - nActions * nCharWidth * 3);
    } else {
        m_pSci->LineScrollDui(-nActions * nLinesPerScroll);
    }
    return true;
}

bool ScintillaControl::OnChar(const ui::EventArgs &msg)
{
    if (m_pSci != nullptr) {
        const std::string text = ToScintillaUTF8(msg);
        if (!text.empty()) {
            m_pSci->AddTextDui(text);
        }
    }
    return true;
}

bool ScintillaControl::OnKeyDown(const ui::EventArgs &msg)
{
    if (m_pSci == nullptr) {
        return false;
    }
    //对应 Qt 的 keyPressEvent：按键先交给 Scintilla，未被消费时交回框架
    const Scintilla::Keys key = KeyTranslate(static_cast<uint32_t>(msg.vkCode));
    const Scintilla::KeyMod modifiers = ToScintillaModifiers(msg.modifierKey);
    bool consumed = false;
    m_pSci->KeyDownDui(key, modifiers, &consumed);
    return consumed;
}

bool ScintillaControl::OnSetFocus(const ui::EventArgs &msg)
{
    const bool bRet = BaseClass::OnSetFocus(msg);
    if (m_pSci != nullptr) {
        //关键：焦点状态必须进入 Scintilla 内部，插入符才会显示并闪烁
        m_pSci->SetFocusStateDui(true);
        GetWindow()->NativeWnd()->SetImeOpenStatus(true);
        UpdateImePosition();
    }
    return bRet;
}

bool ScintillaControl::OnKillFocus(const ui::EventArgs &msg)
{
    const bool bRet = BaseClass::OnKillFocus(msg);
    if (m_pSci != nullptr) {
        m_pSci->SetFocusStateDui(false);
    }
    ClosePopupWindows();
    return bRet;
}

bool ScintillaControl::OnCaptureChanged(const ui::EventArgs & /*msg*/)
{
    if (m_pSci != nullptr) {
        m_pSci->SetMouseCaptureDui(false);
        m_pSci->MouseLeaveDui();
    }
    m_bMouseCaptured = false;
    return true;
}

bool ScintillaControl::OnImeStartComposition(const ui::EventArgs & /*msg*/)
{
    UpdateImePosition();
    return true;
}

bool ScintillaControl::OnImeComposition(const ui::EventArgs & /*msg*/)
{
    UpdateImePosition();
    return false; //继续交给父控件处理
}

bool ScintillaControl::OnImeEndComposition(const ui::EventArgs & /*msg*/)
{
    if (m_pSci != nullptr) {
        m_pSci->EnsureCaretVisibleDui();
        m_pSci->SetFocusStateDui(true);
    }
    return true;
}

void ScintillaControl::OnSetVisible(bool bChanged)
{
    BaseClass::OnSetVisible(bChanged);
    if (bChanged) {
        UpdateScrollBarsFromScintilla();
        Invalidate();
    }
}

} // namespace duilib::ext::scintilla
