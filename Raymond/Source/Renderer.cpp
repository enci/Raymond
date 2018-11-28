#include <thread>
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
	static Material baseMaterial;
	const Material* material = info.Object.lock()->GetMaterial();
	if (!material) material = &baseMaterial;

	vec3 color = material->Texture ?
		material->Texture->GetColor(info.Position) :
		material->Color;

	vec3 output = color * material->Emissive;
	vec3 d = light.Position - info.Position;
	float r = length(d);
	d = normalize(d);
	float intensity = light.Intensity / (r*r);
	output += color * intensity * light.Color * clamp(dot(d, info.Normal), 0.0f, 1.0f);

	vec3 toEye = Scene->Camera->Origin() - info.Position;
	toEye = normalize(toEye);
	vec3 h = toEye + d;
	h = normalize(h);
	output += material->Specular * light.Color * intensity *
		pow(dot(h, info.Normal), material->SpecularPower);

	return output; 
}

void Renderer::Render()
{	
	auto fun = [&](/*int rows*/)
	{
		const int samples = 1;
		const int w = Sensor->Width;
		const int h = Sensor->Height;

		for (int y = 0; y < h; ++y)
		{
			for (int x = 0; x < w; ++x)
			{			
				vec3 fcolor(0.0f, 0.0f, 0.0f);
				for (int s = 0; s < samples; s++)
				{
					float u = float(x + (samples > 1 ? RandInRange(-0.5f, 0.5f) : 0.0f)) / float(w);
					float v = float(y + (samples > 1 ? RandInRange(-0.5f, 0.5f) : 0.0f)) / float(h);

					Ray ray = Scene->Camera->GetRay(u, v);
					IntersectInfo info;
					fcolor += Trace(ray, info);
				}
				fcolor /= samples;
				Color32 color = Sensor->SetPixel(x, y, fcolor);
			}
		}

		int m = 0;

		//LOG("Rows %d", rows);
	};

	worker = new std::thread(fun);
}

Renderer::~Renderer()
{
	worker->join();
}

vec3 Renderer::Trace(const Ray& ray, IntersectInfo& info)
{
	vec3 color(0.0f, 0.0f, 0.0f);

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

				if (t->GetMaterial() && t->GetMaterial()->Emissive >= 1.0f)
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
