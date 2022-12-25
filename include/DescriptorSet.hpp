#pragma once
#include "vulkan/vulkan.h"

#include <array>
#include <stdexcept>

#include "../Settings.hpp" //MAX_FRAME_IN_FLIGHT
#include "UBO.hpp"

//need to be destroy
_NODISCARD VkDescriptorPool createDescriptorPool(VkDevice device,
    VkDescriptorType type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
    VkDescriptorPoolCreateFlags flags = 0);

//need to be destroy
_NODISCARD VkDescriptorSetLayout createDescriptorSetLayout(VkDevice device,
    VkDescriptorType type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
    VkShaderStageFlags stage = VK_SHADER_STAGE_VERTEX_BIT);

//NO need to be freed (automaticaly freed when pool is destroy)
template<class UBOClass>
_NODISCARD std::array<VkDescriptorSet, MAX_FRAMES_IN_FLIGHT> createDescriptorSets(VkDescriptorSetLayout descriptorSetLayout, VkDescriptorPool descriptorPool,
    VkDevice device, UBO_buffmem ubinst) {
    std::array<VkDescriptorSet, MAX_FRAMES_IN_FLIGHT> descriptorSets;

    std::array<VkDescriptorSetLayout, MAX_FRAMES_IN_FLIGHT> layouts{};
    //fill array
    for (int i = 0; i < MAX_FRAMES_IN_FLIGHT; i++)
    {
        layouts[i] = descriptorSetLayout;
    }

    VkDescriptorSetAllocateInfo allocInfo{};
    allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
    allocInfo.descriptorPool = descriptorPool;
    allocInfo.descriptorSetCount = static_cast<uint32_t>(ubinst.uniformBuffers.size());
    allocInfo.pSetLayouts = layouts.data();


    if (vkAllocateDescriptorSets(device, &allocInfo, descriptorSets.data()) != VK_SUCCESS) {
        throw std::runtime_error("failed to allocate descriptor sets!");
    }

    for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
        VkDescriptorBufferInfo bufferInfo{};
        bufferInfo.buffer = ubinst.uniformBuffers[i];
        bufferInfo.offset = 0;
        bufferInfo.range = sizeof(UBOClass);

        VkWriteDescriptorSet descriptorWrite{};
        descriptorWrite.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
        descriptorWrite.dstSet = descriptorSets[i];
        descriptorWrite.dstBinding = 0;
        descriptorWrite.dstArrayElement = 0;
        descriptorWrite.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
        descriptorWrite.descriptorCount = 1;
        descriptorWrite.pBufferInfo = &bufferInfo;

        vkUpdateDescriptorSets(device, 1, &descriptorWrite, 0, nullptr);
    }

    return descriptorSets;
}