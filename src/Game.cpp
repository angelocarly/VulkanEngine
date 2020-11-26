//
// Created by magnias on 20/10/20.
//

#include <graphics/VulkanInitializers.h>
#include <chrono>
#include "Game.h"

vks::Buffer buffer;
vks::Buffer indexBuffer;

const std::vector<uint16_t> indices = {
        0, 1, 2, 2, 3, 0, 3, 4, 5
};

std::vector<Vulkan::Vertex> vertices = {
        {{-0.5f, -0.5f}, {1.0f, 0.0f, 0.0f}},
        {{0.5f,  -0.5f}, {1.0f, 1.0f, 0.0f}},
        {{0.5f,  0.5f},  {1.0f, 1.0f, 1.0f}},
        {{-0.5f, 0.5f},  {1.0f, 1.0f, 1.0f}}
};

struct UniformBufferObject
{
    glm::mat4 model;
    glm::mat4 view;
    glm::mat4 proj;
};
std::vector<VkBuffer> uniformBuffers;
std::vector<VkDeviceMemory> uniformBuffersMemory;

VkDescriptorPool descriptorPool;
std::vector<VkDescriptorSet> descriptorSets;
VkDescriptorSetLayout descriptorSetLayout;

void Game::run()
{
    while (!this->renderManager.window.shouldClose())
    {
        this->update();
        this->renderManager.window.pollEvents();
//        this->render();
        this->renderManager.vulkan.drawFrame();
    }
    update();

    destroy();
    renderManager.destroy();
}

Game::Game() : renderManager(RenderManager())
{
    //Setup local rendering shit
    // Descriptors
    createDescriptorPool(4);
    createDescriptorSetLayout();

    renderManager.vulkan.createGraphicsPipeline(&descriptorSetLayout);

    VkDeviceSize size = sizeof(vertices[0]) * vertices.size();

    VkDeviceSize indexSize = sizeof(indices[0]) * indices.size();

    vks::Buffer stagingbuffer;
    vks::Buffer indexStagingbuffer;

    // Create buffers with staging
    renderManager.vulkan.createBuffer(VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
                                      VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT,
                                      stagingbuffer,
                                      size,
                                      vertices.data());

    renderManager.vulkan.createBuffer(VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
                                      VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT,
                                      indexStagingbuffer,
                                      indexSize,
                                      (void *) indices.data());

    renderManager.vulkan.createBuffer(VK_BUFFER_USAGE_VERTEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT,
                                      VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
                                      buffer,
                                      size,
                                      nullptr);

    renderManager.vulkan.createBuffer(VK_BUFFER_USAGE_INDEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT,
                                      VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
                                      indexBuffer,
                                      indexSize,
                                      nullptr);

    renderManager.vulkan.copyBuffer(stagingbuffer.buffer, buffer.buffer, size);
    renderManager.vulkan.copyBuffer(indexStagingbuffer.buffer, indexBuffer.buffer, indexSize);

    stagingbuffer.destroy();
    indexStagingbuffer.destroy();

    createUniformBuffers(4);
    createDescriptorSets(4);

    renderManager.vulkan.createCommandBuffers(this);
    renderManager.vulkan.createSyncObjects();
}

void Game::createDescriptorPool(int size)
{
    std::vector<VkDescriptorPoolSize> poolSize =
            {
                    vks::initializers::descriptorPoolSize(
                            VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
                            static_cast<uint32_t>(size)
                    )
            };

    VkDescriptorPoolCreateInfo poolInfo = vks::initializers::descriptorPoolCreateInfo(
            poolSize,
            size
    );

    if (vkCreateDescriptorPool(renderManager.vulkan.device, &poolInfo, nullptr, &descriptorPool) != VK_SUCCESS)
    {
        throw std::runtime_error("failed to create descriptor pool");
    }
}

void Game::createDescriptorSets(int size)
{
    std::vector<VkDescriptorSetLayout> layouts(size, descriptorSetLayout);
    VkDescriptorSetAllocateInfo allocInfo = vks::initializers::descriptorSetAllocateInfo(
            descriptorPool,
            layouts.data(),
            static_cast<uint32_t>(size)
    );

    descriptorSets.resize(size);
    if (vkAllocateDescriptorSets(renderManager.vulkan.device, &allocInfo, descriptorSets.data()) != VK_SUCCESS)
    {
        throw std::runtime_error("failed to allocate descriptor sets");
    }

    for (size_t i = 0; i < size; i++)
    {
        VkDescriptorBufferInfo bufferInfo{};
        bufferInfo.buffer = uniformBuffers[i];
        bufferInfo.offset = 0;
        bufferInfo.range = sizeof(UniformBufferObject);

        VkWriteDescriptorSet descriptorWrite = vks::initializers::writeDescriptorSet(
                descriptorSets[i],
                VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
                0,
                &bufferInfo,
                1
        );

        vkUpdateDescriptorSets(renderManager.vulkan.device, 1, &descriptorWrite, 0, nullptr);
    }
}

void Game::createDescriptorSetLayout()
{

    VkDescriptorSetLayoutBinding uboLayoutBinding = vks::initializers::descriptorSetLayoutBinding(
            VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
            VK_SHADER_STAGE_VERTEX_BIT, // Bind to shader vertex
            0, // Index
            1 // Amount of descriptors in layout
    );

    VkDescriptorSetLayoutCreateInfo layoutInfo = vks::initializers::descriptorSetLayoutCreateInfo(
            &uboLayoutBinding,
            1
    );

    if (vkCreateDescriptorSetLayout(renderManager.vulkan.device, &layoutInfo, nullptr, &descriptorSetLayout) !=
        VK_SUCCESS)
    {
        throw std::runtime_error("failed to create descriptor set layout!");
    }
}

void Game::createUniformBuffers(int size)
{
    VkDeviceSize bufferSize = sizeof(UniformBufferObject);

    uniformBuffers.resize(size);
    uniformBuffersMemory.resize(size);

    for (size_t i = 0; i < size; i++)
    {
        renderManager.vulkan.createBuffer(VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
                                          VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
                                          uniformBuffers[i],
                                          uniformBuffersMemory[i],
                                          bufferSize,
                                          nullptr);
    }

}

void Game::updateUniformBuffer(uint32_t currentImage)
{
    static auto startTime = std::chrono::high_resolution_clock::now();

    auto currentTime = std::chrono::high_resolution_clock::now();
    float time = std::chrono::duration<float, std::chrono::seconds::period>(currentTime - startTime).count();

    UniformBufferObject ubo{};
    ubo.model = glm::rotate(glm::mat4(1.0f), time * glm::radians(90.0f), glm::vec3(0.0f, 0.0f, 1.0f));
    ubo.view = glm::lookAt(glm::vec3(2.0f, 2.0f, 2.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f));
    ubo.proj = glm::perspective(glm::radians(45.0f), 800 / (float) 600, 0.1f, // TODO dynamic ratio
                                10.0f);

    ubo.proj[1][1] *= -1;

    void *data;
    vkMapMemory(renderManager.vulkan.device, uniformBuffersMemory[currentImage], 0, sizeof(ubo), 0, &data);
    memcpy(data, &ubo, sizeof(ubo));
    vkUnmapMemory(renderManager.vulkan.device, uniformBuffersMemory[currentImage]);
}

void Game::destroy()
{
    // Destroy objects
    vkDeviceWaitIdle(renderManager.vulkan.device);
    renderManager.vulkan.cleanupSwapChain();

    vkDestroyDescriptorSetLayout(renderManager.vulkan.device, descriptorSetLayout, nullptr);
    vkDestroyDescriptorPool(renderManager.vulkan.device, descriptorPool, nullptr);

    for (size_t i = 0; i < uniformBuffers.size(); i++)
    {
        vkDestroyBuffer(renderManager.vulkan.device, uniformBuffers[i], nullptr);
        vkFreeMemory(renderManager.vulkan.device, uniformBuffersMemory[i], nullptr);
    }

    buffer.destroy();
    indexBuffer.destroy();
    renderManager.vulkan.cleanup();


    renderManager.window.destroy();
}

void Game::update()
{
//    std::cout << "update";
    updateUniformBuffer(renderManager.vulkan.currentFrame);
}

void Game::draw(VkCommandBuffer cmdbuffer, VkPipelineLayout pipelineLayout, int index)
{
    updateUniformBuffer(index);

    VkBuffer vertexBuffers[] = {buffer.buffer};
    VkDeviceSize offsets[] = {0};
    vkCmdBindVertexBuffers(cmdbuffer, 0, 1, vertexBuffers, offsets);

    vkCmdBindIndexBuffer(cmdbuffer, indexBuffer.buffer, 0, VK_INDEX_TYPE_UINT16);

    vkCmdBindDescriptorSets(cmdbuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, pipelineLayout, 0, 1,
                            &descriptorSets[renderManager.vulkan.currentFrame], 0,
                            nullptr);

    vkCmdDrawIndexed(cmdbuffer, static_cast<uint32_t>(indices.size()), 1, 0, 0, 0);
}
