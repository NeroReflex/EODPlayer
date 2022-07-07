#include "FFMPEGDecoder.h"

// for memcpy
#include <cstring>

// ffmpeg
extern "C" {
#include <libavutil/avstring.h>
#include <libavutil/eval.h>
#include <libavutil/mathematics.h>
#include <libavutil/pixdesc.h>
#include <libavutil/imgutils.h>
#include <libavutil/dict.h>
#include <libavutil/bprint.h>
#include <libavutil/parseutils.h>
#include <libavutil/samplefmt.h>
#include <libavutil/avassert.h>
#include <libavutil/display.h>
#include <libavutil/time.h>
#include <libavformat/avformat.h>
#include <libavdevice/avdevice.h>
#include <libswscale/swscale.h>
#include <libavutil/opt.h>
#include <libavcodec/avfft.h>
#include <libswresample/swresample.h>
#include <libavfilter/avfilter.h>
#include <libavfilter/buffersink.h>
#include <libavfilter/buffersrc.h>
#include <libavcodec/avcodec.h>
}

void writeDataToFrameMemory(void* dst, AVFrame *avFrame, int width, int height, int frameIndex) noexcept {
    std::memcpy(dst, avFrame->data[0], Frame::getPixelSizeInBytes(Frame::PixelFormat::RGBA64) * width * height);
}

FFMPEGDecoder::FFMPEGDecoder(
    BufferedFrameOutputDevice* const outputDev,
    const Frame::AllocatorFunctionType& allocate,
    const Frame::DeallocatorFunctionType& deallocate
) noexcept
    : Decoder(
        outputDev,
        allocate,
        deallocate
    ),
    m_ShouldClose(false) {
        
    }

FFMPEGDecoder::~FFMPEGDecoder() {

}

void FFMPEGDecoder::loadFile(const Decoder::FileNameType& filename) noexcept {
    m_LoadedFilename = filename;
}

void FFMPEGDecoder::stop() noexcept {
    m_ShouldClose = true;
}

void FFMPEGDecoder::play() noexcept {
    m_FFMPEGThread.reset(
        new std::thread([this]() {
            // with ffmpeg, you have to first initialize the library.
            // 'av_register_all' is deprecated just omit this function call in ffmpeg
            // 4.0 and later.
            // av_register_all();  // [0]

            
            // declare the AVFormatContext
            AVFormatContext * pFormatCtx = NULL; // [1]

            // now we can actually open the file:
            // the minimum information required to open a file is its URL, which is
            // passed to avformat_open_input(), as in the following code:
            int ret = avformat_open_input(&pFormatCtx, this->m_LoadedFilename->c_str(), NULL, NULL);    // [2]
            if (ret < 0)
            {
                // couldn't open file
                std::cerr << "Could not open file " << this->m_LoadedFilename->c_str() << std::endl;

                // exit with error
                return -1;
            }

            // The call to avformat_open_input(), only looks at the header, so next we
            // need to check out the stream information in the file.:
            // Retrieve stream information
            ret = avformat_find_stream_info(pFormatCtx, NULL);  //[3]
            if (ret < 0)
            {
                // couldn't find stream information
                std::cerr << "Could not find stream information " << this->m_LoadedFilename->c_str() << std::endl;

                // exit with error
                return -1;
            }

            // We introduce a handy debugging function to show us what's inside dumping
            // information about file onto standard error
            av_dump_format(pFormatCtx, 0, this->m_LoadedFilename->c_str(), 0);  // [4]

            // Now pFormatCtx->streams is just an array of pointers, of size
            // pFormatCtx->nb_streams, so let's walk through it until we find a video
            // stream.
            int i;

            // The stream's information about the codec is in what we call the
            // "codec context." This contains all the information about the codec that
            // the stream is using
            AVCodecContext * pCodecCtxOrig = NULL;
            AVCodecContext * pCodecCtx = NULL;

            // Find the first video stream
            int videoStream = -1;
            for (i = 0; i < pFormatCtx->nb_streams; i++)
            {
                // check the General type of the encoded data to match
            // AVMEDIA_TYPE_VIDEO
                if (pFormatCtx->streams[i]->codecpar->codec_type == AVMEDIA_TYPE_VIDEO) // [5]
                {
                    videoStream = i;
                    break;
                }
            }

            if (videoStream == -1)
            {
                // didn't find a video stream
                return -1;
            }

            /**
             * New API.
             * This implementation uses the new API.
             * Please refer to tutorial01-deprecated.c for an implementation using the
             * deprecated FFmpeg API.
             */

            // Get a pointer to the codec context for the video stream.
            // AVStream::codec deprecated
            // https://ffmpeg.org/pipermail/libav-user/2016-October/009801.html
            // pCodecCtxOrig = pFormatCtx->streams[videoStream]->codec;

            // Find the decoder for the video stream
            auto pCodec = avcodec_find_decoder(pFormatCtx->streams[videoStream]->codecpar->codec_id); // [6]
            if (pCodec == nullptr)
            {
                // codec not found
                std::cerr << "Unsupported codec for " << this->m_LoadedFilename->c_str() << std::endl;
                

                // exit with error
                return -1;
            }

            pCodecCtxOrig = avcodec_alloc_context3(pCodec); // [7]
            ret = avcodec_parameters_to_context(pCodecCtxOrig, pFormatCtx->streams[videoStream]->codecpar);

            /**
             * Note that we must not use the AVCodecContext from the video stream
             * directly! So we have to use avcodec_copy_context() to copy the
             * context to a new location (after allocating memory for it, of
             * course).
             */

            // Copy context
            // avcodec_copy_context deprecation
            // http://ffmpeg.org/pipermail/libav-user/2017-September/010615.html
            //ret = avcodec_copy_context(pCodecCtx, pCodecCtxOrig);
            pCodecCtx = avcodec_alloc_context3(pCodec); // [7]
            ret = avcodec_parameters_to_context(pCodecCtx, pFormatCtx->streams[videoStream]->codecpar);
            if (ret != 0)
            {
                // error copying codec context
                std::cerr << "Could not copy codec context for " << this->m_LoadedFilename->c_str() << std::endl;

                // exit with error
                return -1;
            }

            // Open codec
            ret = avcodec_open2(pCodecCtx, pCodec, NULL);   // [8]
            if (ret < 0)
            {
                // Could not open codec
                std::cerr << "Could not open codec for " << this->m_LoadedFilename->c_str() << std::endl;

                // exit with error
                return -1;
            }

            // Now we need a place to actually store the frame:
            AVFrame * pFrame = NULL;

            // Allocate video frame
            pFrame = av_frame_alloc();  // [9]
            if (pFrame == NULL)
            {
                // Could not allocate frame
                std::cerr << "Could not allocate frame for " << this->m_LoadedFilename->c_str() << std::endl;

                // exit with error
                return -1;
            }

            /**
             * Since we're planning to output PPM files, which are stored in 24-bit
             * RGB, we're going to have to convert our frame from its native format
             * to RGB. ffmpeg will do these conversions for us. For most projects
             * (including ours) we're going to want to convert our initial frame to
             * a specific format. Let's allocate a frame for the converted frame
             * now.
             */

            // Allocate an AVFrame structure
            AVFrame * pFrameRGB = NULL;
            pFrameRGB = av_frame_alloc();
            if (pFrameRGB == NULL)
            {
                // Could not allocate frame
                std::cerr << "Could not allocate frame for " << this->m_LoadedFilename->c_str() << std::endl;

                // exit with error
                return -1;
            }

            // Even though we've allocated the frame, we still need a place to put
            // the raw data when we convert it. We use avpicture_get_size to get
            // the size we need, and allocate the space manually:
            uint8_t * buffer = NULL;
            int numBytes;

            // Determine required buffer size and allocate buffer
            // numBytes = avpicture_get_size(AV_PIX_FMT_RGB24, pCodecCtx->width, pCodecCtx->height); // changed to AV_PIX_FMT_AYUV64LE
            // https://ffmpeg.org/pipermail/ffmpeg-devel/2016-January/187299.html
            // what is 'linesize alignment' meaning?:
            // https://stackoverflow.com/questions/35678041/what-is-linesize-alignment-meaning
            numBytes = av_image_get_buffer_size(AV_PIX_FMT_RGBA64, pCodecCtx->width, pCodecCtx->height, 32); // [10]
            buffer = (uint8_t *) av_malloc(numBytes * sizeof(uint8_t));    // [11]

            /**
            * Now we use avpicture_fill() to associate the frame with our newly
            * allocated buffer. About the AVPicture cast: the AVPicture struct is
            * a subset of the AVFrame struct - the beginning of the AVFrame struct
            * is identical to the AVPicture struct.
            */
            // Assign appropriate parts of buffer to image planes in pFrameRGB
            // Note that pFrameRGB is an AVFrame, but AVFrame is a superset
            // of AVPicture
            // Picture data structure - Deprecated: use AVFrame or imgutils functions
            // instead
            // https://www.ffmpeg.org/doxygen/3.0/structAVPicture.html#a40dfe654d0f619d05681aed6f99af21b
            // avpicture_fill( // [12]
            //     (AVPicture *)pFrameRGB,
            //     buffer,
            //     AV_PIX_FMT_RGB24,
            //     pCodecCtx->width,
            //     pCodecCtx->height
            // );
            av_image_fill_arrays( // [12]
                pFrameRGB->data,
                pFrameRGB->linesize,
                buffer,
                AV_PIX_FMT_RGBA64,
                pCodecCtx->width,
                pCodecCtx->height,
                32
            );

            // Finally! Now we're ready to read from the stream!

            /**
             * What we're going to do is read through the entire video stream by
             * reading in the packet, decoding it into our frame, and once our
             * frame is complete, we will convert and save it.
             */

            struct SwsContext * sws_ctx = NULL;

            AVPacket * pPacket = av_packet_alloc();
            if (pPacket == NULL)
            {
                // couldn't allocate packet
                std::cerr << "Could not allocate packet for " << this->m_LoadedFilename->c_str() << std::endl;

                // exit with error
                return -1;
            }

            // initialize SWS context for software scaling
            sws_ctx = sws_getContext(   // [13]
                pCodecCtx->width,
                pCodecCtx->height,
                pCodecCtx->pix_fmt,
                pCodecCtx->width,
                pCodecCtx->height,
                AV_PIX_FMT_BGRA64,   // sws_scale destination color scheme
                SWS_BILINEAR,
                NULL,
                NULL,
                NULL
            );

            /**
             * The process, again, is simple: av_read_frame() reads in a packet and
             * stores it in the AVPacket struct. Note that we've only allocated the
             * packet structure - ffmpeg allocates the internal data for us, which
             * is pointed to by packet.data. This is freed by the av_free_packet()
             * later. avcodec_decode_video() converts the packet to a frame for us.
             * However, we might not have all the information we need for a frame
             * after decoding a packet, so avcodec_decode_video() sets
             * frameFinished for us when we have decoded enough packets the next
             * frame.
             * Finally, we use sws_scale() to convert from the native format
             * (pCodecCtx->pix_fmt) to RGB. Remember that you can cast an AVFrame
             * pointer to an AVPicture pointer. Finally, we pass the frame and
             * height and width information to our SaveFrame function.
             */

            i = 0;
            uint32_t frame_index = 0;
            while ((av_read_frame(pFormatCtx, pPacket) >= 0) && (!m_ShouldClose))  // [14]
            {
                // Is this a packet from the video stream?
                if (pPacket->stream_index == videoStream)
                {
                    // Decode video frame
                    // avcodec_decode_video2(pCodecCtx, pFrame, &frameFinished, &pPacket);
                    // Deprecated: Use avcodec_send_packet() and avcodec_receive_frame().
                    ret = avcodec_send_packet(pCodecCtx, pPacket);    // [15]
                    if (ret < 0)
                    {
                        // could not send packet for decoding
                        printf("Error sending packet for decoding.\n");

                        // exit with eror
                        return -1;
                    }

                    while (ret >= 0)
                    {
                        ret = avcodec_receive_frame(pCodecCtx, pFrame);   // [15]

                        if (ret == AVERROR(EAGAIN) || ret == AVERROR_EOF)
                        {
                            // EOF exit loop
                            break;
                        }
                        else if (ret < 0)
                        {
                            // could not decode packet
                            printf("Error while decoding.\n");

                            // exit with error
                            return -1;
                        }

                        // Convert the image from its native format to RGB
                        sws_scale(  // [16]
                            sws_ctx,
                            (uint8_t const * const *)pFrame->data,
                            pFrame->linesize,
                            0,
                            pCodecCtx->height,
                            pFrameRGB->data,
                            pFrameRGB->linesize
                        );

                        // send frame to FrameCollection
                        this->emitFrame(Frame::PixelFormat::RGBA64, pCodecCtx->width, pCodecCtx->height, [&](void* frameMemory) {
                            writeDataToFrameMemory(frameMemory, pFrameRGB, pCodecCtx->width, pCodecCtx->height, frame_index++);
                        });
                    }
                }

                // Free the packet that was allocated by av_read_frame
                // [FFmpeg-cvslog] avpacket: Replace av_free_packet with
                // av_packet_unref
                // https://lists.ffmpeg.org/pipermail/ffmpeg-cvslog/2015-October/094920.html
                av_packet_unref(pPacket);
            }

            /**
             * Cleanup.
             */

            // Free the RGB image
            av_free(buffer);
            av_frame_free(&pFrameRGB);
            av_free(pFrameRGB);

            // Free the YUV frame
            av_frame_free(&pFrame);
            av_free(pFrame);

            // Close the codecs
            avcodec_close(pCodecCtx);
            avcodec_close(pCodecCtxOrig);

            // Close the video file
            avformat_close_input(&pFormatCtx);

            m_ShouldClose = false;
        })
    );
}