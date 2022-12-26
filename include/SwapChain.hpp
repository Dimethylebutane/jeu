#pragma once

#include "vulkan/vulkan.h"
#include <GLFW/glfw3.h>

#include <vector>
#include <array>

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

    std::vector<VkFence> fences;
};

struct SwapChainSupportDetails {
    VkSurfaceCapabilitiesKHR capabilities;
    std::vector<VkSurfaceFormatKHR> formats;
    std::vector<VkPresentModeKHR> presentModes;
};

//need to be cleanedUp
SwapChainData createSwapChain(GLFWwindow*, DeviceHandler, VkSurfaceKHR, SwapChainParam);

void cleanUpSwapChain(SwapChainData sc, const VkDevice device);

SwapChainSupportDetails querySwapChainSupport(VkPhysicalDevice device, VkSurfaceKHR surface);