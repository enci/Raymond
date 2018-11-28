#include "Sensor.h"
#include "Defines.h"

using namespace Raymond;

Sensor::Sensor(int width, int height)
	: Width(width)
	, Height(height)
	, Pixels(new Color32[width * height])
{
	Clear();
}

Sensor::~Sensor()
{
	delete[] Pixels;
}

void Sensor::Clear() const
{
	memset(Pixels, 0, Width * Height * sizeof(Color32));
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
