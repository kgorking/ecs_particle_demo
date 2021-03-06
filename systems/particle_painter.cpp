#include <iostream>
#include <ecs/ecs.h>
#include <GLFW/glfw3.h>

#include "../components/input.h"
#include "../components/particle.h"
#include "../components/color.h"
#include "../components/frame_context.h"

void setup_particle_painter() {
	// Paint particles purple if they are in range of the cursor
	ecs::system_base & particle_painter = ecs::make_parallel_system([](color &col, particle const& par, frame_context const& fc) {
		float const r_x = fc.cursor_x - par.x;
		float const r_y = fc.cursor_y - par.y;
		float const len_sqr = r_x * r_x + r_y * r_y;

		if (len_sqr > 0.0005f)
			return; // out of range

		col.r = 1;
		col.g = 0;
		col.b = 1;
	});

	// A system that handles input events.
	// Take the 'p' key for this system
	ecs::make_system([&particle_painter](input const& input) {
		if (!input.is_pressed(GLFW_KEY_P))
			return;

		particle_painter.set_enable(!particle_painter.is_enabled());
		std::cout << " particle painter: " << (particle_painter.is_enabled() ? "on\n" : "off\n");
	});
}
