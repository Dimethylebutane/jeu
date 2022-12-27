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
        //VkFramebuffer frameBuffer;
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
};

struct SwapChainSupportDetails {
    VkSurfaceCapabilitiesKHR capabilities;
    std::vector<VkSurfaceFormatKHR> formats;
    std::vector<VkPresentModeKHR> presentModes;
};

//need to be cleanedUp
SwapChain createSwapChain(GLFWwindow*, DeviceHandler, VkSurfaceKHR, SwapChainParam);

void cleanUpSwapChain(SwapChain sc, const VkDevice device);

SwapChainSupportDetails querySwapChainSupport(VkPhysicalDevice device, VkSurfaceKHR surface);