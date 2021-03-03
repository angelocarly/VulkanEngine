//
// Created by magnias on 04/02/2021.
//

#include "vks_model.h"

// lib headers
#define TINYOBJLOADER_IMPLEMENTATION

#include <tiny_obj_loader.h>

#define GLM_ENABLE_EXPERIMENTAL

#include <glm/gtx/hash.hpp>

// std
#include <cassert>
#include <cstring>
#include <iostream>
#include <stdexcept>
#include <unordered_map>
#include <unordered_set>

namespace vks
{
//    VksModel::VksModel(VksDevice &_device, Builder &builder) : device_{_device}
//    {
//        createVertexBuffer(builder);
//        createIndexBuffer(builder);
//    }

    VksModel::VksModel(VksDevice &device, std::vector<Vertex> &vertices) : _device{device}
    {
        createVertexBuffer(vertices);
    }

    void VksModel::cleanup()
    {
        vkDestroyBuffer(_device.getVkDevice(), _vertexBuffer, nullptr);
        vkFreeMemory(_device.getVkDevice(), _vertexBufferMemory, nullptr);

        //vkDestroyBuffer(_device.getVkDevice(), _indexBuffer, nullptr);
        //vkFreeMemory(_device.getVkDevice(), _indexBufferMemory, nullptr);
    }

    void VksModel::createVertexBuffer(const std::vector<Vertex> &vertices)
    {
        _vertexCount = static_cast<uint32_t>(vertices.size());
        assert(_vertexCount >= 3 && "Vertexcount must be at least 3");
        VkDeviceSize bufferSize = sizeof(vertices[0]) * _vertexCount;
        _device.createBuffer(
                bufferSize,
                VK_BUFFER_USAGE_VERTEX_BUFFER_BIT,
                // Allow the host to write to the memory and keep both values consistent with each other
                VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
                _vertexBuffer,
                _vertexBufferMemory);

        // Temporarily map the region of the host memory to the device memory and upload the data
        void* data;
        vkMapMemory(_device.getVkDevice(), _vertexBufferMemory, 0, bufferSize, 0,&data);
        memcpy(data, vertices.data(), static_cast<size_t>(bufferSize));
        vkUnmapMemory(_device.getVkDevice(), _vertexBufferMemory);
    }

    /**
     *
     * @param commandBuffer
     */
    void VksModel::bind(VkCommandBuffer commandBuffer)
    {
        VkBuffer buffers[] = {_vertexBuffer};
        VkDeviceSize offset[] = {0};
        vkCmdBindVertexBuffers(commandBuffer, 0, 1, buffers, offset);
    }

    void VksModel::draw(VkCommandBuffer commandBuffer)
    {
        vkCmdDraw(commandBuffer, _vertexCount, 1, 0, 0);
    }

//    void VksModel::createVertexBuffer(Builder &builder)
//    {
//        auto &vertices = builder.vertices;
//        vertexCount_ = static_cast<uint32_t>(vertices.size());
//        assert(vertexCount_ >= 3 && "Vertex count must be at least 3");
//        VkDeviceSize bufferSize = sizeof(vertices[0]) * vertexCount_;
//        VkBuffer stagingBuffer;
//        VkDeviceMemory stagingBufferMemory;
//        device_.createBuffer(
//                bufferSize,
//                VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
//                VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
//                stagingBuffer,
//                stagingBufferMemory);
//
//        void *data;
//        vkMapMemory(device_.getVkDevice(), stagingBufferMemory, 0, bufferSize, 0, &data);
//        memcpy(data, vertices.data(), (size_t) bufferSize);
//        vkUnmapMemory(device_.getVkDevice(), stagingBufferMemory);
//
//        device_.createBuffer(
//                bufferSize,
//                VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT,
//                VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
//                vertexBuffer_,
//                vertexBufferMemory_);
//
//        device_.copyBuffer(stagingBuffer, vertexBuffer_, bufferSize);
//
//        vkDestroyBuffer(device_.getVkDevice(), stagingBuffer, nullptr);
//        vkFreeMemory(device_.getVkDevice(), stagingBufferMemory, nullptr);
//    }
//
//    void VksModel::createIndexBuffer(Builder &builder)
//    {
//        auto &indices = builder.indices;
//        indexCount_ = static_cast<uint32_t>(indices.size());
//        VkDeviceSize bufferSize = sizeof(indices[0]) * indexCount_;
//
//        VkBuffer stagingBuffer;
//        VkDeviceMemory stagingBufferMemory;
//
//        device_.createBuffer(
//                bufferSize,
//                VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
//                VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
//                stagingBuffer,
//                stagingBufferMemory);
//
//        void *data;
//        vkMapMemory(device_.getVkDevice(), stagingBufferMemory, 0, bufferSize, 0, &data);
//        memcpy(data, indices.data(), (size_t) bufferSize);
//        vkUnmapMemory(device_.getVkDevice(), stagingBufferMemory);
//
//        device_.createBuffer(
//                bufferSize,
//                VK_BUFFER_USAGE_INDEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT,
//                VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
//                indexBuffer_,
//                indexBufferMemory_);
//
//        device_.copyBuffer(stagingBuffer, indexBuffer_, bufferSize);
//        vkDestroyBuffer(device_.getVkDevice(), stagingBuffer, nullptr);
//        vkFreeMemory(device_.getVkDevice(), stagingBufferMemory, nullptr);
//    }
//
//    void VksModel::cleanup()
//    {
//        vkDestroyBuffer(device_.getVkDevice(), vertexBuffer_, nullptr);
//        vkFreeMemory(device_.getVkDevice(), vertexBufferMemory_, nullptr);
//
//        vkDestroyBuffer(device_.getVkDevice(), indexBuffer_, nullptr);
//        vkFreeMemory(device_.getVkDevice(), indexBufferMemory_, nullptr);
//    }
//
//    std::unique_ptr<VksModel> VksModel::loadModelFromFile(VksDevice &deviceRef, std::string filepath)
//    {
//        Builder builder{};
//        builder.loadModel(filepath);
//        std::unique_ptr<VksModel> model = std::make_unique<VksModel>(deviceRef, builder);
//        return model;
//    }
//
//    void VksModel::draw(VkCommandBuffer commandBuffer)
//    {
//        vkCmdDrawIndexed(commandBuffer, indexCount_, 1, 0, 0, 0);
//    }
//
//    void VksModel::bind(VkCommandBuffer commandBuffer)
//    {
//        VkBuffer buffers[] = {vertexBuffer_};
//        VkDeviceSize offsets[] = {0};
//        vkCmdBindVertexBuffers(commandBuffer, 0, 1, buffers, offsets);
//        vkCmdBindIndexBuffer(commandBuffer, indexBuffer_, 0, VK_INDEX_TYPE_UINT32);
//    }

}  // namespace lve
