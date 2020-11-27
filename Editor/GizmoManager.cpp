// BLAEngine Copyright (C) 2016-2020 Vincent Petrella. All rights reserved.

#include "GizmoManager.h"
#include "EditorCommands.h"
#include "Core/DebugDraw.h"
#include "Core/Scene.h"
#include "Renderer/Renderer.h"
#include "SceneEditor.h"
#include "System/Console.h"
#include "System/InputManager.h"
#include "Geometry/PrimitiveGeometry.h"
#include "Physics/ColliderComponent.h"
#include "Core/TransformComponent.h"
#include "External/libcoldet/coldet.h"
#include "Gui/DevGuiManager.h"

#define MAX(a,b) (a > b ? a : b)

#define MIN_SCALE_VALUE -999999999.f// Wanna not let people make inverted scales ? use 0.0001f

using namespace BLA;

GizmoManager::GizmoManager(EditorCommandManager* editorCommandManager) : m_editorCommandManager(editorCommandManager), m_gizmoController(nullptr)
{}

bool AABB(blaVec3 origin, blaVec3 halfExtent, const Ray& ray, blaF32& tmin, blaF32& tmax)
{
    if (isnan(ray.m_direction.x) || isnan(ray.m_direction.y) || isnan(ray.m_direction.z))
    {
        return false;
    }

    blaVec3 bounds[] = { -halfExtent, halfExtent };

    blaF32 tymin, tymax, tzmin, tzmax;

    blaF32 dx = ray.m_direction.x;
    blaF32 dy = ray.m_direction.y;
    blaF32 dz = ray.m_direction.z;

    blaF32 ox = ray.m_origin.x - origin.x;
    blaF32 oy = ray.m_origin.y - origin.y;
    blaF32 oz = ray.m_origin.z - origin.z;

    tmin = (bounds[dx < 0].x - ox) / dx;
    tmax = (bounds[1 - (dx < 0)].x - ox) / dx;
    tymin = (bounds[dy < 0].y - oy) / dy;
    tymax = (bounds[1 - (dy < 0)].y - oy) / dy;

    if ((tmin > tymax) || (tymin > tmax))
        return false;
    if (tymin > tmin)
        tmin = tymin;
    if (tymax < tmax)
        tmax = tymax;

    tzmin = (bounds[dz < 0].z - oz) / dz;
    tzmax = (bounds[1 - (dz < 0)].z - oz) / dz;

    if ((tmin > tzmax) || (tzmin > tmax))
        return false;
    if (tzmin > tmin)
        tmin = tzmin;
    if (tzmax < tmax)
        tmax = tzmax;

    return true;
}

bool OBB(const blaPosQuat& t, blaVec3 halfExtent, const Ray& ray, blaF32& tmin, blaF32& tmax)
{
    blaPosQuat invT = t.GetInverse();
    Ray invRay(invT.TransformPoint(ray.m_origin), invT.TransformVector(ray.m_direction), ray.m_length);
    return AABB(blaVec3(0), halfExtent, invRay, tmin, tmax);
}

bool Disc(const blaVec3& origin, const blaVec3& normal, blaF32 radius, const Ray& ray, blaVec3& outPosition, float& outT)
{
    blaVec3 n = SafeNormalize(normal);

    RaycastHit hit = RayToPlaneIntersection(ray, origin, normal, outT);

    outPosition = hit.hitPosition;

    return glm::length(hit.hitPosition - origin) < radius;
}

TriangleMesh cone = PrimitiveGeometry::MakeCone(40);

Ray screenRay;
void GizmoManager::Update()
{
    SceneEditor* editorSess = dynamic_cast<SceneEditor*>(EngineInstance::GetSingletonInstance());
    Core::GameObject objRef = editorSess->GetSelectedObject();

    if (objRef.IsValid())
    {
        if (!m_gizmoController || !m_gizmoController->ControlsObject(objRef))
        {
            if (InputManager::GetSingletonInstanceRead()->GetKeyState(BLA_KEY_LEFT_ALT).IsDown())
            {
                m_gizmoController = new RotationGizmoControl(objRef, m_editorCommandManager);
            }
            else if (InputManager::GetSingletonInstanceRead()->GetKeyState(BLA_KEY_LEFT_CONTROL).IsDown())
            {
                m_gizmoController = new ScaleGizmoControl(objRef, m_editorCommandManager);
            }
            else
            {
                m_gizmoController = new TranslationGizmoControl(objRef, m_editorCommandManager);
            }
        }
        else if (!m_gizmoController->IsBeingControlled())
        {
            if (InputManager::GetSingletonInstanceRead()->GetKeyState(BLA_KEY_LEFT_ALT).IsDown())
            {
                if (!dynamic_cast<RotationGizmoControl*>(m_gizmoController))
                {
                    delete m_gizmoController;
                    m_gizmoController = new RotationGizmoControl(objRef, m_editorCommandManager);
                }
            }
            else if (InputManager::GetSingletonInstanceRead()->GetKeyState(BLA_KEY_LEFT_CONTROL).IsDown())
            {
                if (!dynamic_cast<ScaleGizmoControl*>(m_gizmoController))
                {
                    delete m_gizmoController;
                    m_gizmoController = new ScaleGizmoControl(objRef, m_editorCommandManager);
                }
            }
            else
            {
                if (!dynamic_cast<TranslationGizmoControl*>(m_gizmoController))
                {
                    delete m_gizmoController;
                    m_gizmoController = new TranslationGizmoControl(objRef, m_editorCommandManager);
                }
            }
        }

        m_gizmoController->Update();

    }
    else
    {
        delete m_gizmoController;
        m_gizmoController = nullptr;
    }
}

void GizmoControl::Update()
{
    SceneEditor* editorSess = dynamic_cast<SceneEditor*>(EngineInstance::GetSingletonInstance());

    if (!editorSess)
        return;

    blaScaledTransform transform = m_controlledObject.GetComponent<TransformComponent>()->GetTransform();

    Core::GameComponent* camera = editorSess->GetWorkingScene()->GetMainCamera();
    if (camera)
    {
        blaVec3 camPos = camera->GetOwnerObject().GetComponent<TransformComponent>()->GetTransform().GetPosition();
        m_sizeMultiplier = glm::length(camPos - transform.GetPosition()) / 5.f;
    }
    else
    {
        Console::LogError("Gizmo Manager failed to find a camera component in the scene, aborting Gizmo update");
    }
}

void TranslationGizmoControl::Update()
{
    GizmoControl::Update();

    SceneEditor* editorSess = dynamic_cast<SceneEditor*>(EngineInstance::GetSingletonInstance());
    Core::GameComponent* camera = editorSess->GetWorkingScene()->GetMainCamera();

    if (!editorSess)
        return;

    TransformComponent* controlledTransform = m_controlledObject.GetComponent<TransformComponent>();

    blaScaledTransform transform = controlledTransform->GetTransform();

    blaF32 halfArrowLength = m_sizeMultiplier * 0.5f * m_arrowLengths;
    blaF32 halfArrowThickness = m_sizeMultiplier * 0.5f * m_arrowThickness;

    BLAMouseState leftMouseButton = InputManager::GetSingletonInstance()->GetMouseButtonState(BLA_MOUSE_BUTTON_1);

    Ray screenRay;
    if (const DevGuiRenderViewportWindow* guiRenderWindow = dynamic_cast<const DevGuiRenderViewportWindow*>(DevGuiManager::GetSingletonInstance()->GetWindow("Editor Window")))
    {
        // Screen to ray should be two steps: QUerry the camera of interest to get clipspace coord
        // externally figure out the relationship with the screen size ... (or texture size or whatever size really)
        // screenRay = editorSess->GetRenderer()->ScreenToRay(guiRenderWindow->GetMousePointerScreenSpaceCoordinates());
    }
    else
    {
        return;
    }

    ESelectedAxis currentScreenAxis = NONE;

    blaVec3 coneVerticalScaling = blaVec3(1.f, 3.f, 1.f);

    blaVec3 objectX = transform.GetRotation() * blaVec3(1.f, 0.f, 0.f);
    blaVec3 objectY = transform.GetRotation() * blaVec3(0.f, 1.f, 0.f);
    blaVec3 objectZ = transform.GetRotation() * blaVec3(0.f, 0.0f, 1.f);

    blaF32 d = halfArrowLength + 2.0f * halfArrowThickness + m_distanceToCenter;
    blaPosQuat xT = blaPosQuat(transform.GetPosition() + objectX * d, transform.GetRotation() * blaPosQuat::EulerToQuat(0, 0, -0.5f * M_PI));
    blaPosQuat yT = blaPosQuat(transform.GetPosition() + objectY * d, transform.GetRotation());
    blaPosQuat zT = blaPosQuat(transform.GetPosition() + objectZ * d, transform.GetRotation() * blaPosQuat::EulerToQuat(0.5f * M_PI, 0, 0));

    if (m_selectedAxis == NONE)
    {
        m_preClickTransform = transform;
        // Todo: write an owned pointer wrapper
        CollisionModel3D* collider = newCollisionModel3D();

        const blaVector<blaU32>&  triangleIndices = cone.m_renderData.m_triangleIndices;
        const blaVector<blaVec3>& vertPos = cone.m_renderData.m_vertPos;

        for (size_t i = 0; i < triangleIndices.size(); i += 3)
        {
            collider->addTriangle(&(vertPos[triangleIndices[i]].x),
                &(vertPos[triangleIndices[i + 1]].x),
                &(vertPos[triangleIndices[i + 2]].x));

        }
        collider->finalize();

        int triangleIndex;
        blaVec3 colPointL;

        float tmin, tmax;
        blaMat4 TransformComponentForCollider;

        blaScaledTransform coneTransform;
        coneTransform.m_scale = coneVerticalScaling * halfArrowThickness;

        blaF32 closestT = MAX_NORMAL_FLOAT;

        coneTransform.m_posQuat = xT;
        coneTransform.GetScaledTransformMatrix(TransformComponentForCollider);
        collider->setTransform(&(TransformComponentForCollider[0][0]));

        if (collider->threadSafeClosestRayCollision(&(screenRay.m_origin.x), &(screenRay.m_direction.x), triangleIndex, tmin, &colPointL.x))
        {
            if (tmin < closestT)
            {
                closestT = tmin;
                currentScreenAxis = X_AXIS;
            }
        }

        coneTransform.m_posQuat = yT;
        coneTransform.GetScaledTransformMatrix(TransformComponentForCollider);
        collider->setTransform(&(TransformComponentForCollider[0][0]));

        if (collider->threadSafeClosestRayCollision(&(screenRay.m_origin.x), &(screenRay.m_direction.x), triangleIndex, tmin, &colPointL.x))
        {
            if (tmin < closestT)
            {
                closestT = tmin;
                currentScreenAxis = Y_AXIS;
            }
        }

        coneTransform.m_posQuat = zT;
        coneTransform.GetScaledTransformMatrix(TransformComponentForCollider);
        collider->setTransform(&(TransformComponentForCollider[0][0]));

        if (collider->threadSafeClosestRayCollision(&(screenRay.m_origin.x), &(screenRay.m_direction.x), triangleIndex, tmin, &colPointL.x))
        {
            if (tmin < closestT)
            {
                closestT = tmin;
                currentScreenAxis = Z_AXIS;
            }
        }

        if (OBB(transform.GetPosQuat(), blaVec3(0.95f * halfArrowThickness), screenRay, tmin, tmax))
        {
            if (tmin < closestT)
            {
                closestT = tmin;
                currentScreenAxis = CENTER_AXIS;
            }
        }

        if (leftMouseButton.IsDown())
        {
            m_selectedAxis = currentScreenAxis;
            m_startingClickPosition = screenRay.m_origin + closestT * screenRay.m_direction;
            m_startingObjectOrigin = transform.GetPosition();
        }

        delete collider;
    }
    else
    {
        if (leftMouseButton.IsUp())
        {
            m_selectedAxis = NONE;
            m_editorCommandManager->Execute(new GameComponentEditCommand(m_controlledObject, BlaStringId("TransformComponent"), BlaStringId("WorldTransform"), ValueCommandDelta(reinterpret_cast<char*>(&m_preClickTransform), reinterpret_cast<char*>(&transform), sizeof(blaScaledTransform))));
        }

        float t;
        currentScreenAxis = m_selectedAxis;
        blaScaledTransform cameraTransform = camera->GetOwnerObject().GetComponent<TransformComponent>()->GetTransform();
        blaVec3 camForward = cameraTransform.GetRotation() * blaVec3(0.f, 0.f, 1.f);
        switch (m_selectedAxis)
        {
        case X_AXIS:
        {
            blaVec3 controlPlane = abs(glm::dot(camForward, objectZ)) > abs(glm::dot(camForward, objectY)) ? objectZ : objectY;
            RaycastHit h = RayToPlaneIntersection(screenRay, m_startingObjectOrigin, controlPlane, t);
            blaF32 posInX = glm::dot(h.hitPosition - m_startingObjectOrigin, objectX);
            blaF32 startClickOnAxis = glm::dot(m_startingClickPosition - m_startingObjectOrigin, objectX);
            transform.SetPosition(m_startingObjectOrigin + (posInX - startClickOnAxis) * objectX);
            break;
        }
        case Y_AXIS:
        {
            blaVec3 controlPlane = abs(glm::dot(camForward, objectZ)) > abs(glm::dot(camForward, objectX)) ? objectZ : objectX;
            RaycastHit h = RayToPlaneIntersection(screenRay, m_startingObjectOrigin, controlPlane, t);
            blaF32 posInY = glm::dot(h.hitPosition - m_startingObjectOrigin, objectY);
            blaF32 startClickOnAxis = glm::dot(m_startingClickPosition - m_startingObjectOrigin, objectY);
            transform.SetPosition(m_startingObjectOrigin + (posInY - startClickOnAxis) * objectY);
            break;
        }
        case Z_AXIS:
        {
            blaVec3 controlPlane = abs(glm::dot(camForward, objectY)) > abs(glm::dot(camForward, objectX)) ? objectY : objectX;
            RaycastHit h = RayToPlaneIntersection(screenRay, m_startingObjectOrigin, controlPlane, t);
            blaF32 posInZ = glm::dot(h.hitPosition - m_startingObjectOrigin, objectZ);
            blaF32 startClickOnAxis = glm::dot(m_startingClickPosition - m_startingObjectOrigin, objectZ);
            transform.SetPosition(m_startingObjectOrigin + (posInZ - startClickOnAxis) * objectZ);
            break;
        }
        case CENTER_AXIS:
        {
            RaycastHit h = RayToPlaneIntersection(screenRay, m_startingObjectOrigin, -camForward, t);
            transform.SetPosition(m_startingObjectOrigin - m_startingClickPosition + h.hitPosition);
            break;
        }
        default:
            break;
        }
    }

    blaVec3 color = currentScreenAxis == X_AXIS ? BLA::WHITE : BLA::RED;
    DebugDraw::DrawLine(transform.GetPosition(), transform.GetPosition() + objectX * d, color);
    blaVec3 coneSize = coneVerticalScaling * halfArrowThickness;
    DebugDraw::DrawArbitraryGeometry(xT, coneSize, cone, blaVec4(color, 0.8f));

    color = currentScreenAxis == Y_AXIS ? BLA::WHITE : BLA::GREEN;
    DebugDraw::DrawLine(transform.GetPosition(), transform.GetPosition() + objectY * d, color);
    DebugDraw::DrawArbitraryGeometry(yT, coneSize, cone, blaVec4(color, 0.8f));

    color = currentScreenAxis == Z_AXIS ? BLA::WHITE : BLA::BLUE;
    DebugDraw::DrawLine(transform.GetPosition(), transform.GetPosition() + objectZ * d, color);
    DebugDraw::DrawArbitraryGeometry(zT, coneSize, cone, blaVec4(color, 0.8f));

    color = currentScreenAxis == CENTER_AXIS ? BLA::WHITE : BLA::YELLOW;
    DebugDraw::DrawPlainOBB(transform.GetPosQuat(), blaVec3(0.94f * halfArrowThickness), blaVec4(color, 0.8f));
    DebugDraw::DrawOBB(transform.GetPosQuat(), blaVec3(0.95f * halfArrowThickness), color);

    controlledTransform->SetTransform(transform);
}

void ScaleGizmoControl::Update()
{
    GizmoControl::Update();

    SceneEditor* editorSess = dynamic_cast<SceneEditor*>(EngineInstance::GetSingletonInstance());
    Core::GameComponent* camera = editorSess->GetWorkingScene()->GetMainCamera();

    if (!editorSess)
        return;

    TransformComponent* controlledTransform = m_controlledObject.GetComponent<TransformComponent>();

    blaScaledTransform transform = controlledTransform->GetTransform();

    blaF32 halfScale = m_sizeMultiplier * 0.5f * m_cubeScale;
    blaF32 distanceToCenter = m_sizeMultiplier * m_distanceToCenter;

    BLAMouseState leftMouseButton = InputManager::GetSingletonInstance()->GetMouseButtonState(BLA_MOUSE_BUTTON_1);

    Ray screenRay;
    if (const DevGuiRenderViewportWindow* guiRenderWindow = dynamic_cast<const DevGuiRenderViewportWindow*>(DevGuiManager::GetSingletonInstance()->GetWindow("Editor Window")))
    {
        // Screen to ray should be two steps: QUerry the camera of interest to get clipspace coord
        // externally figure out the relationship with the screen size ... (or texture size or whatever size really)
        // screenRay = editorSess->GetRenderer()->ScreenToRay(guiRenderWindow->GetMousePointerScreenSpaceCoordinates
    }
    else
    {
        return;
    }

    ESelectedAxis currentScreenAxis = NONE;

    blaVec3 objectX = transform.GetRotation() * blaVec3(1.0f, 0.f, 0.f);
    blaVec3 objectY = transform.GetRotation() * blaVec3(0.f, 1.f, 0.f);
    blaVec3 objectZ = transform.GetRotation() * blaVec3(0.f, 0.f, 1.f);

    blaPosQuat xT = blaPosQuat(transform.GetPosition() + objectX * (halfScale + distanceToCenter), transform.GetRotation());
    blaPosQuat yT = blaPosQuat(transform.GetPosition() + objectY * (halfScale + distanceToCenter), transform.GetRotation());
    blaPosQuat zT = blaPosQuat(transform.GetPosition() + objectZ * (halfScale + distanceToCenter), transform.GetRotation());

    if (m_selectedAxis == NONE)
    {
        m_preClickTransform = transform;
    	
        blaF32 tmin = MAX_NORMAL_FLOAT;
        blaF32 tmax = -MAX_NORMAL_FLOAT;
        float t;
        if (OBB(xT, blaVec3(halfScale), screenRay, t, tmax))
        {
            if (t < tmin)
            {
                tmin = t;
                currentScreenAxis = X_AXIS;
            }
        }
        if (OBB(yT, blaVec3(halfScale), screenRay, t, tmax))
        {
            if (t < tmin)
            {
                tmin = t;
                currentScreenAxis = Y_AXIS;
            }
        }
        if (OBB(zT, blaVec3(halfScale), screenRay, t, tmax))
        {
            if (t < tmin)
            {
                tmin = t;
                currentScreenAxis = Z_AXIS;
            }
        }
        if (OBB(transform.GetPosQuat(), blaVec3(0.95f * halfScale), screenRay, t, tmax))
        {
            if (t < tmin)
            {
                tmin = t;
                currentScreenAxis = CENTER_AXIS;
            }
        }

        if (leftMouseButton.IsDown())
        {
            m_selectedAxis = currentScreenAxis;
            m_startingClickPosition = screenRay.m_origin + tmin * screenRay.m_direction;
            m_startingObjectScale = transform.m_scale;
        }
    }
    else
    {
        if (leftMouseButton.IsUp())
        {
            m_selectedAxis = NONE;
            m_editorCommandManager->Execute(new GameComponentEditCommand(m_controlledObject, BlaStringId("TransformComponent"), BlaStringId("WorldTransform"), ValueCommandDelta(reinterpret_cast<char*>(&m_preClickTransform), reinterpret_cast<char*>(&transform), sizeof(blaScaledTransform))));
        }

        currentScreenAxis = m_selectedAxis;

        float t;
        blaScaledTransform cameraTransform = camera->GetOwnerObject().GetComponent<TransformComponent>()->GetTransform();
        blaVec3 camForward = cameraTransform.GetRotation() * blaVec3(0.f, 0.f, 1.f);
        switch (m_selectedAxis)
        {
        case X_AXIS:
        {
            blaVec3 controlPlane = abs(glm::dot(camForward, objectZ)) > abs(glm::dot(camForward, objectY)) ? objectZ : objectY;
            RaycastHit h = RayToPlaneIntersection(screenRay, transform.GetPosition(), controlPlane, t);
            blaF32 posInX = glm::dot(h.hitPosition - transform.GetPosition(), objectX);
            blaF32 startClickOnAxis = glm::dot(m_startingClickPosition - transform.GetPosition(), objectX);
            transform.m_scale.x = m_startingObjectScale.x * MAX(MIN_SCALE_VALUE, 1.f + (posInX - startClickOnAxis));
            xT.GetTranslation() += blaVec4(objectX, 0.0f) * (posInX - startClickOnAxis);
            break;
        }
        case Y_AXIS:
        {
            blaVec3 controlPlane = abs(glm::dot(camForward, objectX)) > abs(glm::dot(camForward, objectZ)) ? objectX : objectZ;
            RaycastHit h = RayToPlaneIntersection(screenRay, transform.GetPosition(), controlPlane, t);
            blaF32 posInY = glm::dot(h.hitPosition - transform.GetPosition(), objectY);
            blaF32 startClickOnAxis = glm::dot(m_startingClickPosition - transform.GetPosition(), objectY);
            transform.m_scale.y = m_startingObjectScale.y * MAX(MIN_SCALE_VALUE, 1.f + (posInY - startClickOnAxis));
            yT.GetTranslation() += blaVec4(objectY, 0.0f) * (posInY - startClickOnAxis);
            break;
        }
        case Z_AXIS:
        {
            blaVec3 controlPlane = abs(glm::dot(camForward, objectY)) > abs(glm::dot(camForward, objectX)) ? objectY : objectX;
            RaycastHit h = RayToPlaneIntersection(screenRay, transform.GetPosition(), controlPlane, t);
            blaF32 posInZ = glm::dot(h.hitPosition - transform.GetPosition(), objectZ);
            blaF32 startClickOnAxis = glm::dot(m_startingClickPosition - transform.GetPosition(), objectZ);
            transform.m_scale.z = m_startingObjectScale.z * MAX(MIN_SCALE_VALUE, 1.f + (posInZ - startClickOnAxis));
            zT.GetTranslation() += blaVec4(objectZ, 0.0f) * (posInZ - startClickOnAxis);
            break;
        }
        case CENTER_AXIS:
        {
            blaVec3 camUp = cameraTransform.GetRotation() * blaVec3(0.f, 1.f, 0.f);
            blaVec3 camLeft = cameraTransform.GetRotation() * blaVec3(1.f, 0.f, 0.f);
            RaycastHit h = RayToPlaneIntersection(screenRay, transform.GetPosition(), -camForward, t);
            blaVec3 clickPos = h.hitPosition - m_startingClickPosition;
            blaF32 scale = MAX(MIN_SCALE_VALUE, (1.f + (0.5f * glm::dot(clickPos, camUp) + 0.5f * glm::dot(clickPos, camLeft))));
            transform.m_scale = m_startingObjectScale * scale;

            blaVec4 o = transform.GetPosQuat().GetTranslation();

            xT.GetTranslation() = o + transform.GetRotation() * blaVec4(halfScale + distanceToCenter, 0, 0, 0) * scale;
            yT.GetTranslation() = o + transform.GetRotation() * blaVec4(0, halfScale + distanceToCenter, 0, 0) * scale;
            zT.GetTranslation() = o + transform.GetRotation() * blaVec4(0, 0, halfScale + distanceToCenter, 0) * scale;
            break;
        }
        default:
            break;
        }
    }

    blaVec3 color = currentScreenAxis == X_AXIS ? BLA::WHITE : BLA::RED;
    DebugDraw::DrawLine(transform.GetPosition(), xT.GetTranslation3() - objectX * halfScale, color);
    DebugDraw::DrawPlainOBB(xT, 0.99f * blaVec3(halfScale), blaVec4(color, 0.8f));
    DebugDraw::DrawOBB(xT, blaVec3(halfScale), color);

    color = currentScreenAxis == Y_AXIS ? BLA::WHITE : BLA::GREEN;
    DebugDraw::DrawLine(transform.GetPosition(), yT.GetTranslation3() - objectY * halfScale, color);
    DebugDraw::DrawPlainOBB(yT, 0.99f * blaVec3(halfScale), blaVec4(color, 0.8f));
    DebugDraw::DrawOBB(yT, blaVec3(halfScale), color);

    color = currentScreenAxis == Z_AXIS ? BLA::WHITE : BLA::BLUE;
    DebugDraw::DrawLine(transform.GetPosition(), zT.GetTranslation3() - objectZ * halfScale, color);
    DebugDraw::DrawPlainOBB(zT, 0.99f * blaVec3(halfScale), blaVec4(color, 0.8f));
    DebugDraw::DrawOBB(zT, blaVec3(halfScale), color);

    color = currentScreenAxis == CENTER_AXIS ? BLA::WHITE : BLA::YELLOW;
    DebugDraw::DrawPlainOBB(transform.GetPosQuat(), blaVec3(halfScale), blaVec4(color, 0.8f));
    DebugDraw::DrawOBB(transform.GetPosQuat(), blaVec3(halfScale), color);

    controlledTransform->SetTransform(transform);
}

void RotationGizmoControl::Update()
{
    GizmoControl::Update();

    SceneEditor* editorSess = dynamic_cast<SceneEditor*>(EngineInstance::GetSingletonInstance());
    Core::GameComponent* camera = editorSess->GetWorkingScene()->GetMainCamera();

    if (!editorSess)
        return;

    TransformComponent* controlledTransform = m_controlledObject.GetComponent<TransformComponent>();

    blaScaledTransform transform = controlledTransform->GetTransform();

    blaF32 radius = m_sizeMultiplier * m_radius;

    BLAMouseState leftMouseButton = InputManager::GetSingletonInstance()->GetMouseButtonState(BLA_MOUSE_BUTTON_1);

    Ray screenRay;
    if (const DevGuiRenderViewportWindow* guiRenderWindow = dynamic_cast<const DevGuiRenderViewportWindow*>(DevGuiManager::GetSingletonInstance()->GetWindow("Editor Window")))
    {
        // Screen to ray should be two steps: QUerry the camera of interest to get clipspace coord
        // externally figure out the relationship with the screen size ... (or texture size or whatever size really)
        // screenRay = editorSess->GetRenderer()->ScreenToRay(guiRenderWindow->GetMousePointerScreenSpaceCoordinates
    }
    else
    {
        return;
    }

    ESelectedAxis currentScreenAxis = NONE;

    blaVec3 objectX = transform.GetRotation() *  blaVec3(1.0f, 0.f, 0.f);
    blaVec3 objectY = transform.GetRotation() *  blaVec3(0.f, 1.f, 0.f);
    blaVec3 objectZ = transform.GetRotation() *  blaVec3(0.f, 0.f, 1.f);

    if (m_selectedAxis == NONE)
    {
        m_preClickTransform = transform;
    	
        blaVec3 position;
        blaVec3 startClickPosition;
        float t;
        float tmin = MAX_NORMAL_FLOAT;

        if (Disc(transform.GetPosition(), objectX, radius, screenRay, position, t))
        {
            if (t < tmin)
            {
                currentScreenAxis = X_AXIS;
                tmin = t;
                startClickPosition = position;
            }
        }
        if (Disc(transform.GetPosition(), objectY, radius, screenRay, position, t))
        {
            if (t < tmin)
            {
                currentScreenAxis = Y_AXIS;
                tmin = t;
                startClickPosition = position;
            }
        }
        if (Disc(transform.GetPosition(), objectZ, radius, screenRay, position, t))
        {
            if (t < tmin)
            {
                currentScreenAxis = Z_AXIS;
                tmin = t;
                startClickPosition = position;
            }
        }

        if (leftMouseButton.IsDown())
        {
            m_selectedAxis = currentScreenAxis;
            m_startingClickPosition = startClickPosition;
            m_startingRotation = transform.GetRotation();
        }
    }
    else
    {
        if (leftMouseButton.IsUp())
        {
            m_selectedAxis = NONE;
            m_editorCommandManager->Execute(new GameComponentEditCommand(m_controlledObject, BlaStringId("TransformComponent"), BlaStringId("WorldTransform"), ValueCommandDelta(reinterpret_cast<char*>(&m_preClickTransform), reinterpret_cast<char*>(&transform), sizeof(blaScaledTransform))));
        }

        currentScreenAxis = m_selectedAxis;

        float t;
        switch (m_selectedAxis)
        {
        case X_AXIS:
        {
            RaycastHit h = RayToPlaneIntersection(screenRay, transform.GetPosition(), objectX, t);
            transform.SetRotation(QuatBetweenVectors(glm::normalize(m_startingClickPosition - transform.GetPosition()), glm::normalize(h.hitPosition - transform.GetPosition())) * m_startingRotation);
            break;
        }
        case Y_AXIS:
        {
            RaycastHit h = RayToPlaneIntersection(screenRay, transform.GetPosition(), objectY, t);
            transform.SetRotation(QuatBetweenVectors(glm::normalize(m_startingClickPosition - transform.GetPosition()), glm::normalize(h.hitPosition - transform.GetPosition())) * m_startingRotation);
            break;
        }
        case Z_AXIS:
        {
            RaycastHit h = RayToPlaneIntersection(screenRay, transform.GetPosition(), objectZ, t);
            transform.SetRotation(QuatBetweenVectors(glm::normalize(m_startingClickPosition - transform.GetPosition()), glm::normalize(h.hitPosition - transform.GetPosition())) * m_startingRotation);
            break;
        }
        default:
            break;
        }
    }

    blaScaledTransform cameraTransform = camera->GetOwnerObject().GetComponent<TransformComponent>()->GetTransform();
    blaVec3 camForward = cameraTransform.GetRotation() * blaVec3(0.f, 0.f, 1.f);

    blaVec3 color = currentScreenAxis == X_AXIS ? BLA::WHITE : BLA::RED;
    DebugDraw::DrawCircle(transform.GetPosition(), objectX, radius, color);

    color = currentScreenAxis == Y_AXIS ? BLA::WHITE : BLA::GREEN;
    DebugDraw::DrawCircle(transform.GetPosition(), objectY, radius, color);

    color = currentScreenAxis == Z_AXIS ? BLA::WHITE : BLA::BLUE;
    DebugDraw::DrawCircle(transform.GetPosition(), objectZ, radius, color);

	// Fix the plain spheres maybe ?
    //DebugDraw::DrawPlainSphere(transform.GetPosition(), radius, blaVec4(0.3, 0.3, 0.3, 0.3), 48);

    controlledTransform->SetTransform(transform);
}
