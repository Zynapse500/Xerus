#include "stdafx.h"
#include "Collision.h"

bool xr::AABB::contains(glt::vec2f p)
{
	const float left = center.x - size.x / 2;
	const float right = center.x + size.x / 2;
	const float top = center.y - size.y / 2;
	const float bottom = center.y + size.y / 2;

	return
		left < p.x && p.x < right &&
		top < p.y && p.y < bottom;
}

xr::Hit xr::AABB::intersects(glt::vec2f a, glt::vec2f b)
{
	// Bounds of the box
	glt::vec2f lowerBounds = {
		center.x - size.x / 2,
		center.y - size.y / 2
	};

	glt::vec2f upperBounds = {
		center.x + size.x / 2,
		center.y + size.y / 2
	};


	glt::vec2f delta = b - a;

	glt::vec2f entryTimes;
	glt::vec2f exitTimes;

	for (int i = 0; i < 2; i++)
	{
		if (delta[i] == 0) {
			// Has to be in correct range for intersection
			if (!(lowerBounds[i] < a[i] && a[i] < upperBounds[i])) {
				return { false, 1 };
			}

			entryTimes[i] = -INFINITY;
			exitTimes[i] = INFINITY;
		}
		else {
			entryTimes[i] = ((delta[i] > 0 ? lowerBounds[i] : upperBounds[i]) - a[i]) / delta[i];
			exitTimes[i] = ((delta[i] > 0 ? upperBounds[i] : lowerBounds[i]) - a[i]) / delta[i];
		}
	}

	float entryTime = std::max(entryTimes.x, entryTimes.y);
	float exitTime = std::min(exitTimes.x, exitTimes.y);


	if (exitTimes.x < entryTimes.y || exitTimes.y < entryTimes.x) {
		return { false, 1 };
	}


	

	if (0 <= entryTime && entryTime <= 1) {
		float time = entryTime;

		glt::vec2f normal;

		if (entryTimes.x > entryTimes.y) {
			// Collision on x-bound
			normal.x = delta.x > 0 ? -1 : 1;
			normal.y = 0;
		}
		else {
			// Collision on y-bound
			normal.x = 0;
			normal.y = delta.y > 0 ? -1 : 1;
		}

		return { true, time, a + time * delta, normal };
	}


	if (0 < exitTime && exitTime <= 1) {
		float time = exitTime;

		glt::vec2f normal;

		if (exitTimes.x < exitTimes.y) {
			// Collision on x-bound
			normal.x = delta.x > 0 ? -1 : 1;
			normal.y = 0;
		}
		else {
			// Collision on y-bound
			normal.x = 0;
			normal.y = delta.y > 0 ? -1 : 1;
		}
		return { true, time, a + time * delta, normal };
	}
	
	return { false, 1 };
}

bool xr::AABB::intersects(const AABB & box)
{
	glt::vec4f thisBounds = this->getBounds();
	glt::vec4f otherBounds = box.getBounds();

	return thisBounds[0] <= otherBounds[1] && thisBounds[1] >= otherBounds[0] &&
		   thisBounds[2] <= otherBounds[3] && thisBounds[3] >= otherBounds[2];

}

xr::Hit xr::AABB::sweep(const AABB & box, glt::vec2f delta)
{
	// Create new box that's padded with the swept box
	AABB padded = {
		this->center,
		this->size + box.size
	};

	// Pad from the inside if necessary
	if (padded.contains(box.center)) {
		padded.size -= box.size * 2.f;
	} 

	// Intersect the box's path
	return padded.intersects(box.center, box.center + delta);
}

xr::Hit xr::AABB::sweep(const Circle& circle, glt::vec2f delta)
{
	// Create new box that's padded with the swept circle's radius
	AABB padded = {
		this->center,
		this->size + 2 * circle.radius
	};

	Hit hit = padded.intersects(circle.center, circle.center + delta);

	// If we hit a corner, check with a circle instead
	glt::vec2f p = hit.point - this->center;
	float sx = size.x;
	float sy = size.y;

	// Top
	if (p.y < -sy / 2) {
		// Left
		if (p.x < -sx / 2) {
			Circle padding = { this->center + glt::vec2f{-sx / 2, -sy / 2}, circle.radius };
			return std::min(hit, padding.intersects(circle.center, circle.center + delta));
		}
		// Right
		if (p.x > sx / 2) {
			Circle padding = { this->center + glt::vec2f{ sx / 2, -sy / 2 }, circle.radius };
			return std::min(hit, padding.intersects(circle.center, circle.center + delta));
		}
	}

	// Bottom
	if (p.y > sy / 2) {
		// Left
		if (p.x < -sx / 2) {
			Circle padding = { this->center + glt::vec2f{ -sx / 2, sy / 2 }, circle.radius };
			return std::min(hit, padding.intersects(circle.center, circle.center + delta));
		}
		// Right
		if (p.x > sx / 2) {
			Circle padding = { this->center + glt::vec2f{ sx / 2, sy / 2 }, circle.radius };
			return std::min(hit, padding.intersects(circle.center, circle.center + delta));
		}
	}

	return hit;
}

glt::vec4f xr::AABB::getBounds() const
{
	return glt::vec4f(
		center.x - size.x / 2,
		center.x + size.x / 2,
		center.y - size.y / 2,
		center.y + size.y / 2
	);
}

xr::Hit xr::Circle::intersects(glt::vec2f start, glt::vec2f end)
{
	// Direction of line
	glt::vec2f delta = end - start;
	glt::vec2f lineDir = delta;

	// Direction from line start to circle
	glt::vec2f direction = start - center;


	// Solve quadratic equation
	float a = glt::dot(lineDir, lineDir);
	float b = 2.f * glt::dot(direction, lineDir);
	float c = glt::dot(direction, direction) - radius * radius;

	float discriminant = b*b - 4.f * a*c;
	if (discriminant >= 0) {
		discriminant = sqrtf(discriminant);

		float t1 = (-b - discriminant) / (2 * a);
		float t2 = (-b + discriminant) / (2 * a);

		// 3x HIT cases:
		//          -o->             --|-->  |            |  --|->
		// Impale(t1 hit,t2 hit), Poke(t1 hit,t2>1), ExitWound(t1<0, t2 hit), 

		// 3x MISS cases:
		//       ->  o                     o ->              | -> |
		// FallShort (t1>1,t2>1), Past (t1<0,t2<0), CompletelyInside(t1<0, t2>1)

		if (0 <= t1 && t1 <= 1) {
			glt::vec2f point = start + t1 * delta;
			return { true, t1, point, glt::normalize(point - center) };
		}

		if (0 <= t2 && t2 <= 1) {
			glt::vec2f point = start + t2 * delta;
			return { true, t2, point, glt::normalize(point - center) };
		}
	}


	return { false, 1 };
}

xr::Hit xr::Circle::sweep(const Circle & circle, glt::vec2f delta)
{
	// Create new cirlce that's padded with the swept cirlce
	Circle padded = {
		this->center,
		this->radius + circle.radius
	};

	// Intersect the box's path
	return padded.intersects(circle.center, circle.center + delta);
}
