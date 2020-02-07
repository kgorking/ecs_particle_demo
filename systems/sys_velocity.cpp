#include <iostream>
#include <ecs/ecs.h>
#include <GLFW/glfw3.h>

#include "../components/input.h"
#include "../components/particle.h"
#include "../components/velocity.h"
#include "../components/frame_context.h"

// A system that takes an 'input' component.
// Take the 'v' key for this system
ecs::system& sys_vel_input = ecs::add_system([has_velocity = false] (input const& input) mutable {
    if (input.key != GLFW_KEY_V)
        return;

    // By removing the velocity components from the particles,
    // other systems that depend on the velocity component
    // will also stop running.
    ecs::entity_range particles{ 0, max_num_particles };
    if (has_velocity) {
        particles.remove<velocity>();
        std::cout << " removed velocity component\n";
    }
    else {
        // Add a bunch of randomized velocities
        particles.add([](ecs::entity_id) {
            velocity v{ rand() / 16384.0f - 1.0f, rand() / 16384.0f - 1.0f };
            float const len = sqrt(v.x * v.x + v.y * v.y) * 5;
            v.x /= len;
            v.y /= len;
            return v;
        });
        std::cout << " added velocity component\n";
    }

    has_velocity = !has_velocity;
});

// Update a particles position from its velocity
ecs::system& sys_velocity = ecs::add_system_parallel([](particle &par, velocity const& vel, frame_context const& fc) {
    par.x += vel.x * fc.dt;
    par.y += vel.y * fc.dt;
});
