// AudioDecoder.hpp - Header for the audio decoder class.
// Jul 29, 2021
// Chris M.
// https://github.com/RealTimeChris

#ifndef AUDIO_DECODER
#define AUDIO_DECODER

#ifdef WIN32
#pragma comment(lib, "avutil.lib")
#pragma comment(lib, "avcodec.lib")
#pragma comment(lib, "swresample.lib")
#pragma comment(lib, "avformat.lib")
#pragma comment(lib, "swresample.lib")
#endif

#include "FoundationEntities.hpp"
#include "CoRoutine.hpp"
#include "EventEntities.hpp"

extern "C"
{
#include <libswresample/swresample.h>
#include <libavformat/avformat.h>
}

namespace DiscordCoreAPI {

    struct DiscordCoreAPI_Dll AVFrameWrapper {

        struct DiscordCoreAPI_Dll AVFrameDeleter {
            void operator()(AVFrame* other) {
                if (other != nullptr) {
                    av_frame_unref(other);
                    av_frame_free(&other);
                }
            }
        };

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
        std::unique_ptr<AVFrame, AVFrameDeleter> thePtr{ nullptr , AVFrameDeleter{} };
    };

    struct DiscordCoreAPI_Dll AVCodecContextWrapper {

        struct DiscordCoreAPI_Dll AVCodecContextDeleter {
            void operator()(AVCodecContext* other) {
                if (other != nullptr) {
                    avcodec_free_context(&other);
                }
            }
        };

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
        std::unique_ptr<AVCodecContext, AVCodecContextDeleter> thePtr{ nullptr , AVCodecContextDeleter{} };
    };

    struct DiscordCoreAPI_Dll AVFormatContextWrapper01 {

        AVFormatContextWrapper01() {};

        AVFormatContext* theContext{ nullptr };
        bool didItInitialize{ false };

    };

    struct DiscordCoreAPI_Dll AVFormatContextWrapper {

        struct DiscordCoreAPI_Dll AVFormatContextDeleter {
            void operator()(AVFormatContextWrapper01* other) {
                if (other->didItInitialize) {
                    avformat_close_input(&other->theContext);
                }
            }
        };

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
        std::unique_ptr<AVFormatContextWrapper01, AVFormatContextDeleter> thePtr{ new AVFormatContextWrapper01{}, AVFormatContextDeleter{} };
    };

    struct DiscordCoreAPI_Dll SwrContextWrapper {

        struct DiscordCoreAPI_Dll SwrContextDeleter {
            void operator()(SwrContext* other) {
                if (other != nullptr) {
                    swr_free(&other);
                }
            }
        };

        SwrContextWrapper& operator=(SwrContext* other) {
            this->thePtr.reset(other);
            return *this;
        }

        operator SwrContext*() {
            return this->thePtr.get();
        }

        SwrContextWrapper(nullptr_t) {};

    protected:
        std::unique_ptr<SwrContext, SwrContextDeleter> thePtr{ nullptr , SwrContextDeleter{} };
    };

    struct DiscordCoreAPI_Dll AVIOContextWrapper {

        struct DiscordCoreAPI_Dll AVIOContextDeleter {
            void operator()(AVIOContext* other) {
                if (other != nullptr) {
                    av_freep(&other);
                }
            }
        };

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
        std::unique_ptr<AVIOContext, AVIOContextDeleter> thePtr{ nullptr , AVIOContextDeleter{} };
    };

    struct DiscordCoreAPI_Dll AVPacketWrapper {

        struct DiscordCoreAPI_Dll AVPacketDeleter {
            void operator()(AVPacket* other) {
                if (other != nullptr) {
                    av_packet_free(&other);
                }
            }
        };

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
        std::unique_ptr<AVPacket, AVPacketDeleter> thePtr{ nullptr , AVPacketDeleter{} };
    };

    struct DiscordCoreAPI_Dll AVCodecWrapper {

        struct DiscordCoreAPI_Dll AVCodecDeleter {
            void operator()(AVCodec*) {};
        };

        AVCodecWrapper& operator=(AVCodec* other) {
            this->thePtr.reset(other);
            return *this;
        }

        operator AVCodec*() {
            return this->thePtr.get();
        }

        AVCodecWrapper(nullptr_t) {};

    protected:
        std::unique_ptr<AVCodec, AVCodecDeleter> thePtr{ nullptr , AVCodecDeleter{} };
    };

    struct DiscordCoreAPI_Dll AVStreamWrapper {

        struct DiscordCoreAPI_Dll AVStreamDeleter {
            void operator()(AVStream*) {};
        };

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
        std::unique_ptr<AVStream, AVStreamDeleter> thePtr{ nullptr , AVStreamDeleter{} };
    };

    struct DiscordCoreAPI_Dll BuildAudioDecoderData {
    public:
        int64_t totalFileSize{ 0 };
        int32_t bufferMaxSize{ 0 };
    };

    class DiscordCoreAPI_Dll AudioDecoder {
    public:

        AudioDecoder(BuildAudioDecoderData dataPackage);

        static int32_t FileStreamRead(void* opaque, uint8_t* buf, int32_t);

        TSUnboundedMessageBlock<std::vector<uint8_t>>& getInputBuffer();

        void submitDataForDecoding(std::vector<uint8_t> dataToDecode);

        void updateBufferRefreshTime(int32_t newRefreshTime);

        bool getFrame(RawFrameData& dataPackage);

        bool haveWeFailed();

        void cancelMe();

        void startMe();

        ~AudioDecoder();

    protected:

        int32_t audioStreamIndex{ 0 }, audioFrameCount{ 0 }, bufferMaxSize{ 0 }, bytesRead{ 0 }, sentFrameCount{ 0 };
        TSUnboundedMessageBlock<std::vector<uint8_t>> inputDataBuffer{};
        TSUnboundedMessageBlock<RawFrameData> outDataBuffer{};
        AVFrameWrapper frame{ nullptr }, newFrame{ nullptr };
        AVCodecContextWrapper audioDecodeContext{ nullptr };
        std::unique_ptr<CoRoutine<void>> theTask{ nullptr };
        AVFormatContextWrapper formatContext{ nullptr };
        std::atomic<bool> haveWeFailedBool{ false };
        AVIOContextWrapper ioContext{ nullptr };
        SwrContextWrapper swrContext{ nullptr };
        AVStreamWrapper audioStream{ nullptr };
        int32_t refreshTimeForBuffer{ 10000 };
        std::vector<uint8_t> currentBuffer{};
        AVPacketWrapper packet{ nullptr };
        AVCodecWrapper codec{ nullptr };
        bool areWeQuitting{ false };
        bool haveWeBooted{ false };
        int64_t totalFileSize{ 0 };

        CoRoutine<void> run();
    };

}
#endif