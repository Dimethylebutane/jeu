#pragma once
#include "vulkan/vulkan.h"

#include <array>
#include <stdexcept>

#include "../EngineBuildingBlock/Vertex.hpp"
#include "SwapChain.hpp"
#include "Camera.hpp"
#include "Model.hpp"

//TODO: SkBx only for SwapChain images (register to image system?)
struct SkBx
{
	//those 2 data are not accessed at same time
	//frame buffer is used only when creating and destroying SkBx

	std::vector<VkCommandBuffer> commandBuffers;

	std::vector<VkFramebuffer> framebuffers;

	void init(Camera& cam, SwapChain& swapchain, VkCommandPool commandPool, VkDevice device);
	void recreate(SwapChain& swapchain, Camera& cam, VkDevice device);

	void free(VkCommandPool commandPool, VkDevice device);

	//only camera data; Viewport ans Scissor dynamic
	static VkPipelineLayout pipelineLayout;
	static VkRenderPass renderpass;
	static VkPipeline pipeline;
	static Model model;

	static void InitSkBxStruct(const SwapChainParam swapchainParam, const VkQueue trsfrtQ, const VkCommandPool commandPool, const DeviceHandler devh);
	static void CleanSkBxStruct(VkDevice device);
};