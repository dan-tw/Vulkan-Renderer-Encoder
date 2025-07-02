#pragma once
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include <iostream>
#include <stdexcept>

/// Default window width in pixels
constexpr uint32_t WIDTH = 800;

/// Default window height in pixels
constexpr uint32_t HEIGHT = 600;

/**
 * @class VulkanWindow
 * @brief Manages the creation and display of a GLFW window for Vulkan rendering
 *
 * This class handles window creation using GLFW and provides a basic interface
 * for initialising and showing the window. It is intended to work in tandem with
 * VulkanRenderer.
 */
class VulkanWindow {
  public:
    /**
     * @brief Constructs a VulkanWindow and initialises the GLFW window
     */
    VulkanWindow();

    /**
     * @brief Destructs the VulkanWindow and releases GLFW resources
     */
    ~VulkanWindow();

    /**
     * @brief Displays the window and starts the event loop
     */
    void show();

  protected:
    /// Pointer to the GLFW window handle
    GLFWwindow *window;

    /**
     * @brief Initialises GLFW and creates the window
     */
    void init();
};
