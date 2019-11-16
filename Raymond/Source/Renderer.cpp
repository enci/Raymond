#include "Renderer.h"
#include "Traceable.h"
#include "Ray.h"
#include "Scene.h"
#include "Camera.h"
#include "Light.h"
#include "Material.h"
#include "Sensor.h"
#include "Defines.h"
#include "Utils.h"
//#include "BVH.h"

using namespace Raymond;
using namespace glm;

vec3 Renderer::Shade(const LightInfo& lightInfo, const IntersectInfo& info) const
{
	const Material& material = info.Object.lock()->GetMaterial();
	
	const vec3 color = material.GetColor(info.Position);

	vec3 output(0.0f, 0.0f, 0.0f);
	vec3 d = lightInfo.Ray.Direction;	
	output += color * lightInfo.Color * clamp(dot(d, info.Normal), 0.0f, 1.0f);

	vec3 toEye = Scene->Camera->Origin() - info.Position;
	toEye = normalize(toEye);
	vec3 h = toEye + d;
	h = normalize(h);
	output += material.Specular * lightInfo.Color * pow(dot(h, info.Normal), material.SpecularPower);

	return output; 
}

void Renderer::Render()
{
	_bvh = std::make_unique<BVH>(Scene->Objects);
	
	auto task = [&](int taskID)
	{
		const int w = Sensor->Width;
		const int h = Sensor->Height;

		for (int s = 0; s < Samples; s++)
		{
			for (int y = 0; y < h; ++y)
			{
				if(y % NumberOfThreads != taskID)
					continue;

				for (int x = 0; x < w; ++x)
				{							
					float u = float(x + (Samples > 1 ? RandInRange(-0.5f, 0.5f) : 0.0f)) / float(w);
					float v = float(y + (Samples > 1 ? RandInRange(-0.5f, 0.5f) : 0.0f)) / float(h);

					Ray ray = Scene->Camera->GetRay(u, v);
					auto color = Trace(ray, 0);
					this->Sensor->AddSample(x, y, color);

					if (_stop)
						return;
				}
			}
			_progress[taskID]++;
		}
	};
		
	for (int id = 0; id < NumberOfThreads; id++)
	{
		_threads.push_back(std::make_unique<std::thread>(task, id));
		_progress.push_back(0);
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

	/*
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

		auto aabb = t->GetAABB();
		if(aabb.Trace(ray))
		{
			color += vec3(0.1f, 0.1f, 0.1f);
		}
	}
	*/
	
	// IntersectInfo bvhInfo;
	_bvh->Trace(ray, info);
	

	// if(nearest != FLT_MAX)
	if(info.Distance >= 0.0f)
	{
		//IntersectInfo bvhInfo;
		//_bvh->Trace(ray, bvhInfo);
		//assert(bvhInfo.Distance == info.Distance);
		//assert(bvhInfo.Position == info.Position);
		//assert(bvhInfo.Normal == info.Normal);

		const auto& lights = Scene->Lights;
		const auto object = info.Object.lock();
		auto& material = object->GetMaterial();

		color += material.GetColor(info.Position) * material.Emissive;

		// Handle transparent materials
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
				else; // Total internal reflection. Not yet implemented				
			}

			Ray reflectedRay(origin, reflected);
			reflectedRay.Origin += reflectedRay.Direction * 0.001f;
			vec3 reflectedColor = Trace(reflectedRay, bounce + 1);

			Ray refractedRay(origin, refracted);
			refractedRay.Origin += refractedRay.Direction * 0.001f;
			const vec3 refractedColor = Trace(refractedRay, bounce);

			reflectedColor *= fresnel;
			color += reflectedColor + refractedColor * material.Transparency * material.Color;
		}

		// Handle reflective materials
		if(material.Reflectance > 0.0f && bounce < MaxBounces)
		{
			const auto reflected = reflect(ray.Direction, info.Normal);
			const auto origin = info.Position;
			const Ray reflectedRay(origin, reflected);
			color += Trace(reflectedRay, bounce + 1) * material.Reflectance;
		}		

		// Collect all lights
		for (const auto& l : lights)
		{
			auto lightInfo = l->GetLightInfo(info.Position);			
			const Ray& shadowRay = lightInfo.Ray;

			vec3 shade(1.0f, 1.0f, 1.0f);
			
			for (const auto& t : objects)
			{
				if(info.Object.lock().get() == t.get())
					continue;
				
				if (t->GetMaterial().Emissive > 0.0f)
					continue;
				
				IntersectInfo shadowInfo;
				if (t->Trace(shadowRay, shadowInfo) && shadowInfo.Distance < lightInfo.Distance)
				{				
					const auto& mat = t.get()->GetMaterial();
					shade *=  mat.Transparency * mat.Color;
				}
			}
			
			color += Shade(lightInfo, info) * shade;
		}

		static float aoLength = 0.3f;
		float aoCount = 0.0f;
		if (material.Emissive <= 0.0f)
		{
			for (int i = 0; i < AOSamples; i++)
			{
				Ray aoRay(info.Position, RandomOnUnitHemisphere(info.Normal));
				aoRay.Origin += aoRay.Direction * 0.001f;

				for (const auto& t : objects)
				{
					if (t->GetMaterial().Emissive > 0.0f)
						continue;

					IntersectInfo aoInfo;
					if (t->Trace(aoRay, aoInfo) && aoInfo.Distance < aoLength)
					{
						const auto& mat = t.get()->GetMaterial();
						aoCount += 1.0f - mat.Transparency;

						if (aoCount >= 1.0f)
							break;
					}
				}
			}
			aoCount /= AOSamples;
		}
		color *= vec3(1.0f - aoCount);
	}

	if (Scene->FogDistance > 0.0f)
	{
		float normDist = info.Distance / Scene->FogDistance;
		color = mix(color, Scene->BackgroundColor, normDist);
	}
	
	return color;
}
