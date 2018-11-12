#pragma once

#include "glm.hpp"

namespace Raymond
{
class Ray;

struct IntersectInfo
{
	float Distance;
	glm::vec3 Position;
	glm::vec3 Normal;
};

class Traceable
{
public:
	virtual ~Traceable() = default;
	virtual bool Trace(const Ray& r, IntersectInfo& info) const = 0;
	virtual bool Test(const Ray& r) const = 0;
};

}
