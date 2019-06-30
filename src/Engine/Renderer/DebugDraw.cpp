#include "DebugDraw.h"

using namespace BLAengine;

BLA_IMPLEMENT_SINGLETON(DebugDraw)

DebugDraw::DebugDraw(DebugRenderingManager* manager)
{
	m_drawDebugRays = true;
	this->m_debugRenderManager = manager;
}

DebugDraw::~DebugDraw(void)
{}

void DebugDraw::Update()
{
	if (m_drawDebugRays)
	{
		m_debugRenderManager->m_lineMeshes.clear();
		m_debugRenderManager->LoadDebugLineMesh(m_lineMeshVertsAndColor);
	}
	m_lineMeshVertsAndColor.first.clear();
	m_lineMeshVertsAndColor.second.clear();
}

void DebugDraw::DrawLine(const blaVec3& origin, const blaVec3& destination)
{
	GetSingletonInstance()->InstanceDrawLine(origin, destination, blaVec3(0.f));
}

void DebugDraw::DrawLine(const blaVec3& origin, const blaVec3& destination, const blaVec3& color)
{
	GetSingletonInstance()->InstanceDrawLine(origin, destination, color);
}

void DebugDraw::DrawRay(const Ray& ray)
{
	GetSingletonInstance()->InstanceDrawRay(ray, blaVec3(0.f));
}

void DebugDraw::DrawRay(const Ray& ray, const blaVec3& color)
{
	GetSingletonInstance()->InstanceDrawRay(ray, color);
}

void DebugDraw::DrawSphere(const blaVec3& origin, blaF32 radius)
{
	GetSingletonInstance()->InstanceDrawSphere(origin, radius, blaVec3(0.f));
}

void DebugDraw::DrawSphere(const blaVec3& origin, blaF32 radius, const blaVec3& color)
{
	GetSingletonInstance()->InstanceDrawSphere(origin, radius, color);
}

void DebugDraw::DrawBasis(const blaPosQuat& transform)
{
	GetSingletonInstance()->InstanceDrawBasis(transform, 1.f, 1.f);
}

void DebugDraw::DrawBasis(const blaPosQuat& transform, float opacity)
{
	GetSingletonInstance()->InstanceDrawBasis(transform, 1.f, opacity);
}

void DebugDraw::DrawBasis(const blaPosQuat& transform, float size, float opacity)
{
	GetSingletonInstance()->InstanceDrawBasis(transform, size, opacity);
}

void DebugDraw::InstanceDrawLine(const blaVec3& origin, const blaVec3& destination, const blaVec3& color)
{
	m_lineMeshVertsAndColor.first.push_back(origin);
	m_lineMeshVertsAndColor.first.push_back(destination);
	m_lineMeshVertsAndColor.second.push_back(color);
	m_lineMeshVertsAndColor.second.push_back(color);
}

void DebugDraw::InstanceDrawRay(const Ray& ray, const blaVec3& color)
{
	blaVec3 destination = ray.m_origin + ray.m_length* ray.m_direction;

	InstanceDrawLine(ray.m_origin, destination, color);
}

void DebugDraw::InstanceDrawSphere(const blaVec3& origin, blaF32 radius, const blaVec3& color)
{

}

void DebugDraw::InstanceDrawBasis(const blaPosQuat& transform, float size, float opacity)
{
	InstanceDrawLine(transform.GetTranslation3(), transform.TransformPoint(blaVec3(size, 0.f, 0.f)), opacity * blaVec3(1.f, 0.f, 0.f));
	InstanceDrawLine(transform.GetTranslation3(), transform.TransformPoint(blaVec3(0.f, size, 0.f)), opacity * blaVec3(0.f, 1.f, 0.f));
	InstanceDrawLine(transform.GetTranslation3(), transform.TransformPoint(blaVec3(0.f, 0.f, size)), opacity * blaVec3(0.f, 0.f, 1.f));
}
