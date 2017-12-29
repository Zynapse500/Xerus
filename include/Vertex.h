#pragma once

#include <glt.hpp>

namespace xr {
	struct Vertex {
		glt::vec3f position;
		glt::vec2f texCoord;
		glt::vec4f color;


		Vertex()
			: position(0.0), texCoord(0.0), color(1.0) {}

        explicit Vertex(const glt::vec3f &position)
			: position(position), texCoord(0.0), color(1.0) {}

		Vertex(const glt::vec3f &position, const glt::vec4f &color)
			: position(position), texCoord(0.0), color(color) {}

		Vertex(const glt::vec3f &position, const glt::vec2f &texCoord)
			: position(position), texCoord(texCoord), color(1.0) {}

		Vertex(const glt::vec3f &position, const glt::vec2f &texCoord, const glt::vec4f &color)
			: position(position), texCoord(texCoord), color(color) {}
	};
}