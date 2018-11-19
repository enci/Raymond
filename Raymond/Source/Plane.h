#pragma once
#include "Traceable.h"

namespace Raymond
{

class Plane : public Traceable
{
public:
	Plane(const glm::vec3& n, float o);
	Plane();
	bool Trace(const Ray& r, IntersectInfo& info) const override;
	bool Test(const Ray& r) const override;

private:
	glm::vec3 _normal;
	float _offset;
};

}
