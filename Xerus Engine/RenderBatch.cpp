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

	this->currentMesh = &this->currentTextureBatch->transBatches.back().mesh;
}

void xr::RenderBatch::setTexture(const Texture & texture, const Rectangle<float>& region)
{
	// Add a new Transformation batch if this is a new texture or
	// the matrix has changed since last time this texture was used
	TextureBatch& textureBatch = this->textureBatches[texture];
	if (textureBatch.transBatches.size() == 0 ||
		textureBatch.transBatches.back().transformation != currentTransformation) {
		textureBatch.transBatches.emplace_back(currentTransformation);
	}

	this->currentMesh = &textureBatch.transBatches.back().mesh;
	this->currentTextureRegion = region;

	this->currentTextureBatch = &textureBatch;
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
	int startIndex = this->currentMesh->vertices.size();

	// Add indices to all vertices
	this->currentMesh->indices.emplace_back(0 + startIndex);
	this->currentMesh->indices.emplace_back(1 + startIndex);
	this->currentMesh->indices.emplace_back(2 + startIndex);
	this->currentMesh->indices.emplace_back(2 + startIndex);
	this->currentMesh->indices.emplace_back(3 + startIndex);
	this->currentMesh->indices.emplace_back(0 + startIndex);
	
	// Provide alias for 'currentTextureRegion'
	auto& r = this->currentTextureRegion;

	float z = 0;

	// Add vertices
	this->currentMesh->vertices.emplace_back(glm::vec3{ x, y, z }, glm::vec2{ r.x, r.y + r.height }, this->fillColor);
	this->currentMesh->vertices.emplace_back(glm::vec3{ x, y + h, z }, glm::vec2{ r.x, r.y }, this->fillColor);
	this->currentMesh->vertices.emplace_back(glm::vec3{ x + w, y + h, z }, glm::vec2{ r.x + r.width, r.y }, this->fillColor);
	this->currentMesh->vertices.emplace_back(glm::vec3{ x + w, y, z }, glm::vec2{ r.x + r.width, r.y + r.height }, this->fillColor);
}
