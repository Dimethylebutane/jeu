#pragma once
#include "vulkan/vulkan.h"

#include <vector>
#include <stdexcept>

#include "../../Settings.hpp" //MAX_FRAME_IN_FLIGHT
#include "../EngineBuildingBlock/UBO.hpp"

//need to be destroy
[[nodiscard]] VkDescriptorPool createDescriptorPool(
    uint32_t numberOfDescriptor, 
    VkDevice device,
    VkDescriptorType type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
    VkDescriptorPoolCreateFlags flags = 0);

//need to be destroy
[[nodiscard]] VkDescriptorSetLayout createDescriptorSetLayout(VkDevice device,
    VkDescriptorType type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
    VkShaderStageFlags stage = VK_SHADER_STAGE_VERTEX_BIT);

//NO need to be freed (automaticaly freed when pool is destroy)
template<class UBOClass>
[[nodiscard]] std::vector<VkDescriptorSet> createDescriptorSets(uint32_t numberOfFrame, VkDescriptorSetLayout descriptorSetLayout, VkDescriptorPool descriptorPool,
    VkDevice device, UBO_buffmem ubinst)
{
    std::vector<VkDescriptorSet> descriptorSets;
    descriptorSets.resize(numberOfFrame);

    std::vector<VkDescriptorSetLayout> layouts{};
    layouts.resize(numberOfFrame);

    //fill array
    for (int i = 0; i < numberOfFrame; i++)
    {
        layouts[i] = descriptorSetLayout;
    }

    VkDescriptorSetAllocateInfo allocInfo{};
    allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
    allocInfo.descriptorPool = descriptorPool;
    allocInfo.descriptorSetCount = numberOfFrame;
    allocInfo.pSetLayouts = layouts.data();


    if (vkAllocateDescriptorSets(device, &allocInfo, descriptorSets.data()) != VK_SUCCESS) {
        throw std::runtime_error("failed to allocate descriptor sets!");
    }

    for (size_t i = 0; i < numberOfFrame; i++) {
        VkDescriptorBufferInfo bufferInfo{};
        bufferInfo.buffer = ubinst.uniformBuffers()[i];
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
