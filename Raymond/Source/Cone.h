#pragma once

#pragma once

#include "glm.hpp"
#include "Traceable.h"

namespace Raymond
{

class Cone : public Traceable
{
public:
	Cone() = default;
	Cone(const glm::vec3& center, const float angle)
		: Center(center), Angle(angle) {}
	~Cone() override = default;
	bool Trace(const Ray& r, IntersectInfo& info) const  override;

	glm::vec3 Center;
	float Angle;
};
}