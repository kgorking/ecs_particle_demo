#pragma once
#include <ecs/component_specifier.h>

// Contains per-frame data
struct frame_context {
	ecs_flags(ecs::shared);

	double time = 0;
	double dt = 0;
};
