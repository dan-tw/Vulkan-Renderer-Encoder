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
    std::cout << "Mode: release\n";
#else
    std::cout << "Mode: debug\n";
#endif

    try {
        // Create the Vulkan renderer and window
        VulkanRenderer renderer = VulkanRenderer();
        VulkanWindow window = VulkanWindow();

        // Show the window and start the event loop
        window.show();
    } catch (std::exception &e) {
        // Print any exception message and exit with failure
        std::cerr << e.what() << std::endl;
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}
