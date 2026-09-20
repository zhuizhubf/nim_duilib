#ifndef EXAMPLES_SCINTILLA_DEMO_MAIN_FORM_H_
#define EXAMPLES_SCINTILLA_DEMO_MAIN_FORM_H_

// duilib
#include "duilib/duilib.h"

namespace duilib::ext::scintilla {
class ScintillaControl;
}

/** Scintilla 扩展示例的主窗口
 */
class MainForm : public ui::WindowImplBase
{
    typedef ui::WindowImplBase BaseClass;

public:
    MainForm();
    virtual ~MainForm() override;

    /** 资源相关接口
     * GetSkinFolder 接口设置你要绘制的窗口皮肤资源路径
     * GetSkinFile 接口设置你要绘制的窗口的 xml 描述文件
     */
    virtual DString GetSkinFolder() override;
    virtual DString GetSkinFile() override;

    /** 当窗口创建完成以后调用此函数，供子类中做一些初始化的工作
     */
    virtual void OnInitWindow() override;

    /** 键盘按下：用于演示 Ctrl+空格触发自动补全
     * @param [in] vkCode 虚拟键盘代码
     * @param [in] modifierKey 按键标志位
     * @param [in] nativeMsg 从系统接收到的原始消息内容
     * @param [out] bHandled 消息是否已经处理
     * @return 返回消息的处理结果
     */
    virtual LRESULT OnKeyDownMsg(
        ui::VirtualKeyCode vkCode,
        uint32_t modifierKey,
        const ui::NativeMsg &nativeMsg,
        bool &bHandled) override;

private:
    /** 初始化编辑器的样式与内容 */
    void InitEditor();
    /** 更新状态栏信息 */
    void UpdateStatusText();

private:
    //编辑器控件（由 XML 创建，生命周期由窗口管理）
    duilib::ext::scintilla::ScintillaControl *m_pEditor = nullptr;
};

#endif // EXAMPLES_SCINTILLA_DEMO_MAIN_FORM_H_
