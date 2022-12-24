#pragma once

#include "vulkan/vulkan.h"
#include <GLFW/glfw3.h>

#include <vector>

#include "AppHandler.hpp"

struct SwapChainParam
{
    VkSurfaceFormatKHR imageFormat;
    VkExtent2D extent;
    VkPresentModeKHR presentMode;
};

struct SwapChainData
{   
    VkSwapchainKHR vkSwapChain;

    SwapChainParam param;

    std::vector<VkImage> Images;
    std::vector<VkImageView> ImageViews;
    std::vector<VkFramebuffer> Framebuffers;
};

struct SwapChainSupportDetails {
    VkSurfaceCapabilitiesKHR capabilities;
    std::vector<VkSurfaceFormatKHR> formats;
    std::vector<VkPresentModeKHR> presentModes;
};

SwapChainData createSwapChain(GLFWwindow*, DeviceHandler, VkSurfaceKHR, SwapChainParam);

void cleanUpSwapChain(SwapChainData& sc, VkDevice device);

SwapChainSupportDetails querySwapChainSupport(VkPhysicalDevice device, VkSurfaceKHR surface);