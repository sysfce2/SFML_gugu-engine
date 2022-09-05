////////////////////////////////////////////////////////////////
// Header

#include "Gugu/Common.h"
#include "Gugu/Events/ElementEventHandler.h"

////////////////////////////////////////////////////////////////
// Includes

#include "Gugu/Element/Element.h"
#include "Gugu/Window/Window.h"
#include "Gugu/Events/WindowEventHandler.h"
#include "Gugu/Debug/Logger.h"

////////////////////////////////////////////////////////////////
// File Implementation

namespace gugu {

ElementEventHandler::ElementEventHandler(Element* element)
    : m_element(element)
    , m_handler(nullptr)
    , m_interactionsEnabled(true)
    , m_interactionsFilter(EInteractionType::None)
{
}

ElementEventHandler::~ElementEventHandler()
{
    RemoveAllCallbacks();
    UnregisterWindowEventHandler();
}

Element* ElementEventHandler::GetElement() const
{
    return m_element;
}

void ElementEventHandler::UnregisterWindowEventHandler()
{
    if (m_handler)
    {
        m_handler->UnregisterElementEventHandler(this);
    }
}

void ElementEventHandler::SetAllInteractionsEnabled(bool enabled)
{
    if (m_interactionsEnabled == enabled)
        return;

    m_interactionsEnabled = enabled;

    if (m_interactionsEnabled)
    {
        FireCallbacks(EElementEvent::InteractionsEnabled);
    }
    else
    {
        FireCallbacks(EElementEvent::InteractionsDisabled);
    }
}

void ElementEventHandler::SetInteractionEnabled(EInteractionType::Type interactionType, bool enabled)
{
    if (enabled)
    {
        m_interactionsFilter = m_interactionsFilter & ~interactionType;
    }
    else
    {
        m_interactionsFilter = m_interactionsFilter | interactionType;
    }
}

bool ElementEventHandler::IsInteractionEnabled(EInteractionType::Type interactionType) const
{
    return m_interactionsEnabled
        && (m_interactionsFilter & interactionType) == EInteractionType::None
        && m_element->IsVisible(true);
}

void ElementEventHandler::AddCallback(EInteractionEvent::Type event, const DelegateInteractionEvent& callback)
{
    if (!callback || event == EInteractionEvent::None)
        return;

    InteractionCallbackInfos kInfos;
    kInfos.event = event;
    kInfos.callback = callback;
    m_interactionCallbacks.push_back(kInfos);

    CheckRegistration(event);
}

void ElementEventHandler::RemoveCallbacks(EInteractionEvent::Type event)
{
    for (auto iteCallback = m_interactionCallbacks.begin(); iteCallback != m_interactionCallbacks.end();)
    {
        if (iteCallback->event == event)
        {
            iteCallback = m_interactionCallbacks.erase(iteCallback);
        }
        else
        {
             ++iteCallback;
        }
    }
}

void ElementEventHandler::FireCallbacks(EInteractionEvent::Type event, const InteractionInfos& interactionInfos)
{
    for (size_t i = 0; i < m_interactionCallbacks.size(); ++i)
    {
        if (m_interactionCallbacks[i].event == event)
        {
            m_interactionCallbacks[i].callback(interactionInfos);
        }
    }
}

void ElementEventHandler::AddCallback(EElementEvent::Type event, const Callback& callback)
{
    if (!callback || event == EElementEvent::None)
        return;

    ElementCallbackInfos kInfos;
    kInfos.event = event;
    kInfos.callback = callback;
    m_elementCallbacks.push_back(kInfos);
}

void ElementEventHandler::RemoveCallbacks(EElementEvent::Type event)
{
    for (auto iteCallback = m_elementCallbacks.begin(); iteCallback != m_elementCallbacks.end();)
    {
        if (iteCallback->event == event)
        {
            iteCallback = m_elementCallbacks.erase(iteCallback);
        }
        else
        {
            ++iteCallback;
        }
    }
}

void ElementEventHandler::FireCallbacks(EElementEvent::Type event)
{
    for (size_t i = 0; i < m_elementCallbacks.size(); ++i)
    {
        if (m_elementCallbacks[i].event == event)
        {
            m_elementCallbacks[i].callback();
        }
    }
}

void ElementEventHandler::RemoveAllCallbacks()
{
    m_elementCallbacks.clear();
    m_interactionCallbacks.clear();
}

void ElementEventHandler::CheckRegistration(EInteractionEvent::Type event)
{
    if (event == EInteractionEvent::MouseEntered
        || event == EInteractionEvent::MouseLeft)
    {
        GetGameWindow()->GetEventHandler()->RegisterElementEventHandler(this, EInteractionType::Focus);
    }
    else if (event == EInteractionEvent::MousePressed
        || event == EInteractionEvent::MouseReleased)
    {
        GetGameWindow()->GetEventHandler()->RegisterElementEventHandler(this, EInteractionType::Click);
    }
    else if (event == EInteractionEvent::MouseSelected
        || event == EInteractionEvent::MouseDeselected)
    {
        GetGameWindow()->GetEventHandler()->RegisterElementEventHandler(this, EInteractionType::Selection);
    }
    else if (event == EInteractionEvent::MouseScrolled)
    {
        GetGameWindow()->GetEventHandler()->RegisterElementEventHandler(this, EInteractionType::Scroll);
    }
    else if (event == EInteractionEvent::MouseDragBegan
        || event == EInteractionEvent::MouseDragMoved
        || event == EInteractionEvent::MouseDragEnded)
    {
        GetGameWindow()->GetEventHandler()->RegisterElementEventHandler(this, EInteractionType::Drag);
    }
    else if (event == EInteractionEvent::RawSFEvent)
    {
        GetGameWindow()->GetEventHandler()->RegisterElementEventHandler(this, EInteractionType::RawSFEvent);
    }
}

}   // namespace gugu