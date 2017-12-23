#pragma once

#include <Xerus.h>

using namespace xr;





class LevelEditor: public BaseGame
{
public:

	// Inherited via BaseGame
	virtual void setup();
	virtual void update();
	virtual void render(Renderer & renderer);


	virtual void keyPressed(int key);

	virtual void mousePressed(int button, int x, int y);

	virtual void windowResized(int width, int height);

	virtual void mouseMoved(int x, int y);



	// Render batch
	RenderBatch batch;
	RenderBatch shadowBatch;

	// Camera
	OrthographicCamera camera;


	// Size of the window
	float w, h;


	// Size of a tile
	const float TILE_SIZE = 51;

	// Convert mouse coordinates to tile coordinates
	glm::ivec2 mouseToTile(glm::ivec2 mouse);

	// Currently selected tile
	glm::ivec2 selectedTile;


	///////////////////////////
	// Grid ///////////////////
	///////////////////////////
	// Renders the grid
	void drawGrid(RenderBatch& batch);




	///////////////////////////
	// Blocks /////////////////
	///////////////////////////


	struct Block {};

	// Compare two tiles, for use in maps
	struct Compivec2 {
		bool operator()(const glm::ivec2& lhs, const glm::ivec2& rhs) const {
			return lhs.x < rhs.x || lhs.x == rhs.x && lhs.y < rhs.y;
		}
	};


	// Map of all blocks
	std::map<glm::ivec2, Block, Compivec2> blocks;

	// Draw all blocks
	void drawBlocks(RenderBatch& batch);




	///////////////////////////
	// Walls //////////////////
	///////////////////////////

	struct Wall {
		glm::vec2 start, end;

		Wall(glm::vec2 start, glm::vec2 end) :
			start(start), end(end) {}
	};

	// List of all walls
	std::vector<Wall> walls;

	// Generate walls from blocks
	std::vector <Wall> generateWalls(const std::map<glm::ivec2, Block, Compivec2>& blocks);

	// Draw all walls
	void drawWalls(RenderBatch& batch);



	///////////////////////////
	// Shadows ////////////////
	///////////////////////////

	// Enabled?
	bool enableShadows = true;

	// Darkness
	float shadowDarkness = 0.1;

	// Draw shadows
	void drawShadows(glm::vec2 light, float shadowLength, RenderBatch& batch);



};


