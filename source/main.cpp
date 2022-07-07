#include "FFMPEGDecoder.h"
#include "FakeBufferedFrameOutputDevice.h"

#include <queue>
#include <cassert>

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

    size_t sizeInBytesOfLargestFrame = Frame::getPixelSizeInBytes(Frame::PixelFormat::RGBA64) * 1920 * 1080;

    std::queue<void*> fullHDFreeFramesMemory;
    for (size_t i = 0; i < 24; ++i) {
        fullHDFreeFramesMemory.push(malloc(sizeInBytesOfLargestFrame));
    }
    
    auto debugOutput = new FakeBufferedFrameOutputDevice(8);

    FFMPEGDecoder decoder(
        debugOutput,
        [&](size_t sz) -> void* {
            assert(sz <= sizeInBytesOfLargestFrame);

            assert(!fullHDFreeFramesMemory.empty());

            auto result = fullHDFreeFramesMemory.front();
            fullHDFreeFramesMemory.pop();
            return result;
        },
        [&](void* mem) {
            fullHDFreeFramesMemory.push(mem);
        }
    );

    decoder.loadFile("");
    decoder.play();
    
    // this is a blocking call
    debugOutput->exec();

    delete debugOutput;

    glfwTerminate();

    return EXIT_SUCCESS;
}
