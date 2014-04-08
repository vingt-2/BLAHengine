#pragma once
#include "../Std/std.h"
#include "../Std/Graphics.h"
//#include <fileapi.h>

#define FOURCC_DXT1 0x31545844 // "DXT1" in ASCII
#define FOURCC_DXT3 0x33545844 // "DXT3" in ASCII
#define FOURCC_DXT5 0x35545844 // "DXT5" in ASCII

class SharedResources
{
public:
	SharedResources(void);
	~SharedResources(void);

	GLuint LoadMaterial(const char * name, const char * vertexShaderPath,const char * fragmentShaderPath);
	bool loadBMP_custom(const char * name, const char * imagepath);
	bool loadTGA_glfw(const char * name, const char * imagepath);
	bool loadDDS(const char * name, const char * imagepath);

	GLuint GetMaterial(const char * name);
	GLuint GetTexture(const char * name);

	map<std::string,GLuint> GetTable()
	{
		return ressourceTable;
	}

private:

	map<std::string,GLuint> ressourceTable;

};

