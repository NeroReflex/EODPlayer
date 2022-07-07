add_executable(
    EODPlayer
    
    Commands/DecoderCommand.cpp
    Commands/LoadFileDecoderCommand.cpp
    BufferedFrameOutputDevice.cpp
    Decoder.cpp
    Frame.cpp
    FFMPEGDecoder.cpp
    main.cpp
)

target_include_directories(EODPlayer PRIVATE include)

target_include_directories(EODPlayer PRIVATE src)

target_include_directories(EODPlayer PRIVATE ${FFMPEG_INCLUDE_DIRS})

target_link_libraries(EODPlayer PRIVATE m glfw ${Vulkan_LIBRARIES} ${FFMPEG_LIBRARIES})


