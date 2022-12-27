#pragma once

#include "vulkan/vulkan.h"
#include <GLFW/glfw3.h>

#include <vector>
#include <array>
#include <assert.h>

#include "AppHandler.hpp"

struct SwapChainParam
{
    VkSurfaceFormatKHR imageFormat;
    VkExtent2D extent;
    VkPresentModeKHR presentMode;
};

struct SwapChain
{   
    struct ImageImageView
    {
        VkImage image;
        VkImageView imageView;
    };

    VkSwapchainKHR vkSwapChain;

    SwapChainParam param;

    std::vector<ImageImageView> imageData;

    std::vector<VkFence> fences;

    VkResult getNextImage(uint32_t currentFrame, VkSemaphore imageAvailableSemaphores, uint32_t& imageIndex, VkDevice device)
    {
        vkWaitForFences(device, 1, &(fences[currentFrame]), VK_TRUE, UINT64_MAX);

        VkResult result = vkAcquireNextImageKHR(device, vkSwapChain, UINT64_MAX, imageAvailableSemaphores, VK_NULL_HANDLE, &imageIndex);

        assert(result == VK_SUCCESS || result == VK_SUBOPTIMAL_KHR);

        //TODO: proper error handler ?

        return result;
    }

    void init(GLFWwindow*, DeviceHandler, VkSurfaceKHR, SwapChainParam);

    void cleanUp(const VkDevice device);

    void recreate(GLFWwindow*, DeviceHandler, VkSurfaceKHR, SwapChainParam);
};

struct SwapChainSupportDetails {
    VkSurfaceCapabilitiesKHR capabilities;
    std::vector<VkSurfaceFormatKHR> formats;
    std::vector<VkPresentModeKHR> presentModes;
};


SwapChainSupportDetails querySwapChainSupport(VkPhysicalDevice device, VkSurfaceKHR surface);