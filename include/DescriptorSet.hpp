#pragma once
#include "vulkan/vulkan.h"

#include <array>
#include <stdexcept>

#include "../Settings.hpp" //MAX_FRAME_IN_FLIGHT
#include "UBO.hpp"

//need to be destroy
_NODISCARD VkDescriptorPool createDescriptorPool(VkDevice device);

//need to be destroy
_NODISCARD VkDescriptorSetLayout createDescriptorSetLayout(VkDevice device,
    VkDescriptorType type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
    VkShaderStageFlags stage = VK_SHADER_STAGE_VERTEX_BIT);

//NO need to be freed (automaticaly freed when pool is destroy)
_NODISCARD std::array<VkDescriptorSet, MAX_FRAMES_IN_FLIGHT> createDescriptorSets(VkDescriptorSetLayout descriptorSetLayout, VkDescriptorPool descriptorPool,
    VkDevice device, UBO_buffmem ubinst);