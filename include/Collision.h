#pragma once


namespace xr {

	// Forward declarations
	struct AABB;
	struct Circle;


	// Types of collisions

	// An object intersects another object at a specific point
	struct Hit {
		// Is there an intersection
		bool intersects;

		// The 'time' it took for the collision to happen
		float time;

		// The intersection point if there is one
		glm::vec2 point;

		// The normal of the intersection
		glm::vec2 normal;


		// Convert to bool
		operator bool() const {
			return intersects;
		}

		// Compare time
		bool operator<(const Hit& other) {
			return time < other.time;
		}
		bool operator>(const Hit& other) {
			return time > other.time;
		}
	};


	template <class T>
	bool rangeIntersects(T aMin, T aMax, T bMin, T bMax) {
		return aMin < bMax && aMax > bMin;
	}


	template <class T> 
	struct Range {
		T lower, upper;

		Range() {}

		Range(T lower, T upper)
			: lower(lower), upper(upper) {}
	};



	template <class T>
	struct Rectangle {
		T x, y;
		T width, height;

		Rectangle(T x, T y, T width, T height)
			: x(x), y(y), width(width), height(height) {}

		bool intersects(T x, T y, T w, T h)  {
			return (this->x < x + w && this->x + this->width > x &&
					this->y < y + h && this->y + this->height > y);
		}

	};


	struct Ray2 {
		glm::vec2 origin;
		glm::vec2 direction;

		Ray2(glm::vec2 origin, glm::vec2 direction) :
			origin(origin), direction(direction) {}
	};





	struct AABB {
		// The center of the box
		glm::vec2 center;

		// The width and height of the box
		glm::vec2 size;


		AABB(glm::vec2 center, glm::vec2 size) :
			center(center), size(size) {}


		// Detemines if this box contains a point
		bool contains(glm::vec2 p);

		// Determines the intersection point of a line segment, from a to b
		Hit intersects(glm::vec2 a, glm::vec2 b);

		// Determines if two boxes intersect
		bool intersects(const AABB& box);


		// Sweeps a rectangle and returns collision
		Hit sweep(const AABB& box, glm::vec2 delta);

		// Sweeps a circle and returns collision
		Hit sweep(const Circle& circle, glm::vec2 delta);

	private:

		// Returns the bounds of this box [left, right, top, bottom]
		glm::vec4 getBounds() const;

	};


	struct Circle {
		// Center of the circle
		glm::vec2 center;

		// Radius of the circle
		float radius;

		Circle(glm::vec2 center, float radius) :
			center(center), radius(radius) {}

		// Intersect with ray, from a to b
		Hit intersects(glm::vec2 a, glm::vec2 b);

		// Sweep circle and return collision
		Hit sweep(const Circle& circle, glm::vec2 delta);
	};

}
