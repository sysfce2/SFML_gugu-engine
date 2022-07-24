#pragma once

////////////////////////////////////////////////////////////////
// Includes

#include "Gugu/Element/Element.h"

////////////////////////////////////////////////////////////////
// Forward Declarations

namespace gugu
{
    class ElementSprite;
    class ElementListItem;
}

////////////////////////////////////////////////////////////////
// File Declarations

namespace gugu {
    
class ElementList : public Element
{
public:

    ElementList();
    virtual ~ElementList();

    void SetImageSet        (const std::string& _strImageSetPath);
    
    void AddItem            (ElementListItem* _pNewItem);
    void RemoveItem         (int _iIndex);
    void RemoveItem         (ElementListItem* _pItem);
    void RemoveAllItems     ();
    
    void    GetItems        (std::vector<ElementListItem*>& _vecItems) const;
    int     GetItemCount    () const;

    void SetAllowSelection      (bool _bAllow);
    void SetMultipleSelection   (bool _bMultiple);

    void SetItemSelected    (int _iIndex, bool selected);
    void SetItemSelected    (ElementListItem* _pNewItem, bool selected);
    void ToggleItemSelected (int _iIndex);
    void ToggleItemSelected (ElementListItem* _pNewItem);

    int                 GetSelectedIndex    () const;
    ElementListItem*    GetSelectedItem     () const;
    Element*            GetSelectedElement  () const;
    void                GetSelectedIndexes  (std::vector<int>& _vecIndexes) const;
    void                GetSelectedItems    (std::vector<ElementListItem*>& _vecItems) const;
    void                GetSelectedElements (std::vector<Element*>& _vecElements) const;

protected:

    void OnMousePressed(const ElementInteractionInfos& interactionInfos);
    void OnMouseScrolled(const ElementInteractionInfos& interactionInfos);
    void OnSliderDragMove(const ElementInteractionInfos& interactionInfos);

    virtual void RenderImpl(RenderPass& _kRenderPass, const sf::Transform& _kTransformSelf) override;
    virtual void OnSizeChanged() override;

    int     ScrollItems(int _iDelta);

    void    RecomputeScrollBar();
    void    RecomputeItems();
    
protected:

    ElementSprite* m_scrollBackground;
    ElementSprite* m_scrollSlider;
    ElementSprite* m_scrollButtonTop;
    ElementSprite* m_scrollButtonBottom;

    std::vector<ElementListItem*> m_items;
    int m_currentIndexTop;
    int m_displayedItemCount;

    bool m_allowSelection;
    bool m_multipleSelection;
};

}   // namespace gugu
