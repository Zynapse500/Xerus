#pragma once

#include "Mesh.h"
#include "Texture.h"

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
		void setFillColor(float r, float g, float b, float a = 1.0) { setFillColor({ r, g, b, a }); }
		void setFillColor(glm::vec3 color) { setFillColor(color.r, color.g, color.b); }
		void setFillColor(glm::vec4 color);


		// Set the camera matrix
		void setCameraMatrix(const glm::mat4& cameraMatrix);


		// Sets the current texture
		void setTexture(const Texture & texture, const Rectangle<float>& region = { 0.0f, 0.0f, 1.0f, 1.0f });
		void setTexture(const TextureRegion& region);


		// Draw a filled rectangle
		void drawRect(float x, float y, float w, float h);
		void drawRect(glm::vec2 pos, glm::vec2 size) { drawRect(pos.x, pos.y, size.x, size.y); }

	private:

		// Stops rendering with a texture
		// In reality it switches to an all white texture
		void clearTexture();


		// Adds a transformation
	};

}