#include "Graphics.h"
#include "std.h"
#include "Vector3f.h"

class MeshRenderer
{
public:
	vector<vec3> meshVertices;
	vector<vec3> meshNormals;
	vector<vec2> meshUVs;

	MeshRenderer(void);
	~MeshRenderer(void);

	string ToString(void);
};

