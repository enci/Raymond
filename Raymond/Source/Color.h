#pragma once

namespace Raymond
{

struct Color32
{
	union
	{
		struct
		{
			char r, g, b, a;
		};
		int Int;
 	};
};

}