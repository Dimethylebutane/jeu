#pragma once
#include "vulkan/vulkan.h"

#include <array>
#include <stdexcept>
//TODO: tuple group data on memory
#include <tuple>

#include "../Settings.hpp" //max_frame_in_flight
#include "Vertex.hpp"
#include "SwapChain.hpp"
#include "Camera.hpp"
#include "Model.hpp"

//TODO: pb renderpass depend on image format -> skbx = buffer and creation of render pass and pipe by register skbx to image?
struct SkBx
{
	//those 2 data are not accessed at same time
	//frame buffer is used only when creating and destroying SkBx

	std::vector<VkCommandBuffer> commandBuffers;

	std::vector<VkFramebuffer> framebuffers;

	void init(Camera& cam, SwapChain& swapchain, VkCommandPool commandPool, VkDevice device);

	void free(VkCommandPool commandPool, VkDevice device);

	//only camera data; Viewport ans Scissor dynamic
	static VkPipelineLayout pipelineLayout;
	static VkRenderPass renderpass;
	static VkPipeline pipeline;
	static Model model;

	static void InitSkBxStruct(const SwapChain& swapchain, const VkQueue trsfrtQ, const VkCommandPool commandPool, const DeviceHandler devh);
	static void CleanSkBxStruct(VkDevice device);
};