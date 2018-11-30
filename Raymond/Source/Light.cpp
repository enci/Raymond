#include "Light.h"
#include "Defines.h"

using namespace glm;
using namespace Raymond;

vec3 RandomOnUnitSphere()
{
	vec3 v;
	do
	{
		v = vec3(	RandInRange(-1.0f, 1.0f),
					RandInRange(-1.0f, 1.0f),
					RandInRange(-1.0f, 1.0f));
	}
	while (length(v) > 1.0f);
	return normalize(v);
}

LightInfo PointLight::GetLightInfo(const glm::vec3& position)
{
	LightInfo info;
	vec3 direction = (Position + RandomOnUnitSphere() * Radius) - position;
	info.Distance = length(direction);
	direction /= info.Distance;
	info.Ray = Ray(position, direction);
	const float& r = info.Distance - Radius;

	//const float intensity = Intensity / (r*r);

	const float frac = (info.Distance / Radius + 1);
	const float intensity = Intensity / (frac * frac);

	info.Color = this->Color * intensity;
	return info;
}
