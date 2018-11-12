#pragma once
#include <vector>

namespace Raymond
{
class Ray;
class Traceable;
struct IntersectInfo;

class Renderer
{	
public:
	//virtual void Render();
	virtual bool Trace(const Ray& ray, IntersectInfo& info);
	std::vector<Traceable*> Scene;
};

}
