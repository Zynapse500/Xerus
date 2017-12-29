#include "stdafx.h"
#include "RenderBatch.h"

#include "VectorMath.h"

#include "Constants.h"

xr::RenderBatch::RenderBatch() :
	defaultTexture(1, 1, GL_RGBA, new unsigned char[4]{255, 255, 255, 255}),
	currentTextureRegion(0, 0, 1, 1)
{
	this->clearTexture();
}

void xr::RenderBatch::begin(const glt::mat4f &transformation) {
    // TODO: Keep capacity of meshes
    this->textureBatches.clear();

    this->transformation = transformation;
    this->fillColor = { 1.0, 1.0, 1.0, 1.0 };

    this->clearTexture();
}

void xr::RenderBatch::begin(const xr::Camera &camera) {
    begin(camera.getTransform());
}

void xr::RenderBatch::setFillColor(glt::vec4f color)
{
	this->fillColor = color;
}

void xr::RenderBatch::setTexture(const Texture & texture, const Rectangle<float>& region)
{
    currentTextureBatch = & textureBatches[texture];
    currentTextureRegion = region;
}

void xr::RenderBatch::setTexture(const TextureRegion & region)
{
	this->setTexture(region.getTexture(), region.getRegion());
}

void xr::RenderBatch::clearTexture()
{
	this->setTexture(this->defaultTexture);
}


void xr::RenderBatch::fillRect(float x, float y, float w, float h)
{
    Mesh& currentMesh = getCurrentMesh();

	// Get the index of the last vertex and start indexing from there
	int startIndex = currentMesh.vertices.size();

	// Add indices to all vertices
	currentMesh.indices.emplace_back(0 + startIndex);
	currentMesh.indices.emplace_back(1 + startIndex);
	currentMesh.indices.emplace_back(2 + startIndex);
	currentMesh.indices.emplace_back(2 + startIndex);
	currentMesh.indices.emplace_back(3 + startIndex);
	currentMesh.indices.emplace_back(0 + startIndex);

    // Provide alias for 'currentTextureRegion'
	auto& r = this->currentTextureRegion;

	float z = 0;

	// Add vertices
	currentMesh.vertices.emplace_back(glt::vec3f{ x, y, z }, glt::vec2f{ r.x, r.y + r.height }, this->fillColor);
	currentMesh.vertices.emplace_back(glt::vec3f{ x, y + h, z }, glt::vec2f{ r.x, r.y }, this->fillColor);
	currentMesh.vertices.emplace_back(glt::vec3f{ x + w, y + h, z }, glt::vec2f{ r.x + r.width, r.y }, this->fillColor);
	currentMesh.vertices.emplace_back(glt::vec3f{ x + w, y, z }, glt::vec2f{ r.x + r.width, r.y + r.height }, this->fillColor);
}



/*
////////////////////
ALGORITHM:
////////////////////

//// Determine inside
1. Calculate the sum of all the corner's angles, counterclockwise and clockwise
2. The side with the smaller angle sum is the inside

//// Construct triangles
3. Calculate the inner angle of the next corner
4. If the angle is greater than 180 degrees, set that as the new centroid
6. Otherwise construct triangle with the next 2 following points
5. Repeat step 3-5 until all triangles are constructed
*/

void xr::RenderBatch::fillPolygon(const std::vector<glt::vec2f>& points)
{
    Mesh& currentMesh = getCurrentMesh();

    int pointCount = points.size();

	if (pointCount < 3) {
		return;
	}

#define IND(index) (((index) + pointCount) % pointCount)

	auto getPoint = [&](int index) {
		return points[IND(index)];
	};

	// List of all corner's angles
	std::vector<float> angles(pointCount);


	// Determine inside

	// Calculate corner angles
	for (int i = 0; i < pointCount; i++) {
		glt::vec2f o = getPoint(i);
		glt::vec2f a = glt::normalize(getPoint(i - 1) - o);
		glt::vec2f b = glt::normalize(getPoint(i + 1) - o);

		float angle = angleBetween(a, b) + float(PI);

		angles[i] = angle;
	}


	// Sum all angles
	float angleSum = 0;
	for (int i = 0; i < pointCount; i++)
	{
		angleSum += angles[i];
	}

	// The total should be less than pointCount * PI if it's clockwise
	bool clockwise = angleSum < pointCount * float(PI);


	// Recalculate angles
	if (!clockwise) {
		for (int i = 0; i < pointCount; i++)
		{
			angles[i] = 2 * float(PI) - angles[i];
		}
	}

	// Construct triangles

	// Add vertices
	for (int i = 0; i < pointCount; i++)
	{
		currentMesh.vertices.emplace_back(glt::vec3f(getPoint(i), 0), glt::vec2f{ 0, 0 }, this->fillColor);
	}

	// Number of triangles left to construct
	int trianglesLeft = pointCount - 2;
	
	// Current center point of triangle fan
	int centroid = 0;

	// Current point being added to triangle fan
	int pointIndex = 1;

	// Add triangles until all are constructed
	while (trianglesLeft) {
		if (angles[pointIndex] > PI) {
			// Switch centroid if angle is too large
			centroid = pointIndex;
		}
		else {
			// Add triangle
			currentMesh.indices.emplace_back(centroid);
			currentMesh.indices.emplace_back(pointIndex);
			currentMesh.indices.emplace_back(IND(pointIndex + 1));

			// Triangle is done
			trianglesLeft--;
		}

		pointIndex = (pointIndex + 1) % pointCount;
	}

}

void xr::RenderBatch::fillTriangleFan(const std::vector<glt::vec2f>& points)
{
    Mesh& currentMesh = getCurrentMesh();


    int pointCount = points.size();


	// Get the index of the last vertex and start indexing from there
	int startIndex = currentMesh.vertices.size();


	// Add vertices
	for (int i = 0; i < pointCount; i++) {
		currentMesh.vertices.emplace_back(glt::vec3f(points[i], 0), glt::vec2f{ 0, 0 }, this->fillColor);
	}

	// Add indices
	for (int i = 1; i < pointCount - 1; i++) {
		currentMesh.indices.emplace_back(startIndex + 0);
		currentMesh.indices.emplace_back(startIndex + i);
		currentMesh.indices.emplace_back(startIndex + i + 1);
	}
}

void xr::RenderBatch::fillCircle(float x, float y, float r, int segments)
{
    Mesh& currentMesh = getCurrentMesh();

    // Get the index of the last vertex and start indexing from there
	int startIndex = currentMesh.vertices.size();

	// Add vertices
	currentMesh.vertices.reserve(static_cast<unsigned int>(startIndex + segments + 1));
	currentMesh.vertices.emplace_back(glt::vec3f(x, y, 0), glt::vec2f{ 0, 0 }, this->fillColor);
	for (int i = 0; i < segments; i++)
	{
		float dx = r * cosf(2 * float(PI) * i / float(segments));
		float dy = r * sinf(2 * float(PI) * i / float(segments));
		currentMesh.vertices.emplace_back(glt::vec3f(x + dx, y + dy, 0), glt::vec2f{ 0, 0 }, this->fillColor);
	}

	currentMesh.indices.reserve(currentMesh.indices.size() + segments * 3);
	for (int i = 0; i < segments; i++) {
		currentMesh.indices.emplace_back(startIndex + 0);
		currentMesh.indices.emplace_back(startIndex + i + 1);
		currentMesh.indices.emplace_back(startIndex + 1 + (i + 1) % segments);
	}
}



void xr::RenderBatch::drawLine(float x0, float y0, float x1, float y1, float width)
{
    Mesh& currentMesh = getCurrentMesh();

    // Find the direction of the line
	glt::vec2f dir = glt::normalize(glt::vec2f{ x1 - x0, y1 - y0 });

	// Find the perpendicular line
	glt::vec2f perp = { -dir.y, dir.x };

	// Find the corners
	glt::vec2f a = glt::vec2f{ x0, y0 } + perp * width / 2.f;
	glt::vec2f b = glt::vec2f{ x0, y0 } - perp * width / 2.f;
	glt::vec2f c = glt::vec2f{ x1, y1 } + perp * width / 2.f;
	glt::vec2f d = glt::vec2f{ x1, y1 } - perp * width / 2.f;

	
	// Get the index of the last vertex and start indexing from there
	int startIndex = currentMesh.vertices.size();

	// Add vertices

	Vertex va {glt::vec3f{a}, glt::vec2f{0}, this->fillColor};

	currentMesh.vertices.push_back(va);
	currentMesh.vertices.emplace_back(glt::vec3f(b, 0), glt::vec2f(0), this->fillColor);
	currentMesh.vertices.emplace_back(glt::vec3f(c, 0), glt::vec2f(0), this->fillColor);
	currentMesh.vertices.emplace_back(glt::vec3f(d, 0), glt::vec2f(0), this->fillColor);

	currentMesh.indices.emplace_back(startIndex);
	currentMesh.indices.emplace_back(startIndex + 1);
	currentMesh.indices.emplace_back(startIndex + 2);
	currentMesh.indices.emplace_back(startIndex + 2);
	currentMesh.indices.emplace_back(startIndex + 3);
	currentMesh.indices.emplace_back(startIndex + 1);
}


void xr::RenderBatch::fillTriangles(const std::vector<glt::vec2f>& points) 
{
    Mesh& currentMesh = getCurrentMesh();

    // Get the index of the last vertex and start indexing from there
	int startIndex = currentMesh.vertices.size();

	for (auto& point : points)
	{
		currentMesh.vertices.emplace_back(glt::vec3f(point, 0), glt::vec2f{ 0, 0 }, this->fillColor);
		currentMesh.indices.emplace_back(startIndex);
		startIndex++;
	}
}

xr::Mesh &xr::RenderBatch::getCurrentMesh() {
    return currentTextureBatch->mesh;
}




