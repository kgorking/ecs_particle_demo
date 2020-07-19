#include <ecs/ecs.h>
#include <glad/gl.h>

#include "../components/particle.h"
#include "../components/color.h"
#include "../components/velocity.h"
#include "../components/frame_context.h"
#include "../components/render_data.h"
#include "../components/tag_main.h"
#include "linmath.h"

void setup_particle_render() {
	// Create the particles
	ecs::entity_range const particles{
		0, max_num_particles,	// The range of particles
		particle_init,			// initialize the particles
		color_init,	    		// initialize the colors
		velocity_init			// initialize the velocities
	};

    // Set up the buffers and shaders to render the particles.
    // This system is run when the main entity does not have 'render_data' attached.
    // Put in group -2 so it runs before any attempts at rendering takes places.
    ecs::make_system<-2>([particles](tag_main, render_data*) {
        render_data pr;

        glGenBuffers(1, &pr.vertex_buffer);
        glBindBuffer(GL_ARRAY_BUFFER, pr.vertex_buffer);
        glBufferData(GL_ARRAY_BUFFER, (sizeof(particle) + sizeof(color)) * particles.count(), nullptr, GL_DYNAMIC_DRAW);

#include "../shaders.h"

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
        glVertexAttribPointer(pr.vpos_location, 2, GL_FLOAT, GL_FALSE, sizeof(particle), 0);
        glEnableVertexAttribArray(pr.vcol_location);
        glVertexAttribPointer(pr.vcol_location, 3, GL_FLOAT, GL_FALSE, sizeof(color), (void*)(sizeof(particle) * particles.count()));

        glPointSize(2);

        const ecs::entity engine{ -1 };
        engine.add(std::move(pr));
    });
    
    // The system that does the actual rendering. Keeps a copy of the particle range.
	// This system has no obvious dependencies on the other systems, so put it in group 1.
	// If it was left in group 0 it would be scheduled to run concurrently with systems
	// that modify the particle data, and crash
	ecs::make_system<1>([particles](render_data const& data, frame_context const& context) {
		// Set up mvp matrix
		mat4x4 m, p, mvp;
		mat4x4_identity(m);
		//mat4x4_rotate_Z(m, m, (float)glfwGetTime());
		mat4x4_ortho(p, -context.ratio, context.ratio, -1.f, 1.f, 1.f, -1.f);
		mat4x4_mul(mvp, p, m);

		// Copy the particle data
        glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(particle) * particles.count(), particles.get<particle>().data());
        glBufferSubData(GL_ARRAY_BUFFER, sizeof(particle) * particles.count(), sizeof(color) * particles.count(), particles.get<color>().data());

		// Draw the particles
		glUseProgram(data.program);
		glUniformMatrix4fv(data.mvp_location, 1, GL_FALSE, &mvp[0][0]);
		glBindVertexArray(data.vertex_array);
		glDrawArrays(GL_POINTS, 0, static_cast<GLsizei>(particles.count()));
	});
}
