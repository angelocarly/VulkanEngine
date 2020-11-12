//
// Created by magnias on 20/10/20.
//

#include "Game.h"
#include "graphics/Vulkan.h"
#include <vulkan/vulkan.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <vector>
#include <chrono>


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

struct UniformBufferObject {
    glm::mat4 model;
    glm::mat4 view;
    glm::mat4 proj;
};
std::vector<VkBuffer> uniformBuffers;
std::vector<VkDeviceMemory> uniformBuffersMemory;

VkDescriptorPool descriptorPool;
std::vector<VkDescriptorSet> descriptorSets;
VkDescriptorSetLayout descriptorSetLayout;

Game::Game() : window(Window(800, 600)),
               vulkan(Vulkan(this->window.getGLFWwindow()))
{
    vulkan.createInstance();
    vulkan.setupDebugMessenger();
    vulkan.createSurface();
    vulkan.pickPhysicalDevice();
    vulkan.createLogicalDevice();
    vulkan.createSwapChain();
    vulkan.createImageViews();
    vulkan.createRenderPass();
    createDescriptorSetLayout();
    vulkan.createGraphicsPipeline(&descriptorSetLayout);
    vulkan.createFramebuffers();
    vulkan.createCommandPool();
//    createVertexBuffer();
//    vulkan.createIndexBuffer();
//    createUniformBuffers();
//    createDescriptorPool();
//    createDescriptorSets();
//    createCommandBuffers();

    VkDeviceSize size = sizeof(vertices[0]) * vertices.size();

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

    createUniformBuffers(4);
    createDescriptorPool(4);
    createDescriptorSets(4);

    vulkan.createCommandBuffers(this);
    vulkan.createSyncObjects();

}

void Game::createDescriptorSetLayout() {
    VkDescriptorSetLayoutBinding uboLayoutBinding{};
    uboLayoutBinding.binding = 0;
    uboLayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
    uboLayoutBinding.descriptorCount = 1;
    uboLayoutBinding.stageFlags = VK_SHADER_STAGE_VERTEX_BIT;
    uboLayoutBinding.pImmutableSamplers = nullptr;

    VkDescriptorSetLayoutCreateInfo layoutInfo{};
    layoutInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
    layoutInfo.bindingCount = 1;
    layoutInfo.pBindings = &uboLayoutBinding;

    if (vkCreateDescriptorSetLayout(vulkan.device, &layoutInfo, nullptr, &descriptorSetLayout) != VK_SUCCESS) {
        throw std::runtime_error("failed to create descriptor set layout!");
    }
}

void Game::createUniformBuffers(int size) {
    VkDeviceSize bufferSize = sizeof(UniformBufferObject);

    uniformBuffers.resize(size);
    uniformBuffersMemory.resize(size);

    for (size_t i = 0; i < size; i++) {
        vulkan.createBuffer(VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
                     VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
                     uniformBuffers[i],
                     uniformBuffersMemory[i],
                     bufferSize,
                     nullptr);
    }

}

Game::~Game() {

}

void Game::run()
{
    while (!this->window.shouldClose())
    {
        this->update();
        this->window.pollEvents();
        this->render();
        this->vulkan.drawFrame();
    }

    destroy();
}

void Game::createDescriptorPool(int size) {
    VkDescriptorPoolSize poolSize{};
    poolSize.type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
    poolSize.descriptorCount = static_cast<uint32_t>(size);

    VkDescriptorPoolCreateInfo poolInfo{};
    poolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
    poolInfo.poolSizeCount = 1;
    poolInfo.pPoolSizes = &poolSize;
    poolInfo.maxSets = static_cast<uint32_t>(size);

    if (vkCreateDescriptorPool(vulkan.device, &poolInfo, nullptr, &descriptorPool) != VK_SUCCESS) {
        throw std::runtime_error("failed to create descriptor pool");
    }
}

void Game::createDescriptorSets(int size) {
    std::vector<VkDescriptorSetLayout> layouts(size, descriptorSetLayout);
    VkDescriptorSetAllocateInfo allocInfo{};
    allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
    allocInfo.descriptorPool = descriptorPool;
    allocInfo.descriptorSetCount = static_cast<uint32_t>(size);
    allocInfo.pSetLayouts = layouts.data();

    descriptorSets.resize(size);
    if (vkAllocateDescriptorSets(vulkan.device, &allocInfo, descriptorSets.data()) != VK_SUCCESS) {
        throw std::runtime_error("failed to allocate descriptor sets");
    }

    for (size_t i = 0; i < size; i++) {
        VkDescriptorBufferInfo bufferInfo{};
        bufferInfo.buffer = uniformBuffers[i];
        bufferInfo.offset = 0;
        bufferInfo.range = sizeof(UniformBufferObject);

        VkWriteDescriptorSet descriptorWrite{};
        descriptorWrite.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
        descriptorWrite.dstSet = descriptorSets[i];
        descriptorWrite.dstBinding = 0;
        descriptorWrite.dstArrayElement = 0;
        descriptorWrite.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
        descriptorWrite.descriptorCount = 1;
        descriptorWrite.pBufferInfo = &bufferInfo;
        descriptorWrite.pImageInfo = nullptr;
        descriptorWrite.pTexelBufferView = nullptr;

        vkUpdateDescriptorSets(vulkan.device, 1, &descriptorWrite, 0, nullptr);
    }
}

void Game::updateUniformBuffer(uint32_t currentImage) {
    static auto startTime = std::chrono::high_resolution_clock::now();

    auto currentTime = std::chrono::high_resolution_clock::now();
    float time = std::chrono::duration<float, std::chrono::seconds::period>(currentTime - startTime).count();

    UniformBufferObject ubo{};
    ubo.model = glm::rotate(glm::mat4(1.0f), time * glm::radians(90.0f), glm::vec3(0.0f, 0.0f, 1.0f));
    ubo.view = glm::lookAt(glm::vec3(2.0f, 2.0f, 2.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f));
    ubo.proj = glm::perspective(glm::radians(45.0f), window.width / (float) window.height, 0.1f,
                                10.0f);

    ubo.proj[1][1] *= -1;

    void *data;
    vkMapMemory(vulkan.device, uniformBuffersMemory[currentImage], 0, sizeof(ubo), 0, &data);
    memcpy(data, &ubo, sizeof(ubo));
    vkUnmapMemory(vulkan.device, uniformBuffersMemory[currentImage]);
}

void Game::destroy()
{
    // Destroy objects
    vkDeviceWaitIdle(vulkan.device);
    vulkan.cleanupSwapChain();

    vkDestroyDescriptorSetLayout(vulkan.device, descriptorSetLayout, nullptr);
    vkDestroyDescriptorPool(vulkan.device, descriptorPool, nullptr);

    for (size_t i = 0; i < uniformBuffers.size(); i++) {
        vkDestroyBuffer(vulkan.device, uniformBuffers[i], nullptr);
        vkFreeMemory(vulkan.device, uniformBuffersMemory[i], nullptr);
    }

    buffer.destroy();
    indexBuffer.destroy();
    vulkan.cleanup();


    window.destroy();
}

void Game::update()
{
//    std::cout << "update";
    updateUniformBuffer(vulkan.currentFrame);
}

void Game::render()
{

}

void Game::draw(VkCommandBuffer cmdbuffer, VkPipelineLayout pipelineLayout, int index)
{
    updateUniformBuffer(index);

    VkBuffer vertexBuffers[] = {buffer.buffer};
    VkDeviceSize offsets[] = {0};
    vkCmdBindVertexBuffers(cmdbuffer, 0, 1, vertexBuffers, offsets);

    vkCmdBindIndexBuffer(cmdbuffer, indexBuffer.buffer, 0, VK_INDEX_TYPE_UINT16);

    vkCmdBindDescriptorSets(cmdbuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, pipelineLayout, 0, 1,
                            &descriptorSets[vulkan.currentFrame], 0,
                            nullptr);

    vkCmdDrawIndexed(cmdbuffer, static_cast<uint32_t>(indices.size()), 1, 0, 0, 0);
}



