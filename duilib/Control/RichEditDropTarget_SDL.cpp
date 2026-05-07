#include "RichEditDropTarget_SDL.h"

#ifdef DUILIB_BUILD_FOR_SDL

#include "duilib/Control/RichEdit2.h"
#include "duilib/Core/ControlDropTargetImpl_Windows.h"
#include "duilib/Core/ControlDropTargetUtils.h"

namespace ui 
{
RichEditDropTarget_SDL::RichEditDropTarget_SDL(RichEdit2* pRichEdit) :
    m_pRichEdit(pRichEdit),
    m_nStartChar(0),
    m_nEndChar(0)
{
}

int32_t RichEditDropTarget_SDL::OnDropBegin(const UiPoint& pt)
{
    if (m_pRichEdit == nullptr) {
        return S_FALSE;
    }

    m_dropTextList.clear();
    m_dropFileList.clear();

    m_pRichEdit->GetSel(m_nStartChar, m_nEndChar);
    m_scrollPos = m_pRichEdit->GetScrollPos();

    return S_OK;
}

void RichEditDropTarget_SDL::OnDropPosition(const UiPoint& pt)
{
    if (m_pRichEdit == nullptr) {
        return;
    }

    if (!m_dropTextList.empty()) {
        if (!m_pRichEdit->IsMultiLine()) {
            if (m_dropTextList.size() > 1) {
                return;
            }
        }
        if (m_pRichEdit->IsNumberOnly()) {
            if (m_dropTextList.size() > 1) {
                return;
            }

            DString dropText = m_dropTextList.front();
            if (!dropText.empty()) {
                size_t count = dropText.size();
                for (size_t index = 0; index < count; ++index) {
                    if (dropText[index] == L'\0') {
                        break;
                    }
                    if ((dropText[index] > L'9') || (dropText[index] < L'0')) {
                        return;
                    }
                }
            }
        }
        DString limitChars = m_pRichEdit->GetLimitChars();
        if (!limitChars.empty()) {
            for (const DString& dropText : m_dropTextList) {
                size_t count = dropText.size();
                for (size_t index = 0; index < count; ++index) {
                    if (dropText[index] == L'\0') {
                        break;
                    }
                    bool bMatch = false;
                    for (const DString::value_type ch : limitChars) {
                        if (ch == dropText[index]) {
                            bMatch = true;
                            break;
                        }
                    }
                    if (!bMatch) {
                        return;
                    }
                }
            }
        }
    }

    if (!m_pRichEdit->IsFocused()) {
        m_pRichEdit->SetFocus();
    }
    int32_t pos = m_pRichEdit->CharFromPos(pt);
    if (pos >= 0) {
        UiPoint charPt = m_pRichEdit->PosFromChar(pos);
        m_pRichEdit->SetCaretPos(charPt.x, charPt.y);
        m_pRichEdit->ShowCaret(true);
    }
}

void RichEditDropTarget_SDL::OnDropTexts(const std::vector<DString>& textList, const UiPoint& pt)
{
    m_dropTextList = textList;

    if (m_pRichEdit == nullptr) {
        return;
    }

    if (m_pRichEdit->IsReadOnly() || !m_pRichEdit->IsEnabled()) {
        return;
    }

    if (!m_pRichEdit->IsMultiLine()) {
        if (textList.size() > 1) {
            return;
        }
    }

    if (m_pRichEdit->IsNumberOnly()) {
        if (textList.size() > 1) {
            return;
        }

        DString dropText = textList.front();
        if (!dropText.empty()) {
            size_t count = dropText.size();
            for (size_t index = 0; index < count; ++index) {
                if (dropText[index] == L'\0') {
                    break;
                }
                if ((dropText[index] > L'9') || (dropText[index] < L'0')) {
                    return;
                }
            }
        }
    }

    DString limitChars = m_pRichEdit->GetLimitChars();
    if (!limitChars.empty()) {
        for (const DString& dropText : textList) {
            size_t count = dropText.size();
            for (size_t index = 0; index < count; ++index) {
                if (dropText[index] == L'\0') {
                    break;
                }
                bool bMatch = false;
                for (const DString::value_type ch : limitChars) {
                    if (ch == dropText[index]) {
                        bMatch = true;
                        break;
                    }
                }
                if (!bMatch) {
                    return;
                }
            }
        }
    }

    DString dropText;
    for (size_t i = 0; i < textList.size(); ++i) {
        if (i > 0) {
            dropText += _T("\n");
        }
        dropText += textList[i];
    }

    m_pRichEdit->ReplaceSel(dropText, true);
}

void RichEditDropTarget_SDL::OnDropFiles(const DString& /*source*/, const std::vector<DString>& fileList, const UiPoint& pt)
{
    m_dropFileList = fileList;

    if (m_pRichEdit == nullptr) {
        return;
    }

    if (!m_pRichEdit->IsEnableDropFile()) {
        return;
    }

    DString fileTypes = m_pRichEdit->GetDropFileTypes();
    if (!ControlDropTargetUtils::IsFilteredFileTypes(fileTypes, fileList)) {
        return;
    }

    std::vector<DString> filteredFileList = fileList;
    ControlDropTargetUtils::RemoveUnsupportedFiles(filteredFileList, fileTypes);
    if (filteredFileList.empty()) {
        return;
    }

    ControlDropData_SDL data;
    data.m_ptClientX = pt.x;
    data.m_ptClientY = pt.y;
    data.m_bTextData = false;
    data.m_source = _T("");
    data.m_fileList = filteredFileList;
    data.m_bHandled = false;

    EventArgs msg;
    msg.SetSender(m_pRichEdit);
    msg.eventType = EventType::kEventDropData;
    msg.vkCode = VirtualKeyCode::kVK_None;
    msg.wParam = kControlDropTypeSDL;
    msg.lParam = (LPARAM)&data;
    msg.ptMouse = pt;
    msg.modifierKey = 0;
    msg.eventData = 0;

    m_pRichEdit->SendEventMsg(msg);
}

void RichEditDropTarget_SDL::OnDropLeave()
{
    m_dropTextList.clear();
    m_dropFileList.clear();

    if (m_pRichEdit != nullptr) {
        m_pRichEdit->SetSel(m_nStartChar, m_nEndChar);
        m_pRichEdit->SetScrollPos(m_scrollPos);
    }
}

} // namespace ui

#endif // DUILIB_BUILD_FOR_SDL
