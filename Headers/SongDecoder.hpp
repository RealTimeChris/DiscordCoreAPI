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

    struct DiscordCoreAPI_Dll AVFrameDeleter {
        void operator()(AVFrame* other) {
            if (other != nullptr) {
                av_frame_unref(other);
                av_frame_free(&other);
            }
        }
    };

    struct DiscordCoreAPI_Dll AVFrameWrapper {

        AVFrameWrapper& operator=(AVFrame* other) {
            this->thePtr.reset(other);
            return *this;
        }

        AVFrame* operator->() {
            return this->thePtr.get();
        }

        operator AVFrame*() {
            return this->thePtr.get();
        }

        AVFrameWrapper(nullptr_t) {};

    protected:
        unique_ptr<AVFrame, AVFrameDeleter> thePtr{ nullptr , AVFrameDeleter{} };
    };

    struct DiscordCoreAPI_Dll AVCodecContextDeleter {
        void operator()(AVCodecContext* other) {
            if (other != nullptr) {
                avcodec_free_context(&other);
            }
        }
    };

    struct DiscordCoreAPI_Dll AVCodecContextWrapper {

        AVCodecContextWrapper& operator=(AVCodecContext* other) {
            this->thePtr.reset(other);
            return *this;
        }

        AVCodecContext* operator->() {
            return this->thePtr.get();
        }

        operator AVCodecContext*() {
            return this->thePtr.get();
        }

        AVCodecContextWrapper(nullptr_t) {};

    protected:
        unique_ptr<AVCodecContext, AVCodecContextDeleter> thePtr{ nullptr , AVCodecContextDeleter{} };
    };

    struct DiscordCoreAPI_Dll AVFormatContextWrapper01 {

        AVFormatContextWrapper01() {};

        AVFormatContext* theContext{ nullptr };
        bool didItInitialize{ false };

    };

    struct DiscordCoreAPI_Dll AVFormatContextDeleter {
        void operator()(AVFormatContextWrapper01* other) {
            if (other->didItInitialize) {
                avformat_close_input(&other->theContext);
            }
        }
    };

    struct DiscordCoreAPI_Dll AVFormatContextWrapper {

        AVFormatContextWrapper& operator=(AVFormatContext* other) {
            this->thePtr->theContext = other;
            return *this;
        }

        bool* getBoolPtr() {
            return &this->thePtr.get()->didItInitialize;
        }

        AVFormatContext* operator->() {
            return this->thePtr.get()->theContext;
        }

        AVFormatContext** operator*() {
            return &this->thePtr.get()->theContext;
        }

        operator AVFormatContext*() {
            return this->thePtr.get()->theContext;
        }

        AVFormatContextWrapper(nullptr_t) {};

    protected:
        unique_ptr<AVFormatContextWrapper01, AVFormatContextDeleter> thePtr{ new AVFormatContextWrapper01{}, AVFormatContextDeleter{} };
    };

    struct DiscordCoreAPI_Dll SwrContextDeleter {
        void operator()(SwrContext* other) {
            if (other != nullptr) {
                swr_free(&other);
            }
        }
    };

    struct DiscordCoreAPI_Dll SwrContextWrapper {

        SwrContextWrapper& operator=(SwrContext* other) {
            this->thePtr.reset(other);
            return *this;
        }

        operator SwrContext*() {
            return this->thePtr.get();
        }

        SwrContextWrapper(nullptr_t) {};

    protected:
        unique_ptr<SwrContext, SwrContextDeleter> thePtr{ nullptr , SwrContextDeleter{} };
    };

    struct DiscordCoreAPI_Dll AVIOContextDeleter {
        void operator()(AVIOContext* other) {
            if (other != nullptr) {
                av_freep(&other);
            }
        }
    };

    struct DiscordCoreAPI_Dll AVIOContextWrapper {

        AVIOContextWrapper& operator=(AVIOContext* other) {
            this->thePtr.reset(other);
            return *this;
        }

        AVIOContext* operator->() {
            return this->thePtr.get();
        }

        operator AVIOContext*() {
            return this->thePtr.get();
        }

        AVIOContextWrapper(nullptr_t) {};

    protected:
        unique_ptr<AVIOContext, AVIOContextDeleter> thePtr{ nullptr , AVIOContextDeleter{} };
    };

    struct DiscordCoreAPI_Dll AVPacketDeleter {
        void operator()(AVPacket* other) {
            if (other != nullptr) {
                av_packet_free(&other);
            }
        }
    };

    struct DiscordCoreAPI_Dll AVPacketWrapper {

        AVPacketWrapper& operator=(AVPacket* other) {
            this->thePtr.reset(other);
            return *this;
        }

        AVPacket* operator->() {
            return this->thePtr.get();
        }

        operator AVPacket*() {
            return this->thePtr.get();
        }

        AVPacketWrapper(nullptr_t) {};

    protected:
        unique_ptr<AVPacket, AVPacketDeleter> thePtr{ nullptr , AVPacketDeleter{} };
    };

    struct DiscordCoreAPI_Dll AVCodecDeleter {
        void operator()(AVCodec*) {};
    };

    struct DiscordCoreAPI_Dll AVCodecWrapper {

        AVCodecWrapper& operator=(AVCodec* other) {
            this->thePtr.reset(other);
            return *this;
        }

        operator AVCodec*() {
            return this->thePtr.get();
        }

        AVCodecWrapper(nullptr_t) {};

    protected:
        unique_ptr<AVCodec, AVCodecDeleter> thePtr{ nullptr , AVCodecDeleter{} };
    };

    struct DiscordCoreAPI_Dll AVStreamDeleter {
        void operator()(AVStream*) {};
    };

    struct DiscordCoreAPI_Dll AVStreamWrapper {

        AVStreamWrapper& operator=(AVStream* other) {
            this->thePtr.reset(other);
            return *this;
        }

        AVStream* operator->() {
            return this->thePtr.get();
        }

        operator AVStream*() {
            return this->thePtr.get();
        }

        AVStreamWrapper(nullptr_t) {};

    protected:
        unique_ptr<AVStream, AVStreamDeleter> thePtr{ nullptr , AVStreamDeleter{} };
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

        bool getFrame(RawFrameData* dataPackage);

        void cancelMe();

        void startMe();

        ~SongDecoder();

    protected:

        CoRoutineWrapper theTask{ nullptr };

        int32_t audioStreamIndex{ 0 }, audioFrameCount{ 0 }, bufferMaxSize{ 0 }, bytesRead{ 0 }, sentFrameCount{ 0 };
        TSUnboundedMessageBlock<vector<uint8_t>> inputDataBuffer{};
        TSUnboundedMessageBlock<RawFrameData> outDataBuffer{};
        AVFrameWrapper frame{ nullptr }, newFrame{ nullptr };
        AVCodecContextWrapper audioDecodeContext{ nullptr };
        AVFormatContextWrapper formatContext{ nullptr };
        AVIOContextWrapper ioContext{ nullptr };
        SwrContextWrapper swrContext{ nullptr };
        AVStreamWrapper audioStream{ nullptr };
        int32_t refreshTimeForBuffer{ 10000 };
        AVPacketWrapper packet{ nullptr };
        AVCodecWrapper codec{ nullptr };
        vector<uint8_t> currentBuffer{};
        bool areWeQuitting{ false };
        bool haveWeBooted{ false };
        int64_t totalFileSize{ 0 };

        CoRoutine<void> run();
    };

}