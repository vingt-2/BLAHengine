#pragma once
#include "../../../Common/StdInclude.h"
#include "../ObjectTransform.h"

namespace BLAengine
{
    class GameObject;
    class BLACORE_API GameComponent
    {
    public:

        GameComponent() : m_parentObject(nullptr){}

        virtual void Update() = 0;

        const ObjectTransform& GetObjectTransform();

        GameObject* m_parentObject;
    };
}