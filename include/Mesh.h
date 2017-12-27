#pragma once

#include "Vertex.h"

namespace xr{
	// Represents a list of indexed vertices
	struct Mesh
	{
		// Create new Mesh
		Mesh(GLenum drawMode = GL_TRIANGLES);

		// Mode to draw vertices in
		GLuint drawMode;

		// List of vertices
		std::vector<Vertex> vertices;

		// List of indices
		std::vector<GLuint> indices;
	};
}

