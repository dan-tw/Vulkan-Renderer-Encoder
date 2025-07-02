#include "renderer.hpp"
#include "window.hpp"

int main() {

#ifdef NDEBUG
    std::cout << "Mode: release\n";
#else
    std::cout << "Mode: debug\n";
#endif

    try {
        VulkanRenderer renderer = VulkanRenderer();
        VulkanWindow window = VulkanWindow();

        window.show();
    } catch (std::exception &e) {
        std::cerr << e.what() << std::endl;
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}
