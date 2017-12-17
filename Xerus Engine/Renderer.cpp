#include "stdafx.h"

#include "Renderer.h"



const char* vertexSource = R"(#version 330
in vec3 position;
in vec2 texCoord;
in vec4 color;

uniform mat4 camera = mat4(1.0);

out FragData {
	vec3 position;
	vec2 texCoord;
	vec4 color;
} frag;

void main() {
	gl_Position = camera * vec4(position, 1.0);
	frag.position = position;
	frag.texCoord = texCoord;
	frag.color = color;
})";

const char* fragmentSource = R"(#version 330
in FragData {
	vec3 position;
	vec2 texCoord;
	vec4 color;
} frag;

uniform sampler2D texture0;

out vec4 outColor;

void main() {
	outColor = texture(texture0, frag.texCoord) * frag.color;
})";



xr::Renderer::Renderer()
	: shader(vertexSource, fragmentSource)
{
	shader.bindAttribute(ATTR_POSITION, "position");
	shader.bindAttribute(ATTR_TEX_COORD, "texCoord");
	shader.bindAttribute(ATTR_COLOR, "color");

	this->uniformLocations.cameraMatrix = shader.getUniformLocation("camera");
	this->uniformLocations.texture0 = shader.getUniformLocation("texture0");
}

void xr::Renderer::clear(float r, float g, float b, float a)
{
	glClearColor(r, g, b, a);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void xr::Renderer::submit(const RendererSequence & sequence)
{
	int paramOffset = 0;
	// Returns the value of the next parameter
	auto getNextParam = [&]() {
		return sequence.parameters[paramOffset++];
	};

	// Points to the current mesh
	int meshOffset = 0;

	// Points to the current index of a mesh
	std::vector<int> indexOffsets(sequence.meshes.size(), 0);

	// Points to the next camera
	int cameraOffset = 0;

	// Points to the next camera
	int textureOffset = 0;

	this->shader.use();
	for each (auto command in sequence.commands)
	{
		if (command == DRAW_INDICES) {
			// Get the number of indices to draw
			int indexCount = getNextParam();

			// Get the current index offset
			int& indexOffset = indexOffsets[meshOffset];

			// Draw indices
			this->vertexBuffer.drawElements(indexCount, indexOffset);

			// Move index pointer
			indexOffset += indexCount;
		} else 
		if (command == NEXT_CAMERA) {
			// Get the next camera
			const float* cameraMatrix = glm::value_ptr(sequence.cameraMatrices[cameraOffset++]);

			// Upload the camera
			glUniformMatrix4fv(this->uniformLocations.cameraMatrix, 1, GL_FALSE, cameraMatrix);
		} else
		if (command == NEXT_TEXTURE) {
			// Get the next texture
			const Texture& texture = sequence.textures[textureOffset++];

			// Activate the texture
			glActiveTexture(GL_TEXTURE0);
			texture.bind();
			glUniform1i(this->uniformLocations.texture0, 0);
		} else
		if (command == SWITCH_MESH) {
			// Get the next mesh
			int meshIndex = getNextParam();
			const Mesh& mesh = sequence.meshes[meshIndex];

			// Upload mesh data
			this->vertexBuffer.upload(mesh.vertices);
			this->vertexBuffer.upload(mesh.indices);
		}
	}

	Texture::unbind();
}
