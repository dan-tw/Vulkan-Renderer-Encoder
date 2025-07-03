
#pragma once
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include <cstring>
#include <iostream>
#include <map>
#include <optional>
#include <vector>

/// @brief List of Vulkan validation layers to enable (if supported)
const std::vector<const char *> validationLayers = {"VK_LAYER_KHRONOS_validation"};

/// @brief Whether to enable validation layers (only in debug builds)
#ifdef NDEBUG
const bool enableValidationLayers = false;
#else
const bool enableValidationLayers = true;
#endif

/**
 * @brief Holds indices of queue families supported by a physical device
 *
 * This struct is used to track which queue families (e.g. graphics, compute, etc.)
 * are available on a Vulkan physical device. Only the graphics queue family is
 * currently required and checked.
 */
struct QueueFamilyIndices {
    /// @brief Index of a queue family that supports graphics commands
    std::optional<uint32_t> graphicsFamily;

    /**
     * @brief Checks if all required queue families have been found
     *
     * @return true if the graphics queue family has been assigned
     */
    bool isComplete() {
        return graphicsFamily.has_value();
    }
};

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
    /// @brief The Vulkan instance used by the renderer
    VkInstance instance;

    /// @brief The debug messenger for Vulkan validation layer messages
    VkDebugUtilsMessengerEXT debugMessenger;

    /// @brief The selected physical GPU device
    VkPhysicalDevice physicalDevice = VK_NULL_HANDLE;

    /**
     * @brief The logical device created from the selected physical GPU
     */
    VkDevice device;

    /**
     * @brief Graphics queue retrieved from the logical device
     */
    VkQueue graphicsQueue;

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

    /**
     * @brief Picks a suitable physical GPU device that supports required features
     *
     * Enumerates all Vulkan-compatible devices and selects the first one that meets
     * application requirements
     *
     * @throws std::runtime_error if no suitable GPU is found
     */
    void pickPhysicalDevice();

    /**
     * @brief Creates the Vulkan logical device and retrieves the graphics queue
     *
     * This function selects the appropriate queue family from the physical device,
     * enables required features, and creates a logical device interface
     *
     * @throws std::runtime_error if device creation fails.
     */
    void createLogicalDevice();

    /**
     * @brief Checks if a given physical device is suitable for use
     *
     * Currently checks for graphics queue family support
     *
     * @param device The physical device to evaluate
     * @return true if the device meets the required criteria
     */
    bool isDeviceSuitable(VkPhysicalDevice device);

    /**
     * @brief Finds queue families that support required capabilities on a device
     *
     * Searches for queue families that support graphics commands
     *
     * @param device The physical device to query
     * @return A QueueFamilyIndices struct with matching queue families
     */
    QueueFamilyIndices findQueueFamilies(VkPhysicalDevice device);

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
