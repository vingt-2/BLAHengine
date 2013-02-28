#include "OBJImport.h"
#include "std.h"
class Renderer
{
public:

	mat4 projectionMatrix;
	mat4 viewMatrix;
	vector<MeshRenderer*> objects;

	bool initializeContext(char* windowTitle);
	bool Update();

	Renderer(void);
	~Renderer(void);
};

