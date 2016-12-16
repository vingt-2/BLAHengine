#include "Scene.h"
#include "PBRendering\PBRRenderer.h"

using namespace BLAengine;


Scene::Scene()
{
	this->m_enableSimulation = false;
	this->m_rigidBodySystem = new RigidBodySystem(new Time(200));
	this->m_sceneObjectsVector = vector<GameObject*>();
	this->m_camera = nullptr;
	this->m_renderingManager = nullptr;
}

Scene::~Scene()
{
	// TODO: DO SOME STUFF. YO
}

GameObject* Scene::CreateObject(std::string name)
{
	GameObject* newObject = new GameObject(name);

	m_sceneObjectsVector.push_back(newObject);

	return newObject;
}

bool BLAengine::Scene::DeleteObject(std::string name)
{
	GameObject* object = this->FindNameInScene(name);
	if (!object)
		return false;

	for (auto meshRenderer : object->GetComponents<MeshRenderer>())
	{
		m_renderingManager->CancelMeshRendererTicket(meshRenderer);
	}
}

GameObject* Scene::FindNameInScene(std::string name)
{
	for (int i = 0; i < m_sceneObjectsVector.size(); i++)
	{
		GameObject* object = m_sceneObjectsVector[i];
		if (object->GetName().compare(name) == 0)
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


void BLAengine::Scene::Initialize(RenderingManager* renderingManager)
{
	this->m_renderingManager = renderingManager;
}

void Scene::Update()
{
	if (!m_renderingManager) 
	{
		std::cout << "Scene object not initialized !\n";
		return;
	}

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

	for(int i = 0; i < m_sceneObjectsVector.size(); i++)
	{
		GameObject* object = m_sceneObjectsVector[i];

		object->Update();

		for (auto meshRenderer : object->GetComponents<MeshRenderer>())
		{
			if (meshRenderer->m_renderTicket == 0)
			{
				m_renderingManager->RegisterMeshRenderer(meshRenderer);
			}
		}

		for (auto dirLightComp : object->GetComponents<DirectionalLight>())
		{
			Camera* shadowCamera = dirLightComp->m_parentObject->GetComponent<Camera>();
			if (dirLightComp->m_renderTicket == 0 && shadowCamera != nullptr)
			{
				m_renderingManager->RegisterDirectionalLight(dirLightComp, shadowCamera);
			}
		}

		for (auto pbrenderer : object->GetComponents<PBRCamera>())
		{
			if (!pbrenderer->m_shouldRender)
				continue;
			for (auto object : m_sceneObjectsVector)
			{
				if (PBRSurface* obj = object->GetComponent<PBRSurface>())
				{
					pbrenderer->AddObject(obj);
				}
			}
			pbrenderer->Render();
		}
	}
}

Camera* BLAengine::Scene::GetMainCamera()
{
	if (m_camera == nullptr)
	{
		for (auto object : m_sceneObjectsVector)
		{
			Camera* camera = object->GetComponent<Camera>();
			if (camera != nullptr)
				m_camera = camera;
				break;
		}
	}
	return m_camera;
}

vector<Contact>* Scene::GetContacts()
{
	return &(this->m_rigidBodySystem->m_collisionProcessor->m_currentContacts);
}

std::pair<GameObject*, Collider::RayCollision>  Scene::PickGameObjectInScene(Ray ray)
{
	vector<GameObject*> objects = m_sceneObjectsVector;

	float minDistance = INFINITY;
	GameObject* pickedObject = nullptr;
	Collider::RayCollision closestContact;
	for (auto obj : objects)
	{
		Collider* collider = obj->GetComponent<Collider>();

		if (collider == nullptr)
			continue;

		Collider::RayCollision contactPoint = collider->CollideWithRay(ray);

		if (!contactPoint.m_isValid)
			continue;

		float distance = dot(contactPoint.m_colPositionW - ray.m_origin, ray.m_direction);

		if (distance > 0 && distance < minDistance)
		{
			minDistance = distance;
			pickedObject = obj;
			closestContact = contactPoint;
		}
	}

	return std::pair<GameObject*, Collider::RayCollision>(pickedObject, closestContact);
}