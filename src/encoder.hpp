#pragma once
// #include <fstream>
// #include <iostream>
// #include <stdexcept>
#include <string>
// #include <vector>
#include "logger.hpp"
#include "renderer.hpp"
#include <vulkan/vulkan.h>

class VulkanRenderer;

/**
 * @class VulkanEncoder
 * @brief Encodes rendered frames from VulkanRenderer using Vulkan Video (H.264) and writes to file
 */
class VulkanEncoder {
  public:
    /**
     * @brief Constructs the encoder with a VulkanRenderer context and output path
     */
    VulkanEncoder(VulkanRenderer *renderer, const std::string &outputPath);

    /**
     * @brief Captures and encodes one frame
     */
    void encodeFrame();

    /**
     * @brief Finishes encoding and writes output
     */
    void finish();

    /**
     * @brief Destroys encoder resources
     */
    ~VulkanEncoder();

  protected:
    VulkanRenderer *renderer;
    std::string outputPath;

    VkDevice device;
    VkPhysicalDevice physicalDevice;
    VkQueue videoQueue;
    uint32_t videoQueueFamilyIndex;

    VkCommandPool commandPool;
    VkCommandBuffer commandBuffer;

    /**
     * @brief Framebuffer image source
     */
    VkImage srcImage;
    /**
     * @brief Converted image for encoding
     */
    VkImage dstImage;
    VkBuffer encodedOutputBuffer;
    VkDeviceMemory encodedMemory;

    VkVideoSessionKHR videoSession;
    VkVideoSessionParametersKHR videoSessionParams;
    VkVideoEncodeInfoKHR encodeInfo;

    /**
     * @brief Initialises encoding resources (session, buffers, command pool)
     */
    void init();

    /**
     * @brief Creates and configures the video encoding session
     */
    void createVideoSession();

    /**
     * @brief Creates the Vulkan command buffer used for encoding commands
     */
    void createCommandBuffer();

    /**
     * @brief Creates the output buffer where the encoded video data will be stored
     */
    void createOutputBuffer();

    /**
     * @brief Copies the framebuffer image data to a Vulkan image for encoding
     */
    void copyFramebufferToImage();

    /**
     * @brief Performs the actual video encoding process using the prepared buffers and session
     */
    void performEncoding();

    /**
     * @brief Saves the encoded video output buffer to disk
     */
    void saveEncodedOutput();

    /**
     * @brief Shuts down the encoder and releases resources
     */
    void shutdown();
};
