#pragma once

#include "Renderer.h"
#include "Window.h"

#include <type_traits>

namespace xr {
	class BaseGame {

		// The color to clear the window with
		glt::vec4f clearColor;

		// The window the game is in
		Window* window;

	protected:

		BaseGame();

		// Return the window the game is in
		Window& getWindow();


		// Sets the clear color
		void setClearColor(float r, float g, float b, float a = 1);

	public:

		virtual void setup() = 0;
		virtual void update() = 0;
		virtual void render(Renderer& renderer) = 0;


		virtual void keyPressed(int key) {}
		virtual void keyReleased(int key) {}

		virtual void mousePressed(int button, int x, int y) {}
		virtual void mouseReleased(int button, int x, int y) {}

		virtual void mouseMoved(int x, int y) {}

		virtual void windowResized(int width, int height) {}



		// Start and run a new game
		template <class T>
		static void start(int width, int height, const char* title);
	};

	template<class T>
	inline void BaseGame::start(int width, int height, const char * title)
	{
		// Make sure we're starting a game and not something else
		static_assert(std::is_base_of<BaseGame, T>::value, "T must inherit from BaseGame");

		Window* window = nullptr;
		Renderer* renderer = nullptr;
		BaseGame* game = nullptr;


		// Create the render operation
		auto render = [&]() {
			if (game) {
				// Clear the screen
				renderer->clear(game->clearColor);

				// Render the frame
				game->render(*renderer);

				// Show frame
				window->swapBuffers();
			}
		};


		// Create default window preferences
		WindowPreferences prefs;
		prefs.contextVersionMajor = 3;
		prefs.contextVersionMinor = 3;

		prefs.vsync = false;
		prefs.samples = 8;
		prefs.fullscreen = false;

		// Setup callbacks
		prefs.callbacks.keyPressedCallback = [&](int key) { game->keyPressed(key); };
		prefs.callbacks.keyReleasedCallback = [&](int key) { game->keyReleased(key); };
		
		prefs.callbacks.mousePressedCallback = [&](int button, int x, int y) { game->mousePressed(button, x, y); };
		prefs.callbacks.mouseReleasedCallback = [&](int button, int x, int y) { game->mouseReleased(button, x, y); };

		prefs.callbacks.mouseMovedCallback = [&](int x, int y) {game->mouseMoved(x, y); };

		prefs.callbacks.windowResizedCallback = [&](int width, int height) { if (game) game->windowResized(width, height); };

		prefs.callbacks.windowRefreshedCallback = [&]() { render(); };

		// Create window and renderer
		window = new Window(width, height, title, prefs);
		renderer = new Renderer();

		// Create the game
		game = new T();
		game->window = window;

		// Setup game
		game->setup();

		// Tell the game the size of the window right now
		game->windowResized(width, height);

		// Run the game for as long as the window is open
		while (window->isOpen()) {

			// Create the next frame
			game->update();

			// Render the next frame
			render();

			// Poll events
			window->pollEvents();
		}

		delete game;
		delete renderer;
		delete window;
	}
}



