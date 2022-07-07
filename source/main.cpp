#include "EODPlayer.hpp"

/**
 * Entry point.
 *
 * @param   argc    command line arguments counter.
 * @param   argv    command line arguments.
 *
 * @return          execution exit code.
 */
int main(int argc, char * argv[])
{
    if (glfwInit() == 0) {
		return false;
	}

	glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);

    if (!glfwVulkanSupported()) {
        std::cerr << "Vulkan not supported. Exiting..." << std::endl;

        return EXIT_FAILURE;
    }
    
    glfwTerminate();

    return EXIT_SUCCESS;
}
