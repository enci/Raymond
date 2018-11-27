#include "Renderer.h"
#include "Traceable.h"
#include "Ray.h"
#include "Camera.h"

using namespace Raymond;
using namespace glm;

vec3 Renderer::Shade(const Light& light, const IntersectInfo& info) const
{
	static Material baseMaterial;
	const Material* material = info.Object->GetMaterial();
	if (!material) material = &baseMaterial;

	vec3 color = material->Texture ?
		material->Texture->GetColor(info.Position) :
		material->Color;

	vec3 output = color * material->Emissive;
	vec3 d = light.Position - info.Position;
	float r = d.length();
	d = normalize(d);
	float intensity = light.Intensity / (r*r);
	output += color * intensity * light.Color * clamp(dot(d, info.Normal), 0.0f, 1.0f);

	vec3 toEye = camera->Origin() - info.Position;
	toEye = normalize(toEye);
	vec3 h = toEye + d;
	h = normalize(h);
	output += material->Specular * light.Color * intensity *
		pow(dot(h, info.Normal), material->SpecularPower);

	return output; 
}

vec3 Renderer::Trace(const Ray& ray, IntersectInfo& info)
{
	vec3 color(0.0f, 0.0f, 0.0f);

	float nearest = FLT_MAX;
	for (auto t : Scene)
	{
		IntersectInfo tInfo;
		tInfo.Object = t;
		if(t->Trace(ray, tInfo))
		{
			if(tInfo.Distance < nearest && tInfo.Distance > 0.00001f)
			{
				info = tInfo;
				nearest = info.Distance;
			}
		}
	}

	//color = (normalize(info.Normal) * 0.5f) + vec3(0.5f, 0.5f, 0.5f); // Shade(l, info);

	if(nearest != FLT_MAX)
	{
		for (auto l : Lights)
		{
			vec3 direction = l->Position - info.Position;
			int tmax = length(direction);
			direction /= tmax;
			vec3 position = info.Position; // + direction * 0.001f;
			Ray shadowRay(position, direction);

			bool lit = true;
			
			for (auto t : Scene)
			{
				if(info.Object == t)
					continue;

				if(t->Test(shadowRay, tmax))
				{
					lit = false;
					break;
				}
			}

			if(lit)
			{
				color += Shade(*l, info);
			}
		}
	}
	
	return color;
}
