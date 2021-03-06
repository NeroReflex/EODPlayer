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
    typedef std::function<void(void*)>  FrameFillerFunctionType;

    enum class PixelFormat {
        RGBA64, //a pixel is a unt16_t[4]
    };

    /**
     * @brief Get the size of a pixel (in bytes)
     * 
     * Helper static function that given a pixel format return the dimensiont in bytes
     * of a pixel of the given format.
     * 
     * @param pf the pixel format
     * @return size_t number of bytes required to store a pixel in the specified format
     */
    static size_t getPixelSizeInBytes(PixelFormat pf) noexcept;

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
     * @param allocatorFn this is the function that will be called synchronously (inside the method call) that is resposible for allocating the specified amount of bytes on the heap
     * @param deallocatorFn this is the function that will be called by the object destructor so it's importat to think about lifetime of captures
     * @param fillerFn this is the function that will be called synchronously (inside the method call) that is resposible for filling the raw buffer
     */
    void storeFrameData(
        const AllocatorFunctionType& allocatorFn,
        const DeallocatorFunctionType& deallocatorFn,
        const FrameFillerFunctionType& fillerFn
    ) noexcept;

    

private:
    PixelFormat m_PixelFormat;

    uint32_t m_Width;
    
    uint32_t m_Height;

    void* m_RawBuffer;

    DeallocatorFunctionType m_DeallocatorFn;
};

