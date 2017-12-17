#include <iostream>

#include "Xerus.h"

#include <string>

void onKeyPressed(int key);
void onKeyReleased(int key);
void onWindowResize(int width, int height);
void onWindowRefresh();

std::function<void()> render;
std::function<void(double)> update;
std::function<void(int)> keyPressed;


// Time since program start
double elapsed = 0;

double animationStart = 0;

int main() {
	try
	{
		xr::WindowPreferences prefs;
		prefs.contextVersionMajor = 3;
		prefs.contextVersionMinor = 3;
		
		prefs.vsync = true;
		prefs.samples = 8;
		prefs.fullscreen = false;

		prefs.callbacks.keyPressedCallback = onKeyPressed;
		prefs.callbacks.keyReleasedCallback = onKeyReleased;
		prefs.callbacks.windowResizedCallback = onWindowResize;
		prefs.callbacks.windowRefreshedCallback = onWindowRefresh;


		// Load image from disk and create texture atlas
		xr::Image dirt("resources/dirt.png");
		xr::Image stone("resources/stone.png");
		xr::Image atlas;

		std::vector<xr::ImageRegion> imageRegions = xr::stitchImages(atlas, {
			&xr::Image("resources/dirt.png"),
			&xr::Image("resources/stone.png"),
		});


		xr::Window window(512, 512, "Xerus Engine", prefs);
		xr::Renderer renderer;
		xr::RenderBatch renderBatch;

		xr::Texture texture(atlas);
		texture.setMinMagFilter(GL_NEAREST, GL_NEAREST);

		std::vector<xr::TextureRegion> textureRegions;

		for each (auto& region in imageRegions) {
			textureRegions.emplace_back(region, texture);
		}

		// Current texture region to draw
		xr::TextureRegion* currentRegion = &textureRegions.front();


		int tileCount = 0;

		// Render function
		render = [&]() {
			// Clear screen with color
			renderer.clear(0.2f, 0.2f, 0.2f, 1.0f);

			// Clear previous commands
			renderBatch.clear();

			// Set the current camera
			glm::mat4 view = glm::lookAt(2.5f * glm::vec3{ cos(elapsed / 2), 0.5, sin(elapsed / 2) }, { 0, 0, 0 }, { 0, 1, 0 });
			float w = (float)window.getWidth();
			float h = (float)window.getHeight();

			glm::mat4 proj = glm::perspective(glm::radians(70.f), h == 0 ? 0 : w / h, 0.1f, 100.f);

			proj = glm::ortho(0.0f, w, 0.0f, h);

			renderBatch.setCameraMatrix(proj);

			
			// Set the current texture
			// renderBatch.setTexture(*currentRegion);
			renderBatch.setFillColor(0.5, 0.5, 0.5);
			
			// Interpolate
			float t = (elapsed - animationStart);

			float d = 5.;

			// Go from 0 to 1 in in 'd' seconds
			auto p = [d](float t) {
				float p = fmod(t, d) / d;

				// Reverse direction at half time
				if (p < 0.5) {
					p = 2 * p;
				}
				else {
					p = 2 - 2 * p;
				}

				return p;
			};


			float tileSize = 4;
			int tileCount = 30'000;
			for (int i = 0; i < tileCount; i++)
			{
				float xOffset = d * i / 1.0 / tileCount;
				float yOffset = 2 * xOffset + d / 3;

				float x = xr::smootherLerp(p(t + xOffset), 0.f, w - tileSize);
				float y = xr::smootherLerp(p(t + yOffset), 0.f, h - tileSize);

				// Draw textured quad
				renderBatch.drawRect(x, y, tileSize, tileSize);
			}

			
			// Submit drawBuffer to renderer
			renderer.submit(renderBatch);

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

					printf("fps: %d\n", fps);
					// window.setTitle(("Xerus Engine @ " + std::to_string(fps) + " : " + std::to_string(tileCount)).c_str());
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

		keyPressed = [&](int key) {
			if (key == GLFW_KEY_R) {
				animationStart = elapsed;
			} else if (key == GLFW_KEY_ESCAPE) {
				window.close();
			}
			else if (key == GLFW_KEY_F11) {
				window.toggleFullscreen();
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
	keyPressed(key);
}

void onKeyReleased(int key) {
}

void onWindowResize(int width, int height)
{
}

void onWindowRefresh()
{
	render();
}



