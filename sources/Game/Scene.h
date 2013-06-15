#pragma once
#include "../Std/std.h"
#include "../Game/GameObjects/GameManager.h"
#include "../Game/GameObjects/GameChar.h"

class Scene
{
public:
	Scene();
	~Scene();

	void Update();

	void AddObject(GameChar* object);
	void AddObject(GameManager* object);
	int CountMeshs();


private:

	vector<GameObject*>  sceneObjectsVector;

};
