#include "../include/GameStruct/SkyBox.hpp"
#include "../include/Utils/FileMngmtUtils.hpp"
#include "../include/Utils/ShaderModuleUtils.hpp"

const std::vector<VertexPositionColor> vertices = {
     {{-0.5f, -0.5f, -0.5f}, {0.0f, 0.6f, 0.1f}},
    {{0.5f, -0.5f, -0.5f}, {0.0f, 0.6f, 0.1f}},
    {{0.5f, 0.5f, -0.5f}, {0.0f, 0.6f, 0.1f}},
    {{-0.5f, 0.5f, -0.5f}, {0.0f, 0.6f, 0.1f}},

    {{-0.5f, -0.5f, 0.5f}, {0.0f, 0.71f, 1.0f}},
    {{0.5f, -0.5f, 0.5f}, {0.0f, 0.71f, 1.0f}},
    {{0.5f, 0.5f, 0.5f}, {0.0f, 0.71f, 1.0f}},
    {{-0.5f, 0.5f, 0.5f}, {0.0f, 0.71f, 1.0f}}
};

const std::vector<uint16_t> indices = {
    0, 1, 2,
    2, 3, 0,
    0, 3, 7,
    7, 4, 0,
    0, 4, 5,
    5, 1, 0,
    1, 6, 2,
    6, 1, 5,
    2, 7, 3,
    7, 2, 6,
    4, 6, 5,
    6, 4, 7
};

VkPipelineLayout SkBx::pipelineLayout;
VkRenderPass SkBx::renderpass;
VkPipeline SkBx::pipeline;
Model SkBx::model;

[[nodiscard]] VkRenderPass createRenderPass(SwapChainParam scp, VkDevice device)
{
    VkRenderPass r;

    VkAttachmentDescription colorAttachmentSKBX{};
    colorAttachmentSKBX.format = scp.imageFormat.format;
    colorAttachmentSKBX.samples = VK_SAMPLE_COUNT_1_BIT;
    colorAttachmentSKBX.loadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
    colorAttachmentSKBX.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
    colorAttachmentSKBX.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
    colorAttachmentSKBX.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
    colorAttachmentSKBX.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    colorAttachmentSKBX.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR ;//VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

    VkAttachmentReference colorAttachmentRef{};
    colorAttachmentRef.attachment = 0;
    colorAttachmentRef.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

    VkSubpassDescription subpass{};
    subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
    subpass.colorAttachmentCount = 1;
    subpass.pColorAttachments = &colorAttachmentRef;

    VkSubpassDependency dependency{};
    dependency.srcSubpass = VK_SUBPASS_EXTERNAL;
    dependency.dstSubpass = 0;
    dependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
    dependency.srcAccessMask = 0;
    dependency.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
    dependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;

    VkRenderPassCreateInfo renderPassInfo{};
    renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
    renderPassInfo.attachmentCount = 1;
    renderPassInfo.pAttachments = &colorAttachmentSKBX;
    renderPassInfo.subpassCount = 1;
    renderPassInfo.pSubpasses = &subpass;
    renderPassInfo.dependencyCount = 1;
    renderPassInfo.pDependencies = &dependency;

    if (vkCreateRenderPass(device, &renderPassInfo, nullptr, &r) != VK_SUCCESS) {
        throw std::runtime_error("failed to create render pass!");
    }

    return r;
}

//needToBedestroyed
[[nodiscard]] std::vector<VkFramebuffer> createSWPCHNFrameBuffer(SwapChain& swapchain, VkRenderPass SkBx_renderpass, VkDevice device)
{
    std::vector<VkFramebuffer> swapChainFramebuffers;

    swapChainFramebuffers.resize(swapchain.imageData.size());

    for (size_t i = 0; i < swapchain.imageData.size(); i++) {
        VkImageView attachments[] = {
            swapchain.imageData[i].imageView
        };

        VkFramebufferCreateInfo framebufferInfo{};
        framebufferInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
        framebufferInfo.renderPass = SkBx_renderpass;
        framebufferInfo.attachmentCount = 1;
        framebufferInfo.pAttachments = attachments;
        framebufferInfo.width = swapchain.param.extent.width;
        framebufferInfo.height = swapchain.param.extent.height;
        framebufferInfo.layers = 1;

        if (vkCreateFramebuffer(device, &framebufferInfo, nullptr, &swapChainFramebuffers[i]) != VK_SUCCESS) {
            throw std::runtime_error("failed to create framebuffer!");
        }
    }

    return swapChainFramebuffers;
}

[[nodiscard]] VkPipelineLayout createStaticPipelineLayout(VkDevice device)
{
    VkPipelineLayout pipelineLayoutStatic;
    std::vector<VkDynamicState> dynamicStates = {
        VK_DYNAMIC_STATE_VIEWPORT,
        VK_DYNAMIC_STATE_SCISSOR
    };
    VkPipelineDynamicStateCreateInfo dynamicState{};
    dynamicState.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
    dynamicState.dynamicStateCount = static_cast<uint32_t>(dynamicStates.size());
    dynamicState.pDynamicStates = dynamicStates.data();

    VkPipelineLayoutCreateInfo pipelineLayoutInfo{};
    pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
    pipelineLayoutInfo.setLayoutCount = 1;
    pipelineLayoutInfo.pSetLayouts = &Camera::CameraDescriptorSetLayout;

    if (vkCreatePipelineLayout(device, &pipelineLayoutInfo, nullptr, &pipelineLayoutStatic) != VK_SUCCESS) {
        throw std::runtime_error("failed to create pipeline layout!");
    }

    return pipelineLayoutStatic;
}

[[nodiscard]] VkPipeline createSKBXPipeline(VkPipelineLayout pipelineLayoutStatic, VkRenderPass SkBx_renderpass, VkDevice device)
{
    VkPipeline SkBx_pipeline;

    auto vertShaderCode = readFile(RESOURCES_PATH "/shaders/vertSkBx.spv");
    auto fragShaderCode = readFile(RESOURCES_PATH "/shaders/fragSkBx.spv");

    VkShaderModule vertShaderModule = createShaderModule(vertShaderCode, device);
    VkShaderModule fragShaderModule = createShaderModule(fragShaderCode, device);

    VkPipelineShaderStageCreateInfo vertShaderStageInfo{};
    vertShaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
    vertShaderStageInfo.stage = VK_SHADER_STAGE_VERTEX_BIT;
    vertShaderStageInfo.module = vertShaderModule;
    vertShaderStageInfo.pName = "main";

    VkPipelineShaderStageCreateInfo fragShaderStageInfo{};
    fragShaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
    fragShaderStageInfo.stage = VK_SHADER_STAGE_FRAGMENT_BIT;
    fragShaderStageInfo.module = fragShaderModule;
    fragShaderStageInfo.pName = "main";

    VkPipelineShaderStageCreateInfo shaderStages[] = { vertShaderStageInfo, fragShaderStageInfo };

    VkPipelineVertexInputStateCreateInfo vertexInputInfo{};
    vertexInputInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;

    auto bindingDescription = VertexPositionColor::getBindingDescription();
    auto attributeDescriptions = VertexPositionColor::getAttributeDescriptions();

    vertexInputInfo.vertexBindingDescriptionCount = 1;
    vertexInputInfo.vertexAttributeDescriptionCount = static_cast<uint32_t>(attributeDescriptions.size());
    vertexInputInfo.pVertexBindingDescriptions = &bindingDescription;
    vertexInputInfo.pVertexAttributeDescriptions = attributeDescriptions.data();

    VkPipelineInputAssemblyStateCreateInfo inputAssembly{};
    inputAssembly.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
    inputAssembly.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
    inputAssembly.primitiveRestartEnable = VK_FALSE;

    VkPipelineViewportStateCreateInfo viewportState{};
    viewportState.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
    viewportState.viewportCount = 1;
    viewportState.scissorCount = 1;

    VkPipelineRasterizationStateCreateInfo rasterizer{};
    rasterizer.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
    rasterizer.depthClampEnable = VK_FALSE;
    rasterizer.rasterizerDiscardEnable = VK_FALSE;
    rasterizer.polygonMode = VK_POLYGON_MODE_FILL;
    rasterizer.lineWidth = 1.0f;
    rasterizer.cullMode = VK_CULL_MODE_BACK_BIT;
    rasterizer.frontFace = VK_FRONT_FACE_COUNTER_CLOCKWISE;
    rasterizer.depthBiasEnable = VK_FALSE;

    VkPipelineMultisampleStateCreateInfo multisampling{};
    multisampling.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
    multisampling.sampleShadingEnable = VK_FALSE;
    multisampling.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;

    VkPipelineColorBlendAttachmentState colorBlendAttachment{};
    colorBlendAttachment.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
    colorBlendAttachment.blendEnable = VK_FALSE;

    VkPipelineColorBlendStateCreateInfo colorBlending{};
    colorBlending.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
    colorBlending.logicOpEnable = VK_FALSE;
    colorBlending.logicOp = VK_LOGIC_OP_COPY;
    colorBlending.attachmentCount = 1;
    colorBlending.pAttachments = &colorBlendAttachment;
    colorBlending.blendConstants[0] = 0.0f;
    colorBlending.blendConstants[1] = 0.0f;
    colorBlending.blendConstants[2] = 0.0f;
    colorBlending.blendConstants[3] = 0.0f;

    std::vector<VkDynamicState> dynamicStates = {
       VK_DYNAMIC_STATE_VIEWPORT,
       VK_DYNAMIC_STATE_SCISSOR
    };
    VkPipelineDynamicStateCreateInfo dynamicState{};
    dynamicState.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
    dynamicState.dynamicStateCount = static_cast<uint32_t>(dynamicStates.size());
    dynamicState.pDynamicStates = dynamicStates.data();

    VkGraphicsPipelineCreateInfo pipelineInfo{};
    pipelineInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
    pipelineInfo.stageCount = 2;
    pipelineInfo.pStages = shaderStages;
    pipelineInfo.pVertexInputState = &vertexInputInfo;
    pipelineInfo.pInputAssemblyState = &inputAssembly;
    pipelineInfo.pViewportState = &viewportState;
    pipelineInfo.pRasterizationState = &rasterizer;
    pipelineInfo.pMultisampleState = &multisampling;
    pipelineInfo.pColorBlendState = &colorBlending;
    pipelineInfo.pDynamicState = &dynamicState;
    pipelineInfo.layout = pipelineLayoutStatic;
    pipelineInfo.renderPass = SkBx_renderpass;
    pipelineInfo.subpass = 0;
    pipelineInfo.basePipelineHandle = VK_NULL_HANDLE;

    if (vkCreateGraphicsPipelines(device, VK_NULL_HANDLE, 1, &pipelineInfo, nullptr, &SkBx_pipeline) != VK_SUCCESS) {
        throw std::runtime_error("failed to create graphics pipeline!");
    }

    vkDestroyShaderModule(device, fragShaderModule, nullptr);
    vkDestroyShaderModule(device, vertShaderModule, nullptr);

    return SkBx_pipeline;
}

void recordCommandBuffers(std::vector<VkCommandBuffer>& SkBx_commandBuffers, Camera& cam, std::vector<VkFramebuffer>& framebuffers, VkExtent2D ext, Model mod)
{
    int imageIndex = 0;
    for (auto commandBuffer : SkBx_commandBuffers) //SkBx_commandBuffers[2] = nullptr
    {
        VkCommandBufferBeginInfo beginInfo{};
        beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;

        if (vkBeginCommandBuffer(commandBuffer, &beginInfo) != VK_SUCCESS) {
            throw std::runtime_error("failed to begin recording command buffer!");
        }

        VkRenderPassBeginInfo renderPassInfo{};
        renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
        renderPassInfo.renderPass = SkBx::renderpass;
        renderPassInfo.framebuffer = framebuffers[imageIndex];
        renderPassInfo.renderArea.offset = { 0, 0 };
        renderPassInfo.renderArea.extent = ext;
        renderPassInfo.clearValueCount = 0;

        vkCmdBeginRenderPass(commandBuffer, &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);

        vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, SkBx::pipeline);

        VkViewport viewport{};
        viewport.x = 0.0f;
        viewport.y = 0.0f;
        viewport.width = (float)ext.width;
        viewport.height = (float)ext.height;
        viewport.minDepth = 0.0f;
        viewport.maxDepth = 1.0f;
        vkCmdSetViewport(commandBuffer, 0, 1, &viewport);

        VkRect2D scissor{};
        scissor.offset = { 0, 0 };
        scissor.extent = ext;
        vkCmdSetScissor(commandBuffer, 0, 1, &scissor);

        VkBuffer vertexBuffers[] = { mod.vertexBuffer };
        VkDeviceSize offsets[] = { 0 };
        vkCmdBindVertexBuffers(commandBuffer, 0, 1, vertexBuffers, offsets);

        vkCmdBindIndexBuffer(commandBuffer, mod.indexBuffer, 0, VK_INDEX_TYPE_UINT16);

        vkCmdBindDescriptorSets(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, SkBx::pipelineLayout, 0, 1, &cam.m_descrptrSet[imageIndex], 0, nullptr);

        vkCmdDrawIndexed(commandBuffer, static_cast<uint32_t>(indices.size()), 1, 0, 0, 0);

        vkCmdEndRenderPass(commandBuffer);

        if (vkEndCommandBuffer(commandBuffer) != VK_SUCCESS) {
            throw std::runtime_error("failed to record command buffer!");
        }
        imageIndex++;
    }
}

[[nodiscard]] std::vector<VkCommandBuffer> createSkBxCommandBuffer(Camera& cam, Model mod, std::vector<VkFramebuffer> framebuffers, VkCommandPool commandPool, SwapChainParam scp, VkDevice device)
{
    std::vector<VkCommandBuffer> SkBx_commandBuffers;
    SkBx_commandBuffers.resize(framebuffers.size());

    //TODO: secondary command buffer
    VkCommandBufferAllocateInfo allocInfo{};
    allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    allocInfo.commandPool = commandPool; //where is created ?
    allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    allocInfo.commandBufferCount = (uint32_t)(SkBx_commandBuffers.size());

    if (vkAllocateCommandBuffers(device, &allocInfo, SkBx_commandBuffers.data()) != VK_SUCCESS) {
        throw std::runtime_error("failed to allocate command buffers!");
    }

    recordCommandBuffers(SkBx_commandBuffers, cam, framebuffers, scp.extent, mod);

    return SkBx_commandBuffers;
}



void SkBx::InitSkBxStruct(const SwapChainParam swapchainParam, const VkQueue trsfrtQ, const VkCommandPool commandPool, const DeviceHandler devh)
{
    SkBx::renderpass = createRenderPass(swapchainParam, devh.device);
    SkBx::pipelineLayout = createStaticPipelineLayout(devh.device);
    SkBx::pipeline = createSKBXPipeline(SkBx::pipelineLayout, SkBx::renderpass, devh.device);
    SkBx::model.create<VertexPositionColor>(vertices, indices, devh, trsfrtQ, commandPool);
}

void SkBx::init(Camera& cam, SwapChain& swapchain, VkCommandPool commandPool, VkDevice device)
{
    framebuffers = createSWPCHNFrameBuffer(swapchain, SkBx::renderpass, device);

    commandBuffers = createSkBxCommandBuffer(cam, SkBx::model, framebuffers, commandPool, swapchain.param, device); //TODO: bug here
}

void SkBx::recreate(SwapChain& swapchain, Camera& cam, VkDevice device)
{
    //free framebuffer
    for (auto fb : framebuffers)
        vkDestroyFramebuffer(device, fb, nullptr);

    //reset cb
    for(auto cb : commandBuffers)
        vkResetCommandBuffer(cb, /*VkCommandBufferResetFlagBits*/ 0);

    framebuffers = createSWPCHNFrameBuffer(swapchain, SkBx::renderpass, device);

    recordCommandBuffers(commandBuffers, cam, framebuffers, swapchain.param.extent, SkBx::model);
}

void SkBx::free(VkCommandPool commandPool, VkDevice device)
{
    vkFreeCommandBuffers(device, commandPool, (uint32_t)commandBuffers.size(), commandBuffers.data());

    for (auto fb : framebuffers)
        vkDestroyFramebuffer(device, fb, nullptr);
}

void SkBx::CleanSkBxStruct(VkDevice device)
{
    model.destroy(device);
    vkDestroyPipeline(device, SkBx::pipeline, nullptr);
    vkDestroyPipelineLayout(device, SkBx::pipelineLayout, nullptr);
    vkDestroyRenderPass(device, SkBx::renderpass, nullptr);
}
