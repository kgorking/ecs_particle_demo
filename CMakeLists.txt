# CMakeList.txt : CMake project for ecs_particle_demo, include source and define
# project specific logic here.
#
cmake_minimum_required (VERSION 3.8)

project ("ecs_particle_demo")

# Add source to this project's executable.
add_executable (ecs_particle_demo "ecs_particle_demo.cpp" "ecs_particle_demo.h")

# TODO: Add tests and install targets if needed.
