//
// Created by magnias on 04/04/2021.
//

#ifndef _COMPUTE_PIPELINE_H_
#define _COMPUTE_PIPELINE_H_

#include <vks/vks_util.h>
#include "render_pipeline.h"
#include <lodepng.h>
#include <vks/VulkanInitializers.h>

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
		createTexture();
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


//		saveRenderedImage();
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

	VkImage getComputeTarget()
	{
		return _computeTarget;
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

	VkImage _computeTarget;
	VkSampler _computeSampler;
	VkSamplerMipmapMode _miplevels;
	VkImageLayout _imageLayout;
	VkImageView _imageView;
	VkDeviceMemory _computeTextureMemory;

	VkBuffer _buffer;
	VkDeviceMemory _bufferMemory;
	uint32_t _bufferSize;

	// Prepare a texture target that is used to store compute shader calculations
	void createTexture()
	{
		VkFormatProperties formatProperties;

		// Get device properties for the requested texture format
		vkGetPhysicalDeviceFormatProperties(_device.getPhysicalDevice(), VK_FORMAT_R8G8B8A8_UNORM, &formatProperties);
		// Check if requested image format supports image storage operations
		assert(formatProperties.optimalTilingFeatures & VK_FORMAT_FEATURE_STORAGE_IMAGE_BIT);

		VkImageCreateInfo imageCreateInfo = vks::initializers::imageCreateInfo();
		imageCreateInfo.imageType = VK_IMAGE_TYPE_2D;
		imageCreateInfo.format = VK_FORMAT_R8G8B8A8_UNORM;
		imageCreateInfo.extent = { _swapChain.getSwapChainExtent().width, _swapChain.getSwapChainExtent().height, 1 };
		imageCreateInfo.mipLevels = 1;
		imageCreateInfo.arrayLayers = 1;
		imageCreateInfo.samples = VK_SAMPLE_COUNT_1_BIT;
		imageCreateInfo.tiling = VK_IMAGE_TILING_OPTIMAL;
		// Image will be sampled in the fragment shader and used as storage target in the compute shader
		imageCreateInfo.usage = VK_IMAGE_USAGE_SAMPLED_BIT | VK_IMAGE_USAGE_STORAGE_BIT;
		imageCreateInfo.flags = 0;
		// If compute and graphics queue family indices differ, we create an image that can be shared between them
		// This can result in worse performance than exclusive sharing mode, but save some synchronization to keep the sample simple
		std::vector<uint32_t> queueFamilyIndices;
//		if (_device.findQueueFamilies().graphicsFamily != vulkanDevice->queueFamilyIndices.compute) {
//			queueFamilyIndices = {
//				vulkanDevice->queueFamilyIndices.graphics,
//				vulkanDevice->queueFamilyIndices.compute
//			};
//			imageCreateInfo.sharingMode = VK_SHARING_MODE_CONCURRENT;
//			imageCreateInfo.queueFamilyIndexCount = 2;
//			imageCreateInfo.pQueueFamilyIndices = queueFamilyIndices.data();
//		}

		VkMemoryAllocateInfo memAllocInfo = vks::initializers::memoryAllocateInfo();
		VkMemoryRequirements memReqs;

		if(vkCreateImage(_device.getVkDevice(), &imageCreateInfo, nullptr, &_computeTarget) != VK_SUCCESS)
		{
			throw std::runtime_error("Failed to create texture");
		}

		vkGetImageMemoryRequirements(_device.getVkDevice(), _computeTarget, &memReqs);
		memAllocInfo.allocationSize = memReqs.size;
		memAllocInfo.memoryTypeIndex = _device.getMemoryType(memReqs.memoryTypeBits, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);
		if(vkAllocateMemory(_device.getVkDevice(), &memAllocInfo, nullptr, &_computeTextureMemory) != VK_SUCCESS)
		{
			throw std::runtime_error("Failed to allocate texture memory");
		}
		if(vkBindImageMemory(_device.getVkDevice(), _computeTarget, _computeTextureMemory, 0) != VK_SUCCESS)
		{
			throw std::runtime_error("Failed to bind image memory");
		}

		VkCommandBuffer layoutCmd = _device.createCommandBuffer(VK_COMMAND_BUFFER_LEVEL_PRIMARY, true);

		_imageLayout = VK_IMAGE_LAYOUT_GENERAL;
//		vks::tools::setImageLayout(
//			layoutCmd, _computeTarget,
//			VK_IMAGE_ASPECT_COLOR_BIT,
//			VK_IMAGE_LAYOUT_UNDEFINED,
//			_imageLayout);

		_device.endSingleTimeCommands(layoutCmd);

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
		if(vkCreateSampler(_device.getVkDevice(), &sampler, nullptr, &_computeSampler) != VK_SUCCESS)
		{
			throw std::runtime_error("Failed to create texture");
		}

		// Create image view
		VkImageViewCreateInfo view = vks::initializers::imageViewCreateInfo();
		view.image = VK_NULL_HANDLE;
		view.viewType = VK_IMAGE_VIEW_TYPE_2D;
		view.format = VK_FORMAT_R8G8B8A8_UNORM;
		view.components = { VK_COMPONENT_SWIZZLE_R, VK_COMPONENT_SWIZZLE_G, VK_COMPONENT_SWIZZLE_B, VK_COMPONENT_SWIZZLE_A };
		view.subresourceRange = { VK_IMAGE_ASPECT_COLOR_BIT, 0, 1, 0, 1 };
		view.image = _computeTarget;
		if(vkCreateImageView(_device.getVkDevice(), &view, nullptr, &_imageView) != VK_SUCCESS)
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
		binding.descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_IMAGE;
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

//		VkDescriptorBufferInfo bufferInfo{};
//		bufferInfo.buffer = _buffer;
//		bufferInfo.offset = 0;
//		bufferInfo.range = _bufferSize;

		VkDescriptorImageInfo imageInfo{};
		imageInfo.imageView = _imageView;
		imageInfo.sampler = _computeSampler;
		imageInfo.imageLayout = _imageLayout;

		VkWriteDescriptorSet descriptorWrite{};
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
