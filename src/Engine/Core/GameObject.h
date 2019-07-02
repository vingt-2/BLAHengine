#pragma once
#include <Common/BLAReference.h>
#include <Common/DataStructures/Tree.h>
#include <Common/Types.h>
#include "ObjectTransform.h"

namespace BLAengine
{
	class GameObject;
	class BLACORE_API GameObjectReference : public BLAReference<GameObject>
	{
        BLA_DECLARE_REFERENCE(GameObjectReference, GameObject)
		friend class Scene;
	};

    class BLACORE_API GameObject
    {
        // Game component is a friend class to GameObjects. They call Add Component upon Creation
        friend class GameComponent;
        friend class Scene;

    public:

        enum GameObjectStateBits : blaU32
        {
            DIRTY_WORLD_TRANSFORM = 1 << 0,
        };

        typedef blaU32 GameObjectState;

        GameObject(string name, const GameObjectReference& parent);
        ~GameObject(void);

        void Update();

        const ObjectTransform& GetPotentialDirtyTransform() const;
        
        ObjectTransform& GetTransform();
        ObjectTransform& GetLocalTransform();

        void SetTransform(const ObjectTransform& transform);
        void SetLocalTransform(const ObjectTransform& transform);

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

        GameObjectReference GetParent() { return m_parent; }

        void SetParent(GameObjectReference parent)
        {
            m_parent = parent;
            m_objectState |= DIRTY_WORLD_TRANSFORM;
        }

    private:

        void AddComponent(GameComponent* component);

        // Behold the stuff that should die.
        string m_objectName;
        vector<GameComponent*> m_componentVector;

        ObjectTransform m_localTransform;
        ObjectTransform m_cachedWorldTransform;

        GameObjectState m_objectState;
        GameObjectReference m_parent;


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
