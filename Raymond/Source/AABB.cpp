#include "AABB.h"
#include "Ray.h"
#include "Sphere.h"
#include "Box.h"

using namespace Raymond;
using namespace glm;


AABB::AABB()
{
	Clear();
}

AABB::AABB(const glm::vec3& min, const glm::vec3& max)
	: _min(min)
	, _max(max)
{}

bool AABB::Trace(const Ray& r) const
{
	// Interval based test
	const vec3 oneoverdir(1.0f / r.Direction.x, 1.0f / r.Direction.y, 1.0f / r.Direction.z);

	const vec3 tmin = (_min - r.Origin) * oneoverdir;
	const vec3 tmax = (_max - r.Origin) * oneoverdir;
	const vec3 realmin = min(tmax, tmin);
	const vec3 realmax = max(tmax, tmin);

	float minmax = min(min(realmax.x, realmax.y), realmax.z);
	float maxmin = max(max(realmin.x, realmin.y), realmin.z);

	return (minmax >= maxmin && maxmin > 0.0f);
}

void AABB::Clear()
{
	constexpr float mf = std::numeric_limits<float>::max();
	_min.x = mf;
	_min.y = mf;
	_min.z = mf;

	_max.x = -mf;
	_max.y = -mf;
	_max.z = -mf;
}

void AABB::AddPoint(const glm::vec3& point)
{
	// X
	if (point.x > _max.x)
		_max.x = point.x;
	if (point.x < _min.x)
		_min.x = point.x;

	// Y
	if (point.y > _max.y)
		_max.y = point.y;
	if (point.y < _min.y)
		_min.y = point.y;

	// Z
	if (point.z > _max.z)
		_max.z = point.z;
	if (point.z < _min.z)
		_min.z = point.z;
}

void AABB::Add(AABB& aabb)
{
	// X
	if (aabb._max.x > _max.x)
		_max.x = aabb._max.x;
	if (aabb._min.x < _min.x)
		_min.x = aabb._min.x;

	// Y
	if (aabb._max.y > _max.y)
		_max.y = aabb._max.y;
	if (aabb._min.y < _min.y)
		_min.y = aabb._min.y;

	// Z
	if (aabb._max.z > _max.z)
		_max.z = aabb._max.z;
	if (aabb._min.z < _min.z)
		_min.z = aabb._min.z;
}
