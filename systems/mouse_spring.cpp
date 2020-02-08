#include <GLFW/glfw3.h>
#include <iostream>
#include <ecs/ecs.h>

#include "../components/input.h"
#include "../components/particle.h"
#include "../components/velocity.h"
#include "../components/frame_context.h"

static ecs::system& mouse_spring = ecs::add_system_parallel([](velocity& vel, particle const& par, frame_context const& fc) {
	float const r_x = fc.cursor_x - par.x;
	float const r_y = fc.cursor_y - par.y;

	float constexpr spring_constant = 0.7f;
	float const force_x = spring_constant * r_x;
	float const force_y = spring_constant * r_y;

	vel.x += force_x * fc.dt;
	vel.y += force_y * fc.dt;
});

// A system that handles input events.
// Take the 's' key for this system
static ecs::system const& toggle_mouse_spring = ecs::add_system([](input const& input) {
	if (input.key != GLFW_KEY_S)
		return;

	mouse_spring.set_enable(!mouse_spring.is_enabled());
	std::cout << " mouse spring: " << (mouse_spring.is_enabled() ? "on\n" : "off\n");
});
