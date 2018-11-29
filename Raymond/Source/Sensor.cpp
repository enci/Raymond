#include "Sensor.h"
#include "Defines.h"
#include "Defines.h"

using namespace Raymond;

Sensor::Sensor(int width, int height)
	: Width(width)
	, Height(height)
	, Samples(new glm::vec3[width * height])
	, Counts(new ushort[width * height])
{
	Clear();
}

Sensor::~Sensor()
{
	delete[] Samples;
	delete[] Counts;
}

void Sensor::Clear() const
{
	// Set to 0.0f according to IEEE 754
	memset(Samples, 0, Width * Height * sizeof(Samples[0]));
	memset(Counts, 0, Width * Height * sizeof(Counts[0]));
}

Color32 Sensor::GetPixel(int x, int y)
{
	ASSERT(Samples);
	ASSERT(Counts);
	ASSERT(x >= 0);
	ASSERT(y >= 0);
	ASSERT(x < Width);
	ASSERT(y < Height);

	// Get gamma correct sample
	const int i = x + y * Width;
	auto sample = Samples[i];
	sample *= 1.0f / float(Counts[i]);
	const float exp = 1.0f / Gamma;
	sample = pow(sample, glm::vec3(exp, exp, exp));

	const auto pixel = Color32(sample);
	return pixel;
}

const Color32& Sensor::GetPixel(int x, int y) const
{
	return GetPixel(x, y);
}

void Sensor::AddSample(int x, int y, const glm::vec3& color)
{
	ASSERT(Samples);
	ASSERT(Counts);
	ASSERT(x >= 0);
	ASSERT(y >= 0);
	ASSERT(x < Width);
	ASSERT(y < Height);

	const int i = x + y * Width;
	Samples[i] += color;
	Counts[i]++;
}


/*
const Color32& Sensor::SetPixel(int x, int y, const glm::vec3& color)
{
	Color32& pixel = GetPixel(x, y);
	const float exp = 1.0f / Gamma;
	auto c = pow(color, glm::vec3(exp, exp, exp));
	pixel = Color32(c);
	return  pixel;
}
*/
