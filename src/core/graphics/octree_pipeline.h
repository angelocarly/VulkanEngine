//
// Created by magnias on 04/04/2021.
//

#ifndef _OCTREE_PIPELINE_H_
#define _OCTREE_PIPELINE_H_

#include <vks/vks_util.h>
#include "render_pipeline.h"
#include <lodepng.h>
#include <vks/VulkanInitializers.h>
#include <core/engine/octree.h>

class OctreePipeline: public IRenderPipeline
{
public:

	struct oc_node
	{
		int index;
		int child[8];
		int leaf;
		float depth;
		int no2;
		glm::vec4 color;
	};

	OctreePipeline(VksDevice &device, VksSwapChain &swapChain, VkDescriptorPool &descriptorPool)
		: _device(device), _descriptorPool(descriptorPool), _swapChain(swapChain)
	{
		init();
	}

	void init()
	{

		_computeQueue = _device.getComputeQueue();

		createOctreeBuffer();
		createTexture(VK_FORMAT_R8G8B8A8_UNORM);
		createDescriptorSetLayout();
		createPipelineLayout();
		createPipeline();
//		createUniformBuffers();
		createDescriptorSets();
	}

	void begin(VkCommandBuffer &commandBuffer, int frame) override
	{
//		_currentFrame = frame;
		_currentCommandBuffer = commandBuffer;

		vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_COMPUTE, pipeline);
		vkCmdBindDescriptorSets(commandBuffer, VK_PIPELINE_BIND_POINT_COMPUTE, pipelineLayout, 0, 1, &_descriptorSet, 0,
								NULL);

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
		constants.depth = _depth;
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

	void setDepth(float depth)
	{
		_depth = depth;
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
		float depth;
	};

	float _epsilon = 0.001f;
	int _max_passes = 200;
	float _depth = 0.3f;

	const int WIDTH = 1600;
	const int HEIGHT = 900;
	const int WORKGROUP_SIZE = 16;

	VksDevice &_device;
	VkDescriptorPool &_descriptorPool;
	VksSwapChain &_swapChain;
	VkPipelineLayout pipelineLayout;
	VkDescriptorSetLayout descriptorSetLayout;
	VkDescriptorSet _descriptorSet;
	VkPipeline pipeline;
	std::vector<VkBuffer> _uniformBuffers;
	std::vector<VkDeviceMemory> _uniformBuffersMemory;
	VkBuffer _buffer;
	VkDeviceMemory _bufferMemory;
	VkDeviceSize _bufferSize;

	VkCommandBuffer _currentCommandBuffer;
	uint32_t _currentFrame;

	VkQueue _computeQueue;
	VkImage _computeTexture;
	VkSampler _computeSampler;
	VkSamplerMipmapMode _miplevels;
	VkImageLayout _imageLayout;
	VkImageView _imageView;
	VkDeviceMemory _computeTextureMemory;

	glm::mat4 rotXW(float t) {
		return glm::mat4(
			1.0, 0.0, 0.0, 0.0,
			0.0, cos(t), sin(t), 0.0,
			0.0, - sin(t), cos(t), 0.0,
			0.0, 0.0, 0.0, 1.0
		);
	}
	glm::mat4 rotXY(float t) {
		return glm::mat4(
			1.0, 0.0, 0.0, 0.0,
			0.0, 1.0, 0.0, 0.0,
			0.0, 0.0, cos(t), sin(t),
			0.0, 0.0, - sin(t), cos(t)
		);
	}
	glm::mat4 rotXZ(float t) {
		return glm::mat4(
			1.0, 0.0, 0.0, 0.0,
			0.0, cos(t), 0.0, sin(t),
			0.0, 0.0, 1.0, 0.0,
			0.0, - sin(t), 0.0, cos(t)
		);
	}
	glm::mat4 rotYZ(float t) {
		return glm::mat4(
			cos(t), 0.0, 0.0, sin(t),
			0.0, 1.0, 0.0, 0.0,
			0.0, 0.0, 1.0, 0.0,
			- sin(t), 0.0, 0.0, cos(t)
		);
	}
	glm::mat4 rotYW(float t) {
		return glm::mat4(
			cos(t), 0.0, sin(t), 0.0,
			0.0, 1.0, 0.0, 0.0,
			- sin(t), 0.0, cos(t), 0.0,
			0.0, 0.0, 0.0, 1.0
		);
	}
	glm::mat4 rotZW(float t) {
		return glm::mat4(
			cos(t), sin(t), 0.0, 0.0,
			- sin(t), cos(t), 0.0, 0.0,
			0.0, 0.0, 1.0, 0.0,
			0.0, 0.0, 0.0, 1.0
		);
	}

	float box_size = 1.0f;
	glm::vec4 rotate_sdf(glm::vec4 point)
	{
		glm::mat4 g = rotXW(1.2f) * rotZW(3.0f) * rotYW(-1.3f) * rotXZ(3.78f) * rotXY(2.8f) * rotYZ(0.8f) ;
		return g * point;
	}

	float sdf(glm::vec4 point)
	{
//		float radius = 1.0f;
//		return glm::length(point) - radius;

		glm::vec4 d = abs(rotate_sdf(point)) - glm::vec4(box_size);
		return length(max(d, glm::vec4(0))) + std::min(glm::max(d.x, glm::max(d.y, d.w)), 0.0f);
	}

	glm::vec3 sdf_color(glm::vec4 point)
	{
		float mindist = 99999;
		glm::vec3 color;

		glm::vec4 np = rotate_sdf(point);

		glm::vec3 d = abs(glm::vec3(np.x, np.y, np.z)) - glm::vec3(box_size);
		float f = abs(np.w - box_size);
		float dist = length(max(glm::vec4(d, f), glm::vec4(0))) + std::min(glm::max(d.x, glm::max(d.y, d.z)), 0.0f);
		if (dist < mindist) {
			mindist = dist;
			color = glm::vec3(200 / 255.0f, 0 / 241.0f, 0.0f / 255.0f);
		}

		d = abs(glm::vec3(np.x, np.y, np.z)) - glm::vec3(box_size);
		f = abs(np.w + box_size);
		dist = length(max(glm::vec4(d, f), glm::vec4(0))) + std::min(glm::max(d.x, glm::max(d.y, d.z)), 0.0f);
		if (dist < mindist) {
			mindist = dist;
			color = glm::vec3(255 / 255.0f, 140 / 255.0f, 0.0f / 255.0f);
		}

		d = abs(glm::vec3(np.x, np.y, np.w)) - glm::vec3(box_size);
		f = abs(np.z - box_size);
		dist = length(max(glm::vec4(d, f), glm::vec4(0))) + std::min(glm::max(d.x, glm::max(d.y, d.z)), 0.0f);
		if (dist < mindist) {
			mindist = dist;
			color = glm::vec3(236 / 255.0f, 0 / 255.0f, 140 / 255.0f);
		}

		d = abs(glm::vec3(np.x, np.y, np.w)) - glm::vec3(box_size);
		f = abs(np.z + box_size);
		dist = length(max(glm::vec4(d, f), glm::vec4(0))) + std::min(glm::max(d.x, glm::max(d.y, d.z)), 0.0f);
		if (dist < mindist) {
			mindist = dist;
			color = glm::vec3(104 / 255.0f, 33 / 255.0f, 143 / 255.0f);
		}

		d = abs(glm::vec3(np.x, np.z, np.w)) - glm::vec3(box_size);
		f = abs(np.y - box_size);
		dist = length(max(glm::vec4(d, f), glm::vec4(0))) + std::min(glm::max(d.x, glm::max(d.y, d.z)), 0.0f);
		if (dist < mindist) {
			mindist = dist;
			color = glm::vec3(0 / 255.0f, 178 / 255.0f, 148 / 255.0f);
		}

		d = abs(glm::vec3(np.x, np.z, np.w)) - glm::vec3(box_size);
		f = abs(np.y + box_size);
		dist = length(max(glm::vec4(d, f), glm::vec4(0))) + std::min(glm::max(d.x, glm::max(d.y, d.z)), 0.0f);
		if (dist < mindist) {
			mindist = dist;
			color = glm::vec3(0 / 255.0f, 24 / 255.0f, 143 / 255.0f);
		}

		d = abs(glm::vec3(np.y, np.z, np.w)) - glm::vec3(box_size);
		f = abs(np.x - box_size);
		dist = length(max(glm::vec4(d, f), glm::vec4(0))) + std::min(glm::max(d.x, glm::max(d.y, d.z)), 0.0f);
		if (dist < mindist) {
			mindist = dist;
			color = glm::vec3(0 / 255.0f, 188 / 255.0f, 242 / 255.0f);
		}

		d = abs(glm::vec3(np.y, np.z, np.w)) - glm::vec3(box_size);
		f = abs(np.x + box_size);
		dist = length(max(glm::vec4(d, f), glm::vec4(0))) + std::min(glm::max(d.x, glm::max(d.y, d.z)), 0.0f);
		if (dist < mindist) {
			mindist = dist;
			color = glm::vec3(186 / 255.0f, 216 / 255.0f, 10 / 255.0f);
		}

		return color;
	}

	glm::vec4 getNormal(glm::vec4 p) {
		float d = 0.25f; // epsilon
		float sdp = sdf(p);
		return normalize(glm::vec4(
			sdf(p + glm::vec4(d, 0.0, 0.0, 0.0)) - sdp,
			sdf(p + glm::vec4(0.0, d, 0.0, 0.0)) - sdp,
			sdf(p + glm::vec4(0.0, 0.0, d, 0.0)) - sdp,
			sdf(p + glm::vec4(0.0, 0.0, 0.0, d)) - sdp
		));
	}

	struct cast_res
	{
		bool valid;
		float depth;
		glm::vec4 normal;
		glm::vec3 color;
	};
	cast_res cast_ray(glm::vec4 origin, glm::vec4 direction)
	{
		int max_total_dist = 100;
		int max_dist = 10;
		int max_passes = 100;

		float t = 0;
		bool overshot = false;
		for (int i = 0; i < max_passes; i++) {
			float dist = sdf(origin + direction * t);

			t += dist;

			if (dist <= .0004f) {
				break;
			}

			if (t >= max_total_dist || dist >= max_dist || i == max_passes - 1) {
				overshot = true;
			}

		}


		cast_res res;
		if (!overshot) res.normal = getNormal(origin + direction * t);
//		if (!overshot) res.normal = glm::vec3(0, 1, 0);
		if (!overshot) res.color = sdf_color(origin + direction * t);
		res.valid = !overshot;
		res.depth = t;
		return res;
	}

	glm::vec4 normalize_corr(glm::vec4 in)
	{
		float len = glm::sqrt(in.x * in.x + in.y * in.y + in.z * in.z + in.w * in.w);
		return in / len;
	}

	void createOctreeBuffer()
	{

		glm::vec4 position = glm::vec4(0, 0, 0, 2.22f);
		glm::vec4 direction = glm::vec4(0, 0, 0, -0.32f);
		float width = 0.02f;
		float height = 0.02f;
		float depth = 0.02f;

		int size = 64;
		OctreeNode::octree_data *marchdata;
		marchdata = static_cast<OctreeNode::octree_data *>(malloc(sizeof(OctreeNode::octree_data) * size * size * size));
		for (int x = 0; x < size; x++) {
			for (int y = 0; y < size; y++) {
				for (int z = 0; z < size; z++) {

					glm::vec4 p = position + glm::vec4(
						((float)x / size - 0.5f + (1.0f / size) / 2.0f) * width + position.x,
						((float)y / size - 0.5f + (1.0f / size) / 2.0f) * height + position.y,
						((float)z / size - 0.5f + (1.0f / size) / 2.0f) * depth + position.z,
						0
					);
					glm::vec4 n = direction + glm::vec4(
						((float)x / size - 0.5f + (1.0f / size) / 2.0f),
						((float)y / size - 0.5f + (1.0f / size) / 2.0f),
						((float)z / size - 0.5f + (1.0f / size) / 2.0f),
						0
					);
					n = normalize_corr(n);
					cast_res cres = cast_ray(p, n);
//					if (cres.valid) marchdata[x + y * size + z * size * size] = glm::vec3(cres.depth / 1.2f);
					OctreeNode::octree_data cdata{
						glm::vec3(-1),
						-1
					};
					if (cres.valid) {
						cdata.color = cres.color;
						cdata.depth = cres.depth;
					}
					marchdata[x + y * size + z * size * size] = cdata;
//					if (cres.valid) marchdata[x + y * size + z * size * size] = cres.normal;
//					else marchdata[x + y * size + z * size * size] = glm::vec3(-1);
				}
			}
		}

		OctreeNode root = OctreeNode::convert_array(marchdata, size, glm::vec3(0), glm::vec3(size));
		root.cleanup_empty_nodes();
//		root.compact_nodes();
		// convert to oc_node struct

		std::vector<oc_node> nodes;
		std::vector<int> sectionstack;
		std::vector<int> indexstack;
		int index = 0;
		OctreeNode node = root;
		nodes.push_back({0, {0, 0, 0, 0, 0, 0, 0, 0}, 0, node.getMetadata().depth, 0, glm::vec4(node.getMetadata().color, 1)});
		int section = 0;
		while(true){

			if (!node.hasChildren()) {
				nodes.back().leaf = 1;
				if (node.hasParent()) {
					node = *node.getParent();
					section = sectionstack.back() + 1;
					sectionstack.pop_back();
					index = indexstack.back();
					indexstack.pop_back();
					continue;
				} else {
					break;
				}
			}

			bool godown = false;
			for(; section < 8; section++) {
				if (node.getChildren()[section] != nullptr) {
					godown = true;

					int newindex = nodes.size();
					nodes.at(index).child[section] = newindex;
					indexstack.push_back(index);
					index = newindex;
					node = *node.getChildren()[section];
					nodes.push_back({index, {0, 0, 0, 0, 0, 0, 0, 0}, node.isLeaf()? 1:0, node.getMetadata().depth, 0, glm::vec4(node.getMetadata().color,0)});
					sectionstack.push_back(section);
					section = 0;
					break;
				}
			}
			if (godown) continue;

			// go up
			if (sectionstack.empty()) break;
			section = sectionstack.back() + 1;
			sectionstack.pop_back();
			index = indexstack.back();
			indexstack.pop_back();
			node = *node.getParent();
		}

		_bufferSize = sizeof(oc_node) * nodes.size();
		_device.
			createBuffer(
			_bufferSize,
			VK_BUFFER_USAGE_STORAGE_BUFFER_BIT,
			// Allow the host to write to the memory and keep both values consistent with each other
			VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT
				| VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
			_buffer,
			_bufferMemory);

		// Temporarily map the region of the host memory to the device memory and upload the marchdata
		void *data;
		vkMapMemory(_device
						.
							getVkDevice(), _bufferMemory,
					0, _bufferSize, 0, &data);
		memcpy(data, nodes.data(),
			   static_cast
				   <size_t>(_bufferSize)
		);
		vkUnmapMemory(_device
						  .
							  getVkDevice(), _bufferMemory
		);
	}

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
			.createImageWithInfo(imageInfo, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, _computeTexture,
								 _computeTextureMemory);

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
		if (vkCreateSampler(_device.getVkDevice(), &sampler, nullptr, &_computeSampler) != VK_SUCCESS) {
			throw std::runtime_error("Failed to create texture");
		}

		// Create image view
		VkImageViewCreateInfo view = vks::initializers::imageViewCreateInfo();
		view.image = VK_NULL_HANDLE;
		view.viewType = VK_IMAGE_VIEW_TYPE_2D;
		view.format = format;
		view.components = {VK_COMPONENT_SWIZZLE_R, VK_COMPONENT_SWIZZLE_G, VK_COMPONENT_SWIZZLE_B,
						   VK_COMPONENT_SWIZZLE_A};
		view.subresourceRange = {VK_IMAGE_ASPECT_COLOR_BIT, 0, 1, 0, 1};
		view.image = _computeTexture;
		if (vkCreateImageView(_device.getVkDevice(), &view, nullptr, &_imageView) != VK_SUCCESS) {
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
			VK_SUCCESS) {
			throw std::runtime_error("failed to create pipeline layout!");
		}

	}

/**
 * Stores the data that will be accessible to the descriptor set
 */
	void createDescriptorSetLayout()
	{

		VkDescriptorSetLayoutBinding binding[2];
		binding[0].binding = 0;
		binding[0].descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_IMAGE;
		binding[0].descriptorCount = 1;
		binding[0].stageFlags = VK_SHADER_STAGE_COMPUTE_BIT;

		binding[1].binding = 1;
		binding[1].descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
		binding[1].descriptorCount = 1;
		binding[1].stageFlags = VK_SHADER_STAGE_COMPUTE_BIT;

		VkDescriptorSetLayoutCreateInfo layoutInfo{};
		layoutInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
		layoutInfo.bindingCount = 2;
		layoutInfo.pBindings = binding;

		if (vkCreateDescriptorSetLayout(_device.getVkDevice(), &layoutInfo, nullptr, &descriptorSetLayout) !=
			VK_SUCCESS) {
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

		if (vkAllocateDescriptorSets(_device.getVkDevice(), &allocInfo, &_descriptorSet) != VK_SUCCESS) {
			throw std::runtime_error("failed to allocate descriptor sets!");
		}

		// Allocate storage and image buffers
		VkDescriptorImageInfo imageInfo{};
		imageInfo.imageView = _imageView;
		imageInfo.sampler = _computeSampler;
		imageInfo.imageLayout = _imageLayout;

		VkDescriptorBufferInfo bufferInfo{};
		bufferInfo.buffer = _buffer;
		bufferInfo.offset = 0;
		bufferInfo.range = _bufferSize;

		std::array<VkWriteDescriptorSet, 2> descriptorWrites{};

		descriptorWrites[0].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
		descriptorWrites[0].dstSet = _descriptorSet; // Write to this descriptor set
		descriptorWrites[0].dstBinding = 0; // Write to the first and only binding
		descriptorWrites[0].dstArrayElement = 0;
		descriptorWrites[0].descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_IMAGE;
		descriptorWrites[0].descriptorCount = 1; // Update a single descriptor
		descriptorWrites[0].pImageInfo = &imageInfo; // Optional
		descriptorWrites[0].pTexelBufferView = nullptr; // Optional

		descriptorWrites[1].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
		descriptorWrites[1].dstSet = _descriptorSet;
		descriptorWrites[1].dstBinding = 1;
		descriptorWrites[1].dstArrayElement = 0;
		descriptorWrites[1].descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
		descriptorWrites[1].descriptorCount = 1;
		descriptorWrites[1].pBufferInfo = &bufferInfo;

		vkUpdateDescriptorSets(_device.getVkDevice(), descriptorWrites.size(), descriptorWrites.data(), 0, nullptr);
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
		shaderStageCreateInfo.module = _device.createShaderModule(VksUtil::readFile("shaders/octree.comp.spv"));
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
			!= VK_SUCCESS) {
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
			NULL, &pipeline) != VK_SUCCESS) {
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
