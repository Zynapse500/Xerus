#pragma once

#include "Mesh.h"
#include "Texture.h"
#include "Camera.h"

namespace xr {


	// Threadsafe render buffer
	class RenderBatch {
		friend class Renderer;


		struct TextureBatch {
            // Vertices and indices to draw
			Mesh mesh;
		};


        // Texture batches
        std::map<Texture, TextureBatch> textureBatches;

		// Current fill color
		glt::vec4f fillColor;

		// Current transformation matrix
		glt::mat4f transformation;

		// Current texture batch
		TextureBatch* currentTextureBatch;


		// The default, all white, texture
		Texture defaultTexture;

		// The region of the current texture to use
		Rectangle<float> currentTextureRegion;

	public:

		RenderBatch();


		// Start a new drawing session
        void begin(const glt::mat4f& transformation);
        void begin(const Camera& camera);


		// Set the fill color
		void setFillColor(glt::vec4f color);
		void setFillColor(glt::vec3f color) { setFillColor(color.x, color.y, color.z); }
		void setFillColor(float r, float g, float b, float a = 1.0) { setFillColor({ r, g, b, a }); }
		void setFillColor(float g, float a = 1.0) { setFillColor({ g, g, g, a }); }


		// Sets the current texture
		void setTexture(const Texture & texture, const Rectangle<float>& region = { 0.0f, 0.0f, 1.0f, 1.0f });
		void setTexture(const TextureRegion& region);

		// Stops rendering with a texture
		// In reality it switches to an all white texture
		void clearTexture();


		// Draw a filled rectangle
		void fillRect(float x, float y, float w, float h);
		void fillRect(float x, float y, float size) { fillRect(x, y, size, size); }
		void fillRect(glt::vec2f pos, float size) { fillRect(pos.x, pos.y, size, size); }
		void fillRect(glt::vec2f pos, glt::vec2f size) { fillRect(pos.x, pos.y, size.x, size.y); }


		// Draw a filled polygon
		void fillPolygon(const std::vector<glt::vec2f>& points);


		// Draw a filled triangle fan, first point is the center
		void fillTriangleFan(const std::vector<glt::vec2f>& points);


		// Draw a filled circle
		void fillCircle(float x, float y, float r, int segments = 32);
		void fillCircle(glt::vec2f center, float r, int segments = 32) { fillCircle(center.x, center.y, r, segments); }


		// Draw a line
		void drawLine(float x0, float y0, float x1, float y1, float width = 1);
		void drawLine(glt::vec2f p1, glt::vec2f p2, float width = 1) { drawLine(p1.x, p1.y, p2.x, p2.y, width); }


		// Draw a mesh
		void fillTriangles(const std::vector<glt::vec2f>& points);

	private:

        // Returns the mesh currently being rendered to
        Mesh& getCurrentMesh();

	};

}