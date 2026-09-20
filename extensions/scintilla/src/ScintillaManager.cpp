#include "ScintillaManager.h"

#include "ScintillaControl.h"
#include "ScintillaDuiPlatform.h"

#include "duilib/Core/GlobalManager.h"

namespace duilib::ext::scintilla {

namespace {

ui::Control *CreateScintillaControl(const DString &className)
{
    if (className == _T("ScintillaControl")) {
        return new ScintillaControl(nullptr);
    }
    return nullptr;
}

} // namespace

ScintillaManager &ScintillaManager::Instance()
{
    static ScintillaManager self;
    return self;
}

ScintillaManager::ScintillaManager() = default;
ScintillaManager::~ScintillaManager() = default;

bool ScintillaManager::Initialize()
{
    if (m_bInitialized) {
        return true;
    }
    if (!InitializePlatform()) {
        return false;
    }
    ui::GlobalManager::Instance().AddCreateControlCallback(CreateScintillaControl);
    m_bInitialized = true;
    return true;
}

void ScintillaManager::UnInitialize()
{
    m_bInitialized = false;
    UninitializePlatform();
}

bool ScintillaManager::IsInitialized() const
{
    return m_bInitialized;
}

} // namespace duilib::ext::scintilla
