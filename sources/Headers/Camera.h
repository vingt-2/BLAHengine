#pragma once
#include "GameObject.h"
class Camera : public GameObject
{
public:
	RigidBody* rigidBody;

	mat4 projection;
	Transform viewTransform;
	
	void SetProjection(mat4 projection);
	void UpdateView();

	void Update();

	Camera();
	~Camera(void);
};

