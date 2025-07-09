#include "encoder.hpp"
#include "logger.hpp"
#include "renderer.hpp"
#include "window.hpp"

/**
 * @brief Entry point for the VulkanTest application
 *
 * This function initialises the renderer and window,
 * enters the main event loop, and handles basic error reporting
 */
int main() {
// Print the current build mode to the console
#ifdef NDEBUG
    LOG_INFO("Mode: release");
#else
    LOG_INFO("Mode: debug");
#endif

    try {
        // Create the Vulkan renderer and window
        VulkanWindow window = VulkanWindow();
        VulkanRenderer renderer = VulkanRenderer(&window);

        // Show the window and start the event loop
        window.pollEvents([&]() { renderer.drawFrame(); });

        renderer.waitForLogicalDevices();
    } catch (std::exception &e) {
        // Print any exception message and exit with failure
        LOG_ERROR(e.what());
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}
