#pragma once

#include <memory>
#include <glm.hpp>
#include "AABB.h"

namespace Raymond
{

class Ray;
class Traceable;
struct Material;

struct IntersectInfo
{
	float Distance	= -1.0f;
	glm::vec3 Position{};
	glm::vec3 Normal{};
	std::weak_ptr<Traceable> Object;
};

/// 
class Traceable
{
public:
	/// Create a new Traceable with an identity transform
	Traceable() : _transform(1), _inverse(1) { }

	/// Nothing to explicitly destroy
	virtual ~Traceable() = default;

	/// Trace intersection with a ray and fills the intersection
	/// @return true if intersection was found
	virtual bool Trace(const Ray& r, IntersectInfo& info) const = 0;

	/// Get the Axis Aligned Bounding Box for this object
	virtual AABB GetAABB() const { return {}; }
	
	/// Set the world transform matrix (no scaling supported at current)
	void SetTransform(const glm::mat4& transform);

	/// Get the world transform matrix
	const glm::mat4& GetTransform() const					{ return  _transform; }

	/// Set material
	void SetMaterial(std::shared_ptr<Material> material)	{ _material = material; }

	/// Get the material. Returned as a weak pointer and can be null as well
	const Material& GetMaterial() const;

protected:
	/// World transform of the object
	glm::mat4 _transform;

	/// Cached inverse for local space ray intersection
	glm::mat4 _inverse;

	/// Pointer to a material, can also be null (default lambert)
	/// Materials can be shared across objects
	std::shared_ptr<Material> _material = nullptr;
};

inline void Traceable::SetTransform(const glm::mat4& transform)
{
	_transform = transform;
	_inverse = inverse(_transform);
}

}
