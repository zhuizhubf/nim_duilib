#ifndef UI_CONTROL_RICHEDIT_DROP_TARGET_WINDOWS_H_
#define UI_CONTROL_RICHEDIT_DROP_TARGET_WINDOWS_H_

#include "duilib/Core/ControlDropTarget.h"

#if defined (DUILIB_BUILD_FOR_WIN) && !defined (DUILIB_BUILD_FOR_SDL)

namespace ui 
{
class RichEdit2;
class RichEditDropTarget_Windows : public ControlDropTarget_Windows
{
public:
    explicit RichEditDropTarget_Windows(RichEdit2* pRichEdit);
    virtual ~RichEditDropTarget_Windows() override;

public:
    virtual int32_t DragEnter(void* pDataObj, uint32_t grfKeyState, const UiPoint& pt, uint32_t* pdwEffect) override;
    virtual int32_t DragOver(uint32_t grfKeyState, const UiPoint& pt, uint32_t* pdwEffect) override;
    virtual int32_t DragLeave(void) override;
    virtual int32_t Drop(void* pDataObj, uint32_t grfKeyState, const UiPoint& pt, uint32_t* pdwEffect) override;

private:
    void ClearDragStatus();

    /** 检查文本是否满足拖入条件
    */
    bool CheckDropText() const;

    /** 检查是否需要滚动视图，按需滚动
    */
    void CheckTextScroll(const UiPoint& clientPt);

private:
    RichEdit2* m_pRichEdit;
    void* m_pDataObj;
    std::vector<DString> m_dropTextList;
    std::vector<DString> m_dropFileList;
    int32_t m_nStartChar;
    int32_t m_nEndChar;
    UiSize64 m_scrollPos;
};

} // namespace ui

#endif // defined (DUILIB_BUILD_FOR_WIN) && !defined (DUILIB_BUILD_FOR_SDL)
#endif // UI_CONTROL_RICHEDIT_DROP_TARGET_WINDOWS_H_
