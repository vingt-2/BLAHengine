#include "../Engine/Game/GameComponents/Transform.h"
#include "../Engine/Game/GameComponents/MeshRenderer.h"
#include "../Engine/Game/GameComponents/RigidBody.h"

#include <cereal\cereal.hpp>
#include <cereal\types\vector.hpp>
#include <cereal\types\string.hpp>
#include <cereal\archives\xml.hpp>

namespace BLAengine
{
	class TransformSerializer
	{
	public:

		TransformSerializer() = default;

		void FromGameObject(Transform* gobject);
		void ToGameObject(Transform* gobject);

	private:
		friend class cereal::access;

		vec3serializer m_position;
		mat3serializer m_rotation;
		vec3serializer m_scale;

		template <class Archive>
		void serialize(Archive & archive)
		{
			archive
			(
				cereal::make_nvp("position", m_position),
				cereal::make_nvp("rotation", m_rotation),
				cereal::make_nvp("scale", m_scale)
			)
		}
	};

	class MeshRendererSerializer
	{
	public:

		MeshRendererSerializer() = default;

		void FromGameObject(MeshRenderer* gobject);
		void ToGameObject(MeshRenderer* gobject);

	private:
		friend class cereal::access;

		string m_triangleMesh;
		string m_material;

		template <class Archive>
		void serialize(Archive & archive)
		{
			archive
			(
				cereal::make_nvp("triangleMesh", m_triangleMesh),
				cereal::make_nvp("material", m_material)
			)
		}
	};
}