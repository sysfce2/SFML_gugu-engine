#pragma once

////////////////////////////////////////////////////////////////
// Includes

#include "Gugu/Core/Application.h"
#include "Gugu/Events/EventListener.h"

////////////////////////////////////////////////////////////////
// Forward Declarations

namespace gugu
{
    class Element;
    class ElementText;
}

////////////////////////////////////////////////////////////////
// File Declarations

namespace demoproject {
    
class Demo : public gugu::Application, public gugu::EventListener
{
public:

    Demo();
    virtual ~Demo();

    virtual void AppStart() override;
    virtual void AppStop() override;

    virtual void AppUpdateImGui(const gugu::DeltaTime& dt) override;

    virtual bool OnSFEvent(const sf::Event& event) override;

private:

    enum EButton
    {
        ToggleTextA,
        ToggleTextB,
    };

    void OnButtonClick(EButton button);

protected:

    gugu::Element* m_root;
    gugu::ElementText* m_textToggle;
    bool m_showImGuiDemo;
};

}   //namespace demoproject
