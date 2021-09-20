#pragma once

////////////////////////////////////////////////////////////////
// Includes

#include "Gugu/Element/2D/ElementSprite.h"
#include "Gugu/Animation/SpriteAnimation.h"

////////////////////////////////////////////////////////////////
// Forward Declarations

namespace gugu
{
    class Animation;
    class AnimationFrame;
}

////////////////////////////////////////////////////////////////
// File Declarations

namespace gugu {
    
class ElementSpriteAnimated : public ElementSprite, public SpriteAnimation
{
public:

    ElementSpriteAnimated();
    virtual ~ElementSpriteAnimated();

    virtual void Step(const DeltaTime& dt) override;
    virtual void Update(const DeltaTime& dt) override;
    
protected:
    
    virtual void InitCurrentAnimationFrame() override;

protected:

    bool m_isStepOnUpdate;  //TODO: Refactor in AutoUpdate ?
};

}   // namespace gugu
