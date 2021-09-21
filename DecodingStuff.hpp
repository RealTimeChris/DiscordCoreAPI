// DecodingStuff.hpp - Header for the "decoding stuff".
// Jul 29, 2021
// Chris M.
// https://github.com/RealTimeChris

#pragma once

#ifndef _DECODING_STUFF_
#define _DECODING_STUFF_

#include "../pch.h"

namespace DiscordCoreAPI {

    class SoundCloudAPI;

    struct BuildSongDecoderData {
    public:
        unbounded_buffer<vector<uint8_t>>* sendEncodedAudioDataBuffer{};
        size_t totalFileSize{ 0 };
        size_t bufferMaxSize{ 0 };
    };

    class SongDecoder : DiscordCoreInternal::ThreadContext, agent {
    public:

        friend class SoundCloudAPI;
        friend class YouTubeAPI;

        SongDecoder(BuildSongDecoderData dataPackage) 
            : ThreadContext(*DiscordCoreInternal::ThreadManager::getThreadContext(DiscordCoreInternal::ThreadType::Music).get()), agent(*this->scheduler->scheduler) {
            this->bufferMaxSize = (int)dataPackage.bufferMaxSize;
            this->totalFileSize = (int)dataPackage.totalFileSize;
            this->inputDataBuffer = dataPackage.sendEncodedAudioDataBuffer;
        }

        void startMe() {
            this->start();
            this->readyToStartEvent.wait(2500);
        }

        bool getFrame(RawFrameData* dataPackage) {
            if (this != nullptr) {
                if (!this->areWeQuitting) {
                    RawFrameData rawFrame;
                    if (try_receive(this->outDataBuffer, rawFrame)) {
                        if (rawFrame.sampleCount != -1) {
                            *dataPackage = rawFrame;
                            return true;
                        }
                        else {
                            return false;
                        }
                    }
                }
            }
            return false;
        }

        void submitDataForDecoding(vector<uint8_t> dataToDecode, int maxBufferSize = 0) {
            send(this->inputDataBuffer, dataToDecode);
            if (maxBufferSize != 0) {
                this->ioContext->buffer_size = maxBufferSize;
                this->bufferMaxSize = maxBufferSize;
            }
        }

        ~SongDecoder() {
            if (this->formatContext) {
                avformat_close_input(&this->formatContext);
            }
            if (this->formatContext) {
                avformat_free_context(this->formatContext);
            }
            if (this->ioContext) {
                av_free(this->ioContext);
            }
            if (this->frame) {
                av_frame_unref(this->frame);
                av_frame_free(&this->frame);
            }
            if (this->newFrame) {
                av_frame_unref(this->newFrame);
                av_frame_free(&this->newFrame);
            }
            if (this->audioDecodeContext) {
                avcodec_free_context(&this->audioDecodeContext);
            }
            if (this->packet) {
                av_packet_free(&this->packet);
            }
        }

    protected:
        int audioStreamIndex{ 0 }, audioFrameCount{ 0 }, totalFileSize{ 0 }, bufferMaxSize{ 0 }, bytesRead{ 0 }, sentFrameCount{ 0 };
        unbounded_buffer<vector<uint8_t>>* inputDataBuffer{};
        AVFrame* frame{ nullptr }, * newFrame{ nullptr };
        unbounded_buffer<RawFrameData> outDataBuffer{};
        AVCodecContext* audioDecodeContext{ nullptr };
        AVFormatContext* formatContext{ nullptr };
        concurrency::event readyToStartEvent {};
        unbounded_buffer<bool> readyBuffer{};
        SwrContext* swrContext{ nullptr };
        AVIOContext* ioContext{ nullptr };
        int refreshTimeForBuffer{ 10000 };
        AVStream* audioStream{ nullptr };
        vector<uint8_t> currentBuffer{};
        AVPacket* packet{ nullptr };
        bool areWeQuitting{ false };
        bool haveWeBooted{ false };
        AVCodec* codec{ nullptr };

        void run() {
            try {
                if (!this->haveWeBooted) {
                    vector<uint8_t> newVector;
                    auto buffer = receive(this->inputDataBuffer);
                    for (unsigned int x = 0; x < buffer.size(); x += 1) {
                        newVector.push_back(buffer.data()[x]);
                    }
                    this->currentBuffer = newVector;
                    unsigned char* fileStreamBuffer = (unsigned char*)av_malloc(this->bufferMaxSize);
                    if (fileStreamBuffer == nullptr) {
                        throw exception("Failed to allocate filestreambuffer.\n\n");
                        return;
                    }
                    this->ioContext = avio_alloc_context(
                        fileStreamBuffer,
                        (int)this->bufferMaxSize,
                        0,
                        this,
                        FileStreamRead,
                        0,
                        0
                    );

                    if (this->ioContext == nullptr) {
                        throw exception("Failed to allocate AVIOContext.\n\n");
                        return;
                    }

                    this->formatContext = avformat_alloc_context();

                    if (!this->formatContext) {
                        throw exception("Could not allocate the format context.\n\n");
                        return;
                    }

                    this->formatContext->pb = this->ioContext;
                    this->formatContext->flags |= AVFMT_FLAG_CUSTOM_IO;

                    if (avformat_open_input(&this->formatContext, "memory", nullptr, nullptr) < 0) {
                        cout << "Error opening AVFormatContext.\n\n" << endl;
                        return;
                    }
                    AVMediaType type = AVMediaType::AVMEDIA_TYPE_AUDIO;
                    int ret = av_find_best_stream(this->formatContext, type, -1, -1, NULL, 0);
                    if (ret < 0) {
                        string newString = "Could not find ";
                        newString += av_get_media_type_string(type);
                        newString += " stream in input memory stream.\n\n";
                        throw exception(newString.c_str());
                        return;
                    }
                    else {
                        this->audioStreamIndex = ret;
                        this->audioStream = this->formatContext->streams[this->audioStreamIndex];
                        if (!this->audioStream) {
                            throw exception("Could not find an audio stream.\n\n");
                            return;
                        }

                        if (avformat_find_stream_info(this->formatContext, NULL) < 0) {
                            throw exception("Could not find stream information.\n\n");
                            return;
                        }

                        this->codec = avcodec_find_decoder(this->audioStream->codecpar->codec_id);
                        if (!this->codec) {
                            string newString = "Failed to find ";
                            newString += av_get_media_type_string(type);
                            newString += " decoder.\n\n";
                            throw exception(newString.c_str());
                            return;
                        }

                        this->audioDecodeContext = avcodec_alloc_context3(this->codec);
                        if (!this->audioDecodeContext) {
                            string newString = "Failed to allocate the ";
                            newString += av_get_media_type_string(type);
                            newString += " AVCodecContext.\n\n";
                            throw exception(newString.c_str());
                            return;
                        }

                        if (avcodec_parameters_to_context(this->audioDecodeContext, this->audioStream->codecpar) < 0) {
                            string newString = "Failed to copy ";
                            newString += av_get_media_type_string(type);
                            newString += " codec parameters to decoder context.\n\n";
                            throw exception(newString.c_str());
                            return;
                        }

                        if (avcodec_open2(this->audioDecodeContext, this->codec, NULL) < 0) {
                            string newString = "Failed to open ";
                            newString += av_get_media_type_string(type);
                            newString += " AVCodecContext.\n\n";
                            throw exception(newString.c_str());
                            return;
                        }

                        this->swrContext = swr_alloc();
                        this->swrContext = swr_alloc_set_opts(NULL, AV_CH_LAYOUT_STEREO, av_get_alt_sample_fmt(this->audioDecodeContext->sample_fmt, 0), 48000, AV_CH_LAYOUT_STEREO, this->audioDecodeContext->sample_fmt, this->audioDecodeContext->sample_rate, 0, nullptr);
                        swr_init(this->swrContext);
                        av_dump_format(this->formatContext, 0, "memory", 0);
                        send(this->readyBuffer, true);
                    }
                    this->readyToStartEvent.set();
                }
                if (this->currentBuffer.size() > 0) {

                    this->packet = av_packet_alloc();
                    if (!this->packet) {
                        throw exception("Error: Could not allocate packet\n\n");
                        return;
                    }

                    this->frame = av_frame_alloc();
                    if (!this->frame) {
                        throw exception("Error: Could not allocate frame\n\n");
                        return;
                    }

                    this->newFrame = av_frame_alloc();
                    if (!this->newFrame) {
                        throw exception("Error: Could not allocate new-frame\n\n");
                        return;
                    }

                    while (av_read_frame(this->formatContext, this->packet) >= 0) {
                        if (this->packet->stream_index == this->audioStreamIndex) {
                            int ret = avcodec_send_packet(this->audioDecodeContext, this->packet);
                            if (ret < 0) {
                                char charString[32];
                                av_strerror(ret, charString, 32);
                                string newString = "Error submitting a packet for decoding (" + to_string(ret) + "), " + charString + ".\n\n";
                                throw exception(newString.c_str());
                                return;
                            }
                            if (ret >= 0) {
                                ret = avcodec_receive_frame(this->audioDecodeContext, this->frame);
                                if (ret < 0) {
                                    string newString = "Error during decoding (" + to_string(ret) + ")\n\n";
                                    throw exception(newString.c_str());
                                    return;
                                }

                                this->audioFrameCount++;
                                if (!swr_is_initialized(this->swrContext)) {
                                    swr_init(this->swrContext);
                                }
                                this->newFrame->channel_layout = AV_CH_LAYOUT_STEREO;
                                this->newFrame->sample_rate = 48000;
                                this->newFrame->format = AVSampleFormat::AV_SAMPLE_FMT_FLT;
                                this->newFrame->nb_samples = frame->nb_samples;
                                this->newFrame->pts = frame->pts;
                                swr_convert_frame(this->swrContext, this->newFrame, this->frame);
                                //cout << "Audio Frame #: " + to_string(this->audioFrameCount) + " Number of Samples: " + to_string(this->newFrame->nb_samples) + " pts: " + to_string(this->newFrame->pts) + " Size: " + to_string(this->newFrame->pkt_size) + ".\n\n";
                                size_t unpadded_linesize = this->newFrame->nb_samples * av_get_bytes_per_sample((AVSampleFormat)this->newFrame->format) * 2;
                                vector<uint8_t> newVector{};
                                for (int x = 0; x < unpadded_linesize; x += 1) {
                                    newVector.push_back(this->newFrame->extended_data[0][x]);
                                }
                                RawFrameData rawFrame{};
                                rawFrame.data = newVector;
                                rawFrame.sampleCount = newFrame->nb_samples;
                                send(this->outDataBuffer, rawFrame);
                                __int64 sampleCount = swr_get_delay(this->swrContext, this->newFrame->sample_rate);
                                if (sampleCount > 0) {
                                    if (!swr_is_initialized(this->swrContext)) {
                                        swr_init(this->swrContext);
                                    }
                                    swr_convert_frame(this->swrContext, this->newFrame, nullptr);
                                    vector<uint8_t> newVector02{};
                                    for (int x = 0; x < *this->newFrame->linesize; x += 1) {
                                        newVector02.push_back(this->newFrame->extended_data[0][x]);
                                    }

                                    RawFrameData rawFrame02{};
                                    rawFrame02.data = newVector02;
                                    rawFrame02.sampleCount = newFrame->nb_samples;
                                    send(this->outDataBuffer, rawFrame02);
                                }
                                if (ret < 0 || newFrame->nb_samples == 0) {
                                    cout << "Return value is less than zero!\n\n" << endl;
                                    break;
                                }
                            }
                            else {
                                break;
                            }
                        }
                        else {
                            break;
                        }
                        av_packet_unref(this->packet);
                        av_frame_unref(this->frame);
                        av_frame_unref(this->newFrame);
                        this->frame = av_frame_alloc();
                        this->newFrame = av_frame_alloc();
                        this->packet = av_packet_alloc();
                        if (this->areWeQuitting) {
                            break;
                        }
                    }
                    av_packet_unref(this->packet);
                    av_packet_free(&this->packet);
                    av_frame_unref(this->frame);
                    av_frame_free(&this->frame);
                    av_frame_unref(this->newFrame);
                    av_frame_free(&this->newFrame);
                    cout << "Completed decoding!" << endl << endl;
                    this->done();
                }
            }
            catch (...) {
                rethrowException("SongDecoder::run() Error: ");
            }
            done();
        }

        static int FileStreamRead(void* opaque, uint8_t* buf, int) {
            SongDecoder* stream = reinterpret_cast<SongDecoder*>(opaque);
            stream->bytesRead = 0;
            stream->currentBuffer = vector<uint8_t>();
            if (stream->areWeQuitting) {
                return AVERROR_EXIT;
            }
            try {
                stream->currentBuffer = receive(stream->inputDataBuffer, stream->refreshTimeForBuffer);
            }
            catch (...) {
                return AVERROR_EXIT;
            };
            if (stream->currentBuffer.size() > 0) {
                stream->bytesRead = (int)stream->currentBuffer.size();
            }
            else {
                RawFrameData frameData;
                frameData.sampleCount = 0;
                send(stream->outDataBuffer, frameData);
                stream->areWeQuitting = true;
                return AVERROR_EXIT;
            }

            for (int x = 0; x < stream->bytesRead; x += 1) {
                buf[x] = stream->currentBuffer[x];
            }

            if (stream->ioContext->buf_ptr - stream->ioContext->buffer >= stream->totalFileSize) {
                cout << "End of file reached!\n\n";
                return AVERROR_EXIT;
            }

            return stream->bytesRead;
        }
    };

}

#endif
