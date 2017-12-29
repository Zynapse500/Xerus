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
		glt::vec2f point;

		// The normal of the intersection
		glt::vec2f normal;


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
		glt::vec2f origin;
		glt::vec2f direction;

		Ray2(glt::vec2f origin, glt::vec2f direction) :
			origin(origin), direction(direction) {}
	};





	struct AABB {
		// The center of the box
		glt::vec2f center;

		// The width and height of the box
		glt::vec2f size;


		AABB(glt::vec2f center, glt::vec2f size) :
			center(center), size(size) {}


		// Detemines if this box contains a point
		bool contains(glt::vec2f p);

		// Determines the intersection point of a line segment, from a to b
		Hit intersects(glt::vec2f a, glt::vec2f b);

		// Determines if two boxes intersect
		bool intersects(const AABB& box);


		// Sweeps a rectangle and returns collision
		Hit sweep(const AABB& box, glt::vec2f delta);

		// Sweeps a circle and returns collision
		Hit sweep(const Circle& circle, glt::vec2f delta);

	private:

		// Returns the bounds of this box [left, right, top, bottom]
		glt::vec4f getBounds() const;

	};


	struct Circle {
		// Center of the circle
		glt::vec2f center;

		// Radius of the circle
		float radius;

		Circle(glt::vec2f center, float radius) :
			center(center), radius(radius) {}

		// Intersect with ray, from a to b
		Hit intersects(glt::vec2f a, glt::vec2f b);

		// Sweep circle and return collision
		Hit sweep(const Circle& circle, glt::vec2f delta);
	};

}
