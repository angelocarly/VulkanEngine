#ifndef VULKANENGINE_APP_H
#define VULKANENGINE_APP_H

#pragma once

#define GLFW_INCLUDE_VULKAN

#include <GLFW/glfw3.h>

#include <iostream>
#include <stdexcept>
#include <cstdlib>
#include <graphics/vks_window.h>
#include <graphics/vks_device.h>
#include <graphics/vks_swap_chain.h>
#include <graphics/vks_pipeline.h>
#include <memory>
#include <game/Game.h>
#include <imgui/imgui.h>
#include <imgui/imgui_impl_glfw.h>
#include <imgui/imgui_impl_vulkan.h>
#include <graphics/vks_model.h>
#include <spdlog/spdlog.h>

const uint32_t WIDTH = 800;
const uint32_t HEIGHT = 600;

namespace vks
{

class App
{
public:

    void run()
    {
        loadModel();
        createPipelineLayout();
        createPipeline();
        initImGui();
        createCommandBuffers();
        mainLoop();
    }

    ~App()
    {

        //            vkDestroyCommandPool(_device.getVkDevice(), command)

        //            vkDestroyPipeline(_device.getVkDevice(), pipeline.release()->getPipeline(), nullptr);
        vkDestroyDescriptorPool(device.getVkDevice(), descriptorPool, nullptr);
        vkDestroyPipelineLayout(device.getVkDevice(), pipelineLayout, nullptr);
        //            swapChain.destroy();

        ImGui_ImplVulkan_Shutdown();
        ImGui_ImplGlfw_Shutdown();
        ImGui::DestroyContext();

        //            _device.destroy();
    }


private:

    // Vulkan properties
    VksWindow window = VksWindow(WIDTH, HEIGHT, "test");
    VksDevice device = VksDevice(window);
    VksSwapChain swapChain = VksSwapChain(window, device);
    std::unique_ptr<VksPipeline> pipeline;
    VkPipelineLayout pipelineLayout;
    std::vector<VkCommandBuffer> commandBuffers;
    VkDescriptorPool descriptorPool;
    bool imguiwindowcreated = false;
    bool imguirebuildswapchain = false;
    std::unique_ptr<VksModel> vksModel;
    VkResult err;

    // ImGui
    int fps = 1;

    void loadModel();

    /*
     * Main game loop, polls and updates the window and executes the game logic
     */

    void mainLoop()
    {
        new Game();

        spdlog::info("Game initialized");

        int fpsCounter = 0;
        fps = 0;
        std::chrono::milliseconds lastFpsUpdate =
            std::chrono::duration_cast<std::chrono::milliseconds>(
                std::chrono::system_clock::now().time_since_epoch()
            );

        while (!window.shouldClose())
        {
            glfwPollEvents();

            //if (imguirebuildswapchain)
            //{
                //imguirebuildswapchain = false;
                //ImGui_ImplVulkan_SetMinImageCount(g_MinImageCount);
                //ImGui_ImplVulkanH_CreateWindow(g_Instance, g_PhysicalDevice, g_Device, &g_MainWindowData,
                        //g_QueueFamily, g_Allocator, g_SwapChainResizeWidth, g_SwapChainResizeHeight, g_MinImageCount);
                //imguirebuildswapchain.FrameIndex = 0;
            //}

            if (!imguiwindowcreated)
            {
                imGuiSetupWindow();
                imguiwindowcreated = true;
            }

            createCommandBuffers();

            drawFrame();

            fpsCounter++;
            std::chrono::milliseconds newFpsUpdate = std::chrono::duration_cast<std::chrono::milliseconds>(
                    std::chrono::system_clock::now().time_since_epoch()
                    );
            if (newFpsUpdate - lastFpsUpdate >= std::chrono::milliseconds(1000))
            {
                fps = fpsCounter;
                spdlog::info("Fps: {}", fps);
                lastFpsUpdate = newFpsUpdate;
                fpsCounter = 0;

                //                    fps *= 10;
            }
        }

        spdlog::info("Shutting down");

        // Wait until all command buffers are cleared in order to safely destroy
        vkDeviceWaitIdle(device.getVkDevice());
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
        {
            // Todo: Move descriptorpool out of imgui setup
            VkDescriptorPoolSize pool_sizes[] =
            {
                {VK_DESCRIPTOR_TYPE_SAMPLER,                1000},
                {VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 1000},
                {VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE,          1000},
                {VK_DESCRIPTOR_TYPE_STORAGE_IMAGE,          1000},
                {VK_DESCRIPTOR_TYPE_UNIFORM_TEXEL_BUFFER,   1000},
                {VK_DESCRIPTOR_TYPE_STORAGE_TEXEL_BUFFER,   1000},
                {VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,         1000},
                {VK_DESCRIPTOR_TYPE_STORAGE_BUFFER,         1000},
                {VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC, 1000},
                {VK_DESCRIPTOR_TYPE_STORAGE_BUFFER_DYNAMIC, 1000},
                {VK_DESCRIPTOR_TYPE_INPUT_ATTACHMENT,       1000}
            };

            VkDescriptorPoolCreateInfo pool_info = {};
            pool_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
            pool_info.flags = VK_DESCRIPTOR_POOL_CREATE_FREE_DESCRIPTOR_SET_BIT;
            pool_info.maxSets = 1000 * IM_ARRAYSIZE(pool_sizes);
            pool_info.poolSizeCount = (uint32_t) IM_ARRAYSIZE(pool_sizes);
            pool_info.pPoolSizes = pool_sizes;
            vkCreateDescriptorPool(device.getVkDevice(), &pool_info, nullptr, &descriptorPool);
        }

        IMGUI_CHECKVERSION();
        ImGui::CreateContext();
        ImGuiIO &io = ImGui::GetIO();
        (void) io;
        //            io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
        //            io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;
        //io.Fonts->AddFontFromFileTTF("../../Assets/Fonts/Roboto-Medium.ttf", 16.0f);

        // When viewports are enabled we tweak WindowRounding/WindowBg so platform windows can look identical to regular ones.
        ImGuiStyle &style = ImGui::GetStyle();
        //            if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
        //            {
        //                style.WindowRounding = 0.0f;
        //                style.Colors[ImGuiCol_WindowBg].w = 1.0f;
        //            }

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
        init_info.ImageCount = static_cast<uint32_t>(swapChain.getImageCount());
        init_info.CheckVkResultFn = NULL;
        ImGui_ImplVulkan_Init(&init_info, swapChain.getRenderPass());

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
    void imGuiSetupWindow()
    {
        ImGuiIO &io = ImGui::GetIO();
        // Start the Dear ImGui frame
        ImGui_ImplVulkan_NewFrame();
        ImGui_ImplGlfw_NewFrame();

        auto WindowSize = ImVec2((float) window.getWidth(), (float) window.getHeight());
        ImGui::SetNextWindowSize(WindowSize, ImGuiCond_::ImGuiCond_FirstUseEver);
        ImGui::SetNextWindowPos(ImVec2(0, 0), ImGuiCond_::ImGuiCond_FirstUseEver);
        ImGui::NewFrame();

        // render your GUI
        ImGui::Begin("Stats");
        //            ImGui::SliderFloat("red", &gui_red, 0, 1, "%.2f");
        ImGui::Text("%d", fps);
        //            ImGui::Text(std::to_string(10 * 1000.0).c_str());
        //            ImGui::Text(std::to_string(10 * 1000.0).c_str());
        //            bool inputImage = ImGui::InputText("Path to Image", &imageName);
        //            bool logoImage = ImGui::InputText("Path to Logo", &logoImageName);
        //            if (ImGui::Button("Reload")) {
        //                changeImage = true;
        //            }
        //            ImGui::Checkbox("Show OpenCV" );
        //            ImGui::Checkbox("Flip Image", &flip);
        //            ImGui::SliderFloat("Size", &sizeMultiplier, 0.0, 10.0, "%.3f", 1.0f);
        //            ImGui::SliderFloat("Resize Window", &resize, 1.0, 10.0, "%.3f", 1.0f);
        //            ImGui::SliderFloat("XPos", &xTrans, -1.0, 1.0, "%.3f", 1.0f);
        //            ImGui::SliderFloat("YPos", &yTrans, -1.0, 1.0, "%.3f", 1.0f);
        //            ImGui::SliderFloat("Alpha", &alpha, 0.0, 1.0, "%.3f", 1.0f);
        //            ImGui::SliderFloat("Transparency", &transparency, 0.0, 1.0, "%.3f", 1.0f);
        //            bool outputImage = ImGui::InputText("Save As (No file type at the end, only the name)", &outputImageName);
        //            ImGui::ListBox("File format\n(single select)", &fileFormat, listbox_items, 5, 4);
        //            tempOutImageName = outputImageName + listbox_items[fileFormat];
        //            if (ImGui::Button("Save")) {
        //                writeImage = true;
        //            }

        ImGui::End();
        // Render dear imgui UI box into our window
        ImGui::Render();

        // Update and Render additional Platform Windows
        //            if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
        //            {
        GLFWwindow *backup_current_context = glfwGetCurrentContext();
        //                ImGui::UpdatePlatformWindows();
        //                ImGui::RenderPlatformWindowsDefault();
        glfwMakeContextCurrent(backup_current_context);
        //            }
    }

    /**
     * Create a pipeline layout in order to pass values to our shaders
     */
    void createPipelineLayout()
    {
        spdlog::get("vulkan")->debug("Creating pipeline layout..");

        VkPipelineLayoutCreateInfo pipelineLayoutInfo{};
        pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
        pipelineLayoutInfo.setLayoutCount = 0;
        pipelineLayoutInfo.pSetLayouts = nullptr;
        pipelineLayoutInfo.pushConstantRangeCount = 0;
        pipelineLayoutInfo.pPushConstantRanges = nullptr;
        if (vkCreatePipelineLayout(device.getVkDevice(), &pipelineLayoutInfo, nullptr, &pipelineLayout) !=
                VK_SUCCESS)
        {
            throw std::runtime_error("failed to create pipeline layout!");
        }

    }

    /**
     * Create the actual pipeline
     */
    void createPipeline()
    {
        //            pipeline->destroy();
        spdlog::get("vulkan")->debug("Creating pipeline..");

        auto pipelineConfig = VksPipeline::defaultPipelineConfigInfo(swapChain.getSwapChainExtent().width,
                swapChain.getSwapChainExtent().height);
        pipelineConfig.renderPass = swapChain.getRenderPass();
        pipelineConfig.pipelineLayout = pipelineLayout;
        pipeline = std::make_unique<VksPipeline>(device, swapChain, pipelineConfig);
    }

    /**
     * Create the command buffers used to render our scene
     */
    void createCommandBuffers()
    {
        // Before recreating the command buffers, wait until they are no longer in use
        // Might be a reason for slowdowns in the future
        swapChain.waitForImageInFlight();
        device.waitIdle();
        swapChain.getCurrentFrame();

        // Allocate new commandbuffers
        commandBuffers.resize(swapChain.getImageCount());
        VkCommandBufferAllocateInfo allocInfo{};
        allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
        allocInfo.commandPool = device.getCommandPool();
        allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
        allocInfo.commandBufferCount = static_cast<uint32_t>(commandBuffers.size());

        err = vkAllocateCommandBuffers(device.getVkDevice(), &allocInfo, commandBuffers.data());
        check_vk_result(err);

        for (size_t i = 0; i < commandBuffers.size(); i++)
        {
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
            renderPassInfo.renderPass = swapChain.getRenderPass();
            renderPassInfo.framebuffer = swapChain.getFrameBuffer(i);

            renderPassInfo.renderArea.offset = {0, 0};
            renderPassInfo.renderArea.extent = swapChain.getSwapChainExtent();

            std::array<VkClearValue, 2> clearValues{};
            clearValues[0].color = {0.0f, 0.0f, 0.0f, 1.0f};
            clearValues[1].depthStencil = {1.0f, 0};
            renderPassInfo.clearValueCount = static_cast<uint32_t>(clearValues.size());
            renderPassInfo.pClearValues = clearValues.data();

            vkCmdBeginRenderPass(commandBuffers[i], &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);

            pipeline->bind(commandBuffers[i]);
            vksModel->bind(commandBuffers[i]);
            vksModel->draw(commandBuffers[i]);

            // Render imgui data
            if (imguiwindowcreated)
            {
                ImGui_ImplVulkan_RenderDrawData(ImGui::GetDrawData(), commandBuffers[i]);
            }

            vkCmdEndRenderPass(commandBuffers[i]);
            if (vkEndCommandBuffer(commandBuffers[i]) != VK_SUCCESS)
            {
                throw std::runtime_error("failed to record command buffer!");
            }
        }
        imguiwindowcreated = false;
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
            swapChain.recreate();
            createPipeline();
            return;
        }

        // Load the next image in the swapchain
        uint32_t imageIndex;
        auto result = swapChain.acquireNextImage(&imageIndex);
        if (result == VK_ERROR_OUT_OF_DATE_KHR)
        {
            // When the swapchain is out of date, recreate a new one
            spdlog::get("vulkan")->warn("Swapchain image is out of date");
            swapChain.recreate();
            createPipeline();
            return;
        } else if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR)
        {
            spdlog::get("vulkan")->warn("Swapchain image is not optimal");
        }

        // Submit our command buffer to be drawed on the new image
        result = swapChain.submitCommandBuffers(&commandBuffers[imageIndex], &imageIndex);
        if (result == VK_ERROR_OUT_OF_DATE_KHR)// || result == VK_SUBOPTIMAL_KHR
        {
            // When the swapchain is out of date, recreate a new one
            spdlog::get("vulkan")->warn("Swapchain is out of date");
            swapChain.recreate();
            createPipeline();

        } else if (result != VK_SUCCESS)
        {
            throw std::runtime_error("failed to present swap chain image!");
        }
    }

};

}

#endif //VULKANENGINE_APP_H
