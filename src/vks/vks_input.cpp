
#include "vks_input.h"
#include <glm/glm.hpp>

bool VksInput::_keys[GLFW_KEY_LAST];
glm::vec2 VksInput::_mousePosBuffer;
glm::vec2 VksInput::_mouseDeltaBuffer;
int VksInput::_buttonsBuffer[GLFW_MOUSE_BUTTON_LAST];

void VksInput::key_callback(GLFWwindow *window, int key, int scancode, int action, int mods)
{
    _keys[key] = action != GLFW_RELEASE;
}

void VksInput::mouse_button_callback(GLFWwindow *window, int button, int action, int mods)
{
    _buttonsBuffer[button] = action;
}

void VksInput::mouse_cursor_callback(GLFWwindow *window, double xpos, double ypos)
{
    glm::vec2 prevMousePos = _mousePosBuffer;
    _mousePosBuffer.x = (float) xpos;
    _mousePosBuffer.y = (float) ypos;

    glm::vec2 delta = _mousePosBuffer - prevMousePos;
    _mouseDeltaBuffer += delta;
}
