#include <iostream>
#include <string>
#include "SDL.h"
#include "glm.hpp"
#include "gtc/matrix_transform.hpp"
#include "Source/Color.h"
#include "Source/Camera.h"
#include "Source/Sphere.h"
#include "Source/Renderer.h"
#include "Source/Plane.h"
#include "Source/Box.h"
#include "Source/Material.h"
#include "Source/Scene.h"
#include "Source/Light.h"
#include "Source/Sensor.h"
#include "Source/Defines.h"
#include "Source/Cone.h"

using namespace std;
using namespace glm;
using namespace Raymond;

bool IsValidHsv(double h, double s, double v)
{
	return
		h >= 0.0 && h <= 360.0 &&
		s >= 0.0 && s <= 1.0 &&
		v >= 0.0 && v <= 1.0;
}

vec3 CreateFromHsv(double h, double s, double v)
{
	double c = 0.0, m = 0.0, x = 0.0;
	vec3 color(0.0f);
	if (IsValidHsv(h, s, v))
	{
		c = v * s;
		x = c * (1.0 - fabs(fmod(h / 60.0, 2) - 1.0));
		m = v - c;
		if (h >= 0.0 && h < 60.0)
		{
			color = vec3(c + m, x + m, m);
		}
		else if (h >= 60.0 && h < 120.0)
		{
			color = vec3(x + m, c + m, m);
		}
		else if (h >= 120.0 && h < 180.0)
		{
			color = vec3(m, c + m, x + m);
		}
		else if (h >= 180.0 && h < 240.0)
		{
			color = vec3(m, x + m, c + m);
		}
		else if (h >= 240.0 && h < 300.0)
		{
			color = vec3(x + m, m, c + m);
		}
		else if (h >= 300.0 && h < 360.0)
		{
			color = vec3(c + m, m, x + m);
		}
		else
		{
			color = vec3(m, m, m);
		}
	}
	return color;
}

class Checkerboard : public SolidTexture
{
public:
	~Checkerboard() override = default;
	vec3 GetColor(const vec3& position) const override;
};

void Putpixel(SDL_Surface *surface, int x, int y, Uint32 pixel)
{
	int bpp = surface->format->BytesPerPixel;
	// Here p is the address to the pixel we want to set
	Uint8 *p = (Uint8*)surface->pixels + y * surface->pitch + x * bpp;

	switch (bpp) {
	case 1:
		*p = pixel;
		break;

	case 2:
		*(Uint16 *)p = pixel;
		break;

	case 3:
		if (SDL_BYTEORDER == SDL_BIG_ENDIAN) {
			p[0] = (pixel >> 16) & 0xff;
			p[1] = (pixel >> 8) & 0xff;
			p[2] = pixel & 0xff;
		}
		else {
			p[0] = pixel & 0xff;
			p[1] = (pixel >> 8) & 0xff;
			p[2] = (pixel >> 16) & 0xff;
		}
		break;

	case 4:
		*(Uint32 *)p = pixel;
		break;
	}
}

SDL_Surface* CreateSurface(Sensor* sensor)
{
	auto surface = SDL_CreateRGBSurfaceWithFormatFrom(
		sensor->Samples,
		sensor->Width,
		sensor->Height,
		32,
		sensor->Width * 4,
		SDL_PIXELFORMAT_RGBA32);
	ASSERT(surface);
	return surface;
}

void UpdateSurface(SDL_Surface* surface, Sensor* sensor)
{
	ASSERT(surface->w == sensor->Width);
	ASSERT(surface->h == sensor->Height);

	for (int x = 0; x < sensor->Width; ++x)
		for (int y = 0; y < sensor->Height; ++y)
			Putpixel(surface, x, y, sensor->GetPixel(x, y).Int);
}

int kWidth = 600;
int kHeight = 600;

string MakeProgressBar(const float progress)
{
	int l = 50;
	string bar;
	bar.resize(l + 2);
	int p = int(floor(progress * l));
	bar[0] = '|';
	for (int i = 1; i < l + 1; i++)
		bar[i] = i <= p ? '|' : ' ';
	bar[l+1] = '|';
	return bar +  " " + to_string(int(progress * 100)) + "%";
}



Scene* CreateBoxes()
{
	kWidth = 640;
	kHeight = 480;
	
	Scene* scene = new Scene();
	mat4 transform = mat4(1.0f);

	auto plane = make_shared<Plane>();	

	const float lightRad = 0.18f;
	auto mainLight = make_shared<DirecionalLight>();
	mainLight->Color = vec3(1.0f, 1.0f, 1.0f);
	mainLight->Intensity = 1.6f;
	mainLight->Direction = normalize(vec3(-1.0f, -1.0f, -1.0f));
	mainLight->Radius = 0.3f;

	float boxesPerAngle = 28;
	float r = 1.2f;
	float dr = 0.6f;
	for (int ring = 0; ring < 10; ring++)
	{		
		float t = 0.0f;
		int n = int(boxesPerAngle * r);
		float dt = pi<float>() * 2.0f / float(n);		
		for (int i = 0; i < n; i++)
		{
			float sx = RandInRange(0.05f, 0.20f);
			float sy = RandInRange(0.05f, 0.20f);
			float sz = RandInRange(0.35f, 0.60f);
			auto box = make_shared<Box>(vec3(0.0f, 0.0f, 0.0f), vec3(sx, sx, sz));
			float x = r * cos(t) + RandInRange(-0.1f, 0.1f);
			float y = r * sin(t) + RandInRange(-0.1f, 0.1f);
			vec3 pos(x, y, 0.0f);
			transform = mat4(1.0f);
			transform = translate(transform, pos);
			transform = rotate(transform, RandInRange(radians(90.0f), radians(90.0f)), vec3(0.0f, 0.0f, 1.0f));
			transform = rotate(transform, RandInRange(radians(-25.0f), radians(25.0f)), vec3(1.0f, 0.0f, 0.0f));
			transform = rotate(transform, RandInRange(radians(-25.0f), radians(25.0f)), vec3(0.0f, 1.0f, 0.0f));
			box->SetTransform(transform);
			scene->Objects.push_back(box);

			auto mat = make_shared<Material>();
			/*mat->Color = vec3(
				RandInRange(0.4f, 1.0f),
				RandInRange(0.4f, 1.0f),
				RandInRange(0.4f, 1.0f));
			*/
			mat->Color = CreateFromHsv(
				RandInRange(0.0f, 360.0f),
				RandInRange(0.6f, 0.8f),
				RandInRange(0.8f, 1.0f));
			box->SetMaterial(mat);

			t += dt;
		}
		r += dr;
	}

	auto sphere = make_shared<Sphere>(vec3(0.0f, 0.0f, 0.15f), 0.15f);
	auto mirror = make_shared<Material>();
	mirror->Color = vec3(0.8f, 0.8f, 1.0f);
	mirror->Reflectance = 1.0f;
	sphere->SetMaterial(mirror);
		
	scene->Objects.push_back(plane);
	scene->Objects.push_back(sphere);
	scene->Lights.push_back(mainLight);

	scene->Camera = make_shared<Camera>(
		vec3(0.0f, 2.5f, 1.45f),
		vec3(0.0f, 0.0f, 0.0f),
		vec3(0.0f, 0.0f, 1.0f),
		60.0f,
		float(kWidth) / float(kHeight));

	return scene;
}

Scene* CreateBVHTest()
{
	kWidth = 640;
	kHeight = 480;

	Scene* scene = new Scene();
	mat4 transform = mat4(1.0f);

	auto plane = make_shared<Box>(vec3(0.0f, 0.0f, 0.0f), vec3(50.0f, 50.0f, 0.1f));

	const float lightRad = 0.18f;
	auto mainLight = make_shared<DirecionalLight>();
	mainLight->Color = vec3(1.0f, 1.0f, 1.0f);
	mainLight->Intensity = 1.6f;
	mainLight->Direction = normalize(vec3(-1.0f, -1.0f, -1.0f));
	mainLight->Radius = 0.2f;

	float boxesPerAngle = 28;
	float r = 1.6f;
	float dr = 0.62f;
	for (int ring = 0; ring < 16; ring++)
	{
		float t = 0.0f;
		int n = int(boxesPerAngle * r);
		float dt = pi<float>() * 2.0f / float(n);
		for (int i = 0; i < n; i++)
		{
			float sx = RandInRange(0.05f, 0.20f);
			float sy = RandInRange(0.05f, 0.20f);
			float sz = RandInRange(0.35f, 0.60f);
			auto box = make_shared<Box>(vec3(0.0f, 0.0f, 0.0f), vec3(sx, sx, sz));
			//auto box = make_shared<Sphere>(vec3(0.0f, 0.0f, 0.0f), sx);
			float R = r + RandInRange(-dr * 0.7f, dr * 0.7f);
			float x = R * cos(t) + RandInRange(-0.1f, 0.1f);
			float y = R * sin(t) + RandInRange(-0.1f, 0.1f);
			vec3 pos(x, y, 0.0f);
			//box->Center = pos;
			transform = mat4(1.0f);
			transform = translate(transform, pos);
			transform = rotate(transform, RandInRange(radians(90.0f), radians(90.0f)), vec3(0.0f, 0.0f, 1.0f));
			transform = rotate(transform, RandInRange(radians(-25.0f), radians(25.0f)), vec3(1.0f, 0.0f, 0.0f));
			transform = rotate(transform, RandInRange(radians(-25.0f), radians(25.0f)), vec3(0.0f, 1.0f, 0.0f));
			box->SetTransform(transform);
			scene->Objects.push_back(box);

			auto mat = make_shared<Material>();			
			mat->Color = CreateFromHsv(
				RandInRange(0.0f, 360.0f),
				RandInRange(0.6f, 0.8f),
				RandInRange(0.8f, 1.0f));
			box->SetMaterial(mat);

			t += dt;
		}
		//r += dr * sqrt(float(ring));
		r += dr * float(ring);
	}

	auto sphere = make_shared<Sphere>(vec3(0.0f, 0.0f, 0.15f), 0.15f);
	auto mirror = make_shared<Material>();
	mirror->Color = vec3(0.8f, 0.8f, 1.0f);
	mirror->Reflectance = 1.0f;
	sphere->SetMaterial(mirror);

	scene->Objects.push_back(plane);
	scene->Objects.push_back(sphere);
	scene->Lights.push_back(mainLight);

	scene->Camera = make_shared<Camera>(
		vec3(0.0f, 2.5f, 1.5f),
		vec3(0.0f, 0.0f, 0.0f),
		vec3(0.0f, 0.0f, 1.0f),
		60.0f,
		float(kWidth) / float(kHeight));

	scene->BackgroundColor = vec3(1.0f, 1.0f, 1.0f);
	//scene->FogDistance = 75.0f;
	
	return scene;
}

Scene* CreateCornellBox()
{
	Scene* scene = new Scene();
	mat4 transform = mat4(1.0f);
	auto sphere = make_shared<Sphere>(vec3(-0.5f, 0.2f, 0.25f), 0.25f);
	vec3 zero(0.0f, 0.0f, 0.0f);

	vec3 bottomPos(0.0f, 0.0f, -0.2f);
	auto bottom = make_shared<Box>(vec3(0.0f, 0.0f, 0.0f), vec3(1.0f, 1.0f, 0.2f));
	transform = mat4(1.0f);
	transform = translate(transform, bottomPos);
	bottom->SetTransform(transform);

	auto plane = make_shared<Plane>();

	vec3 topPos(0.0f, 0.0f, 2.2f);
	auto top = make_shared<Box>(vec3(0.0f, 0.0f, 0.0f), vec3(1.0f, 1.0f, 0.2f));
	transform = mat4(1.0f);
	transform = translate(transform, topPos);
	top->SetTransform(transform);

	vec3 backPos(0.0f, -1.2f, 1.0f);
	auto back = make_shared<Box>(vec3(0.0f, 0.0f, 0.0f), vec3(1.0f, 0.2f, 1.0f));
	transform = mat4(1.0f);
	transform = translate(transform, backPos);
	back->SetTransform(transform);

	vec3 leftPos(1.2f, 0.0f, 1.0f);
	auto left = make_shared<Box>(vec3(0.0f, 0.0f, 0.0f), vec3(0.2f, 1.0f, 1.0f));
	transform = mat4(1.0f);
	transform = translate(transform, leftPos);
	left->SetTransform(transform);

	vec3 rightPos(-1.2f, 0.0f, 1.0f);
	auto right = make_shared<Box>(vec3(0.0f, 0.0f, 0.0f), vec3(0.2f, 1.0f, 1.0f));
	transform = mat4(1.0f);
	transform = translate(transform, rightPos);
	right->SetTransform(transform);

	vec3 boxPos(0.2f, -0.3f, 0.6f);
	auto box = make_shared<Box>(vec3(0.0f, 0.0f, 0.0f), vec3(0.25f, 0.25f, 0.6f));
	transform = mat4(1.0f);
	transform = rotate(transform, 0.35f, vec3(0.0f, 0.0f, 1.0f));
	transform = translate(transform, boxPos);
	box->SetTransform(transform);

	auto glassSphere = make_shared<Sphere>(vec3(0.3f, 0.5f, 0.25f), 0.25f);

	const float lightRad = 0.18f;
	vec3 lightPos(0.0f, 0.0f, 1.8f);
	auto mainLight = make_shared<PointLight>();
	mainLight->Color = vec3(1.0f, 1.0f, 1.0f);
	mainLight->Intensity = 20.5f;
	mainLight->Position = lightPos;
	mainLight->Radius = lightRad;
	auto lightSphere = make_shared<Sphere>(lightPos, lightRad);
	
	auto red = make_shared<Material>();
	red->Color = vec3(1.0f, 0.0f, 0.0f);
	left->SetMaterial(red);

	auto green = make_shared<Material>();
	green->Color = vec3(0.0f, 1.0f, 0.0f);
	right->SetMaterial(green);

	auto blue = make_shared<Material>();
	blue->Color = vec3(0.0f, 0.0f, 1.0f);
	blue->Specular = 0.2f;
	blue->Reflectance = 0.5f;
	sphere->SetMaterial(blue);
	box->SetMaterial(blue);

	auto glass = make_shared<Material>();
	// glass->Specular = 0.2f;
	glass->Transparency = 0.8f;
	glass->Color = vec3(0.8f, 0.8f, 1.0f);
	glass->RefractiveIndex = 1.52f;
	glassSphere->SetMaterial(glass);

	auto lightMaterial = make_shared<Material>();
	lightMaterial->Color = vec3(1.0f, 1.0f, 1.0f);
	lightMaterial->Emissive = 1.0f;
	lightSphere->SetMaterial(lightMaterial);

	scene->Objects.push_back(sphere);
	scene->Objects.push_back(glassSphere);
	scene->Objects.push_back(bottom);
	scene->Objects.push_back(top);
	scene->Objects.push_back(back);
	scene->Objects.push_back(left);
	scene->Objects.push_back(right);
	scene->Objects.push_back(box);
	//scene->Objects.push_back(plane);
	scene->Objects.push_back(lightSphere);
	scene->Lights.push_back(mainLight);

	float r = 2.8f;
	constexpr float theta = pi<float>() * 0.5f;
	scene->Camera = make_shared<Camera>(
		vec3(r * cos(theta), r * sin(theta), 1.0f),
		vec3(0.0f, 0.0f, 1.0f),
		vec3(0.0f, 0.0f, 1.0f),
		60.0f,
		float(kWidth) / float(kHeight));

	return scene;
}

Scene* CreatePlane()
{
	Scene* scene = new Scene();
	mat4 transform = mat4(1.0f);
	//auto sphere = make_shared<Plane>(vec3(0.0f, 1.0f, 0.0f), 0.0f);
	auto plane = make_shared<Plane>();
	plane->SetTransform(transform);
	vec3 zero(0.0f, 0.0f, 0.0f);
	auto material = make_shared<Material>();
	material->Color = vec3(1.0f, 1.0f, 1.0f);
	material->Emissive = 1.0f;
	material->Texture = new Checkerboard();
	plane->SetMaterial(material);

	const float lightRad = 0.18f;
	vec3 lightPos(0.0f, 0.0f, 1.8f);
	auto mainLight = make_shared<PointLight>();
	mainLight->Color = vec3(1.0f, 1.0f, 1.0f);
	mainLight->Intensity = 20.5f;
	mainLight->Position = lightPos;
	mainLight->Radius = lightRad;

	scene->Objects.push_back(plane);
	//scene->Lights.push_back(mainLight);



	float r = 2.8f;
	constexpr float theta = pi<float>() * 0.5f;
	scene->Camera = make_shared<Camera>(
		vec3(r * cos(theta), r * sin(theta), 1.0f),
		vec3(0.0f, 0.0f, 1.0f),
		vec3(0.0f, 0.0f, 1.0f),
		60.0f,
		float(kWidth) / float(kHeight));

	return scene;
}

Scene* CreateWeek2()
{
	Scene* scene = new Scene();
	mat4 transform = mat4(1.0f);
	auto sphere = make_shared<Sphere>(vec3(0.0f, 0.0f, 1.0f), 1.0f);
	sphere->SetTransform(transform);
	vec3 zero(0.0f, 0.0f, 0.0f);
	auto material = make_shared<Material>();
	material->Color = vec3(1.0f, 0.0f, 0.0f);
	sphere->SetMaterial(material);

	auto plane = make_shared<Plane>();
	plane->SetTransform(transform);
	auto planeMaterial = make_shared<Material>();
	planeMaterial->Color = vec3(1.0f, 1.0f, 1.0f);
	planeMaterial->Texture = new Checkerboard();
	plane->SetMaterial(planeMaterial);

	vec3 lightPos(2.0f, 2.0f, 3.8f);
	auto mainLight = make_shared<PointLight>();
	mainLight->Color = vec3(1.0f, 1.0f, 1.0f);
	mainLight->Intensity = 10.5f;
	mainLight->Position = lightPos;
	mainLight->Radius = 0.0f;

	scene->Objects.push_back(sphere);
	scene->Objects.push_back(plane);
	scene->Lights.push_back(mainLight);

	float r = 5.8f;
	constexpr float theta = pi<float>() * 0.0f;
	scene->Camera = make_shared<Camera>(
		vec3(r * cos(theta), r * sin(theta), 1.0f),
		vec3(0.0f, 0.0f, 1.0f),
		vec3(0.0f, 0.0f, 1.0f),
		60.0f,
		float(kWidth) / float(kHeight));

	return scene;
}

Scene* CreateCone()
{
	Scene* scene = new Scene();
	mat4 transform = mat4(1.0f);
	vec3 zero(0.0f, 0.0f, 0.0f);

	float theta = pi<float>() * 0.13f;

	auto cone = make_shared<Cone>(vec3(0.0f, 0.0f, 2.0f), theta);

	auto plane = make_shared<Plane>();
	
	const float lightRad = 0.18f;
	vec3 lightPos(2.0f, 2.0f, 1.8f);
	auto mainLight = make_shared<PointLight>();
	mainLight->Color = vec3(1.0f, 1.0f, 1.0f);
	mainLight->Intensity = 4.5f;
	mainLight->Position = lightPos;
	mainLight->Radius = lightRad;
	auto lightSphere = make_shared<Sphere>(lightPos, lightRad);

	auto tree = make_shared<Material>();
	tree->Color = vec3(0.1f, 0.9f, 0.02);
	cone->SetMaterial(tree);

	auto lightMaterial = make_shared<Material>();
	lightMaterial->Color = vec3(1.0f, 1.0f, 1.0f);
	lightMaterial->Emissive = 1.0f;
	lightSphere->SetMaterial(lightMaterial);

	scene->Objects.push_back(plane);
	scene->Objects.push_back(cone);
	scene->Objects.push_back(lightSphere);
	scene->Lights.push_back(mainLight);

	/*
	float s = 0.0f;
	float t = 0.0f;
	float u = 0.0f;
	while(s < theta)
	{
		//for(float )
		while(t < pi<float>()* 2.0f)
		{
			while (u > -2.0f)
			{
				//vec3 F(u * tan(s) * cos(t), u * tan(s) * sin(t), u);
				//F.z += 2.1f;
				vec3 F(u * tan(s) * cos(t), u * tan(s) * sin(t), u);
				//F.z += 2.1f;

				auto light = make_shared<PointLight>();
				light->Color = vec3(1.0f, 1.0f, 1.0f);
				light->Intensity = 10.5f;
				light->Position = F;
				light->Radius = 0.1f;
				auto lightPoint = make_shared<Sphere>(F, 0.1f);

				// scene->Objects.push_back(light);
				scene->Lights.push_back(light);
				u -= 2.0f / 10.0f;
			}
			t += pi<float>()* 2.0f / 10.f;
		}
		s += theta / 5.f;
	}
	*/

	float h = 2.1f;
	float t = 0.0f;
	float speed = 80.0f;
	while (h > 0.0f)
	{
		//h = -pow(t, 2) + 2.1f;
		h = -t / tan(theta * 0.5f) + 2.1f;
		vec3 F(t * cos(t * speed), t * sin(t * speed), h);

		auto light = make_shared<PointLight>();
		light->Color = vec3(1.0f, 0.9f, 0.9f);
		light->Intensity = 10.5f;
		light->Position = F;
		light->Radius = 0.02f;
		auto lightPoint = make_shared<Sphere>(F, 0.01f);
		lightPoint->SetMaterial(lightMaterial);

		scene->Objects.push_back(lightPoint);
		scene->Lights.push_back(light);

		t += 0.01f;
	}

	float r = 2.8f;
	theta = pi<float>() * 0.5f;
	scene->Camera = make_shared<Camera>(
		vec3(r * cos(theta), r * sin(theta), 1.0f),
		vec3(0.0f, 0.0f, 1.0f),
		vec3(0.0f, 0.0f, 1.0f),
		60.0f,
		float(kWidth) / float(kHeight));

	return scene;
}


int main(int argc, char* args[])
{
	Renderer renderer;
	renderer.Scene = shared_ptr<Scene>(CreateBVHTest());
	renderer.Sensor = make_shared<Sensor>(kWidth, kHeight);
	renderer.Samples = 64;
	renderer.NumberOfThreads = 4;
	
	if (SDL_Init(SDL_INIT_VIDEO) != 0)
	{
		std::cout << "SDL_Init Error: " << SDL_GetError() << std::endl;
		return 1;
	}

	SDL_Window* win = SDL_CreateWindow("Raymond", 100, 100,
		kWidth, kHeight, SDL_WINDOW_SHOWN);

	if (win == nullptr)
	{
		std::cout << "SDL_CreateWindow Error: " << SDL_GetError() << std::endl;
		SDL_Quit();
		return 1;
	}

	SDL_Surface* screenSurface = nullptr;
	SDL_Event event;
	bool quit = false;		

	renderer.Render();

	screenSurface = SDL_GetWindowSurface(win);
	while (!quit)
	{
		while (SDL_PollEvent(&event))
		{
			if (event.type == SDL_QUIT)
			{
				quit = true;
				renderer.Stop();
			}
		}		
		
		SDL_LockSurface(screenSurface);	
		UpdateSurface(screenSurface, renderer.Sensor.get());
		SDL_UnlockSurface(screenSurface);
		SDL_UpdateWindowSurface(win);

		const auto progress = renderer.GetProgress();
		string name = "Raymond - " + MakeProgressBar(progress);
		SDL_Delay(1000 / 30);
		SDL_SetWindowTitle(win, name.c_str());
	}
			
	return 0;
}

vec3 Checkerboard::GetColor(const vec3& position) const
{
	vec3 white(1, 1, 1);
	vec3 black(0, 0, 0);
	int ox = sign(position.x) > 0 ? 0 : 1;
	int oy = sign(position.y) > 0 ? 0 : 1;
	return (int(abs(position.x) + ox) % 2 ^ int(abs(position.y) + oy) % 2) ? white : black;
}
