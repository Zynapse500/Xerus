#pragma once

#include <map>
#include <functional>

#include "OpenGL.h"
#include <GLFW/glfw3.h>

typedef int KeyType;
typedef int MouseButtonType;

namespace xr {


	// Lists all callbacks for a window
	struct WindowCallbacks {
		std::function<void(KeyType)> keyPressedCallback;
		std::function<void(KeyType)> keyReleasedCallback;
		std::function<void(int, int)> windowResizedCallback;
		std::function<void(int, int)> windowMovedCallback;
		std::function<void()> windowRefreshedCallback;

		std::function<void(MouseButtonType, int, int)> mousePressedCallback;
		std::function<void(MouseButtonType, int, int)> mouseReleasedCallback;

		std::function<void(int, int)> mouseMovedCallback;
	};

	
	// Preferences for creating a window
	struct WindowPreferences {
		// Determines if vertical sync is enabled
		bool vsync = true;

		// The number of multisampling samples
		int samples = 0;

		// Determines if window is opened in fullscreen mode
		bool fullscreen = false;

		// Determines the window size be overriden to the monitor's
		// resolution if fullscreen is enabled
		bool overrideFullscrenSize = true;


		// The OpenGL context version
		int contextVersionMajor = 3;
		// The OpenGL context version
		int contextVersionMinor = 3;


		// The callbacks of the window
		WindowCallbacks callbacks;
	};


	class Window
	{
		// Handle to the GLFW window
		GLFWwindow* glfwHandle;

		// The time between the two most recent calls to swapBuffers
		double lastFrameTime;

		// This size of this window
		glt::vec2i size;

		// The preferred size of the window
		glt::vec2i preferredSize;


		// The position of the cursor
		glt::vec2i cursorPosition;


		// Should vsync be enabled?
		bool verticalSync;

		// False if window is in windowed mode, true if in fullscreen
		bool fullscreen;

		// Should fullscreen use monitor's full resolution
		bool fullscreenUseMaxResolution;

		// The preferred monitor for this window
		GLFWmonitor* preferredMonitor;

	public:

		// Create a window
		Window(int width, int height, const char* title, const WindowPreferences& preferences = WindowPreferences());

		// Destroy this window
		~Window();


		// Return true if the window is currently open
		bool isOpen();

		// Close this window
		void close();


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
		glt::vec2i getSize();

		// Set the size of the window
		void setSize(int width, int height);

		// Sets this window's title
		void setTitle(const char* title);


		// Makes this window fullscreen if true, windowed if false
		void setFullscreen(bool fullscreen);
		void toggleFullscreen();

		// Return false if this window is in windowed mode, true if in fullscreen
		bool getFullscreen();


		// Enables or disables vsync
		void setVerticalSync(bool vsync);

		// Gets the current state of vsync
		bool getVerticalSync();


		// Return true if the button is pressed
		bool getMouseButton(int button);

		// Return true if the key is pressed
		bool getKey(int key);

		// Return the position of the cursor
		glt::vec2i getCursorPosition();

		// Converts window client space [0, size] to screen space [-1, 1]
		glt::vec2f windowToScreen(glt::vec2f window);

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


		// Mouse button was pressed
		static void mouseButtonCallback(GLFWwindow* window, int button, int action, int mods);

		// Mouse was moved
		static void mousePositionCallback(GLFWwindow* window, double x, double y);
	};
}
