#include "stdafx.h"
#include "VectorMath.h"

glt::vec2f xr::rotate(glt::vec2f vec, float angle)
{
	float cs = cosf(angle);
	float sn = sinf(angle);

	return {
		vec.x * cs - vec.y * sn,
		vec.x * sn + vec.y * cs,
	};
}
