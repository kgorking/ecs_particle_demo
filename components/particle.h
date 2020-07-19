#pragma once
#include <stdlib.h>

static constexpr int max_num_particles = 500'000;

struct particle {
    float x, y;
};

// Helper lambda to initialize a single particle
auto constexpr particle_init = [](ecs::entity_id) -> particle {
	float const x = rand() / 16384.0f - 1.0f;
	float const y = rand() / 16384.0f - 1.0f;

	return { x, y };
};
