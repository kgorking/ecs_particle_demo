#pragma once
#include <ecs/component_specifier.h>

struct input {
	ecs_flags(ecs::transient);

	struct GLFWwindow* window = nullptr;
	int key = -1;
	int scancode = 0;
	int action = 0;
	int mods = 0;
};
