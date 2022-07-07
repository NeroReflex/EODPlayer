#pragma once

#include "BufferedFrameOutputDevice.h"

/**
 * @brief The decoder application logic
 * 
 * This class represents the public interface of a video decoder.
 * 
 * Any video decoder is capable of executing all required operation for the player,
 * loading a file or stream and play it as well as stopping it; seek operations can be unsupported
 * this is to support stream.
 * 
 * Every method MUST be non-blocking and everything that needs to happen in "backgroud"
 * as the decode operation MUST live in it's own thread (that can be manipulated by aforementioned
 * non-blocking methods).
 * 
 * Playback is done emitting frames (objects of type Frame) with actual data inside them,
 * this means that allocator and deallocator callback must also be provided.
 * 
 * A video decoder is not meant to be an object shared between threads: is has to be implemented as
 * its public methods are called by the creating thread and it MUST be used that way.
 */
class Decoder {

public:
    typedef std::string FileNameType;

    /**
     * @brief Construct a new Decoder object
     * 
     * Every decoder MUST call this constructor keeping in mind that lifetime of objects used inside the two provided
     * functions must be carefully thought as there are no guarantees other than the one specified in the BufferedFrameOutputDevice
     * as to when the deallocate function has to be called.
     * 
     * @param outputDev the output device frames will be sent to
     * @param allocate the memory allocator function that will be called to fill a new frame with pixel data
     * @param deallocate the memory deallocator function that will be called when a decoded frame is not needed anymore
     */
    Decoder(
        BufferedFrameOutputDevice* outputDev,
        const Frame::AllocatorFunctionType& allocate,
        const Frame::DeallocatorFunctionType& deallocate
    ) noexcept;

    /**
     * @brief Destroy the Decoder object
     *  
     * Free every used resource and stops the actual decoding if the object is deleted while
     * playing a video file.
     */
    virtual ~Decoder();

    Decoder(const Decoder&) = delete;

    Decoder(Decoder&&) = delete;

    Decoder& operator=(const Decoder&) = delete;

    Decoder& operator=(Decoder&&) = delete;

    /**
     * @brief Loads a file into the decoder
     * 
     * If the decoder is actually producing frames from a file this command
     * also stops the playback and prepare everything necessary to execute a play command
     * on that other file.
     * 
     * @param filename the identifier of the file 
     */
    virtual void loadFile(const FileNameType& filename) noexcept = 0;

    /**
     * @brief Lauch a thread that decodes frames from the loaded video file and stores them in the provided BufferedFrameOutputDevice.
     * 
     */
    virtual void play() noexcept = 0;

    virtual void stop() noexcept = 0;

protected:
    /**
     * @brief Emit a frame decoded by the playback thread
     * 
     * This method is supposed to be called by the frame decoder function when a frame has been decoded
     * and needs to be sent to the output device.
     * 
     * @param pf the frame pixel format
     * @param width the frame width (in pixels)
     * @param height the frame height (in pixels)
     * @param frameFillerFn the function that is responsible to fill the frame with provided information
     */
    void emitFrame(
        Frame::PixelFormat pf,
        uint32_t width,
        uint32_t height,
        const Frame::FrameFillerFunctionType& frameFillerFn
    ) noexcept;

private:
    BufferedFrameOutputDevice* m_OutputDevice;

    Frame::AllocatorFunctionType m_AllocatorFn;

    Frame::DeallocatorFunctionType m_DeallocatorFn;

};
