#ifndef DUILIB_EXT_SCINTILLA_SCINTILLA_DUI_PLATFORM_H_
#define DUILIB_EXT_SCINTILLA_SCINTILLA_DUI_PLATFORM_H_

namespace duilib::ext::scintilla {

/** 扩展平台层的初始化/清理入口。
 *
 * 当前版本用于预留平台级资源管理（字体、菜单、拖放等）。
 */
bool InitializePlatform();
void UninitializePlatform();

} // namespace duilib::ext::scintilla

#endif // DUILIB_EXT_SCINTILLA_SCINTILLA_DUI_PLATFORM_H_
