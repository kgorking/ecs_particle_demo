#include <ecs/ecs.h>
#include "../components/input.h"
#include "../components/particle.h"
#include "../components/frame_context.h"
#include <GLFW/glfw3.h>
#include <iostream>

ecs::system& sys_particle_painter = ecs::add_system_parallel([](particle& par, frame_context const& fc) {
	float const r_x = fc.cursor_x - par.x;
	float const r_y = fc.cursor_y - par.y;
	float const len_sqr = r_x * r_x + r_y * r_y;

	if (len_sqr > 0.005f)
		return; // out of range

	par.r = 1;
	par.g = 0;
	par.b = 1;
});

// Take the 'p' key for this system
ecs::system& sys_toggle_particle_painter = ecs::add_system([](input const& input) mutable {
	if (input.key != GLFW_KEY_P)
		return;

	sys_particle_painter.set_enable(!sys_particle_painter.is_enabled());
	std::cout << " particle painter: " << (sys_particle_painter.is_enabled() ? "on\n" : "off\n");
});
