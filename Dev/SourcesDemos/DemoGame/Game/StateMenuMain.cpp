////////////////////////////////////////////////////////////////
// Header

#include "Game/StateMenuMain.h"

////////////////////////////////////////////////////////////////
// Includes

#include "Game/StateScenario.h"

#include "Gugu/Engine.h"
#include "Gugu/Resources/ManagerResources.h"
#include "Gugu/Inputs/ManagerInputs.h"
#include "Gugu/Audio/ManagerAudio.h"
#include "Gugu/Network/ManagerNetwork.h"
#include "Gugu/Window/Window.h"
#include "Gugu/Math/MathUtility.h"
#include "Gugu/System/SystemUtility.h"
#include "Gugu/Element/Element.h"
#include "Gugu/Element/2D/ElementSprite.h"
#include "Gugu/Element/2D/ElementText.h"
#include "Gugu/Element/2D/ElementSpriteGroup.h"
#include "Gugu/Element/2D/ElementSpriteAnimated.h"
#include "Gugu/Element/UI/ElementButton.h"
#include "Gugu/Element/UI/ElementList.h"

using namespace gugu;

////////////////////////////////////////////////////////////////
// File Implementation

namespace demoproject {

StateMenuMain::StateMenuMain()
{
}

StateMenuMain::~StateMenuMain()
{
}

void StateMenuMain::Init()
{
    RegisterHandlerEvents(GetGameWindow());

    //Root UI
    m_menu = GetGameWindow()->GetUINode()->AddChild<Element>();
    m_menu->SetUnifiedSize(UDim2(UDim(1.f, 0.f), UDim(1.f, 0.f)));

    //Box Menu
    ElementSpriteGroup* pBox = m_menu->AddChild<ElementSpriteGroup>();
    pBox->LoadFromFile("Box9_02.xml");
    pBox->SetUnifiedPosition(UDim2(0.f, 50.f, 0.f, 50.f));

    ElementButton* pButton;
    float fPosX = 20.f;
    float fPosY = 20.f;
    float fGapY = 64.f;

    pButton = pBox->AddChild<ElementButton>();
    pButton->SetTexture("ButtonNormal.png", "ButtonFocused.png");
    pButton->GetElementText()->SetFont("Spaceranger.ttf");
    pButton->SetText("New Game");
    pButton->SetOnMouseReleased(std::bind(&StateMenuMain::OnButtonClick, this, 1));
    pButton->SetPosition(fPosX, fPosY);

    fPosY += fGapY;
    pButton = pBox->AddChild<ElementButton>();
    pButton->SetTexture("ButtonNormal.png", "ButtonFocused.png");
    pButton->GetElementText()->SetFont("Spaceranger.ttf");
    pButton->SetText("Quit");
    pButton->SetOnMouseReleased(std::bind(&StateMenuMain::OnButtonClick, this, 0));
    pButton->SetPosition(fPosX, fPosY);

    pBox->SetSize(pButton->GetSize().x + 40.f, fPosY + pButton->GetSize().y + 20.f);
}

void StateMenuMain::Release()
{
    SafeDelete(m_menu);
}

void StateMenuMain::Enter(State* _pStateFrom)
{
}

void StateMenuMain::Exit(State* _pStateTo)
{
}

void StateMenuMain::Step(const DeltaTime& dt)
{
}

void StateMenuMain::Update(const DeltaTime& dt)
{
}

bool StateMenuMain::OnSFEvent(const sf::Event& _oSFEvent)
{
    if (!EventListener::OnSFEvent(_oSFEvent))
        return false;

    ManagerInputs* inputs = GetInputs();

    if (inputs->IsInputReleased("Exit", _oSFEvent))
    {
        GetEngine()->StopMainLoop();
        return false;
    }

    return true;
}

void StateMenuMain::OnButtonClick(int _iButtonID)
{
    if (_iButtonID == 0)
    {
        GetEngine()->StopMainLoop();
    }
    else if (_iButtonID == 1)
    {
        GetOwner()->ChangeState(new StateScenario);
    }
}

}   //namespace demoproject
