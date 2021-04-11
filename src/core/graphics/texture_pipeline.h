//
// Created by magnias on 31/03/2021.
//

#ifndef VULKANENGINE_TEXTURE_PIPELINE_H
#define VULKANENGINE_TEXTURE_PIPELINE_H

#include <vulkan/vulkan.h>
#include <glm/glm.hpp>
#include <stb_image.h>
#include "../../vks/vks_pipeline.h"
#include "../../vks/vks_swap_chain.h"
#include "../camera.h"

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

/**
 * Pipeline that renders a full-screen quad with custom texture
 */
class TextureRenderPipeline : public IRenderPipeline, public IRenderProvider
{
 public:
	TextureRenderPipeline(VksDevice& device, VksSwapChain& swapChain, VkDescriptorPool& descriptorPool)
		: _device(device), _descriptorPool(descriptorPool), _swapChain(swapChain)
	{
		init();
	}

	void init()
	{
		createTextureImage(_device);
		createTextureImageView(_device);
		createTextureSampler(_device);
		createDescriptorSetLayout();
		createPipelineLayout();
		createPipeline();
		createUniformBuffers();
		createDescriptorSets();
	}

	void begin(VkCommandBuffer& commandBuffer, int frame) override
	{
		_currentCommandBuffer = &commandBuffer;
		_currentFrame = frame;

		vkCmdBindDescriptorSets(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, pipelineLayout, 0, 1,
			&descriptorSets[frame], 0, nullptr);

		pipeline->bind(commandBuffer);
	};

	void end() override
	{
		_currentCommandBuffer = nullptr;
		_currentFrame = -1;
	}

	void updateBuffers(Camera camera) override
	{
		UniformBufferObject ubo{};
		ubo.model = camera.calculateModelMatrix();
		ubo.view = camera.calculateViewMatrix();
		ubo.proj = camera.calculateProjectionMatrix();
		ubo.proj[1][1] *= -1;

		ubo.view = glm::inverse(ubo.view);

		void* data;
		vkMapMemory(_device.getVkDevice(), _uniformBuffersMemory[_currentFrame], 0, sizeof(ubo), 0, &data);
		memcpy(data, &ubo, sizeof(ubo));
		vkUnmapMemory(_device.getVkDevice(), _uniformBuffersMemory[_currentFrame]);
	}

	void bindModelTransform(glm::mat4 transform) override
	{
		if (_currentCommandBuffer == nullptr)
		{
			throw std::runtime_error("No command buffer is bound");
		}

		MeshPushConstants constants;
		constants.transform = transform;
		vkCmdPushConstants(*_currentCommandBuffer, pipelineLayout, VK_SHADER_STAGE_VERTEX_BIT, 0,
			sizeof(MeshPushConstants), &constants);

	}

	void bindModel(vks::VksModel& model) override
	{
		_currentModel = &model;
		_currentModel->bind(*_currentCommandBuffer);
	}

	void drawModel() override
	{
		if (_currentModel == nullptr)
		{
			throw std::runtime_error("Can't draw if no model is currently bound");
		}

		_currentModel->draw(*_currentCommandBuffer);
	}

	std::vector<VkDescriptorSet> getVkDescriptorSet()
	{
		return descriptorSets;
	}

	VkPipelineLayout getVkPipelinelayout()
	{
		return pipelineLayout;
	}

	VkPipeline getVkPipeline()
	{
		return pipeline->getPipeline();
	}

 private:

	VksDevice& _device;
	VkDescriptorPool& _descriptorPool;
	VksSwapChain& _swapChain;
	std::unique_ptr<VksPipeline> pipeline = nullptr;
	VkPipelineLayout pipelineLayout;
	VkDescriptorSetLayout descriptorSetLayout;
	std::vector<VkDescriptorSet> descriptorSets;
	std::vector<VkBuffer> _uniformBuffers;
	std::vector<VkDeviceMemory> _uniformBuffersMemory;

	VkCommandBuffer* _currentCommandBuffer;
	int _currentFrame = -1;
	vks::VksModel* _currentModel;

	VkSampler textureSampler;

	VkBuffer stagingBuffer;
	VkDeviceMemory stagingBufferMemory;
	VkImage textureImage;
	VkDeviceMemory textureImageMemory;
	VkImageView textureImageView;

	void createTextureImage(VksDevice &device)
	{
		// TODO: destroy images/buffers/memory

		// Load texture from disk
		int texWidth, texHeight, texChannels;
		stbi_uc* pixels = stbi_load("data/textures/texture.jpg", &texWidth, &texHeight, &texChannels, STBI_rgb_alpha);
		VkDeviceSize imageSize = texWidth * texHeight * 4;

		if (!pixels) {
			throw std::runtime_error("failed to load texture image!");
		}

		device.createBuffer(imageSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, stagingBuffer, stagingBufferMemory);

		void* data;
		vkMapMemory(device.getVkDevice(), stagingBufferMemory, 0, imageSize, 0, &data);
		memcpy(data, pixels, static_cast<size_t>(imageSize));
		vkUnmapMemory(device.getVkDevice(), stagingBufferMemory);

		stbi_image_free(pixels);

		// Create vulkan image
		VkImageCreateInfo imageInfo{};
		imageInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
		imageInfo.imageType = VK_IMAGE_TYPE_2D;
		imageInfo.extent.width = static_cast<uint32_t>(texWidth);
		imageInfo.extent.height = static_cast<uint32_t>(texHeight);
		imageInfo.extent.depth = 1;
		imageInfo.mipLevels = 1;
		imageInfo.arrayLayers = 1;
		imageInfo.format = VK_FORMAT_R8G8B8A8_SRGB;
		imageInfo.tiling = VK_IMAGE_TILING_OPTIMAL;
		imageInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
		imageInfo.usage = VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT;
		imageInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
		imageInfo.samples = VK_SAMPLE_COUNT_1_BIT;
		imageInfo.flags = 0;

		device.createImageWithInfo(imageInfo, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, textureImage, textureImageMemory);

		// Create the texture image
		transitionImageLayout(device, textureImage, VK_FORMAT_R8G8B8A8_SRGB, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL);
		device.copyBufferToImage(stagingBuffer, textureImage, static_cast<uint32_t>(texWidth), static_cast<uint32_t>(texHeight));

		transitionImageLayout(device, textureImage, VK_FORMAT_R8G8B8A8_SRGB, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);
	}

	static void transitionImageLayout(VksDevice device, VkImage image, VkFormat format, VkImageLayout oldLayout, VkImageLayout newLayout)
	{
		VkCommandBuffer commandBuffer = device.beginSingleTimeCommands();

		VkImageMemoryBarrier barrier{};
		barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
		barrier.oldLayout = oldLayout;
		barrier.newLayout = newLayout;
		barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
		barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
		barrier.image = image;
		barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
		barrier.subresourceRange.baseMipLevel = 0;
		barrier.subresourceRange.levelCount = 1;
		barrier.subresourceRange.baseArrayLayer = 0;
		barrier.subresourceRange.layerCount = 1;
		barrier.srcAccessMask = 0; // TODO
		barrier.dstAccessMask = 0; // TODO

		VkPipelineStageFlags sourceStage;
		VkPipelineStageFlags destinationStage;

		if (oldLayout == VK_IMAGE_LAYOUT_UNDEFINED && newLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL) {
			barrier.srcAccessMask = 0;
			barrier.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;

			sourceStage = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
			destinationStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
		} else if (oldLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL && newLayout == VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL) {
			barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
			barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;

			sourceStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
			destinationStage = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
		} else {
			throw std::invalid_argument("unsupported layout transition!");
		}

		vkCmdPipelineBarrier(
			commandBuffer,
			sourceStage, destinationStage,
			0,
			0, nullptr,
			0, nullptr,
			1, &barrier
		);

		device.endSingleTimeCommands(commandBuffer);
	}

	void createTextureImageView(VksDevice& device)
	{
		VkImageViewCreateInfo viewInfo{};
		viewInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
		viewInfo.image = textureImage;
		viewInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
		viewInfo.format = VK_FORMAT_R8G8B8A8_SRGB;
		viewInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
		viewInfo.subresourceRange.baseMipLevel = 0;
		viewInfo.subresourceRange.levelCount = 1;
		viewInfo.subresourceRange.baseArrayLayer = 0;
		viewInfo.subresourceRange.layerCount = 1;

		if (vkCreateImageView(device.getVkDevice(), &viewInfo, nullptr, &textureImageView) != VK_SUCCESS)
		{
			throw std::runtime_error("failed to create texture image view!");
		}
	}
	void createTextureSampler(VksDevice& device)
	{
		VkSamplerCreateInfo samplerInfo{};
		samplerInfo.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
		samplerInfo.magFilter = VK_FILTER_LINEAR;
		samplerInfo.minFilter = VK_FILTER_LINEAR;
		samplerInfo.addressModeU = VK_SAMPLER_ADDRESS_MODE_REPEAT;
		samplerInfo.addressModeV = VK_SAMPLER_ADDRESS_MODE_REPEAT;
		samplerInfo.addressModeW = VK_SAMPLER_ADDRESS_MODE_REPEAT;
		samplerInfo.anisotropyEnable = VK_FALSE;
		samplerInfo.unnormalizedCoordinates = VK_FALSE;
		samplerInfo.compareEnable = VK_FALSE;
		samplerInfo.compareOp = VK_COMPARE_OP_ALWAYS;
		samplerInfo.borderColor = VK_BORDER_COLOR_INT_OPAQUE_BLACK;
		samplerInfo.mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR;
		samplerInfo.mipLodBias = 0.0f;
		samplerInfo.minLod = 0.0f;
		samplerInfo.maxLod = 0.0f;

		if (vkCreateSampler(device.getVkDevice(), &samplerInfo, nullptr, &textureSampler) != VK_SUCCESS)
		{
			throw std::runtime_error("failed to create texture sampler!");
		}
	}

	/**
	 * Create a pipeline layout
	 * Stores the set of resources that can be used by the pipeline
	 */
	void createPipelineLayout()
	{
//        spdlog::get("vulkan")->debug("Creating pipeline layout..");

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
		if (vkCreatePipelineLayout(_device.getVkDevice(), &pipelineLayoutInfo, nullptr, &pipelineLayout) !=
			VK_SUCCESS)
		{
			throw std::runtime_error("failed to create pipeline layout!");
		}

	}

	void createPipeline()
	{
//        assert(_swapChain != nullptr && "Cannot create pipeline before swap chain");
//        assert(pipelineLayout != nullptr && "Cannot create pipeline before pipeline layout");

//        spdlog::get("vulkan")->debug("Creating pipeline..");

		PipelineConfigInfo pipelineConfigInfo{};
		VksPipeline::defaultPipelineConfigInfo(pipelineConfigInfo);
		pipelineConfigInfo.renderPass = _swapChain.getRenderPass();
		pipelineConfigInfo.pipelineLayout = pipelineLayout;
		pipeline = std::make_unique<VksPipeline>(_device, _swapChain, pipelineConfigInfo);
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

		VkDescriptorSetLayoutBinding samplerLayoutBinding{};
		samplerLayoutBinding.binding = 1;
		samplerLayoutBinding.descriptorCount = 1;
		samplerLayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
		samplerLayoutBinding.pImmutableSamplers = nullptr;
		samplerLayoutBinding.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;

		std::array<VkDescriptorSetLayoutBinding, 2> bindings = { uboLayoutBinding, samplerLayoutBinding };

		VkDescriptorSetLayoutCreateInfo layoutInfo{};
		layoutInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
		layoutInfo.bindingCount = static_cast<uint32_t >(bindings.size());
		layoutInfo.pBindings = bindings.data();

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
		allocInfo.descriptorSetCount = static_cast<uint32_t>(_swapChain.getImageCount());
		allocInfo.pSetLayouts = layouts.data();

		descriptorSets.resize(_swapChain.getImageCount());
		if (vkAllocateDescriptorSets(_device.getVkDevice(), &allocInfo, descriptorSets.data()) != VK_SUCCESS)
		{
			throw std::runtime_error("failed to allocate descriptor sets!");
		}

		for (size_t i = 0; i < _swapChain.getImageCount(); i++)
		{
			VkDescriptorBufferInfo bufferInfo{};
			bufferInfo.buffer = _uniformBuffers[i];
			bufferInfo.offset = 0;
			bufferInfo.range = sizeof(UniformBufferObject);

			VkDescriptorImageInfo imageInfo{};
			imageInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
			imageInfo.imageView = textureImageView;
			imageInfo.sampler = textureSampler;

			std::array<VkWriteDescriptorSet, 2> descriptorWrites{};

			descriptorWrites[0].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
			descriptorWrites[0].dstSet = descriptorSets[i];
			descriptorWrites[0].dstBinding = 0;
			descriptorWrites[0].dstArrayElement = 0;
			descriptorWrites[0].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
			descriptorWrites[0].descriptorCount = 1;
			descriptorWrites[0].pBufferInfo = &bufferInfo;

			descriptorWrites[1].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
			descriptorWrites[1].dstSet = descriptorSets[i];
			descriptorWrites[1].dstBinding = 1;
			descriptorWrites[1].dstArrayElement = 0;
			descriptorWrites[1].descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
			descriptorWrites[1].descriptorCount = 1;
			descriptorWrites[1].pImageInfo = &imageInfo;

			vkUpdateDescriptorSets(_device.getVkDevice(), static_cast<uint32_t>(descriptorWrites.size()), descriptorWrites.data(), 0, nullptr);
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

#endif //VULKANENGINE_TEXTURE_PIPELINE_H
