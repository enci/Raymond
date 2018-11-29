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
		
	void Render();

	float GetProgress();

	std::shared_ptr<Scene> Scene;

	std::shared_ptr<Sensor> Sensor;

	bool IsDone = false;

	int MaxBounces = 8;

	int Samples = 64;

private:
	virtual glm::vec3 Trace(const Ray& ray, int bounce);
	glm::vec3 Shade(const Light& light, const IntersectInfo& info) const;
	std::vector<std::unique_ptr<std::thread>>	_threads;
	std::vector<int>							_progress;
	int											_numberOfThreads	= 4;
};

}
