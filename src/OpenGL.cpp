#include "stdafx.h"

#include <stdexcept>

#include "OpenGL.h"

void xr::loadOpenGL()
{
	if (glewInit()) {
		throw std::runtime_error("Failed to create OpenGL context");
	}
}
