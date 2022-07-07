#pragma once

#include "Frame.h"

/**
 * @brief Represents a buffered output device for image frames.
 * 
 * A buffered output device holds a number of frames to be shown.
 * 
 * The main characteristic of a buffered output device is that it is possible to obtain
 * a not-in-use frame via a blocking call in order to write actual pixel data on that empty frame.
 * 
 * As to ensure the player won't lag (via reducing every possible delay) it is necessary to specify at object creation
 * time the number of pre-allocated empty frames.
 */
class BufferedFrameOutputDevice {

public:
    typedef uint32_t FrameCountType;

    /**
     * @brief Construct a new Buffered Frame Output Device object
     * 
     * Every buffered frame output device MUST call this constructor.
     * 
     * @param frames the number of pre-allocated frames
     */
    BufferedFrameOutputDevice(FrameCountType frames) noexcept;

    /**
     * @brief Destroy the Buffered Frame Output Device object
     * 
     * Every class implementing this interface MUST use the desctructor
     * to deallocate resources.
     */
    virtual ~BufferedFrameOutputDevice();

    BufferedFrameOutputDevice(const BufferedFrameOutputDevice&) = delete;

    BufferedFrameOutputDevice(BufferedFrameOutputDevice&&) = delete;

    BufferedFrameOutputDevice& operator=(const BufferedFrameOutputDevice&) = delete;

    BufferedFrameOutputDevice& operator=(BufferedFrameOutputDevice&&) = delete;

    /**
     * @brief Get the number of frames.
     * 
     * @return FrameCountType the number of pre-allocated frames.
     */
    FrameCountType getFramesCount() const noexcept;

    void ExposeNextFrameForWriting(const std::function<void(void*)>& ) noexcept;

private:
    FrameCountType m_FramesCount;

};
