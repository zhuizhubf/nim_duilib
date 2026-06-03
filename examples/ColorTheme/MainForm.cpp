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
            //ui::FilePath resourcePath = ui::GlobalManager::GetResourceRootPath(false);
            //resourcePath /= ui::FilePath(_T("../../docs/Tools/ColorThemeMgr.html"));
            //ui::SystemUtil::OpenUrl(resourcePath.NativePath());

            ui::FilePath resourcePath = ui::GlobalManager::GetResourceRootPath(false);
            resourcePath /= ui::FilePath(_T("themes/color_light/global.xml"));
            ui::ThemeGenerator themeGenerator;
            bool bRet = themeGenerator.LoadConfigFromXml(resourcePath.NativePathA());

            std::string xmlText = themeGenerator.GenerateTheme(210.0f, 0.01f, false);
            DString xmlTextW = ui::StringConvert::UTF8ToT(xmlText);
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

    //主题生成相关
    ui::CheckBox* pGenColorCheckBox = dynamic_cast<ui::CheckBox*>(FindControl(_T("gen_color_checkbox")));
    ui::Box* pGenColorBox = dynamic_cast<ui::Box*>(FindControl(_T("gen_color_box")));

    ui::Slider* pSliderHue = dynamic_cast<ui::Slider*>(FindControl(_T("gen_color_slider_hue")));
    ui::Label* pLabelHue = dynamic_cast<ui::Label*>(FindControl(_T("gen_color_label_hue")));

    ui::Slider* pSliderBaseLuminance = dynamic_cast<ui::Slider*>(FindControl(_T("gen_color_slider_base_luminance")));
    ui::Label* pLabelBaseLuminance = dynamic_cast<ui::Label*>(FindControl(_T("gen_color_label_base_luminance")));
    if ((pGenColorCheckBox == nullptr) || (pGenColorBox == nullptr) ||
        (pSliderHue == nullptr) || (pLabelHue == nullptr) ||
        (pSliderBaseLuminance == nullptr) || (pLabelBaseLuminance == nullptr)) {
        return;
    }

    pSliderHue->AttachValueChanged([this](const ui::EventArgs&) {
        OnGenColorParamChanged();
        return true;
        });
    pSliderBaseLuminance->AttachValueChanged([this](const ui::EventArgs&) {
        OnGenColorParamChanged();
        return true;
        });

    //初始状态
    pGenColorBox->SetEnabled(pGenColorCheckBox->IsSelected());
    OnGenColorEnableStateChanged(pGenColorCheckBox->IsSelected());

    pGenColorCheckBox->AttachSelect([this, pGenColorBox](const ui::EventArgs&) {
        pGenColorBox->SetEnabled(true);
        OnGenColorEnableStateChanged(true);
        return true;
        });

    pGenColorCheckBox->AttachUnSelect([this, pGenColorBox](const ui::EventArgs&) {
        pGenColorBox->SetEnabled(false);
        OnGenColorEnableStateChanged(false);
        return true;
        });
}

void MainForm::OnGenColorEnableStateChanged(bool bEnabled)
{
    if (bEnabled) {
        OpenColorTheme(ui::FilePath(DUILIB_LIGHT_COLOR_PATH));

        //更新当前页面的配置
        OnGenColorParamChanged();
    }
    else {
        CloseColorTheme();
    }
}

void MainForm::OnGenColorParamChanged()
{
    ui::Slider* pSliderHue = dynamic_cast<ui::Slider*>(FindControl(_T("gen_color_slider_hue")));
    ui::Label* pLabelHue = dynamic_cast<ui::Label*>(FindControl(_T("gen_color_label_hue")));

    ui::Slider* pSliderBaseLuminance = dynamic_cast<ui::Slider*>(FindControl(_T("gen_color_slider_base_luminance")));
    ui::Label* pLabelBaseLuminance = dynamic_cast<ui::Label*>(FindControl(_T("gen_color_label_base_luminance")));
    if ((pSliderHue == nullptr) || (pLabelHue == nullptr) ||
        (pSliderBaseLuminance == nullptr) || (pLabelBaseLuminance == nullptr)) {
        return;
    }

    double fHue = pSliderHue->GetValue() / 100.0; // [0 - 360]
    if (fHue < 0.0) {
        fHue = 0.0;
    }
    if (fHue > 360.0) {
        fHue = 360.0;
    }
    double fBaseLuminance = pSliderBaseLuminance->GetValue() / 10000.0; // [0 - 1]
    if (fBaseLuminance < 0.0) {
        fBaseLuminance = 0.0;
    }
    if (fBaseLuminance > 1.0) {
        fBaseLuminance = 1.0;
    }

    DString hueString = ui::StringUtil::Printf(_T("%.02f"), fHue);
    pLabelHue->SetText(hueString);

    DString baseLuminanceString = ui::StringUtil::Printf(_T("%.04f"), fBaseLuminance);
    pLabelBaseLuminance->SetText(baseLuminanceString);

    if (m_pThemeGenerator == nullptr) {
        //从当前主题加载默认的配置
        ui::FilePath globalXmlFileName = ui::FilePath(ui::GlobalManager::Instance().Theme().GetGlobalXmlFileName());
        ASSERT(!globalXmlFileName.IsEmpty());
        if (globalXmlFileName.IsEmpty()) {
            return;
        }

        ui::FilePath themeFullPath = ui::GlobalManager::Instance().Theme().GetThemeRootPath();
        themeFullPath /= ui::GlobalManager::Instance().Theme().GetCurrentColorThemeInfo().m_themePath;
        themeFullPath /= globalXmlFileName;

        ui::WindowBuilder windowBuilder;
        std::string xmlFileData = windowBuilder.ReadXmlFileData(themeFullPath);
        ASSERT(!xmlFileData.empty());
        if (xmlFileData.empty()) {
            return;
        }

        m_pThemeGenerator = std::make_unique<ui::ThemeGenerator>();
        if (!m_pThemeGenerator->LoadConfigFromXml(xmlFileData)) {
            ASSERT(0);
            m_pThemeGenerator.reset();
            return;
        }
    }

    std::string colorThemeXmlData = m_pThemeGenerator->GenerateTheme(fHue, fBaseLuminance, IsColorThemeDarkMode());
    ASSERT(!colorThemeXmlData.empty());
    if (!colorThemeXmlData.empty()) {
        //加载新的主题颜色配置
        OpenColorThemeData(colorThemeXmlData);
    }
}
