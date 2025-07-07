#include "window.hpp"
#include "logger.hpp"

VulkanWindow::VulkanWindow(const uint32_t w, const uint32_t h) {
    width = w;
    height = h;
    init();
}

void VulkanWindow::init() {
    LOG_INFO("Initialising window...");

    // Initialise the GLFW library
    if (!glfwInit()) {
        throw std::runtime_error("failed to initialise GLFW");
    }

    // GLFW was originally designed to create an OpenGL context.
    // Here we tell it not to create an OpenGL context (since we're using Vulkan)
    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);

    // Disable window resizing
    glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);

    // Create the GLFW window. We're not using fullscreen (4th param) or OpenGL sharing (5th param)
    window = glfwCreateWindow(width, height, "Vulkan", nullptr, nullptr);
}

VkSurfaceKHR VulkanWindow::createSurface(VkInstance instance) {
    if (window == nullptr) {
        throw std::runtime_error("failed to create surface, window has not been created");
    }

    VkSurfaceKHR surface;
    if (glfwCreateWindowSurface(instance, window, nullptr, &surface) != VK_SUCCESS) {
        throw std::runtime_error("failed to create window surface");
    }
    return surface;
}

void VulkanWindow::pollEvents() const {

    if (window == nullptr) {
        throw std::runtime_error("cannot poll window events, window is null");
    }

    // Main window loop: poll events until the user closes the window
    while (!glfwWindowShouldClose(window)) {
        glfwPollEvents();
    }
}

std::vector<const char *> VulkanWindow::getRequiredInstanceExtensions() const {
    uint32_t glfwExtensionCount = 0;
    const char **glfwExtensions;
    glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);

    return std::vector<const char *>(glfwExtensions, glfwExtensions + glfwExtensionCount);
}

void VulkanWindow::getFrameBufferSize(int *width, int *height) const {
    glfwGetFramebufferSize(window, width, height);
}

GLFWwindow *VulkanWindow::getGLFWWindow() const {
    return window;
}

VulkanWindow::~VulkanWindow() {
    LOG_INFO("Shutting down window.");

    // Destroy the GLFW window if it was successfully created
    if (window != nullptr) {
        glfwDestroyWindow(window);
    }

    // Clean up and terminate the GLFW library
    glfwTerminate();
}
