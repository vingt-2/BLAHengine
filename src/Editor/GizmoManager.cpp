#include "GizmoManager.h"
#include "Engine/Core/DebugDraw.h"
#include "Engine/Core/Scene.h"
#include "Engine/Renderer/GL33Renderer.h"
#include "EditorSession.h"
#include "Engine/System/Console.h"
#include "Engine/System/InputManager.h"

using namespace BLAengine;

GizmoManager::GizmoManager()
{}

bool AABB(blaVec3 origin, blaVec3 halfExtent, const Ray& ray, blaF32& tmin, blaF32& tmax)
{
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

bool Disc(blaVec3 origin, blaVec3 normal, blaF32 radius, const Ray& ray)
{
	blaVec3 vec1 = normalize(glm::cross(normal, blaVec3(0, 1, 0)));
	blaVec3 vec2 = normalize(glm::cross(normal, vec1));
	RaycastHit hit = ray.RayToPlaneIntersection(origin, vec1, vec2);
	return glm::length2(hit.hitPosition - origin) < radius;
}

Ray screenRay;
void GizmoManager::Update()
{
	EditorSession* editorSess = dynamic_cast<EditorSession*>(EngineInstance::GetSingletonInstance());
	GameObjectReference objRef = editorSess->GetSelectedObject();

	if(objRef.IsValid())
	{
		if(!m_gizmoController)
		{
			m_gizmoController = new TranslationGizmoControl(objRef);
		}
		else
		{
			if (!m_gizmoController->ControlsObject(objRef))
			{
				delete m_gizmoController;
				m_gizmoController = new TranslationGizmoControl(objRef);
			}

			m_gizmoController->Update();
		}
	}
	else
	{
		delete m_gizmoController;
		m_gizmoController = nullptr;
	}
}

void GizmoControl::Update()
{
	EditorSession* editorSess = dynamic_cast<EditorSession*>(EngineInstance::GetSingletonInstance());

	if (!editorSess)
		return;

	ObjectTransform& transform = m_controlledObject->GetTransform();

	GameComponent* camera = editorSess->GetWorkingScene()->GetMainCamera();
	if (camera)
	{
		m_sizeMultiplier = glm::length(camera->GetObjectTransform().GetPosition() - transform.GetPosition()) / 5.f;
	}
	else
	{
		Console::LogError("Gizmo Manager failed to find a camera component in the scene, aborting Gizmo update");
		return;
	}
}

void TranslationGizmoControl::Update()
{
	GizmoControl::Update();

	EditorSession* editorSess = dynamic_cast<EditorSession*>(EngineInstance::GetSingletonInstance());

	if (!editorSess)
		return;

	ObjectTransform& transform = m_controlledObject->GetTransform();

	blaF32 halfArrowLength = m_sizeMultiplier * 0.5f * m_arrowLengths;
	blaF32 halfArrowThickness = m_sizeMultiplier * 0.5f * m_arrowThickness;

	BLAMouseState leftMouseButton = InputManager::GetSingletonInstance()->GetMouseButtonState(BLA_MOUSE_BUTTON_1);

	Ray screenRay;
	if (const BlaGuiRenderWindow* guiRenderWindow = dynamic_cast<const BlaGuiRenderWindow*>(BlaGuiManager::GetSingletonInstance()->GetWindow("Editor Window")))
	{
		screenRay = editorSess->GetRenderer().ScreenToRay(guiRenderWindow->GetMousePointerScreenSpaceCoordinates());
	}
	else
	{
		return;
	}

	ESelectedAxis currentScreenAxis = NONE;

	blaPosQuat xT = blaPosQuat(transform.GetPosition() + transform.GetRotation() * blaVec3(halfArrowLength + 2 * halfArrowThickness + m_distanceToCenter, 0, 0), transform.GetRotation());
	blaPosQuat yT = blaPosQuat(transform.GetPosition() + transform.GetRotation() * blaVec3(0, halfArrowLength + 2 * halfArrowThickness + m_distanceToCenter, 0), transform.GetRotation());
	blaPosQuat zT = blaPosQuat(transform.GetPosition() + transform.GetRotation() * blaVec3(0, 0, halfArrowLength + 2 * halfArrowThickness + m_distanceToCenter), transform.GetRotation());

	if (m_selectedAxis == NONE)
	{
		blaF32 tmin, tmax;

		if (OBB(xT, blaVec3(halfArrowLength, halfArrowThickness, halfArrowThickness), screenRay, tmin, tmax))
			currentScreenAxis = X_AXIS;
		else if (OBB(yT, blaVec3(halfArrowThickness, halfArrowLength, halfArrowThickness), screenRay, tmin, tmax))
			currentScreenAxis = Y_AXIS;
		else if (OBB(zT, blaVec3(halfArrowThickness, halfArrowThickness, halfArrowLength), screenRay, tmin, tmax))
			currentScreenAxis = Z_AXIS;
		else if (OBB(transform.GetPosQuat(), blaVec3(0.95f * halfArrowThickness), screenRay, tmin, tmax))
			currentScreenAxis = CENTER_AXIS;

		if(leftMouseButton.IsDown())
		{
			m_selectedAxis = currentScreenAxis;
			m_startingClickPosition = screenRay.m_origin + tmin * screenRay.m_direction;
			m_startingObjectOrigin = transform.GetPosition();
		}
	}
	else
	{
		if (leftMouseButton.IsUp())
		{
			m_selectedAxis = NONE;
		}

		currentScreenAxis = m_selectedAxis;
	}

	blaVec3 color = currentScreenAxis == X_AXIS ? BLAColors::WHITE : BLAColors::RED;
	DebugDraw::DrawPlainOBB(xT, blaVec3(halfArrowLength, halfArrowThickness, halfArrowThickness), blaVec4(color, 0.5f));
	DebugDraw::DrawOBB(xT, blaVec3(halfArrowLength, halfArrowThickness, halfArrowThickness), color);

	color = currentScreenAxis == Y_AXIS ? BLAColors::WHITE : BLAColors::GREEN;
	DebugDraw::DrawPlainOBB(yT, blaVec3(halfArrowThickness, halfArrowLength, halfArrowThickness), blaVec4(color, 0.5f));
	DebugDraw::DrawOBB(yT, blaVec3(halfArrowThickness, halfArrowLength, halfArrowThickness), color);

	color = currentScreenAxis == Z_AXIS ? BLAColors::WHITE : BLAColors::BLUE;
	DebugDraw::DrawPlainOBB(zT, blaVec3(halfArrowThickness, halfArrowThickness, halfArrowLength), blaVec4(color, 0.5f));
	DebugDraw::DrawOBB(zT, blaVec3(halfArrowThickness, halfArrowThickness, halfArrowLength), color);

	color = currentScreenAxis == CENTER_AXIS ? BLAColors::WHITE : BLAColors::YELLOW;
	DebugDraw::DrawPlainOBB(transform.GetPosQuat(), blaVec3(0.95f * halfArrowThickness), blaVec4(color, 0.5f));
	DebugDraw::DrawOBB(transform.GetPosQuat(), blaVec3(0.95f * halfArrowThickness), color);
}

