// BLAEngine Copyright (C) 2016-2020 Vincent Petrella. All rights reserved.

#pragma once
#include "BLAReference.h"
#include "BLAStringID.h"

#define INVALID_OBJECT_ID GameObjectID("", 0)

namespace BLA
{
    namespace Core
    {
        class GameComponent;

        typedef blaStringId GameObjectID;
        typedef blaStringId GameComponentID;

        class BLACORE_API GameObject
        {
            enum GameObjectFlagTypes : blaU32
            {
                DIRTY_WORLD_TRANSFORM = 1 << 0,
            };

            typedef blaU32 GameObjectFlags;

            friend class Scene;

            GameObjectID m_gameObjectId;

        public:
            GameObject() : m_gameObjectId(INVALID_OBJECT_ID)
            {}

            GameObject(GameObjectID objectId) : m_gameObjectId(objectId)
            {}

            blaBool IsValid() const;

            blaBool operator==(const GameObject& other) const
            {
                return m_gameObjectId == other.m_gameObjectId;
            }

            blaBool operator!=(const GameObject& other) const
            {
                return !(*this == other);
            }

            operator blaStringId() const
            {
                return m_gameObjectId;
            }

            void Reset() { m_gameObjectId = INVALID_OBJECT_ID; }

            static GameObject InvalidReference() { return GameObject(); };

            blaVector<GameComponent*> GetAllComponents() const;

            GameComponent* GetComponent(GameComponentID componentId);

            template<class ComponentType>
            ComponentType* GetComponent();

            GameObject GetParent() const;
            void SetParent(GameObject parent) const;

            GameObjectID GetId() const { return m_gameObjectId; }

            GameComponent* CreateComponent(GameComponentID componentId);
            bool DeleteComponent(GameComponentID componentId);

            template<class T>
            T* CreateComponent();
        };

        template<class T>
        T* GameObject::GetComponent()
        {
            return static_cast<T*>(GetComponent(T::ms_componentDescriptor.m_typeID));
        }

        template <class T>
        T* GameObject::CreateComponent()
        {
            return static_cast<T*>(CreateComponent(T::ms_componentDescriptor.m_typeID));
        }
    }
}
