#pragma once
#include "MeshRenderer.h"
#include "RigidBody.h"
class GameObject
{
public:
MeshRenderer* meshRenderer;
Transform* transform;
RigidBody* rigidBody;

void Update();


GameObject(void);
~GameObject(void);
};