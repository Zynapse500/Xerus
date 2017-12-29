#include "stdafx.h"

#include <algorithm>
#include <picopng.h>
#include "Utility.h"
#include "Collision.h"

#include "Image.h"

xr::Image::Image(int width, int height)
{
	this->width = width;
	this->height = height;

	this->pixels.resize(width * height * 4);
}

xr::Image::Image(ImageFormat format, unsigned char * data, size_t dataSize)
{
	this->decode(format, data, dataSize);
}

xr::Image::Image(const char * path)
{
	std::vector<unsigned char> bytes = util::loadBytes(path);
	
	this->decode(DONT_KNOW, bytes.data(), bytes.size());
}

const unsigned char * xr::Image::data() const
{
	return pixels.data();
}

int xr::Image::getWidth() const
{
	return width;
}

int xr::Image::getHeight() const
{
	return height;
}

void xr::Image::blit(const Image & image, int x, int y)
{
	for (unsigned j = y; j - y < image.height && j < this->height; j++)
	{
		for (unsigned i = x; i - x < image.width && i < this->width; i++)
		{
			int srcIndex = 4 * ((i - x) + (j - y) * image.width);
			int dstIndex = 4 * (i + j * this->width);

			for (int k = 0; k < 4; k++) {
				this->pixels[dstIndex + k] = image.pixels[srcIndex + k];
			}
		}
	}
}

void xr::Image::decode(ImageFormat format, unsigned char * data, size_t dataSize)
{
	if (format == DONT_KNOW) {
		format = getFormatFromBytes(data, dataSize);
	}

	switch (format)
	{
	case PNG:
		decodePNG(this->pixels, this->width, this->height, data, dataSize, true);
		break;

	case DONT_KNOW:
		break;
	default:
		break;
	}
}

xr::ImageFormat xr::Image::getFormatFromBytes(unsigned char * data, size_t size)
{
	// Attempt to match PNG
	if (size > 8) {
		if (data[0] == 137 &&
			data[1] == 80  &&
			data[2] == 78  &&
			data[3] == 71  &&
			data[4] == 13  &&
			data[5] == 10  &&
			data[6] == 26  &&
			data[7] == 10) {
			return PNG;
		}
	}

	return DONT_KNOW;
}

std::vector<xr::ImageRegion> xr::stitchImages(Image& result, std::vector<const Image*> images)
{
	typedef std::pair<int, glt::vec2i> IndexPair;

	std::vector<IndexPair> imageSizes;

	int imagesArea = 0;
	int maxWidth = 0;
	for (unsigned i = 0; i < images.size(); i++)
	{
		int w = images[i]->getWidth();
		int h = images[i]->getHeight();

		imageSizes.push_back(std::make_pair(i, glt::vec2i{ w, h }));
		imagesArea += w * h;

		if (w > maxWidth) maxWidth = w;
	}

	// Sort by height, then width
	std::sort(imageSizes.begin(), imageSizes.end(), [](const IndexPair& a, const IndexPair& b) {
		if (a.second.y > b.second.y) return true;
		if (a.second.y < b.second.y) return false;
		if (a.second.x > b.second.x) return true;
		return false;
	});


	// Width and height of the stitched image
	long currentWidth = 10000000; // Start with large width
	long currentHeight = imageSizes.front().second.y;


	// Represent the smallest rectangle (atlas) and the layout
	struct Rectangle {
		long w, h;
		std::vector<IndexPair> locations;
	} bestRectangle;
	bestRectangle.w = currentWidth;
	bestRectangle.h = currentHeight;

	// While the largest image can fit
	while (currentWidth >= maxWidth) {
		// Cannot place image on collider
		std::vector<xr::Rectangle<float>> colliders;
		
		// Where all the images got or can be placed
		std::vector<IndexPair> finalLocations;
		std::vector<glt::vec2i> leftLocations = { {0, 0} };

		// Could all images fit in the rectangle?
		bool placedAll = true;

		// Attempt to place each image
		for (IndexPair& pair : imageSizes) {

			// At what (left prioritized) position can the current image be placed?
			int leftIndex = -1;
			for (unsigned i = 0; i < leftLocations.size(); i++) {
				// Place rectangle at the leftmost location where it fits
				int x = leftLocations[i].x;
				int y = leftLocations[i].y;
				if (pair.second.y + y <= currentHeight &&
					pair.second.x + x <= currentWidth) {

					// Cannot place ontop of another image
					bool collided = false;
					for (unsigned i = 0; i < colliders.size(); i++)
					{
						if (colliders[i].intersects((float)x, (float)y, (float)pair.second.x, (float)pair.second.y)) {
							collided = true;
							break;
						}
					}
					if (collided) continue;

					leftIndex = i;
					break;
				}
			}

			// Couldn't place image
			if (leftIndex < 0) {
				placedAll = false;
				break;
			};
			// Placed image!

			// Save image position
			IndexPair finalLocation = pair;
			finalLocation.second = leftLocations[leftIndex];
			finalLocations.push_back(finalLocation);

			// Add collider at that position
			colliders.emplace_back((float)finalLocation.second.x,
								   (float)finalLocation.second.y,
								   (float)pair.second.x, 
								   (float)pair.second.y);

			// Erase the position so that no other images try to place here
			leftLocations.erase(leftLocations.begin() + leftIndex);

			// Add new available positions to place
			leftLocations.push_back(finalLocation.second + glt::vec2i{ 0, pair.second.y });
			leftLocations.push_back(finalLocation.second + glt::vec2i{ pair.second.x, 0 });

			// Prioritize positions to the left
			std::sort(leftLocations.begin(), leftLocations.end(), [](glt::vec2i a, glt::vec2i b) {
				return a.x < b.x;
			});
		}

		// Attempt to shrink rectangle as far as possible
		// Go from wide and low to narrow and high

		// If all images were placed
		if (placedAll) {
			// Move the right border as far left as possible
			currentWidth = leftLocations.back().x;

			// Save this layout if it's the best yet
			long bestArea = bestRectangle.w * bestRectangle.h;
			long currentArea = currentWidth * currentHeight;
			if (currentArea < bestArea) {
				bestRectangle.w = currentWidth;
				bestRectangle.h = currentHeight;
				bestRectangle.locations = finalLocations;
			}

			// Move the right border one pixel to the left
			currentWidth -= 1;
		}
		else {
			// Increase the height
			currentHeight += 1;

			// Try to make the rectangle as small as possible
			while (true) {
				// The rectangle's area can't be smaller than the total area of the images
				while (currentHeight * currentWidth < imagesArea) {
					currentHeight += 1;
				}

				// If the rectangle is larger than the smallest rectangle, there's no point in trying
				if (currentWidth * currentHeight > bestRectangle.w * bestRectangle.h) {
					currentWidth -= 1;
				}
				else {
					break;
				}
			}
		}
	}


	result = Image(bestRectangle.w, bestRectangle.h);

	std::vector<ImageRegion> regions(images.size());

	for (IndexPair& index : bestRectangle.locations)
	{
		result.blit(*images[index.first], index.second.x, index.second.y);

		regions[index.first].x = index.second.x;
		regions[index.first].y = index.second.y;
		regions[index.first].width = images[index.first]->getWidth();
		regions[index.first].height = images[index.first]->getHeight();
		regions[index.first].parentWidth = bestRectangle.w;
		regions[index.first].parentHeight = bestRectangle.h;
	}

	return std::move(regions);
}
