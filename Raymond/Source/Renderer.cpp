#include "Renderer.h"
#include "Traceable.h"

using namespace Raymond;

bool Renderer::Trace(const Ray& ray, IntersectInfo& info)
{
	float nearest = FLT_MAX;
	for (auto t : Scene)
	{
		IntersectInfo tInfo;
		if(t->Trace(ray, tInfo))
		{
			if(tInfo.Distance < nearest && tInfo.Distance > 0.000001f)
			{
				info = tInfo;
				nearest = info.Distance;
			}
		}
	}

	return nearest != FLT_MAX;
}
