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

using namespace std;
using namespace glm;
using namespace Raymond;

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


	return scene;
}

int main(int argc, char* args[])
{
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
	float theta = pi<float>() * 0.5f;
	Renderer renderer;

	renderer.Scene = shared_ptr<Scene>(CreateCornellBox());
	renderer.Sensor = make_shared<Sensor>(kWidth, kHeight);
	float r = 2.8f;
	renderer.Scene->Camera = make_shared<Camera>(
		vec3(r * cos(theta), r * sin(theta), 1.0f),
		vec3(0.0f, 0.0f, 1.0f),
		vec3(0.0f, 0.0f, 1.0f),
		60.0f,
		float(kWidth) / float(kHeight));
	renderer.Samples = 400;
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
		SDL_SetWindowTitle(win, name.c_str());
	}
			
	return 0;
}

vec3 Checkerboard::GetColor(const vec3 & position) const
{
	vec3 white(1, 1, 1);
	vec3 black(0, 0, 0);
	return (int(position.x) % 2 ^ int(position.y) % 2) ? white : black;
}
