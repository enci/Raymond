#include "Camera.h"
#include "glm.hpp"

using namespace Raymond;
using namespace glm;

Camera::Camera(
	const glm::vec3& lookFrom,
	const glm::vec3& lookAt,
	const glm::vec3& up,
	float fov,
	float aspect)
{
	vec3 u, v, w;
	float theta = radians(fov);
	float halfHeight = tan(theta * 0.5f);
	float halfWidth = aspect * halfHeight;
	_origin = lookFrom;
	w = normalize(lookFrom - lookAt);
	u = normalize(cross(normalize(up), w));
	v = normalize(cross(w, u));
	_lowerLeftCorner = _origin - halfWidth * u - halfHeight * v - w;
	_horizontal = 2.0f * halfWidth * u;
	_vertical = 2.0f * halfHeight * v;
}

Ray Camera::GetRay(float s, float t) const
{
	return Ray(_origin, _lowerLeftCorner + s * _horizontal + (1.0f -  t) * _vertical - _origin);
}

