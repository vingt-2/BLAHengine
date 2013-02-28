#include "MeshRenderer.h"
#include "Transform.h"
class GameObject
{
public:
MeshRenderer* meshRenderer;
Transform transform;

void SetPosition(vec3 position);


GameObject(void);
~GameObject(void);
};