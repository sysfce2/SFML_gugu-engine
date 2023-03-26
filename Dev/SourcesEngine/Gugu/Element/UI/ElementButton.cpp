////////////////////////////////////////////////////////////////
// Header

#include "Gugu/Common.h"
#include "Gugu/Element/UI/ElementButton.h"

////////////////////////////////////////////////////////////////
// Includes

#include "Gugu/Resources/ManagerResources.h"
#include "Gugu/Events/ElementEventHandler.h"
#include "Gugu/Element/2D/ElementSprite.h"
#include "Gugu/Element/2D/ElementSpriteGroup.h"
#include "Gugu/Element/2D/ElementText.h"
#include "Gugu/Resources/Texture.h"
#include "Gugu/Window/Renderer.h"
#include "Gugu/System/SystemUtility.h"
#include "Gugu/External/PugiXmlUtility.h"

////////////////////////////////////////////////////////////////
// File Implementation

namespace gugu {

ElementButton::ElementButton()
    : m_spriteIdle(nullptr)
    , m_spriteFocused(nullptr)
    , m_spriteDisabled(nullptr)
    , m_currentSprite(nullptr)
    , m_text(nullptr)
    , m_actionOnPressed(nullptr)
    , m_actionOnReleased(nullptr)
    , m_isDisabled(false)
{
    m_text = new ElementText;
    m_text->SetParent(this);

    SetTextAlignment(UDim2::POSITION_TOP_CENTER);

    GetEvents()->AddCallback(EInteractionEvent::MouseEntered, STD_BIND_1(&ElementButton::OnMouseEntered, this));
    GetEvents()->AddCallback(EInteractionEvent::MouseLeft, STD_BIND_1(&ElementButton::OnMouseLeft, this));
    GetEvents()->AddCallback(EInteractionEvent::MousePressed, STD_BIND_1(&ElementButton::OnMousePressed, this));
    GetEvents()->AddCallback(EInteractionEvent::MouseReleased, STD_BIND_1(&ElementButton::OnMouseReleased, this));
}

ElementButton::~ElementButton()
{
    m_currentSprite = nullptr;

    SafeDelete(m_spriteIdle);
    SafeDelete(m_spriteFocused);
    SafeDelete(m_spriteDisabled);
    SafeDelete(m_text);
}

void ElementButton::SetTexture(const std::string& textureIdleID, const std::string& textureFocusedID, const std::string& textureDisabledID)
{
    SetTextureImpl(GetResources()->GetTexture(textureIdleID),
        textureFocusedID.empty() ? nullptr : GetResources()->GetTexture(textureFocusedID),
        textureDisabledID.empty() ? nullptr : GetResources()->GetTexture(textureDisabledID));
}

void ElementButton::SetTexture(Texture* textureIdle, Texture* textureFocused, Texture* textureDisabled)
{
    SetTextureImpl(textureIdle, textureFocused, textureDisabled);
}

void ElementButton::SetTextureImpl(Texture* textureIdle, Texture* textureFocused, Texture* textureDisabled)
{
    if (!textureIdle)
        return;

    ElementSprite* spriteIdle = new ElementSprite;
    spriteIdle->SetTexture(textureIdle);
    spriteIdle->SetUnifiedSize(UDim2::SIZE_FULL);
    spriteIdle->SetParent(this);

    m_spriteIdle = spriteIdle;

    if (textureFocused)
    {
        ElementSprite* spriteFocused = new ElementSprite;
        spriteFocused->SetTexture(textureFocused);
        spriteFocused->SetUnifiedSize(UDim2::SIZE_FULL);
        spriteFocused->SetParent(this);

        m_spriteFocused = spriteFocused;
    }

    if (textureDisabled)
    {
        ElementSprite* spriteDisabled = new ElementSprite;
        spriteDisabled->SetTexture(textureDisabled);
        spriteDisabled->SetUnifiedSize(UDim2::SIZE_FULL);
        spriteDisabled->SetParent(this);

        m_spriteDisabled = spriteDisabled;
    }

    SetSize(Vector2f(textureIdle->GetSize()));

    m_currentSprite = m_spriteIdle;
}

void ElementButton::SetText(const std::string& _strText)
{
    m_text->SetText(_strText);
}

void ElementButton::SetTextAlignment(const UDim2& alignment, const Vector2f& offset)
{
    m_text->SetUnifiedOrigin(alignment);
    m_text->SetUnifiedPosition(alignment + offset);
}

void ElementButton::SetDisabled(bool _bDisabled)
{
    if (m_isDisabled == _bDisabled)
        return;

    m_isDisabled = _bDisabled;

    if (m_isDisabled)
    {
        if (m_currentSprite)
        {
            m_currentSprite = m_spriteDisabled ? m_spriteDisabled : m_spriteIdle;
        }
    }
    else
    {
        if (m_currentSprite)
        {
            m_currentSprite = m_spriteIdle;
        }
    }
}

void ElementButton::SetOnMousePressed(const Callback& _pActionOnPressed)
{
    m_actionOnPressed = _pActionOnPressed;
}

void ElementButton::SetOnMouseReleased(const Callback& _pActionOnReleased)
{
    m_actionOnReleased = _pActionOnReleased;
}

void ElementButton::OnMousePressed(const InteractionInfos& interactionInfos)
{
    if (m_actionOnPressed)
    {
        m_actionOnPressed();
    }
}

void ElementButton::OnMouseReleased(const InteractionInfos& interactionInfos)
{
    if (m_actionOnReleased)
    {
        m_actionOnReleased();
    }
}

void ElementButton::OnMouseEntered(const InteractionInfos& interactionInfos)
{
    if (m_currentSprite && !m_isDisabled)
    {
        m_currentSprite = m_spriteFocused ? m_spriteFocused : m_spriteIdle;
    }
}

void ElementButton::OnMouseLeft(const InteractionInfos& interactionInfos)
{
    if (m_currentSprite && !m_isDisabled)
    {
        m_currentSprite = m_spriteIdle;
    }
}

void ElementButton::RenderImpl(RenderPass& _kRenderPass, const sf::Transform& _kTransformSelf)
{
    sf::FloatRect kGlobalTransformed = _kTransformSelf.transformRect(sf::FloatRect(Vector2::Zero_f, m_size));
    if (_kRenderPass.rectViewport.intersects(kGlobalTransformed))
    {
        if (m_currentSprite)
        {
            m_currentSprite->Render(_kRenderPass, _kTransformSelf);
        }

        m_text->Render(_kRenderPass, _kTransformSelf);
    }
}

void ElementButton::OnSizeChanged()
{
    if (m_spriteIdle)
    {
        m_spriteIdle->ComputeUnifiedDimensions();
    }

    if (m_spriteFocused)
    {
        m_spriteFocused->ComputeUnifiedDimensions();
    }

    if (m_spriteDisabled)
    {
        m_spriteDisabled->ComputeUnifiedDimensions();
    }

    m_text->ComputeUnifiedDimensions();
}

ElementText* ElementButton::GetElementText() const
{
    return m_text;
}

bool ElementButton::LoadFromFile(const std::string& path)
{
    pugi::xml_document document;
    pugi::xml_parse_result result = document.load_file(GetResources()->GetResourceFilePath(path).c_str());
    if (!result)
        return false;

    pugi::xml_node nodeRoot = document.child("Element");
    if (!nodeRoot)
        return false;

    pugi::xml_attribute nodeRootType = nodeRoot.attribute("type");
    if (!nodeRootType || !StringEquals(nodeRootType.value(), "ElementButton"))
        return false;

    return LoadFromXml(nodeRoot);
}

bool ElementButton::LoadFromXmlImpl(const pugi::xml_node& node)
{
    if (!Element::LoadFromXmlImpl(node))
        return false;

    pugi::xml_node nodeComponents = node.child("Components");
    if (nodeComponents)
    {
        for (pugi::xml_node nodeComponentElement = nodeComponents.child("Element"); nodeComponentElement; nodeComponentElement = nodeComponentElement.next_sibling("Element"))
        {
            // TODO: I will need checks on actual element types serialized, to force the usage of elementspritegroups
            pugi::xml_attribute nodeElementName = nodeComponentElement.attribute("name");
            if (nodeElementName && StringEquals(nodeElementName.value(), "Idle") && !m_spriteIdle)
            {
                m_spriteIdle = new ElementSpriteGroup;
                m_spriteIdle->LoadFromXml(nodeComponentElement);
                m_spriteIdle->SetParent(this, true);
            }
            else if (nodeElementName && StringEquals(nodeElementName.value(), "Focused") && !m_spriteFocused)
            {
                m_spriteFocused = new ElementSpriteGroup;
                m_spriteFocused->LoadFromXml(nodeComponentElement);
                m_spriteFocused->SetParent(this, true);
            }
            else if (nodeElementName && StringEquals(nodeElementName.value(), "Disabled") && !m_spriteDisabled)
            {
                m_spriteDisabled = new ElementSpriteGroup;
                m_spriteDisabled->LoadFromXml(nodeComponentElement);
                m_spriteDisabled->SetParent(this, true);
            }
        }
    }

    m_currentSprite = m_spriteIdle;

    return m_currentSprite != nullptr;
}

}   // namespace gugu
