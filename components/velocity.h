#pragma once
#include <cmath>

struct velocity {
	float x;
	float y;
};

// Helper-lambda to init the velocity
auto const velocity_init = [](ecs::entity_id) {
    velocity v{ rand() / 16384.0f - 1.0f, rand() / 16384.0f - 1.0f };
    float const len = sqrt(v.x * v.x + v.y * v.y) * 5;
    v.x /= len;
    v.y /= len;
    return v;
};
