#include <iostream>
#include "SDL.h"
#include "glm.hpp"
#include "Source/Color.h"
#include "Source/Camera.h"
#include "Source/Sphere.h"

using namespace std;
using namespace glm;
using namespace Raymond;

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

int main(int argc, char* args[])
{
	if (SDL_Init(SDL_INIT_VIDEO) != 0)
	{
		std::cout << "SDL_Init Error: " << SDL_GetError() << std::endl;
		return 1;
	}

	SDL_Window* win = SDL_CreateWindow("Hello World!", 100, 100,
		kWidth, kHeight, SDL_WINDOW_SHOWN);

	if (win == nullptr)
	{
		std::cout << "SDL_CreateWindow Error: " << SDL_GetError() << std::endl;
		SDL_Quit();
		return 1;
	}

	// The surface contained by the window
	SDL_Surface* screenSurface = nullptr;

	// The image we will load and show on the screen
	SDL_Surface* helloWorld = nullptr;

	// vec3 origin(0.0f, 0.0f, 0.0f);
	// vec3 lower_left_corner(-2.0f, -1.0f, -1.0f);
	// vec3 horizontal(4.0f, 0.0f, 0.0f);
	// vec3 vertical(0.0f, 2.0f, 0.0f);

	Camera camera(
		vec3(0.0f, 5.0f, 1.0f),
		vec3(0.0f, 0.0f, 0.0f),
		vec3(0.0f, 0.0f, 1.0f),
		60.0f,
		16.0f / 9.0f);

	Sphere sphere(vec3(0.0f, 0.0f, 0.0f), 1.0f);

	// Rendering
	screenSurface = SDL_GetWindowSurface(win);
	SDL_LockSurface(screenSurface);
	for (int x = 0; x < screenSurface->w; ++x)
	{
		for (int y = 0; y < screenSurface->h; ++y)
		{
			float s = float(x) / float(kWidth);
			float t = float(y) / float(kHeight);

			Ray ray = camera.GetRay(s, t);
			IntersectInfo info;

			char c = 0;
			if (sphere.Trace(ray, info))
				c = char(255);

			Color32 color;
			color.r = c;
			color.g = c;
			color.b = c;
			color.a = char(255);
			putpixel(screenSurface, x, y, color.Int);


			/*
			Raymond::Color32 color;
			color.r = rand() % 255;
			color.g = rand() % 255;
			color.b = rand() % 255;
			color.a = char(255);
			// auto color = (abs(640 / 2 - x) < 30 && abs(480 / 2 - y) < 30) ? 0x00FFFFFF : rand();
			putpixel(screenSurface, x, y, color.Int);
			*/
		}
	}
	SDL_UnlockSurface(screenSurface);
	SDL_UpdateWindowSurface(win);

	SDL_Event event;
	bool quit = false;	

	while (!quit)
	{
		while (SDL_PollEvent(&event))
		{
			if (event.type == SDL_QUIT)
			{
				quit = true;
			}
		}		
		SDL_Delay(16);
	}
			
	return 0;
}
