#pragma once

#include <glm.hpp>
#include "Defines.h"

namespace Raymond
{

inline glm::vec3 RandomOnUnitSphere()
{
	glm::vec3 v;
	do
	{
		v = glm::vec3(RandInRange(-1.0f, 1.0f),
			RandInRange(-1.0f, 1.0f),
			RandInRange(-1.0f, 1.0f));
	} while (length(v) > 1.0f);
	return normalize(v);
}

inline glm::vec3 RandomOnUnitHemisphere(glm::vec3& direction)
{
	glm::vec3 v = RandomOnUnitSphere();
	if (dot(v, direction) < 0.0f)
		return -v;
	return v;
}

}