#include "../Engine/Game/GameObjects/PointLight.h"
#include "../Engine/Game/GameObjects/DirectionalLight.h"
#include "../Engine/Game/GameObjects/Camera.h"

#include "GameComponentSerializer.h"

#include <cereal\cereal.hpp>
#include <cereal\types\polymorphic.hpp>
#include <cereal\types\vector.hpp>
#include <cereal\types\string.hpp>
#include <cereal\archives\xml.hpp>

namespace BLAengine
{
	class GameObjectSerializer
	{
	public:

		GameObjectSerializer() = default;

		void FromGameObject(GameObject* gobject);
		void ToGameObject(GameObject* gobject);

	private:
		friend class cereal::access;

		string m_objectName;
		TransformSerializer m_transform;


		template <class Archive>
		void serialize(Archive & archive)
		{
			archive
			(
				cereal::make_nvp("name", m_objectName),
				cereal::make_nvp("transform", m_transform)
			)
		}
	};

	class GameChar
	{

	};
}