#include <stdlib.h>

#include <toolbox/gl/gl.h>
#include <toolbox/factory/factory.h>
#include <toolbox/geometry/vector.h>
#include <toolbox/prefs.h>

#include "lightball.h"
#include <particles.h>



namespace particles{

	class ParticleDemo : public vr_core::Particles{
		ParticleDemo();
	public:
		ParticleDemo(
			float size,
			const TB::Image& sprite,
			unsigned numOfParticles);
	private:
		static FACTORY<vr_core::Module> factory;
		static TB::Prefs<bool> enable;
		static vr_core::Module* New();

		static float R(float min, float max){
			return (((max - min) * rand()) / RAND_MAX) + min;
		};

		void UpdateElement(Element& e, float delta) final{
			Particles::UpdateElement(e, delta);

			const unsigned id(&e - elements);

			//加減速
			for(unsigned n(id + 1); n < numOfParticles; ++n){
				Element& ee(elements[n]);
				const float dx(ee.position.x - e.position.x);
				const float dy(ee.position.y - e.position.y);
				const float dz(ee.position.z - e.position.z);
				const float d2(dx*dx + dy*dy + dz*dz +0.625);
				const float a(0.01 /  (((n ^ id) & 1) ? d2 : -d2));
				const float ax(dx* a);
				const float ay(dy* a);
				const float az(dz* a);

				e.velocity.x += (ax - (0.000001 * e.position.x)) * delta;
				e.velocity.y += (ay - (0.000001 * e.position.y)) * delta;
				e.velocity.z += (az - (0.000001 * e.position.z)) * delta;
				ee.velocity.x -=ax;
				ee.velocity.y -= ay;
				ee.velocity.z -= az;
			}

			//速度に従って移動
			Particles::UpdateElement(e, delta);
		};
	};


	TB::Prefs<bool> ParticleDemo::enable(
		"--demo/particleDemo", false, TB::CommonPrefs::nosave);


	FACTORY<vr_core::Module> ParticleDemo::factory(New);
		vr_core::Module* ParticleDemo::New(){
		if(enable){
			LIGHTBALL lightBall;
			new ParticleDemo(128.0f, lightBall, 1000);
		}
		return 0;
	}


	ParticleDemo::ParticleDemo(
		float size,
		const TB::Image& sprite,
		unsigned numOfParticles) :
		Particles(size, sprite, numOfParticles){
		syslog(LOG_INFO, "module:particles");

		for(unsigned n(0); n < numOfParticles; ++n){
			Element& e(elements[n]);
			e.velocity.x = R(-1.0, 1.0);
			e.velocity.y = R(-1.0, 1.0);
			e.velocity.z = R(-1.0, 1.0);;
			const float v(sqrtf(
				e.velocity.x*e.velocity.x +
				e.velocity.y*e.velocity.y +
				e.velocity.z*e.velocity.z) / 256);
			e.velocity.x /= v;
			e.velocity.y /= v;
			e.velocity.z /= v;
		}
	}

}
