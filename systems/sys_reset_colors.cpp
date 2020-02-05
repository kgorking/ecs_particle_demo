#include <ecs/ecs.h>
#include <iostream>
#include <GLFW/glfw3.h>

#include "../components/input.h"
#include "../components/particle.h"

// Take the 'c' key for this system
ecs::system& sys_reset_colors = ecs::add_system([](input const& input) mutable {
    if (input.key != GLFW_KEY_R)
        return;

    std::cout << " reseting colors\n";
    particle* p = ecs::get_component<particle>(0);
    particle* last = ecs::get_component<particle>(max_num_particles);
    while (p != last) {
        p->r = p->x / 2 + 0.5f;
        p->g = p->y / 2 + 0.5f;
        p->b = 1 - p->r - p->g;
        ++p;
    }
});
