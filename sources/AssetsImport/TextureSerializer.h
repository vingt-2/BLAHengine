#include <fstream>
#include "../Engine/Assets/Texture.h"
#include <cereal\cereal.hpp>
#include <cereal\types\vector.hpp>
#include <cereal\types\string.hpp>
#include <cereal\archives\binary.hpp>

namespace BLAengine
{
	class Texture2DSerializer
	{
	public:
		
		Texture2DSerializer() = default;

		void FromTexture(Texture2D* texture) { m_texture = *texture; }
		void ToTexture(Texture2D* texture) { *texture = m_texture; }

	private:
		friend class cereal::access;

		Texture2D m_texture;

		template <class Archive>
		void serialize(Archive & archive)
		{
			archive
			(
				m_texture
			)
		}
	};
}