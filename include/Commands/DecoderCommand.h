#pragma once

#include "Decoder.h"

/**
 * @brief The interface of a generic command that can be executed on a video decoder.
 * 
 * The target video decoder MUST be given at object creation-time and MUST remain valid
 * during the whole execution method call.
 */
class DecoderCommand {

public:
    DecoderCommand(Decoder& decoder) noexcept;

    DecoderCommand(const DecoderCommand&) = delete;

    DecoderCommand(DecoderCommand&&) = delete;

    DecoderCommand& operator=(const DecoderCommand&) = delete;

    DecoderCommand& operator=(DecoderCommand&&) = delete;

    virtual ~DecoderCommand();

    /**
     * @brief The execution method.
     * 
     * Calling this method attempts to execute the given command on the decoder
     * specified while constructing the DecoderCommand object.
     */
    virtual void execute() noexcept = 0;

private:
    Decoder& m_Decoder;

};
