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

/// Base class for all lights
class Light
{
public:
	/// The color if this light
	glm::vec3 Color;

	/// Intensity (acts as a multiplier for the Color)
	float Intensity;

	/// Lights need implement this method and return a ray to the light,
	/// distance and radiance (LightInfo) so that lighting calculation
	/// can use it for shading.
	virtual LightInfo GetLightInfo(const glm::vec3& position) = 0;
};

/// Point light with some radius for softer shadows
class PointLight : public Light
{
public:
	LightInfo GetLightInfo(const glm::vec3& position) override;
	glm::vec3 Position = glm::vec3(0.0f);
	float Radius = 1.0f;
};

class DirecionalLight : public Light
{	
public:
	LightInfo GetLightInfo(const glm::vec3& position) override;
	glm::vec3 Direction = glm::vec3(0.0f);
	float Radius = 1.0f;
};

}
