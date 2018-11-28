#include "Traceable.h"
#include "Material.h"

using namespace Raymond;

const Material& Traceable::GetMaterial() const
{
	static Material defaultMaterial; // Simple lambertian
	return _material != nullptr ? *_material.get() : defaultMaterial;
}