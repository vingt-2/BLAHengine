#pragma once
#include "./GameAlgebra/Ray.h"
#include "./GameSingleton.h"
#include "./Scene.h"
#include "..\..\Common\StdInclude.h"

class BLACORE_API CursorPicker
{
public:

	const GameSingleton* m_gameSingleton;
	
	CursorPicker(const GameSingleton* singleton);

	Ray ScreenToRay(float length);
	GameChar* PickGameCharInScene(Scene &pickedScene, Ray &ray, vec3 &hitInWorld);
};