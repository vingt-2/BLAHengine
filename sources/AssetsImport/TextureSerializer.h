#pragma once

#include "../Engine/Assets/Texture.h"
#include <cereal\cereal.hpp>
#include <cereal\types\vector.hpp>
#include <cereal\types\string.hpp>
#include <cereal\archives\binary.hpp>

class Texture2DSerializer
{
public:
		
	Texture2DSerializer() = default;

	void FromTexture(BLAengine::Texture2D* texture) { m_texture = *texture; }
	void ToTexture(BLAengine::Texture2D* texture) { *texture = m_texture; }

private:
	friend class cereal::access;

	BLAengine::Texture2D m_texture;

	template <class Archive>
	void serialize(Archive & archive)
	{
		archive
		(
			m_texture
		)
	}
};
