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

    ui::Slider* pSliderBaseChroma = dynamic_cast<ui::Slider*>(FindControl(_T("gen_color_slider_base_chroma")));
    ui::Label* pLabelBaseChroma = dynamic_cast<ui::Label*>(FindControl(_T("gen_color_label_base_chroma")));
    if ((pGenColorCheckBox == nullptr) || (pGenColorBox == nullptr) ||
        (pSliderHue == nullptr) || (pLabelHue == nullptr) ||
        (pSliderBaseChroma == nullptr) || (pLabelBaseChroma == nullptr)) {
        return;
    }

    pSliderHue->AttachValueChanged([this](const ui::EventArgs&) {
        OnGenColorParamChanged();
        return true;
        });
    pSliderBaseChroma->AttachValueChanged([this](const ui::EventArgs&) {
        OnGenColorParamChanged();
        return true;
        });

    ui::Option* pOptionColorType = dynamic_cast<ui::Option*>(FindControl(_T("gen_color_radio_light")));
    if (pOptionColorType != nullptr) {
        pOptionColorType->AttachSelect([this](const ui::EventArgs&) {
            OnGenColorParamChanged();
            return true;
            });
    }
    pOptionColorType = dynamic_cast<ui::Option*>(FindControl(_T("gen_color_radio_dark")));
    if (pOptionColorType != nullptr) {
        pOptionColorType->AttachSelect([this](const ui::EventArgs&) {
            OnGenColorParamChanged();
            return true;
            });
    }

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

    ui::Slider* pSliderBaseChroma = dynamic_cast<ui::Slider*>(FindControl(_T("gen_color_slider_base_chroma")));
    ui::Label* pLabelBaseChroma = dynamic_cast<ui::Label*>(FindControl(_T("gen_color_label_base_chroma")));
    if ((pSliderHue == nullptr) || (pLabelHue == nullptr) ||
        (pSliderBaseChroma == nullptr) || (pLabelBaseChroma == nullptr)) {
        return;
    }

    double fHue = pSliderHue->GetValue() / 10.0; // [0 - 360]
    if (fHue < 0.0) {
        fHue = 0.0;
    }
    if (fHue > 360.0) {
        fHue = 360.0;
    }
    double fBaseLuminance = pSliderBaseChroma->GetValue() / 10000.0; // [0 - 0.03]
    if (fBaseLuminance < 0.0) {
        fBaseLuminance = 0.0;
    }
    if (fBaseLuminance > 1.0) {
        fBaseLuminance = 1.0;
    }

    DString hueString = ui::StringUtil::Printf(_T("%.02f"), fHue);
    pLabelHue->SetText(hueString);

    DString baseLuminanceString = ui::StringUtil::Printf(_T("%.04f"), fBaseLuminance);
    pLabelBaseChroma->SetText(baseLuminanceString);

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

    bool bDarkMode = false;
    ui::Option* pOptionColorType = dynamic_cast<ui::Option*>(FindControl(_T("gen_color_radio_dark")));
    if ((pOptionColorType != nullptr) && (pOptionColorType->IsSelected())){
        bDarkMode = true;
    }

    std::string colorThemeXmlData = m_pThemeGenerator->GenerateTheme(fHue, fBaseLuminance, bDarkMode);
    DString testXml = ui::StringConvert::UTF8ToT(colorThemeXmlData);
    ASSERT(!colorThemeXmlData.empty());
    if (!colorThemeXmlData.empty()) {
        //加载新的主题颜色配置
        OpenColorThemeData(colorThemeXmlData);
    }

    //更新主色预览区(背景色和对应的前景色)
    std::vector<std::pair<DString, DString>> previewColors;
    previewColors.push_back(std::make_pair<DString, DString>(_T("--background"), _T("--foreground")));
    previewColors.push_back(std::make_pair<DString, DString>(_T("--accent"), _T("--accent_foreground")));
    previewColors.push_back(std::make_pair<DString, DString>(_T("--success"), _T("")));
    previewColors.push_back(std::make_pair<DString, DString>(_T("--warning"), _T("")));
    previewColors.push_back(std::make_pair<DString, DString>(_T("--error"), _T("")));

    previewColors.push_back(std::make_pair<DString, DString>(_T("--surface_0"), _T("")));
    previewColors.push_back(std::make_pair<DString, DString>(_T("--surface_1"), _T("")));
    previewColors.push_back(std::make_pair<DString, DString>(_T("--surface_2"), _T("")));
    previewColors.push_back(std::make_pair<DString, DString>(_T("--surface_3"), _T("")));
    previewColors.push_back(std::make_pair<DString, DString>(_T("--surface_4"), _T("")));

    for (auto iter : previewColors) {
        const DString& bg = iter.first;
        const DString& fg = iter.second;
        ui::Label* pLabel = dynamic_cast<ui::Label*>(FindControl(bg));
        if (pLabel != nullptr) {
            std::string bkColor = m_pThemeGenerator->GetGeneratedColor(ui::StringConvert::TToUTF8(bg));
            pLabel->SetBkColor(ui::StringConvert::UTF8ToT(bkColor));
            pLabel->SetToolTipText(ui::StringConvert::UTF8ToT(bkColor));
            if (!fg.empty()) {
                std::string fgColor = m_pThemeGenerator->GetGeneratedColor(ui::StringConvert::TToUTF8(fg));
                pLabel->SetStateTextColor(ui::ControlStateType::kControlStateNormal, ui::StringConvert::UTF8ToT(fgColor));
            }
        }
    }
}
