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

namespace std
{
    template<>
    struct hash<vks::VksModel::Vertex>
    {
        size_t operator()(vks::VksModel::Vertex const &vertex) const
        {
            return ((hash<glm::vec3>()(vertex.position) ^ (hash<glm::vec3>()(vertex.color) << 1)) >> 1) ^
                   (hash<glm::vec2>()(vertex.uv) << 1) ^ (hash<glm::vec3>()(vertex.normal) << 1) ^
                   (hash<glm::vec3>()(vertex.tangent) << 1);
        }
    };
}  // namespace std

namespace vks
{
    VksModel::VksModel(VksDevice &device, Builder &builder) : device_{device}
    {
        createVertexBuffer(builder);
        createIndexBuffer(builder);
    }

    void VksModel::createVertexBuffer(Builder &builder)
    {
        auto &vertices = builder.vertices;
        vertexCount_ = static_cast<uint32_t>(vertices.size());
        assert(vertexCount_ >= 3 && "Vertex count must be at least 3");
        VkDeviceSize bufferSize = sizeof(vertices[0]) * vertexCount_;
        VkBuffer stagingBuffer;
        VkDeviceMemory stagingBufferMemory;
        device_.createBuffer(
                bufferSize,
                VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
                VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
                stagingBuffer,
                stagingBufferMemory);

        void *data;
        vkMapMemory(device_.getVkDevice(), stagingBufferMemory, 0, bufferSize, 0, &data);
        memcpy(data, vertices.data(), (size_t) bufferSize);
        vkUnmapMemory(device_.getVkDevice(), stagingBufferMemory);

        device_.createBuffer(
                bufferSize,
                VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT,
                VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
                vertexBuffer_,
                vertexBufferMemory_);

        device_.copyBuffer(stagingBuffer, vertexBuffer_, bufferSize);

        vkDestroyBuffer(device_.getVkDevice(), stagingBuffer, nullptr);
        vkFreeMemory(device_.getVkDevice(), stagingBufferMemory, nullptr);
    }

    void VksModel::createIndexBuffer(Builder &builder)
    {
        auto &indices = builder.indices;
        indexCount_ = static_cast<uint32_t>(indices.size());
        VkDeviceSize bufferSize = sizeof(indices[0]) * indexCount_;

        VkBuffer stagingBuffer;
        VkDeviceMemory stagingBufferMemory;

        device_.createBuffer(
                bufferSize,
                VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
                VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
                stagingBuffer,
                stagingBufferMemory);

        void *data;
        vkMapMemory(device_.getVkDevice(), stagingBufferMemory, 0, bufferSize, 0, &data);
        memcpy(data, indices.data(), (size_t) bufferSize);
        vkUnmapMemory(device_.getVkDevice(), stagingBufferMemory);

        device_.createBuffer(
                bufferSize,
                VK_BUFFER_USAGE_INDEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT,
                VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
                indexBuffer_,
                indexBufferMemory_);

        device_.copyBuffer(stagingBuffer, indexBuffer_, bufferSize);
        vkDestroyBuffer(device_.getVkDevice(), stagingBuffer, nullptr);
        vkFreeMemory(device_.getVkDevice(), stagingBufferMemory, nullptr);
    }

    void VksModel::cleanup()
    {
        vkDestroyBuffer(device_.getVkDevice(), vertexBuffer_, nullptr);
        vkFreeMemory(device_.getVkDevice(), vertexBufferMemory_, nullptr);

        vkDestroyBuffer(device_.getVkDevice(), indexBuffer_, nullptr);
        vkFreeMemory(device_.getVkDevice(), indexBufferMemory_, nullptr);
    }

    std::unique_ptr<VksModel> VksModel::loadModelFromFile(VksDevice &deviceRef, std::string filepath)
    {
        Builder builder{};
        builder.loadModel(filepath);
        std::unique_ptr<VksModel> model = std::make_unique<VksModel>(deviceRef, builder);
        return model;
    }

    void VksModel::draw(VkCommandBuffer commandBuffer)
    {
        vkCmdDrawIndexed(commandBuffer, indexCount_, 1, 0, 0, 0);
    }

    void VksModel::bind(VkCommandBuffer commandBuffer)
    {
        VkBuffer buffers[] = {vertexBuffer_};
        VkDeviceSize offsets[] = {0};
        vkCmdBindVertexBuffers(commandBuffer, 0, 1, buffers, offsets);
        vkCmdBindIndexBuffer(commandBuffer, indexBuffer_, 0, VK_INDEX_TYPE_UINT32);
    }

    struct Vertex
    {
        glm::vec3 position;
        glm::vec3 normal;
        glm::vec4 color;
        glm::vec2 uv;
        glm::vec4 tangent;  // w component is -1 or 1 and indicates handedness of the tangent basis

        static VkVertexInputBindingDescription getBindingDescription()
        {
            VkVertexInputBindingDescription bindingDescription{};
            bindingDescription.binding = 0;
            bindingDescription.stride = sizeof(Vertex);
            bindingDescription.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;
            return bindingDescription;
        }

        static std::vector<VkVertexInputAttributeDescription> getAttributeDescriptions(
                const std::vector<VksModel::VertexAttribute> &attributes);

        bool operator==(const Vertex &other) const
        {
            return position == other.position && color == other.color && uv == other.uv &&
                   normal == other.normal && tangent == other.tangent;
        }
    };

    void VksModel::Builder::loadModel(std::string filepath)
    {
        tinyobj::attrib_t attrib;
        std::vector<tinyobj::shape_t> shapes;
        std::vector<tinyobj::material_t> materials;
        std::string warn, err;

        if (!tinyobj::LoadObj(&attrib, &shapes, &materials, &warn, &err, filepath.c_str()))
        {
            throw std::runtime_error(warn + err);
        }

        vertices.clear();
        indices.clear();
        std::unordered_map<Vertex, uint32_t> uniqueVertices{};
        for (const auto &shape : shapes)
        {
            int count = 0;
            for (const auto &index : shape.mesh.indices)
            {
                Vertex vertex{};

                if (index.vertex_index >= 0)
                {
                    vertex.position = {
                            attrib.vertices[3 * index.vertex_index + 0],
                            attrib.vertices[3 * index.vertex_index + 1],
                            attrib.vertices[3 * index.vertex_index + 2]};
                }

                if (index.texcoord_index >= 0)
                {
                    vertex.uv = {
                            attrib.texcoords[2 * index.texcoord_index + 0],
                            1.0f - attrib.texcoords[2 * index.texcoord_index + 1]};
                }

                vertex.color = {1.0f, 1.0f, 1.0f, 1.0f};

                if (index.normal_index >= 0)
                {
                    vertex.normal = {
                            attrib.normals[3 * index.normal_index + 0],
                            attrib.normals[3 * index.normal_index + 1],
                            attrib.normals[3 * index.normal_index + 2]};
                }

                if (uniqueVertices.count(vertex) == 0)
                {
                    uniqueVertices[vertex] = static_cast<uint32_t>(vertices.size());
                    vertices.push_back(vertex);
                }
                indices.push_back(uniqueVertices[vertex]);
            }
        }
    }
}  // namespace lve