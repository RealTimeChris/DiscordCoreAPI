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

    DiscordCoreAPI_Dll int32_t FileStreamRead(void* opaque, uint8_t* buf, int32_t);

    struct DiscordCoreAPI_Dll AVFrameDeleter {
        void operator()(AVFrame* other) {
            av_frame_unref(other);
            av_frame_free(&other);
        }
    };

    struct DiscordCoreAPI_Dll AVFormatContextDeleter {
        void operator()(AVFormatContext* other) {
            avformat_close_input(&other);
            avformat_free_context(other);
        }
    };

    struct DiscordCoreAPI_Dll AVIOContextDeleter {
        void operator()(AVIOContext* other) {
            av_freep(other);
        }
    };

    struct DiscordCoreAPI_Dll AVCodedContextDeleter {
        void operator()(AVCodecContext* other) {
            avcodec_free_context(&other);
        }
    };

    struct DiscordCoreAPI_Dll PacketDeletr {
        void operator()(AVPacket* other) {
            av_packet_unref(other);
        }
    };

    struct DiscordCoreAPI_Dll SwrContextDeleter {
        void operator()(SwrContext* other) {
            swr_close(other);
        }
    };

    struct DiscordCoreAPI_Dll BuildSongDecoderData {
    public:
        int64_t totalFileSize{ 0 };
        int32_t bufferMaxSize{ 0 };
    };

    class DiscordCoreAPI_Dll SongDecoder {
    public:

        friend DiscordCoreAPI_Dll int32_t FileStreamRead(void* opaque, uint8_t* buf, int32_t);
        friend class SoundCloudAPI;
        friend class YouTubeAPI;

        SongDecoder(BuildSongDecoderData dataPackage);

        void submitDataForDecoding(vector<uint8_t> dataToDecode);

        void updateBufferRefreshTime(int32_t newRefreshTime);

        bool getFrame(RawFrameData* dataPackage);

        CoRoutine<void> run();

        void startMe();

        ~SongDecoder();

    protected:

        unique_ptr<AVCodecContext, AVCodedContextDeleter> audioDecodeContext{ nullptr };
        unique_ptr<AVFormatContext, AVFormatContextDeleter> formatContext{ nullptr };
        unique_ptr<AVFrame, AVFrameDeleter> frame{ nullptr }, * newFrame{ nullptr };
        int32_t audioStreamIndex{ 0 }, bufferMaxSize{ 0 }, bytesRead{ 0 };
        unique_ptr<AVIOContext, AVIOContextDeleter> ioContext{ nullptr };
        unique_ptr<SwrContext, SwrContextDeleter> swrContext{ nullptr };
        TSUnboundedMessageBlock<vector<uint8_t>> inputDataBuffer{};
        TSUnboundedMessageBlock<RawFrameData> outDataBuffer{};
        unique_ptr<AVPacket, PacketDeletr> packet{ nullptr };
        unique_ptr<AVStream> audioStream{ nullptr };
        int32_t refreshTimeForBuffer{ 10000 };
        unique_ptr<AVCodec> codec{ nullptr };
        vector<uint8_t> currentBuffer{};
        bool areWeQuitting{ false };
        bool haveWeBooted{ false };
        int64_t totalFileSize{ 0 };
        CoRoutine<void> theTask{};
        bool areWeDone{ false };
    };

}