#pragma once

#include "Traceable.h"

namespace Raymond
{

class Box : public Traceable
{
public:
	Box(const glm::vec3& position, const glm::vec3& extent);
	bool Trace(const Ray& r, IntersectInfo& info) const override;

private:
	glm::vec3 _position;
	glm::vec3 _extent;
};

}

