#pragma once

#include "Vertex.h"




namespace xr {

	enum AttributeLocations {
		ATTR_POSITION,
		ATTR_TEX_COORD,
		ATTR_COLOR,
	};

	class Buffer {

		// Handle to buffer
		GLuint buffer;

		// Buffer type
		GLenum type;

	public:

		// Create new buffer
		Buffer(GLenum bufferType);

		~Buffer();

		// Upload data to buffer
		void upload(void* data, int length);

		// Bind this buffer
		void bind();
	};


	class VertexBuffer
	{
		// Handle to the VAO
		GLuint vao;

		// Vertex buffer
		Buffer vbo;

		// Index buffer
		Buffer ibo;

	public:

		VertexBuffer();
		~VertexBuffer();


		// Upload vertices to buffer
		void upload(const std::vector<Vertex>& vertices);

		// Upload indices to buffer
		void upload(const std::vector<GLuint>& indices);

		// Draw the vertices in this buffer
		void drawElements(GLuint count, GLuint offset);


	private:



	};
}

