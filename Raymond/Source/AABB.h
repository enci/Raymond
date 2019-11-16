#pragma once

#include <glm.hpp> 

namespace Raymond
{
class Ray;

struct AABB
{
	AABB();
	AABB(const glm::vec3& min, const glm::vec3& max);
	bool IsValid() const { return _min.x < _max.x; }
	bool Trace(const Raymond::Ray& r) const;
	void Clear();
	void AddPoint(const glm::vec3& point);
	void Add(AABB& aabb);
	glm::vec2 GetCenter() const { return (_min + _max) * 0.5f; }
private:
	glm::vec3 _min;
	glm::vec3 _max;
};

}
