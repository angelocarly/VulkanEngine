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
            createCommandBuffers();
            setupImgui();
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
        std::vector<VkImageView> imGuiImageViews;
        VkDescriptorPool descriptorPool;
        VkRenderPass imGuiRenderPass;

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

                ImGui_ImplVulkan_NewFrame();
                ImGui_ImplGlfw_NewFrame();
                ImGui::NewFrame();
                ImGui::ShowDemoWindow();
                ImGui::Render();
//                memcpy(&wd->ClearValue.color.float32[0], &clear_color, 4 * sizeof(float));
//                FrameRender();
//                FramePresent(wd);

                renderImGuiFrame();

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

        void setupImgui()
        {
            IMGUI_CHECKVERSION();
            ImGui::CreateContext();
            ImGuiIO &io = ImGui::GetIO();
            (void) io;

            ImGui::StyleColorsDark();

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
            {
                VkAttachmentDescription attachment = {};
                attachment.format = swapChain.getImagesFormat();
                attachment.samples = VK_SAMPLE_COUNT_1_BIT;
                attachment.loadOp = VK_ATTACHMENT_LOAD_OP_LOAD;
                attachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
                attachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
                attachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
                attachment.initialLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
                attachment.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

                VkAttachmentReference color_attachment = {};
                color_attachment.attachment = 0;
                color_attachment.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

                VkSubpassDescription subpass = {};
                subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
                subpass.colorAttachmentCount = 1;
                subpass.pColorAttachments = &color_attachment;

                VkSubpassDependency dependency = {};
                dependency.srcSubpass = VK_SUBPASS_EXTERNAL;
                dependency.dstSubpass = 0;
                dependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
                dependency.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
                dependency.srcAccessMask = 0;  // or VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
                dependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;

                VkRenderPassCreateInfo info = {};
                info.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
                info.attachmentCount = 1;
                info.pAttachments = &attachment;
                info.subpassCount = 1;
                info.pSubpasses = &subpass;
                info.dependencyCount = 1;
                info.pDependencies = &dependency;
                if (vkCreateRenderPass(device.getVkDevice(), &info, nullptr, &imGuiRenderPass) != VK_SUCCESS)
                {
                    throw std::runtime_error("Could not create Dear ImGui's render pass");
                }

            }
            {
                imGuiFramebuffers.resize(swapChain.getImageCount());
                VkImageView attachment[1];
                VkFramebufferCreateInfo info = {};
                info.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
                info.renderPass = imGuiRenderPass;
                info.attachmentCount = 1;
                info.pAttachments = attachment;
                info.width = window.getWidth();
                info.height = window.getHeight();
                info.layers = 1;
                for (uint32_t i = 0; i < swapChain.getImageCount(); i++)
                {
//                    ImGui_ImplVulkanH_Frame *fd = wd->Frames[i];
//                    attachment[0] = fd->BackbufferView;
                    vkCreateFramebuffer(device.getVkDevice(), &info, nullptr, &imGuiFramebuffers.at(i));
//                    check_vk_result(err);
                }
            }

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
            init_info.Allocator = nullptr;
            init_info.MinImageCount = swapChain.getImageCount();
            init_info.ImageCount = swapChain.getImageCount();
            init_info.CheckVkResultFn = check_vk_result;
            ImGui_ImplVulkan_Init(&init_info, imGuiRenderPass);

            createCommandPool(&imGuiCommandPool, VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT);
            imGuiCommandBuffers.resize(swapChain.getImageCount());
            createCommandBuffers(imGuiCommandBuffers.data(), static_cast<uint32_t>(imGuiCommandBuffers.size()),
                                 imGuiCommandPool);

            ImGui_ImplVulkan_CreateFontsTexture(imGuiCommandBuffers.at(swapChain.getCurrentFrame()));

//            VkCommandBuffer command_buffer = beginSingleTimeCommands();
            ImGui_ImplVulkan_CreateFontsTexture(imGuiCommandBuffers.at(swapChain.getCurrentFrame()));
//            endSingleTimeCommands(command_buffer);

        }

        void renderImGuiFrame()
        {
            {
                VkRenderPassBeginInfo info = {};
                info.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
                info.renderPass = imGuiRenderPass;
                info.framebuffer = imGuiFramebuffers.at(swapChain.getCurrentFrame());
                info.renderArea.extent.width = window.getWidth();
                info.renderArea.extent.height = window.getHeight();
                info.clearValueCount = 1;
                info.pClearValues = nullptr;
                vkCmdBeginRenderPass(commandBuffers.at(swapChain.getCurrentFrame()), &info, VK_SUBPASS_CONTENTS_INLINE);
            }

            ImGui_ImplVulkan_RenderDrawData(ImGui::GetDrawData(), imGuiCommandBuffers.at(swapChain.getCurrentFrame()));
            vkCmdEndRenderPass(imGuiCommandBuffers.at(swapChain.getCurrentFrame()));
            vkEndCommandBuffer(imGuiCommandBuffers.at(swapChain.getCurrentFrame()));

            std::array<VkCommandBuffer, 2> submitCommandBuffers =
                    { commandBuffers[swapChain.getCurrentFrame()], imGuiCommandBuffers[swapChain.getCurrentFrame()] };
            VkSubmitInfo submitInfo = {};
            submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
            // ...
            submitInfo.commandBufferCount = static_cast<uint32_t>(submitCommandBuffers.size());
            submitInfo.pCommandBuffers = submitCommandBuffers.data();
            if (vkQueueSubmit(device.getGraphicsQueue(), 1, &submitInfo, swapChain.getInFlightFences()[swapChain.getCurrentFrame()]) != VK_SUCCESS) {
                throw std::runtime_error("Could not submit commands to the graphics queue");
            }
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
                clearValues[0].color = {0.1f, 0.1f, 0.1f, 1.0f};
                clearValues[1].depthStencil = {1.0f, 0};
                renderPassInfo.clearValueCount = static_cast<uint32_t>(clearValues.size());
                renderPassInfo.pClearValues = clearValues.data();

                vkCmdBeginRenderPass(commandBuffers[i], &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);

                pipeline->bind(commandBuffers[i]);

//                lveModel.bind(commandBuffers[i]);
//                lveModel.draw(commandBuffers[i]);
                vkCmdDraw(commandBuffers[i], 3, 1, 0, 0);

                vkCmdEndRenderPass(commandBuffers[i]);
                if (vkEndCommandBuffer(commandBuffers[i]) != VK_SUCCESS)
                {
                    throw std::runtime_error("failed to record command buffer!");
                }
            }
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

    };

    VkInstance getInstance();


}