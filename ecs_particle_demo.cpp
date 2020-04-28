#include <iostream>
#include <ecs/entity.h>
#include <ecs/runtime.h>

#include <glad/gl.h>
#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

#include "components/input.h"
#include "components/frame_context.h"
#include "components/render_data.h"
#include "components/particle.h"

#include "shaders.h"

// The 'main' entity
constexpr ecs::entity engine{ -1 };

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
	std::cout << "GLFW Error(" << error << "): " << description << '\n';
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
    engine.add(input{ window, key, scancode, action, mods });
}

// Set up the buffers and shaders to render the particles
static void particle_render_setup() {

    // The particles
    constexpr ecs::entity_range particles{ 0, max_num_particles };

    render_data pr;

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

    engine.add(std::move(pr));
}


int main() {
	if (!glfwInit()) {
		std::cout << "Failed to init glfw.\n";
		return 1;
	}

    // Print out the help
    print_help();

    // Setup opengl version
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwSetErrorCallback(error_callback);

	GLFWwindow* window = glfwCreateWindow(800, 800, "ECS particle demo", NULL, NULL);
    engine.add(std::move(window));

    if (window == nullptr) {
        glfwTerminate();
        return 1;
    }

	glfwSetKeyCallback(window, key_callback);
	glfwMakeContextCurrent(window);
    gladLoadGL(glfwGetProcAddress);

    particle_render_setup();

    frame_context& frame = ecs::get_global_component<frame_context>();
    frame.time = static_cast<float>(glfwGetTime());

    while (!glfwWindowShouldClose(window)) {
        // Set up the viewport
        glfwGetFramebufferSize(window, &frame.width, &frame.height);
        frame.ratio = static_cast<float>(frame.width);
        frame.ratio /= frame.height;

        // Update the frame time
        float const time = static_cast<float>(glfwGetTime());
        frame.dt = time - frame.time;
        frame.time = time;

        // Update cursor position
        double cx = 0, cy = 0;
        glfwGetCursorPos(window, &cx, &cy);
        frame.cursor_x = static_cast<float>(cx / frame.width);    // [0, 1]
        frame.cursor_y = static_cast<float>(cy / frame.height);
        frame.cursor_y = 1 - frame.cursor_y;
        frame.cursor_x -= 0.5f;      // [-0.5, 0.5]
        frame.cursor_y -= 0.5f;
        frame.cursor_x *= 2;        // [-1, 1]
        frame.cursor_y *= 2;
        frame.cursor_x = std::clamp<float>(frame.cursor_x, -1, +1);
        frame.cursor_y = std::clamp<float>(frame.cursor_y, -1, +1);

        // Set up and clear the viewport
        glViewport(0, 0, frame.width, frame.height);
        glClear(GL_COLOR_BUFFER_BIT);

        // Commit component/system changes and run the systems
        ecs::update_systems();

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwDestroyWindow(window);

	glfwTerminate();
	return 0;
}
