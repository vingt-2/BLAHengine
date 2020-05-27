// BLAEngine Copyright (C) 2016-2020 Vincent Petrella. All rights reserved.

#include "Core/GameComponent.h"
#include "Core/Scene.h"
#include "GameObject.h"

#define SCENE Scene::GetSingletonInstance()

using namespace BLA;

void GameObject::SetParent(GameObject parent) const
{
    SCENE->SetGameObjectParent(*this, parent);
    SCENE->m_gameObjectsFlags[*this] |= DIRTY_WORLD_TRANSFORM;
}

blaBool GameObject::IsValid() const
{
    if (m_gameObjectId == INVALID_OBJECT_ID) return false;

    if (std::find(SCENE->GetObjectsID().begin(), SCENE->GetObjectsID().end(), m_gameObjectId) != SCENE->GetObjectsID().end()) return true;

    return false;
}

blaVector<GameComponent*> GameObject::GetAllComponents() const
{
    return SCENE->GetComponentsPerObject(*this);
}

GameComponent* GameObject::GetComponent(GameComponentID componentId)
{
    return SCENE->GetComponentPerObject(componentId, *this);
}

GameObject GameObject::GetParent() const
{
    return SCENE->GetGameObjectParent(*this);
}

GameComponent* GameObject::CreateComponent(GameComponentID componentId)
{
    return SCENE->AddComponent(*this, componentId);
}

bool GameObject::DeleteComponent(GameComponentID componentId)
{
    return SCENE->RemoveComponent(*this, componentId);
}

//namespace std {
//
//    template <>
//    struct hash<blaStringId>
//    {
//        std::size_t operator()(const blaStringId& k) const
//        {
//            return k.m_crc;
//        }
//    };
//
//}


