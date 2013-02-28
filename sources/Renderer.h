#include "OBJImport.h"
#include "std.h"
class Renderer
{
public:

	mat4 projectionMatrix;
	mat4 viewMatrix;
	vector<MeshRenderer*> renderVector;

	void Resize(int xRes,int yRes);
	bool InitializeContext(char* windowTitle);
	bool Update();

	Renderer(void);
	~Renderer(void);
};

