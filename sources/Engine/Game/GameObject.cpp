#include <Engine/Game/GameComponents/GameComponent.h>
#include "GameObject.h"

using namespace BLAengine;

GameObject::GameObject(string name, const GameObjectReference& parent):
    m_objectName(name),
    m_transform(ObjectTransform()),
    m_parent(parent)
{}


GameObject::~GameObject(void)
{
    for(int i = 0; i < m_componentVector.size(); ++i)
    {
        delete m_componentVector[i];
    }
}

void GameObject::Update()
{
    for (size_t c = 0; c < this->m_componentVector.size(); c++)
    {
        GameComponent* component = this->m_componentVector[c];

        component->Update();
    }
}

const ObjectTransform& GameObject::GetPotentialDirtyTransform() const
{
    return m_transform;
}

ObjectTransform& GameObject::GetTransform()
{
    if (m_parent.IsValid() && m_objectState & DIRTY_TRANSFORM)
    {
        m_transform.m_transform = m_parent->GetTransform().m_transform * m_transform.m_transform;
        m_transform.m_scale = m_parent->GetTransform().m_scale * m_transform.m_scale;
        m_objectState &= ~DIRTY_TRANSFORM;
    }
    return m_transform;
}

//TODO: We might not actually want to recursively update the world transform every time we change a parent
void GameObject::SetTransform(const ObjectTransform& transform)
{
    m_objectState |= DIRTY_TRANSFORM;

    m_transform = transform;
}

void GameObject::AddComponent(GameComponent* component)
{
    m_componentVector.push_back(component);
}