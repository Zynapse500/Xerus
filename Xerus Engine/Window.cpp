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

bool xr::Window::isOpen()
{
	return !glfwWindowShouldClose(this->glfwHandle);
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
	static auto last = high_resolution_clock::now();
	auto now = high_resolution_clock::now();

	this->lastFrameTime = (now - last).count() / 1e9;
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

glm::ivec2 xr::Window::getSize() {
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
	// Set OpenGL version
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, preferences.contextVersionMajor);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, preferences.contextVersionMinor);

	// Multisampling samples
	glfwWindowHint(GLFW_SAMPLES, preferences.samples);

	this->glfwHandle = glfwCreateWindow(width, height, title, nullptr, nullptr);

	if (!glfwHandle) {
		throw std::runtime_error("Failed to create GLFW window");
	}

	this->windows[this->glfwHandle] = this;
	this->size = { width, height };
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

	// Initialize callbacks
	sizeCallback(this->glfwHandle, size.x, size.y);

	// Vertical sync
	glfwSwapInterval(preferences.vsync);

	// Enable multisampling
	if (preferences.samples > 0) {
		glEnable(GL_MULTISAMPLE);
	}

	// Enable depth culling
	glEnable(GL_DEPTH_TEST);

	// Enable back face culling
	/*
	glFrontFace(GL_CCW);
	glCullFace(GL_BACK);
	glEnable(GL_CULL_FACE);
	*/
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
	if (Window* wnd = getWindow(window); wnd != nullptr) {
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
}

void xr::Window::keyCallback(GLFWwindow * window, int key, int scancode, int action, int mods)
{
	if (WindowCallbacks* callbacks = getCallbacks(window); callbacks != nullptr) {
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
	if (Window* wnd = getWindow(window); wnd != nullptr) {
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
	if (WindowCallbacks* callbacks = getCallbacks(window); callbacks != nullptr) {
		if (callbacks->windowMovedCallback) {
			callbacks->windowMovedCallback(x, y);
		}
	}
}

void xr::Window::refreshCallback(GLFWwindow * window)
{
	if (WindowCallbacks* callbacks = getCallbacks(window); callbacks != nullptr) {
		if (callbacks->windowRefreshedCallback) {
			callbacks->windowRefreshedCallback();
		}
	}
}


