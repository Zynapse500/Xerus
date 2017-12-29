#pragma once
#include "Shader.h"
#include "Vertex.h"
#include "Buffer.h"
#include "Texture.h"

#include "Mesh.h"

#include "RenderBatch.h"

namespace xr {


	class Renderer
	{
		// Main shader
		Shader shader;

		// Vertex buffer
		VertexBuffer vertexBuffer;

		// Uniform locations
		struct UniformLocations {
			GLuint cameraMatrix;
			GLuint texture0;
			GLuint colorFilter;
		} uniformLocations;


		// Filter color
		glt::vec4f colorFilter;

	public:

		// Create a new renderer
		Renderer();


		// Clear color and depth buffer
		void clear(float r, float g, float b, float a);
		void clear(glt::vec4f color) { this->clear(color.r, color.g, color.b, color.a); }


		// Submit a batch to the renderer
		void submit(const RenderBatch& batch);


		// Set the filter color
		void setColorFilter(glt::vec4f color);
	};
}

