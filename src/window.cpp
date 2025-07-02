#include "window.hpp"

void VulkanWindow::init() {
    std::cout << "Initialising window..." << std::endl;

    // Initialise the GLFW library
    glfwInit();

    // GLFW was originally designed to create an OpenGL context. 
    // Here we tell it not to create an OpenGL context (since we're using Vulkan)
    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);

    // Disable window resizing
    glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);

    // Set the width, height and title of the window. The fourth param allows us to optionally
    // specify a monitor while the last param is used for OpenGL.
    window = glfwCreateWindow(WIDTH, HEIGHT, "Vulkan", nullptr, nullptr);
}

void VulkanWindow::show() {
    if(window == NULL) {
        throw std::runtime_error("Window is null");
    }

    while(!glfwWindowShouldClose(window)) {
        glfwPollEvents();
    }
}

VulkanWindow::VulkanWindow() {
    init();
}

VulkanWindow::~VulkanWindow() {
    std::cout << "Shutting down window." << std::endl;
    glfwDestroyWindow(window);
    glfwTerminate();
}
