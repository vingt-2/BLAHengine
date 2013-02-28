#include "Graphics.h"
#include "std.h"
class Transform
{
public:
	vec3 position;
	vec3 rotation;
	vec3 scale;

	mat4 GetTransform();

	Transform(void);
	~Transform(void);
};

