#pragma once

#include <Engine/Core/GameObject.h>

namespace BLAengine
{
    class Scene;
    class GizmoManager
    {
    public:
        GizmoManager() {};

    private:
        void CreateManipulationGizmo();

        GameObjectReference m_selectedObjectManipulationGizmo; 
    };
}
