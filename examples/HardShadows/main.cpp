
#include <Xerus.h>

void onKeyPressed(int key);

void onMousePressed(int button, int x, int y);
void onMouseReleased(int button, int x, int y);
void onMouseMoved(int x, int y);


xr::Window* window = nullptr;

xr::WindowPreferences createPreferences() {
	xr::WindowPreferences prefs;
	prefs.contextVersionMajor = 3;
	prefs.contextVersionMinor = 3;

	prefs.vsync = true;
	prefs.samples = 8;
	prefs.fullscreen = false;

	prefs.callbacks.keyPressedCallback = onKeyPressed;

	prefs.callbacks.mousePressedCallback = onMousePressed;
	prefs.callbacks.mouseReleasedCallback = onMouseReleased;
	prefs.callbacks.mouseMovedCallback = onMouseMoved;

	return prefs;
}


struct Wall {
	glt::vec2f start;
	glt::vec2f end;
};

std::vector<Wall> walls;

glt::vec2f* wallPaint;


glt::vec2f lightPosition;
glt::vec2f* dragLight;


void drawShadows(glt::vec2f light, const std::vector<Wall>& walls, float shadowLength, xr::RenderBatch* batch);


int main() {
	xr::Window window(512, 512, "Xerus Engine", createPreferences());
	::window = &window;
	xr::Renderer renderer;
	xr::RenderBatch renderBatch;
	xr::RenderBatch shadowBatch;

	lightPosition = { window.getWidth() / 2, window.getHeight() / 2 };

	double elapsed = 0;

	while (window.isOpen())
	{
		double deltaTime = window.getLastFrameTime();
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
			}
		}


		renderer.clear(0.1, 0.1, 0.1, 1.0);

		renderBatch.clear();
		shadowBatch.clear();
		
		// Set the current camera
		float w = (float)window.getWidth();
		float h = (float)window.getHeight();

		glt::mat4f proj = glt::orthographic(0.0f, w, h, 0.0f);
		renderBatch.setCamera(proj);
		shadowBatch.setCamera(proj);


        // Draw background
        renderBatch.setFillColor(1, 0.4, 0);
        renderBatch.fillRect(0, 0, w, h);


        // Draw walls
		renderBatch.setFillColor(1.0, 1.0, 1.0);
		int wallCount = walls.size();
		for (int i = 0; i < wallCount; i++){
			renderBatch.drawLine(walls[i].start, walls[i].end, 2);
		}

        // Draw light
        renderBatch.setFillColor(1.0, 1.0, 0.0);
        renderBatch.fillCircle(lightPosition, 10);


		// Draw shadows
		// their length should enclose the whole window
		shadowBatch.setFillColor(0.05, 0.05, 0.05);
		drawShadows(lightPosition, walls, w * h, &shadowBatch);
		// shadowBatch.fillTriangles(computeShadow(lightPosition, walls, w * h));


		/*
		1. Enable stencil testing and write ones to drawn fragments
		2. Draw shadows
		3. Pass stencil test if stencil value is one
		4. Draw scene
		5. Disable stencil testing
		*/

		glEnable(GL_STENCIL_TEST);

		// Mark drawn stencils with 1
		glStencilFunc(GL_ALWAYS, 1, 0xFF);
		glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);
		glStencilMask(0xFF);
		glDepthMask(GL_FALSE);
		glColorMask(0, 0, 0, 0);

		// Clear stencil buffer
		glClear(GL_STENCIL_BUFFER_BIT);

		// Draw shadows
		renderer.submit(shadowBatch);


		// Pass if stencil is 1
		glStencilFunc(GL_NOTEQUAL, 1, 0xFF);
		glStencilMask(0x00);
		glDepthMask(GL_TRUE);
		glColorMask(1, 1, 1, 1);


		// Draw scene
		renderer.submit(renderBatch);

		glDisable(GL_STENCIL_TEST);

		window.swapBuffers();
		window.pollEvents();
	}
}



void onKeyPressed(int key)
{
	if (key == GLFW_KEY_ESCAPE) {
		window->close();
	}

	if (key == GLFW_KEY_F11) {
		window->toggleFullscreen();
	}

	if (key == GLFW_KEY_R) {
		walls.clear();
	}
}

void onMousePressed(int button, int x, int y)
{
	if (button == GLFW_MOUSE_BUTTON_LEFT) {
		walls.push_back({ {x, y}, {x, y} });
		wallPaint = &walls.back().end;
	}
	if (button == GLFW_MOUSE_BUTTON_RIGHT) {
		dragLight = &lightPosition;
	}
}

void onMouseReleased(int button, int x, int y)
{
	if (button == GLFW_MOUSE_BUTTON_LEFT) {
		wallPaint = nullptr;
	}
	if (button == GLFW_MOUSE_BUTTON_RIGHT) {
		dragLight = nullptr;
	}
}

void onMouseMoved(int x, int y)
{
	if (wallPaint) {
		wallPaint->x = x;
		wallPaint->y = y;
	}

	if (dragLight) {
		dragLight->x = x;
		dragLight->y = y;
	}
}

void drawShadows(glt::vec2f light, const std::vector<Wall>& walls, float shadowLength, xr::RenderBatch* batch)
{
	std::vector<glt::vec2f> shadowPoints;

	int wallCount = walls.size();
	for (int i = 0; i < wallCount; i++)
	{
		// Find the direction of the shadow from the light to the corners
		glt::vec2f dStart = glt::normalize(walls[i].start - light);
		glt::vec2f dEnd = glt::normalize(walls[i].end - light);

		// Cast shadow from corners
		glt::vec2f farStart = walls[i].start + dStart * shadowLength;
		glt::vec2f farEnd = walls[i].end + dEnd * shadowLength;

		// Find line perpendicular to wall and light
		glt::vec2f farPerp;

		// Translate origin to wall start
		glt::vec2f wallEnd = walls[i].end - walls[i].start;
		glt::vec2f lightPos = light - walls[i].start;

		// Project the light's position onto the wall
		glt::vec2f wallDir = glt::normalize(wallEnd);

		//  a.b = |a||b| cos 0 = |lightPos| cos 0
		float proj = glt::dot(lightPos, wallDir);

		// Proj has to lie on wall
		if (proj <= 0 || proj >= glt::length(wallEnd)) {
			// Place in the middle of the others
			farPerp = (farStart + farEnd) / 2.f;
		}
		else {
			// Find the projected point on the wall
			glt::vec2f perpPoint = walls[i].start + proj * wallDir;

			// Find perpendicular direction
			glt::vec2f dPerp = glt::normalize(perpPoint - light);

			// Find the far perpendicular point
			farPerp = perpPoint + dPerp * shadowLength;
		}


		// Add all points into triangle mesh

		shadowPoints.push_back(walls[i].start);
		shadowPoints.push_back(farStart);
		shadowPoints.push_back(farPerp);

		shadowPoints.push_back(walls[i].start);
		shadowPoints.push_back(farPerp);
		shadowPoints.push_back(farEnd);

		shadowPoints.push_back(walls[i].start);
		shadowPoints.push_back(farEnd);
		shadowPoints.push_back(walls[i].end);
	}

	batch->fillTriangles(shadowPoints);
}




