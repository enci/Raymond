#include "Plane.h"
#include "Ray.h"

using namespace Raymond;
using namespace glm;

Plane::Plane(const vec3& n, float o)
	: _normal(n)
	, _offset(o)
{
	_normal = normalize(_normal);
}

Plane::Plane() : Plane(vec3(0.0f, 0.0f, 1.0f), 0.0f)
{
}

bool Plane::Trace(const Ray& r, IntersectInfo& info) const
{
	float d = dot(_normal, r.Direction);

	if (d == 0.0f)
		return false;
	
	// Normal and ray perpendicular, early out 
	float t = (-_offset - dot(_normal, r.Origin)) / d;

	// Behind ray, early out		
	if (t <= 0.0f)
		return false;

	info.Position = r.PointAtParameter(t);
	info.Normal = _normal;
	info.Distance = t;
	
	return true;
}

AABB Plane::GetAABB() const
{
	AABB aabb(
		vec3(-FLT_MAX, -FLT_MAX, -FLT_MAX),
		vec3(FLT_MAX, FLT_MAX, FLT_MAX));
	return aabb;
}
