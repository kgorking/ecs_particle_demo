#pragma once
#include <ecs/component_specifier.h>

// Contains per-frame data
struct frame_context {
	ecs_flags(ecs::shared);

	float time = 0;
	float dt = 0;

	float cursor_x = 0.0;
	float cursor_y = 0.0;
};
