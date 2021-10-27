// SongDecoder.hpp - Header for the song decoder class.
// Jul 29, 2021
// Chris M.
// https://github.com/RealTimeChris

#pragma once

#ifndef _SONG_DECODER_
#define _SONG_DECODER_

#include "IndexInitial.hpp"
#include "FoundationEntities.hpp"
#include "CoRoutine.hpp"

namespace DiscordCoreAPI {

    DiscordCoreAPI_Dll __int32 FileStreamRead(void* opaque, unsigned __int8* buf, __int32);

    struct DiscordCoreAPI_Dll BuildSongDecoderData {
    public:
        unbounded_buffer<vector<unsigned __int8>>* sendEncodedAudioDataBuffer{};
        __int32 totalFileSize{ 0 };
        __int32 bufferMaxSize{ 0 };
    };

    class DiscordCoreAPI_Dll SongDecoder : DiscordCoreInternal::ThreadContext, public agent {
    public:

        friend DiscordCoreAPI_Dll __int32 FileStreamRead(void* opaque, unsigned __int8* buf, __int32);

        SongDecoder();
        
        SongDecoder(BuildSongDecoderData dataPackage);

        bool startMe();

        static CoRoutine<SongDecoder*> generateSongDecoder(BuildSongDecoderData dataPackage);

        void submitDataForDecoding(vector<unsigned __int8> dataToDecode, __int32 maxBufferSize = 0);

        void updateBufferRefreshTime(__int32 newRefreshTime);

        bool getFrame(RawFrameData* dataPackage);

        ~SongDecoder();

    protected:

        __int32 audioStreamIndex{ 0 }, audioFrameCount{ 0 }, totalFileSize{ 0 }, bufferMaxSize{ 0 }, bytesRead{ 0 }, sentFrameCount{ 0 };
        unbounded_buffer<vector<unsigned __int8>>* inputDataBuffer{};
        AVFrame* frame{ nullptr }, * newFrame{ nullptr };
        unbounded_buffer<RawFrameData> outDataBuffer{};
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
        AVCodec* codec{ nullptr };

        void run();

    };
}

#endif
