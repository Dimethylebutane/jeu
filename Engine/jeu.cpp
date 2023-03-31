// jeu.cpp : Defines the entry point for the application.
//

#include "jeu.h"

#define TINYOBJLOADER_IMPLEMENTATION
#include <tiny_obj_loader.h>

class HelloTriangleApplication {
public:
    void run() {
        initWindow(); //glfw stuff
        initVulkan(); //vulkan stuff
        initEngine(); //Engine static Structure stuff

        createGameStruct();

        mainLoop();
        cleanup();
    }

private:
#pragma region GameEngineProperties

    GLFWwindow* window;

#ifdef ENABLEVALIDATIONLAYERS
    VkDebugUtilsMessengerEXT debugMessenger;
#endif // !NDEBUG

    ISHandler m_is;

    DeviceHandler m_devh{ VK_NULL_HANDLE,  VK_NULL_HANDLE };

    QueueHandler m_queues;

    SwapChain m_swapchain;

    VkCommandPool m_commandPool;

#pragma endregion

    //VkRenderPass drawMainRenderPass; //todo: renderPass and model and stuff

    Camera m_defaultCam;
    SkBx m_defaultSkBx;

    std::vector<VkSemaphore> imageAvailableSemaphores;
    std::vector<VkSemaphore> renderFinishedSemaphores;

    void initWindow() {
        glfwInit();

        glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);

        window = glfwCreateWindow(WIDTH, HEIGHT, "Jeu", nullptr, nullptr);
        glfwSetWindowUserPointer(window, this);
        glfwSetFramebufferSizeCallback(window, framebufferResizeCallback);

        //input parameter
        glfwSetInputMode(window, GLFW_STICKY_KEYS, GLFW_TRUE);

        if (glfwRawMouseMotionSupported())
            glfwSetInputMode(window, GLFW_RAW_MOUSE_MOTION, GLFW_TRUE);
    }

    bool framebufferResized = false; //framebuffer resize callback
    static void framebufferResizeCallback(GLFWwindow* window, int width, int height) {
        auto app = reinterpret_cast<HelloTriangleApplication*>(glfwGetWindowUserPointer(window));
        app->framebufferResized = true;
    }

    void initVulkan()
    {
        createInstance();

#ifdef ENABLEVALIDATIONLAYERS
        setupDebugMessenger(m_is.instance, debugMessenger);
#endif // !ENABLEVALIDATIONLAYERS

        createSurface();
        pickPhysicalDevice();
        createLogicalDevice();

        m_swapchain.init(window, m_devh, m_is.surface, swapChainWantedParam);
        m_commandPool = createCommandPool(m_devh, m_is.surface);
    }

    void initEngine()
    {
        createFirstSemaphore();

        Camera::InitCamerasClass((uint32_t)m_swapchain.imageData.size(), m_devh);

        SkBx::InitSkBxStruct(m_swapchain.param, m_queues.graphicsQueue, m_commandPool, m_devh);
    }

    //TODO: virtual and override this function when engine will be separated from game
    void createGameStruct()
    {
        m_defaultCam.init(static_cast<char>(m_swapchain.imageData.size()));
        m_defaultCam.m_data.proj = glm::perspective(glm::radians(45.0f), m_swapchain.param.extent.width / (float)m_swapchain.param.extent.height, 0.1f, 10.0f);
        m_defaultCam.m_data.proj[1][1] *= -1;
        m_defaultCam.updateCamPosition(glm::vec3(0, 0, 0));
        m_defaultCam.lookAt(glm::vec3(1, 0, 0));

        m_defaultSkBx.init(m_defaultCam, m_swapchain, m_commandPool, m_devh.device);

        //createMainDrawRenderPass();


        //TODO: pipeline creation and Model renderer
        //TODO: text renderer
    }

    void mainLoop() {
        while (!glfwWindowShouldClose(window)) {
            glfwPollEvents();
            drawFrame();
        }

        vkDeviceWaitIdle(m_devh.device);
    }

    uint32_t currentFrame = 0; // % max_frame_in_flight
    void drawFrame()
    {
        uint32_t currentImageIndex = 0;

        VkResult result = m_swapchain.getNextImage(currentFrame, imageAvailableSemaphores[currentFrame], currentImageIndex, m_devh.device);

        if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR) {
            throw std::runtime_error("failed to acquire swap chain image!");
        }

        static auto startTime = std::chrono::high_resolution_clock::now();

        auto currentTime = std::chrono::high_resolution_clock::now();
        float time = std::chrono::duration<float, std::chrono::seconds::period>(currentTime - startTime).count();
        startTime = currentTime;
        m_defaultCam.rotateCamera({ 0.f, 1.f, 0.f }, time * glm::pi<float>());

        m_defaultCam.updateBuffer(currentImageIndex);

        vkResetFences(m_devh.device, 1, &(m_swapchain.fences[currentFrame]));

        VkSubmitInfo submitInfo{};
        submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;

        VkSemaphore waitSemaphores[] = { imageAvailableSemaphores[currentFrame] };
        VkPipelineStageFlags waitStages[] = { VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT };
        submitInfo.waitSemaphoreCount = 1;
        submitInfo.pWaitSemaphores = waitSemaphores;
        submitInfo.pWaitDstStageMask = waitStages;

        submitInfo.commandBufferCount = 1;
        submitInfo.pCommandBuffers = &m_defaultSkBx.commandBuffers[currentImageIndex]; //one per image not frame

        VkSemaphore signalSemaphores[] = { renderFinishedSemaphores[currentFrame] };
        submitInfo.signalSemaphoreCount = 1;
        submitInfo.pSignalSemaphores = signalSemaphores;

        if (vkQueueSubmit(m_queues.graphicsQueue, 1, &submitInfo, (m_swapchain.fences[currentFrame])) != VK_SUCCESS) {
            throw std::runtime_error("failed to submit draw command buffer!");
        }

        VkPresentInfoKHR presentInfo{};
        presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;

        presentInfo.waitSemaphoreCount = 1;
        presentInfo.pWaitSemaphores = signalSemaphores;

        VkSwapchainKHR swapChains[] = { m_swapchain.vkSwapChain };
        presentInfo.swapchainCount = 1;
        presentInfo.pSwapchains = swapChains;

        presentInfo.pImageIndices = &currentImageIndex;

        result = vkQueuePresentKHR(m_queues.presentQueue, &presentInfo);

        if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR || framebufferResized) {
            framebufferResized = false;
            recreateSwapChain();
        }
        else if (result != VK_SUCCESS) {
            throw std::runtime_error("failed to present swap chain image!");
        }

        currentFrame = (currentFrame + 1) % MAX_FRAMES_IN_FLIGHT;
    }

    void cleanup() {

        for (auto s : imageAvailableSemaphores)
            vkDestroySemaphore(m_devh.device, s, nullptr);

        for (auto s : renderFinishedSemaphores)
            vkDestroySemaphore(m_devh.device, s, nullptr);

        m_swapchain.cleanUp(m_devh.device);

        //vkDestroyPipeline(m_devh.device, graphicsPipeline, nullptr);
        //vkDestroyPipelineLayout(m_devh.device, pipelineLayout, nullptr);

        m_defaultSkBx.free(m_commandPool, m_devh.device);
        m_defaultCam.clean();

        /*for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
            vkDestroySemaphore(m_devh.device, renderFinishedSemaphores[i], nullptr);
            vkDestroySemaphore(m_devh.device, imageAvailableSemaphores[i], nullptr);
            vkDestroyFence(m_devh.device, inFlightFences[i], nullptr);
        }*/

        SkBx::CleanSkBxStruct(m_devh.device);

        vkDestroyCommandPool(m_devh.device, m_commandPool, nullptr);

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

        m_swapchain.recreate(window, m_devh, m_is.surface, swapChainWantedParam);

        m_defaultSkBx.recreate(m_swapchain, m_defaultCam, m_devh.device);
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

    void createFirstSemaphore()
    {
        imageAvailableSemaphores.resize(MAX_FRAMES_IN_FLIGHT);
        renderFinishedSemaphores.resize(MAX_FRAMES_IN_FLIGHT);

        VkSemaphoreCreateInfo semaphoreInfo{};
        semaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

        for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
            if (vkCreateSemaphore(m_devh.device, &semaphoreInfo, nullptr, &imageAvailableSemaphores[i]) != VK_SUCCESS ||
                vkCreateSemaphore(m_devh.device, &semaphoreInfo, nullptr, &renderFinishedSemaphores[i]) != VK_SUCCESS)
            {
                throw std::runtime_error("failed to create synchronization objects for a frame!");
            }
        }
    }

    /*
    void createMainDrawRenderPass() {
        VkAttachmentDescription colorAttachment{};
        colorAttachment.format = m_swapchain.param.imageFormat.format;
        colorAttachment.samples = VK_SAMPLE_COUNT_1_BIT;
        colorAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_LOAD;
        colorAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
        colorAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
        colorAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
        colorAttachment.initialLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
        colorAttachment.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

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
        renderPassInfo.pAttachments = &colorAttachment;
        renderPassInfo.subpassCount = 1;
        renderPassInfo.pSubpasses = &subpass;
        renderPassInfo.dependencyCount = 1;
        renderPassInfo.pDependencies = &dependency;

        if (vkCreateRenderPass(m_devh.device, &renderPassInfo, nullptr, &drawMainRenderPass) != VK_SUCCESS) {
            throw std::runtime_error("failed to create render pass!");
        }
    }
    */

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

        app.run();
    PAUSE;

    return EXIT_SUCCESS;
}
