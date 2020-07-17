#include <ecs/ecs.h>
#include "../components/particle.h"
#include "../components/velocity.h"

void setup_collision() {
	// Make sure the particles stay within the bounds.
	// This system is always active
	ecs::make_parallel_system([](particle& par, velocity& vel) {
		if (par.x > 1) {
			par.x = 1;
			float const p = 2 * vel.x * -1;
			vel.x = vel.x - p * -1;
		}
		else if (par.x < -1) {
			par.x = -1;
			float const p = 2 * vel.x * 1;
			vel.x = vel.x - p * 1;
		}

		if (par.y > 1) {
			par.y = 1;
			float const p = 2 * vel.y * -1;
			vel.y = vel.y - p * -1;
		}
		else if (par.y < -1) {
			par.y = -1;
			float const p = 2 * vel.y * 1;
			vel.y = vel.y - p * 1;
		}
	});
}
