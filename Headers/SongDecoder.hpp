// SongDecoder.hpp - Header for the song decoder class.
// Jul 29, 2021
// Chris M.
// https://github.com/RealTimeChris

#pragma once

#include "IndexInitial.hpp"
#include "FoundationEntities.hpp"
#include "CoRoutine.hpp"

namespace DiscordCoreAPI {

    enum class SendOrReceive {
        Send = 0,
        Receive = 1
    };

    DiscordCoreAPI_Dll __int32 FileStreamRead(void* opaque, unsigned __int8* buf, __int32);

    struct DiscordCoreAPI_Dll BuildSongDecoderData {
    public:
        __int32 totalFileSize{ 0 };
        __int32 bufferMaxSize{ 0 };
    };

    class DiscordCoreAPI_Dll SongDecoder {
    public:

        friend DiscordCoreAPI_Dll __int32 FileStreamRead(void* opaque, unsigned __int8* buf, __int32);

        concurrent_queue<vector<unsigned __int8>>* getInputBuffer();

        SongDecoder();

        SongDecoder(BuildSongDecoderData dataPackage);

        bool startMe();

        void submitDataForDecoding(vector<unsigned __int8> dataToDecode, __int32 maxBufferSize = 0);

        void updateBufferRefreshTime(__int32 newRefreshTime);

        bool sendOrReceiveData(RawFrameData* dataPackage, SendOrReceive sendOrReceive);

        ~SongDecoder();

    protected:

        __int32 audioStreamIndex{ 0 }, audioFrameCount{ 0 }, totalFileSize{ 0 }, bufferMaxSize{ 0 }, bytesRead{ 0 }, sentFrameCount{ 0 };
        concurrent_queue<vector<unsigned __int8>> inputDataBuffer{};
        AVFrame* frame{ nullptr }, * newFrame{ nullptr };
        concurrent_queue<RawFrameData> outDataBuffer{};
        AVCodecContext* audioDecodeContext{ nullptr };
        AVFormatContext* formatContext{ nullptr };
        vector<unsigned __int8> currentBuffer{};
        concurrency::event readyToStartEvent {};
        __int32 refreshTimeForBuffer{ 10000 };
        unbounded_buffer<bool> readyBuffer{};
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