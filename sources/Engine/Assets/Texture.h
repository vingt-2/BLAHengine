#pragma once
#include "Asset.h"
#include "../../Common/StdInclude.h"
#include "../../Common/System.h"
#include "../../Common/Maths.h"

namespace BLAengine
{


	class BLACORE_API Texture2D : public Asset
	{
	public:

		//TODO: Support mipmapping 
		Texture2D(string name, uint8_t dim, vector<uint8_t> data, uint32_t width, uint32_t height);
		~Texture2D(void);

		uint8_t m_nComponents;
		uint32_t m_dataSize;
		uint32_t m_width, m_height;

		std::vector<uint8_t> m_data;

	private:
	};

	class BLACORE_API TextureImport
	{
	public:

		static Texture2D* LoadBMP(string name, string filePath);
		static Texture2D* LoadDDS(string name, string filePath);
	};


}