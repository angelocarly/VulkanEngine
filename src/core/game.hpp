#ifndef VULKANENGINE_GAME_H
#define VULKANENGINE_GAME_H


#include <bits/stdint-uintn.h>
#include <cstddef>
#include <cstring>
#include <glm/ext/matrix_clip_space.hpp>
#include <glm/ext/matrix_transform.hpp>
#include <glm/fwd.hpp>
#include <glm/ext.hpp>
#include <glm/gtx/string_cast.hpp>
#include <vulkan/vulkan_core.h>

#pragma once

#define GLFW_INCLUDE_VULKAN
#define GLM_ENABLE_EXPERIMENTAL

#include <GLFW/glfw3.h>

#include <iostream>
#include <stdexcept>
#include <cstdlib>
#include <vks/vks_window.h>
#include <vks/vks_device.h>
#include <vks/vks_swap_chain.h>
#include <vks/vks_pipeline.h>
#include <memory>
#include <imgui/imgui.h>
#include <imgui/imgui_impl_glfw.h>
#include <imgui/imgui_impl_vulkan.h>
#include <vks/vks_model.h>
#include <spdlog/spdlog.h>

#define GLM_FORCE_DEFAULT_ALIGNED_GENTYPES

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <chrono>
#include <libwebsockets.h>
#include <core/graphics/renderable.h>
#include <core/game/world.h>
#include "camera.h"

const bool VALIDATION_LAYERS_ENABLED = true;
const bool VSYNC = false;

using namespace vks;

class Game
{
public:

    Game(VksWindow &window)
            : window(window)
    {

    };

    void init()
    {
        loadModel();
        createDescriptorSetLayout();
        createPipelineLayout();
        recreateSwapChain();
        createUniformBuffers();
        createDescriptorPool();
        createDescriptorSets();
        createCommandBuffers();
        initImGui();

        handler.init(&window);
        camera.setInputHandler(&handler);
        handler.swallowMouse();
    }

    // Render game and handle input
    void update(float delta)
    {
        input(delta);

        if (!imguiDataAvailable)
        {
            imGuiCreateRenderData();
            imguiDataAvailable = true;
        }


        recordCommandBuffers();
        drawFrame();

        handler.syncInputState();
    }

    void destroy()
    {
        // Wait until any operation towards the gpu is completed
        device.waitIdle();
    }

    ~Game()
    {

//        vkDestroyDescriptorSetLayout(device.getVkDevice(), descriptorSetLayout, nullptr);
        //            vkDestroyCommandPool(_device.getVkDevice(), command)

        //            vkDestroyPipeline(_device.getVkDevice(), pipeline.release()->getPipeline(), nullptr);
//        vkDestroyDescriptorPool(device.getVkDevice(), descriptorPool, nullptr);
//        vkDestroyPipelineLayout(device.getVkDevice(), pipelineLayout, nullptr);

//        for (size_t i = 0; i < uniformBuffers.size(); i++)
//        {
//            vkDestroyBuffer(device.getVkDevice(), uniformBuffers[i], nullptr);
//            vkFreeMemory(device.getVkDevice(), uniformBuffersMemory[i], nullptr);
//        }


        ImGui_ImplVulkan_Shutdown();
        ImGui_ImplGlfw_Shutdown();
        ImGui::DestroyContext();

//        device.destroy();
    }

private:

    VksWindow &window;

    // Vulkan properties
    VksDevice device = VksDevice(window, VALIDATION_LAYERS_ENABLED);
    std::unique_ptr<VksSwapChain> swapChain;
    std::unique_ptr<VksPipeline> pipeline;
    VkPipelineLayout pipelineLayout;
    std::vector<VkCommandBuffer> commandBuffers;
    std::vector<VkBuffer> uniformBuffers;
    std::vector<VkDeviceMemory> uniformBuffersMemory;
    VkDescriptorSetLayout descriptorSetLayout;
    std::vector<VkDescriptorSet> descriptorSets;
    VkDescriptorPool descriptorPool;
    VksInput handler = VksInput();
    Camera camera;

    VulkanRenderManager vulkanRenderManager;

    bool imguiDataAvailable = false;
    IRenderable* world = new World(device);
    VkResult err;

    // ImGui
    float gui_x = 0;
    float gui_y = 0;
    float gui_z = 0;

    struct UniformBufferObject
    {
        glm::mat4 model;
        glm::mat4 view;
        glm::mat4 proj;
    };

    struct MeshPushConstants
    {
        glm::mat4 transform;
    };

    void loadModel();

    void input(float delta)
    {

        if (handler.isKeyDown(GLFW_KEY_ESCAPE))
        {
            handler.showMouse();
        }

        if (handler.isButtonClicked(GLFW_MOUSE_BUTTON_1))
        {

            ImGuiIO &io = ImGui::GetIO();
            if (!io.WantCaptureMouse)
            {
                handler.swallowMouse();
            }
        }

//        entity.position = glm::vec3(gui_x, gui_y, gui_z);

        if (!handler.isMouseSwallowed()) return;

        camera.update(delta);

    }

    /**
     * Handle vulkan error
     * @param err
     */
    static void check_vk_result(VkResult err)
    {
        if (err == 0)
            return;
        fprintf(stderr, "[vulkan] Error: VkResult = %d\n", err);
        if (err < 0)
            abort();
    }

    /**
     * Setup the ImGui GUI bindings
     */
    void initImGui()
    {

        IMGUI_CHECKVERSION();
        ImGui::CreateContext();
        ImGuiIO &io = ImGui::GetIO();
        (void) io;
        //            io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
        //            io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;
        //io.Fonts->AddFontFromFileTTF("../../Assets/Fonts/Roboto-Medium.ttf", 16.0f);

        io.DisplaySize = ImVec2(window.getWidth(), window.getHeight());
        io.DisplayFramebufferScale = ImVec2(1.0f, 1.0f);

        // Setup Platform/Renderer bindings
        ImGui_ImplGlfw_InitForVulkan(window.getWindow(), true);
        ImGui_ImplVulkan_InitInfo init_info = {};
        init_info.Instance = device.getInstance();
        init_info.PhysicalDevice = device.getPhysicalDevice();
        init_info.Device = device.getVkDevice();
        init_info.QueueFamily = device.findPhysicalQueueFamilies().graphicsFamily.value();
        init_info.Queue = device.getGraphicsQueue();
        init_info.PipelineCache = VK_NULL_HANDLE;
        init_info.DescriptorPool = descriptorPool;
        init_info.Allocator = NULL;
        init_info.MinImageCount = 2;
        init_info.ImageCount = static_cast<uint32_t>(swapChain->getImageCount());
        init_info.CheckVkResultFn = NULL;
        ImGui_ImplVulkan_Init(&init_info, swapChain->getRenderPass());

        // Setup Dear ImGui style
        ImGui::StyleColorsDark();

        VkCommandBuffer commandBuffer = device.beginCommandBuffer();
        ImGui_ImplVulkan_CreateFontsTexture(commandBuffer);
        device.endCommandBuffer(commandBuffer);
        ImGui_ImplVulkan_DestroyFontUploadObjects();
    }

    /**
     * Draw an ImGUI GUI window in the commandbuffer
     */
    void imGuiCreateRenderData()
    {
        ImGuiIO &io = ImGui::GetIO();
        ImGui_ImplVulkan_NewFrame();
        ImGui_ImplGlfw_NewFrame();

        auto WindowSize = ImVec2((float) window.getWidth(), (float) window.getHeight());
        ImGui::SetNextWindowSize(WindowSize, ImGuiCond_::ImGuiCond_FirstUseEver);
        ImGui::SetNextWindowPos(ImVec2(0, 0), ImGuiCond_::ImGuiCond_FirstUseEver);

        ImGui::NewFrame();

        ImGui::Begin("Stats");
        {
            ImGui::Text("Fps: %d", 0);
            ImGui::SliderFloat("x", &gui_x, -1, 1, "%.2f");
            ImGui::SliderFloat("y", &gui_y, -1, 1, "%.2f");
            ImGui::SliderFloat("z", &gui_z, -1, 1, "%.2f");
            ImGui::Text("pos: %f %f %f", camera.getPosition().x,
                        camera.getPosition().y, camera.getPosition().z);
            glm::vec3 look = camera.getForward();
            ImGui::Text("forward: %f %f %f", look.x, look.y, look.z);
            glm::vec3 right = camera.getRight();
            ImGui::Text("right: %f %f %f", right.x, right.y, right.z);
        }
        ImGui::End();

        // Store the created window in memory until it can be rendered
        ImGui::Render();
    }

    /**
     * Create a pipeline layout
     * Stores the set of resources that can be used by the pipeline
     */
    void createPipelineLayout()
    {
        spdlog::get("vulkan")->debug("Creating pipeline layout..");

        VkPushConstantRange push_constant;
        push_constant.offset = 0;
        push_constant.size = sizeof(MeshPushConstants);
        push_constant.stageFlags = VK_SHADER_STAGE_VERTEX_BIT;

        VkPipelineLayoutCreateInfo pipelineLayoutInfo{};
        pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
        pipelineLayoutInfo.setLayoutCount = 1;
        pipelineLayoutInfo.pSetLayouts = &descriptorSetLayout;
        pipelineLayoutInfo.pushConstantRangeCount = 1;
        pipelineLayoutInfo.pPushConstantRanges = &push_constant;
        if (vkCreatePipelineLayout(device.getVkDevice(), &pipelineLayoutInfo, nullptr, &pipelineLayout) !=
            VK_SUCCESS)
        {
            throw std::runtime_error("failed to create pipeline layout!");
        }

    }

    void createPipeline()
    {
        assert(swapChain != nullptr && "Cannot create pipeline before swap chain");
        assert(pipelineLayout != nullptr && "Cannot create pipeline before pipeline layout");

        spdlog::get("vulkan")->debug("Creating pipeline..");

        PipelineConfigInfo pipelineConfigInfo{};
        VksPipeline::defaultPipelineConfigInfo(pipelineConfigInfo);
        pipelineConfigInfo.renderPass = swapChain->getRenderPass();
        pipelineConfigInfo.pipelineLayout = pipelineLayout;
        pipeline = std::make_unique<VksPipeline>(device, *swapChain, pipelineConfigInfo);
    }

    /**
     * Stores the data that will be accessible to the descriptor set
     */
    void createDescriptorSetLayout()
    {
        VkDescriptorSetLayoutBinding uboLayoutBinding{};
        uboLayoutBinding.binding = 0;
        uboLayoutBinding.descriptorCount = 1;
        uboLayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
        uboLayoutBinding.pImmutableSamplers = nullptr;
        uboLayoutBinding.stageFlags = VK_SHADER_STAGE_VERTEX_BIT;

        VkDescriptorSetLayoutCreateInfo layoutInfo{};
        layoutInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
        layoutInfo.bindingCount = 1;
        layoutInfo.pBindings = &uboLayoutBinding;

        if (vkCreateDescriptorSetLayout(device.getVkDevice(), &layoutInfo, nullptr, &descriptorSetLayout) !=
            VK_SUCCESS)
        {
            throw std::runtime_error("failed to create descriptor set layout!");
        }
    }

    void createUniformBuffers()
    {
        VkDeviceSize bufferSize = sizeof(UniformBufferObject);

        uniformBuffers.resize(swapChain->getImageCount());
        uniformBuffersMemory.resize(swapChain->getImageCount());

        for (size_t i = 0; i < swapChain->getImageCount(); i++)
        {
            device.createBuffer(
                    bufferSize,
                    VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
                    VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
                    uniformBuffers[i],
                    uniformBuffersMemory[i]
            );
        }
    }

    void createDescriptorPool()
    {
        VkDescriptorPoolSize pool_sizes[] =
                {
                        {VK_DESCRIPTOR_TYPE_SAMPLER,                5},
                        {VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 5},
                        {VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE,          5},
                        {VK_DESCRIPTOR_TYPE_STORAGE_IMAGE,          5},
                        {VK_DESCRIPTOR_TYPE_UNIFORM_TEXEL_BUFFER,   5},
                        {VK_DESCRIPTOR_TYPE_STORAGE_TEXEL_BUFFER,   5},
                        {VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,         5},
                        {VK_DESCRIPTOR_TYPE_STORAGE_BUFFER,         5},
                        {VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC, 5},
                        {VK_DESCRIPTOR_TYPE_STORAGE_BUFFER_DYNAMIC, 5},
                        {VK_DESCRIPTOR_TYPE_INPUT_ATTACHMENT,       5}
                };

        VkDescriptorPoolCreateInfo pool_info = {};
        pool_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
        pool_info.flags = VK_DESCRIPTOR_POOL_CREATE_FREE_DESCRIPTOR_SET_BIT;
        pool_info.maxSets = swapChain->getImageCount() * IM_ARRAYSIZE(pool_sizes);
        pool_info.poolSizeCount = (uint32_t) IM_ARRAYSIZE(pool_sizes);
        pool_info.pPoolSizes = pool_sizes;
        vkCreateDescriptorPool(device.getVkDevice(), &pool_info, nullptr, &descriptorPool);
    }

    void createDescriptorSets()
    {
        std::vector<VkDescriptorSetLayout> layouts(swapChain->getImageCount(), descriptorSetLayout);
        VkDescriptorSetAllocateInfo allocInfo{};
        allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
        allocInfo.descriptorPool = descriptorPool;
        allocInfo.descriptorSetCount = static_cast<uint32_t>(swapChain->getImageCount());
        allocInfo.pSetLayouts = layouts.data();

        descriptorSets.resize(swapChain->getImageCount());
        if (vkAllocateDescriptorSets(device.getVkDevice(), &allocInfo, descriptorSets.data()) != VK_SUCCESS)
        {
            throw std::runtime_error("failed to allocate descriptor sets!");
        }

        for (size_t i = 0; i < swapChain->getImageCount(); i++)
        {
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
            descriptorWrite.pImageInfo = nullptr; // Optional
            descriptorWrite.pTexelBufferView = nullptr; // Optional
            vkUpdateDescriptorSets(device.getVkDevice(), 1, &descriptorWrite, 0, nullptr);
        }
    }

    void freeCommandBuffers()
    {
        if (commandBuffers.empty()) return;

        vkFreeCommandBuffers(
                device.getVkDevice(),
                device.getCommandPool(),
                static_cast<uint32_t>(commandBuffers.size()),
                commandBuffers.data());
        commandBuffers.clear();
    }

    void createCommandBuffers()
    {
        // Allocate new commandbuffers
        commandBuffers.resize(swapChain->getImageCount());
        VkCommandBufferAllocateInfo allocInfo{};
        allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
        allocInfo.commandPool = device.getCommandPool();
        allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
        allocInfo.commandBufferCount = static_cast<uint32_t>(commandBuffers.size());

        err = vkAllocateCommandBuffers(device.getVkDevice(), &allocInfo, commandBuffers.data());
        check_vk_result(err);
    }

    /**
     * Create the command buffers used to render our scene
     */
    void recordCommandBuffers()
    {
        // Before recreating the command buffers, wait until they are no longer in use
        // Might be a reason for slowdowns in the future
        swapChain->waitForImageInFlight();
        device.waitIdle();

        for (size_t i = 0; i < commandBuffers.size(); i++)
        {
            err = vkResetCommandBuffer(commandBuffers[i], VK_COMMAND_BUFFER_RESET_RELEASE_RESOURCES_BIT);
            check_vk_result(err);

            updateUniformBuffer(i);

            VkCommandBufferBeginInfo beginInfo = {};
            beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
            beginInfo.flags = 0;                   // Optional
            beginInfo.pInheritanceInfo = nullptr;  // Optional

            if (vkBeginCommandBuffer(commandBuffers[i], &beginInfo) != VK_SUCCESS)
            {
                throw std::runtime_error("failed to begin recording command buffer!");
            }
            VkRenderPassBeginInfo renderPassInfo = {};
            renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
            renderPassInfo.renderPass = swapChain->getRenderPass();
            renderPassInfo.framebuffer = swapChain->getFrameBuffer(i);

            renderPassInfo.renderArea.offset = {0, 0};
            renderPassInfo.renderArea.extent = swapChain->getSwapChainExtent();

            std::array<VkClearValue, 2> clearValues{};
            clearValues[0].color = {0.0f, 0.0f, 0.0f, 1.0f};
            clearValues[1].depthStencil = {1.0f, 0};
            renderPassInfo.clearValueCount = static_cast<uint32_t>(clearValues.size());
            renderPassInfo.pClearValues = clearValues.data();

            vkCmdBeginRenderPass(commandBuffers[i], &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);

            VkViewport viewport{};
            viewport.x = 0.0f;
            viewport.y = 0.0f;
            viewport.width = static_cast<float>(swapChain->getSwapChainExtent().width);
            viewport.height = static_cast<float>(swapChain->getSwapChainExtent().height);
            viewport.minDepth = 0.0f;
            viewport.maxDepth = 1.0f;
            VkRect2D scissor{{0, 0}, swapChain->getSwapChainExtent()};
            vkCmdSetViewport(commandBuffers[i], 0, 1, &viewport);
            vkCmdSetScissor(commandBuffers[i], 0, 1, &scissor);

            vkCmdBindDescriptorSets(commandBuffers[i], VK_PIPELINE_BIND_POINT_GRAPHICS, pipelineLayout, 0, 1,
                                    &descriptorSets[i], 0, nullptr);


            pipeline->bind(commandBuffers[i]);

            vulkanRenderManager.startFrame(commandBuffers[i], pipelineLayout);
            world->draw(vulkanRenderManager);
            vulkanRenderManager.drawFrame();

            // Render imgui data
            if (imguiDataAvailable)
            {
                ImGui_ImplVulkan_RenderDrawData(ImGui::GetDrawData(), commandBuffers[i]);
            }

            vkCmdEndRenderPass(commandBuffers[i]);
            if (vkEndCommandBuffer(commandBuffers[i]) != VK_SUCCESS)
            {
                throw std::runtime_error("failed to record command buffer!");
            }
        }
        imguiDataAvailable = false;
    }

    /**
     * Render our command buffers to the swapchain
     */
    void drawFrame()
    {

        // Recreate the swapchain if GLFW emits a resized event
        if (window.pollFrameBufferResized())
        {
            spdlog::get("vulkan")->warn("Swapchain image is out of date");
            recreateSwapChain();
            return;
        }

        // Load the next image in the swapchain
        uint32_t imageIndex;
        auto result = swapChain->acquireNextImage(&imageIndex);
        if (result == VK_ERROR_OUT_OF_DATE_KHR)
        {
            // When the swapchain is out of date, recreate a new one
            spdlog::get("vulkan")->warn("Swapchain image is out of date");
            recreateSwapChain();
            return;
        } else if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR)
        {
            spdlog::get("vulkan")->warn("Swapchain image is not optimal");
        }

        // Submit our command buffer to be drawed on the new image
        result = swapChain->submitCommandBuffers(&commandBuffers[imageIndex], &imageIndex);
        if (result == VK_ERROR_OUT_OF_DATE_KHR)// || result == VK_SUBOPTIMAL_KHR
        {
            // When the swapchain is out of date, recreate a new one
            spdlog::get("vulkan")->warn("Swapchain is out of date");
            recreateSwapChain();

        } else if (result != VK_SUCCESS)
        {
            throw std::runtime_error("failed to present swap chain image!");
        }
    }

    void updateUniformBuffer(uint32_t imageIndex)
    {
        static auto startTime = std::chrono::high_resolution_clock::now();

        auto currentTime = std::chrono::high_resolution_clock::now();
        float time = std::chrono::duration<float, std::chrono::seconds::period>(currentTime - startTime).count();

        UniformBufferObject ubo{};
        ubo.model = camera.calculateModelMatrix();
        ubo.view = camera.calculateViewMatrix();
        ubo.proj = camera.calculateProjectionMatrix();
        ubo.proj[1][1] *= -1;

        ubo.view = glm::inverse(ubo.view);

        void *data;
        vkMapMemory(device.getVkDevice(), uniformBuffersMemory[imageIndex], 0, sizeof(ubo), 0, &data);
        memcpy(data, &ubo, sizeof(ubo));
        vkUnmapMemory(device.getVkDevice(), uniformBuffersMemory[imageIndex]);
    }

    void recreateSwapChain()
    {
        auto extent = window.getExtent();
        while (extent.width == 0 || extent.height == 0)
        {
            extent = window.getExtent();
            glfwWaitEvents();
        }
        vkDeviceWaitIdle(device.getVkDevice());

        if (swapChain == nullptr)
        {
            swapChain = std::make_unique<VksSwapChain>(device, extent, VSYNC);
        } else
        {
            swapChain = std::make_unique<VksSwapChain>(device, extent, VSYNC, std::move(swapChain));
            if (swapChain->getImageCount() != commandBuffers.size())
            {
                freeCommandBuffers();
                createCommandBuffers();
            }
        }

        createPipeline();
    }

};


#endif //VULKANENGINE_GAME_H
