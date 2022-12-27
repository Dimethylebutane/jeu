#pragma once
#include <vulkan/vulkan.h>

#include <stdexcept>

#include "../EngineBuildingBlock/AppHandler.hpp"


uint32_t findMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties, const VkPhysicalDevice physicalDevice);

VkCommandBuffer beginSingleTimeCommands(const VkDevice device, const VkCommandPool commandPool);

void endSingleTimeCommands(VkCommandBuffer commandBuffer, const VkDevice device, const VkQueue queue, const VkCommandPool commandPool);

void copyBuffer(VkBuffer srcBuffer, VkBuffer dstBuffer, VkDeviceSize size,
    const VkDevice device, const VkQueue TrsfrtQueue, const VkCommandPool commandPool);

void createBuffer(VkDeviceSize size, VkBufferUsageFlags usage,
    VkMemoryPropertyFlags properties, VkBuffer& buffer, VkDeviceMemory& bufferMemory,
    const DeviceHandler devh);