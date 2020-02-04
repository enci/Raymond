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
	: _origin(lookFrom)
	, _lookAt(lookAt)
	, _up(up)
	, _fov(fov)
	, _aspect(aspect)
{
	Update();
}

Ray Camera::GetRay(float s, float t) const
{
	return Ray(_origin, _lowerLeftCorner + s * _horizontal + (1.0f -  t) * _vertical - _origin);
}

void Camera::Update()
{
	vec3 u, v, w;
	float theta = radians(_fov);
	float halfHeight = tan(theta * 0.5f);
	float halfWidth = _aspect * halfHeight;;
	w = normalize(_origin - _lookAt);
	u = normalize(cross(normalize(_up), w));
	v = normalize(cross(w, u));
	_lowerLeftCorner = _origin - halfWidth * u - halfHeight * v - w;
	_horizontal = 2.0f * halfWidth * u;
	_vertical = 2.0f * halfHeight * v;
}

