#pragma once

#include <map>
#include <functional>

#include "OpenGL.h"
#include <GLFW/glfw3.h>

typedef int KeyType;


namespace xr {


	// Lists all callbacks for a window
	struct WindowCallbacks {
		std::function<void(KeyType)> keyPressedCallback;
		std::function<void(KeyType)> keyReleasedCallback;
		std::function<void(int, int)> windowResizedCallback;
		std::function<void(int, int)> windowMovedCallback;
		std::function<void()> windowRefreshedCallback;
	};

	
	// Preferences for creating a window
	struct WindowPreferences {
		bool vsync = true;
		int samples = 0;

		int contextVersionMajor = 3;
		int contextVersionMinor = 3;

		WindowCallbacks callbacks;
	};


	class Window
	{
		// Handle to the GLFW window
		GLFWwindow* glfwHandle;

		// The time between the two most recent calls to swapBuffers
		double lastFrameTime;

		// This size of this window
		glm::ivec2 size;

	public:

		// Create a window
		Window(int width, int height, const char* title, const WindowPreferences& preferences = WindowPreferences());


		// Return true if the window is currently open
		bool isOpen();


		// Poll the window for events
		void pollEvents();

		// Swap the window buffers
		void swapBuffers();

		// Calculate the time between the two most recent calls to swapBuffers
		void calculateLastFrameTime();

		// Return the time between the two most recent calls to swapBuffers
		double getLastFrameTime();


		// Return the size of the window
		int getWidth();
		int getHeight();
		glm::ivec2 getSize();

		// Set the size of the window
		void setSize(int width, int height);

		// Sets this window's title
		void setTitle(const char* title);

	private:

		// Initializes GLFW
		void initGLFW();

		// Create a window with a specific size and title
		void create(int width, int height, const char* title, const WindowPreferences& preferences);

		
		// Load the opengl bindings
		void loadGL();

		// Set the settings for the window and OpenGL
		void setup(WindowPreferences preferences);



		////////////////////////////////////////////////////
		// CALLBACKS ///////////////////////////////////////
		////////////////////////////////////////////////////

		// Each window has it's own list of callbacks
		WindowCallbacks windowCallbacks;


		// Map each glfwWindow to a Window wrapper
		static std::map<GLFWwindow*, Window*> windows;


		// Return the GLFWwindow's Window wrapper
		static Window* getWindow(GLFWwindow* window);

		// Return the window's callback
		static WindowCallbacks* getCallbacks(GLFWwindow* window);


		// Set all the callbacks
		void setupCallbacks();



		// Key was pressed
		static void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods);

		// Window was resized
		static void sizeCallback(GLFWwindow* window, int width, int height);

		// Window was moved
		static void moveCallback(GLFWwindow* window, int x, int y);

		// Window was refreshed
		static void refreshCallback(GLFWwindow* window);
	};
}
