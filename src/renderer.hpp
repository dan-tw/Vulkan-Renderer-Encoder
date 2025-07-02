
#pragma once
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include <cstring>
#include <iostream>
#include <map>
#include <vector>

/// List of Vulkan validation layers to enable (if supported)
const std::vector<const char *> validationLayers = {"VK_LAYER_KHRONOS_validation"};

/// Whether to enable validation layers (only in debug builds)
#ifdef NDEBUG
const bool enableValidationLayers = false;
#else
const bool enableValidationLayers = true;
#endif

/**
 * @class VulkanRenderer
 * @brief Handles Vulkan initialisation, rendering, and frame capture.
 *
 * This class encapsulates the setup and teardown of Vulkan components,
 * and provides methods to render frames.
 */
class VulkanRenderer {

  public:
    /**
     * @brief Constructs the VulkanRenderer and initialising Vulkan resources
     */
    VulkanRenderer();

    /**
     * @brief Destructs the VulkanRenderer and cleans up all Vulkan resources
     */
    ~VulkanRenderer();

    void drawFrame();
    void captureFrame();

  protected:
    /// The Vulkan instance used by the renderer
    VkInstance instance;

    /// The debug messenger for Vulkan validation layer messages
    VkDebugUtilsMessengerEXT debugMessenger;

    VkPhysicalDevice physicalDevice = VK_NULL_HANDLE;

    /**
     * @brief Initialises Vulkan (instance, debug layers, etc.)
     */
    void init();

    /**
     * @brief Sets up the Vulkan debug messenger for validation callback
     */
    void setupDebugMessenger();

    /**
     * @brief Populates the debug messenger creation info struct
     * @param createInfo The struct to fill with layer callback info.
     */
    void populateDebugMessengerCreateInfo(VkDebugUtilsMessengerCreateInfoEXT &createInfo);

    /**
     * @brief Manually creates the debug messenger (if extension is available)
     * @param pCreateInfo Creation parameters
     * @param pAllocator Optional custom memory allocator
     * @return VK_SUCCESS or error code
     */
    VkResult createDebugUtilsMessengerEXT(const VkDebugUtilsMessengerCreateInfoEXT *pCreateInfo,
                                          const VkAllocationCallbacks *pAllocator);

    /**
     * @brief Manually destroys the debug messenger
     * @param pAllocator Optional custom memory allocator
     */
    void destroyDebugUtilsMessenger(const VkAllocationCallbacks *pAllocator);

    /**
     * @brief Creates the Vulkan instance
     */
    void createInstance();

    void pickPhysicalDevice();
    bool isDeviceSuitable(VkPhysicalDevice device);

    /**
     * @brief Checks if the requested validation layers are available
     * @return True if all requested layers are supported
     */
    bool checkValidationLayerSupport();

    /**
     * @brief Gets the required extensions for Vulkan initialisation
     * @return A list of extension names
     */
    std::vector<const char *> getRequiredExtensions();

    /**
     * @brief Shuts down the renderer and releases resources
     */
    void shutdown();

    /**
     * @brief Callback for Vulkan validation layer messages
     * @param messageSeverity Severity of the message
     * @param messageType Type of message (general, validation, performance)
     * @param pCallbackData Message details
     * @param pUserData Optional user data
     * @return VK_FALSE to indicate message should not abort execution
     */
    static VKAPI_ATTR VkBool32 VKAPI_CALL
    debugCallback(VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
                  VkDebugUtilsMessageTypeFlagsEXT messageType,
                  const VkDebugUtilsMessengerCallbackDataEXT *pCallbackData, void *pUserData);
};
