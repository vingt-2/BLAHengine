#pragma once
#include "../Std/std.h"
#include "../Game/GameObjects/GameManager.h"
#include "../Game/GameObjects/GameChar.h"
#include "../Game/GameObjects/DirectionalLight.h"

class Scene
{
public:
	Scene();
	~Scene();

	void Update();

	void AddObject(GameChar* object);
	void AddObject(GameManager* object);

	void AddDirectionalLight(DirectionalLight* directionalLight);

	int CountMeshs();


private:

	vector<GameObject*>  m_sceneObjectsVector;

	vector<DirectionalLight*> m_directionalLights;

};
