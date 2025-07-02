#pragma once
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include <stdexcept>
#include <iostream>

const uint32_t WIDTH = 800;
const uint32_t HEIGHT = 600;

class VulkanWindow {
    public:
        VulkanWindow();
        ~VulkanWindow();

        void show();

    private:
        GLFWwindow* window;

        void init();
};
