#include "GameObject.h"

using namespace BLAengine;

GameObject::GameObject(string name):
    m_objectName(name)
{
    m_transform = new Transform();
}


GameObject::~GameObject(void)
{
    m_transform->~Transform();
}

void BLAengine::GameObject::Update()
{
    this->UpdateTransform();

    for (int c = 0; c < this->m_componentVector.size(); c++)
    {
        GameComponent* component = this->m_componentVector[c];

        component->Update();
    }
}

const Transform& GameObject::GetTransform() 
{
    return *m_transform;
}

void GameObject::SetTransform(const Transform& transform)
{
    m_transform->m_position = transform.m_position;
    m_transform->m_rotation = transform.m_rotation;
    m_transform->m_scale = transform.m_scale;

    m_transform->UpdateTransform();
}

void GameObject::UpdateTransform()
{
    this->m_transform->UpdateTransform();
}

void GameObject::AddComponent(GameComponent* component)
{
    m_componentVector.push_back(component);
    component->m_parentObject = this;
    component->Update();
}