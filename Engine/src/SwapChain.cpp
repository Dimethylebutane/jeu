#include "../include/GameStruct/SwapChain.hpp"

#include "../include/Utils/QueueUtils.hpp"

#include "../Settings.hpp"

#include <algorithm>
#include <stdexcept>

SwapChainParam chooseParam(SwapChainParam wanted, const SwapChainSupportDetails availables)
{
    SwapChainParam r{};
    r.imageFormat = availables.formats[0];
    r.presentMode = VK_PRESENT_MODE_FIFO_KHR;

    for (const auto& availableFormat : availables.formats) {
        //if (availableFormat.format == VK_FORMAT_B8G8R8A8_SRGB && availableFormat.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR) {
        if (availableFormat.format == wanted.imageFormat.format && availableFormat.colorSpace == wanted.imageFormat.colorSpace) {
            r.imageFormat = availableFormat;
            break;
        }
    }

    for (const auto& availablePresentMode : availables.presentModes) {
        //if (availablePresentMode == VK_PRESENT_MODE_MAILBOX_KHR) {
        if (availablePresentMode == wanted.presentMode) {
            r.presentMode = availablePresentMode;
            break;
        }
    }

    if (availables.capabilities.currentExtent.width != std::numeric_limits<uint32_t>::max()) {
        r.extent = availables.capabilities.currentExtent;
    }
    else {
        r.extent.width = std::clamp(wanted.extent.width, availables.capabilities.minImageExtent.width, availables.capabilities.maxImageExtent.width);
        r.extent.height = std::clamp(wanted.extent.height, availables.capabilities.minImageExtent.height, availables.capabilities.maxImageExtent.height);
    }

    return r;
}

SwapChainSupportDetails querySwapChainSupport(VkPhysicalDevice device, VkSurfaceKHR surface)
{
    SwapChainSupportDetails details;

    vkGetPhysicalDeviceSurfaceCapabilitiesKHR(device, surface, &details.capabilities);

    uint32_t formatCount;
    vkGetPhysicalDeviceSurfaceFormatsKHR(device, surface, &formatCount, nullptr);

    if (formatCount != 0) {
        details.formats.resize(formatCount);
        vkGetPhysicalDeviceSurfaceFormatsKHR(device, surface, &formatCount, details.formats.data());
    }

    uint32_t presentModeCount;
    vkGetPhysicalDeviceSurfacePresentModesKHR(device, surface, &presentModeCount, nullptr);

    if (presentModeCount != 0) {
        details.presentModes.resize(presentModeCount);
        vkGetPhysicalDeviceSurfacePresentModesKHR(device, surface, &presentModeCount, details.presentModes.data());
    }

    return details;
}

void createVkImageViews(VkDevice device, SwapChain& data)
{
    for (size_t i = 0; i < data.imageData.size(); i++) {
        VkImageViewCreateInfo createInfo{};
        createInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
        createInfo.image = data.imageData[i].image;
        createInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
        createInfo.format = data.param.imageFormat.format;
        createInfo.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
        createInfo.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
        createInfo.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
        createInfo.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;
        createInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        createInfo.subresourceRange.baseMipLevel = 0;
        createInfo.subresourceRange.levelCount = 1;
        createInfo.subresourceRange.baseArrayLayer = 0;
        createInfo.subresourceRange.layerCount = 1;

        if (vkCreateImageView(device, &createInfo, nullptr, &data.imageData[i].imageView) != VK_SUCCESS) {
            throw std::runtime_error("failed to create image views!");
        }
    }
}

void createSwapChain(SwapChain& swapchain, SwapChainSupportDetails swapChainSupport, const VkSurfaceKHR& surface, DeviceHandler& devh)
{
    uint32_t imageCount = MAX_FRAMES_IN_FLIGHT; // /!\ not guaranted, we want this
    if (swapChainSupport.capabilities.maxImageCount > 0 && imageCount > swapChainSupport.capabilities.maxImageCount)
        imageCount = swapChainSupport.capabilities.maxImageCount;
    if (imageCount < swapChainSupport.capabilities.minImageCount)
		imageCount = swapChainSupport.capabilities.minImageCount;

    VkSwapchainCreateInfoKHR createInfo{};
    createInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
    createInfo.surface = surface;

    createInfo.minImageCount = imageCount;
    createInfo.imageFormat = swapchain.param.imageFormat.format;
    createInfo.imageColorSpace = swapchain.param.imageFormat.colorSpace;
    createInfo.imageExtent = swapchain.param.extent;
    createInfo.imageArrayLayers = 1;
    createInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;

    QueueFamilyIndices indices = findQueueFamilies(devh.physicalDevice, surface);
    uint32_t queueFamilyIndices[] = { indices.graphicsFamily.value(), indices.presentFamily.value() };

    if (indices.graphicsFamily != indices.presentFamily) {
        createInfo.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
        createInfo.queueFamilyIndexCount = 2;
        createInfo.pQueueFamilyIndices = queueFamilyIndices;
    }
    else {
        createInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
    }

    createInfo.preTransform = swapChainSupport.capabilities.currentTransform;
    createInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
    createInfo.presentMode = swapchain.param.presentMode;
    createInfo.clipped = VK_TRUE;

    if (vkCreateSwapchainKHR(devh.device, &createInfo, nullptr, &swapchain.vkSwapChain) != VK_SUCCESS) {
        throw std::runtime_error("failed to create swap chain!");
    }

    vkGetSwapchainImagesKHR(devh.device, swapchain.vkSwapChain, &imageCount, nullptr);

    swapchain.imageData.resize(imageCount);

    { //no need to images vector after this block
        std::vector<VkImage> images;
        images.resize(imageCount);

        vkGetSwapchainImagesKHR(devh.device, swapchain.vkSwapChain, &imageCount, images.data());

        for (int i = 0; i < imageCount; i++)
            swapchain.imageData[i].image = images[i];
    }

    createVkImageViews(devh.device, swapchain);
}

void SwapChain::init(GLFWwindow* window, DeviceHandler devh, VkSurfaceKHR surface, SwapChainParam Param)
{
    //check capabilities of swapchain
    SwapChainSupportDetails swapChainSupport = querySwapChainSupport(devh.physicalDevice, surface);

    Param = chooseParam(Param, swapChainSupport);

    this->param = Param;

    createSwapChain(*this, swapChainSupport, surface, devh);

    //FENCE
    fences.resize(MAX_FRAMES_IN_FLIGHT);

    VkFenceCreateInfo fenceInfo{};
    fenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
    fenceInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;

    for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
        if (vkCreateFence(devh.device, &fenceInfo, nullptr, &fences[i]) != VK_SUCCESS) {
            throw std::runtime_error("failed to create synchronization objects for a frame!");
        }
    }
}

void SwapChain::cleanUp(const VkDevice device)
{
    for (auto f : fences)
        vkDestroyFence(device, f, nullptr);

    for (auto iData : imageData)
    {
        vkDestroyImageView(device, iData.imageView, nullptr);
    }

    vkDestroySwapchainKHR(device, vkSwapChain, nullptr);
}

void SwapChain::recreate(GLFWwindow* window, DeviceHandler devh, VkSurfaceKHR surface, SwapChainParam Param)
{
    //free everything except fences
    for (auto iData : imageData)
    {
        vkDestroyImageView(devh.device, iData.imageView, nullptr);
    }

    vkDestroySwapchainKHR(devh.device, vkSwapChain, nullptr);

    //recreate stuff
    SwapChainSupportDetails swapChainSupport = querySwapChainSupport(devh.physicalDevice, surface);

    Param = chooseParam(Param, swapChainSupport);

    this->param = Param;

    createSwapChain(*this, swapChainSupport, surface, devh);
}
