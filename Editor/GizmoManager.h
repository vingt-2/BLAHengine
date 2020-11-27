// BLAEngine Copyright (C) 2016-2020 Vincent Petrella. All rights reserved.

#pragma once

#include "Core/GameObject.h"
#include "Maths/Maths.h"

namespace BLA
{
    namespace Core
    {
        class Scene;
    }

    class EditorCommandManager;
    class GizmoControl
    {
    public:
        GizmoControl(Core::GameObject object, EditorCommandManager* editorCommandManager) : m_controlledObject(object), m_sizeMultiplier(1.f), m_editorCommandManager(editorCommandManager) {}

        virtual void Update();

        bool ControlsObject(Core::GameObject obj) { return obj == m_controlledObject; }

        virtual bool IsBeingControlled() = 0;
    protected:
        Core::GameObject m_controlledObject;
        EditorCommandManager* m_editorCommandManager;
        blaF32 m_sizeMultiplier;
        blaScaledTransform m_preClickTransform;
    };

    class TranslationGizmoControl : public GizmoControl
    {
    public:
        TranslationGizmoControl(Core::GameObject object, EditorCommandManager* editorCommandManager) : GizmoControl(object, editorCommandManager), m_arrowLengths(1.f), m_arrowThickness(0.2f), m_distanceToCenter(0.5f), m_selectedAxis(NONE)
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
        ScaleGizmoControl(Core::GameObject object, EditorCommandManager* editorCommandManager) : GizmoControl(object, editorCommandManager), m_cubeScale(0.15f), m_distanceToCenter(1.f), m_selectedAxis(NONE)
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
        RotationGizmoControl(Core::GameObject object, EditorCommandManager* editorCommandManager) : GizmoControl(object, editorCommandManager), m_radius(1.f), m_selectedAxis(NONE)
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
        GizmoManager(EditorCommandManager* editorCommandManager);

        void Update();

    private:
        EditorCommandManager* m_editorCommandManager;
        GizmoControl* m_gizmoController;
    };
}
