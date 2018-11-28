#pragma once

#include <memory>
#include <glm.hpp>

namespace std
{
	class thread;
}

namespace Raymond
{

class Ray;
struct IntersectInfo;
struct Sensor;
struct Scene;
class Light;

class Renderer
{	
public:
	virtual ~Renderer();
	virtual glm::vec3 Trace(const Ray& ray, IntersectInfo& info);
	glm::vec3 Shade(const Light& light, const IntersectInfo& info) const;
	void Render();
	Scene* Scene = nullptr;
	std::shared_ptr<Sensor> Sensor;
	bool IsDone = false;
private:
	std::thread* worker = nullptr;
};

}
