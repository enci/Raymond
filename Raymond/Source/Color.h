#pragma once

#include <glm.hpp>

namespace Raymond
{

struct Color32
{
	union
	{
		struct
		{
			//unsigned char r, g, b, a;
			unsigned char b, g, r, a;
		};
		int Int;
 	};

	Color32() { r = g = b = a = 0; }
	Color32(const glm::vec3& color);
	Color32(const glm::vec4& color);
};

inline Color32::Color32(const glm::vec3& color)
{
	r = unsigned char(glm::clamp(color.r * 255.99f, 0.0f, 255.0f));
	g = unsigned char(glm::clamp(color.g * 255.99f, 0.0f, 255.0f));
	b = unsigned char(glm::clamp(color.b * 255.99f, 0.0f, 255.0f));
	a = unsigned char(255);
}

inline Color32::Color32(const glm::vec4& color)
{
	r = unsigned char(glm::clamp(color.r * 255.99f, 0.0f, 255.0f));
	g = unsigned char(glm::clamp(color.g * 255.99f, 0.0f, 255.0f));
	b = unsigned char(glm::clamp(color.b * 255.99f, 0.0f, 255.0f));
	a = unsigned char(glm::clamp(color.a * 255.99f, 0.0f, 255.0f));
}

}
