////////////////////////////////////////////////////////////////
// Header

#include "Gugu/Common.h"
#include "Gugu/Element/UI/ElementButton.h"

////////////////////////////////////////////////////////////////
// Includes

#include "Gugu/Resources/ManagerResources.h"
#include "Gugu/Element/2D/ElementSprite.h"
#include "Gugu/Element/2D/ElementSpriteGroup.h"
#include "Gugu/Element/2D/ElementText.h"
#include "Gugu/System/SystemUtility.h"
#include "Gugu/Resources/Texture.h"
#include "Gugu/Window/Renderer.h"
#include "Gugu/External/PugiXmlWrap.h"

////////////////////////////////////////////////////////////////
// File Implementation

namespace gugu {

ElementButton::ElementButton()
{
    m_spriteIdle = nullptr;
    m_spriteFocused = nullptr;
    m_spriteDisabled = nullptr;
    m_currentSprite = nullptr;

    m_text = new ElementText;
    m_text->SetParent(this);

    SetTextAlignment(UDim2::POSITION_TOP_CENTER);

    m_actionOnPressed = nullptr;
    m_actionOnReleased = nullptr;
    
    AddInteractionFlag(EInteraction::Focus);
    AddInteractionFlag(EInteraction::Click);

    m_isDisabled = false;
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

    m_spriteIdle = new ElementSpriteGroup;
    m_spriteIdle->SetTexture(textureIdle);
    m_spriteIdle->SetUnifiedSize(UDim2::SIZE_FULL);
    m_spriteIdle->SetParent(this, true);

    ElementSpriteGroupItem* idleItem = new ElementSpriteGroupItem;
    idleItem->SetSubRect(textureIdle->GetRect());
    m_spriteIdle->AddItem(idleItem);

    if (textureFocused)
    {
        m_spriteFocused = new ElementSpriteGroup;
        m_spriteFocused->SetTexture(textureFocused);
        m_spriteFocused->SetUnifiedSize(UDim2::SIZE_FULL);
        m_spriteFocused->SetParent(this, true);

        ElementSpriteGroupItem* focusedItem = new ElementSpriteGroupItem;
        focusedItem->SetSubRect(textureFocused->GetRect());
        m_spriteFocused->AddItem(focusedItem);
    }

    if (textureDisabled)
    {
        m_spriteDisabled = new ElementSpriteGroup;
        m_spriteDisabled->SetTexture(textureDisabled);
        m_spriteDisabled->SetUnifiedSize(UDim2::SIZE_FULL);
        m_spriteDisabled->SetParent(this, true);

        ElementSpriteGroupItem* disabledItem = new ElementSpriteGroupItem;
        disabledItem->SetSubRect(textureDisabled->GetRect());
        m_spriteDisabled->AddItem(disabledItem);
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
        AddInteractionFlag(EInteraction::Disabled);

        if (m_currentSprite)
        {
            m_currentSprite = m_spriteDisabled ? m_spriteDisabled : m_spriteIdle;
        }
    }
    else
    {
        RemoveInteractionFlag(EInteraction::Disabled);

        if (m_currentSprite)
        {
            m_currentSprite = m_spriteIdle;
        }
    }
}

void ElementButton::SetOnMousePressed(Callback _pActionOnPressed)
{
    m_actionOnPressed = _pActionOnPressed;
}

void ElementButton::SetOnMouseReleased(Callback _pActionOnReleased)
{
    m_actionOnReleased = _pActionOnReleased;
}

bool ElementButton::OnMousePressed()
{
    if (m_actionOnPressed)
    {
        m_actionOnPressed();
        return false;
    }

    return true;
}

bool ElementButton::OnMouseReleased()
{
    if (m_actionOnReleased)
    {
        m_actionOnReleased();
        return false;
    }

    return true;
}

void ElementButton::OnMouseEnter()
{
    if (m_currentSprite)
    {
        m_currentSprite = m_spriteFocused ? m_spriteFocused : m_spriteIdle;
    }
}

void ElementButton::OnMouseLeave()
{
    if (m_currentSprite)
    {
        m_currentSprite = m_spriteIdle;
    }
}

void ElementButton::RenderImpl(RenderPass& _kRenderPass, const sf::Transform& _kTransformSelf)
{
    sf::FloatRect kGlobalTransformed = _kTransformSelf.transformRect(sf::FloatRect(Vector2f(), m_size));
    if (_kRenderPass.rectViewport.intersects(kGlobalTransformed))
    {
        if (m_currentSprite)
        {
            m_currentSprite->Render(_kRenderPass, _kTransformSelf);
        }

        m_text->Render(_kRenderPass, _kTransformSelf);
    }
}

void ElementButton::OnSizeChanged(Vector2f _kOldSize)
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
    pugi::xml_parse_result result = document.load_file(GetResources()->GetResourcePathName(path).c_str());
    if (!result)
        return false;

    pugi::xml_node nodeRoot = document.child("Element");
    if (!nodeRoot)
        return false;

    pugi::xml_attribute nodeRootType = nodeRoot.attribute("type");
    if (!nodeRootType)
        return false;

    if (std::strcmp(nodeRootType.value(), "ElementButton") != 0)
        return false;

    return LoadFromXml(nodeRoot);
}

bool ElementButton::LoadFromXml(const pugi::xml_node& nodeSelf)
{
    if (!Element::LoadFromXml(nodeSelf))
        return false;

    pugi::xml_node nodeComponents = nodeSelf.child("Components");
    if (nodeComponents)
    {
        for (pugi::xml_node nodeComponentElement = nodeComponents.child("Element"); nodeComponentElement; nodeComponentElement = nodeComponentElement.next_sibling("Element"))
        {
            pugi::xml_attribute nodeElementName = nodeComponentElement.attribute("name");
            if (nodeElementName && std::strcmp(nodeElementName.value(), "Idle") == 0 && !m_spriteIdle)
            {
                m_spriteIdle = new ElementSpriteGroup;
                m_spriteIdle->LoadFromXml(nodeComponentElement);
                m_spriteIdle->SetParent(this, true);
            }
            else if (nodeElementName && std::strcmp(nodeElementName.value(), "Focused") == 0 && !m_spriteFocused)
            {
                m_spriteFocused = new ElementSpriteGroup;
                m_spriteFocused->LoadFromXml(nodeComponentElement);
                m_spriteFocused->SetParent(this, true);
            }
            else if (nodeElementName && std::strcmp(nodeElementName.value(), "Disabled") == 0 && !m_spriteDisabled)
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
