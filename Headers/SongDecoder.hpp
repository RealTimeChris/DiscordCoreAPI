// SongDecoder.hpp - Header for the song decoder class.
// Jul 29, 2021
// Chris M.
// https://github.com/RealTimeChris
#pragma once

#include "IndexInitial.hpp"
#include "FoundationEntities.hpp"
#include "CoRoutine.hpp"

namespace DiscordCoreAPI {

    DiscordCoreAPI_Dll int32_t FileStreamRead(void* opaque, uint8_t* buf, int32_t);
    struct alignas(hardware_destructive_interference_size)  DiscordCoreAPI_Dll BuildSongDecoderData {
    public:

        uint64_t totalFileSize{ 0 };
        int32_t bufferMaxSize{ 0 };
    };


    class alignas(hardware_destructive_interference_size) DiscordCoreAPI_Dll SongDecoder {
    public:

        friend DiscordCoreAPI_Dll int32_t FileStreamRead(void* opaque, uint8_t* buf, int32_t);
        friend class SoundCloudAPI;
        friend class YouTubeAPI;

        SongDecoder(BuildSongDecoderData dataPackage);

        void submitDataForDecoding(vector<uint8_t> dataToDecode, int32_t maxBufferSize = 0);

        void updateBufferRefreshTime(int32_t newRefreshTime);

        bool getFrame(RawFrameData* dataPackage);

        bool startMe();

        ~SongDecoder();

    protected:

        int32_t audioStreamIndex{ 0 }, audioFrameCount{ 0 }, bufferMaxSize{ 0 }, bytesRead{ 0 }, sentFrameCount{ 0 };
        unbounded_buffer<vector<uint8_t>> inputDataBuffer{};
        AVFrame* frame{ nullptr }, * newFrame{ nullptr };
        unbounded_buffer<RawFrameData> outDataBuffer{};
        AVCodecContext* audioDecodeContext{ nullptr };
        AVFormatContext* formatContext{ nullptr };
        vector<uint8_t> currentBuffer{};
        concurrency::event readyToStartEvent {};
        int32_t refreshTimeForBuffer{ 10000 };
        unbounded_buffer<bool> readyBuffer{};
        uint64_t totalFileSize{ 0 };
        SwrContext* swrContext{ nullptr };
        AVIOContext* ioContext{ nullptr };
        AVStream* audioStream{ nullptr };
        AVPacket* packet{ nullptr };
        bool areWeQuitting{ false };
        bool haveWeBooted{ false };
        CoRoutine<void> theTask{};
        AVCodec* codec{ nullptr };        

        CoRoutine<void> run();

    };
    
}