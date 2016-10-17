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

void Scene::AddObject(GameObject* objectPtr)
{
	m_sceneObjectsVector.push_back(objectPtr);

	if (GameChar* gameCharPtr = dynamic_cast<GameChar*>(objectPtr))
	{
		if (gameCharPtr->m_rigidBody->m_collider != nullptr)
			m_rigidBodySystem->RegisterRigidBody(*(gameCharPtr->m_rigidBody));
	}

}
void Scene::AddObject(GameManager* managerPtr)
{
	m_sceneObjectsVector.push_back(managerPtr);
}

int Scene::CountChar()
{
	int count = 0;

	for(int i=0; i < m_sceneObjectsVector.size() ; i++)
	{
		GameObject* objectPointer = m_sceneObjectsVector.at(i);
		GameChar* type = dynamic_cast<GameChar*>(objectPointer);
		if(type != 0)
		{
			count ++;
		}
	}
	return count;
}

void BLAengine::Scene::SetTimeObject(Time * time)
{
	m_rigidBodySystem->SetTimeObject(time);
}

void Scene::AddDirectionalLight(DirectionalLight* light)
{
	m_directionalLights.push_back(light);
}

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

	for(int i=0;i < m_sceneObjectsVector.size() ; i++)
	{
		GameChar* gameChar = dynamic_cast<GameChar*>(m_sceneObjectsVector.at(i));
		if(gameChar != 0)
		{
			if(!m_directionalLights.empty())
			{
				m_directionalLights.at(0)->Update();
			}
		}
		m_sceneObjectsVector.at(i)->Update();
	}
}

vector<Contact>* Scene::GetContacts()
{
	return &(this->m_rigidBodySystem->m_collisionProcessor->m_currentContacts);
}