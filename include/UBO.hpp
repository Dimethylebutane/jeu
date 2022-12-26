#pragma once
#define GLM_FORCE_RADIANS
#include <glm/glm.hpp>

#include "vulkan/vulkan.h"

#include <array>

#include "BufferUtils.hpp"
#include "AppHandler.hpp"
#include "../Settings.hpp"

/*
struct UBO_buffmem {
    std::array<VkBuffer, MAX_FRAMES_IN_FLIGHT> uniformBuffers;
    std::array<VkDeviceMemory, MAX_FRAMES_IN_FLIGHT> uniformBuffersMemory;
    std::array<void*, MAX_FRAMES_IN_FLIGHT> uniformBuffersMapped;
};*/

//ensure that data are at the same place in memory so it ease data access
struct UBO_buffmem
{
    char* pData;
    [readonly] unsigned char numberOfFrame;

    VkBuffer* uniformBuffers() { return reinterpret_cast<VkBuffer*>(pData); }
    VkDeviceMemory* uniformBuffersMemory() { return  reinterpret_cast<VkDeviceMemory*>(pData + numberOfFrame * sizeof(VkBuffer)); }
    void** uniformBuffersMapped() { return reinterpret_cast<void**>(pData + numberOfFrame * (sizeof(VkBuffer) + sizeof(VkDeviceMemory)) ); }

    size_t size()
    {
        return numberOfFrame * (sizeof(VkBuffer) + sizeof(VkDeviceMemory) + sizeof(void*));
    }
};

template<class UBOClass>
_NODISCARD UBO_buffmem createUBO(const unsigned char NumberOfFrame, const DeviceHandler devh)
{
    UBO_buffmem r{nullptr, NumberOfFrame};
    r.pData = new char[r.size()]; //allocate memory

    VkDeviceSize bufferSize = sizeof(UBOClass);

    for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
        createBuffer(bufferSize, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, r.uniformBuffers()[i], r.uniformBuffersMemory()[i], devh);

        vkMapMemory(devh.device, r.uniformBuffersMemory()[i], 0, bufferSize, 0, &(r.uniformBuffersMapped()[i]) );
    }

    return r;
}

//free buffer and memory
inline void destroyUBO(UBO_buffmem ub, const VkDevice device)
{
    for (size_t i = 0; i < ub.numberOfFrame; i++) {
        vkDestroyBuffer(device, ub.uniformBuffers()[i], nullptr);
        vkFreeMemory(device, ub.uniformBuffersMemory()[i], nullptr);
    }

    delete[] ub.pData; //free memory
}