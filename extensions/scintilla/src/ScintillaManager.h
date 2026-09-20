#ifndef DUILIB_EXT_SCINTILLA_SCINTILLA_MANAGER_H_
#define DUILIB_EXT_SCINTILLA_SCINTILLA_MANAGER_H_

namespace duilib::ext::scintilla {

/** Scintilla 扩展库的全局初始化和控件工厂注册入口。
 *
 * 应用在加载包含 ScintillaControl 的 XML 之前调用 Initialize()。
 */
class ScintillaManager
{
public:
    static ScintillaManager &Instance();

    ScintillaManager();
    ScintillaManager(const ScintillaManager &) = delete;
    ScintillaManager &operator=(const ScintillaManager &) = delete;
    ~ScintillaManager();

    bool Initialize();
    void UnInitialize();
    bool IsInitialized() const;

private:
    bool m_bInitialized = false;
};

} // namespace duilib::ext::scintilla

#endif // DUILIB_EXT_SCINTILLA_SCINTILLA_MANAGER_H_
