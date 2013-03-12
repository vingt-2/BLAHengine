#include "Camera.h"


Camera::Camera():
	projection(mat4(1))
{
	view = &(transform.modelTransform);
}


Camera::~Camera(void)
{
}

void Camera::SetProjection(mat4 projection)
{
	this->projection = projection;
}

void Camera::SetView(mat4 view)
{
	transform.modelTransform = view;
}