#pragma once
#include <vector>
#include <vulkan/vulkan.h>

/**
 * @brief Interface for objects that provide a Vulkan-compatible window surface
 */
class SurfaceProvider {
  public:
    virtual ~SurfaceProvider() = default;

    /**
     * @brief Create the platform-specific Vulkan surface for rendering
     * @param instance The Vulkan instance
     * @return A valid VkSurfaceKHR handle
     */
    virtual VkSurfaceKHR createSurface(VkInstance instance) = 0;

    /**
     * @brief Returns any Vulkan instance extensions required to support the surface
     */
    virtual std::vector<const char *> getRequiredInstanceExtensions() const = 0;

    /**
     * @brief Retrieves the current size of the framebuffer in pixels
     * @param width Reference to an integer that will receive the framebuffer width
     * @param height Reference to an integer that will receive the framebuffer height
     */
    virtual void getFrameBufferSize(int *width, int *height) const = 0;
};
