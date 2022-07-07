#include "FakeBufferedFrameOutputDevice.h"

FakeBufferedFrameOutputDevice::FakeBufferedFrameOutputDevice(BufferedFrameOutputDevice::FrameCountType frameCount) noexcept
 : BufferedFrameOutputDevice(frameCount) {

}

FakeBufferedFrameOutputDevice::~FakeBufferedFrameOutputDevice() {

}

void FakeBufferedFrameOutputDevice::enqueueFrame(Frame&& frame) noexcept {
    //std::cout << "Frame enqueued, width: " << frame.getWidth() << ", height: " << frame.getHeight() << "";

    this->m_Frames.push_back(std::move(frame));

}

void FakeBufferedFrameOutputDevice::exec() noexcept {
    while (true) {
        auto possibly_frame = m_Frames.pop_front();
        if (possibly_frame.has_value()) {
            //std::cout << "got a frame!" << std::endl;
        } else {
            //std::this_thread::sleep_for(std::chrono::milliseconds(10));
        }

    }
}