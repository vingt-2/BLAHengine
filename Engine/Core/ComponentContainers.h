// BLAEngine Copyright (C) 2016-2020 Vincent Petrella. All rights reserved.

#pragma once
#include "StdInclude.h"
#include "Core/GameObject.h"
#include "Core/GameComponent.h"

// TODO
// TODO: Use per component pool and use placement constructor for instantiation
// TODO

namespace BLA
{
    namespace Core
    {
        template <class T>
        using InputComponents = blaHashMap<GameObjectID, const T*, GameObjectID::Hasher>;

        template <class T>
        using OutputComponents = blaHashMap<GameObjectID, T*, GameObjectID::Hasher>;

        class ComponentContainer;

        class BLACORE_API ComponentSystemIOInterface
        {
        public:

            ComponentSystemIOInterface(ComponentContainer* pContainer) :
                m_pComponentContainer(pContainer) {}

            template<class T>
            InputComponents<T> GetInputComponents();

            template<class T>
            OutputComponents<T> GetOutputComponents();

        private:

            ComponentContainer* m_pComponentContainer;
        };

        class BLACORE_API ComponentContainer
        {
        public:
            ComponentContainer() : m_ioInterface(this) {}
            friend class ComponentSystemIOInterface;

            GameComponent* AddComponent(GameObjectID object, blaStringId componentId);
            bool RemoveComponent(GameObjectID object, blaStringId componentId);

            void Clear();

            // TODO: Should we really keep this ? ... Should we really have GameComponent pointers  ?? mmmm ?
            GameComponent* GetComponentPerObject(GameComponentID componentId, GameObjectID obj);
            blaVector<GameComponent*> GetComponentsPerObject(GameObjectID obj);

            ComponentSystemIOInterface& GetComponentSystemIOInterface() { return m_ioInterface; }

        private:
            // TODO: Temp structures, this is to improve into a proper system down the line
            blaHashMap<blaStringId, blaHashMap<blaStringId, GameComponent*, blaStringId::Hasher>, blaStringId::Hasher> m_components;
            ComponentSystemIOInterface m_ioInterface;
        };

        template <class T>
        InputComponents<T> ComponentSystemIOInterface::GetInputComponents()
        {
            blaStringId componentName = T::ms_componentDescriptor.m_typeID;
            if (m_pComponentContainer->m_components.find(componentName) != m_pComponentContainer->m_components.end())
            {
                InputComponents<T> input;
                for (auto oc : m_pComponentContainer->m_components[componentName])
                {
                    input[oc.first] = static_cast<const T*>(oc.second);
                }
                return input;
            }

            // BLA_ASSERT(false);
            return InputComponents<T>();
        }

        template <class T>
        OutputComponents<T> ComponentSystemIOInterface::GetOutputComponents()
        {
            blaStringId componentName = T::ms_componentDescriptor.m_typeID;
            if (m_pComponentContainer->m_components.find(componentName) != m_pComponentContainer->m_components.end())
            {
                OutputComponents<T> output;
                for (auto oc : m_pComponentContainer->m_components[componentName])
                {
                    output[oc.first] = static_cast<T*>(oc.second);
                }
                return output;
            }

            // BLA_ASSERT(false);
            return OutputComponents<T>();
        }
    } 
}
