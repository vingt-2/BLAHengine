#include "SceneManager.h"

using namespace BLAengine;

bool SceneManager::SaveScene(string filepath, Scene* scene)
{
	SceneSerializer sceneSerializer;

	sceneSerializer.FromScene(scene);

	cout << "Scene obj vector " << scene->GetObjects().size() << "\n";

	std::ofstream fs;
	fs.open(string(filepath));

	if (!fs.is_open())
	{
		cout << "Could not Write on file " << "Test_scene " << "\n";
	}

	cereal::JSONOutputArchive output(fs);

	output(cereal::make_nvp("Scene", sceneSerializer));

	return true;
}

Scene* SceneManager::LoadScene(std::string filepath)
{
	SceneSerializer sceneSerializer;

	std::ifstream fs;
	fs.open(string(filepath));

	cereal::JSONInputArchive input(fs);

	input(sceneSerializer);

	Scene* scene = sceneSerializer.BuildScene();

	return scene;
}