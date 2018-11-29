
#include "Renderer.h"
#include "Traceable.h"
#include "Ray.h"
#include "Scene.h"
#include "Camera.h"
#include "Light.h"
#include "Material.h"
#include "Sensor.h"
#include "Defines.h"

using namespace Raymond;
using namespace glm;

vec3 Renderer::Shade(const Light& light, const IntersectInfo& info) const
{
	const Material& material = info.Object.lock()->GetMaterial();
	
	vec3 color = material.Texture ?
		material.Texture->GetColor(info.Position) :
		material.Color;

	vec3 output = color * material.Emissive;
	vec3 d = light.Position - info.Position;
	float r = length(d);
	d = normalize(d);
	float intensity = light.Intensity / (r*r);
	output += color * intensity * light.Color * clamp(dot(d, info.Normal), 0.0f, 1.0f);

	vec3 toEye = Scene->Camera->Origin() - info.Position;
	toEye = normalize(toEye);
	vec3 h = toEye + d;
	h = normalize(h);
	output += material.Specular * light.Color * intensity *
		pow(dot(h, info.Normal), material.SpecularPower);

	return output; 
}

void Renderer::Render()
{	
	auto task = [&](int taskID)
	{
		const int w = Sensor->Width;
		const int h = Sensor->Height;

		for (int s = 0; s < Samples; s++)
		{
			for (int y = 0; y < h; ++y)
			{
				if(y % _numberOfThreads != taskID)
					continue;

				for (int x = 0; x < w; ++x)
				{							
					float u = float(x + (Samples > 1 ? RandInRange(-0.5f, 0.5f) : 0.0f)) / float(w);
					float v = float(y + (Samples > 1 ? RandInRange(-0.5f, 0.5f) : 0.0f)) / float(h);

					Ray ray = Scene->Camera->GetRay(u, v);
					auto color = Trace(ray, 0);
					this->Sensor->AddSample(x, y, color);
				}
			}
			_progress[taskID]++;
		}
	};
		
	for (int id = 0; id < _numberOfThreads; id++)
	{
		_threads.push_back(std::make_unique<std::thread>(task, id));
		_progress.push_back(0.0f);
	}
}

float Renderer::GetProgress()
{
	float total = 0.0f;
	for (auto p : _progress)	
		total += float(p) / float(Samples);

	return total / float(_progress.size());
}


Renderer::~Renderer()
{
	for (const auto& w : _threads)
	{
		w->join();
	}
}

vec3 Renderer::Trace(const Ray& ray, int bounce)
{
	vec3 color(0.0f, 0.0f, 0.0f);
	IntersectInfo info;

	const auto& objects = Scene->Objects;

	float nearest = FLT_MAX;
	for (auto t : objects)
	{
		IntersectInfo tInfo;
		tInfo.Object = t;
		if(t->Trace(ray, tInfo))
		{
			if(tInfo.Distance < nearest && tInfo.Distance > 0.0001f)
			{
				info = tInfo;
				nearest = info.Distance;
			}
		}
	}

	const auto& lights = Scene->Lights;

	if(nearest != FLT_MAX)
	{
		auto object = info.Object.lock();
		auto material = object->GetMaterial();

		// Hande transparent materials
		if(material.Transparency > 0.0f && bounce < MaxBounces)
		{
			// Check if it's back facing
			vec3 n = info.Normal;
			vec3 d = ray.Direction;
			auto origin = info.Position;
			float dn = dot(d, n);
			vec3 reflected;
			vec3 refracted;
			float fresnel = 0.0f;

			if(dn < 0)	// Enter material
			{
				reflected = reflect(d, n);
				refracted = refract(d, n, 1.0f / material.RefractiveIndex);

				if (refracted != vec3(0, 0, 0))
					fresnel = (0.5f * (1.0f + dn));
			}
			else // Exit material
			{
				reflected = reflect(d, -n);
				refracted = refract(d, -n, material.RefractiveIndex / 1.0f);

				if (refracted != vec3(0, 0, 0))
					fresnel = (1.0f - dn);
				else; // Total internal reflection. Not yet implmented				
			}

			Ray reflectedRay(origin, reflected);
			reflectedRay.Origin += reflectedRay.Direction * 0.001f;
			vec3 reflectedColor = Trace(reflectedRay, bounce + 1);

			Ray refractedRay(origin, refracted);
			refractedRay.Origin += refractedRay.Direction * 0.001f;
			vec3 refractedColor = Trace(refractedRay, bounce);

			reflectedColor *= fresnel;
			color += reflectedColor + refractedColor;
		}

		// Handle reflective materials
		if(material.Reflectance > 0.0f && bounce < MaxBounces)
		{
			auto reflected = reflect(ray.Direction, info.Normal);
			auto origin = info.Position;
			Ray reflectedRay(origin, reflected);
			color += Trace(reflectedRay, bounce + 1) * material.Reflectance;
		}		

		// Collect all lights
		for (const auto& l : lights)
		{
			vec3 direction = l->Position - info.Position;
			float tmax = length(direction);
			direction /= tmax;
			vec3 position = info.Position; // + direction * 0.001f;
			Ray shadowRay(position, direction);

			bool lit = true;
			
			for (const auto& t : objects)
			{
				if(info.Object.lock().get() == t.get())
					continue;
				
				// if (t->GetMaterial() && t->GetMaterial().get()->Emissive >= 1.0f)
				//	continue;

				IntersectInfo tInfo;

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
