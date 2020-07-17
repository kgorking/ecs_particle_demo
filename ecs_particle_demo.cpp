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
#include "components/glfw_window.h"
#include "components/tag_main.h"

// The 'main' entity
const ecs::entity engine{ -1, tag_main{} };

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

// Create the systems
void create_systems() {
#define ECS_SETUP(x) extern void setup_##x(); setup_##x();
    ECS_SETUP(frame_context);
    ECS_SETUP(velocity);
    ECS_SETUP(mouse_spring);
    ECS_SETUP(collision);
    ECS_SETUP(reset_colors);
    ECS_SETUP(particle_painter);
    ECS_SETUP(particle_render);
#undef ECS_SETUP
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

    // Create the window and add it to the engine entity
	GLFWwindow* window = glfwCreateWindow(800, 800, "ECS particle demo", NULL, NULL);
    if (window == nullptr) {
        std::cout << "glfw could not create a window.\n";
        glfwTerminate();
        return 1;
    }
    ecs::get_global_component<glfw_window>().handle = window;

	glfwSetKeyCallback(window, key_callback);
	glfwMakeContextCurrent(window);
    gladLoadGL(glfwGetProcAddress);

    create_systems();

    while (!glfwWindowShouldClose(window)) {
        // Commit component/system changes and run the systems
        ecs::update_systems();

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwDestroyWindow(window);

	glfwTerminate();
	return 0;
}
