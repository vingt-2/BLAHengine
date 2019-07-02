#pragma once
#include <Engine/Core/ObjectTransform.h>
#include <Engine/Core/GameObject.h>

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
}
	