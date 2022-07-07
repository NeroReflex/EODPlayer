#pragma once

#include "BufferedFrameOutputDevice.h"

#include "DequeHM.h"

class FakeBufferedFrameOutputDevice : public BufferedFrameOutputDevice {

public:
    FakeBufferedFrameOutputDevice(BufferedFrameOutputDevice::FrameCountType frameCount) noexcept;

    ~FakeBufferedFrameOutputDevice() override;

    void enqueueFrame(Frame&& frame) noexcept override;

    void exec() noexcept;

private:
    DequeHM<Frame> m_Frames;
};
