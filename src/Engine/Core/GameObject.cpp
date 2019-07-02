#include <Engine/Core/GameComponent.h>
#include "GameObject.h"

using namespace BLAengine;

GameObject::GameObject(string name, const GameObjectReference& parent):
    m_objectName(name),
    m_localTransform(ObjectTransform()),
    m_parent(parent)
{
    m_objectState |= DIRTY_WORLD_TRANSFORM;
}


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
    return m_cachedWorldTransform;
}

ObjectTransform& GameObject::GetTransform()
{
    if (m_parent.IsValid() && m_objectState & DIRTY_WORLD_TRANSFORM)
    {
        m_cachedWorldTransform.m_posQuat = m_parent->GetTransform().m_posQuat * m_localTransform.m_posQuat;
        m_cachedWorldTransform.m_scale = m_parent->GetTransform().m_scale * m_localTransform.m_scale;
        m_objectState &= ~DIRTY_WORLD_TRANSFORM;
    }
    return m_cachedWorldTransform;
}

ObjectTransform& GameObject::GetLocalTransform()
{
    return m_localTransform;
}

void GameObject::SetTransform(const ObjectTransform& transform)
{
    ObjectTransform& parentT = m_parent->GetTransform();

    m_localTransform.m_posQuat = parentT.m_posQuat.GetInverse() * transform.m_posQuat;
    m_localTransform.m_scale = transform.m_scale / parentT.m_scale; //TODO: Make this safe.

    m_cachedWorldTransform = transform;
    m_objectState &= ~DIRTY_WORLD_TRANSFORM;
}

void GameObject::SetLocalTransform(const ObjectTransform& transform)
{
    m_objectState |= DIRTY_WORLD_TRANSFORM;

    m_localTransform = transform;
}

void GameObject::AddComponent(GameComponent* component)
{
    m_componentVector.push_back(component);
}