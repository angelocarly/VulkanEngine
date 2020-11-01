//
// Created by magnias on 20/10/20.
//

#include "Game.h"
#include "graphics/Vulkan.h"
#include <vulkan/vulkan.h>
#include <GLFW/glfw3.h>
#include <iostream>

vks::Buffer buffer;

Game::Game() : window(Window(800, 600)),
               vulkan(Vulkan(this->window.getGLFWwindow()))
{
    this->vulkan.initVulkan();

    vulkan.createVertexBuffer(&buffer);
    vulkan.createCommandBuffers(this);
}

Game::~Game() {
    buffer.cleanup();
    vulkan.cleanup();
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

void Game::draw(VkCommandBuffer cmdbuffer, VkPipelineLayout pipelineLayout)
{
    VkBuffer vertexBuffers[] = {buffer.buffer};
    VkDeviceSize offsets[] = {0};
    vkCmdBindVertexBuffers(cmdbuffer, 0, 1, vertexBuffers, offsets);
}



