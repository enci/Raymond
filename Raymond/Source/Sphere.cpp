#include "Sphere.h"
#include "Ray.h"

using namespace Raymond;
using namespace glm;

bool Sphere::Trace(const Ray& r, IntersectInfo& info) const
{
	vec3 oc = r.Origin - Center;
	float a = dot(r.Direction, r.Direction);
	float b = 2.0f * dot(oc, r.Direction);
	float c = dot(oc, oc) - (Radius * Radius);
	float discriminant = b * b - 4 * a*c;

	if (discriminant > 0.0f)
	{
		float t = (-b - sqrt(discriminant)) / (2.0f * a);
		if (t > 0.0f)
		{
			info.Distance = t;
			info.Position = r.PointAtParameter(t);
			info.Normal = (info.Position - Center) / Radius;
			return true;
		}

		t = (-b + sqrt(discriminant)) / (2.0f * a);
		if (t > 0.0f)
		{
			info.Distance = t;
			info.Position = r.PointAtParameter(t);
			info.Normal = (info.Position - Center) / Radius;
			return true;
		}
	}

	return false;
}

AABB Sphere::GetAABB() const
{
	vec4 center = vec4(Center, 1.0f);
	center = _transform * center;
	vec3 min = center;
	vec3 max = center;
	const vec3 ex(Radius, Radius, Radius);
	min -= ex;
	max += ex;

	return AABB(min, max);
}
