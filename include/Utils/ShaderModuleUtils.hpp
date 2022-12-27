#pragma once
#include <vector>
#include <stdexcept>

#include "vulkan/vulkan.h"


VkShaderModule createShaderModule(const std::vector<char>& code, VkDevice device);