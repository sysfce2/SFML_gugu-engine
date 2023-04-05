////////////////////////////////////////////////////////////////
// Header

#include "Gugu/Common.h"
#include "Gugu/Element/2D/ElementSpriteGroup.h"

////////////////////////////////////////////////////////////////
// Includes

#include "Gugu/Element/ElementUtility.h"
#include "Gugu/Resources/ManagerResources.h"
#include "Gugu/Resources/ElementWidget.h"
#include "Gugu/Window/Renderer.h"
#include "Gugu/Resources/Texture.h"
#include "Gugu/Resources/ImageSet.h"
#include "Gugu/System/SystemUtility.h"
#include "Gugu/External/PugiXmlUtility.h"

#include <SFML/Graphics/RenderTarget.hpp>

////////////////////////////////////////////////////////////////
// File Implementation

namespace gugu {

ElementSpriteGroupItem::ElementSpriteGroupItem()
    : m_spriteGroup(nullptr)
    , m_cachedVertexCount(0)
{
}

ElementSpriteGroupItem::~ElementSpriteGroupItem()
{
}

void ElementSpriteGroupItem::SetSpriteGroup(ElementSpriteGroup* spriteGroup)
{
    m_spriteGroup = spriteGroup;

    if (!m_parent)
    {
        SetParent(m_spriteGroup);
    }
}

void ElementSpriteGroupItem::RaiseDirtyVertices()
{
    ElementSpriteBase::RaiseDirtyVertices();

    if (m_spriteGroup)
    {
        m_spriteGroup->RaiseNeedRecompute();
    }
}

void ElementSpriteGroupItem::OnTransformChanged()
{
    RaiseDirtyVertices();
}

void ElementSpriteGroupItem::OnVisibleChanged()
{
    RaiseDirtyVertices();
}

bool ElementSpriteGroupItem::HasDirtyVertices() const
{
    return m_dirtyVertices;
}

size_t ElementSpriteGroupItem::RecomputeVertexCount()
{
    m_cachedVertexCount = ElementSpriteBase::GetRequiredVertexCount();
    return m_cachedVertexCount;
}

size_t ElementSpriteGroupItem::GetCachedVertexCount() const
{
    return m_cachedVertexCount;
}

size_t ElementSpriteGroupItem::RecomputeItemVertices(sf::VertexArray& vertices, size_t indexFirstVertex)
{
    m_dirtyVertices = false;

    ElementSpriteBase::RecomputeVerticesPositionAndTextureCoords(&vertices[indexFirstVertex]);
    ElementSpriteBase::RecomputeVerticesColor(&vertices[indexFirstVertex], m_cachedVertexCount);

    // Combine generated vertices with the item transform.
    const sf::Transform& transform = GetTransform();
    for (size_t i = 0; i < m_cachedVertexCount; ++i)
    {
        vertices[indexFirstVertex + i].position = transform * vertices[indexFirstVertex + i].position;
    }

    return m_cachedVertexCount;
}

bool ElementSpriteGroupItem::LoadFromDataImpl(ElementDataContext& context)
{
    if (!ElementSpriteBase::LoadFromDataImpl(context))
        return false;

    ElementSpriteGroupItemData* spriteGroupItemData = dynamic_cast<ElementSpriteGroupItemData*>(context.data);

    return true;
}


ElementSpriteGroup::ElementSpriteGroup()
    : m_texture(nullptr)
    , m_needRecompute(true)
{
}

ElementSpriteGroup::~ElementSpriteGroup()
{
    ClearStdVector(m_items);
}

void ElementSpriteGroup::SetTexture(const std::string& _strTexturePath)
{
    SetTexture(GetResources()->GetTexture(_strTexturePath));
}

void ElementSpriteGroup::SetTexture(Texture* _pTexture)
{
    m_texture = _pTexture;
}

Texture* ElementSpriteGroup::GetTexture() const
{
    return m_texture;
}

void ElementSpriteGroup::OnSizeChanged()
{
    for (size_t i = 0; i < m_items.size(); ++i)
    {
        m_items[i]->OnParentResized();
    }
}

void ElementSpriteGroup::RenderImpl(RenderPass& _kRenderPass, const sf::Transform& _kTransformSelf)
{
    if (!m_texture || !m_texture->GetSFTexture())
        return;

    //TODO: maybe need a parameter to bypass this check ?
    sf::FloatRect kGlobalTransformed = _kTransformSelf.transformRect(sf::FloatRect(Vector2::Zero_f, m_size));
    if (_kRenderPass.rectViewport.intersects(kGlobalTransformed))
    {
        if (m_needRecompute)
        {
            m_needRecompute = false;

            size_t indexForceRecompute = (size_t)-1;

            size_t totalVertexCount = 0;
            for (size_t i = 0; i < m_items.size(); ++i)
            {
                size_t cachedVertexCount = m_items[i]->GetCachedVertexCount();
                
                size_t currentVertexCount = 0;
                if (m_items[i]->IsVisible())
                {
                    currentVertexCount = m_items[i]->RecomputeVertexCount();
                    totalVertexCount += currentVertexCount;
                }

                if (indexForceRecompute == (size_t)-1 && cachedVertexCount != currentVertexCount)
                {
                    indexForceRecompute = i;
                }
            }

            // Reset vertices
            m_vertices.setPrimitiveType(sf::Triangles);
            m_vertices.resize(totalVertexCount);

            if (totalVertexCount > 0)
            {
                size_t indexItemVertices = 0;
                for (size_t i = 0; i < m_items.size(); ++i)
                {
                    if (m_items[i]->IsVisible())
                    {
                        if (i >= indexForceRecompute || m_items[i]->HasDirtyVertices())
                        {
                            indexItemVertices += m_items[i]->RecomputeItemVertices(m_vertices, indexItemVertices);
                        }
                        else
                        {
                            indexItemVertices += m_items[i]->GetCachedVertexCount();
                        }
                    }
                }
            }
        }

        if (m_vertices.getVertexCount() > 0)
        {
            sf::RenderStates states;
            states.transform = _kTransformSelf;
            states.texture = m_texture->GetSFTexture();
            _kRenderPass.target->draw(m_vertices, states);

            //Stats
            if (_kRenderPass.frameInfos)
            {
                _kRenderPass.frameInfos->statDrawCalls += 1;
                _kRenderPass.frameInfos->statTriangles += (int)m_vertices.getVertexCount() / 3;
            }

            //TODO: special stat category for ElementSpriteGroup
        }
    }
}

void ElementSpriteGroup::RaiseNeedRecompute()
{
    m_needRecompute = true;
}

size_t ElementSpriteGroup::AddItem(ElementSpriteGroupItem* _pNewItem)
{
    _pNewItem->SetSpriteGroup(this);
    m_items.push_back(_pNewItem);

    m_needRecompute = true;
    return m_items.size() - 1;
}

ElementSpriteGroupItem* ElementSpriteGroup::GetItem(size_t _iIndex) const
{
    if (_iIndex < 0 || _iIndex >= m_items.size())
        return nullptr;
    return m_items[_iIndex];
}

const std::vector<ElementSpriteGroupItem*>& ElementSpriteGroup::GetItems() const
{
    return m_items;
}

bool ElementSpriteGroup::LoadFromDataImpl(ElementDataContext& context)
{
    if (!Element::LoadFromDataImpl(context))
        return false;

    ElementSpriteGroupData* spriteGroupData = dynamic_cast<ElementSpriteGroupData*>(context.data);
    bool result = true;

    if (spriteGroupData->imageSet)
    {
        SetTexture(spriteGroupData->imageSet->GetTexture());
    }
    else if (spriteGroupData->texture)
    {
        SetTexture(spriteGroupData->texture);
    }
    else
    {
        return false;
    }

    size_t componentCount = spriteGroupData->components.size();
    if (componentCount > 0)
    {
        ElementData* backupData = context.data;

        for (size_t i = 0; i < componentCount; ++i)
        {
            ElementData* componentData = spriteGroupData->components[i];
            ElementSpriteGroupItem* component = new ElementSpriteGroupItem;

            AddItem(component);

            context.data = componentData;
            result &= component->LoadFromData(context);
        }

        context.data = backupData;
    }

    m_needRecompute = true;
    return result;
}

}   // namespace gugu
