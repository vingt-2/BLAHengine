#include "Scene.h"
using namespace BLAengine;


Scene::Scene()
{
	this->m_enableSimulation = false;
	this->m_rigidBodySystem = new RigidBodySystem(new Time(200));
	this->m_directionalLights = vector<DirectionalLight*>();
	this->m_sceneObjectsVector = vector<GameObject*>();
	this->m_camera = nullptr;
}

Scene::~Scene()
{
	// TODO: DO SOME STUFF. YO
}

//void Scene::AddObject(GameObject* objectPtr)
//{
//	m_sceneObjectsVector.push_back(objectPtr);
//
//	if (RigidBody* rgbdy = objectPtr->GetComponent<RigidBody>())
//	{
//		if (Collider* collider = objectPtr->GetComponent<Collider>())
//		{
//			m_rigidBodySystem->RegisterRigidBody(*rgbdy);
//		}
//	}
//}

GameObject* Scene::CreateObject(std::string name)
{
	GameObject* newObject = new GameObject(name);

	m_sceneObjectsVector.push_back(newObject);

	return newObject;
}

GameObject* Scene::FindNameInScene(std::string name)
{
	for (int i = 0; i < m_sceneObjectsVector.size(); i++)
	{
		GameObject* object = m_sceneObjectsVector[i];
		if (object->m_objectName.compare(name) == 0)
		{
			return object;
		}
	}
	return nullptr;
}

void BLAengine::Scene::SetTimeObject(Time * time)
{
	m_rigidBodySystem->SetTimeObject(time);
}

//void Scene::AddDirectionalLight(DirectionalLight* light)
//{
//	m_directionalLights.push_back(light);
//}

void Scene::Update()
{
	if (m_enableSimulation)
	{
		m_rigidBodySystem->EnableSimulation();
	}
	else
	{
		m_rigidBodySystem->DisableSimulation();
	}

	m_rigidBodySystem->UpdateSystem();
	//m_camera->m_rigidBody->Update();

	for(int i = 0;i < m_sceneObjectsVector.size() ; i++)
	{
		GameObject* object = m_sceneObjectsVector[i];

		object->Update();
	}

	if (!m_directionalLights.empty())
	{
		m_directionalLights.at(0)->Update();
	}
}

vector<Contact>* Scene::GetContacts()
{
	return &(this->m_rigidBodySystem->m_collisionProcessor->m_currentContacts);
}

Ray Scene::ScreenToRay(vec2 renderSize, vec2 cursor, float length)
{
	////TODO: FIX THAT SHEET
	//vec2 cursor(0, 0); // Was get cursor on renderer, but that's long gone, renderer should *not* and doesnt know about this anymore... So ... How to handle this ?
	//float x = cursor.x;
	//float y = cursor.y;

	//vec3 rayDirection = vec3(1.f);
	//rayDirection.x = (((2.0f * (renderSize.x - x)) / renderSize.x) - 1) / m_gameSingleton->renderer->m_mainRenderCamera.m_perspectiveProjection[0][0];

	//rayDirection.y = -(((2.0f * (renderSize.y - (y))) / renderSize.y) - 1) / m_gameSingleton->renderer->m_mainRenderCamera.m_perspectiveProjection[1][1];

	//mat4 inverseView = inverse(m_gameSingleton->renderer->m_mainRenderCamera.m_attachedCamera->m_viewTransform.m_transformMatrix);

	//vec4 direction = (inverseView * vec4(rayDirection, 0));
	//rayDirection = normalize(vec3(direction.x, direction.y, direction.z));

	//vec3 rayOrigin = vec3(inverseView[3][0], inverseView[3][1], inverseView[3][2]);

	//return Ray(rayOrigin, -1.f * rayDirection, length);
	return Ray(vec3(0), vec3(0), 0);
}

GameObject* Scene::PickGameObjectInScene(Ray ray, vec3 &hitInWorld)
{
	vector<GameObject*> objects = m_sceneObjectsVector;

	float minDistance = INFINITY;
	GameObject* pickedObject = nullptr;
	for (auto obj : objects)
	{
		Collider* collider = obj->GetComponent<Collider>();
		Transform transform = obj->GetTransform();

		collider->m_collisionMesh->setTransform(&(transform.m_transformMatrix[0][0]));

		bool closestPoint = &hitInWorld != 0;

		bool collision = collider->m_collisionMesh->rayCollision(&(ray.m_origin.x), &(ray.m_direction.x), closestPoint, 0, ray.m_length);

		if (!collision) continue;

		vec3 collisionPoint;
		collider->m_collisionMesh->getCollisionPoint(&(collisionPoint.x), false);

		float distance = dot(collisionPoint - ray.m_origin, ray.m_direction);

		if (distance > 0 && distance < minDistance)
		{
			minDistance = distance;
			pickedObject = obj;
		}
		if (&hitInWorld != nullptr)
		{
			hitInWorld = collisionPoint;
		}
	}

	return pickedObject;
}