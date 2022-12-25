// jeu.cpp : Defines the entry point for the application.
//

#include "jeu.h"

#include "vulkan/vulkan.h"
#include "glm/glm.hpp"

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include <tiny_gltf.h>

//#include <iostream>
//#include <fstream>
//#include <stdexcept>
//#include <algorithm>
//#include <vector>
//#include <cstring>
//#include <cstdlib>
//#include <cstdint>
//#include <limits>
//#include <optional>
#include <set>

#include "include/SwapChain.hpp"
#include "include/AppHandler.hpp"
#include "include/ShaderModuleUtils.hpp"
#include "include/FileMngmtUtils.hpp"
#include "Settings.hpp"
#include "include/Model.hpp"
#include "include/CommandPool.hpp"
#include "include/DescriptorSet.hpp"
#include "include/Camera.hpp"



#include "libUtils/include/MacroGlobal.hpp"


#ifndef NDEBUG
#define ENABLEVALIDATIONLAYERS
#endif

//include validation layer stuff if enable
#ifdef ENABLEVALIDATIONLAYERS
#include "include/ValidationLayer.hpp"
#endif 


class HelloTriangleApplication {
public:
    void run() {
        initWindow();
        initVulkan();
        mainLoop();
        cleanup();
    }

private:
    GLFWwindow* window;

#ifdef ENABLEVALIDATIONLAYERS
    VkDebugUtilsMessengerEXT debugMessenger;
#endif // !NDEBUG

    ISHandler m_is;

    DeviceHandler m_devh{VK_NULL_HANDLE,  VK_NULL_HANDLE};

    QueueHandler m_queues;

    SwapChainData m_swapchain;

    VkRenderPass SkBx_renderpass;
    std::vector<VkFramebuffer> swapChainFramebuffers;

    VkPipelineLayout pipelineLayout;
    VkPipeline graphicsPipeline;

    Camera m_defaultCam;

    VkCommandPool commandPool;
    std::vector<VkCommandBuffer> commandBuffers;

    std::vector<VkSemaphore> imageAvailableSemaphores;
    std::vector<VkSemaphore> renderFinishedSemaphores;
    std::vector<VkFence> inFlightFences;
    uint32_t currentFrame = 0;

    bool framebufferResized = false;

    void initWindow() {
        glfwInit();

        glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);

        window = glfwCreateWindow(WIDTH, HEIGHT, "Jeu", nullptr, nullptr);
        glfwSetWindowUserPointer(window, this);
        glfwSetFramebufferSizeCallback(window, framebufferResizeCallback);
    }

    static void framebufferResizeCallback(GLFWwindow* window, int width, int height) {
        auto app = reinterpret_cast<HelloTriangleApplication*>(glfwGetWindowUserPointer(window));
        app->framebufferResized = true;
    }

    void initVulkan() {
        createInstance();

#ifdef ENABLEVALIDATIONLAYERS
        setupDebugMessenger(m_is.instance, debugMessenger);
#endif // !ENABLEVALIDATIONLAYERS

        createSurface();
        pickPhysicalDevice();
        createLogicalDevice();

        m_swapchain = createSwapChain(window, m_devh, m_is.surface, swapChainWantedParam);

#ifdef CAMERA
        Camera::InitCamerasClass(m_devh);
#endif // CAMERA

        createRenderPass();

        createSWPCHNFrameBuffer();

        commandPool = createCommandPool(m_devh, m_is.surface);

        m_defaultCam.init();

        //TODO: model + shader + pipeline

    }

    //needToBedestroyed
    void createRenderPass()
    {
        VkAttachmentDescription colorAttachmentSKBX{};
        colorAttachmentSKBX.format = m_swapchain.param.imageFormat.format;
        colorAttachmentSKBX.samples = VK_SAMPLE_COUNT_1_BIT;
        colorAttachmentSKBX.loadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
        colorAttachmentSKBX.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
        colorAttachmentSKBX.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
        colorAttachmentSKBX.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
        colorAttachmentSKBX.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
        colorAttachmentSKBX.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

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

        if (vkCreateRenderPass(m_devh.device, &renderPassInfo, nullptr, &SkBx_renderpass) != VK_SUCCESS) {
            throw std::runtime_error("failed to create render pass!");
        }
    }

    //needToBedestroyed
    void createSWPCHNFrameBuffer()
    {
        swapChainFramebuffers.resize( m_swapchain.ImageViews.size());

        for (size_t i = 0; i < m_swapchain.ImageViews.size(); i++) {
            VkImageView attachments[] = {
                m_swapchain.ImageViews[i]
            };

            VkFramebufferCreateInfo framebufferInfo{};
            framebufferInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
            framebufferInfo.renderPass = SkBx_renderpass;
            framebufferInfo.attachmentCount = 1;
            framebufferInfo.pAttachments = attachments;
            framebufferInfo.width = m_swapchain.param.extent.width;
            framebufferInfo.height = m_swapchain.param.extent.height;
            framebufferInfo.layers = 1;

            if (vkCreateFramebuffer(m_devh.device, &framebufferInfo, nullptr, &swapChainFramebuffers[i]) != VK_SUCCESS) {
                throw std::runtime_error("failed to create framebuffer!");
            }
        }
    }

    void mainLoop() {
        while (!glfwWindowShouldClose(window)) {
            glfwPollEvents();
            //drawFrame();
        }

        vkDeviceWaitIdle(m_devh.device);
    }

    void cleanup() {
        
        for (auto framebuffer : swapChainFramebuffers) {
            vkDestroyFramebuffer(m_devh.device, framebuffer, nullptr);
        }

        cleanUpSwapChain(m_swapchain, m_devh.device);

        //vkDestroyPipeline(m_devh.device, graphicsPipeline, nullptr);
        //vkDestroyPipelineLayout(m_devh.device, pipelineLayout, nullptr);

        vkDestroyRenderPass(m_devh.device, SkBx_renderpass, nullptr);


        m_defaultCam.clean();

        //vkDestroyDescriptorPool(m_devh.device, CamDescPool, nullptr);
        //vkDestroyDescriptorSetLayout(m_devh.device, CamDescLayout, nullptr);

        /*for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
            vkDestroySemaphore(m_devh.device, renderFinishedSemaphores[i], nullptr);
            vkDestroySemaphore(m_devh.device, imageAvailableSemaphores[i], nullptr);
            vkDestroyFence(m_devh.device, inFlightFences[i], nullptr);
        }*/

        vkDestroyCommandPool(m_devh.device, commandPool, nullptr);

#ifdef CAMERA
        Camera::CleanCameraClass();
#endif // CAMERA

        vkDestroyDevice(m_devh.device, nullptr);

#ifdef ENABLEVALIDATIONLAYERS
        DestroyDebugUtilsMessengerEXT(m_is.instance, debugMessenger, nullptr);
#endif // ENABLEVALIDATIONLAYERS

        vkDestroySurfaceKHR(m_is.instance, m_is.surface, nullptr);
        vkDestroyInstance(m_is.instance, nullptr);

        glfwDestroyWindow(window);

        glfwTerminate();
    }

    void recreateSwapChain() {
        int width = 0, height = 0;
        glfwGetFramebufferSize(window, &width, &height);
        while (width == 0 || height == 0) {
            glfwGetFramebufferSize(window, &width, &height);
            glfwWaitEvents();
        }

        vkDeviceWaitIdle(m_devh.device);

        cleanUpSwapChain(m_swapchain, m_devh.device);

        createSwapChain(window, m_devh, m_is.surface, swapChainWantedParam);

        //createFramebuffers();
    }

    void createInstance() {

#ifdef ENABLEVALIDATIONLAYERS
        if (!checkValidationLayerSupport()) {
            throw std::runtime_error("validation layers requested, but not available!");
        }
#endif

        VkApplicationInfo appInfo{};
        appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
        appInfo.pApplicationName = "Jeu";
        appInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
        appInfo.pEngineName = "Custom Engine";
        appInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
        appInfo.apiVersion = VK_API_VERSION_1_0;

        VkInstanceCreateInfo createInfo{};
        createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
        createInfo.pApplicationInfo = &appInfo;

        auto extensions = getRequiredExtensions();
        createInfo.enabledExtensionCount = static_cast<uint32_t>(extensions.size());
        createInfo.ppEnabledExtensionNames = extensions.data();

        VkDebugUtilsMessengerCreateInfoEXT debugCreateInfo{};


#ifdef ENABLEVALIDATIONLAYERS
        createInfo.enabledLayerCount = static_cast<uint32_t>(validationLayers.size());
        createInfo.ppEnabledLayerNames = validationLayers.data();

        populateDebugMessengerCreateInfo(debugCreateInfo);
        createInfo.pNext = (VkDebugUtilsMessengerCreateInfoEXT*)&debugCreateInfo;
#else
        createInfo.enabledLayerCount = 0;

        createInfo.pNext = nullptr;
#endif // ENABLEVALIDATIONLAYERS


        if (vkCreateInstance(&createInfo, nullptr, &m_is.instance) != VK_SUCCESS) {
            throw std::runtime_error("failed to create instance!");
        }
    }

    void createSurface() {
        if (glfwCreateWindowSurface(m_is.instance, window, nullptr, &m_is.surface) != VK_SUCCESS) {
            throw std::runtime_error("failed to create window surface!");
        }
    }

    void pickPhysicalDevice() {
        uint32_t deviceCount = 0;
        vkEnumeratePhysicalDevices(m_is.instance, &deviceCount, nullptr);

        if (deviceCount == 0) {
            throw std::runtime_error("failed to find GPUs with Vulkan support!");
        }

        std::vector<VkPhysicalDevice> devices(deviceCount);
        vkEnumeratePhysicalDevices(m_is.instance, &deviceCount, devices.data());

        for (const auto& device : devices) {
            if (isDeviceSuitable(device)) {
                m_devh.physicalDevice = device;
                break;
            }
        }

        if (m_devh.physicalDevice == VK_NULL_HANDLE) {
            throw std::runtime_error("failed to find a suitable GPU!");
        }
    }

    void createLogicalDevice() {
        QueueFamilyIndices indices = findQueueFamilies(m_devh.physicalDevice, m_is.surface);

        std::vector<VkDeviceQueueCreateInfo> queueCreateInfos;
        std::set<uint32_t> uniqueQueueFamilies = { indices.graphicsFamily.value(), indices.presentFamily.value() };

        float queuePriority = 1.0f;
        for (uint32_t queueFamily : uniqueQueueFamilies) {
            VkDeviceQueueCreateInfo queueCreateInfo{};
            queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
            queueCreateInfo.queueFamilyIndex = queueFamily;
            queueCreateInfo.queueCount = 1;
            queueCreateInfo.pQueuePriorities = &queuePriority;
            queueCreateInfos.push_back(queueCreateInfo);
        }

        VkPhysicalDeviceFeatures deviceFeatures{};

        VkDeviceCreateInfo createInfo{};
        createInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;

        createInfo.queueCreateInfoCount = static_cast<uint32_t>(queueCreateInfos.size());
        createInfo.pQueueCreateInfos = queueCreateInfos.data();

        createInfo.pEnabledFeatures = &deviceFeatures;

        createInfo.enabledExtensionCount = static_cast<uint32_t>(deviceExtensions.size());
        createInfo.ppEnabledExtensionNames = deviceExtensions.data();


#ifdef ENABLEVALIDATIONLAYERS
        createInfo.enabledLayerCount = static_cast<uint32_t>(validationLayers.size());
        createInfo.ppEnabledLayerNames = validationLayers.data();
#else
        createInfo.enabledLayerCount = 0;
#endif // ENABLEVALIDATIONLAYERS


        if (vkCreateDevice(m_devh.physicalDevice, &createInfo, nullptr, &m_devh.device) != VK_SUCCESS) {
            throw std::runtime_error("failed to create logical device!");
        }

        vkGetDeviceQueue(m_devh.device, indices.graphicsFamily.value(), 0, &m_queues.graphicsQueue);
        vkGetDeviceQueue(m_devh.device, indices.presentFamily.value(), 0, &m_queues.presentQueue);
    }

#pragma region DeviceChoice

bool isDeviceSuitable(VkPhysicalDevice device) {
    QueueFamilyIndices indices = findQueueFamilies(device, m_is.surface);

    bool extensionsSupported = checkDeviceExtensionSupport(device);

    bool swapChainAdequate = false;
    if (extensionsSupported) {
        SwapChainSupportDetails swapChainSupport = querySwapChainSupport(device, m_is.surface);
        swapChainAdequate = !swapChainSupport.formats.empty() && !swapChainSupport.presentModes.empty();
    }

    return indices.isComplete() && extensionsSupported && swapChainAdequate;
}

bool checkDeviceExtensionSupport(VkPhysicalDevice device) {
    uint32_t extensionCount;
    vkEnumerateDeviceExtensionProperties(device, nullptr, &extensionCount, nullptr);

    std::vector<VkExtensionProperties> availableExtensions(extensionCount);
    vkEnumerateDeviceExtensionProperties(device, nullptr, &extensionCount, availableExtensions.data());

    std::set<std::string> requiredExtensions(deviceExtensions.begin(), deviceExtensions.end());

    for (const auto& extension : availableExtensions) {
        requiredExtensions.erase(extension.extensionName);
    }

    return requiredExtensions.empty();
}

std::vector<const char*> getRequiredExtensions() {
    uint32_t glfwExtensionCount = 0;
    const char** glfwExtensions;
    glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);

    std::vector<const char*> extensions(glfwExtensions, glfwExtensions + glfwExtensionCount);


#ifdef ENABLEVALIDATIONLAYERS
    extensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
#endif // ENABLEVALIDATIONLAYERS


    return extensions;
}

#pragma endregion
};

int main() {
    HelloTriangleApplication app;

    LOG(RESOURCES_PATH)

    //app.run();
    PAUSE;

    return EXIT_SUCCESS;
}