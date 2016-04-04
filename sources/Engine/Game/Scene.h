#pragma once
#include "../../Std/std.h"
#include "../Game/GameObjects/GameManager.h"
#include "../Game/GameObjects/GameChar.h"
#include "../Game/GameObjects/DirectionalLight.h"
#include "../Game/GameObjects/PointLight.h"
#include "CollisionProcessor.h"

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
	CollisionProcessor m_collisionProcessor;
	vector<DirectionalLight*> m_directionalLights;
	vector<PointLight*> m_pointLights;
};
