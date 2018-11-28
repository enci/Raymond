
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

const int threadsNum = 4;

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
	auto task = [&](int yfrom, int yto)
	{
		const int samples = 64; // TODO: Move to some settings and a member
		const int w = Sensor->Width;
		const int h = Sensor->Height;

		for (int y = yfrom; y < yto; ++y)
		{
			for (int x = 0; x < w; ++x)
			{			
				vec3 color(0.0f, 0.0f, 0.0f);

				for (int s = 0; s < samples; s++)
				{
					float u = float(x + (samples > 1 ? RandInRange(-0.5f, 0.5f) : 0.0f)) / float(w);
					float v = float(y + (samples > 1 ? RandInRange(-0.5f, 0.5f) : 0.0f)) / float(h);

					Ray ray = Scene->Camera->GetRay(u, v);
					color += Trace(ray);
				}
				color /= samples;
				auto& c = Sensor->SetPixel(x, y, color);
			}
		}
	};

	
	int rowsPerThread = Sensor->Height / threadsNum;
	int yfrom = 0;
	int yto = 0;
	for (; yto < Sensor->Height; yfrom += rowsPerThread)
	{		
		yto = yfrom + rowsPerThread;
		if (yto > Sensor->Height)
			yto = Sensor->Height;
		_workers.push_back(std::make_unique<std::thread>(task, yfrom, yto));
	}
}

Renderer::~Renderer()
{
	for (const auto& w : _workers)
	{
		w->join();
	}
}

vec3 Renderer::Trace(const Ray& ray)
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
			if(tInfo.Distance < nearest && tInfo.Distance > 0.00001f)
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

		if(material.Transparency > 0.0f)
		{
			// Check if it's back facing
			vec3 n = info.Normal;
			vec3 d = ray.Direction;
			float dn = dot(d, n);

			if(dn < 0)	// Enter material
			{
				auto reflected = reflect(ray.Direction, info.Normal);
				auto refracted = refract(ray.Direction, info.Normal, 1.0f / material.RefractiveIndex);
				auto origin = info.Position;

				Ray rfRay(origin, reflected);
				//rfRay.Origin += rfRay.Direction * 0.001f;
				vec3 rfCol = Trace(rfRay);
				
				Ray rrRay(origin, refracted);
				// rrRay.Origin += rrRay.Direction * 0.001f;
				vec3 rrCol = Trace(rrRay);
				
				rrCol = rrCol * (0.5f * (1.0f + dn));
				color += rfCol + rrCol;
			}
			else
			{
				auto reflected = reflect(ray.Direction, -info.Normal);
				auto refracted = refract(ray.Direction, -info.Normal, material.RefractiveIndex / 1.0f);
				auto origin = info.Position;

				Ray rfRay(origin, reflected);
				//rfRay.Origin += rfRay.Direction * 0.001f;
				vec3 rfCol = Trace(rfRay);

				Ray rrRay(origin, refracted);
				// rrRay.Origin += rrRay.Direction * 0.001f;
				vec3 rrCol = Trace(rrRay);

				rrCol = rrCol * (1.0f - dn);
				color += rfCol + rrCol;
			}
		}


		if(material.Reflectance > 0.0f)
		{
			auto reflected = reflect(ray.Direction, info.Normal);
			auto origin = info.Position;
			Ray reflectedRay(origin, reflected);
			color += Trace(reflectedRay) * material.Reflectance;
		}		

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
