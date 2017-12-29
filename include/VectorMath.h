#pragma once

#include <cmath>

namespace xr {
	// Computes the angle between two vectors counter-clockwise
	template <class T>
	float angleBetween(glt::vec2<T> a, glt::vec2<T> b) {
		// Compute angle
		float dot = a.x*b.x + a.y*b.y;
		float det = a.x*b.y - a.y*b.x;

		float angle = atan2f(det, dot);
		return angle;
	}

	
	// Rotate a 2d-vector counter-clockwise
	glt::vec2f rotate(glt::vec2f vec, float angle);
}
