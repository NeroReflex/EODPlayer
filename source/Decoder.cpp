#include "Decoder.h"

Decoder::Decoder(
    BufferedFrameOutputDevice* outputDev,
    const Frame::AllocatorFunctionType& allocate,
    const Frame::DeallocatorFunctionType& deallocate
) noexcept 
 : m_OutputDevice(outputDev),
 m_AllocatorFn(allocate),
 m_DeallocatorFn(deallocate) {

}

Decoder::~Decoder() {
    
}

void Decoder::emitFrame(
    Frame::PixelFormat pf,
    uint32_t width,
    uint32_t height,
    const Frame::FrameFillerFunctionType& frameFillerFn
) noexcept {
    // create the frame and fill it with actual data
    Frame frame(pf, width, height);
    frame.storeFrameData(m_AllocatorFn, m_DeallocatorFn, frameFillerFn);

    // move the frame (fast operation) to the output device as here it's not needed anymore
    m_OutputDevice->enqueueFrame(std::move(frame));
}

