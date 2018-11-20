#pragma once

#define GLM_SWIZZLE 
#include "glm.hpp"
#include "gtc/quaternion.hpp"

namespace Raymond
{
class Ray;
class Traceable;

struct IntersectInfo
{
	float Distance;
	glm::vec3 Position;
	glm::vec3 Normal;
	Traceable* Object;
};

class Traceable
{
public:
	Traceable() : Transform(1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1) {}
	virtual ~Traceable() = default;
	virtual bool Trace(const Ray& r, IntersectInfo& info) const = 0;
	virtual bool Test(const Ray& r) const = 0;
	glm::mat4 Transform;
};

}
