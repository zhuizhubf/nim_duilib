#include "duilib/Utils/TrayIcon.h"
#include "duilib/Core/Window.h"
#include "duilib/Core/GlobalManager.h"
#include "duilib/Utils/StringConvert.h"
#include "duilib/Utils/FileUtil.h"

#ifdef DUILIB_BUILD_FOR_SDL

#include <SDL3/SDL.h>
#include <SDL3/SDL_tray.h>

namespace ui
{

/** 托盘图标的SDL实现
*/
class UILIB_API TrayIconImpl : public TrayIcon
{
public:
    TrayIconImpl();
    virtual ~TrayIconImpl() override;

    /** 初始化托盘图标
    * @param [in] pWindow 关联的窗口指针
    * @param [in] iconFilePath 图标文件路径（支持ico格式）
    * @param [in] tooltip 托盘提示文本
    * @return 初始化成功返回true，失败返回false
    */
    bool Initialize(const Window* pWindow, const DString& iconFilePath, const DString& tooltip);

public:
    virtual bool SetIcon(const Window* pWindow, const DString& iconFilePath) override;
    virtual bool SetTooltip(const DString& tooltip) override;
    virtual bool ShowBalloon(const DString& title, const DString& content, uint32_t timeoutMs = 3000) override;
    virtual bool Hide() override;
    virtual bool Show() override;
    virtual bool IsTrayVisible() const override;
    virtual bool Remove() override;
    virtual void* GetTrayHandle() const override;

private:
    /** 从文件加载图标为SDL_Surface
    * @param [in] pWindow 关联的窗口
    * @param [in] iconFilePath 图标文件路径
    * @return SDL_Surface指针，失败返回nullptr
    */
    SDL_Surface* LoadSDLSurfaceFromFile(const Window* pWindow, const DString& iconFilePath);

    /** 从文件数据加载图标为SDL_Surface
    * @param [in] fileData 文件数据
    * @param [in] iconFilePath 文件路径
    * @return SDL_Surface指针，失败返回nullptr
    */
    SDL_Surface* LoadSDLSurfaceFromFileData(const std::vector<uint8_t>& fileData, const DString& iconFilePath);

    /** 创建托盘菜单（用于接收事件）
    */
    void CreateTrayMenu();

    /** 托盘事件回调函数
    */
    static void SDLCALL TrayEntryCallback(void* userdata, SDL_TrayEntry* entry);

    /** 获取当前鼠标位置
    */
    void GetMousePosition(int32_t& x, int32_t& y);

private:
    /** SDL托盘对象指针
    */
    SDL_Tray* m_sdlTray;

    /** SDL托盘菜单指针
    */
    SDL_TrayMenu* m_sdlTrayMenu;

    /** 当前图标表面
    */
    SDL_Surface* m_iconSurface;

    /** 提示文本
    */
    DString m_tooltip;

    /** 图标文件路径
    */
    DString m_iconFilePath;

    /** 是否隐藏
    */
    bool m_bHidden;

    /** 关联的窗口指针
    */
    const Window* m_pWindow;

    /** 最后一次点击的时间
    */
    uint64_t m_lastClickTime;

    /** 点击计数（用于检测双击）
    */
    int32_t m_clickCount;
};

TrayIconImpl::TrayIconImpl() :
    m_sdlTray(nullptr),
    m_sdlTrayMenu(nullptr),
    m_iconSurface(nullptr),
    m_bHidden(false),
    m_pWindow(nullptr),
    m_lastClickTime(0),
    m_clickCount(0)
{
}

TrayIconImpl::~TrayIconImpl()
{
    Remove();
}

bool TrayIconImpl::Initialize(const Window* pWindow, const DString& iconFilePath, const DString& tooltip)
{
    m_pWindow = pWindow;
    m_tooltip = tooltip;
    m_iconFilePath = iconFilePath;
    m_bHidden = false;
    m_lastClickTime = 0;
    m_clickCount = 0;

    // 加载图标
    m_iconSurface = LoadSDLSurfaceFromFile(pWindow, iconFilePath);
    //auto iconSurface = SDL_LoadBMP("public\\caption\\logo.bmp");

    // 创建SDL托盘图标
    std::string tooltipUTF8 = StringConvert::TToUTF8(tooltip);
    m_sdlTray = SDL_CreateTray(m_iconSurface, tooltipUTF8.c_str());

    if (m_sdlTray != nullptr) {
        // 创建托盘菜单用于事件处理
        //CreateTrayMenu();
        return true;
    }

    return false;
}

void TrayIconImpl::CreateTrayMenu()
{
    if (m_sdlTray == nullptr) {
        return;
    }

    // 创建托盘菜单
    m_sdlTrayMenu = SDL_CreateTrayMenu(m_sdlTray);
    if (m_sdlTrayMenu == nullptr) {
        return;
    }

    // 添加一个默认菜单项，用于接收托盘点击事件
    // 注意：SDL的托盘API是通过菜单项的回调来处理事件的
    // 这里我们创建菜单项来模拟托盘图标的点击事件
    SDL_TrayEntry* entry = SDL_InsertTrayEntryAt(m_sdlTrayMenu, -1, StringConvert::TToUTF8(m_tooltip).c_str(), SDL_TRAYENTRY_BUTTON);
    if (entry != nullptr) {
        SDL_SetTrayEntryCallback(entry, TrayEntryCallback, this);
    }
}

void SDLCALL TrayIconImpl::TrayEntryCallback(void* userdata, SDL_TrayEntry* /*entry*/)
{
    TrayIconImpl* pThis = static_cast<TrayIconImpl*>(userdata);
    if (pThis == nullptr) {
        return;
    }

    // 获取鼠标位置
    int32_t x = 0, y = 0;
    pThis->GetMousePosition(x, y);

    // 检测双击（300ms内点击两次）
    uint64_t currentTime = SDL_GetTicks();
    if ((currentTime - pThis->m_lastClickTime) < 300) {
        // 双击事件
        pThis->m_clickCount = 0;
        pThis->NotifyMessage(TrayIconMessageType::kLeftDoubleClick, x, y);
    }
    else {
        // 单击事件
        pThis->m_clickCount = 1;
        pThis->NotifyMessage(TrayIconMessageType::kLeftClick, x, y);
    }
    pThis->m_lastClickTime = currentTime;
}

void TrayIconImpl::GetMousePosition(int32_t& x, int32_t& y)
{
    float fx = 0.0f, fy = 0.0f;
    if (SDL_GetMouseState(&fx, &fy)) {
        x = static_cast<int32_t>(fx);
        y = static_cast<int32_t>(fy);
    }
    else {
        x = 0;
        y = 0;
    }
}

SDL_Surface* TrayIconImpl::LoadSDLSurfaceFromFile(const Window* pWindow, const DString& iconFilePath)
{
    if (iconFilePath.empty()) {
        return nullptr;
    }

    FilePath windowResPath;
    FilePath windowXmlPath;
    if (pWindow != nullptr) {
        windowResPath = pWindow->GetResourcePath();
        windowXmlPath = pWindow->GetXmlPath();
    }
    FilePath iconFullPath = GlobalManager::Instance().GetExistsResFullPath(windowResPath, windowXmlPath, FilePath(iconFilePath));
    ASSERT(!iconFullPath.IsEmpty());
    if (iconFullPath.IsEmpty()) {
        return nullptr;
    }

    std::vector<uint8_t> fileData;
    if (GlobalManager::Instance().Zip().IsUseZip() &&
        GlobalManager::Instance().Zip().IsZipResExist(iconFullPath)) {
        //使用压缩包        
        GlobalManager::Instance().Zip().GetZipData(iconFullPath, fileData);
    }
    else {
        //使用本地文件
        ASSERT(iconFullPath.IsExistsFile());
        if (iconFullPath.IsExistsFile()) {
            FileUtil::ReadFileData(iconFullPath, fileData);
        }
    }
    return LoadSDLSurfaceFromFileData(fileData, iconFullPath.ToString());
}

SDL_Surface* TrayIconImpl::LoadSDLSurfaceFromFileData(const std::vector<uint8_t>& fileData, const DString& iconFilePath)
{
    ASSERT(!fileData.empty());
    if (fileData.empty()) {
        return nullptr;
    }

    ImageDecoderFactory& imageDecoders = GlobalManager::Instance().ImageDecoders();
    float fImageSizeScale = GlobalManager::Instance().Dpi().GetDisplayScale();
    ImageDecodeParam decodeParam;
    decodeParam.m_imageFilePath = iconFilePath;
    decodeParam.m_fImageSizeScale = fImageSizeScale;
    decodeParam.m_pFileData = std::make_shared<std::vector<uint8_t>>(fileData);
    std::shared_ptr<IBitmap> pBitmap = imageDecoders.DecodeImageData(decodeParam);
    if (pBitmap == nullptr) {
        return nullptr;
    }
    uint32_t nWidth = pBitmap->GetWidth();
    uint32_t nHeight = pBitmap->GetHeight();
    if ((nWidth < 1) || (nHeight < 1)) {
        return nullptr;
    }

    void* pPixelBits = pBitmap->LockPixelBits();
    ASSERT(pPixelBits != nullptr);
    if (pPixelBits == nullptr) {
        return nullptr;
    }

#ifdef DUILIB_BUILD_FOR_WIN
    SDL_PixelFormat format = SDL_PIXELFORMAT_BGRA32;
#else
    SDL_PixelFormat format = SDL_PIXELFORMAT_RGBA32;
#endif
    SDL_Surface* iconSurface = SDL_CreateSurfaceFrom(pBitmap->GetWidth(), pBitmap->GetHeight(), format, pPixelBits, pBitmap->GetWidth() * sizeof(uint32_t));
    ASSERT(iconSurface != nullptr);
    return iconSurface;
}

bool TrayIconImpl::SetIcon(const Window* pWindow, const DString& iconFilePath)
{
    if (m_sdlTray == nullptr) {
        return false;
    }

    // 释放旧图标
    if (m_iconSurface != nullptr) {
        SDL_DestroySurface(m_iconSurface);
        m_iconSurface = nullptr;
    }

    m_iconFilePath = iconFilePath;

    // 加载新图标
    m_iconSurface = LoadSDLSurfaceFromFile(pWindow, iconFilePath);

    // 设置托盘图标
    SDL_SetTrayIcon(m_sdlTray, m_iconSurface);
    return true;
}

bool TrayIconImpl::SetTooltip(const DString& tooltip)
{
    if (m_sdlTray == nullptr) {
        return false;
    }

    m_tooltip = tooltip;
    std::string tooltipUTF8 = StringConvert::TToUTF8(tooltip);
    SDL_SetTrayTooltip(m_sdlTray, tooltipUTF8.c_str());
    return true;
}

bool TrayIconImpl::ShowBalloon(const DString& title, const DString& content, uint32_t timeoutMs)
{
    // SDL3的托盘API目前不直接支持气泡提示
    // 可以考虑使用SDL_ShowSimpleMessageBox或其他方式实现
    UNUSED_VARIABLE(title);
    UNUSED_VARIABLE(content);
    UNUSED_VARIABLE(timeoutMs);
    return false;
}

bool TrayIconImpl::Hide()
{
    if (m_bHidden || m_sdlTray == nullptr) {
        return true;
    }

    // SDL的托盘API没有隐藏功能，只能销毁后重建
    // 注意：销毁托盘会同时销毁菜单和菜单项
    if (m_sdlTray != nullptr) {
        SDL_DestroyTray(m_sdlTray);
        m_sdlTray = nullptr;
        m_sdlTrayMenu = nullptr;
    }
    m_bHidden = true;
    return true;
}

bool TrayIconImpl::Show()
{
    if (!m_bHidden) {
        return true;
    }

    // 重建托盘图标
    std::string tooltipUTF8 = StringConvert::TToUTF8(m_tooltip);
    m_sdlTray = SDL_CreateTray(m_iconSurface, tooltipUTF8.c_str());
    m_bHidden = false;

    if (m_sdlTray != nullptr) {
        //CreateTrayMenu();
        return true;
    }

    return false;
}

bool TrayIconImpl::IsTrayVisible() const
{
    return (m_sdlTray != nullptr) && !m_bHidden;
}

bool TrayIconImpl::Remove()
{
    if (m_iconSurface != nullptr) {
        SDL_DestroySurface(m_iconSurface);
        m_iconSurface = nullptr;
    }

    if (m_sdlTray != nullptr) {
        SDL_DestroyTray(m_sdlTray);
        m_sdlTray = nullptr;
        m_sdlTrayMenu = nullptr;
    }

    m_bHidden = true;
    return true;
}

void* TrayIconImpl::GetTrayHandle() const
{
    return (void*)m_sdlTray;
}

// TrayIcon 基类的Create函数，SDL平台实现
std::unique_ptr<TrayIcon> TrayIcon::Create(const Window* pWindow, const DString& iconFilePath, const DString& tooltip)
{
    std::unique_ptr<TrayIconImpl> pTrayIcon = std::make_unique<TrayIconImpl>();
    if (pTrayIcon->Initialize(pWindow, iconFilePath, tooltip)) {
        return pTrayIcon;
    }
    return nullptr;
}

} //namespace ui

#endif // DUILIB_BUILD_FOR_SDL
