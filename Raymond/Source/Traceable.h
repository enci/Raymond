#pragma once

#include <memory>
#include "glm.hpp"

namespace Raymond
{
class Ray;
class Traceable;
struct Material;

struct IntersectInfo
{
	float Distance	= -1.0f;
	glm::vec3 Position;
	glm::vec3 Normal;
	std::weak_ptr<Traceable> Object;
};

class Traceable
{
public:
	Traceable() : _transform(1) { _inverse = _transform; }
	virtual ~Traceable() = default;
	virtual bool Trace(const Ray& r, IntersectInfo& info) const = 0;
	virtual bool Test(const Ray& r, float tmax) const = 0;
	void SetTransform(const glm::mat4& transform)
	{ _transform = transform, _inverse = inverse(_transform); }
	const glm::mat4& GetTransform() const { return  _transform; }
	void SetMaterial(const Material* material) { _material = material; }
	const Material* GetMaterial() const { return _material; }

protected:
	glm::mat4 _transform;
	glm::mat4 _inverse;
	//std::shared_ptr<Material> _material = nullptr;
	const Material* _material = nullptr;
};

}