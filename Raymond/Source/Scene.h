#pragma once

#include <memory>
#include <vector>

namespace Raymond
{

class Ray;
class Traceable;
class Camera;

struct Scene
{
	std::vector<std::shared_ptr<Traceable>>		Objects;
	std::vector< std::shared_ptr<Light>>		Lights;
	std::shared_ptr<Camera>						Camera;
};

}