#include "renderer.hpp"
#include "window.hpp"

int main() {

    try {
        VulkanRenderer renderer = VulkanRenderer();
        VulkanWindow window = VulkanWindow();
        
        window.show();
    } catch(std::exception& e) {
        std::cerr << e.what() << std::endl;
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}
