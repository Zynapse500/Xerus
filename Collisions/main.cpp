
#include <Xerus.h>

void onKeyPressed(int key);

void onMousePressed(int button, int x, int y);
void onMouseReleased(int button, int x, int y);
void onMouseMoved(int x, int y);

void onWindowResized(int width, int height);

xr::Window* window = nullptr;

xr::WindowPreferences createPreferences() {
	xr::WindowPreferences prefs;
	prefs.contextVersionMajor = 3;
	prefs.contextVersionMinor = 3;

	prefs.vsync = false;
	prefs.samples = 8;
	prefs.fullscreen = false;

	prefs.callbacks.keyPressedCallback = onKeyPressed;

	prefs.callbacks.mousePressedCallback = onMousePressed;
	prefs.callbacks.mouseReleasedCallback = onMouseReleased;
	prefs.callbacks.mouseMovedCallback = onMouseMoved;

	prefs.callbacks.windowResizedCallback = onWindowResized;

	return prefs;
}



// Camera 
xr::OrthographicCamera camera(1280, 720);

// Converts screen coordinates to world coordinates
glm::ivec2 mouseToWorld(int x, int y);
glm::ivec2 mouseToWorld(glm::ivec2 mouse) { return mouseToWorld(mouse.x, mouse.y); }





// Box
xr::AABB box{ {640, 360}, {400, 200} };

// Cirlce
xr::Circle circle{ { 640, 360 }, 100 };
xr::Circle sweepCircle{ {0, 0}, 25.f };

// Line
struct Line {
	glm::vec2 start, end;
} line = { {300, 200}, {500, 500} };



int main() {
	window = new xr::Window(1280, 720, "Collisions", createPreferences());
	xr::Renderer renderer;
	xr::RenderBatch renderBatch;

	float elapsed = 0;

	while (window->isOpen())
	{
		float deltaTime = window->getLastFrameTime();
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


		renderer.clear(0.0, 0.0, 0.0, 1.0);

		renderBatch.clear();


		float w = (float)window->getWidth();
		float h = (float)window->getHeight();
		
		
		renderBatch.setCamera(camera);



		if (window->getMouseButton(GLFW_MOUSE_BUTTON_LEFT)) {
			line.start = window->getCursorPosition();
		}
		if (window->getMouseButton(GLFW_MOUSE_BUTTON_RIGHT)) {
			line.end = window->getCursorPosition();
		}

		
		renderBatch.setFillColor(0.1, 0.1, 0.7);
		renderBatch.fillRect(box.center - box.size / 2.f, box.size);

		renderBatch.setFillColor(0.1, 0.5, 0.4);
		renderBatch.fillCircle(circle.center, circle.radius, 64);



		// Line intersect box
		if (xr::Hit hit = box.intersects(line.start, line.end)) {
			renderBatch.setFillColor(1, 1, 1);
			renderBatch.drawLine(line.start, line.end);

			renderBatch.setFillColor(1, 0, 0);
			renderBatch.drawLine(line.start, hit.point);
		}
		else {
			renderBatch.setFillColor(1, 0, 0);
			renderBatch.drawLine(line.start, line.end);
		}



		// Box sweep circle

		//if (xr::Hit hit = box.sweep(sweepCircle, line.end - line.start)) {
		//	renderBatch.setFillColor(1, 0, 0);
		//	renderBatch.drawLine(line.start, hit.point, 1);

		//	renderBatch.setFillColor(1, 1, 1);
		//	renderBatch.drawLine(hit.point, line.end, 1);

		//	renderBatch.setFillColor(1, 0, 1);
		//	renderBatch.fillCircle(hit.point, sweepCircle.radius);

		//	renderBatch.setFillColor(0, 1, 1);
		//	renderBatch.drawLine(hit.point, hit.point + 20.f * hit.normal, 2);
		//}
		//else {
		//	renderBatch.setFillColor(1, 0, 1);
		//	renderBatch.fillCircle(line.end, sweepCircle.radius);

		//	renderBatch.setFillColor(1, 0, 0);
		//	renderBatch.drawLine(line.start, line.end, 1);
		//}
		


		renderer.submit(renderBatch);



		window->swapBuffers();
		window->pollEvents();
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
}

void onMousePressed(int button, int x, int y)
{
	
}

void onMouseReleased(int button, int x, int y)
{

}

void onMouseMoved(int x, int y)
{
}

void onWindowResized(int width, int height)
{
	camera.setProjection(width, height);
}

glm::ivec2 mouseToWorld(int x, int y)
{
	glm::vec2 screen = window->windowToScreen({ x, y });

	glm::vec2 world = camera.screenToWorld(screen);
	return world;
}


