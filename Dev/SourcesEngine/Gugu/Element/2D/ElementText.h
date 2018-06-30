#pragma once

////////////////////////////////////////////////////////////////
// Includes

#include "Gugu/Element/Element.h"
#include "Gugu/Utility/Types.h"

#include <SFML/Graphics/Color.hpp>

////////////////////////////////////////////////////////////////
// Forward Declarations

namespace gugu
{
    class Font;
    class Action;
}

namespace sf
{
    class Text;
}

////////////////////////////////////////////////////////////////
// File Declarations

namespace gugu {
    
namespace ETextResizeRule
{
    enum Type
    {
        FixedSize,      //No auto-resize, no auto-scale
        FitSize,        //Recompute size to fit the text, no auto-scale (Default) (use before SetValue)
        FitHeight,      //Recompute height to fit the text, no auto-scale (Use before SetSize)
        FitScale,       //No auto-resize, rescale text to fill (Use before SetSize)

        //TODO: Find a more robust way yo handle initialization :
        //if setsize is called while FitSize is active and text is empty, it will be reset to size=0,
        //and following instructions with FitHeight and SetValue wont be able to resize the Element.
    };
}

class ElementText : public Element
{
public:

	ElementText();
	virtual ~ElementText();

	void SetText		(const std::string& _strValue /* , bool _bResize = true */);

	void SetFont		(const std::string& _strFontPath);
	void SetFont		(Font* _pFont);
	void SetFontSize	(uint32 _uiSize);
	void SetFontColor	(const sf::Color& _oColor);

    void SetResizeRule  (ETextResizeRule::Type _eResizeRule);
	void SetMultiline	(bool _bIsMultiline);
	void SetEditable	(bool _bIsEditable);

    void StartEdition   ();
    void StopEdition    ();

    virtual void Update(const DeltaTime& dt) override;

    virtual bool OnMouseSelected() override;
    virtual bool OnMouseDeselected() override;
    virtual bool OnSFEvent(const sf::Event& _oSFEvent) override;

	std::string GetValue() const;

	void SetOnValidate	    (Action* _pAction);

    void SetDebugBoundsVisible(bool _bShowDebugBounds);
    
private:

    void StartEditionImpl   ();
    void StopEditionImpl    ();

	void Recompute();

    virtual void DrawSelf(RenderPass& _kRenderPass, const sf::Transform& _kTransformSelf) override;
    virtual void SetSizeImpl(sf::Vector2f _kOldSize) override;
    
protected:

    Font* m_font;

	sf::Text* m_sfText;
	sf::Text* m_sfTextCursor;

	std::string m_textValue;

    ETextResizeRule::Type m_resizeRule;
	bool m_isMultiline;
	bool m_isEditable;
    
    bool m_skipRecomputeOnResize;
    bool m_isEditing;
    bool m_isTickDisplayed;
    float m_timeSinceTick;

	std::vector<sf::Text*> m_components;

	Action* m_actionOnValidate;

    bool m_showDebugBounds;
};

}   // namespace gugu
