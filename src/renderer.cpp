#include "renderer.hpp"

VulkanRenderer::VulkanRenderer(SurfaceProvider *surfaceProvider)
    : surfaceProvider(surfaceProvider) {
    init();
}

VkInstance VulkanRenderer::getInstance() const {
    return instance;
}

void VulkanRenderer::init() {
    LOG_INFO("Initialising Vulkan renderer...");

    // Create the Vulkan instance (entry point into the Vulkan API)
    createInstance();

    // Set up the debug messenger (if validation layers are enabled)
    setupDebugMessenger();

    // If we have a surface provider, create and attach the VK surface
    if (surfaceProvider) {
        surface = surfaceProvider->createSurface(instance);
        if (surface != VK_NULL_HANDLE) {
            LOG_DEBUG("VK surface attached. Enabling extension " +
                      std::string(VK_KHR_SWAPCHAIN_EXTENSION_NAME));
            deviceExtensions.push_back(VK_KHR_SWAPCHAIN_EXTENSION_NAME);
        }
    }

    // Pick the physical device
    pickPhysicalDevice();

    // Create a logical device from the physical device
    createLogicalDevice();
}

void VulkanRenderer::setupDebugMessenger() {
    if (!enableValidationLayers) {
        return;
    }

    VkDebugUtilsMessengerCreateInfoEXT createInfo = {};
    populateDebugMessengerCreateInfo(createInfo);

    // Create the Vulkan debug messenger extension if available
    if (createDebugUtilsMessengerEXT(&createInfo, nullptr) != VK_SUCCESS) {
        throw std::runtime_error("failed to set up debug messenger");
    }
}

void VulkanRenderer::populateDebugMessengerCreateInfo(
    VkDebugUtilsMessengerCreateInfoEXT &createInfo) {
    createInfo = {};
    createInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;

    // Configure what severity levels will trigger the callback
    createInfo.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT |
                                 VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT |
                                 VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;

    // Configure which message types will be handled
    createInfo.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT |
                             VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT |
                             VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;

    createInfo.pfnUserCallback = debugCallback;
    createInfo.pUserData = nullptr; // Optional user-specific data
}

VkResult
VulkanRenderer::createDebugUtilsMessengerEXT(const VkDebugUtilsMessengerCreateInfoEXT *pCreateInfo,
                                             const VkAllocationCallbacks *pAllocator) {
    // Query function pointer to the extension function (not loaded by default)
    auto func = (PFN_vkCreateDebugUtilsMessengerEXT)vkGetInstanceProcAddr(
        instance, "vkCreateDebugUtilsMessengerEXT");
    if (func != nullptr) {
        return func(instance, pCreateInfo, pAllocator, &debugMessenger);
    } else {
        return VK_ERROR_EXTENSION_NOT_PRESENT;
    }
}

void VulkanRenderer::destroyDebugUtilsMessenger(const VkAllocationCallbacks *pAllocator) {
    // Destroy the debug messenger via its extension
    auto func = (PFN_vkDestroyDebugUtilsMessengerEXT)vkGetInstanceProcAddr(
        instance, "vkDestroyDebugUtilsMessengerEXT");
    if (func != nullptr) {
        func(instance, debugMessenger, pAllocator);
    }
}

void VulkanRenderer::createInstance() {
    // Fail early if validation layers are requested but unavailable
    if (enableValidationLayers && !checkValidationLayerSupport()) {
        throw std::runtime_error("validation layers requested but not available");
    }

    // Fill in application info (optional, but helps some drivers optimise)
    VkApplicationInfo appInfo = {};
    appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
    appInfo.pApplicationName = "Vulkan Renderer and Encoder";
    appInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
    appInfo.pEngineName = "No Engine";
    appInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
    appInfo.apiVersion = VK_API_VERSION_1_3;

    // Fill in instance creation info
    VkInstanceCreateInfo createInfo = {};
    createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
    createInfo.pApplicationInfo = &appInfo;

    // Vulkan is a platform agnostic API, which means we need to pass an extension interface.
    // List required extensions (e.g. from GLFW)
    auto extensions = getRequiredExtensions();
    createInfo.enabledExtensionCount = static_cast<uint32_t>(extensions.size());
    createInfo.ppEnabledExtensionNames = extensions.data();

    // Enable validation layers if requested (when running in debug mode)
    VkDebugUtilsMessengerCreateInfoEXT debugCreateInfo = {};
    if (enableValidationLayers) {
        createInfo.enabledLayerCount = static_cast<uint32_t>(validationLayers.size());
        createInfo.ppEnabledLayerNames = validationLayers.data();

        // Hook debug messenger creation into instance creation
        populateDebugMessengerCreateInfo(debugCreateInfo);
        createInfo.pNext = (VkDebugUtilsMessengerCreateInfoEXT *)&debugCreateInfo;
    } else {
        createInfo.enabledLayerCount = 0;
        createInfo.pNext = nullptr;
    }

    // Finally, create the Vulkan instance
    if (vkCreateInstance(&createInfo, nullptr, &instance) != VK_SUCCESS) {
        throw std::runtime_error("failed to create vulkan instance");
    }
}

void VulkanRenderer::pickPhysicalDevice() {
    uint32_t deviceCount = 0;
    vkEnumeratePhysicalDevices(instance, &deviceCount, nullptr);

    if (deviceCount == 0) {
        throw std::runtime_error("failed to find GPUs with Vulkan support");
    }

    // Retrieve all physical devices
    std::vector<VkPhysicalDevice> devices(deviceCount);
    vkEnumeratePhysicalDevices(instance, &deviceCount, devices.data());

    // Select the first suitable physical device
    for (const auto &device : devices) {
        if (isDeviceSuitable(device)) {
            physicalDevice = device;
            break;
        }
    }

    if (physicalDevice == VK_NULL_HANDLE) {
        throw std::runtime_error("failed to find a suitable GPU");
    }

    // Log the selected device name
    VkPhysicalDeviceProperties deviceProperties;
    vkGetPhysicalDeviceProperties(physicalDevice, &deviceProperties);

    std::string deviceName = deviceProperties.deviceName;
    LOG_INFO("Using device " + deviceName);
}

void VulkanRenderer::createLogicalDevice() {
    // Find queue family indices for the selected physical device
    QueueFamilyIndices indicies = findQueueFamilies(physicalDevice);

    std::vector<VkDeviceQueueCreateInfo> queueCreateInfos;
    std::set<uint32_t> uniqueQueueFamilies = {indicies.graphicsFamily.value()};

    // If surface is attached insert present queue family
    if (surface != VK_NULL_HANDLE) {
        uniqueQueueFamilies.insert(indicies.presentFamily.value());
    }

    // Iterate over queue families and fill queue create info
    float queuePriority = 1.0f;
    for (uint32_t queueFamily : uniqueQueueFamilies) {
        VkDeviceQueueCreateInfo queueCreateInfo{};
        queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
        queueCreateInfo.queueFamilyIndex = queueFamily;
        queueCreateInfo.queueCount = 1;
        queueCreateInfo.pQueuePriorities = &queuePriority;
        queueCreateInfos.push_back(queueCreateInfo);
    }

    // Enable required device features (none for now)
    VkPhysicalDeviceFeatures deviceFeatures = {};

    // Fill in the device creation info
    VkDeviceCreateInfo createInfo = {};
    createInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
    createInfo.pQueueCreateInfos = queueCreateInfos.data();
    createInfo.queueCreateInfoCount = static_cast<uint32_t>(queueCreateInfos.size());

    createInfo.pEnabledFeatures = &deviceFeatures;

    // Enable device extensions (e.g. H.264 encoding)
    createInfo.enabledExtensionCount = static_cast<uint32_t>(deviceExtensions.size());
    createInfo.ppEnabledExtensionNames = deviceExtensions.data();

    // Enable validation layers if requested
    if (enableValidationLayers) {
        createInfo.enabledLayerCount = static_cast<uint32_t>(validationLayers.size());
        createInfo.ppEnabledLayerNames = validationLayers.data();
    } else {
        createInfo.enabledLayerCount = 0;
    }

    // Create the logical device
    if (vkCreateDevice(physicalDevice, &createInfo, nullptr, &device) != VK_SUCCESS) {
        throw std::runtime_error("failed to create logical device");
    }

    // Retrieve a handle to the graphics queue from the created device
    vkGetDeviceQueue(device, indicies.graphicsFamily.value(), 0, &graphicsQueue);

    // Optionally retrieve handle to the present queue from the created device
    if (surface != VK_NULL_HANDLE) {
        vkGetDeviceQueue(device, indicies.presentFamily.value(), 0, &presentQueue);
    }
}

bool VulkanRenderer::isDeviceSuitable(VkPhysicalDevice device) {
    VkPhysicalDeviceProperties deviceProperties;
    vkGetPhysicalDeviceProperties(device, &deviceProperties);
    std::string deviceName = deviceProperties.deviceName;

    QueueFamilyIndices indicies = findQueueFamilies(device);
    bool extensionsSupported = checkDeviceExtensionSupport(device);

    if (!indicies.isComplete(surface)) {
        LOG_WARN("Device " + std::string(deviceProperties.deviceName) +
                 " is missing required queue families");
    }

    if (!extensionsSupported) {
        LOG_WARN("Device " + std::string(deviceProperties.deviceName) +
                 " is missing required extensions");
    }

    return indicies.isComplete(surface) && extensionsSupported;
}

bool VulkanRenderer::checkDeviceExtensionSupport(VkPhysicalDevice device) {
    uint32_t extensionCount;
    vkEnumerateDeviceExtensionProperties(device, nullptr, &extensionCount, nullptr);

    std::vector<VkExtensionProperties> availableExtensions(extensionCount);
    vkEnumerateDeviceExtensionProperties(device, nullptr, &extensionCount,
                                         availableExtensions.data());

    std::set<std::string> requiredExtensions(deviceExtensions.begin(), deviceExtensions.end());
    for (const auto &extension : availableExtensions) {
        LOG_VERBOSE("Found device extension " + std::string(extension.extensionName));

        std::size_t erased = requiredExtensions.erase(extension.extensionName);
        if (erased > 0) {
            LOG_DEBUG("Found required device extension " + std::string(extension.extensionName));
        }
    }

    if (!requiredExtensions.empty()) {
        for (const auto &missing : requiredExtensions) {
            LOG_WARN("Missing device extension: " + std::string(missing));
        }
    }

    return requiredExtensions.empty();
}

VulkanRenderer::QueueFamilyIndices VulkanRenderer::findQueueFamilies(VkPhysicalDevice device) {
    QueueFamilyIndices indices;

    uint32_t queueFamilyCount = 0;
    vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, nullptr);

    std::vector<VkQueueFamilyProperties> queueFamilies(queueFamilyCount);
    vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, queueFamilies.data());

    for (std::size_t i = 0; i < queueFamilies.size(); ++i) {
        // Look for a queue family that supports graphics commands
        if (queueFamilies[i].queueFlags & VK_QUEUE_GRAPHICS_BIT) {
            indices.graphicsFamily = i;
        }

        // Check to see if present support is available for the device.
        // Only needed if we have a surface attached
        if (surface != VK_NULL_HANDLE) {
            VkBool32 presentSupport = false;
            vkGetPhysicalDeviceSurfaceSupportKHR(device, i, surface, &presentSupport);

            if (presentSupport) {
                indices.presentFamily = i;
            }
        }

        // Stop early if all required queues are found
        if (indices.isComplete(surface)) {
            break;
        }
    }

    return indices;
}

bool VulkanRenderer::checkValidationLayerSupport() {
    uint32_t layerCount;
    vkEnumerateInstanceLayerProperties(&layerCount, nullptr);

    std::vector<VkLayerProperties> availableLayers(layerCount);
    vkEnumerateInstanceLayerProperties(&layerCount, availableLayers.data());

    // Check if all requested validation layers are available
    for (const char *layerName : validationLayers) {
        bool layerFound = false;

        for (const auto &layerProperties : availableLayers) {
            if (strcmp(layerName, layerProperties.layerName) == 0) {
                layerFound = true;
                break;
            }
        }

        if (!layerFound) {
            return false;
        }
    }

    return true;
}

void VulkanRenderer::drawFrame() {
    // Draw something to the framebuffer
}

void VulkanRenderer::captureFrame() {
    // Copy framebuffer to memory or export image
}

std::vector<const char *> VulkanRenderer::getRequiredExtensions() {
    std::vector<const char *> extensions;

    if (surfaceProvider) {
        extensions = surfaceProvider->getRequiredInstanceExtensions();
    }

    // Add debug utils extension if validation is enabled
    if (enableValidationLayers) {
        extensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
    }

    return extensions;
}

void VulkanRenderer::shutdown() {
    LOG_INFO("Shutting down Vulkan renderer.");

    if (enableValidationLayers) {
        destroyDebugUtilsMessenger(nullptr);
    }

    vkDestroyDevice(device, nullptr);

    if (surface != VK_NULL_HANDLE) {
        vkDestroySurfaceKHR(instance, surface, nullptr);
    }

    vkDestroyInstance(instance, nullptr);
}

VulkanRenderer::~VulkanRenderer() {
    shutdown();
}

VKAPI_ATTR VkBool32 VKAPI_CALL VulkanRenderer::debugCallback(
    VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
    VkDebugUtilsMessageTypeFlagsEXT messageType,
    const VkDebugUtilsMessengerCallbackDataEXT *pCallbackData, void *pUserData) {

    // Map enum values to strings for readable output
    std::map<VkDebugUtilsMessageTypeFlagsEXT, std::string> messageTypes{
        {VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT, "General"},
        {VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT, "Performance"},
        {VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT, "Validation"}};
    std::map<VkDebugUtilsMessageSeverityFlagBitsEXT, std::string> messageSeverityTypes{
        {VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT, "Verbose"},
        {VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT, "Info"},
        {VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT, "Warning"},
        {VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT, "Error"}};

    // Log formatted debug message at relevant log-level
    std::string message = "SEVERITY = " + messageSeverityTypes[messageSeverity] +
                          ", TYPE = " + messageTypes[messageType] + ": " + pCallbackData->pMessage;
    switch (messageSeverity) {
    case VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT:
        LOG_VERBOSE(message);
        if (pUserData != nullptr) {
            LOG_VERBOSE("User data: " + std::string((char *)pUserData));
        }
        break;
    case VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT:
        LOG_INFO(message);
        if (pUserData != nullptr) {
            LOG_INFO("User data: " + std::string((char *)pUserData));
        }
        break;
    case VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT:
        LOG_WARN(message);
        if (pUserData != nullptr) {
            LOG_WARN("User data: " + std::string((char *)pUserData));
        }
        break;
    case VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT:
        LOG_ERROR(message);
        if (pUserData != nullptr) {
            LOG_ERROR("User data: " + std::string((char *)pUserData));
        }
        break;
    default:
        LOG_INFO(message);
        if (pUserData != nullptr) {
            LOG_INFO("User data: " + std::string((char *)pUserData));
        }
    }

    return VK_FALSE; // Always return false to not interrupt Vulkan calls
}
