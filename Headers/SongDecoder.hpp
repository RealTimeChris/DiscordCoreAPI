// SongDecoder.hpp - Header for the song decoder class.
// Jul 29, 2021
// Chris M.
// https://github.com/RealTimeChris

#pragma once

#include "IndexInitial.hpp"
#include "FoundationEntities.hpp"
#include "CoRoutine.hpp"
#include "EventEntities.hpp"

namespace DiscordCoreAPI {

    struct AVFrameDeleter {
        void operator()(AVFrame* other) {
            av_frame_unref(other);
            av_frame_free(&other);
        }
    };

    struct AVCodecContextDeleter {
        void operator()(AVCodecContext* other) {
            avcodec_free_context(&other);
        }
    };

    struct AVFormatContextDeleter {
        void operator()(AVFormatContext* other) {
            avformat_close_input(&other);
            avformat_free_context(other);
        }
    };

    struct SwrContextDeleter {
        void operator()(SwrContext* other) {
            swr_free(&other);
        }
    };

    struct AVIOContextDeleter {
        void operator()(AVIOContext* other) {
            av_freep(other);
        }
    };

    struct AVPacketDeleter {
        void operator()(AVPacket* other) {
            av_packet_free(&other);
        }
    };

    struct AVCodecDeleter {
        void operator()(AVCodec* other) {}
    };

    struct AVStreamDeleter {
        void operator()(AVStream* other) {}
    };

    struct DiscordCoreAPI_Dll BuildSongDecoderData {
    public:
        int64_t totalFileSize{ 0 };
        int32_t bufferMaxSize{ 0 };
    };

    class DiscordCoreAPI_Dll SongDecoder {
    public:

        friend class SoundCloudAPI;
        friend class YouTubeAPI;

        SongDecoder(BuildSongDecoderData dataPackage);

        static int32_t FileStreamRead(void* opaque, uint8_t* buf, int32_t);

        void submitDataForDecoding(vector<uint8_t> dataToDecode);

        void updateBufferRefreshTime(int32_t newRefreshTime);

        void cancelMe();

        bool getFrame(RawFrameData* dataPackage);

        void startMe();

        ~SongDecoder();

    protected:

        int32_t audioStreamIndex{ 0 }, audioFrameCount{ 0 }, bufferMaxSize{ 0 }, bytesRead{ 0 }, sentFrameCount{ 0 };
        unique_ptr<AVCodecContext, AVCodecContextDeleter> audioDecodeContext{ nullptr };
        unique_ptr<AVFormatContext, AVFormatContextDeleter> formatContext{ nullptr };
        unique_ptr<AVFrame, AVFrameDeleter> frame{ nullptr }, newFrame{ nullptr };
        unique_ptr<AVIOContext, AVIOContextDeleter> ioContext{ nullptr };
        unique_ptr<SwrContext, SwrContextDeleter> swrContext{ nullptr };
        unique_ptr<AVStream, AVStreamDeleter> audioStream{ nullptr };
        TSUnboundedMessageBlock<vector<uint8_t>> inputDataBuffer{};
        unique_ptr<AVPacket, AVPacketDeleter> packet{ nullptr };
        TSUnboundedMessageBlock<RawFrameData> outDataBuffer{};
        unique_ptr<AVCodec, AVCodecDeleter> codec{ nullptr };
        int32_t refreshTimeForBuffer{ 10000 };
        vector<uint8_t> currentBuffer{};
        bool areWeQuitting{ false };
        bool haveWeBooted{ false };
        int64_t totalFileSize{ 0 };
        CoRoutine<void> theTask{};

        CoRoutine<void> run();
    };

}