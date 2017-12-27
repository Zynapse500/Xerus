#pragma once

#include <glm/glm.hpp>

namespace xr {
	struct Vertex {
		glm::vec3 position;
		glm::vec2 texCoord;
		glm::vec4 color;


		Vertex()
			: position(0.0), texCoord(0.0), color(1.0) {}

		Vertex(glm::vec3 position)
			: position(position), texCoord(0.0), color(1.0) {}

		Vertex(glm::vec3 position, glm::vec4 color)
			: position(position), texCoord(0.0), color(color) {}

		Vertex(glm::vec3 position, glm::vec2 texCoord)
			: position(position), texCoord(texCoord), color(1.0) {}

		Vertex(glm::vec3 position, glm::vec2 texCoord, glm::vec4 color)
			: position(position), texCoord(texCoord), color(color) {}
	};
}