#pragma once
#include <Common/StdInclude.h>
#include <Engine/Core/ObjectTransform.h>
#include <Engine/Core/GameObject.h>
#include <Common/BLASingleton.h>

#include "ComponentReflection.h"

#define BLA_CREATE_COMPONENT(ComponentType, parentObject)\
    parentObject.IsValid() ? new ComponentType(parentObject) : nullptr;


namespace BLAengine
{
    class GameObject;
    class BLACORE_API GameComponent
    {
    public:

        GameComponent(GameObjectReference parentObject);
        virtual ~GameComponent();
        virtual void Update() = 0;

        const ObjectTransform& GetObjectTransform() const;
        const ObjectTransform& GetLocalObjectTransform() const;

        GameObjectReference GetParentObject() const { return m_parentObject; }

    private:
        GameObjectReference m_parentObject;
    };

    //Todo: Don't return GameComponent raw pointers... but references ?
    class GameComponentManager
    {
        BLA_DECLARE_SINGLETON(GameComponentManager)

        friend GameComponent;
        class ComponentManagerEntry
        {
        public:
            typedef GameComponent* (*ComponentFactory)(GameObjectReference);
            ComponentManagerEntry(const blaString& name, ComponentFactory factory) : m_componentName(name), m_factory(factory) {}

            GameComponent* CreateComponent(GameObjectReference parentObj) const { return m_factory(parentObj); }
            const blaString& GetName() const { return m_componentName; }

        private:
            blaString m_componentName;
            ComponentFactory m_factory;
        };

        typedef blaVector<ComponentManagerEntry> CompEntries;
        CompEntries m_componentEntries;

    public:
        void __RegisterComponent(const blaString& name, ComponentManagerEntry::ComponentFactory factory);
        blaVector<blaString> GetComponents();
        GameComponent* CreateComponent(const blaString& componentName, GameObjectReference objRef);
    };
}

/*
 *  Component declaration interface:
 *  BLA_DECLARE_COMPONENT(COMPONENT_NAME)
 *      blaVec3 m_someValue;
 *      blaVector<blaU32> m_someVector;
 *      
 *      void SomeComponentPrivateFunction();
 *  END_COMPONENT_DECLARATION()
 */
	