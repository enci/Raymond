#pragma once

#include <glm.hpp>
#include <vector>
#include "AABB.h"
#include <memory>
#include <functional>

namespace Raymond
{
	
struct IntersectInfo;
class Traceable;

class BVH
{
public:
	BVH(const std::vector<std::shared_ptr<Traceable>>& objects);
	bool Trace(const Ray& r, IntersectInfo& info) const;
private:
	void Recurse(size_t from, size_t to, size_t parent, unsigned depth);
	struct BVHNode
	{
		std::weak_ptr<Traceable> Object;
		AABB BoundingBox;
	};
	bool Trace(
		const Ray& r,
		IntersectInfo& info, 
		int parent) const;
	std::vector<BVHNode> Volumes;
	std::vector<std::weak_ptr<Traceable>> _bodies;
};




}
