#pragma once
#include <vector>
#include "Light.h"

namespace Raymond
{
class Ray;
class Traceable;
struct IntersectInfo;

class Renderer
{	
public:
	virtual ~Renderer() = default;
	virtual glm::vec3 Trace(const Ray& ray, IntersectInfo& info);
	std::vector<Traceable*> Scene;
	std::vector<Light> Lights;
};

}
