//
// Created by magnias on 04/04/2021.
//

#ifndef _COMPUTE_PIPELINE_H_
#define _COMPUTE_PIPELINE_H_

#include <vks/vks_util.h>
#include "render_pipeline.h"
#include <lodepng.h>

struct Pixel
{
	glm::vec4 color;
};

class ComputePipeline : public IRenderPipeline, public IRenderProvider
{
 public:
	ComputePipeline(VksDevice& device, VksSwapChain& swapChain, VkDescriptorPool& descriptorPool)
		: _device(device), _descriptorPool(descriptorPool), _swapChain(swapChain)
	{
		init();
	}

	void init()
	{
		_bufferSize = sizeof(Pixel) * WIDTH * HEIGHT;
		createDescriptorSetLayout();
		createPipelineLayout();
		createPipeline();
		createBuffer();
		createUniformBuffers();
		createDescriptorSets();

	}

	void begin(VkCommandBuffer& commandBuffer, int frame) override
	{
		_currentCommandBuffer = &commandBuffer;
		_currentFrame = frame;

		vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_COMPUTE, pipeline);
		vkCmdBindDescriptorSets(commandBuffer, VK_PIPELINE_BIND_POINT_COMPUTE, pipelineLayout, 0, 1, &_descriptorSet, 0, NULL);

		vkCmdDispatch(commandBuffer, (uint32_t)ceil(WIDTH / float(WORKGROUP_SIZE)), (uint32_t)ceil(
			HEIGHT / float(WORKGROUP_SIZE)), 1);
	};

	void end() override
	{
		_currentCommandBuffer = nullptr;
		_currentFrame = -1;

		sleep(1);

		saveRenderedImage();
//		throw std::runtime_error("stop");
	}

	void updateBuffers(Camera camera) override
	{
//		UniformBufferObject ubo{};
//		ubo.model = camera.calculateModelMatrix();
//		ubo.view = camera.calculateViewMatrix();
//		ubo.proj = camera.calculateProjectionMatrix();
//		ubo.proj[1][1] *= -1;
//
//		ubo.view = glm::inverse(ubo.view);
//
//		void* data;
//		vkMapMemory(_device.getVkDevice(), _uniformBuffersMemory[_currentFrame], 0, sizeof(ubo), 0, &data);
//		memcpy(data, &ubo, sizeof(ubo));
//		vkUnmapMemory(_device.getVkDevice(), _uniformBuffersMemory[_currentFrame]);
	}

	void bindModelTransform(glm::mat4 transform) override
	{
//		if (_currentCommandBuffer == nullptr)
//		{
//			throw std::runtime_error("No command buffer is bound");
//		}
//
//		MeshPushConstants constants;
//		constants.transform = transform;
//		vkCmdPushConstants(*_currentCommandBuffer, pipelineLayout, VK_SHADER_STAGE_VERTEX_BIT, 0,
//			sizeof(MeshPushConstants), &constants);
//
	}

	void bindModel(vks::VksModel& model) override
	{
//		_currentModel = &model;
//		_currentModel->bind(*_currentCommandBuffer);
	}

	void drawModel() override
	{
//		if (_currentModel == nullptr)
//		{
//			throw std::runtime_error("Can't draw if no model is currently bound");
//		}
//
//		_currentModel->draw(*_currentCommandBuffer);
	}

	VkBuffer getBuffer()
	{
		return _buffer;
	}

 private:

	const int WIDTH = 1600;
	const int HEIGHT = 900;
	const int WORKGROUP_SIZE = 32;

	VksDevice& _device;
	VkDescriptorPool& _descriptorPool;
	VksSwapChain& _swapChain;
//	std::unique_ptr<VksPipeline> pipeline = nullptr;
	VkPipelineLayout pipelineLayout;
	VkDescriptorSetLayout descriptorSetLayout;
	std::vector<VkDescriptorSet> descriptorSets;
	VkDescriptorSet _descriptorSet;
	VkPipeline pipeline;
	std::vector<VkBuffer> _uniformBuffers;
	std::vector<VkDeviceMemory> _uniformBuffersMemory;

	VkCommandBuffer* _currentCommandBuffer;
	int _currentFrame = -1;

	VkBuffer _buffer;
	VkDeviceMemory _bufferMemory;
	uint32_t _bufferSize;

	/**
	 * Create a pipeline layout
	 * Stores the set of resources that can be used by the pipeline
	 */
	void createPipelineLayout()
	{

//		VkPushConstantRange push_constant;
//		push_constant.offset = 0;
//		push_constant.size = sizeof(MeshPushConstants);
//		push_constant.stageFlags = VK_SHADER_STAGE_VERTEX_BIT;

		VkPipelineLayoutCreateInfo pipelineLayoutInfo{};
		pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
		pipelineLayoutInfo.setLayoutCount = 1;
		pipelineLayoutInfo.pSetLayouts = &descriptorSetLayout;
		pipelineLayoutInfo.pushConstantRangeCount = 0;
		pipelineLayoutInfo.pPushConstantRanges = VK_NULL_HANDLE;
		if (vkCreatePipelineLayout(_device.getVkDevice(), &pipelineLayoutInfo, nullptr, &pipelineLayout) !=
			VK_SUCCESS)
		{
			throw std::runtime_error("failed to create pipeline layout!");
		}

	}

	/**
	 * Stores the data that will be accessible to the descriptor set
	 */
	void createDescriptorSetLayout()
	{
		VkDescriptorSetLayoutBinding binding = {};
		binding.binding = 0;
		binding.descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
		binding.descriptorCount = 1;
		binding.stageFlags = VK_SHADER_STAGE_COMPUTE_BIT;

//		bindings[1].binding = 1;
//		bindings[1].descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
//		bindings[1].descriptorCount = 1;
//		bindings[1].stageFlags = VK_SHADER_STAGE_COMPUTE_BIT;

		VkDescriptorSetLayoutCreateInfo layoutInfo{};
		layoutInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
		layoutInfo.bindingCount = 1;
		layoutInfo.pBindings = &binding;

		if (vkCreateDescriptorSetLayout(_device.getVkDevice(), &layoutInfo, nullptr, &descriptorSetLayout) !=
			VK_SUCCESS)
		{
			throw std::runtime_error("failed to create descriptor set layout!");
		}
	}

	void saveRenderedImage() {
		void* mappedMemory = NULL;
		// Map the buffer memory, so that we can read from it on the CPU.
		vkMapMemory(_device.getVkDevice(), _bufferMemory, 0, _bufferSize, 0, &mappedMemory);
		Pixel* pmappedMemory = (Pixel *)mappedMemory;

		// Get the color data from the buffer, and cast it to bytes.
		// We save the data to a vector.
		std::vector<unsigned char> image;
		image.reserve(WIDTH * HEIGHT * 4);
		for (int i = 0; i < WIDTH*HEIGHT; i += 1) {
			image.push_back((unsigned char)(255.0f * (pmappedMemory[i].color.r)));
			image.push_back((unsigned char)(255.0f * (pmappedMemory[i].color.g)));
			image.push_back((unsigned char)(255.0f * (pmappedMemory[i].color.b)));
			image.push_back((unsigned char)(255.0f * (1)));
		}
		// Done reading, so unmap.
		vkUnmapMemory(_device.getVkDevice(), _bufferMemory);

		// Now we save the acquired color data to a .png.
		unsigned error = lodepng::encode("mandelbrot.png", image, WIDTH, HEIGHT);
		if (error) printf("encoder error %d: %s", error, lodepng_error_text(error));
	}

	void createDescriptorSets()
	{
		std::vector<VkDescriptorSetLayout> layouts(_swapChain.getImageCount(), descriptorSetLayout);
		VkDescriptorSetAllocateInfo allocInfo{};
		allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
		allocInfo.descriptorPool = _descriptorPool;
//		allocInfo.descriptorSetCount = static_cast<uint32_t>(_swapChain.getImageCount());
		allocInfo.descriptorSetCount = 1;
		allocInfo.pSetLayouts = &descriptorSetLayout;

		if (vkAllocateDescriptorSets(_device.getVkDevice(), &allocInfo, &_descriptorSet) != VK_SUCCESS)
		{
			throw std::runtime_error("failed to allocate descriptor sets!");
		}

		// Allocate the storage buffer to the descriptor set

		VkDescriptorBufferInfo bufferInfo{};
		bufferInfo.buffer = _buffer;
		bufferInfo.offset = 0;
		bufferInfo.range = _bufferSize;

		VkWriteDescriptorSet descriptorWrite{};
		descriptorWrite.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
		descriptorWrite.dstSet = _descriptorSet; // Write to this descriptor set
		descriptorWrite.dstBinding = 0; // Write to the first and only binding
		descriptorWrite.dstArrayElement = 0;
		descriptorWrite.descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
		descriptorWrite.descriptorCount = 1; // Update a single descriptor
		descriptorWrite.pBufferInfo = &bufferInfo;
		descriptorWrite.pImageInfo = nullptr; // Optional
		descriptorWrite.pTexelBufferView = nullptr; // Optional
		vkUpdateDescriptorSets(_device.getVkDevice(), 1, &descriptorWrite, 0, nullptr);
	}

	void createBuffer()
	{
		_device.createBuffer(
			_bufferSize,
			VK_BUFFER_USAGE_STORAGE_BUFFER_BIT,
			VK_MEMORY_PROPERTY_HOST_COHERENT_BIT | VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT,
			_buffer,
			_bufferMemory
		);
	}

	void createPipeline()
	{
//		PipelineConfigInfo pipelineConfigInfo{};
//		VksPipeline::defaultPipelineConfigInfo(pipelineConfigInfo);
//		pipelineConfigInfo.renderPass = _swapChain.getRenderPass();
//		pipelineConfigInfo.pipelineLayout = pipelineLayout;
//		pipeline = std::make_unique<VksPipeline>(_device, _swapChain, pipelineConfigInfo);

		VkPipelineShaderStageCreateInfo shaderStageCreateInfo = {};
		shaderStageCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
		shaderStageCreateInfo.stage = VK_SHADER_STAGE_COMPUTE_BIT;
		shaderStageCreateInfo.module = _device.createShaderModule(VksUtil::readFile("shaders/raycasting.comp.spv"));
		shaderStageCreateInfo.pName = "main";

		/*
        The pipeline layout allows the pipeline to access descriptor sets.
        So we just specify the descriptor set layout we created earlier.
        */
		VkPipelineLayoutCreateInfo pipelineLayoutCreateInfo = {};
		pipelineLayoutCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
		pipelineLayoutCreateInfo.setLayoutCount = 1;
		pipelineLayoutCreateInfo.pSetLayouts = &descriptorSetLayout;
		if (vkCreatePipelineLayout(_device.getVkDevice(), &pipelineLayoutCreateInfo, nullptr, &pipelineLayout)
			!= VK_SUCCESS)
		{
			throw std::runtime_error("Failed to create compute pipeline layout");
		}

		VkComputePipelineCreateInfo pipelineCreateInfo = {};
		pipelineCreateInfo.sType = VK_STRUCTURE_TYPE_COMPUTE_PIPELINE_CREATE_INFO;
		pipelineCreateInfo.stage = shaderStageCreateInfo;
		pipelineCreateInfo.layout = pipelineLayout;

		/*
		Now, we finally create the compute pipeline.
		*/
		if (vkCreateComputePipelines(
			_device.getVkDevice(), VK_NULL_HANDLE,
			1, &pipelineCreateInfo,
			NULL, &pipeline) != VK_SUCCESS)
		{
			throw std::runtime_error("Failed to create compute pipeline");
		}
	}

	void createUniformBuffers()
	{
		VkDeviceSize bufferSize = sizeof(UniformBufferObject);

		_uniformBuffers.resize(_swapChain.getImageCount());
		_uniformBuffersMemory.resize(_swapChain.getImageCount());

		for (size_t i = 0; i < _swapChain.getImageCount(); i++)
		{
			_device.createBuffer(
				bufferSize,
				VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
				VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
				_uniformBuffers[i],
				_uniformBuffersMemory[i]
			);
		}
	}
};

#endif //_COMPUTE_PIPELINE_H_
