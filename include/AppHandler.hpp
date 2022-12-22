#pragma once

#include "vulkan/vulkan.h"

struct ISHandler
{
	VkInstance instance;
	VkSurfaceKHR surface;
};

struct DeviceHandler
{
	VkPhysicalDevice physicalDevice = VK_NULL_HANDLE;
	VkDevice device;
};

struct QueueHandler
{
	VkQueue graphicsQueue;
	VkQueue presentQueue;
};