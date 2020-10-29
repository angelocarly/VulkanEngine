//
// Created by magnias on 20/10/20.
//

#include "Game.h"
#include "graphics/Vulkan.h"
#include <vulkan/vulkan.h>
#include <GLFW/glfw3.h>
#include <iostream>

Game::Game() : vulkan(Vulkan(this->window.getGLFWwindow()))
{
//    vulkan = Vulkan(this->window);
    this->vulkan.initVulkan();
}

void Game::run()
{
    while (!this->window.shouldClose())
    {
        this->window.pollEvents();
        this->update();
        this->render();
        this->vulkan.drawFrame();
    }

    this->vulkan.cleanup();
    window.destroy();
}

void Game::update()
{
//    std::cout << "update";
}

void Game::render()
{

}


