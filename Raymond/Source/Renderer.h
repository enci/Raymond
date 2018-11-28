#pragma once

#include <memory>
#include <glm.hpp>
#include <vector>
#include <thread>

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

	Renderer() = default; // TODO: Set init params in here

	virtual ~Renderer();

	virtual glm::vec3 Trace(const Ray& ray);
	
	void Render();

	std::shared_ptr<Scene> Scene;

	std::shared_ptr<Sensor> Sensor;

	bool IsDone = false;

private:
	glm::vec3 Shade(const Light& light, const IntersectInfo& info) const;
	std::vector<std::unique_ptr<std::thread>> _workers;
};

}
