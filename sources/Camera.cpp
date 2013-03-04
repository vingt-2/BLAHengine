#include "Camera.h"


Camera::Camera():
	projection(mat4(1)),
	view(mat4(1))
{
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

void Camera::UpdateView()
{
	this->view = transform.modelTransform;
}
