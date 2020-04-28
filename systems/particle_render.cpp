#include <ecs/ecs.h>
#include <glad/gl.h>

#include "../components/particle.h"
#include "../components/velocity.h"
#include "../components/frame_context.h"
#include "../components/render_data.h"
#include "linmath.h"

// Create the particles
ecs::entity_range const particles{
	0, max_num_particles,	// The range of particles
	particle_init,			// initialize the particles using the lambda
	velocity_init			// initialize the velocities using the lambda
};

// The system that does the actual rendering
static ecs::system_base const& particle_render = ecs::make_system([](render_data const& data, frame_context const& context) {
	// Set up mvp matrix
	mat4x4 m, p, mvp;
	mat4x4_identity(m);
	//mat4x4_rotate_Z(m, m, (float)glfwGetTime());
	mat4x4_ortho(p, -context.ratio, context.ratio, -1.f, 1.f, 1.f, -1.f);
	mat4x4_mul(mvp, p, m);

	// Copy the particle data
	glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(particle) * particles.count(), particles.get<particle>().data());

	// Draw the particles
	glUseProgram(data.program);
	glUniformMatrix4fv(data.mvp_location, 1, GL_FALSE, &mvp[0][0]);
	glBindVertexArray(data.vertex_array);
	glDrawArrays(GL_POINTS, 0, static_cast<GLsizei>(particles.count()));
});
