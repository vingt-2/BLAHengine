#pragma once

#include <Engine/Game/Scene.h>
#include <Engine/Renderer/Renderer.h>

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
