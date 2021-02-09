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

const uint32_t WIDTH = 800;
const uint32_t HEIGHT = 600;

namespace vks
{
    class App
    {
    public:
        void run()
        {
            createPipelineLayout();
            createPipeline();
//            setupImgui();
            createCommandBuffers();
            initImGui();
            mainLoop();
        }

        ~App()
        {
//            vkDestroyCommandPool(device.getVkDevice(), command)
            vkDestroyPipelineLayout(device.getVkDevice(), pipelineLayout, nullptr);
        }

    private:
        VksWindow window = VksWindow(WIDTH, HEIGHT, "test");
        VksDevice device = VksDevice(window);
        VksSwapChain swapChain = VksSwapChain(window, device);
        std::unique_ptr<VksPipeline> pipeline;
        VkPipelineLayout pipelineLayout;
        std::vector<VkCommandBuffer> commandBuffers;
        VkCommandPool imGuiCommandPool;
        std::vector<VkCommandBuffer> imGuiCommandBuffers;
        std::vector<VkFramebuffer> imGuiFramebuffers;
        VkDescriptorPool descriptorPool;
        VkRenderPass imGuiRenderPass;
        bool imguiwindowcreated = false;

        void mainLoop()
        {
            new Game();

            while (!window.shouldClose())
            {
                glfwPollEvents();

//                if (g_SwapChainRebuild)
//                {
//                    g_SwapChainRebuild = false;
//                    ImGui_ImplVulkan_SetMinImageCount(g_MinImageCount);
//                    ImGui_ImplVulkanH_CreateWindow(g_Instance, g_PhysicalDevice, g_Device, &g_MainWindowData,
//                                                   g_QueueFamily, g_Allocator, g_SwapChainResizeWidth, g_SwapChainResizeHeight, g_MinImageCount);
//                    g_MainWindowData.FrameIndex = 0;
//                }

                if (!imguiwindowcreated)
                {
                    imGuiSetupWindow();
                    imguiwindowcreated = true;
                }
                createCommandBuffers();

                drawFrame();
            }

            // Wait until all command buffers are cleared or other operations
            vkDeviceWaitIdle(device.getVkDevice());
        }

        static void check_vk_result(VkResult err)
        {
            if (err == 0)
                return;
            fprintf(stderr, "[vulkan] Error: VkResult = %d\n", err);
            if (err < 0)
                abort();
        }

        void initImGui()
        {
            {
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

            VkCommandBuffer commandBuffer = beginSingleTimeCommands();
            ImGui_ImplVulkan_CreateFontsTexture(commandBuffer);
            endSingleTimeCommands(commandBuffer);
            ImGui_ImplVulkan_DestroyFontUploadObjects();
        }

        void imGuiSetupWindow()
        {
            ImGuiIO &io = ImGui::GetIO();
            // Start the Dear ImGui frame
            ImGui_ImplVulkan_NewFrame();
            ImGui_ImplGlfw_NewFrame();

            auto WindowSize = ImVec2((float) window.getWidth(), (float) window.getHeight()); // TODO use swapchainextent
            ImGui::SetNextWindowSize(WindowSize, ImGuiCond_::ImGuiCond_FirstUseEver);
            ImGui::SetNextWindowPos(ImVec2(0, 0), ImGuiCond_::ImGuiCond_FirstUseEver);
            ImGui::NewFrame();

            // render your GUI
            ImGui::Begin("Thr34d5");
            ImGui::Text(std::to_string(10 * 1000.0).c_str());
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
            ImGui::Text("fdsaf");
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

        void createPipelineLayout()
        {
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

        void createPipeline()
        {
            auto pipelineConfig = VksPipeline::defaultPipelineConfigInfo(WIDTH, HEIGHT);
            pipelineConfig.renderPass = swapChain.getRenderPass();
            pipelineConfig.pipelineLayout = pipelineLayout;
            pipeline = std::make_unique<VksPipeline>(device, swapChain, pipelineConfig);
        }

        void createCommandBuffers()
        {
            commandBuffers.resize(swapChain.getImageCount());
            VkCommandBufferAllocateInfo allocInfo{};
            allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
            allocInfo.commandPool = device.getCommandPool();
            allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
            allocInfo.commandBufferCount = static_cast<uint32_t>(commandBuffers.size());

            if (vkAllocateCommandBuffers(device.getVkDevice(), &allocInfo, commandBuffers.data()) !=
                VK_SUCCESS)
            {
                throw std::runtime_error("failed to allocate command buffers!");
            }

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

//                lveModel.bind(commandBuffers[i]);
//                lveModel.draw(commandBuffers[i]);
                vkCmdDraw(commandBuffers[i], 3, 1, 0, 0);

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

        void drawFrame()
        {
            uint32_t imageIndex;
            auto result = swapChain.acquireNextImage(&imageIndex);
            if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR)
            {
                throw std::runtime_error("failed to acquire swap chain image!");
            }

            result = swapChain.submitCommandBuffers(&commandBuffers[imageIndex], &imageIndex);
            if (result != VK_SUCCESS)
            {
                throw std::runtime_error("failed to present swap chain image!");
            }
        }

        void createCommandPool(VkCommandPool *commandPool, VkCommandPoolCreateFlags flags)
        {
            VkCommandPoolCreateInfo commandPoolCreateInfo = {};
            commandPoolCreateInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
            commandPoolCreateInfo.queueFamilyIndex = device.findPhysicalQueueFamilies().graphicsFamily.value();
            commandPoolCreateInfo.flags = flags;

            if (vkCreateCommandPool(device.getVkDevice(), &commandPoolCreateInfo, nullptr, commandPool) != VK_SUCCESS)
            {
                throw std::runtime_error("Could not create graphics command pool");
            }
        }

        void
        createCommandBuffers(VkCommandBuffer *commandBuffer, uint32_t commandBufferCount, VkCommandPool &commandPool)
        {
            VkCommandBufferAllocateInfo commandBufferAllocateInfo = {};
            commandBufferAllocateInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
            commandBufferAllocateInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
            commandBufferAllocateInfo.commandPool = commandPool;
            commandBufferAllocateInfo.commandBufferCount = commandBufferCount;
            vkAllocateCommandBuffers(device.getVkDevice(), &commandBufferAllocateInfo, commandBuffer);
        }

        VkCommandBuffer beginSingleTimeCommands()
        {
            VkCommandBufferAllocateInfo allocInfo = {};
            allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
            allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
            allocInfo.commandPool = device.getCommandPool();
            allocInfo.commandBufferCount = 1;

            VkCommandBuffer commandBuffer;
            vkAllocateCommandBuffers(device.getVkDevice(), &allocInfo, &commandBuffer);

            VkCommandBufferBeginInfo beginInfo = {};
            beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
            beginInfo.flags |= VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

            vkBeginCommandBuffer(commandBuffer, &beginInfo);

            return commandBuffer;
        }

        void endSingleTimeCommands(VkCommandBuffer commandBuffer)
        {
            vkEndCommandBuffer(commandBuffer);

            VkSubmitInfo submitInfo = {};
            submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
            submitInfo.commandBufferCount = 1;
            submitInfo.pCommandBuffers = &commandBuffer;

            vkQueueSubmit(device.getGraphicsQueue(), 1, &submitInfo, VK_NULL_HANDLE);
            vkQueueWaitIdle(device.getGraphicsQueue());

            vkFreeCommandBuffers(device.getVkDevice(), device.getCommandPool(), 1, &commandBuffer);
        }

    };

    VkInstance getInstance();


}