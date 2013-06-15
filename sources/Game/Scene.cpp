#include "Scene.h"

Scene::Scene()
{
	sceneObjectsVector = vector<GameObject*>();
}

Scene::~Scene()
{

}

void Scene::AddObject(GameChar* objectPtr)
{
	sceneObjectsVector.push_back(objectPtr);
}
void Scene::AddObject(GameManager* managerPtr)
{
	sceneObjectsVector.push_back(managerPtr);
}

int Scene::CountMeshs()
{
	int count = 0;

	for(int i=0; i < sceneObjectsVector.size() ; i++)
	{
		GameObject* objectPointer = sceneObjectsVector.at(i);
		GameChar* type = dynamic_cast<GameChar*>(objectPointer);
		if(type != 0)
		{
			count ++;
		}
	}
	return count;
}

void Scene::Update()
{
	for(int i=0;i < sceneObjectsVector.size() ; i++)
	{
		sceneObjectsVector.at(i)->Update();
	}
}