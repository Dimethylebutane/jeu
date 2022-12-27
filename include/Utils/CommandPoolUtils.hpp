#pragma once
#include <vulkan/vulkan.h>

#include <stdexcept>

#include "QueueUtils.hpp"
#include "../EngineBuildingBlock/AppHandler.hpp"

_NODISCARD  VkCommandPool createCommandPool(const DeviceHandler devh, const VkSurfaceKHR surf)
{
    VkCommandPool r{};

    QueueFamilyIndices queueFamilyIndices = findQueueFamilies(devh.physicalDevice, surf);

    VkCommandPoolCreateInfo poolInfo{};
    poolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
    poolInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
    poolInfo.queueFamilyIndex = queueFamilyIndices.graphicsFamily.value();

    if (vkCreateCommandPool(devh.device, &poolInfo, nullptr, &r) != VK_SUCCESS) {
        throw std::runtime_error("failed to create graphics command pool!");
    }

    return r;
}