#pragma once
#include <vector>
#include "Light.h"
#include "Material.h"

namespace Raymond
{
class Ray;
class Traceable;
class Camera;
struct IntersectInfo;

class Renderer
{	
public:
	virtual ~Renderer() = default;
	virtual glm::vec3 Trace(const Ray& ray, IntersectInfo& info);
	glm::vec3 Shade(const Light& light, const IntersectInfo& info) const;
	std::vector<Traceable*> Scene;
	std::vector<Light*> Lights;
	Camera* camera = nullptr;
};

}
