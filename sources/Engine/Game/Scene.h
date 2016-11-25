#pragma once
#include "./RigidBodySystem.h"
#include "../../Common/StdInclude.h"
#include "./GameComponents/RigidBody.h"
#include "./GameComponents/Camera.h"
#include "./GameComponents/Collider.h"
#include "./GameComponents/DirectionalLight.h"
#include "./GameComponents/MeshRenderer.h"
#include "./GameComponents/PointLight.h"
#include "GameObject.h"

namespace BLAengine
{
	class BLACORE_API Scene
	{
	public:
		Scene();
		~Scene(); // TODO: IMPLEMENT!

		void Initialize(); // TODO: IMPLEMENT! Find camera object and so on
		void Update();

		GameObject* CreateObject(std::string name);

		GameObject* FindNameInScene(std::string name);

		vector<GameObject*> GetObjects() { return m_sceneObjectsVector; }
		vector<Contact>* GetContacts();

		bool m_enableSimulation;

		void EnableGravity() { m_rigidBodySystem->m_enableGravity = true; }
		void DisableGravity() { m_rigidBodySystem->m_enableGravity = false; }
		bool GetGravity() { return m_rigidBodySystem->m_enableGravity; }
		RigidBodySystem* m_rigidBodySystem;

		void SetTimeObject(Time* time); //TODO: Implement !

		Ray ScreenToRay(vec2 renderSize, vec2 cursor, float length);
		GameObject* PickGameObjectInScene(Ray ray, vec3 &hitInWorld);

	private:

		Camera* m_camera; //TODO: Find in object scene !
		vector<GameObject*>  m_sceneObjectsVector;
		vector<DirectionalLight*> m_directionalLights;
		vector<PointLight*> m_pointLights;
	};

}