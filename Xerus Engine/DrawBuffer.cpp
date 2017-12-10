#include "stdafx.h"
#include "DrawBuffer.h"


xr::DrawBuffer::DrawBuffer(bool layering, float layerThickness)
	: defaultTexture(2, 2, GL_RGBA,
					 new unsigned char[16]{ 255, 255, 255, 255, 255, 255, 255, 255,
											255, 255, 255, 255, 255, 255, 255, 255 }),
	currentTextureRegion(0.0f, 0.0f, 1.0f, 1.0f)
{
	this->layering = layering;
	this->layerThickness = layerThickness;

	this->fillColor = { 1.0, 1.0, 1.0, 1.0 };
	this->clear();
}


xr::DrawBuffer::~DrawBuffer()
{
}

void xr::DrawBuffer::clear()
{
	sequence.clear();
	this->setTexture(this->defaultTexture);
}

void xr::DrawBuffer::setFillColor(float r, float g, float b, float a)
{
	this->fillColor = { r, g, b, a };
}


void xr::DrawBuffer::setTexture(const Texture & texture, const Rectangle<float>& region)
{
	// TODO: We want to switch textures as little as possible
	if (this->sequence.textures.size() == 0 || this->sequence.textures.back() != texture) {
		this->sequence.commands.push_back(NEXT_TEXTURE);
		this->sequence.textures.push_back(texture);
	}
	this->currentTextureRegion = region;
}

void xr::DrawBuffer::setTexture(const TextureRegion & region)
{
	this->setTexture(region.getTexture(), region.getRegion());
}


void xr::DrawBuffer::setCameraMatrix(const glm::mat4& cameraMatrix) {

	this->sequence.commands.push_back(NEXT_CAMERA);
	this->sequence.cameraMatrices.push_back(cameraMatrix);
}

void xr::DrawBuffer::drawRect(float x, float y, float z, float w, float h)
{
	// If we are not drawing yet, start drawing
	if (this->sequence.commands.back() != DRAW_INDICES) {
		this->sequence.commands.push_back(DRAW_INDICES);
		this->sequence.parameters.push_back(0);
	}

	// Get the index of the last vertex and start indexing from there
	int startIndex = this->sequence.vertices.size();

	// Add indices to all vertices
	this->sequence.indices.emplace_back(0 + startIndex);
	this->sequence.indices.emplace_back(1 + startIndex);
	this->sequence.indices.emplace_back(2 + startIndex);
	this->sequence.indices.emplace_back(2 + startIndex);
	this->sequence.indices.emplace_back(3 + startIndex);
	this->sequence.indices.emplace_back(0 + startIndex);

	// Save the number of additional indices
	this->sequence.parameters.back() += 6;

	auto& r = this->currentTextureRegion;

	// Add vertices
	this->sequence.vertices.emplace_back(glm::vec3{ x, y, z },			glm::vec2{ r.x, r.y + r.height },			this->fillColor);
	this->sequence.vertices.emplace_back(glm::vec3{ x, y + h, z },		glm::vec2{ r.x, r.y },						this->fillColor);
	this->sequence.vertices.emplace_back(glm::vec3{ x + w, y + h, z },	glm::vec2{ r.x + r.width, r.y },			this->fillColor);
	this->sequence.vertices.emplace_back(glm::vec3{ x + w, y, z },		glm::vec2{ r.x + r.width, r.y + r.height }, this->fillColor);
}

const xr::RendererSequence & xr::DrawBuffer::encodeSequence()
{
	return this->sequence;
}

//
//xr::RendererSequence xr::DrawBuffer::encode()
//{
//	static RendererSequence sequence;
//	sequence.clear();
//
//	sequence.commands.push_back(NEXT_CAMERA);
//	sequence.cameraMatrices.push_back(this->cameraMatrix);
//
//
//	int shapeCount = 0;
//	for each (auto& pair in this->textureBatches) {
//		shapeCount += pair.second.shapes.size();
//	}
//
//	float depthOffset = 0.0f;
//	float depthIncrement = (this->layering) ? this->layerThickness / shapeCount : 0;
//	
//	for each (auto& pair in this->textureBatches)
//	{
//		const Texture& texture = pair.first;
//		const Batch& batch = pair.second;
//
//		// Add the texture
//		sequence.commands.push_back(NEXT_TEXTURE);
//		sequence.textures.push_back(texture);
//
//		// Add a new batch
//		sequence.commands.push_back(DRAW_INDICES);
//		int batchIndexCount = 0;
//		for each (auto& shape in batch.shapes)
//		{
//			// Add all indices with a offset to the (currently) last vertex
//			GLuint startIndex = sequence.vertices.size();
//			for each (auto& index in shape.indices) {
//				sequence.indices.push_back(index + startIndex);
//			}
//
//			for each (auto& vertex in shape.vertices) {
//				sequence.vertices.emplace_back(vertex.position + glm::vec3{0.0f, 0.0f, depthOffset}, vertex.texCoord, vertex.color);
//			}
//			depthOffset += depthIncrement;
//
//			batchIndexCount += shape.indices.size();
//		}
//		sequence.parameters.push_back(batchIndexCount);
//	}
//
//	return sequence;
//}
