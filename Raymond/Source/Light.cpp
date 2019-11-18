#include "Light.h"
#include "Utils.h"


using namespace glm;
using namespace Raymond;

LightInfo PointLight::GetLightInfo(const glm::vec3& position)
{
	LightInfo info;
	vec3 direction = (Position + RandomOnUnitSphere() * Radius) - position;
	info.Distance = length(direction);
	direction /= info.Distance;
	info.Ray = Ray(position, direction);
	const float& r = info.Distance - Radius;

	const float intensity = Intensity / (r*r);

	//const float frac = (info.Distance / Radius + 1);
	//const float intensity = Intensity / (frac * frac);

	info.Color = this->Color * intensity;
	return info;
}

LightInfo DirecionalLight::GetLightInfo(const glm::vec3& position)
{
	LightInfo info;
	vec3 direction = -normalize(Direction);
	direction += RandomOnUnitSphere() * Radius;
	info.Distance = FLT_MAX;
	info.Ray = Ray(position, direction);
	info.Color = this->Color * Intensity;
	return info;
}
