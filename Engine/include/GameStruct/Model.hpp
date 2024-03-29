#pragma once

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/glm.hpp>

#include <tinygltf/tiny_gltf.h>

#include <tinyobjloader/tiny_obj_loader.h>

#include <vulkan/vulkan.h>

#include <assert.h>
#include <vector>

#include "Vertex.hpp"
#include "../EngineBuildingBlock/UBO.hpp"
#include "../EngineBuildingBlock/AppHandler.hpp"
#include "../Utils/BufferUtils.hpp"

#include "MacroGlobal.hpp"

struct Model
{
    VkBuffer vertexBuffer;
    VkDeviceMemory vertexBufferMemory;
    VkBuffer indexBuffer;
    VkDeviceMemory indexBufferMemory;

    template<class Vert>
    void create(std::vector<Vert> vertices, std::vector<uint16_t> indices,
        const DeviceHandler devh, const VkQueue trsfrtQueue, const VkCommandPool commandPool)
    {
        //vertex
        VkDeviceSize bufferSize = sizeof(vertices[0]) * vertices.size();

        VkBuffer stagingBuffer;
        VkDeviceMemory stagingBufferMemory;
        createBuffer(bufferSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
            VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
            stagingBuffer, stagingBufferMemory, devh);

        void* data;
        vkMapMemory(devh.device, stagingBufferMemory, 0, bufferSize, 0, &data);
        memcpy(data, vertices.data(), (size_t)bufferSize);
        vkUnmapMemory(devh.device, stagingBufferMemory);

        createBuffer(bufferSize, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT,
            VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, vertexBuffer, vertexBufferMemory, devh);

        copyBuffer(stagingBuffer, vertexBuffer, bufferSize, devh.device, trsfrtQueue, commandPool);

        vkDestroyBuffer(devh.device, stagingBuffer, nullptr);
        vkFreeMemory(devh.device, stagingBufferMemory, nullptr);

        //Indices

        bufferSize = sizeof(indices[0]) * indices.size();

        createBuffer(bufferSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
            stagingBuffer, stagingBufferMemory, devh);

        vkMapMemory(devh.device, stagingBufferMemory, 0, bufferSize, 0, &data);
        memcpy(data, indices.data(), (size_t)bufferSize);
        vkUnmapMemory(devh.device, stagingBufferMemory);

        createBuffer(bufferSize, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_INDEX_BUFFER_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
            indexBuffer, indexBufferMemory, devh);

        copyBuffer(stagingBuffer, indexBuffer, bufferSize, devh.device, trsfrtQueue, commandPool);

        vkDestroyBuffer(devh.device, stagingBuffer, nullptr);
        vkFreeMemory(devh.device, stagingBufferMemory, nullptr);

    }

    void destroy(const VkDevice dev);
};

bool LoadModelMeshGeometry(Model& toLoad, VkPipelineVertexInputStateCreateInfo& vertexInputInfo, VkPipelineInputAssemblyStateCreateInfo& inputAssembly,
    const tinygltf::Model& model, const tinygltf::Mesh& mesh,
    DeviceHandler devh, VkQueue trsfrtQueue, VkCommandPool commandPool);

bool loadModelFromObj(Model& toLoad, const std::string& MODEL_PATH,
    VkVertexInputBindingDescription& inputBindingDescriptor, std::vector<VkVertexInputAttributeDescription>& InputAttrDesc,
    DeviceHandler devh, VkQueue trsfrtQueue, VkCommandPool commandPool);
