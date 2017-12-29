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
	virtual void mouseReleased(int button, int x, int y);

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
	glt::vec2i mouseToTile(glt::vec2i mouse);

	// Currently selected tile
	glt::vec2i selectedTile;


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
		bool operator()(const glt::vec2i& lhs, const glt::vec2i& rhs) const {
			return lhs.x < rhs.x || lhs.x == rhs.x && lhs.y < rhs.y;
		}
	};


	// Map of all blocks
	std::map<glt::vec2i, Block, Compivec2> blocks;

	// Draw all blocks
	void drawBlocks(RenderBatch& batch);




	///////////////////////////
	// Block Placement ////////
	///////////////////////////

	// Selection
	glt::vec2i* selectionStart;

	// Return list of tiles in selection
	std::vector<glt::vec2i> getSelectionTiles(glt::vec2i start, glt::vec2i end);



	///////////////////////////
	// Walls //////////////////
	///////////////////////////

	struct Wall {
		glt::vec2f start, end;

		Wall(glt::vec2f start, glt::vec2f end) :
			start(start), end(end) {}
	};

	// List of all walls
	std::vector<Wall> walls;

	// Generate walls from blocks
	std::vector <Wall> generateWalls(const std::map<glt::vec2i, Block, Compivec2>& blocks);

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
	void drawShadows(glt::vec2f light, float shadowLength, RenderBatch& batch);



};


