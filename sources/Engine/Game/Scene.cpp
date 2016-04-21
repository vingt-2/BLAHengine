#include "Scene.h"

Scene::Scene(Camera* camera)
{
	this->m_enableSimulation = false;
	this->m_camera = camera;
	this->m_rigidBodySystem = new RigidBodySystem();
	this->m_directionalLights = vector<DirectionalLight*>();
	this->m_sceneObjectsVector = vector<GameObject*>();
	//m_rigidBodySystem->RegisterRigidBody(*(camera->m_rigidBody));
}

Scene::~Scene()
{

}

void Scene::AddObject(GameChar* objectPtr)
{
	m_sceneObjectsVector.push_back(objectPtr);

	if (objectPtr->m_rigidBody->m_collider != NULL)
		m_rigidBodySystem->RegisterRigidBody(*(objectPtr->m_rigidBody));
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
	m_camera->m_rigidBody->Update();

	for(int i=0;i < m_sceneObjectsVector.size() ; i++)
	{
		GameChar* gameChar = dynamic_cast<GameChar*>(m_sceneObjectsVector.at(i));
		if(gameChar != 0)
		{
			if(!m_directionalLights.empty())
			{
				m_directionalLights.at(0)->Update();
				//gameChar->m_meshRenderer->m_directionalLight = m_directionalLights.at(0)->GetDirection();
			}
		}
		m_sceneObjectsVector.at(i)->Update();
	}
}

vector<Contact>* Scene::GetContacts()
{
	return &(this->m_rigidBodySystem->m_collisionProcessor->m_currentContacts);
}