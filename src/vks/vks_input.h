#ifndef VULKANENGINE_VKS_INPUT_H
#define VULKANENGINE_VKS_INPUT_H

#include <GLFW/glfw3.h>
#include <vulkan/vulkan_core.h>
#include "vks_window.h"
#include <spdlog/spdlog.h>
#include <glm/vec2.hpp>

class VksInput
{
public:

    VksInput() {}

    void init(vks::VksWindow *window)
    {
        glfwSetKeyCallback(window->getWindow(), key_callback);
        glfwSetMouseButtonCallback(window->getWindow(), mouse_button_callback);
        glfwSetCursorPosCallback(window->getWindow(), mouse_cursor_callback);

        _mousePos = glm::vec2();
        _mouseDelta = glm::vec2();
    }

    bool isKeyDown(int key)
    {
        return _keys[key];
    }

    glm::vec2 pollMouseDelta() {
        glm::vec2 delta = _mouseDelta;
        _mouseDelta = glm::vec2();
        return delta;
    }

private:
    static bool _keys[GLFW_KEY_LAST];
    static glm::vec2 _mousePos;
    static glm::vec2 _mouseDelta;

    static void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods);
    static void mouse_button_callback(GLFWwindow* window, int button, int action, int mods);
    static void mouse_cursor_callback(GLFWwindow* window, double xpos, double ypos);
};

#endif //VULKANENGINE_VKS_INPUT_H
