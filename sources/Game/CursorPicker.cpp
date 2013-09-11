#include "CursorPicker.h"
#include <iostream>

CursorPicker::CursorPicker(const GameSingleton* singleton)
{
    this->gameSingleton = singleton;
}

Ray CursorPicker::ScreenToRay(float length)
{
    vec2 renderSize = gameSingleton->renderer->GetRenderSize();
    
    double x,y ;
    
    glfwGetCursorPos(gameSingleton->renderer->GetWindow(), &x, &y);
    
    
    vec3 rayDirection = vec3(1.f);
    rayDirection.x = ( ( ( 2.0f * (renderSize.x - x)) / renderSize.x ) -1 ) / gameSingleton->renderer->mainCamera->m_projection[0][0];
    
    // OMG HACK, adjust from 75 pixel down the actual clicking, INVESTIFUCKINGATE;
    // REALLY FUCKING FIX IT ALREADY
    // !!!
	// ON MAC, Y NEEDS A COMPENSATION OF AOUNRD 75PX on Y, DONT KNOW WHY
    //rayDirection.y = - ( ( ( 2.0f * (renderSize.y - (y+75))) / renderSize.y ) -1 ) / gameSingleton->renderer->mainCamera->m_projection[1][1];
    
    rayDirection.y = - ( ( ( 2.0f * (renderSize.y - (y))) / renderSize.y ) -1 ) / gameSingleton->renderer->mainCamera->m_projection[1][1];


    mat4 inverseView = inverse(gameSingleton->renderer->mainCamera->m_viewTransform.transformMatrix);
    
    vec4 direction = (inverseView * vec4(rayDirection,0));
    rayDirection = vec3(direction.x,direction.y,direction.z);
    
    vec3 rayOrigin = vec3(inverseView[3][0],inverseView[3][1],inverseView[3][2]);
    
    
    return Ray(rayOrigin, rayDirection, -length);
}

GameChar* PickGameCharInScene(const Scene* pickedScene,const Ray* ray)
{
    return 0;
}