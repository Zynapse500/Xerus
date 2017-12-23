#pragma once

#include "Mesh.h"
#include "Texture.h"
#include "Camera.h"

namespace xr {


	// Threadsafe render buffer
	class RenderBatch {
		friend class Renderer;



		struct TransformationBatch {
			glm::mat4 transformation;

			Range<int> indexRange;

			TransformationBatch(glm::mat4 transformation) :
				transformation(transformation) {}
		};

		struct TextureBatch {
			std::vector<TransformationBatch> transBatches;
		};


		// List of meshes
		Mesh meshBuffer;


		// Pointer to the current index range
		Range<int>* currentIndexRange;

		// Texture batches
		std::map<Texture, TextureBatch> textureBatches;

		// Current fill color
		glm::vec4 fillColor;

		// Current transformation matrix
		glm::mat4 currentTransformation;

		// Current texture batch
		TextureBatch* currentTextureBatch;


		// The default, all white, texture
		Texture defaultTexture;

		// The region of the current texture to use
		Rectangle<float> currentTextureRegion;

	public:

		RenderBatch();


		// Clear all buffered commands, including capacity
		void clear();


		// Set the fill color
		void setFillColor(glm::vec4 color);
		void setFillColor(glm::vec3 color) { setFillColor(color.r, color.g, color.b); }
		void setFillColor(float r, float g, float b, float a = 1.0) { setFillColor({ r, g, b, a }); }
		void setFillColor(float g, float a = 1.0) { setFillColor({ g, g, g, a }); }


		// Set the camera matrix
		void setCamera(const glm::mat4& cameraMatrix);
		void setCamera(const Camera& camera);


		// Sets the current texture
		void setTexture(const Texture & texture, const Rectangle<float>& region = { 0.0f, 0.0f, 1.0f, 1.0f });
		void setTexture(const TextureRegion& region);

		// Stops rendering with a texture
		// In reality it switches to an all white texture
		void clearTexture();


		// Draw a filled rectangle
		void fillRect(float x, float y, float w, float h);
		void fillRect(float x, float y, float size) { fillRect(x, y, size, size); }
		void fillRect(glm::vec2 pos, float size) { fillRect(pos.x, pos.y, size, size); }
		void fillRect(glm::vec2 pos, glm::vec2 size) { fillRect(pos.x, pos.y, size.x, size.y); }


		// Draw a filled polygon
		void fillPolygon(const std::vector<glm::vec2>& points);


		// Draw a filled triangle fan, first point is the center
		void fillTriangleFan(const std::vector<glm::vec2>& points);


		// Draw a filled circle
		void fillCircle(float x, float y, float r, int segments = 32);
		void fillCircle(glm::vec2 center, float r, int segments = 32) { fillCircle(center.x, center.y, r, segments); }


		// Draw a line
		void drawLine(float x0, float y0, float x1, float y1, float width = 1);
		void drawLine(glm::vec2 p1, glm::vec2 p2, float width = 1) { drawLine(p1.x, p1.y, p2.x, p2.y, width); }


		// Draw a mesh
		void fillTriangles(const std::vector<glm::vec2>& points);

	private:

	};

}