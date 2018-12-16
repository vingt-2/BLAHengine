#pragma once
#include "../../../Common/StdInclude.h"
#include "../ObjectTransform.h"

#define BLA_CREATE_COMPONENT(ComponentType, parentObject)\
    parentObject != nullptr ? new ComponentType(parentObject) : nullptr;

namespace BLAengine
{
    class GameObject;
    class BLACORE_API GameComponent
    {
    public:

        GameComponent(GameObject* parentObject);

        virtual void Update() = 0;

        const ObjectTransform& GetObjectTransform() const;

        GameObject* GetParentObject() { return m_parentObject; };

    private:
        GameObject* m_parentObject;
    };
}