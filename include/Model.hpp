#pragma once

#define GLM_FORCE_RADIANS
#include <glm/glm.hpp>

#include <vulkan/vulkan.h>

#include <vector>

#include "Vertex.hpp"
#include "UBO.hpp"
#include "BufferUtils.hpp"
#include "AppHandler.hpp"


//TODO: model struct, function inside class
struct Model
{
    VkBuffer vertexBuffer;
    VkDeviceMemory vertexBufferMemory;
    VkBuffer indexBuffer;
    VkDeviceMemory indexBufferMemory;

    void create(std::vector<Vertex> vertices, std::vector<uint16_t> indices,
        const DeviceHandler devh, const VkQueue trsfrtQueue, const VkCommandPool commandPool);

    void destroy(const VkDevice dev);
};