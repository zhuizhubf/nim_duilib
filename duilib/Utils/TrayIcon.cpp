#include "duilib/Utils/TrayIcon.h"

namespace ui
{

TrayIcon::TrayIcon():
    m_messageCallback(nullptr)
{
}

TrayIcon::~TrayIcon()
{
}

void TrayIcon::SetMessageCallback(TrayIconMessageCallback callback)
{
    m_messageCallback = callback;
}

void TrayIcon::NotifyMessage(TrayIconMessageType msgType, int32_t x, int32_t y)
{
    if (m_messageCallback) {
        m_messageCallback(msgType, x, y);
    }
}

} //namespace ui
