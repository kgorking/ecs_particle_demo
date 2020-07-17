#pragma once
#include <ecs/component_specifier.h>

extern "C" double glfwGetTime(void);

// Contains per-frame data
struct frame_context {
	ecs_flags(ecs::global);

	float time = static_cast<float>(glfwGetTime());
	float dt = 0;

	float cursor_x = 0.0;
	float cursor_y = 0.0;

	// window data
	int width = 0;
	int height = 0;
	float ratio = 0;
};
