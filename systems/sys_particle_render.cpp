#include <ecs/ecs.h>
#include <glad/gl.h>

#include "../components/particle.h"
#include "../components/velocity.h"
#include "../components/frame_context.h"
#include "../shaders.h"
#include "linmath.h"

// Create the particles
static ecs::entity_range const particles{ 0, max_num_particles,
	particle_init,   // initialize the particles using the lambda
	frame_context{}, // the static frame context. Only uses O(1) memory
	velocity_init    // initialize the velocities using the lambda
};

// Component that holds the data for rendering particles
struct particle_render {
	GLuint vertex_buffer;
	GLuint vertex_array;
	GLuint vertex_shader;
	GLuint fragment_shader;
	GLuint program;
	GLint mvp_location;
	GLint vpos_location;
	GLint vcol_location;
};

// Component used to initialize the rendering.
// TODO: Find a better way?
struct render_init { ecs_flags(ecs::transient, ecs::tag); };

// Add the render_init component to the 'main' entity
static ecs::entity render{ -1, render_init{} };

// System that sets up the buffers and shaders to render the particles
static ecs::system const& sys_particle_init = ecs::add_system([](render_init const&) {
	particle_render pr;

	glGenBuffers(1, &pr.vertex_buffer);
	glBindBuffer(GL_ARRAY_BUFFER, pr.vertex_buffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(particle) * particles.count(), nullptr, GL_DYNAMIC_DRAW);

	pr.vertex_shader = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(pr.vertex_shader, 1, &vertex_shader_text, NULL);
	glCompileShader(pr.vertex_shader);

	pr.fragment_shader = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(pr.fragment_shader, 1, &fragment_shader_text, NULL);
	glCompileShader(pr.fragment_shader);

	pr.program = glCreateProgram();
	glAttachShader(pr.program, pr.vertex_shader);
	glAttachShader(pr.program, pr.fragment_shader);
	glLinkProgram(pr.program);

	pr.mvp_location = glGetUniformLocation(pr.program, "MVP");
	pr.vpos_location = glGetAttribLocation(pr.program, "vPos");
	pr.vcol_location = glGetAttribLocation(pr.program, "vCol");

	glGenVertexArrays(1, &pr.vertex_array);
	glBindVertexArray(pr.vertex_array);
	glEnableVertexAttribArray(pr.vpos_location);
	glVertexAttribPointer(pr.vpos_location, 2, GL_FLOAT, GL_FALSE, sizeof(particle), reinterpret_cast<void const*>(offsetof(particle, x)));
	glEnableVertexAttribArray(pr.vcol_location);
	glVertexAttribPointer(pr.vcol_location, 3, GL_FLOAT, GL_FALSE, sizeof(particle), reinterpret_cast<void const*>(offsetof(particle, r)));

	glPointSize(2);

	render.add(pr, frame_context{});
});

// The sytem that does the actual rendering
static ecs::system const& sys_particle_render = ecs::add_system(
	[](particle_render const& render, frame_context const& context) {
		// Set up mvp matrix
		mat4x4 m, p, mvp;
		mat4x4_identity(m);
		//mat4x4_rotate_Z(m, m, (float)glfwGetTime());
		mat4x4_ortho(p, -context.ratio, context.ratio, -1.f, 1.f, 1.f, -1.f);
		mat4x4_mul(mvp, p, m);

		// Copy the particle data
		glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(particle) * particles.count(), particles.get<particle>().data());

		// Draw the particles
		glUseProgram(render.program);
		glUniformMatrix4fv(render.mvp_location, 1, GL_FALSE, &mvp[0][0]);
		glBindVertexArray(render.vertex_array);
		glDrawArrays(GL_POINTS, 0, static_cast<GLsizei>(ecs::get_component_count<particle>()));
	}
);
