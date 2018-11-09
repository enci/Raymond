#pragma once

#include "glm.hpp"

namespace Raymond
{

class Ray
{
public:
	Ray() = default;
	Ray(const glm::vec3& origin, const glm::vec3& direction)
		: Origin(origin), Direction(glm::normalize(direction)) {}
	glm::vec3 PointAtParameter(const float t) const { return Origin + t * Direction; }
	glm::vec3 Origin;
	glm::vec3 Direction;
};

}