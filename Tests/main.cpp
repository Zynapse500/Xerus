#include <iostream>

#include "Xerus.h"

#include <string>

void onKeyPressed(int key);
void onKeyReleased(int key);
void onWindowResize(int width, int height);
void onWindowRefresh();

std::function<void()> render;
std::function<void(double)> update;

int main() {
	try
	{
		xr::WindowPreferences prefs;
		prefs.contextVersionMajor = 3;
		prefs.contextVersionMinor = 3;

		prefs.vsync = false;
		prefs.samples = 8;

		prefs.callbacks.keyPressedCallback = onKeyPressed;
		prefs.callbacks.keyReleasedCallback = onKeyReleased;
		prefs.callbacks.windowResizedCallback = onWindowResize;
		prefs.callbacks.windowRefreshedCallback = onWindowRefresh;


		// Load image from disk and create texture
		xr::Image dirt("resources/dirt.png");
		xr::Image stone("resources/stone.png");
		xr::Image atlas;

		std::vector<xr::ImageRegion> imageRegions = xr::stitchImages(atlas, {
			&xr::Image("resources/dirt.png"),
			&xr::Image("resources/stone.png"),
		});


		xr::Window window(512, 512, "Xerus Engine", prefs);
		xr::Renderer renderer;
		xr::DrawBuffer drawBuffer(true);

		xr::Texture texture(atlas);
		texture.setMinMagFilter(GL_NEAREST, GL_NEAREST);

		std::vector<xr::TextureRegion> textureRegions;

		for each (auto& region in imageRegions) {
			textureRegions.emplace_back(region, texture);
		}

		// Current texture region to draw
		xr::TextureRegion* currentRegion = &textureRegions.front();

		// Time since program start
		double elapsed = 0;

		int tileCount = 0;

		// Render function
		render = [&]() {
			// Clear screen with color
			renderer.clear(0.2f, 0.2f, 0.2f, 1.0f);

			// Clear previous commands
			drawBuffer.clear();

			// Set the current camera
			glm::mat4 view = glm::lookAt(2.5f * glm::vec3{ cos(elapsed / 2), 0.5, sin(elapsed / 2) }, { 0, 0, 0 }, { 0, 1, 0 });
			float w = (float)window.getWidth();
			float h = (float)window.getHeight();

			glm::mat4 proj = glm::perspective(glm::radians(70.f), w / h, 0.1f, 100.f);

			proj = glm::ortho(0.0f, w, 0.0f, h);

			drawBuffer.setCameraMatrix(proj);

			
			// Set the current texture
			drawBuffer.setTexture(*currentRegion);

			// Set the color
			// drawBuffer.setFillColor(float(sin(elapsed)) / 2.f + 0.5f, 0.2f, float(cos(elapsed)) / 2.f + 0.5f);

			// Draw textured quad

			// drawBuffer.drawRect(0.0f, 0.0f, w, h);
			tileCount = 0;
			int textureRegionCount = textureRegions.size();
			int tileSize = 16;

			srand(1337);
			for (float x = 0; x < w; x += tileSize)
			{
				for (int y = 0; y < h; y += tileSize)
				{
					tileCount++;
					drawBuffer.setTexture(textureRegions[rand() % textureRegionCount]);
					drawBuffer.drawRect(x, y, tileSize, tileSize);
				}
			}
			

			// Encode a render sequence
			const xr::RendererSequence& sequence = drawBuffer.encodeSequence();

			// Submit render sequence to renderer
			renderer.submit(sequence);

			window.swapBuffers();
		};

		update = [&](double deltaTime) {
			elapsed += deltaTime;

			// Calculate framerate
			{
				static double elapsedTime = 0;
				static int frames = 0;
				frames++;
				elapsedTime += deltaTime;

				if (elapsedTime > 0.5) {
					int fps = int(round(frames / elapsedTime));
					frames = 0; elapsedTime = 0;

					window.setTitle(("Xerus Engine @ " + std::to_string(fps) + " : " + std::to_string(tileCount)).c_str());
				}
			}

			// Update the region to draw
			int region = fmod(elapsed, textureRegions.size());
			
			// Changed region
			static int lastRegion = 0;
			if (region != lastRegion) {
				lastRegion = region;
				currentRegion = &textureRegions[region];
			}
		};


		// Main loop
		while (window.isOpen()) {
			window.pollEvents();
			double deltaTime = window.getLastFrameTime();
			update(deltaTime);
			render();
		}
	}

	catch (const std::runtime_error err)
	{
		printf("%s", err.what());
		getchar();
	}

	return 0;
}

void onKeyPressed(int key) {
	std::cout << "Pressed: " << (char)key << std::endl;
}

void onKeyReleased(int key) {
	std::cout << "Released: " << (char)key << std::endl;
}

void onWindowResize(int width, int height)
{
	// printf("Size: (%d, %d)\n", width, height);
}

void onWindowRefresh()
{
	render();
}



