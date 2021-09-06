//
// Created by magnias on 8/22/21.
//
#include "../../imgui/imgui.h"
#include "../../imgui/imgui_impl_glfw.h"
#include "../../imgui/imgui_impl_vulkan.h"

#ifndef VULKANENGINE_GUI_H
#define VULKANENGINE_GUI_H

class Gui
{
public:

	struct gui_output
	{
		float deltatime;
		float fps;
		glm::vec3 camdir;
		glm::vec4 campos;
	};

	struct gui_input
	{
		glm::vec4 lookat;
		int pass_cutoff;
		int depth;
	};

	Gui(vks::VksWindow* window)
	:_window(window)
	{
		data = new gui_input{};
	}

	void destroy()
	{
		ImGui_ImplVulkan_Shutdown();
		ImGui_ImplGlfw_Shutdown();

		if (imguiInitialized)
		{
			ImGui::DestroyContext();
		}
	}

	/**
	 * Setup the ImGui GUI bindings
	 */
	void initImGui(vks::VksDevice& device, vks::VksSwapChain& swapChain, VkDescriptorPool descriptorPool)
	{

		IMGUI_CHECKVERSION();
		ImGui::CreateContext();
		ImGuiIO& io = ImGui::GetIO();
		(void)io;
		//            io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
		//            io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;
		//io.Fonts->AddFontFromFileTTF("../../Assets/Fonts/Roboto-Medium.ttf", 16.0f);

		io.DisplaySize = ImVec2(_window->getWidth(), _window->getHeight());
		io.DisplayFramebufferScale = ImVec2(1.0f, 1.0f);

		// Setup Platform/Renderer bindings
		ImGui_ImplGlfw_InitForVulkan(_window->getWindow(), true);
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
		init_info.ImageCount = static_cast<uint32_t>(swapChain.getImageCount());
		init_info.CheckVkResultFn = NULL;
		ImGui_ImplVulkan_Init(&init_info, swapChain.getRenderPass());

		// Setup Dear ImGui style
		ImGui::StyleColorsDark();

		VkCommandBuffer commandBuffer = device.beginSingleTimeCommands();
		ImGui_ImplVulkan_CreateFontsTexture(commandBuffer);
		device.endSingleTimeCommands(commandBuffer);
		ImGui_ImplVulkan_DestroyFontUploadObjects();

		imguiInitialized = true;
	}

	/**
	 * Draw an ImGUI GUI window in the commandbuffer
	 */
	void createRenderData(gui_output output_data)
	{
		ImGuiIO &io = ImGui::GetIO();
		ImGui_ImplVulkan_NewFrame();
		ImGui_ImplGlfw_NewFrame();

		auto WindowSize = ImVec2((float)_window->getWidth(), (float)_window->getHeight());
		ImGui::SetNextWindowSize(WindowSize, ImGuiCond_::ImGuiCond_FirstUseEver);
		ImGui::SetNextWindowPos(ImVec2(0, 0), ImGuiCond_::ImGuiCond_FirstUseEver);

		ImGui::NewFrame();

		ImGui::Begin("stats");
		{
			ImGui::Text("average %.3f ms/frame (%.0f FPS)", output_data.deltatime, output_data.fps);
			ImGui::Text("pos: %.2f %.2f %.2f", output_data.campos.x,
						output_data.campos.y, output_data.campos.z);
//			glm::vec3 right = camera.getRight();
//			ImGui::Text("right: %f %f %f", right.x, right.y, right.z);
//			glm::vec3 up = camera.getUp();
//			ImGui::Text("up: %f %f %f", up.x, up.y, up.z);
		}
		ImGui::End();

		ImGui::Begin("Shader settings");
		{
			ImGui::SliderFloat4("lookat", glm::value_ptr(data->lookat), -1.0f, 1.0f, "%.7f");
			ImGui::SliderInt("pass cutoff", &data->pass_cutoff, 1, 100);
			ImGui::SliderInt("depth", &data->depth, 1, 20);
		}
		ImGui::End();

		// Store the created window in memory until it can be rendered
		ImGui::Render();
	}

	void render(VkCommandBuffer& commandBuffer)
	{
		ImGui_ImplVulkan_RenderDrawData(ImGui::GetDrawData(), commandBuffer);
	}

	gui_input* getData()
	{
		return data;
	}

private:
	vks::VksWindow* _window;

	bool imguiInitialized = false;
	gui_input* data;
};

#endif //VULKANENGINE_GUI_H
