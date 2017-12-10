#pragma once

#include "Image.h"
#include "Collision.h"

namespace xr {

	// 2D-texture
	class Texture
	{
		// Handle to the texture
		GLuint texture;
		
	public:

		// Create a new texture
		Texture(int width, int height, GLenum format = GL_RGBA, const unsigned char* data = nullptr);
		Texture(const Image& image);

		// Bind this texture
		void bind() const;

		// Unbind the 2d texture
		static void unbind();


		// Change the min and mag filters
		void setMinMagFilter(GLenum min, GLenum mag);


		// Compare two textures (used for hashing in map)
		bool operator<(const Texture& other) const;


		// Are two textures pointing to same OpenGL texture?
		bool operator==(const Texture& other) const;
		bool operator!=(const Texture& other) const;
	};


	// Represents a subrectangle of a texture
	class TextureRegion {

		// Region
		Rectangle<float> region;

		// Parent
		Texture texture;

	public:

		// Convert from image region
		TextureRegion(const ImageRegion& region, const Texture& texture);

		// Return the region
		Rectangle<float> getRegion() const;


		// Return the parent texture
		Texture getTexture() const;

	};

}
