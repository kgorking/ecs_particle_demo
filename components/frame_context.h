#pragma once
#include <ecs/component_specifier.h>

// Contains per-frame data
struct frame_context {
	ecs_flags(ecs::share);

	float time = 0;
	float dt = 0;

	float cursor_x = 0.0;
	float cursor_y = 0.0;

	// window data
	int width = 0;
	int height = 0;
	float ratio = 0;
};
