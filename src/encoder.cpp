#include "encoder.hpp"
#include "renderer.hpp"

VulkanEncoder::VulkanEncoder(VulkanRenderer *renderer, const std::string &outputPath)
    : renderer(renderer), outputPath(outputPath) {
    init();
}

void VulkanEncoder::init() {
    LOG_INFO("Initialising Vulkan encoder...");
    device = renderer->getDevice();
    physicalDevice = renderer->getPhysicalDevice();
    LOG_INFO("Device handle: " + std::to_string(reinterpret_cast<uintptr_t>(device)));
    LOG_INFO("Physical Device handle: " +
             std::to_string(reinterpret_cast<uintptr_t>(physicalDevice)));

    // TODO: Select queue family with encode support and init videoQueue
    createVideoSession();
    createCommandBuffer();
    createOutputBuffer();
}

void VulkanEncoder::createVideoSession() {
    // TODO: Fill VkVideoSessionCreateInfoKHR and call vkCreateVideoSessionKHR
    // TODO: Create VkVideoSessionParametersKHR
}

void VulkanEncoder::createCommandBuffer() {
    VkCommandPoolCreateInfo poolInfo = {};
    poolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
    poolInfo.queueFamilyIndex = renderer->getGraphicsQueueFamilyIndex();
    poolInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;

    if (vkCreateCommandPool(device, &poolInfo, nullptr, &commandPool) != VK_SUCCESS) {
        throw std::runtime_error("failed to create command pool for encoder");
    }

    VkCommandBufferAllocateInfo allocInfo = {};
    allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    allocInfo.commandPool = commandPool;
    allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    allocInfo.commandBufferCount = 1;

    if (vkAllocateCommandBuffers(device, &allocInfo, &commandBuffer) != VK_SUCCESS) {
        throw std::runtime_error("failed to allocate encoder command buffer");
    }
}

void VulkanEncoder::createOutputBuffer() {
    VkBufferCreateInfo bufferInfo = {};
    bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
    bufferInfo.size = 10 * 1024 * 1024; // 10MB buffer for encoded output
    bufferInfo.usage = VK_BUFFER_USAGE_TRANSFER_DST_BIT;
    bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

    if (vkCreateBuffer(device, &bufferInfo, nullptr, &encodedOutputBuffer) != VK_SUCCESS) {
        throw std::runtime_error("failed to create encoded output buffer");
    }

    VkMemoryRequirements memRequirements;
    vkGetBufferMemoryRequirements(device, encodedOutputBuffer, &memRequirements);

    VkMemoryAllocateInfo allocInfo = {};
    allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
    allocInfo.allocationSize = memRequirements.size;
    allocInfo.memoryTypeIndex = 0;

    VkPhysicalDeviceMemoryProperties memProperties;
    vkGetPhysicalDeviceMemoryProperties(physicalDevice, &memProperties);
    for (uint32_t i = 0; i < memProperties.memoryTypeCount; i++) {
        if ((memRequirements.memoryTypeBits & (1 << i)) &&
            (memProperties.memoryTypes[i].propertyFlags & VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT)) {
            allocInfo.memoryTypeIndex = i;
            break;
        }
    }

    if (vkAllocateMemory(device, &allocInfo, nullptr, &encodedMemory) != VK_SUCCESS) {
        throw std::runtime_error("failed to allocate output buffer memory");
    }

    vkBindBufferMemory(device, encodedOutputBuffer, encodedMemory, 0);
}

void VulkanEncoder::copyFramebufferToImage() {
    // TODO: Transition and copy image from framebuffer to format supported by encoder
}

void VulkanEncoder::performEncoding() {
    // TODO: Fill VkVideoEncodeInfoKHR and submit it via vkCmdEncodeVideoKHR
}

void VulkanEncoder::encodeFrame() {
    copyFramebufferToImage();
    performEncoding();
}

void VulkanEncoder::saveEncodedOutput() {
    void *data;
    vkMapMemory(device, encodedMemory, 0, VK_WHOLE_SIZE, 0, &data);

    std::ofstream file(outputPath, std::ios::out | std::ios::binary);
    if (!file.is_open()) {
        throw std::runtime_error("failed to open output file for writing");
    }

    file.write(static_cast<char *>(data), 1024); // Replace size with actual size after encoding

    file.close();
    vkUnmapMemory(device, encodedMemory);
}

void VulkanEncoder::finish() {
    saveEncodedOutput();
}

void VulkanEncoder::shutdown() {
    if (encodedOutputBuffer != VK_NULL_HANDLE) {
        vkDestroyBuffer(device, encodedOutputBuffer, nullptr);
    }
    if (encodedMemory != VK_NULL_HANDLE) {
        vkFreeMemory(device, encodedMemory, nullptr);
    }
    if (commandPool != VK_NULL_HANDLE) {
        vkDestroyCommandPool(device, commandPool, nullptr);
    }
}

VulkanEncoder::~VulkanEncoder() {
    shutdown();
}
