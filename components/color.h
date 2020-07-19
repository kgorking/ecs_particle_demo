#pragma once
#include "particle.h"

struct color {
	float r, g, b;
};

// Helper lambda to initialize a color
auto constexpr color_init = [](ecs::entity_id) -> color {
	auto const p = particle_init(0);
	float const r = p.x / 2 + 0.5f;
	float const g = p.y / 2 + 0.5f;

	return { r, g, 1 - r - g };
};
