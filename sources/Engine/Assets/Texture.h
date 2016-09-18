#pragma once
#include "../../Common/StdInclude.h"
#include "../../Common/System.h"

class BLACORE_API Texture2D
{
public:
	
	//TODO: Support mipmapping 
	
	Texture2D(void);
	Texture2D(string name, uint8_t dim, uint8_t* data, uint32_t width, uint32_t height);
	~Texture2D(void);

	uint8_t m_nComponents;
	uint32_t m_dataSize;
	uint8_t * m_data;
	uint32_t m_width, m_height;

	string m_name;

private:
};

class BLACORE_API TextureLoader
{
public:

	static Texture2D* LoadBMP(string name, string filePath);
	static Texture2D* LoadDDS(string name, string filePath);
};

