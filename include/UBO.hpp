#pragma once
#define GLM_FORCE_RADIANS
#include <glm/glm.hpp>

#include "vulkan/vulkan.h"

#include <array>

#include "BufferUtils.hpp"
#include "AppHandler.hpp"
#include "../Settings.hpp"

struct UBO_buffmem {
    std::array<VkBuffer, MAX_FRAMES_IN_FLIGHT> uniformBuffers;
    std::array<VkDeviceMemory, MAX_FRAMES_IN_FLIGHT> uniformBuffersMemory;
    std::array<void*, MAX_FRAMES_IN_FLIGHT> uniformBuffersMapped;
};

template<unsigned char size>
struct UBOTEST
{
    void* pData;

    constexpr VkBuffer* uniformBuffers = pData;
    constexpr VkDeviceMemory* uniformBuffersMemory = pData + size * sizeof(VkBuffer);
    constexpr void* uniformBuffersMapped = pData + size * (sizeof(VkBuffer) + sizeof(VkDeviceMemory));
};

template<class UBOClass>
_NODISCARD UBO_buffmem createUBO(DeviceHandler devh)
{
    UBO_buffmem r{};

    VkDeviceSize bufferSize = sizeof(UBOClass);

    for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
        createBuffer(bufferSize, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, r.uniformBuffers[i], r.uniformBuffersMemory[i], devh);

        vkMapMemory(devh.device, r.uniformBuffersMemory[i], 0, bufferSize, 0, &(r.uniformBuffersMapped[i]) );
    }

    return r;
}

//free buffer and memory
inline void destroyUBO(UBO_buffmem& ub, VkDevice device)
{
    for (size_t i = 0; i < ub.uniformBuffers.size(); i++) {
        vkDestroyBuffer(device, ub.uniformBuffers[i], nullptr);
        vkFreeMemory(device, ub.uniformBuffersMemory[i], nullptr);
    }
}