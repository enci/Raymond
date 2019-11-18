#include "BVH.h"
#include "Ray.h"
#include "Traceable.h"
#include <algorithm>
#include "Utils.h"

using namespace Raymond;
using namespace glm;
using namespace std;

BVH::BVH(const std::vector<std::shared_ptr<Traceable>>& objects)
{
	_bodies.clear();
	for (const auto& b : objects)
			_bodies.push_back(b);

	const uint capacity = NextPowerOfTwo(_bodies.size()) * 2u;
	Volumes.resize(capacity);
	Volumes.assign(capacity, BVHNode());
	Recurse(0, _bodies.size() - 1, 0, 0);
}

bool BVH::Trace(const Ray& r, IntersectInfo& info) const
{
	auto f = [](std::shared_ptr<Traceable> t) -> bool { return true; };
	return Trace(r, info, 0, f);
}

bool BVH::Trace(
	const Ray& r,
	IntersectInfo& info,
	std::function<bool(std::shared_ptr<Traceable>)> filter) const
{
	return Trace(r, info, 0, filter);
}

void BVH::Recurse(size_t from, size_t to, size_t parent, unsigned depth)
{
	auto& root = Volumes[parent];
	if (from >= to)
	{
		root.Object = _bodies[from];
		root.BoundingBox = root.Object.lock()->GetAABB();
	}
	else
	{
		static auto sort_x = [](weak_ptr<Traceable> body0, weak_ptr<Traceable> body1)
		{ return body0.lock()->GetAABB().GetCenter().x > body1.lock()->GetAABB().GetCenter().x; };

		static auto sort_y = [](weak_ptr<Traceable> body0, weak_ptr<Traceable> body1)
		{ return body0.lock()->GetAABB().GetCenter().y > body1.lock()->GetAABB().GetCenter().y; };

		if (depth % 2)
			sort(_bodies.begin() + from, _bodies.begin() + to, sort_x);
		else
			sort(_bodies.begin() + from, _bodies.begin() + to, sort_y);

		const auto left = 2 * parent + 1;
		const auto right = left + 1;
		const auto len = to - from;
		Recurse(from, from + len / 2, left, depth + 1);
		Recurse(from + len / 2 + 1, to, right, depth + 1);

		auto& lv = Volumes[left];
		auto& rv = Volumes[right];

		//root.Object = nullptr;
		root.BoundingBox = AABB();
		root.BoundingBox.Add(lv.BoundingBox);
		root.BoundingBox.Add(rv.BoundingBox);
	}
}

bool BVH::Trace(
	const Ray& r,
	IntersectInfo& info,
	int parent,
	std::function<bool(std::shared_ptr<Traceable>)> filter) const
{
	auto& volume = Volumes[parent];

	if (!volume.BoundingBox.IsValid())
		return false;

	if (!volume.Object.expired())
	{
		if (volume.BoundingBox.Trace(r))
		{
			IntersectInfo tInfo;
			if (volume.Object.lock()->Trace(r, tInfo))
			{
				if (info.Distance < 0.0f || (tInfo.Distance < info.Distance && tInfo.Distance > 0.00f))
				{
					info = tInfo;
					info.Object = volume.Object;
					return true;
				}
			}
		}
	}
	else if(volume.BoundingBox.Trace(r))
	{
		const auto left = 2 * parent + 1;
		const auto right = left + 1;
		Trace(r, info, left, filter);
		Trace(r, info, right, filter);
	}

	return false;
}
