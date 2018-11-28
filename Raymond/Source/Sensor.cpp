#include "Sensor.h"
#include "Defines.h"

using namespace Raymond;

Sensor::Sensor(int width, int height)
	: Width(width)
	, Height(height)
	, Pixels(new Color32[width * height])
{
	for (int i = 0; i < width * height; ++i)
	{
		Color32& c = Pixels[i];
		c.r = 255;
		c.g = 0;
		c.b = 255;
		c.a = 255;
	}
}

Sensor::~Sensor()
{
	delete[] Pixels;
}

void Sensor::Clear() const
{
	Color32 c;
	c.r = 128;
	c.g = 0;
	c.b = 128;
	c.a = 128;
	memset(Pixels, c.Int, Width * Height * sizeof(Color32));
}

Color32& Sensor::GetPixel(int x, int y)
{
	ASSERT(Pixels);
	ASSERT(x >= 0);
	ASSERT(y >= 0);
	ASSERT(x < Width);
	ASSERT(y < Height);
	return Pixels[x + y * Width];
}

const Color32& Sensor::GetPixel(int x, int y) const
{
	return GetPixel(x, y);
}

const Color32& Sensor::SetPixel(int x, int y, const glm::vec3& color)
{
	Color32& pixel = GetPixel(x, y);
	const float exp = 1.0f / Gamma;
	auto c = pow(color, glm::vec3(exp, exp, exp));
	pixel = Color32(c);
	return  pixel;
}
