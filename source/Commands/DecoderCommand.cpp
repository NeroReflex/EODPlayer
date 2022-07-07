#include "Commands/DecoderCommand.h"

DecoderCommand::DecoderCommand(
    Decoder& decoder
) noexcept
    : m_Decoder(decoder) {}

DecoderCommand::~DecoderCommand() {

}