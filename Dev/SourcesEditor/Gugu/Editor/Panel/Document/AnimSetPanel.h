#pragma once

////////////////////////////////////////////////////////////////
// Includes

#include "Gugu/Editor/Panel/Document/DocumentPanel.h"

#include "Gugu/Math/Vector2.h"

////////////////////////////////////////////////////////////////
// Forward Declarations

namespace gugu
{
    class DeltaTime;
    class RenderViewport;
    class AnimSet;
}

////////////////////////////////////////////////////////////////
// File Declarations

namespace gugu {

class AnimSetPanel : public DocumentPanel
{
public:

    AnimSetPanel(AnimSet* resource);
    virtual ~AnimSetPanel();

    virtual void UpdateProperties(const gugu::DeltaTime& dt) override;

protected:

    virtual void UpdatePanelImpl(const gugu::DeltaTime& dt) override;

private:

    RenderViewport* m_renderViewport;

    AnimSet* m_animSet;
};

}   //namespace gugu
