//
// Created by magnias on 20/10/20.
//

#include "Game.h"
#include "graphics/Vulkan.h"
#include <vulkan/vulkan.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <vector>


vks::Buffer buffer;
vks::Buffer indexBuffer;

Game::Game() : window(Window(800, 600)),
               vulkan(Vulkan(this->window.getGLFWwindow()))
{
    this->vulkan.initVulkan();


    std::vector<Vulkan::Vertex> vertices = {
            {{-0.5f, -0.5f}, {1.0f, 0.0f, 0.0f}},
            {{0.5f,  -0.5f}, {1.0f, 1.0f, 0.0f}},
            {{0.5f,  0.5f},  {1.0f, 1.0f, 1.0f}},
            {{-0.5f, 0.5f},  {1.0f, 1.0f, 1.0f}}
    };
    VkDeviceSize size = sizeof(vertices[0]) * vertices.size();

    const std::vector<uint16_t> indices = {
            0, 1, 2, 2, 3, 0
    };
    VkDeviceSize indexSize = sizeof(indices[0]) * indices.size();

    vks::Buffer stagingbuffer;
    vks::Buffer indexStagingbuffer;

    // Create buffers with staging
    vulkan.createBuffer(VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
                         VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT,
                         stagingbuffer,
                         size,
                         vertices.data());

    vulkan.createBuffer(VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
                         VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT,
                         indexStagingbuffer,
                         indexSize,
                         (void *) indices.data());

    vulkan.createBuffer(VK_BUFFER_USAGE_VERTEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT,
                         VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
                         buffer,
                         size,
                         nullptr);

    vulkan.createBuffer(VK_BUFFER_USAGE_INDEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT,
                         VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
                         indexBuffer,
                         indexSize,
                         nullptr);

    vulkan.copyBuffer(stagingbuffer.buffer, buffer.buffer, size);
    vulkan.copyBuffer(indexStagingbuffer.buffer, indexBuffer.buffer, indexSize);

    stagingbuffer.destroy();
    indexStagingbuffer.destroy();

    vulkan.createCommandBuffers(this);
}

Game::~Game() {
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

    // Destroy objects
    vkDeviceWaitIdle(vulkan.device);
    vulkan.cleanupSwapChain();
    buffer.destroy();
    indexBuffer.destroy();
    vulkan.cleanup();
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

    vkCmdBindIndexBuffer(cmdbuffer, indexBuffer.buffer, 0, VK_INDEX_TYPE_UINT16);
}



