#pragma once

#include <memory>
#include <glm.hpp>

namespace Raymond
{

class Ray;
struct IntersectInfo;
struct Sensor;
struct Scene;
struct Light;

class Renderer
{	
public:
	virtual ~Renderer() = default;
	virtual glm::vec3 Trace(const Ray& ray, IntersectInfo& info);
	glm::vec3 Shade(const Light& light, const IntersectInfo& info) const;
	void Render();
	Scene* Scene = nullptr;
	std::shared_ptr<Sensor> Sensor;
	bool IsDone = false;
};

}
