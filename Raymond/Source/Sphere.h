#pragma once

#include "glm.hpp"
#include "Traceable.h"

namespace Raymond
{

class Sphere : public Traceable
{
public:
	Sphere() = default;
	Sphere(const glm::vec3& center, const float radius)
		: Center(center), Radius(radius) {}
	~Sphere() override = default;
	bool Trace(const Ray& r, IntersectInfo& info) const  override;

	glm::vec3 Center;
	float Radius;
};

}
