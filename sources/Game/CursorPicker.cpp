#include "CursorPicker.h"
#include <iostream>

CursorPicker::CursorPicker(const GameSingleton* singleton)
{
    this->m_gameSingleton = singleton;
}

Ray CursorPicker::ScreenToRay(float length)
{
    vec2 renderSize = m_gameSingleton->renderer->GetRenderSize();
    
    double x,y ;
    
    glfwGetCursorPos(m_gameSingleton->renderer->GetWindow(), &x, &y);
    
    
    vec3 rayDirection = vec3(1.f);
    rayDirection.x = ( ( ( 2.0f * (renderSize.x - x)) / renderSize.x ) -1 ) / m_gameSingleton->renderer->m_mainCamera->m_projection[0][0];
    
    rayDirection.y = - ( ( ( 2.0f * (renderSize.y - (y))) / renderSize.y ) -1 ) / m_gameSingleton->renderer->m_mainCamera->m_projection[1][1];

	mat4 inverseView = inverse(m_gameSingleton->renderer->m_mainCamera->m_viewTransform.transformMatrix);
    
    vec4 direction = (inverseView * vec4(rayDirection,0));
    rayDirection = vec3(direction.x,direction.y,direction.z);
    
	vec3 rayOrigin = vec3(inverseView[3][0], inverseView[3][1], inverseView[3][2]);
    
    
    return Ray(rayOrigin, rayDirection, -length);
}

GameChar* PickGameCharInScene(const Scene* pickedScene,const Ray* ray)
{
    return 0;
}