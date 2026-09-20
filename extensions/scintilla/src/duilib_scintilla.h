#ifndef DUILIB_EXT_SCINTILLA_DUILIB_SCINTILLA_H_
#define DUILIB_EXT_SCINTILLA_DUILIB_SCINTILLA_H_

/** DUI 原生 Scintilla 扩展库的聚合头。
 *
 * 本扩展不修改 nim_duilib 核心库，使用方式：
 *   1. 应用额外链接 duilib-scintilla 静态库；
 *   2. 在加载窗口 XML 之前调用 ScintillaManager::Initialize()；
 *   3. XML 中使用 <ScintillaControl .../>。
 */

// clang-format off
//Scintilla 的公共类型与消息定义（SendSciMessage / 通知回调需要）
//注意：Scintilla 头文件有包含顺序要求，禁止自动排序（ScintillaTypes 必须最先包含）
#include "Sci_Position.h"
#include "ScintillaTypes.h"
#include "ScintillaStructures.h"
#include "ScintillaMessages.h"
#include "Scintilla.h"
// clang-format on

#include "ScintillaControl.h"
#include "ScintillaManager.h"

#endif // DUILIB_EXT_SCINTILLA_DUILIB_SCINTILLA_H_
