
#pragma once
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include <iostream>

// #include <stdexcept>
// #include <cstdlib>

class VulkanRenderer {
public:
    VulkanRenderer();
    ~VulkanRenderer();

    void drawFrame();
    void captureFrame();

private:
    VkInstance instance;

    void init();
    void createInstance();
    void shutdown();
};
