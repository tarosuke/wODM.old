#include <math.h>

#include "lightball.h"



namespace particles{

	LIGHTBALL::LIGHTBALL(unsigned size) : TB::Image(size, size, true){
		TB::Image::Raw raw(*this);

		//内容生成
		for(unsigned y(0); y < size; ++y){
			unsigned* line(raw[y]);
			const float y2((y - 0.5 * size) / size);
			for(unsigned x(0); x < size; ++x){
				const float x2((x - 0.5 * size) / size);
				const float l(1.0 - (x2*x2 + y2*y2));
				const unsigned c(255 * (l < 0.0 ? 0.0 : l));
				line[x] = 0xff000000 | c | (c << 8) | (c << 16);
			}
		}
	}

}
