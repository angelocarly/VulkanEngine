
#include "vks_input.h"
#include <glm/glm.hpp>

bool VksInput::_keys[GLFW_KEY_LAST];
glm::vec2 VksInput::_mousePos;
glm::vec2 VksInput::_mouseDelta;

void VksInput::key_callback(GLFWwindow *window, int key, int scancode, int action, int mods)
{
    _keys[key] = action != GLFW_RELEASE;
}

void VksInput::mouse_button_callback(GLFWwindow *window, int button, int action, int mods)
{

}

void VksInput::mouse_cursor_callback(GLFWwindow *window, double xpos, double ypos)
{
    glm::vec2 prevMousePos = _mousePos;
    _mousePos.x = (float) xpos;
    _mousePos.y = (float) ypos;
    glm::vec2 delta = _mousePos - prevMousePos;

    _mouseDelta += delta;
}
