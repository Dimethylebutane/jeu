#include "../include/SwapChain.hpp"

#include "../include/QueueUtils.hpp"

#include <algorithm>
#include <stdexcept>

SwapChainParam chooseParam(SwapChainParam wanted, SwapChainSupportDetails availables)
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
        wanted.extent.width = std::clamp(wanted.extent.width, availables.capabilities.minImageExtent.width, availables.capabilities.maxImageExtent.width);
        wanted.extent.height = std::clamp(wanted.extent.height, availables.capabilities.minImageExtent.height, availables.capabilities.maxImageExtent.height);
    }

    return r;
}

SwapChainSupportDetails querySwapChainSupport(VkPhysicalDevice device, VkSurfaceKHR surface) {
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

void createImageViews(VkDevice device, SwapChainData& data)
{
    data.ImageViews.resize(data.Images.size());

    for (size_t i = 0; i < data.Images.size(); i++) {
        VkImageViewCreateInfo createInfo{};
        createInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
        createInfo.image = data.Images[i];
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

        if (vkCreateImageView(device, &createInfo, nullptr, &data.ImageViews[i]) != VK_SUCCESS) {
            throw std::runtime_error("failed to create image views!");
        }
    }
}

_NODISCARD SwapChainData createSwapChain(GLFWwindow* window, DeviceHandler devh, VkSurfaceKHR surface, SwapChainParam param)
{
    //initialize blank data to return
    SwapChainData data{};

    //check capabilities of swapchain
    SwapChainSupportDetails swapChainSupport = querySwapChainSupport(devh.physicalDevice, surface);

    param = chooseParam(param, swapChainSupport);

    data.param = param;

    uint32_t imageCount = swapChainSupport.capabilities.minImageCount + 1;
    if (swapChainSupport.capabilities.maxImageCount > 0 && imageCount > swapChainSupport.capabilities.maxImageCount) {
        imageCount = swapChainSupport.capabilities.maxImageCount;
    }

    VkSwapchainCreateInfoKHR createInfo{};
    createInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
    createInfo.surface = surface;

    createInfo.minImageCount = imageCount;
    createInfo.imageFormat = param.imageFormat.format;
    createInfo.imageColorSpace = param.imageFormat.colorSpace;
    createInfo.imageExtent = param.extent;
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
    createInfo.presentMode = param.presentMode;
    createInfo.clipped = VK_TRUE;

    if (vkCreateSwapchainKHR(devh.device, &createInfo, nullptr, &data.vkSwapChain) != VK_SUCCESS) {
        throw std::runtime_error("failed to create swap chain!");
    }

    vkGetSwapchainImagesKHR(devh.device, data.vkSwapChain, &imageCount, nullptr);
    data.Images.resize(imageCount);
    vkGetSwapchainImagesKHR(devh.device, data.vkSwapChain, &imageCount, data.Images.data());

    createImageViews(devh.device, data);

    return data;
}

void cleanUpSwapChain(SwapChainData& sc, VkDevice device)
{
    for (auto framebuffer : sc.Framebuffers) {
        vkDestroyFramebuffer(device, framebuffer, nullptr);
    }

    for (auto imageView : sc.ImageViews) {
        vkDestroyImageView(device, imageView, nullptr);
    }

    vkDestroySwapchainKHR(device, sc.vkSwapChain, nullptr);

    sc = SwapChainData{};
}