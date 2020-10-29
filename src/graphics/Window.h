//
// Created by magnias on 20/10/20.
//

#ifndef VULKANENGINE_WINDOW_H
#define VULKANENGINE_WINDOW_H


#include <GLFW/glfw3.h>

class Window
{
public:
    Window(int width, int height);
    bool shouldClose();
    void pollEvents();

    bool framebufferResized = false;
    static void framebufferResizeCallback(GLFWwindow *window, int width, int height);

    void destroy();

    GLFWwindow *getGLFWwindow();

private:
    GLFWwindow *window_handle;
};


#endif //VULKANENGINE_WINDOW_H
