#include <iostream>
#include <execution>
#include <gsl/span>

#include <ecs/ecs.h>
#include <GLFW/glfw3.h>

#include "../components/input.h"
#include "../components/particle.h"

// A system that handles input events.
// Take the 'c' key for this system
static ecs::system const& sys_reset_colors = ecs::add_system([](input const& input) {
    if (input.key != GLFW_KEY_R)
        return;

    std::cout << " reseting colors\n";
    auto const particles = ecs::get_components<particle>({ 0, max_num_particles });
    std::for_each(std::execution::par, particles.begin(), particles.end(), [](particle& p) {
        p.r = p.x / 2 + 0.5f;
        p.g = p.y / 2 + 0.5f;
        p.b = 1 - p.r - p.g;
    });
});
