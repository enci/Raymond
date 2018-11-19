#pragma once
#include "glm.hpp"

namespace Raymond
{

enum class LightType { Directional, Point };

class Light
{
public:
	union
	{
		glm::vec3 Position;
		glm::vec3 Direction;
	};
	glm::vec3 Color;
	float Intensity;
	LightType Type;
};

}
