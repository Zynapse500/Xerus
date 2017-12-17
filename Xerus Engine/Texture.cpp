#include "stdafx.h"

#include "Texture.h"

xr::Texture::Texture(int width, int height, GLenum format, const unsigned char * data)
{
	glGenTextures(1, &this->texture);
	
	this->bind();
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, format, GL_UNSIGNED_BYTE, data);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	
	this->setMinMagFilter(GL_NEAREST, GL_NEAREST);
	Texture::unbind();
}

xr::Texture::Texture(const Image & image)
	: Texture(image.getWidth(), image.getHeight(), GL_RGBA, image.data())
{
}

xr::Texture::Texture()
	: texture(-1)
{
}

void xr::Texture::bind() const
{
	glBindTexture(GL_TEXTURE_2D, this->texture);
}

void xr::Texture::unbind()
{
	glBindTexture(GL_TEXTURE_2D, 0);
}

void xr::Texture::setMinMagFilter(GLenum min, GLenum mag)
{
	this->bind();
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, min);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, mag);
	Texture::unbind();
}

bool xr::Texture::operator<(const Texture & other) const
{
	return this->texture < other.texture;
}

bool xr::Texture::operator==(const Texture & other) const
{
	return this->texture == other.texture;
}

bool xr::Texture::operator!=(const Texture & other) const
{
	return !(*this == other);
}

xr::TextureRegion::TextureRegion(const ImageRegion & region, const Texture & texture)
	: region((float)region.x / region.parentWidth,
			 (float)region.y / region.parentHeight,
			 (float)region.width / region.parentWidth,
			 (float)region.height / region.parentHeight),
	texture(texture)
{
}

xr::Rectangle<float> xr::TextureRegion::getRegion() const
{
	return this->region;
}

xr::Texture xr::TextureRegion::getTexture() const
{
	return this->texture;
}
