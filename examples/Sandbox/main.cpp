
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


struct Line {
	glt::vec2f start;
	glt::vec2f end;
};


xr::AABB player = xr::AABB({ 0, 0 }, { 32, 32 });


struct Box : xr::AABB {
	int hp = 10;
	int maxHp = 10;

	Box(glt::vec2f center, glt::vec2f size) :
		xr::AABB(center, size)
	{}
};

std::vector<Box> boxes;


enum ParticleType {
	CIRCLE,
	LINE
};

struct Particle {
	ParticleType type;

	glt::vec2f position;
	glt::vec2f velocity;
	glt::vec3f color;

	float lifetime;

	bool deleteWhenDead = true;
	bool killWhenCollided = true;

	// Called when particle dies
	// Return list of new particles to spawn
	std::function<std::vector<Particle*>(xr::Hit*)> onKill;

	union
	{
		float radius;

		struct {
			float length;
			glt::vec2f lastPosition;
			float width;
		} line;
	};


	Particle() {}


	static Particle newLine(glt::vec2f pos, glt::vec2f vel, glt::vec3f color, float length = 10.f, float width = 1.f, float lt = INFINITY) {
		Particle particle;
		particle.type = LINE;
		particle.position = pos;
		particle.velocity = vel;
		particle.color = color;
		particle.lifetime = lt;
		particle.line.lastPosition = pos;
		particle.line.length = length;
		particle.line.width = width;

		return particle;
	}
};


class Bullet : public Particle {
public :
	Bullet(glt::vec2f pos, glt::vec2f vel):
		Particle(Particle::newLine(pos, vel, {1, 0.6, 0}, 30, 2, 1))
	{
		onKill = [&](xr::Hit* hit) -> std::vector<Particle*> {

			if (hit) {
				std::vector<Particle*> newParticles;

				glt::vec2f reflection = glt::normalize(velocity - 2.f * glt::dot(velocity, hit->normal) * hit->normal);

				for (int i = 0; i < 4; i++)
				{
					// Rotate reflection randomly
					float angle = static_cast<float>(3.14159265359f * (rand() / float(RAND_MAX) - 0.5) / 2);
					glt::vec2f newVel = xr::rotate(reflection, angle);

					newParticles.push_back(new Particle(Particle::newLine(hit->point, 500.f * newVel, { 1, 0.8, 0 }, 10, 1, 0.1)));
				}
				
				return newParticles;
			}

			return {};
		};
	}
};


std::vector<Particle*> particles;
std::vector<Bullet> bullets;

void fireBullet(glt::vec2f target);






struct Wall {
	glt::vec2f start;
	glt::vec2f end;
};

std::vector<Wall> walls;

void drawShadows(glt::vec2f light, const std::vector<Wall>& walls, float shadowLength, xr::RenderBatch* batch);



// Camera 
xr::OrthographicCamera camera(1280, 720);

// Converts screen coordinates to world coordinates
glt::vec2i mouseToWorld(int x, int y);
glt::vec2i mouseToWorld(glt::vec2i mouse) { return mouseToWorld(mouse.x, mouse.y); }


int main() {
	window = new xr::Window(1280, 720, "Sandbox", createPreferences());
	xr::Renderer renderer;
	xr::RenderBatch renderBatch;
	xr::RenderBatch darkBatch;
	xr::RenderBatch shadowBatch;

	double elapsed = 0;

	while (window->isOpen())
	{
		double deltaTime = window->getLastFrameTime();
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
		darkBatch.clear();
		shadowBatch.clear();



		float w = (float)window->getWidth();
		float h = (float)window->getHeight();
		
		// Set the current camera
		camera.setPosition(player.center - glt::vec2f{w, h} / 2.f);

		renderBatch.setCamera(camera);
		darkBatch.setCamera(camera);
		shadowBatch.setCamera(camera);






		// Render shadows

		// Draw shadows
		// their length should enclose the whole window
		shadowBatch.setFillColor(0.05, 0.05, 0.05, 0.5);
		drawShadows(player.center, walls, w * h, &shadowBatch);


		/*
		1. Enable stencil testing and write ones to drawn fragments
		2. Draw shadows
		3. Pass stencil test if stencil value is one
		4. Draw scene
		5. Disable stencil testing
		*/

		glEnable(GL_STENCIL_TEST);

		// Mark drawn stencils with 1
		glStencilFunc(GL_ALWAYS, 1, 0xFF);
		glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);
		glStencilMask(0xFF);
		glDepthMask(GL_FALSE);
		glColorMask(0, 0, 0, 0);

		// Clear stencil buffer
		glClear(GL_STENCIL_BUFFER_BIT);

		// Draw shadows
		renderer.submit(shadowBatch);



		// Render backdrop
		{
			glt::vec2f position = camera.screenToWorld({ -1, 1 });

			renderBatch.setFillColor(0.2, 0.4, 0.2);
			renderBatch.fillRect(position, { w, h });
		}



		// Render boxes
		for (auto& box : boxes)
		{
			float hp = box.hp / float(box.maxHp);
			renderBatch.setFillColor({ 1 - hp, hp, 0 });
			renderBatch.fillRect(box.center - box.size / 2.f, box.size);
		}



		// Draw walls
		renderBatch.setFillColor(1.0, 1.0, 1.0);
		glLineWidth(1);
		int wallCount = walls.size();
		for (int i = 0; i < wallCount; i++) {
			renderBatch.drawLine(walls[i].start, walls[i].end);
		}


		// Move player
		glt::vec2f delta;
		float speed = window->getKey(GLFW_KEY_LEFT_SHIFT) ? 100 : 600;

		if (window->getKey(GLFW_KEY_W)) {
			delta.y -= 1;
		}
		if (window->getKey(GLFW_KEY_S)) {
			delta.y += 1;
		}
		if (window->getKey(GLFW_KEY_A)) {
			delta.x -= 1;
		}
		if (window->getKey(GLFW_KEY_D)) {
			delta.x += 1;
		}


		if (window->getMouseButton(GLFW_MOUSE_BUTTON_RIGHT)) {
			glt::vec2f target = mouseToWorld(window->getCursorPosition());
			glt::vec2f dir = glt::normalize(target - player.center);
			delta += dir;
		}



		if (glt::length(delta) != 0) {
			delta = float(deltaTime) * speed * glt::normalize(delta);
		}


		// Collisions
		int i = 0;
		while (glt::length(delta) > 0 && i < 100) {
			xr::Hit* hit = nullptr;

			for (auto& box : boxes) {
				auto h = box.sweep(player, delta);
				if (h.intersects) {
					if (!hit || h.time < hit->time) {
						delete hit;
						hit = new xr::Hit(h);
					}
				}
			}


			if (hit) {
				player.center = hit->point;

				glt::vec2f axis = { hit->normal.y, hit->normal.x };
				float remainingTime = 1 - hit->time;

				float magnitude = glt::length(delta) * remainingTime;
				float dot = glt::dot(axis, delta);

				if (dot > 0) {
					dot = 1;
				}
				if (dot < 0) {
					dot = -1;
				}

				delta = axis * dot * magnitude;
			}
			else {
				player.center += delta;
				delta = { 0, 0 };
			}

			delete hit;
			i++;
		}


		// Render player
		renderBatch.setFillColor(1, 0, 0);
		renderBatch.fillRect(player.center - player.size / 2.f, player.size);


		// Update and render particles
		
		// Fire particles
		if (window->getMouseButton(GLFW_MOUSE_BUTTON_LEFT)) {
			const float maxCooldown = 1.f / 200;
			static float cooldown = maxCooldown;

			while (cooldown < 0) {
				glt::vec2i mouse = window->getCursorPosition();
				mouse = mouseToWorld(mouse.x, mouse.y);

				fireBullet(mouse);
				cooldown += maxCooldown;
			}

			cooldown -= deltaTime;
		}


		// Particles to remove
		std::vector<int> deadParticles;
		std::vector<Particle*> newParticles;
		i = 0;
		for (Particle* particle : particles)
		{
			// Move
			glt::vec2f delta = float(deltaTime) * particle->velocity;

			// Check for collisions
			xr::Hit* earliest = nullptr;

			int hitBox = -1;

			int b = 0;
			for (auto& box : boxes) {
				auto hit = box.intersects(particle->position, particle->position + delta);
				if (hit.intersects) {
					if (!earliest || hit.time < earliest->time) {
						earliest = new xr::Hit(hit);
						hitBox = b;
					}
				}

				b++;
			}

			if (earliest) {
				deadParticles.push_back(i);

				if (particle->onKill) {
					auto spawned = particle->onKill(earliest);
					newParticles.insert(newParticles.end(), spawned.begin(), spawned.end());
				}

				// Destroy box if it is out of hp
				boxes[hitBox].hp--;
				if (boxes[hitBox].hp <= 0) {
					boxes.erase(boxes.begin() + hitBox);
					walls.erase(walls.begin() + hitBox * 4, walls.begin() + hitBox * 4 + 4);
				}

				i++;
				continue;
			}

			particle->position += delta;

			// Render
			renderBatch.setFillColor(particle->color);
			switch (particle->type) {
			case LINE:
				renderBatch.drawLine(particle->position, particle->line.lastPosition, particle->line.width);
				if (glt::distance(particle->position, particle->line.lastPosition) > particle->line.length) {
					particle->line.lastPosition = particle->position - particle->line.length * glt::normalize(particle->velocity);
				}
				break;
			}


			particle->lifetime -= deltaTime;

			if (particle->lifetime < 0) {
				deadParticles.push_back(i);
			}
			i++;
		}

		// Remove all particles
		int removedParticles = 0;
		
		for (int particle : deadParticles) {
			int index = particle - removedParticles;


			if (particles[index]->deleteWhenDead) {
				delete particles[index];
			}

			particles.erase(particles.begin() + index);

			removedParticles++;
		}

		// Add new particles
		for (auto& newParticle : newParticles)
		{
			particles.push_back(newParticle);
		}



		// Pass if stencil is 0, only draw where there is no shadow
		glStencilFunc(GL_EQUAL, 0, 0xFF);
		glStencilMask(0x00);
		glDepthMask(GL_TRUE);
		glColorMask(1, 1, 1, 1);


		// Draw scene
		renderer.setColorFilter({ 1, 1, 1, 1 });
		renderer.submit(renderBatch);



		// Darken scene, only draw in shadow
		glStencilFunc(GL_EQUAL, 1, 0xFF);
		renderer.setColorFilter({ 0.25, 0.25, 0.25, 1 });
		renderer.submit(renderBatch);





		glDisable(GL_STENCIL_TEST);

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
	if (button == GLFW_MOUSE_BUTTON_MIDDLE) {

		glt::vec2i mouse = mouseToWorld(x, y);
		y = mouse.y;
		x = mouse.x;

		int w = rand() % 256 + 10;
		int h = rand() % 256 + 10;
		Box box = {
			{ x, y },
			{ w, h }
		};

		box.hp = static_cast<int>(sqrt(w * h));
		box.maxHp = box.hp;

		boxes.push_back(box);

		x -= w / 2;
		y -= h / 2;

		walls.push_back({ { x, y },{ x + w, y } });
		walls.push_back({ { x + w, y },{ x + w, y + h } });
		walls.push_back({ { x + w, y + h },{ x, y + h } });
		walls.push_back({ { x, y + h },{ x, y } });
	}
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

void fireBullet(glt::vec2f target)
{
	glt::vec2f vel = 2000.f * glt::normalize(target - player.center);

	// Add some inaccuracy
	float angle = static_cast<float>(3.14159265359 * (float(rand()) / RAND_MAX - 0.5) / 12);

	vel =  xr::rotate(vel, angle);

	particles.push_back(new Bullet(player.center, vel));
}




void drawShadows(glt::vec2f light, const std::vector<Wall>& walls, float shadowLength, xr::RenderBatch* batch)
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
		glt::vec2f farPerp;

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
			farPerp = (farStart + farEnd) / 2.f;
		}
		else {
			// Find the projected point on the wall
			glt::vec2f perpPoint = walls[i].start + proj * wallDir;

			// Find perpendicular direction
			glt::vec2f dPerp = glt::normalize(perpPoint - light);

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

	batch->fillTriangles(shadowPoints);
}

glt::vec2i mouseToWorld(int x, int y)
{
	glt::vec2f screen = window->windowToScreen({ x, y });

	glt::vec2f world = camera.screenToWorld(screen);
	return world;
}


