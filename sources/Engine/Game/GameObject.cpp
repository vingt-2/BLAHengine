#include <Engine/Game/GameComponents/GameComponent.h>
#include "GameObject.h"

using namespace BLAengine;

GameObject::GameObject(string name):
    m_objectName(name),
    m_transform(ObjectTransform())
{}


GameObject::~GameObject(void)
{}

void GameObject::Update()
{
    for (size_t c = 0; c < this->m_componentVector.size(); c++)
    {
        GameComponent* component = this->m_componentVector[c];

        component->Update();
    }
}

const ObjectTransform& GameObject::GetTransform() const
{
    return m_transform;
}

//TODO: We might not actually want to recursively update the world transform every time we change a parent
void GameObject::SetTransform(const ObjectTransform& transform)
{
    /*GameObjectReference child = GetChild();
    while(child != nullptr)
    {
        ObjectTransform newChildT = child->GetTransform();
        newChildT.GetPosQuat() = transform.GetPosQuat() * newChildT.GetPosQuat();
        child->SetTransform(newChildT);
    }*/
    m_transform = transform;
}

void GameObject::AddComponent(GameComponent* component)
{
    m_componentVector.push_back(component);
}