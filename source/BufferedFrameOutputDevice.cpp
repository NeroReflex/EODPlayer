#include "BufferedFrameOutputDevice.h"

BufferedFrameOutputDevice::BufferedFrameOutputDevice(
    FrameCountType frames
) noexcept
    : m_FramesCount(frames) {

}

BufferedFrameOutputDevice::~BufferedFrameOutputDevice() {
    
}

BufferedFrameOutputDevice::FrameCountType BufferedFrameOutputDevice::getFramesCount() const noexcept {
    return m_FramesCount;
}