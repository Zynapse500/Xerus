#include "stdafx.h"
#include <chrono>

#include "Window.h"

xr::Window::Window(int width, int height, const char * title, const WindowPreferences& preferences)
{
	this->initGLFW();
	this->create(width, height, title, preferences);
	this->loadGL();
	this->setup(preferences);
}

xr::Window::~Window()
{
	glfwTerminate();
}

bool xr::Window::isOpen()
{
	return !glfwWindowShouldClose(this->glfwHandle);
}

void xr::Window::close()
{
	glfwSetWindowShouldClose(this->glfwHandle, true);
}

void xr::Window::pollEvents()
{
	glfwPollEvents();
}

void xr::Window::swapBuffers()
{
	glfwSwapBuffers(this->glfwHandle);

	this->calculateLastFrameTime();
}

void xr::Window::calculateLastFrameTime()
{
	using namespace std::chrono;
	static time_point<high_resolution_clock > last = high_resolution_clock::now();
	time_point<high_resolution_clock > now = high_resolution_clock::now();

    auto dur = duration_cast<nanoseconds>(now - last);
	this->lastFrameTime = dur.count() / 1e9;
	last = now;
}

double xr::Window::getLastFrameTime()
{
	return this->lastFrameTime;
}


int xr::Window::getWidth() {
	return size.x;
}

int xr::Window::getHeight() {
	return size.y;
}

glt::vec2i xr::Window::getSize() {
	return size;
}

void xr::Window::setSize(int width, int height)
{
	glfwSetWindowSize(this->glfwHandle, width, height);
}

void xr::Window::setTitle(const char * title)
{
	glfwSetWindowTitle(this->glfwHandle, title);
}


void xr::Window::setFullscreen(bool fullscreen)
{
	this->fullscreen = fullscreen;

	// Size, position and monitor to use for fullscreen
	int width = this->preferredSize.x, height = this->preferredSize.y;
	int x = 0, y = 0;
	GLFWmonitor* monitor = nullptr;
	
	// Info about the preferred monitor
	const GLFWvidmode* vidmode = glfwGetVideoMode(this->preferredMonitor);

	if (fullscreen) {
		// Find the monitor's size
		if (this->fullscreenUseMaxResolution) {
			width = vidmode->width;
			height = vidmode->height;
		}

		monitor = this->preferredMonitor;
	}
	else {
		// Center window on monitor
		glfwGetMonitorPos(this->preferredMonitor, &x, &y);
		x += (vidmode->width - width) / 2;
		y += (vidmode->height - height) / 2;
	}

	glfwSetWindowMonitor(this->glfwHandle, monitor, x, y, width, height, GLFW_DONT_CARE);
    glViewport(0, 0, width, height);

	glfwSwapInterval(this->verticalSync);
    this->size = { width, height };
}

void xr::Window::toggleFullscreen()
{
	this->setFullscreen(!this->fullscreen);
}

bool xr::Window::getFullscreen()
{
	return this->fullscreen;
}

void xr::Window::setVerticalSync(bool vsync)
{
	this->verticalSync = vsync;
	glfwSwapInterval(vsync);
}

bool xr::Window::getVerticalSync()
{
	return this->verticalSync;
}

bool xr::Window::getMouseButton(int button)
{
	return static_cast<bool>(glfwGetMouseButton(this->glfwHandle, button));
}

bool xr::Window::getKey(int key)
{
	return static_cast<bool>(glfwGetKey(this->glfwHandle, key));
}

glt::vec2i xr::Window::getCursorPosition()
{
	double x, y;
	glfwGetCursorPos(this->glfwHandle, &x, &y);
	return {static_cast<int>(x), static_cast<int>(y)};
}

glt::vec2f xr::Window::windowToScreen(glt::vec2f window)
{
	return (2.f * window / glt::vec2f(size) - 1.f) * glt::vec2f(1, -1);
}




void xr::Window::initGLFW()
{
	static bool initialized = false;

	if (!initialized) {
		if (!glfwInit()) {
			throw std::runtime_error("Failed to init GLFW!");
			initialized = true;
		}
	}
}

void xr::Window::create(int width, int height, const char * title, const WindowPreferences& preferences)
{
	// Set the preferred monitor for the window
	this->preferredMonitor = glfwGetPrimaryMonitor();
	this->fullscreenUseMaxResolution = preferences.overrideFullscrenSize;
	this->preferredSize = { width, height };

	// Set OpenGL version
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, preferences.contextVersionMajor);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, preferences.contextVersionMinor);

	// Multisampling samples
	glfwWindowHint(GLFW_SAMPLES, preferences.samples);

    GLFWmonitor* monitor = nullptr;


    if (preferences.fullscreen) {
        monitor = this->preferredMonitor;
        if (fullscreenUseMaxResolution) {
            const GLFWvidmode* vidmode = glfwGetVideoMode(this->preferredMonitor);
            width = vidmode->width;
            height = vidmode->height;
        }
    }



	this->glfwHandle = glfwCreateWindow(width, height, title, monitor, nullptr);

    if (!glfwHandle) {
        throw std::runtime_error("Failed to create GLFW window");
    }

    this->windows[this->glfwHandle] = this;
    this->size = { width, height };

    setFullscreen(preferences.fullscreen);
}

void xr::Window::loadGL()
{
	glfwMakeContextCurrent(this->glfwHandle);
	xr::loadOpenGL();
}

void xr::Window::setup(WindowPreferences preferences)
{
	// Callbacks
	this->windowCallbacks = preferences.callbacks;
	this->setupCallbacks();

	// Vertical sync
	glfwSwapInterval(preferences.vsync);
	this->verticalSync = preferences.vsync;

	// Enable multisampling
	if (preferences.samples > 0) {
		glEnable(GL_MULTISAMPLE);
	}

	// Enable depth culling
	// glEnable(GL_DEPTH_TEST);


	// Enable transparency
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}






////////////////////////////////////////////////////
// CALLBACKS ///////////////////////////////////////
////////////////////////////////////////////////////

std::map<GLFWwindow*, xr::Window*> xr::Window::windows;

xr::Window * xr::Window::getWindow(GLFWwindow * window)
{
	auto windowIterator = windows.find(window);

	if (windowIterator != windows.end()) {
		return (*windowIterator).second;
	}

	return nullptr;
}

xr::WindowCallbacks* xr::Window::getCallbacks(GLFWwindow * window)
{
	if (Window* wnd = getWindow(window)) {
		return &wnd->windowCallbacks;
	}

	return nullptr;
}

void xr::Window::setupCallbacks()
{
	// Keys
	glfwSetKeyCallback(this->glfwHandle, keyCallback);

	// Window size
	glfwSetWindowSizeCallback(this->glfwHandle, sizeCallback);

	// Window position
	glfwSetWindowPosCallback(this->glfwHandle, moveCallback);

	// Window refresh
	glfwSetWindowRefreshCallback(this->glfwHandle, refreshCallback);


	// Mouse button
	glfwSetMouseButtonCallback(this->glfwHandle, mouseButtonCallback);

	// Mouse move
	glfwSetCursorPosCallback(this->glfwHandle, mousePositionCallback);
}

void xr::Window::keyCallback(GLFWwindow * window, int key, int scancode, int action, int mods)
{
	if (WindowCallbacks* callbacks = getCallbacks(window)) {
		if (action == GLFW_PRESS) {
			if (callbacks->keyPressedCallback) {
				callbacks->keyPressedCallback(key);
			}
		} else if(action == GLFW_RELEASE) {
			if (callbacks->keyReleasedCallback) {
				callbacks->keyReleasedCallback(key);
			}
		}
	}
}

void xr::Window::sizeCallback(GLFWwindow * window, int width, int height)
{
	if (Window* wnd = getWindow(window)) {
		glViewport(0, 0, width, height);

		wnd->size.x = width;
		wnd->size.y = height;

		WindowCallbacks* callbacks = &wnd->windowCallbacks;
		if (callbacks->windowResizedCallback) {
			callbacks->windowResizedCallback(width, height);
		}
	}
}

void xr::Window::moveCallback(GLFWwindow * window, int x, int y)
{
	if (WindowCallbacks* callbacks = getCallbacks(window)) {
		if (callbacks->windowMovedCallback) {
			callbacks->windowMovedCallback(x, y);
		}
	}
}

void xr::Window::refreshCallback(GLFWwindow * window)
{
	if (WindowCallbacks* callbacks = getCallbacks(window)) {
		if (callbacks->windowRefreshedCallback) {
			callbacks->windowRefreshedCallback();
		}
	}
}

void xr::Window::mouseButtonCallback(GLFWwindow* window, int button, int action, int mods) {
	if (Window* wnd = getWindow(window)) {

		int x = wnd->cursorPosition.x;
		int y = wnd->cursorPosition.y;

		WindowCallbacks* callbacks = &wnd->windowCallbacks;
		if (action == GLFW_PRESS) {
			if (callbacks->mousePressedCallback) {
				callbacks->mousePressedCallback(button, x, y);
			}
		}
		else if (action == GLFW_RELEASE) {
			if (callbacks->mouseReleasedCallback) {
				callbacks->mouseReleasedCallback(button, x, y);
			}
		}
	}
}

void xr::Window::mousePositionCallback(GLFWwindow* window, double x, double y) {
	if (Window* wnd = getWindow(window)) {

		wnd->cursorPosition.x = int(x);
		wnd->cursorPosition.y = int(y);

		WindowCallbacks* callbacks = &wnd->windowCallbacks;
		if (callbacks->mouseMovedCallback) {
			callbacks->mouseMovedCallback(int(x), int(y));
		}
	}
}

