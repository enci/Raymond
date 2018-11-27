#pragma once

#include "glm.hpp"

namespace Raymond
{

class SolidTexture
{
public:
	virtual ~SolidTexture() = default;
	virtual glm::vec3 GetColor(const glm::vec3& position) const = 0;
};

struct Material
{
	glm::vec3 Color				= glm::vec3(1.0f, 1.0f, 1.0f);
	float Emissive				= 0.0f;
	float Specular				= 0.0f;
	float SpecularPower			= 16.0f;
	float Transparency			= 0.0f;
	float Reflectance			= 0.0f;
	float RefractiveIndex		= 1.0f;
	SolidTexture* Texture		= nullptr;
};

}