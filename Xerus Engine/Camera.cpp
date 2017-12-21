#include "stdafx.h"
#include "Camera.h"


xr::Camera::Camera(glm::mat4 projection, glm::vec3 position, glm::vec3 direction, glm::vec3 up) :
	projection(projection),
	position(position),
	direction(glm::normalize(direction)),
	up(up),
	view(glm::lookAt(position, position + direction, up))
{
}

void xr::Camera::setPosition(glm::vec3 position)
{
	this->position = position;
	updateView();
}

void xr::Camera::setDirection(glm::vec3 direction)
{
	this->direction = glm::normalize(direction);
	updateView();
}

glm::mat4 xr::Camera::getTransform() const
{
	return projection * view;
}

glm::vec3 xr::Camera::screenToWorld(glm::vec2 screen)
{
	glm::vec4 screenExt = {screen, -1, 1};

	glm::mat4 inverse = glm::inverse(projection * view);

	glm::vec4 projection = inverse * screenExt;
	glm::vec3 result = glm::vec3(projection) / projection.w;

	return result;
}

void xr::Camera::updateView()
{
	view = glm::lookAt(position, position + direction, up);
}

xr::OrthographicCamera::OrthographicCamera(float width, float height) :
	OrthographicCamera(0, width, 0, height)
{
}

xr::OrthographicCamera::OrthographicCamera(float left, float right, float top, float bottom) :
	Camera(glm::ortho(left, right, bottom, top), { 0, 0, 0 }, { 0, 0, -1 }, {0, 1, 0})
{
}

void xr::OrthographicCamera::setProjection(float left, float right, float top, float bottom)
{
	this->projection = glm::ortho(left, right, bottom, top);
}

