/*
*
* LveModel Class
*
* Little vulkan engine model class
*
* Copyright (C) 2020 by Blurrypiano - https://github.com/blurrypiano/littleVulkanEngine
*
* This code is licensed under the MIT license (MIT) (http://opensource.org/licenses/MIT)
 */

#ifndef VULKANENGINE_VKS_MODEL_H
#define VULKANENGINE_VKS_MODEL_H


#pragma once

#include "vks_device.h"

// libs
#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE  // open gl uses -1 to 1, vk is 0 to 1

#include <glm/glm.hpp>

// std
#include <memory>
#include <vector>

namespace vks
{

class VksModel
{
public:
    enum class VertexAttribute
    {
        POSITION
    };

    struct Vertex
    {
        glm::vec2 position;
        //            glm::vec3 normal;
        //            glm::vec4 color;
        //            glm::vec2 uv;
        //            glm::vec4 tangent;  // w component is -1 or 1 and indicates handedness of the tangent basis

        static std::vector<VkVertexInputBindingDescription> getBindingDescriptions()
        {
            std::vector<VkVertexInputBindingDescription> bindingDescriptions(1);
            bindingDescriptions[0].binding = 0;
            bindingDescriptions[0].stride = sizeof(Vertex); // Bytes to advance per vertex
            bindingDescriptions[0].inputRate = VK_VERTEX_INPUT_RATE_VERTEX;
            return bindingDescriptions;
        }

        static std::vector<VkVertexInputAttributeDescription> getAttributesDescriptions()
        {
            std::vector<VkVertexInputAttributeDescription> attributeDescriptions(1);
            attributeDescriptions[0].binding = 0;
            attributeDescriptions[0].location = 0; // Location in the shader
            attributeDescriptions[0].format = VK_FORMAT_R32G32_SFLOAT;
            attributeDescriptions[0].offset = 0;
            return attributeDescriptions;
        }

        //            bool operator==(const Vertex &other) const
        //            {
        //                return position == other.position && color == other.color && uv == other.uv &&
        //                       normal == other.normal && tangent == other.tangent;
        //            }
    };

    //        class Builder
    //        {
    //        public:
    //            std::vector<Vertex> vertices{};
    //            std::vector<uint32_t> indices{};
    //
    //            void loadModel(std::string filepath);
    //        };

    //        VksModel(VksDevice &_device, Builder &builder);
    VksModel(VksDevice &device, std::vector<Vertex> &vertices);

    ~VksModel()
    { cleanup(); }

    //        VksModel(const VksModel &) = delete;
    //
    //        void operator=(const VksModel &) = delete;
    //
    //        void draw(VkCommandBuffer commandBuffer);
    //
    //        void bind(VkCommandBuffer commandBuffer);

    //        void swapChainReset()
    //        {};

    void bind(VkCommandBuffer commandBuffer);
    void draw(VkCommandBuffer commandBuffer);

    //        static std::unique_ptr<VksModel> loadModelFromFile(VksDevice &_device, std::string filepath);

private:
    VksDevice &_device;

    VkBuffer _vertexBuffer;
    VkDeviceMemory _vertexBufferMemory;
    uint32_t _vertexCount;

    VkBuffer _indexBuffer;
    VkDeviceMemory _indexBufferMemory;
    uint32_t indexCount;

    //        void createVertexBuffer(Builder &builder);
    void createVertexBuffer(const std::vector<Vertex> &vertices);

    //void createIndexBuffer(Builder &builder);
    //void createIndexBuffer(const std::

    void cleanup();
};
}  // namespace lve


#endif //VULKANENGINE_VKS_MODEL_H
