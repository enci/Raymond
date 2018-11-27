#include <iostream>
#include "SDL.h"
#include "glm.hpp"
#include "gtc/matrix_transform.hpp"
#include "Source/Color.h"
#include "Source/Camera.h"
#include "Source/Sphere.h"
#include "Source/Renderer.h"
#include "Source/Plane.h"
#include "Source/Box.h"

using namespace std;
using namespace glm;
using namespace Raymond;

/// Returns a random float between zero and 1
inline float RandFloat() { return static_cast<float>((rand()) / (RAND_MAX + 1.0)); }

/// Returns a random float between x and y
inline float RandInRange(float x, float y) { return x + RandFloat()*(y - x); }

class Checkerboard : public SolidTexture
{
public:
	~Checkerboard() override = default;
	vec3 GetColor(const vec3& position) const override;
};

void putpixel(SDL_Surface *surface, int x, int y, Uint32 pixel)
{
	int bpp = surface->format->BytesPerPixel;
	/* Here p is the address to the pixel we want to set */
	Uint8 *p = (Uint8 *)surface->pixels + y * surface->pitch + x * bpp;

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

int kWidth = 640;
int kHeight = 480;

void CreateCornellBox(Renderer& renderer)
{
	mat4 transform = mat4(1.0f);
	Sphere* sphere = new Sphere(vec3(0.0f, 0.0f, 0.25f), 0.25f);
	vec3 zero(0.0f, 0.0f, 0.0f);

	vec3 bottomPos(0.0f, 0.0f, -0.2f);
	Box* bottom = new Box(vec3(0.0f, 0.0f, 0.0f), vec3(1.0f, 1.0f, 0.2f));
	transform = mat4(1.0f);
	transform = translate(transform, bottomPos);
	bottom->SetTransform(transform);

	vec3 topPos(0.0f, 0.0f, 2.2f);
	Box* top = new Box(vec3(0.0f, 0.0f, 0.0f), vec3(1.0f, 1.0f, 0.2f));
	transform = mat4(1.0f);
	transform = translate(transform, topPos);
	top->SetTransform(transform);

	vec3 backPos(0.0f, -1.2f, 1.0f);
	Box* back = new Box(vec3(0.0f, 0.0f, 0.0f), vec3(1.0f, 0.2f, 1.0f));
	transform = mat4(1.0f);
	transform = translate(transform, backPos);
	back->SetTransform(transform);

	vec3 leftPos(1.2f, 0.0f, 1.0f);
	Box* left = new Box(vec3(0.0f, 0.0f, 0.0f), vec3(0.2f, 1.0f, 1.0f));
	transform = mat4(1.0f);
	transform = translate(transform, leftPos);
	left->SetTransform(transform);

	vec3 rightPos(-1.2f, 0.0f, 1.0f);
	Box* right = new Box(vec3(0.0f, 0.0f, 0.0f), vec3(0.2f, 1.0f, 1.0f));
	transform = mat4(1.0f);
	transform = translate(transform, rightPos);
	right->SetTransform(transform);

	vec3 lightPos(0.0f, 0.0f, 1.2f);
	Light* mainLight = new Light();
	mainLight->Color = vec3(1.0f, 1.0f, 1.0f);
	mainLight->Intensity = 5.0f;
	mainLight->Position = lightPos;
	mainLight->Type = LightType::Point;

	Sphere* lightSphere = new Sphere(lightPos, 0.05f);
	
	Material* red = new Material();
	red->Color = vec3(1.0f, 0.0f, 0.0f);
	left->SetMaterial(red);

	Material* green = new Material();
	green->Color = vec3(0.0f, 1.0f, 0.0f);
	right->SetMaterial(green);

	Material* lightMaterial = new Material();
	lightMaterial->Color = vec3(1.0f, 1.0f, 1.0f);
	lightMaterial->Emissive = 1.0f;
	lightSphere->SetMaterial(lightMaterial);

	//Material checker;
	//checker.Texture = new Checkerboard();
	//plane.SetMaterial(&checker);	
	//renderer.Scene.push_back(&plane);	
	//renderer.Scene.push_back(&sphere2);

	renderer.Scene.push_back(sphere);
	renderer.Scene.push_back(bottom);
	renderer.Scene.push_back(top);
	renderer.Scene.push_back(back);
	renderer.Scene.push_back(left);
	renderer.Scene.push_back(right);
	//renderer.Scene.push_back(lightSphere);

	//renderer.Lights.push_back(fillLight2);
	renderer.Lights.push_back(mainLight);
	
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

	// The surface contained by the window
	SDL_Surface* screenSurface = nullptr;

	SDL_Event event;
	bool quit = false;	

	float theta = pi<float>() * 0.5f;

	/*
	Sphere sphere(vec3(0.0f, 0.0f, 1.0f), 1.0f);
	Sphere sphere2(vec3(1.5f, 0.0f, 0.5f), 0.5f);
	mat4 transform = mat4(1.0f);
	//transform = scale(transform, vec3(1.0f, 1.0f, 5.0f));
	sphere2.SetTransform(transform);
	Plane plane;
	Box box(vec3(0.0f, 0.0f, 0.0f), vec3(0.6f, 0.6f, 2.5f));
	transform = mat4(1.0f);
	transform = rotate(transform, radians(45.0f), vec3(1.0f, 0.0f, 0.0f));
	transform = translate(transform, vec3(1.0f, 0.0f, 0.0f));
	//transform = scale(transform, vec3(2.0f, 2.0f, 2.0f));
	box.SetTransform(transform);

	Light mainLight;
	mainLight.Color = vec3(1.0f, 1.0f, 1.0f);
	mainLight.Intensity = 0.9f;
	mainLight.Position = vec3(5.0f, 10.f, 10.0f);
	mainLight.Type = LightType::Point;

	Light fillLight;
	fillLight.Color = vec3(1.0f, 1.0f, 1.0f);
	fillLight.Intensity = 0.2f;
	fillLight.Position = vec3(-10.0f, 10.f, 15.0f);
	fillLight.Type = LightType::Point;

	Light fillLight2;
	fillLight2.Color = vec3(1.0f, 1.0f, 1.0f);
	fillLight2.Intensity = 0.3f;
	fillLight2.Position = vec3(10.0f, -10.f, 15.0f);
	fillLight2.Type = LightType::Point;

	Material blue;
	blue.Color = vec3(0.0f, 0.0f, 1.0f);
	box.SetMaterial(&blue);

	Material red;
	red.Color = vec3(1.0f, 0.0f, 0.0f);
	red.Specular = 0.7f;
	red.SpecularPower = 32.0f;
	sphere.SetMaterial(&red);

	Material green;
	green.Color = vec3(0.0f, 1.0f, 0.0f);
	sphere2.SetMaterial(&green);

	Material checker;
	checker.Texture = new Checkerboard();
	plane.SetMaterial(&checker);

	Renderer renderer;
	renderer.Scene.push_back(&plane);
	renderer.Scene.push_back(&sphere);
	renderer.Scene.push_back(&sphere2);
	renderer.Scene.push_back(&box);
	renderer.Lights.push_back(&mainLight);
	renderer.Lights.push_back(&fillLight);
	renderer.Lights.push_back(&fillLight2);
	*/

	Renderer renderer;

	CreateCornellBox(renderer);

	//renderer.Lights.push_back(&fillLight);	;

	float r = 5.0f;

	while (!quit)
	{
		while (SDL_PollEvent(&event))
		{
			if (event.type == SDL_QUIT)
			{
				quit = true;
			}
		}		

		theta += 0.05f;
		Camera camera(
			vec3(r * cos(theta), r * sin(theta), 1.0f),
			vec3(0.0f, 0.0f, 1.0f),
			vec3(0.0f, 0.0f, 1.0f),
			60.0f,
			float(kWidth) / float(kHeight));

		renderer.camera = &camera;
		const int samples = 8;

		// Rendering
		screenSurface = SDL_GetWindowSurface(win);
		SDL_LockSurface(screenSurface);
		for (int x = 0; x < screenSurface->w; ++x)
		{
			for (int y = 0; y < screenSurface->h; ++y)
			{
				vec3 fcolor(0.0f, 0.0f, 0.0f);
				for (int s = 0; s < samples; s++)
				{
					float u = float(x + RandInRange(-0.5f, 0.5f)) / float(kWidth);
					float v = float(y + RandInRange(-0.5f, 0.5f)) / float(kHeight);

					Ray ray = camera.GetRay(u, v);
					IntersectInfo info;
					fcolor += renderer.Trace(ray, info);
				}
				fcolor /= samples;
				const float exp = 1.0f / 2.2f;
				fcolor = pow(fcolor, vec3(exp, exp, exp));
				Color32 color = Color32(fcolor);
				
				putpixel(screenSurface, x, y, color.Int);
			}
		}
		SDL_UnlockSurface(screenSurface);
		SDL_UpdateWindowSurface(win);
	}
			
	return 0;
}

vec3 Checkerboard::GetColor(const vec3 & position) const
{
	vec3 white(1, 1, 1);
	vec3 black(0, 0, 0);
	return (int(position.x) % 2 ^ int(position.y) % 2) ? white : black;
}
