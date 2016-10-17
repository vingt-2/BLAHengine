#include "../../AssetsImport/SceneSerializer.h"

namespace BLAengine
{
	class BLACORE_API SceneManager
	{
	public:
		bool SaveScene(std::string filepath, Scene* scene);

		Scene* LoadScene(std::string filepath);
	};
}