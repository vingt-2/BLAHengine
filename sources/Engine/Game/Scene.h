#pragma once
#include "./RigidBodySystem.h"
#include "../../Common/StdInclude.h"
#include "../Game/GameObjects/GameManager.h"
#include "../Game/GameObjects/Camera.h"
#include "../Game/GameObjects/DirectionalLight.h"
#include "../Game/GameObjects/PointLight.h"

namespace BLAengine
{
	class BLACORE_API Scene
	{
	public:
		Scene();
		~Scene(); // TODO: IMPLEMENT!

		void Initialize(); // TODO: IMPLEMENT! Find camera object and so on
		void Update();

		void AddObject(GameObject* object);
		void AddObject(GameManager* object);

		void AddDirectionalLight(DirectionalLight* directionalLight);

		vector<GameObject*> GetObjects() { return m_sceneObjectsVector; }
		vector<Contact>* GetContacts();

		int CountChar();

		bool m_enableSimulation;

		void EnableGravity() { m_rigidBodySystem->m_enableGravity = true; }
		void DisableGravity() { m_rigidBodySystem->m_enableGravity = false; }
		bool GetGravity() { return m_rigidBodySystem->m_enableGravity; }
		RigidBodySystem* m_rigidBodySystem;

		void SetTimeObject(Time* time); //TODO: Implement !

	private:

		Camera* m_camera; //TODO: Find in object scene !
		vector<GameObject*>  m_sceneObjectsVector;
		vector<DirectionalLight*> m_directionalLights;
		vector<PointLight*> m_pointLights;
	};

}