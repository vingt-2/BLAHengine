#pragma once
#include "../../Common/StdInclude.h"
#include "../../Common/System.h"
#include "../../Common/Maths.h"
#include "Texture.h"

namespace BLAengine
{
	class BLACORE_API Material : public Asset
	{
	public:

		Material(std::string name) : Asset(name)
		{};

		bool AssignTexture(std::string textureName, std::string handleName);

		std::string m_vertexShader;
		std::string m_fragmentShader;

		std::vector<pair<std::string, std::string>> m_textureSamplerAttributes;
	};

	class BLACORE_API MaterialImport
	{
	public:
		Material* LoadShaderCode(string name, string frag, string vert);
	};
}