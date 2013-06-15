#include "Scene.h"

Scene::Scene()
{
	m_directionalLights = vector<DirectionalLight*>();
	m_sceneObjectsVector = vector<GameObject*>();
}

Scene::~Scene()
{

}

void Scene::AddObject(GameChar* objectPtr)
{
	m_sceneObjectsVector.push_back(objectPtr);
}
void Scene::AddObject(GameManager* managerPtr)
{
	m_sceneObjectsVector.push_back(managerPtr);
}

int Scene::CountMeshs()
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
	
	for(int i=0;i < m_sceneObjectsVector.size() ; i++)
	{
		GameChar* gameChar = dynamic_cast<GameChar*>(m_sceneObjectsVector.at(i));
		if(gameChar != 0)
		{
			if(!m_directionalLights.empty())
			{
				m_directionalLights.at(0)->Update();
				gameChar->meshRenderer->m_directionalLight = m_directionalLights.at(0)->GetDirection();
			}
		}
		m_sceneObjectsVector.at(i)->Update();
	}
}