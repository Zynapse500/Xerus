#include "stdafx.h"
#include "Buffer.h"


xr::Buffer::Buffer(GLenum bufferType)
{
	this->type = bufferType;
	glGenBuffers(1, &this->buffer);
}

xr::Buffer::~Buffer()
{
	glDeleteBuffers(1, &this->buffer);
}

void xr::Buffer::upload(void * data, int length)
{
	this->bind();
	glBufferData(this->type, length, data, GL_DYNAMIC_DRAW);
}

void xr::Buffer::bind()
{
	glBindBuffer(this->type, this->buffer);
}



xr::VertexBuffer::VertexBuffer()
	: vbo(GL_ARRAY_BUFFER),
	  ibo(GL_ELEMENT_ARRAY_BUFFER)
{
	glGenVertexArrays(1, &this->vao);
	glBindVertexArray(this->vao);

	this->vbo.bind();
	this->ibo.bind();

	// Enable attributes
	// Position
	glEnableVertexAttribArray(ATTR_POSITION);
	glVertexAttribPointer(ATTR_POSITION, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)(offsetof(Vertex, position)));

	// Texture coordiantes
	glEnableVertexAttribArray(ATTR_TEX_COORD);
	glVertexAttribPointer(ATTR_TEX_COORD, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)(offsetof(Vertex, texCoord)));

	// Color
	glEnableVertexAttribArray(ATTR_COLOR);
	glVertexAttribPointer(ATTR_COLOR, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)(offsetof(Vertex, color)));


	glBindVertexArray(0);
}

xr::VertexBuffer::~VertexBuffer()
{
	glDeleteVertexArrays(1, &this->vao);
}

void xr::VertexBuffer::upload(const std::vector<Vertex>& vertices)
{
	glBindVertexArray(this->vao);
	this->vbo.upload((void*)vertices.data(), vertices.size() * sizeof(Vertex));
	glBindVertexArray(0);
}

void xr::VertexBuffer::upload(const std::vector<GLuint>& indices) {
	glBindVertexArray(this->vao);
	this->ibo.upload((void*)indices.data(), indices.size() * sizeof(GLuint));
	glBindVertexArray(0);
}


void xr::VertexBuffer::drawElements(GLuint count, GLuint offset, GLenum mode)
{
	glBindVertexArray(this->vao);
	
	glDrawElements(mode, count, GL_UNSIGNED_INT, (void*)(offset * sizeof(GLuint)));

	glBindVertexArray(0);
}


