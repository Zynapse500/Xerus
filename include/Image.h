#pragma once



namespace xr {
	// Supported image formats
	enum ImageFormat {
		PNG,
		DONT_KNOW
	};


	// Represents a region in an image
	struct ImageRegion {
		int x, y;
		int width, height;

		int parentWidth, parentHeight;
	};


	// Decodes image data and stores them in a raw format
	class Image
	{
		// The raw pixel data
		std::vector<unsigned char> pixels;

		// Size of the image
		unsigned long width, height;

	public:

		// Create new empty image
		Image(int width = 0, int height = 0);

		// Decode image of specific type
		Image(ImageFormat format, unsigned char* data, size_t dataSize);

        // Construct from raw data
        Image(const std::vector<unsigned char>& data, int width, int height);
        Image(unsigned char* data, int width, int height);

		// Load and decode image from a file
		Image(const char* path);
		

		// Get the raw data of the image
		const unsigned char* data() const;

		// Get the size of the image
		int getWidth() const;
		int getHeight() const;
		


		// Paste another image into this one
		void blit(const Image& image, int x, int y);

	private:

		// Decodes an image
		void decode(ImageFormat format, unsigned char* data, size_t dataSize);


		// Attempts to derive the image format from the bytes making up the encoded image
		ImageFormat getFormatFromBytes(unsigned char* data, size_t size);

	};



	// Stitches multiple images together into one
	// Returns the location of each contained image
	std::vector<ImageRegion> stitchImages(Image& result, const std::vector<const Image*>& images);
	std::vector<ImageRegion> stitchImages(Image& result, const std::vector<Image>& images);

}




