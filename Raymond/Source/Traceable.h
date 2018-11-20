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
	Traceable() : _transform(1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1) { _inverse = _transform; }
	virtual ~Traceable() = default;
	bool TraceWorld(const Ray& r, IntersectInfo& info);
	virtual bool Trace(const Ray& r, IntersectInfo& info) const = 0;
	virtual bool Test(const Ray& r) const = 0;
	void SetTransform(const glm::mat4& transform) { _transform = transform, _inverse = inverse(_transform); }
	const glm::mat4& GetTransform() const { return  _transform; }

protected:
	glm::mat4 _transform;
	glm::mat4 _inverse;
};

}
