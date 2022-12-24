#pragma once
#define GLM_FORCE_RADIANS
#include <glm/glm.hpp>

#include "vulkan/vulkan.h"

#include "AppHandler.hpp"

struct UniformBufferObject {
    alignas(16) glm::mat4 model;
    alignas(16) glm::mat4 view;
    alignas(16) glm::mat4 proj;
};

struct UBO_inst {
    std::vector<VkBuffer> uniformBuffers;
    std::vector<VkDeviceMemory> uniformBuffersMemory;
    std::vector<void*> uniformBuffersMapped;
};

//TODO: size of vector -> 1 per frame -> global stuff? how to cleanly propagate this info
template<class UBOClass>
_NODISCARD UBO_inst createUBO(DeviceHandler devh)
{
    UBO_inst r{};

    VkDeviceSize bufferSize = sizeof(UBOClass);

    r.uniformBuffers.resize(MAX_FRAMES_IN_FLIGHT);
    r.uniformBuffersMemory.resize(MAX_FRAMES_IN_FLIGHT);
    r.uniformBuffersMapped.resize(MAX_FRAMES_IN_FLIGHT);

    for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
        createBuffer(bufferSize, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, r.uniformBuffers[i], r.uniformBuffersMemory[i], devh);

        vkMapMemory(devh.device, r.uniformBuffersMemory[i], 0, bufferSize, 0, &(r.uniformBuffersMapped[i]) );
    }

    return r;
}

inline void destroyUBO(UBO_inst& ub, VkDevice device)
{
    for (size_t i = 0; i < ub.uniformBuffers.size(); i++) {
        vkDestroyBuffer(device, ub.uniformBuffers[i], nullptr);
        vkFreeMemory(device, ub.uniformBuffersMemory[i], nullptr);
    }

    ub.uniformBuffers.clear();
    ub.uniformBuffersMapped.clear();
    ub.uniformBuffersMemory.clear();
}