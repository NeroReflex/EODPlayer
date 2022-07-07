#pragma once

#include "EODPlayer.hpp"

/**
 * @brief This class represents a frame to be handled and displayed by a BufferedFrameOutputDevice.
 * 
 * As for an object of this class to actually holds data a memory allocation is required.
 * 
 * An object of this class is filled with data via a function call that has parameters not stored
 * inside the object, therefore an object of this class cannot be copied using copy-constructors.
 * 
 * Also, executing a copy would be a slow operation that could copy a lot of data and potentially do a
 * system call to obtain new memory and such a slowness may compromise video player efficiency
 * and introduct randomic delays.
 * 
 * An object of this class can however be moved and that move operation will be really fast. 
 */
class Frame {

public:
    typedef std::function<void*(size_t)>  AllocatorFunctionType;
    typedef std::function<void(void*)>  DeallocatorFunctionType;

    enum class PixelFormat {
        RGBA
    };

    /**
     * @brief Construct a Frame object with the specified pixel format
     * 
     * Do not allocates data for the internal buffer to handle the specified pixel format:
     * that operation is done at filling-time using the storeFrameData method.
     * 
     * @param pf the color format for each pixel in the data
     * @param width the number of horizontal pixels
     * @param height the number of vertical pixels
     */
    Frame(PixelFormat pf, uint32_t width, uint32_t height) noexcept;

    Frame(const Frame&) = delete;

    Frame(Frame&& src) noexcept;

    Frame& operator=(const Frame&) = delete;

    Frame& operator=(Frame&& src) noexcept;

    ~Frame();

    PixelFormat getPixelFormat() const noexcept;

    uint32_t getWidth() const noexcept;

    uint32_t getHeight() const noexcept;

    /**
     * @brief Return a value indicating the presence of filled pixel buffer
     * 
     * @return true IIF the interal buffer holds pixel data 
     * @return false IIF the internal buffer does not hold pixel data
     */
    bool isHoldingData() const noexcept;

    /**
     * @brief 
     * 
     * @param fillerFn this is the function that will be called synchronously (inside the method call) that is resposible for filling the raw buffer
     */
    void storeFrameData(
        const AllocatorFunctionType& allocatorFn,
        const DeallocatorFunctionType& deallocatorFn,
        const std::function<void(void*)>& fillerFn
    ) noexcept;

    

private:
    PixelFormat m_PixelFormat;

    uint32_t m_Width;
    
    uint32_t m_Height;

    void* m_RawBuffer;

    DeallocatorFunctionType m_DeallocatorFn;
};

