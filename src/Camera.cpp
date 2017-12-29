#include "stdafx.h"
#include "Camera.h"


xr::Camera::Camera(glt::mat4f projection, glt::vec3f position, glt::vec3f direction, glt::vec3f up) :
	projection(projection),
	position(position),
	direction(glt::normalize(direction)),
	up(up),
	view(glt::lookAt(position, position + direction, up))
{
}

void xr::Camera::setPosition(glt::vec3f position)
{
	this->position = position;
	updateView();
}

glt::vec3f xr::Camera::getPosition()
{
	return this->position;
}

void xr::Camera::setDirection(glt::vec3f direction)
{
	this->direction = glt::normalize(direction);
	updateView();
}

glt::mat4f xr::Camera::getTransform() const
{
	return projection * view;
}

glt::vec3f xr::Camera::screenToWorld(glt::vec2f screen)
{
	glt::vec4f screenExt = {screen, -1, 1};

	glt::mat4f inverse = glt::inverse(projection * view);

	glt::vec4f projection = inverse * screenExt;
	glt::vec3f result = glt::vec3f(projection) / projection.w;

	return result;
}

void xr::Camera::updateView()
{
	view = glt::translate(glt::mat4f(), -position); // glt::lookAt(position, position + direction, up);
}


xr::OrthographicCamera::OrthographicCamera() :
	OrthographicCamera(-1, 1, 1, -1)
{
}

xr::OrthographicCamera::OrthographicCamera(float width, float height) :
	OrthographicCamera(0, width, 0, height)
{
}

xr::OrthographicCamera::OrthographicCamera(float left, float right, float top, float bottom) :
	Camera(glt::orthographic(left, right, bottom, top), { 0, 0, 0 }, { 0, 0, -1 }, {0, 1, 0})
{
}

void xr::OrthographicCamera::setProjection(float left, float right, float top, float bottom)
{
	this->projection = glt::orthographic(left, right, bottom, top);
}

