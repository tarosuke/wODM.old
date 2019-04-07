#include <math.h>

#include "lightball.h"



namespace particles{

	LIGHTBALL::LIGHTBALL(unsigned size) : TB::Image(size, size, true){
		TB::Image::Raw raw(*this);
		const float hs(0.5 * size);

		//内容生成
		for(unsigned y(0); y < size; ++y){
			unsigned* line(raw[y]);
			const float y2((y - hs) / hs);
			for(unsigned x(0); x < size; ++x){
				const float x2((x - hs) / hs);
				const float l(sqrtf(x2*x2 + y2*y2));
				const float ll((l <= 1.0) ? l : 1.0);
				const unsigned c(255 * 0.5 * (cosf(ll * M_PI) + 1.0));
				line[x] = 0xff000000 | c | (c << 8) | (c << 16);
			}
		}
	}

}
