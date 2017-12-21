#include "stdafx.h"
#include "VectorMath.h"

glm::vec2 xr::rotate(glm::vec2 vec, float angle)
{
	float cs = cos(angle);
	float sn = sin(angle);

	return {
		vec.x * cs - vec.y * sn,
		vec.x * sn + vec.y * cs,
	};
}
