#pragma once

namespace Raymond
{
class Ray;

class Traceable
{
public:
	virtual ~Traceable() = default;
	virtual bool Trace(const Ray& r) = 0;
	virtual bool Test(Ray r) = 0;
};

}
