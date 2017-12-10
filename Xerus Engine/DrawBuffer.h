#pragma once
#include <memory>
#include <map>

#include "Vertex.h"
#include "Renderer.h"
#include "Texture.h"

namespace xr {

	// Constructs a rendering sequence from abstracted commands
	class DrawBuffer
	{
		// The render sequence to construct
		RendererSequence sequence;

		// The default, all white, texture
		Texture defaultTexture;
		
		// Current fill color
		glm::vec4 fillColor;

		// Current region of texture
		Rectangle<float> currentTextureRegion;

		// Is layering enabled?
		bool layering;

		// How thick should the layers be
		float layerThickness;

	public:

		// Create a new drawbuffer with/without layering (2d objects will appear on top of earlier objects
		DrawBuffer(bool layering = false, float layerThickness = 0.05f);
		~DrawBuffer();


		// Clear all buffered commands
		void clear();


		// Set the fill color
		void setFillColor(float r, float g, float b, float a = 1.0);
		void setFillColor(glm::vec3 color) { setFillColor(color.r, color.g, color.b); }
		void setFillColor(glm::vec4 color) { setFillColor(color.r, color.g, color.b, color.a); }


		// Set the texture
		void setTexture(const Texture & texture, const Rectangle<float>& region = { 0.0f, 0.0f, 1.0f, 1.0f });
		void setTexture(const TextureRegion& region);



		// Set the camera matrix
		void setCameraMatrix(const glm::mat4& cameraMatrix);


		// Draw a filled rectangle
		void drawRect(float x, float y, float z, float w, float h);
		void drawRect(float x, float y, float w, float h) { drawRect(x, y, 0.0, w, h); }
		void drawRect(glm::vec3 pos, glm::vec2 size) { drawRect(pos.x, pos.y, pos.z, size.x, size.y); }


		// Encode objects into a rendering sequence
		const RendererSequence& encodeSequence();

	};
}


