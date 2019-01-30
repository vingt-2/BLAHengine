#pragma once
#include <Common/StdInclude.h>
#include <Common/DataStructures/Tree.h>
#include "GameComponents/GameComponent.h"

#include <Common/Types.h>

namespace BLAengine
{
    class BLACORE_API GameObject : public IntrusiveTree<GameObject>
    {
        // Game component is a friend class to GameObjects. They call Add Component upon Creation
        friend class GameComponent;

    public:

        void Update();

        const ObjectTransform& GetTransform() const;
        void SetTransform(const ObjectTransform& transform);

        GameObject(string name);
        ~GameObject(void);

        void SetName(std::string name) { m_objectName = name; }
        std::string GetName() const { return m_objectName; }

        vector<GameComponent*> GetAllComponents() const;

        template<class ComponentType>
        ComponentType* GetComponent();

        template<class ComponentType>
        ComponentType* GetComponentInChildren();

        template<class ComponentType>
        vector<ComponentType*> GetComponents();

        template<class ComponentType>
        vector<ComponentType*> GetComponentsInChildren();

    private:

        void AddComponent(GameComponent* component);

        string m_objectName;
        ObjectTransform m_transform;

        vector<GameComponent*> m_componentVector;
    };
    
    inline vector<GameComponent*> GameObject::GetAllComponents() const
    {
        return m_componentVector;
    }

    template<class ComponentType>
    ComponentType* GameObject::GetComponent()
    {
        for (size_t i = 0; i < m_componentVector.size(); i++)
        {
            GameComponent* gComp = m_componentVector[i];
            if (ComponentType* comp = dynamic_cast<ComponentType*>(gComp))
            {
                return comp;
            }
        }
        return nullptr;
    }

    template<class ComponentType>
    vector<ComponentType*> GameObject::GetComponents()
    {
        vector<ComponentType*> comps;

        for (size_t i = 0; i < m_componentVector.size(); i++)
        {
            GameComponent* baseComp = m_componentVector[i];

            ComponentType* comp;
            
            BLA_DYNAMIC_CAST(ComponentType, baseComp, comp);

            if(comp != nullptr)
            {
                comps.push_back(comp);
            }
        }
        return comps;
    }
}
