#pragma once
#include "GameChar.h"
class Camera : public GameChar
{
public:
	// Members
	Transform	m_viewTransform;
	mat4		m_projection;

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
	vec2* m_previousMouseInput;

	// Private Functions
	void SimpleControls();
};

