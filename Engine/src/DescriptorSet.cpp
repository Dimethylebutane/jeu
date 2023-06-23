#include "../include/Utils/DescriptorSetUtils.hpp"

[[nodiscard]] VkDescriptorPool createDescriptorPool(
    uint32_t numberOfDescriptor,
    VkDevice device,
    VkDescriptorType type,
    VkDescriptorPoolCreateFlags flags) {
    VkDescriptorPool r{};

    VkDescriptorPoolSize poolSize{};
    poolSize.type = type;
    poolSize.descriptorCount = numberOfDescriptor;

    VkDescriptorPoolCreateInfo poolInfo{};
    poolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
    poolInfo.poolSizeCount = 1;
    poolInfo.pPoolSizes = &poolSize;
    poolInfo.maxSets = numberOfDescriptor;
    poolInfo.flags = flags;

    if (vkCreateDescriptorPool(device, &poolInfo, nullptr, &r) != VK_SUCCESS) {
        throw std::runtime_error("failed to create descriptor pool!");
    }

    return r;
}

[[nodiscard]] VkDescriptorSetLayout createDescriptorSetLayout(VkDevice device,
    VkDescriptorType type,
    VkShaderStageFlags stage)
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