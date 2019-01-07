#include "./Debug.h"
using namespace BLAengine;

Debug::Debug(DebugRenderingManager* manager)
{
    m_drawDebugRays = true;
    this->m_debugRenderManager = manager;
}

Debug::~Debug(void)
{
}

void Debug::Update()
{
    if (m_drawDebugRays)
    {
        m_debugRenderManager->m_lineMeshes.clear();
        m_debugRenderManager->LoadDebugLineMesh(m_lineMeshVertsAndColor);
    }
    m_lineMeshVertsAndColor.first.clear();
    m_lineMeshVertsAndColor.second.clear();
}

void Debug::DrawLine(const blaVec3& origin, const blaVec3& destination)
{
    m_lineMeshVertsAndColor.first.push_back(origin); 
    m_lineMeshVertsAndColor.first.push_back(destination);
    m_lineMeshVertsAndColor.second.push_back(blaVec3(0));
    m_lineMeshVertsAndColor.second.push_back(blaVec3(0));
}

void Debug::DrawLine(const blaVec3& origin, const blaVec3& destination, const blaVec3& color)
{
    m_lineMeshVertsAndColor.first.push_back(origin);
    m_lineMeshVertsAndColor.first.push_back(destination);
    m_lineMeshVertsAndColor.second.push_back(color);
    m_lineMeshVertsAndColor.second.push_back(color);
}

void Debug::DrawRay(Ray ray, const blaVec3& color)
{
    blaVec3 destination = ray.m_origin + ray.m_length* ray.m_direction;

    DrawLine(ray.m_origin, destination, color);
}

void Debug::DrawRay(Ray ray)
{
    blaVec3 destination = ray.m_origin + ray.m_length* ray.m_direction;

    DrawLine(ray.m_origin, destination, blaVec3(1.f));
}

void Debug::DrawSphere(const blaVec3& origin, const blaVec3& color)
{

}

void Debug::DrawSphere(const blaVec3& origin)
{
    DrawSphere(origin, blaVec3(1.f));
}

void Debug::DrawGrid(int size, float spacing, const blaVec3& color)
{
    for (int i=-size/2; i<=size/2; i++)
    {
        float iSpacing = i*spacing;
        float sizeSpacing = size*spacing;
        DrawLine(blaVec3(sizeSpacing /2, 0, iSpacing),blaVec3(-sizeSpacing /2, 0, iSpacing), color);
        DrawLine(blaVec3(iSpacing, 0, sizeSpacing /2),blaVec3(iSpacing, 0, -sizeSpacing /2), color);
    }
}

void Debug::DrawBasis(const blaPosQuat& transform)
{
    DrawBasis(transform, 1.f, 1.f);
}

void Debug::DrawBasis(const blaPosQuat& transform, float opacity)
{
    DrawBasis(transform, 1.f, opacity);
}

void Debug::DrawBasis(const blaPosQuat& transform, float size, float opacity)
{
    DrawLine(transform.GetTranslation3(), transform.TransformPoint(blaVec3(size, 0.f, 0.f)), opacity * blaVec3(1.f, 0.f, 0.f));
    DrawLine(transform.GetTranslation3(), transform.TransformPoint(blaVec3(0.f, size, 0.f)), opacity * blaVec3(0.f, 1.f, 0.f));
    DrawLine(transform.GetTranslation3(), transform.TransformPoint(blaVec3(0.f, 0.f, size)), opacity * blaVec3(0.f, 0.f, 1.f));
}

void Debug::OutputToDebug(char* m_debug)
{
    
    // WINDOWS SPECIFIC WINDOW HANDLER
    
    //    OutputDebugString(debug);
    //    OutputDebugString("\n");
}
