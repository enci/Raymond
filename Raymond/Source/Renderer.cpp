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
#include <omp.h>

using namespace Raymond;
using namespace glm;
using namespace std;

const float kGeometricEpsilon = 0.0001f;

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
	const int w = Sensor->Width;
	const int h = Sensor->Height;
	this->Sensor->Clear();

	for (int s = 0; s < Samples; s++)
	{
		//#pragma omp parallel simd //for reduction(+:sum) private(this->Se)
		#pragma omp parallel for schedule(dynamic)
		for (int y = 0; y < h; ++y)
		{
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
	}

	/*
	auto task = [&](int taskID)
	{
		const int w = Sensor->Width;
		const int h = Sensor->Height;

		for (int s = 0; s < Samples; s++)
		{
			this->Sensor->Clear();
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

	for (const auto& w : _threads)
	{
		w->join();
	}
	*/
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
	// info.Distance = FLT_MAX;
	float distance = FLT_MAX;
	const auto& objects = Scene->Objects;

	_bvh->Trace(ray, info);

	// use the fancy shadow acne offset (epsilon) with using N.L^3 (and 1-N.L^3) in weight an offset along
	// the normal and along the ray, so that at grazing angles you offset the origin

	/*
	for (const auto& obj : objects)
	{
		IntersectInfo tInfo;
		if (obj->Trace(ray, tInfo))
		{
			if(tInfo.Distance < distance)
			{
				distance = tInfo.Distance;
				info = tInfo;
				info.Object = obj;
			}
		}
	}
	*/

	if(info.Distance >= kGeometricEpsilon)
	{
		const auto& lights = Scene->Lights;
		const auto object = info.Object.lock();
		auto& material = object->GetMaterial();

		color += material.GetColor(info.Position) * material.Emissive;

		// color += material.GetColor(info.Position);
		// return color;

		// float dist = info.Distance / 10.0f;
		// return vec3(dist, dist, dist);


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

			Ray reflectedRay(origin + reflected * kGeometricEpsilon, reflected);
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
			const Ray reflectedRay(origin + reflected * kGeometricEpsilon, reflected);
			color += Trace(reflectedRay, bounce + 1) * material.Reflectance;
		}		

		// Collect all lights
		for (const auto& l : lights)
		{
			auto lightInfo = l->GetLightInfo(info.Position);			
			const Ray& shadowRay = lightInfo.Ray;

			vec3 shade(1.0f, 1.0f, 1.0f);

			IntersectInfo shadowInfo;
			_bvh->Trace(shadowRay, shadowInfo);			
			if(!shadowInfo.Object.expired() &&
				shadowInfo.Distance > kGeometricEpsilon &&
				shadowInfo.Distance < lightInfo.Distance)
			{
				const auto& mat = shadowInfo.Object.lock()->GetMaterial();
				shade *= mat.Transparency * mat.Color;
			}

			/*
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
			*/
			
			color += Shade(lightInfo, info) * shade;			
		}

		/*
		if (Scene->FogDistance > 0.0f)
		{
			float normDist = info.Distance / Scene->FogDistance;
			color = mix(color, Scene->BackgroundColor, normDist);
		}
		*/
	}
	else
	{
		color = Scene->BackgroundColor;
	}		
	
	return color;
}
