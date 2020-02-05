# ecs_particle_demo

This is a demo project to test out my ecs library.

It is just a bunch of particles that you can manipulate by adding new components and systems. The nice thing about using ecs is that you can add functionality to the particles without having to fiddle with the particle implementation itself. An example of this is the collision system, which enforces a bounding box around the particles without interfering with any of the other systems.
