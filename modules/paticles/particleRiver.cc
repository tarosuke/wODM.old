#include <stdlib.h>

#include <toolbox/gl/gl.h>
#include <toolbox/factory/factory.h>
#include <toolbox/geometry/vector.h>
#include <toolbox/prefs.h>

#include "lightball.h"
#include <particles.h>



namespace particles{

	class ParticleRiver : public vr_core::Particles{
		ParticleRiver();
	public:
		ParticleRiver(
			unsigned size,
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
			if(e.position.z < -range){
				e.position.z = range;
			}
		};

		static const float range;
	};

	const float ParticleRiver::range(10);

	TB::Prefs<bool> ParticleRiver::enable(
		"--demo/particleRiver", false, TB::CommonPrefs::nosave);


	FACTORY<vr_core::Module> ParticleRiver::factory(New);
	vr_core::Module* ParticleRiver::New(){
		if(enable){
			new ParticleRiver(128, LIGHTBALL(), 1);
		}
		return 0;
	}


	ParticleRiver::ParticleRiver(
		unsigned size,
		const TB::Image& sprite,
		unsigned numOfParticles) :
		Particles(size, sprite, numOfParticles){

		for(unsigned n(0); n < numOfParticles; ++n){
			Element& e(elements[n]);
			e.position.x = R(-range, range);
			e.position.y = -1.5;
			e.position.z = R(-range, range);
			e.velocity.x = e.velocity.y = 0;
			e.velocity.z = -0.1;
		}
	}

}
