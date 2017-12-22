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
uniform vec4 colorFilter;

out vec4 outColor;

void main() {
	outColor = texture(texture0, frag.texCoord) * frag.color * colorFilter;
})";


xr::Renderer::Renderer() :
	shader(vertexSource, fragmentSource),
	colorFilter(1, 1, 1, 1)
{
	shader.bindAttribute(ATTR_POSITION, "position");
	shader.bindAttribute(ATTR_TEX_COORD, "texCoord");
	shader.bindAttribute(ATTR_COLOR, "color");

	this->uniformLocations.cameraMatrix = shader.getUniformLocation("camera");
	this->uniformLocations.texture0 = shader.getUniformLocation("texture0");
	this->uniformLocations.colorFilter = shader.getUniformLocation("colorFilter");
}

void xr::Renderer::clear(float r, float g, float b, float a)
{
	glClearColor(r, g, b, a);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void xr::Renderer::submit(const RenderBatch & batch)
{
	this->vertexBuffer.upload(batch.meshBuffer.vertices);
	this->vertexBuffer.upload(batch.meshBuffer.indices);

	this->shader.use();

	glUniform4f(this->uniformLocations.colorFilter, colorFilter.r, colorFilter.g, colorFilter.b, colorFilter.a);

	for (auto& texturePair : batch.textureBatches) {
		texturePair.first.bind();
		for (auto& transBatch: texturePair.second.transBatches) {
			glUniformMatrix4fv(this->uniformLocations.cameraMatrix, 1, 0, glm::value_ptr(transBatch.transformation));

			auto& range = transBatch.indexRange;
			this->vertexBuffer.drawElements(range.upper - range.lower, range.lower);
		}
	}

}

void xr::Renderer::setColorFilter(glm::vec4 color)
{
	this->colorFilter = color;
}
