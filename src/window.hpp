#pragma once
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include <iostream>
#include <stdexcept>

const uint32_t WIDTH = 800;
const uint32_t HEIGHT = 600;

class VulkanWindow {
  public:
    VulkanWindow();
    ~VulkanWindow();

    void show();

  protected:
    GLFWwindow *window;

    void init();
};
