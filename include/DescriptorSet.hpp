#pragma once
#include "vulkan/vulkan.h"

#include <array>

#include "../Settings.hpp" //MAX_FRAME_IN_FLIGHT

//need to be destroy
_NODISCARD VkDescriptorPool createDescriptorPool(VkDevice device) {
    VkDescriptorPool r{};

    VkDescriptorPoolSize poolSize{};
    poolSize.type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
    poolSize.descriptorCount = static_cast<uint32_t>(MAX_FRAMES_IN_FLIGHT);

    VkDescriptorPoolCreateInfo poolInfo{};
    poolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
    poolInfo.poolSizeCount = 1;
    poolInfo.pPoolSizes = &poolSize;
    poolInfo.maxSets = static_cast<uint32_t>(MAX_FRAMES_IN_FLIGHT);

    if (vkCreateDescriptorPool(device, &poolInfo, nullptr, &r) != VK_SUCCESS) {
        throw std::runtime_error("failed to create descriptor pool!");
    }

    return r;
}

//need to be destroy
_NODISCARD VkDescriptorSetLayout createDescriptorSetLayout(VkDevice device,
        VkDescriptorType type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
        VkShaderStageFlags stage = VK_SHADER_STAGE_VERTEX_BIT)
{
    VkDescriptorSetLayout r{};

    VkDescriptorSetLayoutBinding uboLayoutBinding{};
    uboLayoutBinding.binding = 0;
    uboLayoutBinding.descriptorCount = 1;
    uboLayoutBinding.descriptorType = type;
    uboLayoutBinding.pImmutableSamplers = nullptr;
    uboLayoutBinding.stageFlags = stage;

    VkDescriptorSetLayoutCreateInfo layoutInfo{};
    layoutInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
    layoutInfo.bindingCount = 1;
    layoutInfo.pBindings = &uboLayoutBinding;

    if (vkCreateDescriptorSetLayout(device, &layoutInfo, nullptr, &r) != VK_SUCCESS) {
        throw std::runtime_error("failed to create descriptor set layout!");
    }

    return r;
}

//NO need to be freed (automaticaly freed when pool is destroy)
_NODISCARD std::array<VkDescriptorSet, MAX_FRAMES_IN_FLIGHT> createDescriptorSets(VkDescriptorSetLayout descriptorSetLayout, VkDescriptorPool descriptorPool,
        VkDevice device, UBO_buffmem ubinst) {
    std::array<VkDescriptorSet, MAX_FRAMES_IN_FLIGHT> descriptorSets;

    std::array<VkDescriptorSetLayout, MAX_FRAMES_IN_FLIGHT> layouts{};
        //fill array
    for (int i = 0; i < MAX_FRAMES_IN_FLIGHT; i++)
    { layouts[i] = descriptorSetLayout; }

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
        bufferInfo.range = sizeof(CamUBObj);

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