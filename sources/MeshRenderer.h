#pragma once

#include "Graphics.h"
#include "std.h"
#include "Vector3f.h"
#define FOURCC_DXT1 0x31545844 // Equivalent to "DXT1" in ASCII
#define FOURCC_DXT3 0x33545844 // Equivalent to "DXT3" in ASCII
#define FOURCC_DXT5 0x35545844 // Equivalent to "DXT5" in ASCII

class MeshRenderer
{
public:
	vector<vec3> meshVertices;
	vector<vec3> meshNormals;
	vector<vec2> meshUVs;

	mat4* modelTransform;

	GLuint vertexArrayID;
	GLuint vertexbuffer;
	GLuint uvbuffer;
	GLuint textureID;
	GLuint shaderID;
	GLuint matrixID;

	MeshRenderer(mat4* modelTransform);
	~MeshRenderer(void);

	bool Draw(mat4 projection, mat4 view);
	bool GenerateArrays();
	bool CleanUp();

	GLuint LoadShaders(const char * vertex_file_path,const char * fragment_file_path);
	GLuint loadBMP_custom(const char * imagepath);
	GLuint loadTGA_glfw(const char * imagepath);
	GLuint loadDDS(const char * imagepath);


	string ToString(void);
};

