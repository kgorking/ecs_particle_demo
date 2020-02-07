# CMakeList.txt : CMake project for ecs_particle_demo, include source and define
# project specific logic here.
#
cmake_minimum_required (VERSION 3.8)

project (ecs_particle_demo)

# Require c++17
set(CMAKE_CXX_STANDARD 17)
set(CXX_STANDARD_REQUIRED on)

find_package(OpenGL REQUIRED)

# glfw
set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
add_subdirectory("ext/glfw")
include_directories("ext/glfw/deps")

# ecs
add_subdirectory("ext/ecs")
include_directories("ext/ecs/include")
include_directories("ext/ecs/GSL/include")

# Add source to this project's executable.
add_executable (ecs_particle_demo "ecs_particle_demo.cpp" "ecs_particle_demo.h" "ext/glfw/deps/glad_gl.c"   "systems/sys_velocity.cpp" "systems/sys_collision.cpp" "systems/sys_reset_colors.cpp" "systems/sys_mouse_spring.cpp" "systems/sys_particle_painter.cpp" "systems/sys_particle_render.cpp")

# TODO: Add tests and install targets if needed.
target_link_libraries(ecs_particle_demo ecs glfw OpenGL::GL)
