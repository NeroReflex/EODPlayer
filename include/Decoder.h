#pragma once

#include "EODPlayer.hpp"

/**
 * @brief The decoder application logic
 * 
 * This class represents the public interface of a video decoder.
 * 
 * Any video decoder is capable of executing all required operation for the player,
 * loading a file or stream and play it as well as stopping it and seek.
 * 
 * Every method MUST be non-blocking and everything that needs to happen in "backgroud"
 * as the decode operation MUST live in it's own thread (that can be manipulated by aforementioned
 * non-blocking methods).
 */
class Decoder {

public:
    typedef std::string FileNameType;

    Decoder() noexcept;

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
     * @brief Lauch a thread that decodes frames from the loaded video file and stores them in the given BufferedFrameOutputDevice.
     * 
     */
    virtual void play() noexcept = 0;

private:
    

};
