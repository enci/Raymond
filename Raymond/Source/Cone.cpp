#include "Cone.h"
#include "Ray.h"

using namespace glm;
using namespace Raymond;

bool Cone::Trace(const Ray& r, IntersectInfo& info) const
{
	const vec3& O = r.Origin;
	const vec3& D = r.Direction;
	const vec3& C = Center;
	vec3 V(0.0f, 0.0f, -1.0f);
	vec3 CO = O - C;
	float theta = Angle / 2.0f;

	float a = pow(dot(D, V),2) - pow(cos(theta),2);
	float b = 2.0f * (dot(D, V)*dot(CO,V) - dot(D,CO) * pow(cos(theta), 2));
	float c = pow(dot(CO, V),2) - dot(CO,CO) * pow(cos(theta), 2);
	float discriminant = b * b - 4 * a*c;

	if (discriminant > 0.0f)
	{
		float t0 = (-b - sqrt(discriminant)) / (2.0f * a);
		float t1 = (-b + sqrt(discriminant)) / (2.0f * a);
		if (t0 > 0.0f || t1 > 0.0f)
		{
			float t = min(t0, t1);
			info.Distance = t;
			info.Position = r.PointAtParameter(t);
			vec3 down = info.Position - Center;
			vec3 side = cross(down, V);
			info.Normal = cross(down, side);
			info.Normal = normalize(info.Normal);
			return true;
		}
	}

	return false;
}
