#pragma once
#include "../../Std/std.h"
#include "RigidBodySystem.h"
#include "../Game/GameObjects/GameManager.h"
#include "../Game/GameObjects/Camera.h"
#include "../Game/GameObjects/DirectionalLight.h"
#include "../Game/GameObjects/PointLight.h"

class Scene
{
public:
	Scene(Camera* camera);
	~Scene();

	void Update();

	void AddObject(GameChar* object);
	void AddObject(GameManager* object);

	void AddDirectionalLight(DirectionalLight* directionalLight);

	vector<GameObject*> GetObjects() { return m_sceneObjectsVector;  }
	vector<Contact>* GetContacts();

	int CountChar();

	bool m_enableSimulation;

	void EnableGravity() { m_rigidBodySystem->m_enableGravity = true; }
	void DisableGravity() { m_rigidBodySystem->m_enableGravity = false; }
	bool GetGravity() { return m_rigidBodySystem->m_enableGravity; }
	RigidBodySystem* m_rigidBodySystem;


private:

	Camera* m_camera;	vector<GameObject*>  m_sceneObjectsVector;
	vector<DirectionalLight*> m_directionalLights;
	vector<PointLight*> m_pointLights;
};
