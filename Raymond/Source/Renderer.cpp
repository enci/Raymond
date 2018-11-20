#include "Renderer.h"
#include "Traceable.h"
#include "Ray.h"

using namespace Raymond;
using namespace glm;

vec3 Shade(const Light& light, const IntersectInfo& info)
{
	vec3 d = normalize(light.Position - info.Position);
	return light.Intensity * light.Color * dot(d, info.Normal);
}

glm::vec3 Renderer::Trace(const Ray& ray, IntersectInfo& info)
{
	vec3 color(0.0f, 0.0f, 0.0f);

	float nearest = FLT_MAX;
	for (auto t : Scene)
	{
		IntersectInfo tInfo;
		if(t->Trace(ray, tInfo))
		{
			if(tInfo.Distance < nearest && tInfo.Distance > 0.00001f)
			{
				info = tInfo;
				nearest = info.Distance;
			}
		}
	}

	if(nearest != FLT_MAX)
	{
		for (auto l : Lights)
		{
			vec3 direction = normalize(l.Position - info.Position);
			vec3 position = info.Position + direction * 0.00001f;
			Ray shadowRay(position, direction);

			bool lit = true;
			for (auto t : Scene)
			{

				if(t->Test(shadowRay))
				{
					lit = false;
				}
			}

			if(lit)
			{
				color += Shade(l, info);
			}
		}
	}

	return color;
}
