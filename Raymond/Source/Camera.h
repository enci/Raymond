#pragma once

#include "Ray.h"

namespace Raymond
{

class Camera
{
public:
	Camera(
		const glm::vec3& lookFrom,
		const glm::vec3& lookAt,
		const glm::vec3& up,
		float fov,
		float aspect);
	Ray GetRay(float s, float t) const;
	const glm::vec3& Origin() const { return _origin; }

private:
	glm::vec3 _origin;
	glm::vec3 _lowerLeftCorner;
	glm::vec3 _horizontal;
	glm::vec3 _vertical;
};

}