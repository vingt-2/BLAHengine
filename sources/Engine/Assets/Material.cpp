#include "Material.h"
using namespace BLAengine;

bool Material::AssignTexture(std::string textureName, std::string handleName)
{
	string texture = string(textureName);
	string handle = string(handleName);
	pair<string, string> entry = pair<string, string>(texture, handle);
	m_textureSamplerAttributes.push_back(entry);

	return true;
}

Material* BLAengine::MaterialImport::LoadShaderCode(string name, string fragmentCodeFile, string vertexCodeFile)
{
	// Read the Vertex Shader code from the file
	std::string VertexShaderCode;
	std::ifstream VertexShaderStream(vertexCodeFile, std::ios::in);
	if (VertexShaderStream.is_open())
	{
		std::string Line = "";
		while (getline(VertexShaderStream, Line))
			VertexShaderCode += "\n" + Line;
		VertexShaderStream.close();
	}
	else 
	{
		cout << vertexCodeFile << "could not be opened.\n";
		return nullptr;
	}

	// Read the Fragment Shader code from the file
	std::string FragmentShaderCode;
	std::ifstream FragmentShaderStream(fragmentCodeFile, std::ios::in);
	if (FragmentShaderStream.is_open())
	{

		std::string Line = "";
		while (getline(FragmentShaderStream, Line))
			FragmentShaderCode += "\n" + Line;
		FragmentShaderStream.close();
	}
	else
	{
		cout << fragmentCodeFile << "could not be opened.\n";
		return nullptr;
	}

	Material* mat = new Material(name);
	mat->m_vertexShader = VertexShaderCode;
	mat->m_fragmentShader = FragmentShaderCode;

	return mat;
}
