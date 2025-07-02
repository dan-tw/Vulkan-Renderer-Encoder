#include "window.hpp"

void VulkanWindow::init() {
    std::cout << "Initialising window..." << std::endl;

    // Initialise the GLFW library
    if (!glfwInit()) {
        throw std::runtime_error("failed to initialise GLFW");
    }

    // GLFW was originally designed to create an OpenGL context.
    // Here we tell it not to create an OpenGL context (since we're using Vulkan)
    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);

    // Disable window resizing
    glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);

    // Create the GLFW window. We’re not using fullscreen (4th param) or OpenGL sharing (5th param)
    window = glfwCreateWindow(WIDTH, HEIGHT, "Vulkan", nullptr, nullptr);
}

void VulkanWindow::show() {
    if (window == nullptr) {
        throw std::runtime_error("Window is null");
    }

    // Main window loop: poll events until the user closes the window
    while (!glfwWindowShouldClose(window)) {
        glfwPollEvents();
    }
}

VulkanWindow::VulkanWindow() {
    init();
}

VulkanWindow::~VulkanWindow() {
    std::cout << "Shutting down window." << std::endl;

    // Destroy the GLFW window if it was successfully created
    if (window != nullptr) {
        glfwDestroyWindow(window);
    }

    // Clean up and terminate the GLFW library
    glfwTerminate();
}
