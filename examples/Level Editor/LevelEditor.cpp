#include "LevelEditor.h"


void LevelEditor::setup()
{
	setClearColor(0, 0.4f, 0.1f);
	getWindow().setVerticalSync(false);

	selectionStart = nullptr;


	font = TrueTypeFont{"D:/Code/Xerus/examples/Level Editor/res/arial.ttf", 42};
}

void LevelEditor::update()
{
    static double elapsedTime = 0;
    static int frames = 0;
    elapsedTime += getWindow().getLastFrameTime();
    frames++;

    if (elapsedTime > 0.5) {
        this->frameRate = static_cast<int>(round(frames / elapsedTime));
        frames = 0; elapsedTime = 0;
    }
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

	glt::vec2f lightPosition = camera.screenToWorld(getWindow().windowToScreen(getWindow().getCursorPosition()));

	// Draw the selected tile
	batch.setFillColor(0.5, 1, 1, 0.5);
	batch.fillRect(selectedTile, 1);

	// Draw current selection
	if (selectionStart) {
		glt::vec2i tile = mouseToTile(getWindow().getCursorPosition());
		std::vector<glt::vec2i> tiles = getSelectionTiles(*selectionStart, tile);

		batch.setFillColor(0, 0, 1, 0.5);
		for (auto& t : tiles) {
			batch.fillRect(t, 1);
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

	renderer.setColorFilter(glt::vec4f{glt::vec3f{ shadowDarkness }, 1.0 });
	renderer.submit(batch);
	renderer.setColorFilter({ 1, 1, 1, 1 });

	glStencilFunc(GL_NOTEQUAL, 1, 0xff);

	renderer.submit(batch);

    glStencilFunc(GL_ALWAYS, 0, 0xff);



    // Draw text
    batch.clear();

    glt::mat4f ortho = glt::orthographic(0.f, w, h, 0.f);
    batch.setCamera(ortho);

    font.renderText("FPS:" + std::to_string(frameRate), {10, 42}, &batch);
    font.renderText("ABCDEFGHIJKLMNOPQRSTUVXYZ", {100, 100}, &batch);

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
		glt::vec2i tile = mouseToTile({ x, y });

		if (getWindow().getKey(GLFW_KEY_LEFT_SHIFT)) {
			selectionStart = new glt::vec2i(tile);
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
		glt::vec2i tile = mouseToTile({ x, y });

		if (getWindow().getKey(GLFW_KEY_LEFT_SHIFT)) {
			std::vector<glt::vec2i> tiles = getSelectionTiles(*selectionStart, tile);

			if (getWindow().getKey(GLFW_KEY_LEFT_ALT)) {
				for (auto& t : tiles) {
					blocks.erase(t);
				}
			}
			else {
				for (auto& t : tiles) {
					blocks[t] = Block();
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
	static glt::vec2i lastMousePosition = { x, y };

	if (getWindow().getMouseButton(GLFW_MOUSE_BUTTON_RIGHT)) {
		glt::vec2f cameraDelta = camera.screenToWorld(getWindow().windowToScreen(lastMousePosition)) -
				camera.screenToWorld(getWindow().windowToScreen({float(x), float(y)}));
		camera.setPosition(glt::vec2f(camera.getPosition()) + cameraDelta);
    }
	selectedTile = mouseToTile({ x, y });






	// Finish
	lastMousePosition = { x, y };
}

glt::vec2i LevelEditor::mouseToTile(glt::vec2i mouse)
{
	glt::vec2f world = camera.screenToWorld(getWindow().windowToScreen(mouse));

	glt::vec2i tile = glt::floor(world);

	return tile;
}

void LevelEditor::drawGrid(RenderBatch & batch)
{
	// Set color
	batch.setFillColor(0.2);
	float lineWidth = 2 / TILE_SIZE;

	glt::vec2i topLeft = mouseToTile({ 0, 0 });
	glt::vec2i bottomRight = mouseToTile({static_cast<int>(w), static_cast<int>(h)});

	// Vertical lines
	for (int x = topLeft.x; x <= bottomRight.x; ++x) {
		batch.drawLine(x, topLeft.y, x, bottomRight.y + TILE_SIZE, lineWidth);
	}
	// Horizontal lines
	for (int y = topLeft.y; y <= bottomRight.y; ++y) {
		batch.drawLine(topLeft.x, y, bottomRight.x + TILE_SIZE, y, lineWidth);
	}
}

void LevelEditor::drawBlocks(RenderBatch & batch)
{
	for (auto& pair : blocks) {
		glt::vec2i position = pair.first;
		const Block& block = pair.second;

		// Set the color
		batch.setFillColor(0, 1, 0);
		batch.fillRect(position, 1);
	}
}

std::vector<glt::vec2i> LevelEditor::getSelectionTiles(glt::vec2i start, glt::vec2i end)
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


	std::vector<glt::vec2i> tiles;

	for (int x = left; x <= right; x++) {
		for (int y = top; y <= bottom; y++) {
			tiles.emplace_back(x, y);
		}
	}

	return tiles;
}

std::vector<LevelEditor::Wall> LevelEditor::generateWalls(const std::map<glt::vec2i, Block, Compivec2>& blocks)
{
	std::vector<Wall> walls;

	glt::vec2i deltas[4] = {
		{ 1, 0 },
		{ 0, 1 },
		{ -1, 0 },
		{ 0, -1 },
	};


	for (auto& pair : blocks) {
		glt::vec2i position = pair.first;
		
		glt::vec2f wallStart = position + deltas[0];

		for (int i = 0; i < 4; i++) {
			glt::vec2f wallEnd = wallStart + glt::vec2f(deltas[(i + 1) % 4]);
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

void LevelEditor::drawShadows(glt::vec2f light, float shadowLength, RenderBatch & batch)
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
		glt::vec2f farPerpendicular{};

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
			farPerpendicular = (farStart + farEnd) / 2.f;
		}
		else {
			// Find the projected point on the wall
			glt::vec2f perpPoint = walls[i].start + proj * wallDir;

			// Find perpendicular direction
			glt::vec2f dPerp = glt::normalize(perpPoint - light);

			// Find the far perpendicular point
			farPerpendicular = perpPoint + dPerp * shadowLength;
		}


		// Add all points into triangle mesh

		shadowPoints.push_back(walls[i].start);
		shadowPoints.push_back(farStart);
		shadowPoints.push_back(farPerpendicular);

		shadowPoints.push_back(walls[i].start);
		shadowPoints.push_back(farPerpendicular);
		shadowPoints.push_back(farEnd);

		shadowPoints.push_back(walls[i].start);
		shadowPoints.push_back(farEnd);
		shadowPoints.push_back(walls[i].end);
	}

	batch.fillTriangles(shadowPoints);
}
