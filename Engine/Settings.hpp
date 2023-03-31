#pragma once
#include "include/GameStruct/SwapChain.hpp"
#include "vulkan/vulkan.h"

const int MAX_FRAMES_IN_FLIGHT = 2;

const uint32_t WIDTH = 800;
const uint32_t HEIGHT = 600;

constexpr SwapChainParam swapChainWantedParam{
    {VK_FORMAT_B8G8R8A8_SRGB, VK_COLOR_SPACE_SRGB_NONLINEAR_KHR}, //format
    {WIDTH, HEIGHT},    //extent
    VK_PRESENT_MODE_MAILBOX_KHR //present mode
};

const std::vector<const char*> deviceExtensions = {
    VK_KHR_SWAPCHAIN_EXTENSION_NAME
};