#pragma once
#include <ecs/component_specifier.h>

struct glfw_window {
    ecs_flags(ecs::global);

    struct GLFWwindow* handle;
};
