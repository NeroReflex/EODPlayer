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
     * to deallocate every resource this includes destructing every frame
     * (this that implies calling the destructor function on all frames).
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

    /**
     * @brief enqueue a Frame object to be shown when the right timing comes.
     * 
     * This is a blocking function that can make the user wait if there is actually not enough room
     * to store the given frame or the exec method is accessing the frame collection in a way that
     * prevents the data to remain valid; however this method and the exec method should both be designed
     * to never block the caller as that caller is supposed to be a Decoder that will delay important
     * decode operations if blocked during this call.
     * 
     * @param frame the Frame object filled with actual data. 
     */
    virtual void enqueueFrame(Frame&& frame) noexcept = 0;

    /**
     * @brief Execute the main cycle
     * 
     * This method MUST implement a loop that can only be user-interrupted and while active
     * it shows frames to the user when the right time comes.
     * 
     * This method is supposed to be a blocking call that only returns when the loop is interrupted,
     * 
     * If there are no frames to show (as the playback is not running or the video is seeking)
     * it is suposed to show the last valid frame.
     */
    virtual void exec() noexcept = 0;

private:
    FrameCountType m_FramesCount;

};
