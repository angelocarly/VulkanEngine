#ifndef VULKANENGINE_VKS_INPUT_H
#define VULKANENGINE_VKS_INPUT_H

#include <GLFW/glfw3.h>
#include <vulkan/vulkan_core.h>
#include "vks_window.h"
#include <spdlog/spdlog.h>
#include <glm/vec2.hpp>

/**
 * Input handler
 *
 * Stores all glfw input data into intermediate buffers and makes them easily available
 */
class VksInput
{
public:

    VksInput()
    {}

    void init(vks::VksWindow *window)
    {
        _window = window;

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

    glm::vec2 getMouseDelta()
    {
        return _mouseDelta;
    }

    /**
     * Copy all buffered data and clean them up
     */
    void syncInputState()
    {
        _mousePos = _mousePosBuffer;

        _mouseDelta = _mouseDeltaBuffer;
        _mouseDeltaBuffer = glm::vec2();

        for(int i = 0; i < GLFW_MOUSE_BUTTON_LAST; i++)
        {
            _buttons[i] = _buttonsBuffer[i];
            _buttonsBuffer[i] = 0;
        }
    }

    glm::vec2 getMousePos()
    {
        return _mousePos;
    }

    void swallowMouse()
    {
        mouseSwallowed = true;
        glfwSetInputMode(_window->getWindow(), GLFW_CURSOR, GLFW_CURSOR_DISABLED);
        glfwSetInputMode(_window->getWindow(), GLFW_RAW_MOUSE_MOTION, GLFW_TRUE);
    }

    void showMouse()
    {
        mouseSwallowed = false;
        glfwSetInputMode(_window->getWindow(), GLFW_CURSOR, GLFW_CURSOR_NORMAL);
        glfwSetInputMode(_window->getWindow(), GLFW_RAW_MOUSE_MOTION, GLFW_FALSE);
    }

    bool isButtonClicked(int button)
    {
        return _buttons[button] == GLFW_PRESS;
    }

    bool isMouseSwallowed()
    {
        return mouseSwallowed;
    }

private:
    vks::VksWindow *_window;

    bool mouseSwallowed = false;

    static bool _keys[GLFW_KEY_LAST];
    glm::vec2 _mousePos;
    static glm::vec2 _mousePosBuffer;
    glm::vec2 _mouseDelta;
    static glm::vec2 _mouseDeltaBuffer;

    int _buttons[GLFW_MOUSE_BUTTON_LAST];
    static int _buttonsBuffer[GLFW_MOUSE_BUTTON_LAST];

    static void key_callback(GLFWwindow *window, int key, int scancode, int action, int mods);

    static void mouse_button_callback(GLFWwindow *window, int button, int action, int mods);

    static void mouse_cursor_callback(GLFWwindow *window, double xpos, double ypos);
};

#endif //VULKANENGINE_VKS_INPUT_H
