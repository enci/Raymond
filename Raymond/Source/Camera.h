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
	void Update();
	
	const glm::vec3& Origin() const { return _origin;	}
	const glm::vec3& LookAt() const { return _lookAt;	}
	const glm::vec3& Up() const		{ return _up;		}

	void SetOrigin(const glm::vec3& origin) { _origin = origin; }
	void SetLookAt(const glm::vec3& lookAt) { _lookAt = lookAt; }
	void SetUp(const glm::vec3& up)			{ _up = up; }

private:
	glm::vec3 _origin;
	glm::vec3 _lookAt;
	glm::vec3 _up;
	float _fov;
	float _aspect;
	glm::vec3 _lowerLeftCorner;
	glm::vec3 _horizontal;
	glm::vec3 _vertical;
};

}