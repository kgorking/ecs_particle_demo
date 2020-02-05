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

static void print_help() {
    std::cout << "\nOptions:\n";
    std::cout << "h: print this help\n";
    std::cout << "v: add/remove velocity component\n";
    std::cout << "r: reset colors\n";
    std::cout << "s: toggle mouse spring on/off\n";
    std::cout << "p: toggle particle painter\n";
    std::cout << '\n';
}

static void error_callback(int error, const char* description) {
	std::cout << "GLFW Error: " << description << '\n';
}

static void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods) {
    // Only react to keys that are pressed down
    if (action != GLFW_PRESS)
        return;

    if (key == GLFW_KEY_ESCAPE) {
        glfwSetWindowShouldClose(window, GLFW_TRUE);
        return;
    }
    if (key == GLFW_KEY_H) {
        print_help();
        return;
    }

    // Allow systems to react to the input
    ecs::entity global{ -1 };
    global.add(input{ window, key, scancode, action, mods });
}

int main() {
	if (!glfwInit()) {
		std::cout << "Failed to init glfw.\n";
		return 1;
	}

    // Create particles
    std::cout << "Creating " << max_num_particles << " particles.\n";
    ecs::entity_range particles{ 0, max_num_particles, [](ecs::entity_id) -> particle {
        float const x = rand() / 16384.0f - 1.0f;
        float const y = rand() / 16384.0f - 1.0f;
        float const r = x / 2 + 0.5f;
        float const g = y / 2 + 0.5f;

        return {
            x, y,
            r, g, 1 - r - g
        };
    }};
    particles.add<frame_context>();
    ecs::commit_changes();

    // Print out the help
    print_help();

    // Setup opengl version
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	GLFWwindow* window = glfwCreateWindow(640, 640, "ECS particle demo", NULL, NULL);
	if (window) {
		glfwSetKeyCallback(window, key_callback);
		glfwMakeContextCurrent(window);
        gladLoadGL(glfwGetProcAddress);


        GLuint vertex_buffer;
        glGenBuffers(1, &vertex_buffer);
        glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer);
        glBufferData(GL_ARRAY_BUFFER, sizeof(particle) * particles.count(), (const void*)ecs::get_component<particle>(0), GL_DYNAMIC_DRAW);

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

        glPointSize(2);

        frame_context& frame = ecs::get_shared_component<frame_context>();
        frame.time = static_cast<float>(glfwGetTime());
        while (!glfwWindowShouldClose(window)) {
            glfwPollEvents();

            // Set up the viewport
            int width, height;
            glfwGetFramebufferSize(window, &width, &height);

            glViewport(0, 0, width, height);
            //glClearColor(0.2, 0.2, 0.2, 1);
            glClear(GL_COLOR_BUFFER_BIT);

            // Update the frame time
            float const time = static_cast<float>(glfwGetTime());
            frame.dt = time - frame.time;
            frame.time = time;

            // Update cursor position
            double cx = 0, cy = 0;
            glfwGetCursorPos(window, &cx, &cy);
            frame.cursor_x = static_cast<float>(cx / width);    // [0, 1]
            frame.cursor_y = static_cast<float>(cy / height);
            frame.cursor_y = 1 - frame.cursor_y;
            frame.cursor_x -= 0.5f;      // [-0.5, 0.5]
            frame.cursor_y -= 0.5f;
            frame.cursor_x *= 2;        // [-1, 1]
            frame.cursor_y *= 2;
            frame.cursor_x = std::clamp<float>(frame.cursor_x, -1, +1);
            frame.cursor_y = std::clamp<float>(frame.cursor_y, -1, +1);

            // Commit changes and run the systems
            ecs::update_systems();


            // Set up mvp matrix
            const float ratio = width / (float)height;
            mat4x4 m, p, mvp;
            mat4x4_identity(m);
            //mat4x4_rotate_Z(m, m, (float)glfwGetTime());
            mat4x4_ortho(p, -ratio, ratio, -1.f, 1.f, 1.f, -1.f);
            mat4x4_mul(mvp, p, m);

            // Copy the particle data
            glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(particle) * particles.count(), (const void*)ecs::get_component<particle>(0));

            // Draw the particles
            glUseProgram(program);
            glUniformMatrix4fv(mvp_location, 1, GL_FALSE, (const GLfloat*)&mvp);
            glBindVertexArray(vertex_array);
            glDrawArrays(GL_POINTS, 0, static_cast<GLsizei>(ecs::get_component_count<particle>()));

            glfwSwapBuffers(window);
        }

        glfwDestroyWindow(window);
    }

	glfwTerminate();
	return 0;
}
