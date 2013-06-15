#pragma once
#include "GameChar.h"
class Camera : public GameChar
{
public:
	// Members
	Transform	viewTransform;
	mat4		projection;

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

