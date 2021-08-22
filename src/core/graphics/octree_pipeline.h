//
// Created by magnias on 04/04/2021.
//

#ifndef _OCTREE_PIPELINE_H_
#define _OCTREE_PIPELINE_H_

#include <vks/vks_util.h>
#include "render_pipeline.h"
#include <lodepng.h>
#include <vks/VulkanInitializers.h>

class OctreePipeline : public IRenderPipeline
{
 public:
    OctreePipeline(VksDevice& device, VksSwapChain& swapChain, VkDescriptorPool& descriptorPool)
		: _device(device), _descriptorPool(descriptorPool), _swapChain(swapChain)
	{
		init();
	}

	void init()
	{

		_computeQueue = _device.getComputeQueue();

		createTexture(VK_FORMAT_R8G8B8A8_UNORM);
		createDescriptorSetLayout();
		createPipelineLayout();
		createPipeline();
//		createUniformBuffers();
		createDescriptorSets();
	}

	void begin(VkCommandBuffer& commandBuffer, int frame) override
	{
//		_currentFrame = frame;
		_currentCommandBuffer = commandBuffer;

		vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_COMPUTE, pipeline);
		vkCmdBindDescriptorSets(commandBuffer, VK_PIPELINE_BIND_POINT_COMPUTE, pipelineLayout, 0, 1, &_descriptorSet, 0, NULL);

	};

	void end() override
	{
	}

	void setEpsilon(float epsilon)
	{
		_epsilon = epsilon;
	}

	void setMaxPasses(int max_passes)
	{
		_max_passes = max_passes;
	}

    void updateBuffers(Camera camera)
    {}

	void updateBuffers(Camera camera, glm::vec4 lookat)
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
//		vkMapMemory(_device.getVkDevice(), _uniformBuffersMemory[0], 0, sizeof(ubo), 0, &data);
//		memcpy(data, &ubo, sizeof(ubo));
//		vkUnmapMemory(_device.getVkDevice(), _uniformBuffersMemory[0]);
		MeshPushConstants constants;
		constants.model = camera.calculateModelMatrix();
		constants.view = camera.calculateViewMatrix();
		constants.projection = camera.calculateProjectionMatrix();
		constants.lookat = lookat;
		std::chrono::milliseconds time =
			std::chrono::duration_cast<std::chrono::milliseconds>(
				std::chrono::system_clock::now().time_since_epoch()
			);
		constants.time = time.count() % 10000 / 1000.0f;
		constants.epsilon = _epsilon;
		constants.max_passes = _max_passes;
		vkCmdPushConstants(_currentCommandBuffer, pipelineLayout, VK_SHADER_STAGE_COMPUTE_BIT, 0,
			sizeof(MeshPushConstants), &constants);

		vkCmdDispatch(_currentCommandBuffer, (uint32_t)ceil(WIDTH / float(WORKGROUP_SIZE)), (uint32_t)ceil(
			HEIGHT / float(WORKGROUP_SIZE)), 1);
	}

	VkImage getResultImage()
	{
		return _computeTexture;
	}

	VkDescriptorImageInfo getComputeTarget()
	{
		VkDescriptorImageInfo imageInfo{};
		imageInfo.imageView = _imageView;
		imageInfo.sampler = _computeSampler;
		imageInfo.imageLayout = _imageLayout;
		return imageInfo;
	}

 private:

	struct MeshPushConstants
	{
		glm::mat4 model;
		glm::mat4 view;
		glm::mat4 projection;
        glm::vec4 lookat;
		float time;
		float epsilon;
		int max_passes;
	};

	float _epsilon = 0.001f;
	int _max_passes = 200;

	const int WIDTH = 1600;
	const int HEIGHT = 900;
	const int WORKGROUP_SIZE = 32;

	VksDevice& _device;
	VkDescriptorPool& _descriptorPool;
	VksSwapChain& _swapChain;
	VkPipelineLayout pipelineLayout;
	VkDescriptorSetLayout descriptorSetLayout;
	VkDescriptorSet _descriptorSet;
	VkPipeline pipeline;
	std::vector<VkBuffer> _uniformBuffers;
	std::vector<VkDeviceMemory> _uniformBuffersMemory;

	VkCommandBuffer _currentCommandBuffer;
	uint32_t _currentFrame;

	VkQueue _computeQueue;
	VkImage _computeTexture;
	VkSampler _computeSampler;
	VkSamplerMipmapMode _miplevels;
	VkImageLayout _imageLayout;
	VkImageView _imageView;
	VkDeviceMemory _computeTextureMemory;

	// Prepare a texture target that is used to store compute shader calculations
	void createTexture(VkFormat format)
	{
		VkImageCreateInfo imageInfo{};
		imageInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
		imageInfo.imageType = VK_IMAGE_TYPE_2D;
		imageInfo.extent.width = static_cast<uint32_t>(_swapChain.getSwapChainExtent().width);
		imageInfo.extent.height = static_cast<uint32_t>(_swapChain.getSwapChainExtent().height);
		imageInfo.extent.depth = 1;
		imageInfo.mipLevels = 1;
		imageInfo.arrayLayers = 1;
		imageInfo.format = format;
		imageInfo.tiling = VK_IMAGE_TILING_OPTIMAL;
		imageInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
		imageInfo.usage = VK_IMAGE_USAGE_SAMPLED_BIT | VK_IMAGE_USAGE_STORAGE_BIT;
//		imageInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
		imageInfo.samples = VK_SAMPLE_COUNT_1_BIT;
		imageInfo.flags = 0;

		// Get device properties for the requested texture format
		VkFormatProperties formatProperties;
		vkGetPhysicalDeviceFormatProperties(_device.getPhysicalDevice(), format, &formatProperties);
		// Check if requested image format supports image storage operations
		assert(formatProperties.optimalTilingFeatures & VK_FORMAT_FEATURE_STORAGE_IMAGE_BIT);

		_device
			.createImageWithInfo(imageInfo, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, _computeTexture, _computeTextureMemory);

		// Update layout
		_imageLayout = VK_IMAGE_LAYOUT_GENERAL;
		_device.transitionImageLayout(_computeTexture, VK_IMAGE_LAYOUT_UNDEFINED, _imageLayout);

		// Create sampler
		VkSamplerCreateInfo sampler = vks::initializers::samplerCreateInfo();
		sampler.magFilter = VK_FILTER_LINEAR;
		sampler.minFilter = VK_FILTER_LINEAR;
		sampler.mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR;
		sampler.addressModeU = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_BORDER;
		sampler.addressModeV = sampler.addressModeU;
		sampler.addressModeW = sampler.addressModeU;
		sampler.mipLodBias = 0.0f;
		sampler.maxAnisotropy = 1.0f;
		sampler.compareOp = VK_COMPARE_OP_NEVER;
		sampler.minLod = 0.0f;
		sampler.maxLod = _miplevels;
		sampler.borderColor = VK_BORDER_COLOR_FLOAT_OPAQUE_WHITE;
		if (vkCreateSampler(_device.getVkDevice(), &sampler, nullptr, &_computeSampler) != VK_SUCCESS)
		{
			throw std::runtime_error("Failed to create texture");
		}

		// Create image view
		VkImageViewCreateInfo view = vks::initializers::imageViewCreateInfo();
		view.image = VK_NULL_HANDLE;
		view.viewType = VK_IMAGE_VIEW_TYPE_2D;
		view.format = format;
		view.components = { VK_COMPONENT_SWIZZLE_R, VK_COMPONENT_SWIZZLE_G, VK_COMPONENT_SWIZZLE_B,
							VK_COMPONENT_SWIZZLE_A };
		view.subresourceRange = { VK_IMAGE_ASPECT_COLOR_BIT, 0, 1, 0, 1 };
		view.image = _computeTexture;
		if (vkCreateImageView(_device.getVkDevice(), &view, nullptr, &_imageView) != VK_SUCCESS)
		{
			throw std::runtime_error("Failed to create image view");
		}

		// Initialize a descriptor for later use
//		tex->descriptor.imageLayout = tex->imageLayout;
//		tex->descriptor.imageView = tex->view;
//		tex->descriptor.sampler = tex->sampler;
//		tex->device = vulkanDevice;

	}

	/**
	 * Create a pipeline layout
	 * Stores the set of resources that can be used by the pipeline
	 */
	void createPipelineLayout()
	{

		VkPushConstantRange push_constant;
		push_constant.offset = 0;
		push_constant.size = sizeof(MeshPushConstants);
		push_constant.stageFlags = VK_SHADER_STAGE_COMPUTE_BIT;

		VkPipelineLayoutCreateInfo pipelineLayoutInfo{};
		pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
		pipelineLayoutInfo.setLayoutCount = 1;
		pipelineLayoutInfo.pSetLayouts = &descriptorSetLayout;
		pipelineLayoutInfo.pushConstantRangeCount = 1;
		pipelineLayoutInfo.pPushConstantRanges = &push_constant;
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
		binding.descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_IMAGE;
		binding.descriptorCount = 1;
		binding.stageFlags = VK_SHADER_STAGE_COMPUTE_BIT;

//		binding.binding = 0;
//		binding.descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
//		binding.descriptorCount = 1;
//		binding.stageFlags = VK_SHADER_STAGE_COMPUTE_BIT;

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

//		VkDescriptorBufferInfo bufferInfo{};
//		bufferInfo.buffer = _buffer;
//		bufferInfo.offset = 0;
//		bufferInfo.range = _bufferSize;
//		VkDescriptorBufferInfo bufferInfo{};
//		bufferInfo.buffer = _uniformBuffers[0];
//		bufferInfo.offset = 0;
//		bufferInfo.range = sizeof(UniformBufferObject);

		VkDescriptorImageInfo imageInfo{};
		imageInfo.imageView = _imageView;
		imageInfo.sampler = _computeSampler;
		imageInfo.imageLayout = _imageLayout;

		VkWriteDescriptorSet descriptorWrite{};
//		std::array<VkWriteDescriptorSet, 2> descriptorWrites{};
//
//		descriptorWrites[0].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
//		descriptorWrites[0].dstSet = _descriptorSet;
//		descriptorWrites[0].dstBinding = 0;
//		descriptorWrites[0].dstArrayElement = 0;
//		descriptorWrites[0].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
//		descriptorWrites[0].descriptorCount = 1;
//		descriptorWrites[0].pBufferInfo = &bufferInfo;

		descriptorWrite.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
		descriptorWrite.dstSet = _descriptorSet; // Write to this descriptor set
		descriptorWrite.dstBinding = 0; // Write to the first and only binding
		descriptorWrite.dstArrayElement = 0;
		descriptorWrite.descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_IMAGE;
		descriptorWrite.descriptorCount = 1; // Update a single descriptor
//		descriptorWrite.pBufferInfo = &bufferInfo;
		descriptorWrite.pImageInfo = &imageInfo; // Optional
		descriptorWrite.pTexelBufferView = nullptr; // Optional
		vkUpdateDescriptorSets(_device.getVkDevice(), 1, &descriptorWrite, 0, nullptr);
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

		VkPushConstantRange push_constant;
		push_constant.offset = 0;
		push_constant.size = sizeof(MeshPushConstants);
		push_constant.stageFlags = VK_SHADER_STAGE_COMPUTE_BIT;
		/*
        The pipeline layout allows the pipeline to access descriptor sets.
        So we just specify the descriptor set layout we created earlier.
        */
		VkPipelineLayoutCreateInfo pipelineLayoutCreateInfo = {};
		pipelineLayoutCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
		pipelineLayoutCreateInfo.setLayoutCount = 1;
		pipelineLayoutCreateInfo.pSetLayouts = &descriptorSetLayout;
		pipelineLayoutCreateInfo.pushConstantRangeCount = 1;
		pipelineLayoutCreateInfo.pPushConstantRanges = &push_constant;
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

//	void createUniformBuffers()
//	{
//		VkDeviceSize bufferSize = sizeof(UniformBufferObject);
//
//		_uniformBuffers.resize(_swapChain.getImageCount());
//		_uniformBuffersMemory.resize(_swapChain.getImageCount());
//
//		for (size_t i = 0; i < _swapChain.getImageCount(); i++)
//		{
//			_device.createBuffer(
//				bufferSize,
//				VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
//				VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
//				_uniformBuffers[i],
//				_uniformBuffersMemory[i]
//			);
//		}
//	}

};

#endif //_OCTREE_PIPELINE_H_
