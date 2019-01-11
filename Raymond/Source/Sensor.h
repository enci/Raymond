#pragma once

#include "Color.h"
#include "Defines.h"

namespace Raymond
{

struct Sensor
{
	Sensor(int width, int height);
	~Sensor();
	void Clear() const;
	Color32 GetPixel(int x, int y);
	const Color32& GetPixel(int x, int y) const;
	void AddSample(int x, int y, const glm::vec3& color);
	int			Width		= -1;
	int			Height		= -1;
	glm::vec3*	Samples		= nullptr;
	ushort*		Counts		= nullptr;
	float		Gamma		= 2.2f;
};

}
