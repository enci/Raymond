#pragma once

namespace Raymond
{

struct Color32
{
	union
	{
		struct
		{
			unsigned char r, g, b, a;
		};
		int Int;
 	};

	Color32() { r = g = b = a = 0; }
	Color32(const glm::vec3& color);
	Color32(const glm::vec4& color);
};

inline Color32::Color32(const glm::vec3& color)
{
	r = unsigned char(color.r * 255.99f);
	g = unsigned char(color.g * 255.99f);
	b = unsigned char(color.b * 255.99f);
	a = unsigned char(255);
}

inline Color32::Color32(const glm::vec4& color)
{
	r = unsigned char(color.r * 255.99f);
	g = unsigned char(color.g * 255.99f);
	b = unsigned char(color.b * 255.99f);
	a = unsigned char(color.a * 255.99f);
}

}
