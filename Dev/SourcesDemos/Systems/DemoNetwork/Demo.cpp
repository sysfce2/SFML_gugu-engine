////////////////////////////////////////////////////////////////
// Header

#include "Gugu/Common.h"
#include "Demo.h"

////////////////////////////////////////////////////////////////
// Includes

#include "Gugu/Window/Window.h"
#include "Gugu/Element/UI/ElementButton.h"
#include "Gugu/Network/ManagerNetwork.h"

using namespace gugu;

////////////////////////////////////////////////////////////////
// File Implementation

namespace demoproject {
    
Demo::Demo()
: m_root(nullptr)
{
}

Demo::~Demo()
{
}

void Demo::AppStart()
{
    RegisterEventHandler(GetGameWindow());

    //Root
    m_root = GetGameWindow()->GetUINode()->AddChild<Element>();
    m_root->SetUnifiedSize(UDim2::SIZE_FULL);

    ElementButton* button;

    button = m_root->AddChild<ElementButton>();
    button->LoadFromWidget("Button_01.widget.xml");
    button->SetText("Listen A");
    button->Move(10.0, 10.0);
    button->SetOnMouseReleased(std::bind(&Demo::OnButtonClick, this, 1));

    button = m_root->AddChild<ElementButton>();
    button->LoadFromWidget("Button_01.widget.xml");
    button->SetText("Connect A");
    button->Move(10.0, 60.0);
    button->SetOnMouseReleased(std::bind(&Demo::OnButtonClick, this, 2));

    button = m_root->AddChild<ElementButton>();
    button->LoadFromWidget("Button_01.widget.xml");
    button->SetText("Ping All");
    button->Move(10.0, 160.0);
    button->SetOnMouseReleased(std::bind(&Demo::OnButtonClick, this, 3));

    button = m_root->AddChild<ElementButton>();
    button->LoadFromWidget("Button_01.widget.xml");
    button->SetText("Listen B");
    button->Move(300.0, 10.0);
    button->SetOnMouseReleased(std::bind(&Demo::OnButtonClick, this, 4));

    button = m_root->AddChild<ElementButton>();
    button->LoadFromWidget("Button_01.widget.xml");
    button->SetText("Connect B");
    button->Move(300.0, 60.0);
    button->SetOnMouseReleased(std::bind(&Demo::OnButtonClick, this, 5));

    button = m_root->AddChild<ElementButton>();
    button->LoadFromWidget("Button_01.widget.xml");
    button->SetText("Disconnect");
    button->Move(590.0, 110.0);
    button->SetOnMouseReleased(std::bind(&Demo::OnButtonClick, this, 7));
}

void Demo::AppStop()
{
}

void Demo::OnButtonClick(int _iButtonID)
{
    if (_iButtonID == 1)
    {
        GetNetwork()->StartListening(GUGU_PORT_DEFAULT);
    }
    else if (_iButtonID == 2)
    {
        GetNetwork()->ConnectToClient(sf::IpAddress::LocalHost.toString(), GUGU_PORT_DEFAULT + 1);
    }
    else if (_iButtonID == 3)
    {
        GetNetwork()->SendNetPacketToAll(new NetPacket(ENetPacket::Ping), false);
    }
    else if (_iButtonID == 4)
    {
        GetNetwork()->StartListening(GUGU_PORT_DEFAULT + 1);
    }
    else if (_iButtonID == 5)
    {
        GetNetwork()->ConnectToClient(sf::IpAddress::LocalHost.toString(), GUGU_PORT_DEFAULT);
    }
    else if (_iButtonID == 7)
    {
        GetNetwork()->DisconnectAll();
    }
}

}   //namespace demoproject
