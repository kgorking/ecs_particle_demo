#include <ecs/ecs.h>

#include <glad/gl.h>
#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

#include "ecs_particle_demo.h"
#include "shaders.h"
#include "linmath.h"

#include "components/particle.h"
#include "components/input.h"
#include "components/frame_context.h"

ecs::entity global{ -1 };

static void error_callback(int error, const char* description) {
	std::cout << "GLFW Error: " << description << '\n';
}

static void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods) {
    switch (key) {
        case GLFW_KEY_ESCAPE:
            if (action == GLFW_PRESS) {
                glfwSetWindowShouldClose(window, GLFW_TRUE);
                return;
            }
            break;
    }

    global.add(input{window, key, scancode, action, mods});
}

int main() {
	if (!glfwInit()) {
		std::cout << "Failed to init glfw.\n";
		return 1;
	}

    // Create 1k particles
    std::cout << "Creating " << max_num_particles << " particles.\n";
    ecs::entity_range particles{ 0, max_num_particles, [](ecs::entity_id) -> particle {
        float const x = rand() / 16384.0f - 1.0f;
        float const y = rand() / 16384.0f - 1.0f;
        return {
            x, y,

            x/2+0.5f,
            y/2+0.5f,
            0//rand() / 32768.0f
        };
    }};
    particles.add<frame_context>();
    ecs::update_systems();

    // Print out the help
    std::cout << "\nOptions:\n";
    std::cout << "v: toggle velocity component\n";

    // Setup opengl version
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	GLFWwindow* window = glfwCreateWindow(640, 480, "OpenGL Triangle", NULL, NULL);
	if (window) {
		glfwSetKeyCallback(window, key_callback);
		glfwMakeContextCurrent(window);
        gladLoadGL(glfwGetProcAddress);


        GLuint vertex_buffer;
        glGenBuffers(1, &vertex_buffer);
        glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer);
        //glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_DYNAMIC_DRAW);
        glBufferData(GL_ARRAY_BUFFER, sizeof(particle) * particles.count(), (const void*)&ecs::get_component<particle>(0), GL_DYNAMIC_DRAW);

        const GLuint vertex_shader = glCreateShader(GL_VERTEX_SHADER);
        glShaderSource(vertex_shader, 1, &vertex_shader_text, NULL);
        glCompileShader(vertex_shader);

        const GLuint fragment_shader = glCreateShader(GL_FRAGMENT_SHADER);
        glShaderSource(fragment_shader, 1, &fragment_shader_text, NULL);
        glCompileShader(fragment_shader);

        const GLuint program = glCreateProgram();
        glAttachShader(program, vertex_shader);
        glAttachShader(program, fragment_shader);
        glLinkProgram(program);

        const GLint mvp_location = glGetUniformLocation(program, "MVP");
        const GLint vpos_location = glGetAttribLocation(program, "vPos");
        const GLint vcol_location = glGetAttribLocation(program, "vCol");

        GLuint vertex_array;
        glGenVertexArrays(1, &vertex_array);
        glBindVertexArray(vertex_array);
        glEnableVertexAttribArray(vpos_location);
        glVertexAttribPointer(vpos_location, 2, GL_FLOAT, GL_FALSE, sizeof(particle), (void*)offsetof(particle, x));
        glEnableVertexAttribArray(vcol_location);
        glVertexAttribPointer(vcol_location, 3, GL_FLOAT, GL_FALSE, sizeof(particle), (void*)offsetof(particle, r));

        frame_context& frame = ecs::get_shared_component<frame_context>();
        frame.time = glfwGetTime();
        while (!glfwWindowShouldClose(window)) {

            // Update the frame time
            double const time = glfwGetTime();
            frame.dt = time - frame.time;
            frame.time = time;

            // Commit changes and run the systems
            ecs::update_systems();

            //
            // Render the particles
            int width, height;
            glfwGetFramebufferSize(window, &width, &height);
            const float ratio = width / (float)height;

            glViewport(0, 0, width, height);
            glClear(GL_COLOR_BUFFER_BIT);

            mat4x4 m, p, mvp;
            mat4x4_identity(m);
            //mat4x4_rotate_Z(m, m, (float)glfwGetTime());
            mat4x4_ortho(p, -ratio, ratio, -1.f, 1.f, 1.f, -1.f);
            mat4x4_mul(mvp, p, m);

            // Copy the particle data
            glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(particle) * particles.count(), (const void*)&ecs::get_component<particle>(0));

            glUseProgram(program);
            glUniformMatrix4fv(mvp_location, 1, GL_FALSE, (const GLfloat*)&mvp);
            glBindVertexArray(vertex_array);
            glDrawArrays(GL_POINTS, 0, ecs::get_component_count<particle>());

            glfwSwapBuffers(window);
            glfwPollEvents();
        }

        glfwDestroyWindow(window);
    }

	glfwTerminate();
	return 0;
}
