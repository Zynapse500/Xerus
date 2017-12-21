
#include <Xerus.h>

void onKeyPressed(int key);

void onMousePressed(int button, int x, int y);
void onMouseReleased(int button, int x, int y);
void onMouseMoved(int x, int y);


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

	return prefs;
}


struct Line {
	glm::vec2 start;
	glm::vec2 end;
};


xr::AABB player = xr::AABB({ 72, 128 }, { 32, 32 });


struct Box : xr::AABB {
	int hp = 10;

	Box(glm::vec2 center, glm::vec2 size) :
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

	glm::vec2 position;
	glm::vec2 velocity;
	glm::vec3 color;

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
			glm::vec2 lastPosition;
			float width;
		} line;
	};


	Particle() {}


	static Particle newLine(glm::vec2 pos, glm::vec2 vel, glm::vec3 color, float length = 10.f, float width = 1.f, float lt = INFINITY) {
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
	Bullet(glm::vec2 pos, glm::vec2 vel):
		Particle(Particle::newLine(pos, vel, {1, 0.6, 0}, 30, 2, 1))
	{
		onKill = [&](xr::Hit* hit) -> std::vector<Particle*> {

			if (hit) {
				std::vector<Particle*> newParticles;

				glm::vec2 reflection = glm::normalize(velocity - 2.f * glm::dot(velocity, hit->normal) * hit->normal);

				for (int i = 0; i < 10; i++)
				{
					// Rotate reflection randomly
					float angle = glm::pi<float>() * (rand() / float(RAND_MAX) - 0.5);
					glm::vec2 newVel = xr::rotate(reflection, angle);

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

void fireBullet(glm::vec2 target);






struct Wall {
	glm::vec2 start;
	glm::vec2 end;
};

std::vector<Wall> walls;

void drawShadows(glm::vec2 light, const std::vector<Wall>& walls, float shadowLength, xr::RenderBatch* batch);



// Camera matrix
glm::mat4 camera;


// Converts screen coordinates to world coordinates
glm::ivec2 mouseToWorld(int x, int y);
glm::ivec2 mouseToWorld(glm::ivec2 mouse) { return mouseToWorld(mouse.x, mouse.y); }


int main() {
	window = new xr::Window(1280, 720, "Collisions", createPreferences());
	xr::Renderer renderer;
	xr::RenderBatch renderBatch;
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
		shadowBatch.clear();

		// Set the current camera
		float w = (float)window->getWidth();
		float h = (float)window->getHeight();

		glm::mat4 proj = glm::ortho(0.0f, w, h, 0.0f);

		glm::mat4 view = glm::translate(glm::mat4(), glm::vec3(-player.center + glm::vec2{w, h} / 2.f, 0));

		camera = proj * view;

		renderBatch.setCameraMatrix(camera);
		shadowBatch.setCameraMatrix(camera);









		// Render shadows

		// Draw shadows
		// their length should enclose the whole window
		shadowBatch.setFillColor(0.05, 0.05, 0.05);
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



		


		// Draw walls
		renderBatch.setFillColor(1.0, 1.0, 1.0);
		glLineWidth(1);
		int wallCount = walls.size();
		for (int i = 0; i < wallCount; i++) {
			renderBatch.drawLine(walls[i].start, walls[i].end);
		}


		// Move player
		glm::vec2 delta;
		float speed = window->getKey(GLFW_KEY_LEFT_SHIFT) ? 800 : 600;

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
			glm::vec2 target = mouseToWorld(window->getCursorPosition());
			glm::vec2 dir = glm::normalize(target - player.center);
			delta += dir;
		}



		if (glm::length(delta) != 0) {
			delta = float(deltaTime) * speed * glm::normalize(delta);
		}

		// Find intersection point
		Line* normal = nullptr;

		// Collisions
		int i = 0;
		while (glm::length(delta) > 0 && i < 100) {
			xr::Hit* hit = nullptr;

			for (auto& box : boxes) {
				auto h = box.sweep(player, delta);
				if (h.intersects) {
					if (!hit || h.time < hit->time) {
						hit = new xr::Hit(h);
					}
				}
			}


			if (hit) {
				player.center = hit->point;

				glm::vec2 normalStart = player.center - player.size / 2.f * hit->normal;

				normal = new Line{
					normalStart,
					normalStart + 25.f * hit->normal
				};

				glm::vec2 axis = { hit->normal.y, hit->normal.x };
				float remainingTime = 1 - hit->time;

				float magnitude = glm::length(delta) * remainingTime;
				float dot = glm::dot(axis, delta);

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

			i++;
		}


		// Render normal
		if (normal) {
			renderBatch.setFillColor(0, 1, 1);
			renderBatch.drawLine(normal->start, normal->end);
		}

		// Render player
		renderBatch.setFillColor(1, 0, 0);
		renderBatch.fillRect(player.center - player.size / 2.f, player.size);


		// Update and render particles
		
		// Fire particles
		if (window->getMouseButton(GLFW_MOUSE_BUTTON_LEFT)) {
			const float maxCooldown = 1. / 20;
			static float cooldown = maxCooldown;

			if (cooldown < 0) {
				glm::ivec2 mouse = window->getCursorPosition();
				mouse = mouseToWorld(mouse.x, mouse.y);

				fireBullet(mouse);
				cooldown = maxCooldown;
			}

			cooldown -= deltaTime;
		}


		// Particles to remove
		std::vector<int> deadParticles;
		std::vector<Particle*> newParticles;
		i = 0;
		for each (Particle* particle in particles)
		{
			// Move
			glm::vec2 delta = float(deltaTime) * particle->velocity;

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
				if (glm::distance(particle->position, particle->line.lastPosition) > particle->line.length) {
					particle->line.lastPosition = particle->position - particle->line.length * glm::normalize(particle->velocity);
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
		
		for each (int particle in deadParticles) {
			int index = particle - removedParticles;


			if (particles[index]->deleteWhenDead) {
				delete particles[index];
			}

			particles.erase(particles.begin() + index);

			removedParticles++;
		}

		// Add new particles
		for each (auto& newParticle in newParticles)
		{
			particles.push_back(newParticle);
		}


		// Render boxes
		renderBatch.setFillColor(0, 1, 0);
		for (auto& box : boxes)
		{
			renderBatch.fillRect(box.center - box.size / 2.f, box.size);
		}


		// Render backdrop
		{
			glm::vec2 position = view * glm::vec4(0, 0, 0, 1);

			renderBatch.setFillColor(0.2, 0.4, 0.2);
			renderBatch.fillRect(-position, { w, h });
		}



		// Pass if stencil is 1
		glStencilFunc(GL_NOTEQUAL, 1, 0xFF);
		glStencilMask(0x00);
		glDepthMask(GL_TRUE);
		glColorMask(1, 1, 1, 1);


		// Draw scene
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

		glm::ivec2 mouse = mouseToWorld(x, y);
		x = mouse.x;
		y = mouse.y;

		int w = rand() % 256 + 10;
		int h = rand() % 256 + 10;
		Box box = {
			{ x, y },
			{ w, h }
		};

		box.hp = sqrt(w * h) / 4;

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

void fireBullet(glm::vec2 target)
{
	glm::vec2 vel = 2000.f * glm::normalize(target - player.center);

	// Add some inaccuracy
	float angle = glm::pi<float>() * (float(rand()) / RAND_MAX - 0.5) / 8;

	vel =  xr::rotate(vel, angle);

	particles.push_back(new Bullet(player.center, vel));
}




void drawShadows(glm::vec2 light, const std::vector<Wall>& walls, float shadowLength, xr::RenderBatch* batch)
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

	batch->fillTriangles(shadowPoints);
}

glm::ivec2 mouseToWorld(int x, int y)
{
	glm::vec2 size = window->getSize();
	return { x + player.center.x - size.x / 2.f, y + player.center.y - size.y / 2.f };
}


