#include "FakeBufferedFrameOutputDevice.h"

FakeBufferedFrameOutputDevice::FakeBufferedFrameOutputDevice(BufferedFrameOutputDevice::FrameCountType frameCount) noexcept
 : BufferedFrameOutputDevice(frameCount),
 m_Semaphore(MaxEnqueuedFrames) {

}

FakeBufferedFrameOutputDevice::~FakeBufferedFrameOutputDevice() {

}

void FakeBufferedFrameOutputDevice::enqueueFrame(Frame&& frame) noexcept {
    this->m_Frames.emplace_back(std::move(frame));

    m_Semaphore.release();
}

void FakeBufferedFrameOutputDevice::exec() noexcept {
    while (true) {
        m_Semaphore.acquire();

        auto frame = std::move(m_Frames.front());
        m_Frames.pop_front();

        std::cout << "got a frame!" << std::endl;
    }
}