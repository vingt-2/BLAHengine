#pragma once
#include "MeshRenderer.h"
#include "Transform.h"
class GameObject
{
public:
MeshRenderer* meshRenderer;
Transform transform;

void Update();


GameObject(void);
~GameObject(void);
};