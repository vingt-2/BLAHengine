#pragma once
#include "std.h"
#include "Graphics.h"

#define FOURCC_DXT1 0x31545844 // Equivalent to "DXT1" in ASCII
#define FOURCC_DXT3 0x33545844 // Equivalent to "DXT3" in ASCII
#define FOURCC_DXT5 0x35545844 // Equivalent to "DXT5" in ASCII

class SharedRessources
{
public:

	SharedRessources(void);
	~SharedRessources(void);

	bool LoadMaterial(const char * name, const char * vertexShaderPath,const char * fragmentShaderPath);
	bool loadBMP_custom(const char * name, const char * imagepath);
	bool loadTGA_glfw(const char * name, const char * imagepath);
	bool loadDDS(const char * name, const char * imagepath);

	GLuint GetMaterial(const char * name);
	GLuint GetTexture(const char * name);


private:

	map<const char*, int> ressourceTable;

};

