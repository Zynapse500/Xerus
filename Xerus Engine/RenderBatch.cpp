#include "stdafx.h"
#include "RenderBatch.h"

xr::RenderBatch::RenderBatch() :
	defaultTexture(1, 1, GL_RGBA, new unsigned char[4]{255, 255, 255, 255}),
	currentTextureRegion(0, 0, 1, 1)
{
	this->clearTexture();
}

void xr::RenderBatch::clear()
{
	this->textureBatches.clear();

	this->meshBuffer.vertices.clear();
	this->meshBuffer.indices.clear();

	this->currentTransformation = glm::mat4();
	this->fillColor = { 1.0, 1.0, 1.0, 1.0 };

	this->clearTexture();
}

void xr::RenderBatch::setFillColor(glm::vec4 color)
{
	this->fillColor = color;
}

void xr::RenderBatch::setCameraMatrix(const glm::mat4 & cameraMatrix)
{
	this->currentTransformation = cameraMatrix;

	this->currentTextureBatch->transBatches.emplace_back(currentTransformation);
	this->currentIndexRange = &this->currentTextureBatch->transBatches.back().indexRange;
	this->currentIndexRange->lower = this->currentIndexRange->upper = this->meshBuffer.indices.size();
}

void xr::RenderBatch::setTexture(const Texture & texture, const Rectangle<float>& region)
{
	this->currentTextureRegion = region;

	TextureBatch& textureBatch = this->textureBatches[texture];

	if (this->currentTextureBatch == &textureBatch) {
		return;
	}

	this->currentTextureBatch = &textureBatch;

	// Add a new Transformation batch if this is a new texture or
	// the matrix has changed since last time this texture was used
	if (textureBatch.transBatches.size() == 0 ||
		textureBatch.transBatches.back().transformation != currentTransformation) {
		this->setCameraMatrix(currentTransformation);
	}

	this->currentIndexRange = &this->currentTextureBatch->transBatches.back().indexRange;
}

void xr::RenderBatch::setTexture(const TextureRegion & region)
{
	this->setTexture(region.getTexture(), region.getRegion());
}

void xr::RenderBatch::clearTexture()
{
	this->setTexture(this->defaultTexture);
}


void xr::RenderBatch::drawRect(float x, float y, float w, float h)
{
	// Get the index of the last vertex and start indexing from there
	int startIndex = this->meshBuffer.vertices.size();

	// Add indices to all vertices
	this->meshBuffer.indices.emplace_back(0 + startIndex);
	this->meshBuffer.indices.emplace_back(1 + startIndex);
	this->meshBuffer.indices.emplace_back(2 + startIndex);
	this->meshBuffer.indices.emplace_back(2 + startIndex);
	this->meshBuffer.indices.emplace_back(3 + startIndex);
	this->meshBuffer.indices.emplace_back(0 + startIndex);

	// Increase index range
	this->currentIndexRange->upper += 6;
	
	// Provide alias for 'currentTextureRegion'
	auto& r = this->currentTextureRegion;

	float z = 0;

	// Add vertices
	this->meshBuffer.vertices.emplace_back(glm::vec3{ x, y, z }, glm::vec2{ r.x, r.y + r.height }, this->fillColor);
	this->meshBuffer.vertices.emplace_back(glm::vec3{ x, y + h, z }, glm::vec2{ r.x, r.y }, this->fillColor);
	this->meshBuffer.vertices.emplace_back(glm::vec3{ x + w, y + h, z }, glm::vec2{ r.x + r.width, r.y }, this->fillColor);
	this->meshBuffer.vertices.emplace_back(glm::vec3{ x + w, y, z }, glm::vec2{ r.x + r.width, r.y + r.height }, this->fillColor);
}
