#include "MainForm.h"

MainForm::MainForm()
{
}

MainForm::~MainForm()
{
}

DString MainForm::GetSkinFolder()
{
    return _T("color_theme");
}

DString MainForm::GetSkinFile()
{
    return _T("color_theme.xml");
}

void MainForm::OnInitWindow()
{
    BaseClass::OnInitWindow();

    ui::Button* pOpenThemeMgrBtn = dynamic_cast<ui::Button*>(FindControl(_T("open_theme_manager")));
    if (pOpenThemeMgrBtn != nullptr) {
        pOpenThemeMgrBtn->AttachClick([](const ui::EventArgs&) {
            //打开主题配色管理器
            ui::FilePath resourcePath = ui::GlobalManager::GetResourceRootPath(false);
            resourcePath /= ui::FilePath(_T("../../docs/Tools/ColorThemeMgr.html"));
            ui::SystemUtil::OpenUrl(resourcePath.NativePath());
            return true;
            });
    }

    ui::Button* pOpenNewWindowDark = dynamic_cast<ui::Button*>(FindControl(_T("open_new_window_as_dark")));
    if (pOpenNewWindowDark != nullptr) {
        pOpenNewWindowDark->AttachClick([](const ui::EventArgs&) {
            //以深色主题打开
            MainForm* window = new MainForm();
            window->CreateWnd(nullptr, ui::WindowCreateParam(_T("ColorTheme"), true));
            window->ShowWindow(ui::kSW_SHOW_NORMAL);
            window->OpenColorTheme(ui::FilePath(_T("color_dark")));
            return true;
            });
    }

    ui::Button* pOpenNewWindowLight = dynamic_cast<ui::Button*>(FindControl(_T("open_new_window_as_light")));
    if (pOpenNewWindowLight != nullptr) {
        pOpenNewWindowLight->AttachClick([](const ui::EventArgs&) {
            //以浅色色主题打开
            MainForm* window = new MainForm();
            window->CreateWnd(nullptr, ui::WindowCreateParam(_T("ColorTheme"), true));
            window->ShowWindow(ui::kSW_SHOW_NORMAL);
            window->OpenColorTheme(ui::FilePath(_T("color_light")));
            return true;
            });
    }
}
