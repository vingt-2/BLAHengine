#include "GizmoManager.h"
#include "Engine/Core/DebugDraw.h"
#include "Engine/Core/Scene.h"
#include "Engine/Renderer/GL33Renderer.h"
#include "EditorSession.h"
#include "Engine/System/Console.h"

#pragma optimize("",off)

using namespace BLAengine;

GizmoManager::GizmoManager() : m_sizeMultiplier(1.f), m_arrowLengths(1.f), m_arrowThickness(0.1f), m_distanceToCenter(0.2f)
{}

bool AABB(blaVec3 origin, blaVec3 halfExtent, const Ray& ray)
{
	blaVec3 bounds[] = { origin - halfExtent, origin + halfExtent };


	Console::LogMessage("Screen ray direction" + std::to_string(ray.m_direction.x) + ", " + std::to_string(ray.m_direction.y) + ", " + std::to_string(ray.m_direction.z));

	blaF32 tmin, tmax, tymin, tymax, tzmin, tzmax;

	blaF32 dx = ray.m_direction.x;
	blaF32 dy = ray.m_direction.y;
	blaF32 dz = ray.m_direction.z;

	blaF32 ox = ray.m_origin.x;
	blaF32 oy = ray.m_origin.y;
	blaF32 oz = ray.m_origin.z;

	tmin = (bounds[dx < 0].x - ox) / dx;
	tmax = (bounds[1 - (dx < 0)].x - ox) / dx;
	tymin = (bounds[dy < 0].y - oy) / dy;
	tymax = (bounds[1 -(dy < 0)].y - oy) / dy;

	if ((tmin > tymax) || (tymin > tmax))
		return false;
	if (tymin > tmin)
		tmin = tymin;
	if (tymax < tmax)
		tmax = tymax;

	tzmin = (bounds[dz < 0].z - oz) / dz;
	tzmax = (bounds[1 - (dz < 0)].z - oz) * dz;

	if ((tmin > tzmax) || (tzmin > tmax))
		return false;
	if (tzmin > tmin)
		tmin = tzmin;
	if (tzmax < tmax)
		tmax = tzmax;

	return true;
}

bool OBB(blaVec3 origin, blaVec3 halfExtent, const blaPosQuat t, const Ray& ray)
{
	blaPosQuat invT = t.GetInverse();
	Ray invRay(invT.TransformPoint(ray.m_origin), invT.TransformVector(ray.m_direction), ray.m_length);
	return AABB(origin, halfExtent, invRay);
}

void GizmoManager::Update()
{
	EditorSession* editorSess = dynamic_cast<EditorSession*>(EngineInstance::GetSingletonInstance());

	if (!editorSess)
		return;

	if (!editorSess->GetSelectedObject().IsValid())
		return;

	ObjectTransform& transform = editorSess->GetSelectedObject()->GetTransform();

	if(GameComponent* camera = editorSess->GetWorkingScene()->GetMainCamera()) 
	{
		m_sizeMultiplier = glm::length(camera->GetObjectTransform().GetPosition() - transform.GetPosition()) / 5.f;
	}

	blaF32 halfArrowLength = m_sizeMultiplier * 0.5f * m_arrowLengths;
	blaF32 halfArrowThickness = m_sizeMultiplier * 0.5f * m_arrowThickness;

	// Render gizmos ...

	blaPosQuat xT = blaPosQuat(transform.GetPosition() + transform.GetRotation() * blaVec3(halfArrowLength + 2 * halfArrowThickness + m_distanceToCenter, 0, 0), transform.GetRotation());

	Ray screenRay;
	if (const BlaGuiRenderWindow* guiRenderWindow = dynamic_cast<const BlaGuiRenderWindow*>(BlaGuiManager::GetSingletonInstance()->GetWindow("Editor Window")))
	{
		screenRay = editorSess->GetRenderer().ScreenToRay(guiRenderWindow->GetMousePointerScreenSpaceCoordinates());
	}

	Console::LogMessage("Screen ray direction" + std::to_string(screenRay.m_direction.x) + ", " + std::to_string(screenRay.m_direction.y) + ", " + std::to_string(screenRay.m_direction.z));

	if(OBB(xT.GetTranslation3(), blaVec3(halfArrowLength, halfArrowThickness, halfArrowThickness), xT, screenRay)) 
	{
		DebugDraw::DrawPlainOBB(xT, blaVec3(halfArrowLength, halfArrowThickness, halfArrowThickness), blaVec4(1, 0, 0, 0.5f));
		DebugDraw::DrawOBB(xT, blaVec3(halfArrowLength, halfArrowThickness, halfArrowThickness), blaVec3(1,0,0));
	}
	else
	{
		DebugDraw::DrawPlainOBB(xT, blaVec3(halfArrowLength, halfArrowThickness, halfArrowThickness), blaVec4(BLAColors::ORANGE, 0.5f));
		DebugDraw::DrawOBB(xT, blaVec3(halfArrowLength, halfArrowThickness, halfArrowThickness), BLAColors::ORANGE);
	}

	blaPosQuat yT = blaPosQuat(transform.GetPosition() + transform.GetRotation() * blaVec3(0, halfArrowLength + 2 * halfArrowThickness + m_distanceToCenter, 0), transform.GetRotation());
	DebugDraw::DrawPlainOBB(yT, blaVec3(halfArrowThickness, halfArrowLength, halfArrowThickness), blaVec4(0, 1, 0, 0.5f));
	DebugDraw::DrawOBB(yT, blaVec3(halfArrowThickness, halfArrowLength, halfArrowThickness), blaVec3(0, 1, 0));

	blaPosQuat zT = blaPosQuat(transform.GetPosition() + transform.GetRotation() * blaVec3(0, 0, halfArrowLength + 2 * halfArrowThickness + m_distanceToCenter), transform.GetRotation());
	DebugDraw::DrawPlainOBB(zT, blaVec3(halfArrowThickness, halfArrowThickness, halfArrowLength), blaVec4(0, 0, 1, 0.5f));
	DebugDraw::DrawOBB(zT, blaVec3(halfArrowThickness, halfArrowThickness, halfArrowLength), blaVec3(0, 0, 1));

	DebugDraw::DrawPlainOBB(transform.GetPosQuat(), blaVec3(0.75f * halfArrowThickness), blaVec4(BLAColors::YELLOW, 0.5f));
	DebugDraw::DrawOBB(transform.GetPosQuat(), blaVec3(0.75f * halfArrowThickness), BLAColors::YELLOW);
}
