#include "GameObject.h"

using namespace BLAengine;

GameObject::GameObject(string name):
    m_objectName(name)
{
    m_transform = new ObjectTransform();
}


GameObject::~GameObject(void)
{
    m_transform->~ObjectTransform();
}

void GameObject::Update()
{
    for (size_t c = 0; c < this->m_componentVector.size(); c++)
    {
        GameComponent* component = this->m_componentVector[c];

        component->Update();
    }
}

const ObjectTransform& GameObject::GetTransform() 
{
    return *m_transform;
}

void GameObject::SetTransform(const ObjectTransform& transform)
{
    *m_transform = transform;
}

void GameObject::AddComponent(GameComponent* component)
{
    m_componentVector.push_back(component);
    component->m_parentObject = this;
    component->Update();
}