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
	this->view = view;
}

void Camera::UpdateView()
{
	mat4 currentTransform = this->transform.GetTransform();
	this->view = view * currentTransform;
}
