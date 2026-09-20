#include "MainForm.h"

#include "duilib/Utils/StringUtil.h"
#include "duilib_scintilla.h"

using namespace ui;

MainForm::MainForm() = default;

MainForm::~MainForm() = default;

DString MainForm::GetSkinFolder()
{
    return _T("scintilla");
}

DString MainForm::GetSkinFile()
{
    return _T("scintilla.xml");
}

void MainForm::OnInitWindow()
{
    BaseClass::OnInitWindow();

    //编辑器控件在 XML 中创建（控件类型名为 ScintillaControl）
    m_pEditor = dynamic_cast<duilib::ext::scintilla::ScintillaControl *>(
        FindControl(_T("scintilla_editor")));
    if (m_pEditor == nullptr) {
        return;
    }
    InitEditor();
    UpdateStatusText();
    //编辑器启动时获取焦点，插入符立即显示
    m_pEditor->SetFocus();
}

void MainForm::InitEditor()
{
    if (m_pEditor == nullptr) {
        return;
    }

    //默认样式：字体大小是当前 DPI 下的像素值
    ui::UiFont font;
    font.m_fontName = _T("Consolas");
    font.m_fontSize = m_pEditor->Dpi().GetScaleInt(14);
    m_pEditor->SetDefaultStyle(
        ui::UiColor(0xFF, 0x1E, 0x1E, 0x1E), ui::UiColor(0xFF, 0xFF, 0xFF, 0xFF), font);

    //C++ 语法高亮的常用样式（样式编号参见 Lexilla 的 LexCPP 定义）
    m_pEditor->SetStyle(1, ui::UiColor(0xFF, 0x00, 0x80, 0x00), ui::UiColor(0xFF, 0xFF, 0xFF, 0xFF));
    m_pEditor->SetStyle(2, ui::UiColor(0xFF, 0x00, 0x80, 0x00), ui::UiColor(0xFF, 0xFF, 0xFF, 0xFF));
    m_pEditor->SetStyle(3, ui::UiColor(0xFF, 0x00, 0x80, 0x00), ui::UiColor(0xFF, 0xFF, 0xFF, 0xFF));
    m_pEditor->SetStyle(4, ui::UiColor(0xFF, 0x80, 0x80, 0x80), ui::UiColor(0xFF, 0xFF, 0xFF, 0xFF));
    m_pEditor
        ->SetStyle(5, ui::UiColor(0xFF, 0x00, 0x00, 0xC0), ui::UiColor(0xFF, 0xFF, 0xFF, 0xFF), true);
    m_pEditor->SetStyle(6, ui::UiColor(0xFF, 0x80, 0x00, 0x80), ui::UiColor(0xFF, 0xFF, 0xFF, 0xFF));
    m_pEditor->SetStyle(7, ui::UiColor(0xFF, 0xC0, 0x60, 0x00), ui::UiColor(0xFF, 0xFF, 0xFF, 0xFF));
    m_pEditor->SetStyle(10, ui::UiColor(0xFF, 0xC0, 0x60, 0x00), ui::UiColor(0xFF, 0xFF, 0xFF, 0xFF));
    m_pEditor->SetStyle(13, ui::UiColor(0xFF, 0x00, 0x60, 0xA0), ui::UiColor(0xFF, 0xFF, 0xFF, 0xFF));

    m_pEditor->SetLexer("cpp");
    m_pEditor->SetTabWidth(4);
    m_pEditor->SetUseTabs(false);
    m_pEditor->SetLineNumbersVisible(true);
    m_pEditor->SetFoldingVisible(true);
    m_pEditor->SetWordWrap(false);

    m_pEditor->SetTextUTF8(
        "// DUI native Scintilla demo (duilib-scintilla extension)\n"
        "#include <iostream>\n"
        "#include <vector>\n\n"
        "static int Sum(const std::vector<int>& values)\n"
        "{\n"
        "    int total = 0;\n"
        "    for (int value : values) {\n"
        "        total += value;\n"
        "    }\n"
        "    return total;\n"
        "}\n\n"
        "int main()\n"
        "{\n"
        "    //Ctrl+滚轮可以缩放，Ctrl+空格可以触发自动补全\n"
        "    std::cout << \"Hello, nim_duilib!\" << std::endl;\n"
        "    return 0;\n"
        "}\n");
    m_pEditor->SetSavePoint();

    //光标位置/行列变化时刷新状态栏；输入字符时刷新“已修改”标记
    m_pEditor->SetNotifyCallback([this](const SCNotification &notification) {
        if ((notification.nmhdr.code == SCN_CHARADDED) || (notification.nmhdr.code == SCN_UPDATEUI)
            || (notification.nmhdr.code == SCN_SAVEPOINTREACHED)
            || (notification.nmhdr.code == SCN_SAVEPOINTLEFT)) {
            UpdateStatusText();
        }
    });
}

void MainForm::UpdateStatusText()
{
    if (m_pEditor == nullptr) {
        return;
    }
    Label *pStatus = dynamic_cast<Label *>(FindControl(_T("scintilla_status")));
    if (pStatus == nullptr) {
        return;
    }
    const intptr_t nLine
        = m_pEditor
              ->SendSciMessage(SCI_LINEFROMPOSITION, m_pEditor->SendSciMessage(SCI_GETCURRENTPOS));
    const intptr_t nColumn
        = m_pEditor->SendSciMessage(SCI_GETCOLUMN, m_pEditor->SendSciMessage(SCI_GETCURRENTPOS));
    DString text;
    text += _T("Lexer: cpp    行: ");
    text += ui::StringUtil::Int32ToString(static_cast<int32_t>(nLine) + 1);
    text += _T("    列: ");
    text += ui::StringUtil::Int32ToString(static_cast<int32_t>(nColumn) + 1);
    text += m_pEditor->IsModified() ? _T("    已修改") : _T("");
    pStatus->SetText(text);
}

LRESULT MainForm::OnKeyDownMsg(
    ui::VirtualKeyCode vkCode, uint32_t modifierKey, const ui::NativeMsg &nativeMsg, bool &bHandled)
{
    //Ctrl+空格：显示自动补全列表（对应 SCI_AUTOCSHOW）
    if ((m_pEditor != nullptr) && (vkCode == ui::kVK_SPACE)
        && ((modifierKey & ui::ModifierKey::kControl) != 0)) {
        m_pEditor->ShowAutoComplete(
            true,
            "std::vector std::string std::cout std::endl std::map std::unique_ptr"
            " for while return const static struct class");
        bHandled = true;
        return 0;
    }
    return BaseClass::OnKeyDownMsg(vkCode, modifierKey, nativeMsg, bHandled);
}
