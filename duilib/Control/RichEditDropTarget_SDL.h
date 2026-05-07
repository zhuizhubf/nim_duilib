#ifndef UI_CONTROL_RICHEDIT_DROP_TARGET_SDL_H_
#define UI_CONTROL_RICHEDIT_DROP_TARGET_SDL_H_

#include "duilib/Core/ControlDropTarget.h"

#ifdef DUILIB_BUILD_FOR_SDL

namespace ui 
{
class RichEdit2;
class RichEditDropTarget_SDL : public ControlDropTarget_SDL
{
public:
    explicit RichEditDropTarget_SDL(RichEdit2* pRichEdit);

public:
    virtual int32_t OnDropBegin(const UiPoint& pt) override;
    virtual void OnDropPosition(const UiPoint& pt) override;
    virtual void OnDropTexts(const std::vector<DString>& textList, const UiPoint& pt) override;
    virtual void OnDropFiles(const DString& source, const std::vector<DString>& fileList, const UiPoint& pt) override;
    virtual void OnDropLeave() override;

private:
    RichEdit2* m_pRichEdit;
    std::vector<DString> m_dropTextList;
    std::vector<DString> m_dropFileList;
    int32_t m_nStartChar;
    int32_t m_nEndChar;
    UiSize64 m_scrollPos;
};

} // namespace ui

#endif // DUILIB_BUILD_FOR_SDL

#endif // UI_CONTROL_RICHEDIT_DROP_TARGET_SDL_H_
