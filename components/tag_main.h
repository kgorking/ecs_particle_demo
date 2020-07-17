#include <ecs/component_specifier.h>

// A tag component for the main entity
struct tag_main {
    ecs_flags(ecs::tag);
};
