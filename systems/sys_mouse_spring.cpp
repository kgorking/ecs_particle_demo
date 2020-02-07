#include <ecs/ecs.h>
#include "../components/input.h"
#include "../components/particle.h"
#include "../components/velocity.h"
#include "../components/frame_context.h"
#include <GLFW/glfw3.h>
#include <iostream>

static ecs::system& sys_mouse_spring = ecs::add_system_parallel([](velocity& vel, particle const& par, frame_context const& fc) {
	float r_x = fc.cursor_x - par.x;
	float r_y = fc.cursor_y - par.y;

	float constexpr spring_constant = 0.7f;
	float const force_x = spring_constant * r_x;
	float const force_y = spring_constant * r_y;

	vel.x += force_x * fc.dt;
	vel.y += force_y * fc.dt;
});

// A system that handles input events.
// Take the 's' key for this system
static ecs::system& sys_toggle_mouse_spring = ecs::add_system([](input const& input) mutable {
	if (input.key != GLFW_KEY_S)
		return;

	sys_mouse_spring.set_enable(!sys_mouse_spring.is_enabled());
	std::cout << " mouse spring: " << (sys_mouse_spring.is_enabled() ? "on\n" : "off\n");
});
