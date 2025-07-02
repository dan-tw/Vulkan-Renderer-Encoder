
#pragma once
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include <cstring>
#include <iostream>
#include <vector>

// #include <stdexcept>
// #include <cstdlib>

const std::vector<const char *> validationLayers = {"VK_LAYER_KHRONOS_validation"};

#ifdef NDEBUG
const bool enableValidationLayers = false;
#else
const bool enableValidationLayers = true;
#endif

class VulkanRenderer {
  public:
    VulkanRenderer();
    ~VulkanRenderer();

    void drawFrame();
    void captureFrame();

  protected:
    VkInstance instance;

    void init();
    void createInstance();
    bool checkValidationLayerSupport();
    void shutdown();
};
