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
#include "../vks/vks_window.h"
#include "../vks/vks_device.h"
#include "../vks/vks_swap_chain.h"
#include "../vks/vks_pipeline.h"
#include <memory>
#include "../imgui/imgui.h"
#include "../imgui/imgui_impl_glfw.h"
#include "../imgui/imgui_impl_vulkan.h"
#include "../vks/vks_model.h"
#include <spdlog/spdlog.h>

#define GLM_FORCE_DEFAULT_ALIGNED_GENTYPES

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <chrono>
#include <libwebsockets.h>
#include <core/graphics/render_pipeline.h>
#include <core/graphics/compute_pipeline.h>
#include "graphics/renderable.h"
#include "game/world.h"
#include "camera.h"

const bool VALIDATION_LAYERS_ENABLED = true;
const bool VSYNC = true;

using namespace vks;

class Game
{
 public:

	Game(VksWindow& window)
		: window(window)
	{

	};

	void init()
	{
		recreateSwapChain();
		createDescriptorPool();
//		computepipeline = new ComputePipeline(device, *swapChain, descriptorPool);
		basepipeline = new BaseRenderPipeline(device, *swapChain, descriptorPool);
		createCommandBuffers();

		generateQuad();

		inputhandler.init(&window);
		camera.setInputHandler(&inputhandler);
		inputhandler.swallowMouse();

		spdlog::get("vulkan")->info("Instantiated application");
	}

	// Render game and handle input
	void update(float delta)
	{
		input(delta);

		if (!imguiDataAvailable)
		{
//			imGuiCreateRenderData();
//			imguiDataAvailable = true;
		}

		recordCommandBuffers();
		drawFrame();

		inputhandler.syncInputState();
	}

	void destroy()
	{
		// Wait until any operation towards the gpu is completed
		device.waitIdle();
	}

	~Game()
	{
//		ImGui_ImplVulkan_Shutdown();
//		ImGui_ImplGlfw_Shutdown();
//		ImGui::DestroyContext();
	}

 private:

	VksWindow& window;

	// Vulkan properties
	VksDevice device = VksDevice(window, VALIDATION_LAYERS_ENABLED);
	std::unique_ptr<VksSwapChain> swapChain;
	std::vector<VkCommandBuffer> commandBuffers;
	VkDescriptorPool descriptorPool;
	VksInput inputhandler = VksInput();
	Camera camera;

	ComputePipeline* computepipeline = nullptr;
	BaseRenderPipeline* basepipeline = nullptr;

	bool imguiDataAvailable = false;
	IRenderable* world = new World(device);
	VkResult err;

	VksModel* model;

	// ImGui
	float gui_x = 0;
	float gui_y = 0;
	float gui_z = 0;

	void input(float delta)
	{

		if (inputhandler.isKeyDown(GLFW_KEY_ESCAPE))
		{
			inputhandler.showMouse();
		}

		if (inputhandler.isButtonClicked(GLFW_MOUSE_BUTTON_1))
		{

			ImGuiIO& io = ImGui::GetIO();
			if (!io.WantCaptureMouse)
			{
				inputhandler.swallowMouse();
			}
		}

//        entity.position = glm::vec3(gui_x, gui_y, gui_z);

		if (!inputhandler.isMouseSwallowed()) return;

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
		ImGuiIO& io = ImGui::GetIO();
		(void)io;
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
		init_info.QueueFamily = device.findQueueFamilies().graphicsFamily.value();
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

		VkCommandBuffer commandBuffer = device.beginSingleTimeCommands();
		ImGui_ImplVulkan_CreateFontsTexture(commandBuffer);
		device.endSingleTimeCommands(commandBuffer);
		ImGui_ImplVulkan_DestroyFontUploadObjects();
	}

	/**
	 * Draw an ImGUI GUI window in the commandbuffer
	 */
	void imGuiCreateRenderData()
	{
		ImGuiIO& io = ImGui::GetIO();
		ImGui_ImplVulkan_NewFrame();
		ImGui_ImplGlfw_NewFrame();

		auto WindowSize = ImVec2((float)window.getWidth(), (float)window.getHeight());
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

	void createDescriptorPool()
	{
		VkDescriptorPoolSize pool_sizes[] =
			{
				{ VK_DESCRIPTOR_TYPE_SAMPLER, 5 },
				{ VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 5 },
				{ VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE, 5 },
				{ VK_DESCRIPTOR_TYPE_STORAGE_IMAGE, 5 },
				{ VK_DESCRIPTOR_TYPE_UNIFORM_TEXEL_BUFFER, 5 },
				{ VK_DESCRIPTOR_TYPE_STORAGE_TEXEL_BUFFER, 5 },
				{ VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 5 },
				{ VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, 5 },
				{ VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC, 5 },
				{ VK_DESCRIPTOR_TYPE_STORAGE_BUFFER_DYNAMIC, 5 },
				{ VK_DESCRIPTOR_TYPE_INPUT_ATTACHMENT, 5 }
			};

		VkDescriptorPoolCreateInfo pool_info = {};
		pool_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
		pool_info.flags = VK_DESCRIPTOR_POOL_CREATE_FREE_DESCRIPTOR_SET_BIT;
		pool_info.maxSets = swapChain->getImageCount() * IM_ARRAYSIZE(pool_sizes);
		pool_info.poolSizeCount = (uint32_t)IM_ARRAYSIZE(pool_sizes);
		pool_info.pPoolSizes = pool_sizes;
		vkCreateDescriptorPool(device.getVkDevice(), &pool_info, nullptr, &descriptorPool);
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


		// Swapchain renderpasses
		for (size_t i = 0; i < commandBuffers.size(); i++)
		{
			err = vkResetCommandBuffer(commandBuffers[i], VK_COMMAND_BUFFER_RESET_RELEASE_RESOURCES_BIT);
			check_vk_result(err);

			VkCommandBufferBeginInfo beginInfo = {};
			beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
			beginInfo.flags = 0;                   // Optional
			beginInfo.pInheritanceInfo = nullptr;  // Optional

			if (vkBeginCommandBuffer(commandBuffers[i], &beginInfo) != VK_SUCCESS)
			{
				throw std::runtime_error("failed to begin recording command buffer!");
			}

			// Compute
//			computepipeline->begin(commandBuffers[i], i);
//			computepipeline->end();
//
//			// Wait until the compute shader is finished rendering to it's texture
//			VkImageMemoryBarrier computeBarrier{};
//			computeBarrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
//			computeBarrier.srcAccessMask = VK_ACCESS_SHADER_WRITE_BIT;
//			computeBarrier.dstAccessMask = VK_ACCESS_MEMORY_READ_BIT;
//			computeBarrier.oldLayout = VK_IMAGE_LAYOUT_GENERAL;
//			computeBarrier.newLayout = VK_IMAGE_LAYOUT_GENERAL;
//			computeBarrier.image = computepipeline->getComputeTarget();
//			computeBarrier.subresourceRange = { VK_IMAGE_ASPECT_COLOR_BIT, 0, 1, 0, 1 };
//			computeBarrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
//			computeBarrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
//
//			vkCmdPipelineBarrier(
//				commandBuffers[i],
//				VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT,
//				VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT,
//				0,
//				0, NULL,
//				0, NULL,
//				1, &computeBarrier);

			// Renderpass
			VkRenderPassBeginInfo renderPassInfo = {};
			renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
			renderPassInfo.renderPass = swapChain->getRenderPass();
			renderPassInfo.framebuffer = swapChain->getFrameBuffer(i);

			renderPassInfo.renderArea.offset = { 0, 0 };
			renderPassInfo.renderArea.extent = swapChain->getSwapChainExtent();

			std::array<VkClearValue, 2> clearValues{};
			clearValues[0].color = { 0.0f, 0.0f, 0.0f, 1.0f };
			clearValues[1].depthStencil = { 1.0f, 0 };
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
			VkRect2D scissor{{ 0, 0 }, swapChain->getSwapChainExtent() };
			vkCmdSetViewport(commandBuffers[i], 0, 1, &viewport);
			vkCmdSetScissor(commandBuffers[i], 0, 1, &scissor);

			// Render screen quad
			basepipeline->begin(commandBuffers[i], i);
			basepipeline->updateBuffers(camera);
			world->draw(*basepipeline);
//			basepipeline->bindModelTransform(glm::mat4(1));
//
//			basepipeline->bindModel(*model);
//			basepipeline->drawModel();

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
		}
		else if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR)
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

		}
		else if (result != VK_SUCCESS)
		{
			throw std::runtime_error("failed to present swap chain image!");
		}
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
		}
		else
		{
			swapChain = std::make_unique<VksSwapChain>(device, extent, VSYNC, std::move(swapChain));
			if (swapChain->getImageCount() != commandBuffers.size())
			{
				freeCommandBuffers();
				createCommandBuffers();
			}
		}
	}

	void generateQuad()
	{
		// Setup vertices for a single uv-mapped quad made from two triangles
		std::vector<vks::VksModel::Vertex> vertices =
			{
				{{ 1.0f, 1.0f, 0.0f }, { 1.0f, 1.0f }},
				{{ -1.0f, 1.0f, 0.0f }, { 0.0f, 1.0f }},
				{{ -1.0f, -1.0f, 0.0f }, { 0.0f, 0.0f }},
				{{ 1.0f, 1.0f, 0.0f }, { 1.0f, 0.0f }},
				{{ 1.0f, -1.0f, 0.0f }, { 1.0f, 0.0f }},
				{{ 1.0f, -1.0f, 0.0f }, { 1.0f, 0.0f }}
			};

		model = new vks::VksModel(device, vertices);
	}

};

#endif //VULKANENGINE_GAME_H
