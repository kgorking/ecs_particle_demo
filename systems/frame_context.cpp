#include <ecs/ecs.h>
#include <glad/gl.h>
#include <GLFW/glfw3.h>

#include "../components/frame_context.h"
#include "../components/render_data.h"
#include "../components/glfw_window.h"
#include "../components/tag_main.h"

struct tag_main;

void setup_frame_context() {
    ecs::make_system([](tag_main, frame_context& fc, glfw_window const& window) {
        // Set up the viewport
        glfwGetFramebufferSize(window.handle, &fc.width, &fc.height);
        fc.ratio = static_cast<float>(fc.width);
        fc.ratio /= fc.height;

        // Update the fc time
        float const time = static_cast<float>(glfwGetTime());
        fc.dt = time - fc.time;
        fc.time = time;

        // Update cursor position
        double cx = 0, cy = 0;
        glfwGetCursorPos(window.handle, &cx, &cy);
        fc.cursor_x = static_cast<float>(cx / fc.width);    // [0, 1]
        fc.cursor_y = static_cast<float>(cy / fc.height);
        fc.cursor_y = 1 - fc.cursor_y;
        fc.cursor_x -= 0.5f;      // [-0.5, 0.5]
        fc.cursor_y -= 0.5f;
        fc.cursor_x *= 2;        // [-1, 1]
        fc.cursor_y *= 2;
        fc.cursor_x = std::clamp<float>(fc.cursor_x, -1, +1);
        fc.cursor_y = std::clamp<float>(fc.cursor_y, -1, +1);

        // Set up and clear the viewport
        glViewport(0, 0, fc.width, fc.height);
        glClear(GL_COLOR_BUFFER_BIT);
    });
}
