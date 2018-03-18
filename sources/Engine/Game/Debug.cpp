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

void Debug::DrawLine(const vec3 origin,const vec3 destination)
{
    m_lineMeshVertsAndColor.first.push_back(origin); 
    m_lineMeshVertsAndColor.first.push_back(destination);
    m_lineMeshVertsAndColor.second.push_back(vec3(0));
    m_lineMeshVertsAndColor.second.push_back(vec3(0));
}

void Debug::DrawLine(const vec3 origin,const vec3 destination,const vec3 color)
{
    m_lineMeshVertsAndColor.first.push_back(origin);
    m_lineMeshVertsAndColor.first.push_back(destination);
    m_lineMeshVertsAndColor.second.push_back(color);
    m_lineMeshVertsAndColor.second.push_back(color);
}

void Debug::DrawRay(Ray ray,const vec3 color)
{
    vec3 destination = ray.m_origin + ray.m_length* ray.m_direction;

    DrawLine(ray.m_origin, destination, color);
}

void Debug::DrawRay(Ray ray)
{
    vec3 destination = ray.m_origin + ray.m_length* ray.m_direction;

    DrawLine(ray.m_origin, destination, vec3(1));
}

void Debug::DrawGrid(int size, float spacing, const vec3 color)
{
    for (int i=-size/2; i<=size/2; i++)
    {
        float iSpacing = i*spacing;
        float sizeSpacing = size*spacing;
        DrawLine(vec3(sizeSpacing /2, 0, iSpacing),vec3(-sizeSpacing /2, 0, iSpacing), color);
        DrawLine(vec3(iSpacing, 0, sizeSpacing /2),vec3(iSpacing, 0, -sizeSpacing /2), color);
    }
}

void Debug::DrawBasis(Transform* transform, float opacity)
{
    DrawRay(Ray(transform->LocalPositionToWorld(vec3(0, 0, 0)), normalize((transform->LocalDirectionToWorld(vec3(1, 0, 0)))), 1), vec3(1, 0, 0));
    DrawRay(Ray(transform->LocalPositionToWorld(vec3(0, 0, 0)), normalize((transform->LocalDirectionToWorld(vec3(0, 1, 0)))), 1), vec3(0, 1, 0));
    DrawRay(Ray(transform->LocalPositionToWorld(vec3(0, 0, 0)), normalize((transform->LocalDirectionToWorld(vec3(0, 0, 1)))), 1), vec3(0, 0, 1));
}

void Debug::OutputToDebug(char* m_debug)
{
    
    // WINDOWS SPECIFIC WINDOW HANDLER
    
    //    OutputDebugString(debug);
    //    OutputDebugString("\n");
}
