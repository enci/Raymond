#include "Sphere.h"
#include "Ray.h"

using namespace Raymond;
using namespace glm;

bool Sphere::Trace(const Ray& r)
{
	vec3 oc = r.Origin - Center;
	float a = dot(r.Direction, r.Direction);
	float b = 2.0f * dot(oc, r.Direction);
	float c = dot(oc, oc) - (Radius * Radius);
	float discriminant = b * b - 4 * a*c;

	// rec.mat_ptr = mat_ptr;
	/*
	if (discriminant > 0.0f)
	{
		float t = (-b - sqrt(discriminant)) / (2.0f * a);
		if (t < t_max && t > t_min)
		{
			rec.t = t;
			rec.p = r.PointAtParameter(rec.t);
			rec.normal = (rec.p - center) / radius;
			return true;
		}

		t = (-b + sqrt(discriminant)) / (2.0f * a);
		if (t < t_max && t > t_min)
		{
			rec.t = t;
			rec.p = r.PointAtParameter(rec.t);
			rec.normal = (rec.p - center) / radius;
			return true;
		}
	}
	*/

	return false;
}
