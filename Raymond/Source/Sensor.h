#pragma once

#include "Color.h"

namespace Raymond
{

struct Sensor
{
	Sensor(int width, int height);
	~Sensor();
	void Clear() const;
	Color32& GetPixel(int x, int y);
	const Color32& GetPixel(int x, int y) const;
	const Color32& SetPixel(int x, int y, const glm::vec3& color);	
	int			Width		= -1;
	int			Height		= -1;
	Color32*	Pixels		= nullptr;
	float		Gamma		= 2.2f;
};

}
