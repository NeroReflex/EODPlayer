#pragma once

#include "Decoder.h"

/**
 * @brief The implementation of a decoder that uses FFMPEG.
 * 
 * Uses libavcoded, which is part of ffmpeg to decode video files.
 */
class FFMPEGDecoder : public Decoder {

public:
    FFMPEGDecoder(
        BufferedFrameOutputDevice* outputDev,
        const Frame::AllocatorFunctionType& allocate,
        const Frame::DeallocatorFunctionType& deallocate
    ) noexcept;

    ~FFMPEGDecoder() override;

    void loadFile(const FileNameType& filename) noexcept override;

    void play() noexcept override;

    void stop() noexcept override;

private:
    std::unique_ptr<std::thread> m_FFMPEGThread;

    std::optional<Decoder::FileNameType> m_LoadedFilename;

    std::atomic_bool m_ShouldClose;
};
