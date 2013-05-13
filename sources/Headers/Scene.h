#pragma once
#include "std.h"
#include "GameObject.h"
#include "GameManager.h"
#include "GameChar.h"

class Scene
{
public:

	void AddObject(GameChar* object);
	void AddObject(GameManager* object);
	int CountMeshs();


	Scene();
	~Scene();

private:

	vector<GameObject*>  sceneObjectsVector;

};
