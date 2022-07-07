#include "Frame.h"

static const Frame::DeallocatorFunctionType defaultDeallocFn = [](void*) {};

Frame::Frame(PixelFormat pf, uint32_t width, uint32_t height) noexcept
 : m_PixelFormat(pf),
 m_Width(width),
 m_Height(height),
 m_RawBuffer(nullptr),
 m_DeallocatorFn(defaultDeallocFn) {

}

Frame::Frame(Frame&& src) noexcept 
 : m_PixelFormat(src.m_PixelFormat),
 m_Width(src.m_Width),
 m_Height(src.m_Height),
 m_RawBuffer(src.m_RawBuffer),
 m_DeallocatorFn(src.m_DeallocatorFn) {
    src.m_RawBuffer = nullptr;
    src.m_DeallocatorFn = defaultDeallocFn;
}

Frame& Frame::operator=(Frame&& src) noexcept {
    if (&src != this) {
        m_PixelFormat = src.m_PixelFormat;
        m_Width = src.m_Width;
        m_Height = src.m_Height;
        m_RawBuffer = src.m_RawBuffer;
        m_DeallocatorFn = src.m_DeallocatorFn;
        src.m_RawBuffer = nullptr;
        src.m_DeallocatorFn = defaultDeallocFn;
    }

    return *this;
}

Frame::~Frame() {
    if (isHoldingData()) {
        m_DeallocatorFn(m_RawBuffer);
    }
}

Frame::PixelFormat Frame::getPixelFormat() const noexcept {
    return m_PixelFormat;
}

uint32_t Frame::getWidth() const noexcept {
    return m_Width;
}

uint32_t Frame::getHeight() const noexcept {
    return m_Height;
}

bool Frame::isHoldingData() const noexcept {
    return m_RawBuffer != nullptr;
}

void Frame::storeFrameData(
    const AllocatorFunctionType& allocatorFn,
    const DeallocatorFunctionType& deallocatorFn,
    const std::function<void(void*)>& fillerFn
) noexcept {
    // decide the pixel size (in bytes)
    size_t pixelSize = 0;
    switch (getPixelFormat()) {
        case Frame::PixelFormat::RGBA:
            pixelSize = 1;
            break;
    }

    // allocate bytes on the heap to store the data
    m_RawBuffer = allocatorFn(pixelSize);

    // store the memory deallocator function so that it can be called on the destructor
    m_DeallocatorFn = deallocatorFn;

    // allows the caller to fill the allocated buffer with pixel data in the specified format 
    fillerFn(m_RawBuffer);
}