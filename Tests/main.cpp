#include <iostream>

#include "Xerus.h"

#include <string>

void onKeyPressed(int key);
void onKeyReleased(int key);
void onWindowResize(int width, int height);
void onWindowRefresh();

void onMousePressed(int button, int x, int y);
void onMouseReleased(int button, int x, int y);
void onMouseMoved(int x, int y);

std::function<void()> render;
std::function<void(double)> update;
std::function<void(int)> keyPressed;


// Time since program start
double elapsed = 0;

double animationStart = 0;


// Points in polygon
std::vector<glm::vec2> points;
glm::vec2* grabbedPoint = nullptr;


int main() {
	try
	{
		xr::WindowPreferences prefs;
		prefs.contextVersionMajor = 3;
		prefs.contextVersionMinor = 3;
		
		prefs.vsync = false;
		prefs.samples = 8;
		prefs.fullscreen = false;

		prefs.callbacks.keyPressedCallback = onKeyPressed;
		prefs.callbacks.keyReleasedCallback = onKeyReleased;
		prefs.callbacks.windowResizedCallback = onWindowResize;
		prefs.callbacks.windowRefreshedCallback = onWindowRefresh;

		prefs.callbacks.mousePressedCallback = onMousePressed;
		prefs.callbacks.mouseReleasedCallback = onMouseReleased;
		prefs.callbacks.mouseMovedCallback = onMouseMoved;


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

			proj = glm::ortho(0.0f, w, h, 0.0f);
			renderBatch.setCameraMatrix(proj);

			
			renderBatch.clearTexture();

			renderBatch.setFillColor(1.0, 1.0, 1.0);
			renderBatch.fillTriangleFan(points);

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

void onMousePressed(int button, int x, int y)
{
	if (button == GLFW_MOUSE_BUTTON_LEFT) {
		points.push_back({ x, y });
	}
	if (button == GLFW_MOUSE_BUTTON_RIGHT) {
		int nearest = 0;

		for (int i = 0; i < points.size(); i++)
		{
			if (glm::distance(points[i], { x, y }) < glm::distance(points[nearest], { x, y })) {
				nearest = i;
			}
		}

		grabbedPoint = &points[nearest];
	}
}

void onMouseReleased(int button, int x, int y)
{
	if (button == GLFW_MOUSE_BUTTON_RIGHT) {
		grabbedPoint = nullptr;
	}
}

void onMouseMoved(int x, int y)
{
	if (grabbedPoint) {
		grabbedPoint->x = x;
		grabbedPoint->y = y;
	}
}



