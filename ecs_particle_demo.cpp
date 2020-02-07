#include <ecs/ecs.h>

#include <glad/gl.h>
#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

#include "ecs_particle_demo.h"
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
    ecs::add_component(-1, input{ window, key, scancode, action, mods });
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

	GLFWwindow* window = glfwCreateWindow(800, 800, "ECS particle demo", NULL, NULL);
	if (window) {
		glfwSetKeyCallback(window, key_callback);
		glfwMakeContextCurrent(window);
        gladLoadGL(glfwGetProcAddress);

        frame_context& frame = ecs::get_shared_component<frame_context>();
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
    }

	glfwTerminate();
	return 0;
}
