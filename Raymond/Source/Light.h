#pragma once
#include "glm.hpp"
#include "Ray.h"

namespace Raymond
{

struct LightInfo
{
	Ray			Ray;
	float		Distance;
	glm::vec3	Color; // Actually radiance
};

class Light
{
public:
	glm::vec3 Color;
	float Intensity;
	virtual LightInfo GetLightInfo(const glm::vec3& position) = 0;
};

class PointLight : public Light
{
public:
	LightInfo GetLightInfo(const glm::vec3& position) override;
	glm::vec3 Position = glm::vec3(0.0f);
	float Radius = 1.0f;
};

/*
class SphereLight : public PointLight
{	
};
*/

}
