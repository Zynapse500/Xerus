#include "LevelEditor.h"


void LevelEditor::setup()
{
	setClearColor(0, 0.4f, 0.1f);
	getWindow().setVerticalSync(false);

	selectionStart = nullptr;
}

void LevelEditor::update()
{
}

void LevelEditor::render(Renderer & renderer)
{
	batch.clear();
	batch.setCamera(camera);

	shadowBatch.clear();
	shadowBatch.setCamera(camera);


	// Draw background
	batch.setFillColor(1, 0.4, 0.1);
	batch.fillRect(camera.screenToWorld({ -1, 1 }), {w / TILE_SIZE, h / TILE_SIZE});


	drawGrid(batch);
	drawBlocks(batch);
	drawWalls(batch);

	glm::vec2 lightPosition = camera.screenToWorld(getWindow().windowToScreen(getWindow().getCursorPosition()));

	// Draw the selected tile
	batch.setFillColor(0.5, 1, 1, 0.5);
	batch.fillRect(selectedTile, 1);

	// Draw current selection
	if (selectionStart) {
		glm::ivec2 tile = mouseToTile(getWindow().getCursorPosition());
		std::vector<glm::ivec2> tiles = getSelectionTiles(*selectionStart, tile);

		batch.setFillColor(0, 0, 1, 0.5);
		for (auto& tile : tiles) {
			batch.fillRect(tile, 1);
		}
	}


	if (enableShadows)
		glEnable(GL_STENCIL_TEST);
	else
		glDisable(GL_STENCIL_TEST);

	// Draw to stencil
	glStencilFunc(GL_ALWAYS, 1, 0xff);
	glStencilMask(0xff);
	glStencilOp(GL_KEEP, GL_REPLACE, GL_REPLACE);
	glColorMask(0, 0, 0, 0);
	glDepthMask(0);

	// Clear stencil buffer
	glClear(GL_STENCIL_BUFFER_BIT);


	// Render shadows
	shadowBatch.setFillColor(shadowDarkness);
	drawShadows(lightPosition, (w+h) / TILE_SIZE, shadowBatch);

	renderer.submit(shadowBatch);
		
	glStencilFunc(GL_EQUAL, 1, 0xff);
	glStencilMask(0x00);
	glColorMask(1, 1, 1, 1);
	glDepthMask(1);

	renderer.setColorFilter({ glm::vec3{ shadowDarkness }, 1.0 });
	renderer.submit(batch);
	renderer.setColorFilter({ 1, 1, 1, 1 });

	glStencilFunc(GL_NOTEQUAL, 1, 0xff);

	renderer.submit(batch);
}

void LevelEditor::keyPressed(int key)
{
	if (key == GLFW_KEY_ESCAPE) {
		getWindow().close();
	}
	if (key == GLFW_KEY_F11) {
		getWindow().toggleFullscreen();
	}
	if (key == GLFW_KEY_S) {
		enableShadows = !enableShadows;
	}
}

void LevelEditor::mousePressed(int button, int x, int y)
{	
	if (button == GLFW_MOUSE_BUTTON_LEFT) {
		glm::ivec2 tile = mouseToTile({ x, y });

		if (getWindow().getKey(GLFW_KEY_LEFT_SHIFT)) {
			selectionStart = new glm::ivec2(tile);
		}
		else if (getWindow().getKey(GLFW_KEY_LEFT_ALT)) {
			blocks.erase(tile);
			walls = generateWalls(blocks);
		}
		else {
			blocks[tile] = Block();
			walls = generateWalls(blocks);
		}

	}
}

void LevelEditor::mouseReleased(int button, int x, int y)
{
	if (button == GLFW_MOUSE_BUTTON_LEFT) {
		glm::ivec2 tile = mouseToTile({ x, y });

		if (getWindow().getKey(GLFW_KEY_LEFT_SHIFT)) {
			std::vector<glm::ivec2> tiles = getSelectionTiles(*selectionStart, tile);

			if (getWindow().getKey(GLFW_KEY_LEFT_ALT)) {
				for (auto& tile : tiles) {
					blocks.erase(tile);
				}
			}
			else {
				for (auto& tile : tiles) {
					blocks[tile] = Block();
				}
			}

			walls = generateWalls(blocks);
		}

		delete selectionStart;
		selectionStart = nullptr;
	}
}

void LevelEditor::windowResized(int width, int height)
{
	this->w = width;
	this->h = height;
	
	float right = w / TILE_SIZE / 2;
	float bottom = h / TILE_SIZE / 2;

	camera.setProjection(-right, right, -bottom, bottom);
}

void LevelEditor::mouseMoved(int x, int y)
{
	static glm::ivec2 lastMousePosition = { x, y };

	if (getWindow().getMouseButton(GLFW_MOUSE_BUTTON_RIGHT)) {
		glm::vec2 cameraDelta = lastMousePosition - glm::ivec2(x, y);
		camera.setPosition(glm::vec2(camera.getPosition()) + cameraDelta / TILE_SIZE);
	}

	selectedTile = mouseToTile({ x, y });

	// Finish
	lastMousePosition = { x, y };
}

glm::ivec2 LevelEditor::mouseToTile(glm::ivec2 mouse)
{
	glm::vec2 world = camera.screenToWorld(getWindow().windowToScreen(mouse));

	glm::ivec2 tile = glm::floor(world);

	return tile;
}

void LevelEditor::drawGrid(RenderBatch & batch)
{
	// Set color
	batch.setFillColor(0.2);
	float lineWidth = 2 / TILE_SIZE;

	glm::ivec2 topLeft = mouseToTile({ 0, 0 });
	glm::ivec2 bottomRight = mouseToTile({ w, h });

	// Vertical lines
	for (float x = topLeft.x; x <= bottomRight.x; x++) {
		batch.drawLine(x, topLeft.y, x, bottomRight.y + TILE_SIZE, lineWidth);
	}
	// Horizontal lines
	for (float y = topLeft.y; y <= bottomRight.y; y++) {
		batch.drawLine(topLeft.x, y, bottomRight.x + TILE_SIZE, y, lineWidth);
	}
}

void LevelEditor::drawBlocks(RenderBatch & batch)
{
	for (auto& pair : blocks) {
		glm::ivec2 position = pair.first;
		const Block& block = pair.second;

		// Set the color
		batch.setFillColor(0, 1, 0);
		batch.fillRect(position, 1);
	}
}

std::vector<glm::ivec2> LevelEditor::getSelectionTiles(glm::ivec2 start, glm::ivec2 end)
{
	int left = start.x;
	int right = end.x;
	if (start.x > end.x) {
		left = end.x;
		right = start.x;
	}

	int top = start.y;
	int bottom = end.y;
	if (start.y > end.y) {
		top = end.y;
		bottom = start.y;
	}


	std::vector<glm::ivec2> tiles;

	for (int x = left; x <= right; x++) {
		for (int y = top; y <= bottom; y++) {
			tiles.emplace_back(x, y);
		}
	}

	return tiles;
}

std::vector<LevelEditor::Wall> LevelEditor::generateWalls(const std::map<glm::ivec2, Block, Compivec2>& blocks)
{
	std::vector<Wall> walls;

	glm::ivec2 deltas[4] = {
		{ 1, 0 },
		{ 0, 1 },
		{ -1, 0 },
		{ 0, -1 },
	};


	for (auto& pair : blocks) {
		glm::ivec2 position = pair.first;
		
		glm::vec2 wallStart = position + deltas[0];

		for (int i = 0; i < 4; i++) {
			glm::vec2 wallEnd = wallStart + glm::vec2(deltas[(i + 1) % 4]);
			if (!blocks.count(position + deltas[i])) {
				walls.emplace_back(wallStart, wallEnd);
			}
			wallStart = wallEnd;
		}
	}
	
	printf("Walls: %d\n", walls.size());

	return walls;
}

void LevelEditor::drawWalls(RenderBatch & batch)
{
	// Set the color
	batch.setFillColor(1, 1, 1);
	for (auto& wall : walls) {
		batch.drawLine(wall.start, wall.end, 1 / TILE_SIZE);
	}
}

void LevelEditor::drawShadows(glm::vec2 light, float shadowLength, RenderBatch & batch)
{

	std::vector<glm::vec2> shadowPoints;

	int wallCount = walls.size();
	for (int i = 0; i < wallCount; i++)
	{
		// Find the direction of the shadow from the light to the corners
		glm::vec2 dStart = glm::normalize(walls[i].start - light);
		glm::vec2 dEnd = glm::normalize(walls[i].end - light);

		// Cast shadow from corners
		glm::vec2 farStart = walls[i].start + dStart * shadowLength;
		glm::vec2 farEnd = walls[i].end + dEnd * shadowLength;

		// Find line perpendicular to wall and light
		glm::vec2 farPerp;

		// Translate origin to wall start
		glm::vec2 wallEnd = walls[i].end - walls[i].start;
		glm::vec2 lightPos = light - walls[i].start;

		// Project the light's position onto the wall
		glm::vec2 wallDir = glm::normalize(wallEnd);

		//  a.b = |a||b| cos 0 = |lightPos| cos 0
		float proj = glm::dot(lightPos, wallDir);

		// Proj has to lie on wall
		if (proj <= 0 || proj >= glm::length(wallEnd)) {
			// Place in the middle of the others
			farPerp = (farStart + farEnd) / 2.f;
		}
		else {
			// Find the projected point on the wall
			glm::vec2 perpPoint = walls[i].start + proj * wallDir;

			// Find perpendicular direction
			glm::vec2 dPerp = glm::normalize(perpPoint - light);

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

	batch.fillTriangles(shadowPoints);
}
