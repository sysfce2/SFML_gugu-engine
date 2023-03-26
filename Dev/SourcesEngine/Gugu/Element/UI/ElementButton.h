#pragma once

////////////////////////////////////////////////////////////////
// Includes

#include "Gugu/Element/Element.h"
#include "Gugu/Core/Callback.h"

////////////////////////////////////////////////////////////////
// Forward Declarations

namespace gugu
{
    class ElementText;
    class Texture;
}

////////////////////////////////////////////////////////////////
// File Declarations

namespace gugu {
    
namespace ETextAlignment {
    enum Type
    {
        Center,
        Left,
        Right,
        Top,
        Bottom,
    };
}

class ElementButton : public Element
{
public:

    ElementButton();
    virtual ~ElementButton();

    [[deprecated("Deprecated, use LoadFromFile() instead.")]]
    void SetTexture(const std::string& textureIdleID, const std::string& textureFocusedID = "", const std::string& textureDisabledID = "");
    [[deprecated("Deprecated, use LoadFromFile() instead.")]]
    void SetTexture(Texture* textureIdle, Texture* textureFocused, Texture* textureDisabled);

    void SetText(const std::string& _strText);

    // Use alignment for origin, and alignment + offset for position.
    void SetTextAlignment(const UDim2& alignment, const Vector2f& offset = Vector2f(0.f, 0.f));

    void SetDisabled(bool _bDisabled);

    void SetOnMousePressed(const Callback& _pActionOnPressed);
    void SetOnMouseReleased(const Callback& _pActionOnReleased);

    ElementText* GetElementText() const;

    bool LoadFromFile(const std::string& path);

protected:

    void OnMousePressed(const InteractionInfos& interactionInfos);
    void OnMouseReleased(const InteractionInfos& interactionInfos);
    void OnMouseEntered(const InteractionInfos& interactionInfos);
    void OnMouseLeft(const InteractionInfos& interactionInfos);

    void SetTextureImpl(Texture* textureIdle, Texture* textureFocused, Texture* textureDisabled);

    virtual void RenderImpl(RenderPass& _kRenderPass, const sf::Transform& _kTransformSelf) override;
    virtual void OnSizeChanged() override;
    
    virtual bool LoadFromXmlImpl(const pugi::xml_node& node) override;

protected:

    Element* m_spriteIdle;
    Element* m_spriteFocused;
    Element* m_spriteDisabled;
    Element* m_currentSprite;

    ElementText* m_text;

    Callback m_actionOnPressed;
    Callback m_actionOnReleased;

    bool m_isDisabled;
};

}   // namespace gugu
