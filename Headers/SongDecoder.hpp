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

        bool startMe();

        ~SongDecoder();

    protected:

        int32_t audioStreamIndex{ 0 }, audioFrameCount{ 0 }, bufferMaxSize{ 0 }, bytesRead{ 0 }, sentFrameCount{ 0 };
        UnboundedMessageBlock<vector<uint8_t>> inputDataBuffer{};
        UnboundedMessageBlock<RawFrameData> outDataBuffer{};
        AVFrame* frame{ nullptr }, * newFrame{ nullptr };
        AVCodecContext* audioDecodeContext{ nullptr };
        UnboundedMessageBlock<bool> readyBuffer{};
        AVFormatContext* formatContext{ nullptr };
        int32_t refreshTimeForBuffer{ 10000 };
        SwrContext* swrContext{ nullptr };
        AVIOContext* ioContext{ nullptr };
        AVStream* audioStream{ nullptr };
        vector<uint8_t> currentBuffer{};
        AVPacket* packet{ nullptr };
        bool areWeQuitting{ false };
        bool haveWeBooted{ false };
        int64_t totalFileSize{ 0 };
        AVCodec* codec{ nullptr };
        CoRoutine<void> theTask{};

        CoRoutine<void> run();
    };

}