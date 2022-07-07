#pragma once

#include "BufferedFrameOutputDevice.h"

#include <deque>
#include <semaphore>

class FakeBufferedFrameOutputDevice : public BufferedFrameOutputDevice {

public:
    FakeBufferedFrameOutputDevice(BufferedFrameOutputDevice::FrameCountType frameCount) noexcept;

    ~FakeBufferedFrameOutputDevice() override;

    void enqueueFrame(Frame&& frame) noexcept override;

    void exec() noexcept;

private:
    static constexpr std::ptrdiff_t MaxEnqueuedFrames = 512;

    std::counting_semaphore<MaxEnqueuedFrames> m_Semaphore;

    std::deque<Frame> m_Frames;
};
