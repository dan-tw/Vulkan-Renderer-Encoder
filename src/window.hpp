#pragma once
#define GLFW_INCLUDE_VULKAN
#include "surfaceprovider.hpp"
#include <GLFW/glfw3.h>
#include <iostream>
#include <stdexcept>
#include <vector>

/// @brief Default window width in pixels
constexpr uint32_t WIDTH = 800;

/// @brief Default window height in pixels
constexpr uint32_t HEIGHT = 600;

/**
 * @class VulkanWindow
 * @brief Manages the creation and display of a GLFW window for Vulkan rendering
 *
 * This class handles window creation using GLFW and provides a basic interface
 * for initialising and showing the window. It is intended to work in tandem with
 * VulkanRenderer.
 */
class VulkanWindow : public SurfaceProvider {
  public:
    /**
     * @brief Constructs a VulkanWindow and initialises the GLFW window
     */
    VulkanWindow(const uint32_t w = WIDTH, const uint32_t h = HEIGHT);

    /**
     * @brief Destructs the VulkanWindow and releases GLFW resources
     */
    ~VulkanWindow();

    /**
     * @brief Starts the event loop for the window
     */
    void pollEvents() const;

    /**
     * @brief Creates a Vulkan-compatible surface from the GLFW window
     * @param instance The Vulkan instance to associate the surface with
     * @return The created VkSurfaceKHR handle
     */
    VkSurfaceKHR createSurface(VkInstance instance) override;

    /**
     * @brief Returns the required Vulkan instance extensions for GLFW-based surface creation
     * @return A vector of required extension names
     */
    std::vector<const char *> getRequiredInstanceExtensions() const override;

    /**
     * @brief Gets the raw GLFW window pointer
     * @return Pointer to the GLFWwindow created by this window class
     */
    GLFWwindow *getGLFWWindow() const;

  protected:
    /// @brief Pointer to the GLFW window handle
    GLFWwindow *window;

    /// @brief Width of the created window in pixels
    uint32_t width;

    /// @brief Height of the created window in pixels
    uint32_t height;

    /**
     * @brief Initialises GLFW and creates the window
     */
    void init();
};
