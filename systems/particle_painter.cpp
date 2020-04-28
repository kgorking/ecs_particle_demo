#include <ecs/ecs.h>
#include "../components/input.h"
#include "../components/particle.h"
#include "../components/frame_context.h"
#include <GLFW/glfw3.h>
#include <iostream>

// Paint particles purple if they are in range of the cursor
static ecs::system_base & particle_painter = ecs::make_parallel_system([](particle& par, frame_context const& fc) {
	float const r_x = fc.cursor_x - par.x;
	float const r_y = fc.cursor_y - par.y;
	float const len_sqr = r_x * r_x + r_y * r_y;

	if (len_sqr > 0.0005f)
		return; // out of range

	par.r = 1;
	par.g = 0;
	par.b = 1;
});

// A system that handles input events.
// Take the 'p' key for this system
static ecs::system_base const& toggle_particle_painter = ecs::make_system([](input const& input) mutable {
	if (input.key != GLFW_KEY_P)
		return;

	particle_painter.set_enable(!particle_painter.is_enabled());
	std::cout << " particle painter: " << (particle_painter.is_enabled() ? "on\n" : "off\n");
});
