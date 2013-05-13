#pragma once
#include "GameObject.h"
class Camera : public GameObject
{
public:
	// Members
	RigidBody* rigidBody;
	mat4 projection;
	Transform viewTransform;

	// States
	bool isControlEnabled;



	// Functions
	void SetProjection(mat4 projection);
	void UpdateView();

	void Update();

	Camera();
	~Camera(void);

private:
	// Private members
	vec2* previousMouseInput;

	// Private Functions
	void SimpleControls();
};

