#include <iostream>
#include <execution>

#include <ecs/ecs.h>
#include <GLFW/glfw3.h>

#include "../components/input.h"
#include "../components/particle.h"
#include "../components/color.h"

void setup_reset_colors() {
    // A system that handles input events.
    // Take the 'r' key for this system
    ecs::make_system([](input const& input) {
        if (!input.is_pressed(GLFW_KEY_R))
            return;

        std::cout << " reseting colors\n";
        auto const particles = ecs::get_components<particle>({ 0, max_num_particles });
        auto const colors = ecs::get_components<color>({ 0, max_num_particles });
        for (int i = 0; i < max_num_particles; i++) {
            colors[i].r = particles[i].x / 2 + 0.5f;
            colors[i].g = particles[i].y / 2 + 0.5f;
            colors[i].b = 1 - colors[i].r - colors[i].g;
        }
    });
}
