#pragma once
#include "./GameAlgebra/Ray.h"
#include "./GameSingleton.h"
#include "./Scene.h"

class CursorPicker
{
public:

    const GameSingleton* gameSingleton;
    
    CursorPicker(const GameSingleton* singleton);
    
    Ray ScreenToRay(float length);
    
    GameChar* PickGameCharInScene(const Scene* pickedScene,const Ray* ray);

};