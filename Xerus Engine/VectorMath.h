#pragma once

#include <cmath>

namespace xr {
	// Computes the angle between two vectors counter-clockwise
	template <class VecType>
	float angleBetween(VecType a, VecType b) {
		// Compute angle
		float dot = a.x*b.x + a.y*b.y;
		float det = a.x*b.y - a.y*b.x;

		float angle = atan2(det, dot);
		return angle;
	}
}
