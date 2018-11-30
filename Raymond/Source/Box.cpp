#include "Box.h"
#include "Ray.h"

using namespace Raymond;
using namespace glm;

Ray Transform(const Ray& ray, const mat4& transform)
{
	const vec4 origin(ray.Origin, 1.0f);
	const vec4 direction(ray.Direction, 0.0f);
	return Ray(vec3(transform * origin), vec3(transform * direction));
}

void Transform(IntersectInfo& info, const mat4& transform)
{
	
}

Box::Box(const vec3& position, const vec3& extent)
	: _position(position)
	, _extent(extent)
{}

bool Box::Trace(const Ray& ray, IntersectInfo& info) const
{
	Ray r = Transform(ray, _inverse);

	const vec3 boxmax = _position + _extent;
	const vec3 boxmin = _position - _extent;

	// Interval based test
	const vec3 oneoverdir(1.0f / r.Direction.x, 1.0f / r.Direction.y, 1.0f / r.Direction.z);
	
	const vec3 tmin = (boxmin - r.Origin) * oneoverdir;
	const vec3 tmax = (boxmax - r.Origin) * oneoverdir;
	const vec3 realmin = min(tmax, tmin);
	const vec3 realmax = max(tmax, tmin);

	float minmax = min(min(realmax.x, realmax.y), realmax.z);
	float maxmin = max(max(realmin.x, realmin.y), realmin.z);
	
	if (minmax >= maxmin && maxmin > 0.0f)
	{ 
		// Have intersection
		// Get position
		float t = maxmin;
		vec3 position = r.PointAtParameter(t);

		// Get normal
		// 1. Get vector to relative position
		vec3 normal = position - _position;

		// 2. Scale to matching unit box
		normal.x /= boxmax.x - boxmin.x;
		normal.y /= boxmax.y - boxmin.y;
		normal.z /= boxmax.z - boxmin.z;

		// 3. Keep the largest axis
		if (abs(normal.x) > abs(normal.y))
		{
			normal.y = 0.0f;
			if (abs(normal.x) > abs(normal.z))
				normal.z = 0;
			else
				normal.x = 0;
		}
		else {
			normal.x = 0;
			if (abs(normal.y) > abs(normal.z))
				normal.z = 0;
			else
				normal.y = 0;
		}

		// 4. Normalize to unit and fill-in rest		
		info.Normal =  vec3(_transform * vec4(normalize(normal), 0.0f));
		auto l =  length(info.Normal);
		info.Position = vec3(_transform * vec4(position, 1.0f));
		info.Distance = t / l;
		info.Normal /= l;
		return true;
	}

	return false;

}
