#pragma once

#include <Engine/Core/GameObject.h>

namespace BLAengine
{
    class Scene;
    class GizmoControl;

    class GizmoControl
    {
    public:
        GizmoControl(GameObjectReference object) : m_controlledObject(object), m_sizeMultiplier(1.f) {}

        virtual void Update();

        bool ControlsObject(GameObjectReference obj) { return obj == m_controlledObject; }

        virtual bool IsBeingControlled() = 0;
    protected:
        GameObjectReference m_controlledObject;
        blaF32 m_sizeMultiplier;
    };

    class TranslationGizmoControl : public GizmoControl
    {
    public:
        TranslationGizmoControl(GameObjectReference object) : GizmoControl(object), m_arrowLengths(1.f), m_arrowThickness(0.2f), m_distanceToCenter(0.5f), m_selectedAxis(NONE)
        {}

        void Update() override;
        bool IsBeingControlled() { return m_selectedAxis != NONE; }

    protected:
        blaF32 m_arrowThickness;
        blaF32 m_arrowLengths;
        blaF32 m_distanceToCenter;

        blaVec3 m_startingObjectOrigin;
        blaVec3 m_startingClickPosition;
        enum ESelectedAxis : blaU8
        {
            NONE = 0,
            X_AXIS,
            Y_AXIS,
            Z_AXIS,
            CENTER_AXIS
        } m_selectedAxis;
    };

    class ScaleGizmoControl : public GizmoControl
    {
    public:
        ScaleGizmoControl(GameObjectReference object) : GizmoControl(object), m_cubeScale(0.15f), m_distanceToCenter(1.f), m_selectedAxis(NONE)
        {}

        void Update() override;
        bool IsBeingControlled() { return m_selectedAxis != NONE; }

    protected:
        blaF32 m_cubeScale;
        blaF32 m_distanceToCenter;

        blaVec3 m_startingObjectScale;
        blaVec3 m_startingClickPosition;
        enum ESelectedAxis : blaU8
        {
            NONE = 0,
            X_AXIS,
            Y_AXIS,
            Z_AXIS,
            CENTER_AXIS
        } m_selectedAxis;

    };

    class RotationGizmoControl : public GizmoControl
    {
    public:
        RotationGizmoControl(GameObjectReference object) : GizmoControl(object), m_radius(1.f), m_selectedAxis(NONE)
        {}

        void Update() override;
        bool IsBeingControlled() { return m_selectedAxis != NONE; }

    protected:
        blaF32 m_radius;

        blaQuat m_startingRotation;
        blaVec3 m_startingClickPosition;
        enum ESelectedAxis : blaU8
        {
            NONE = 0,
            X_AXIS,
            Y_AXIS,
            Z_AXIS,
            CENTER_AXIS
        } m_selectedAxis;
    };

    class GizmoManager
    {
    public:
        GizmoManager();

        void Update();

    private:

        GizmoControl* m_gizmoController;
    };
}
