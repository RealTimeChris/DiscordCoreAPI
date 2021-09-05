// DecodingStuff.hpp - Header for the "decoding stuff".
// Jul 29, 2021
// Chris M.
// https://github.com/RealTimeChris

#pragma once

#ifndef _DECODING_STUFF_
#define _DECODING_STUFF_

#include "../pch.h"

namespace DiscordCoreAPI {

    struct BuildSongDecoderData {
    public:
        unbounded_buffer<vector<uint8_t>>* dataBuffer{};
        size_t totalFileSize{ 0 };
        size_t bufferMaxSize{ 0 };
    };

    class SongDecoder : agent {
    public:

        friend class YouTubeAPICore;

        SongDecoder(BuildSongDecoderData dataPackage, shared_ptr<DiscordCoreInternal::ThreadContext> threadContextNew) : agent(*threadContextNew->scheduler->scheduler) {
            this->bufferMaxSize = (int)dataPackage.bufferMaxSize;
            this->totalFileSize = (int)dataPackage.totalFileSize;
            this->completionBuffer = new unbounded_buffer<bool>;
            this->dataBuffer = dataPackage.dataBuffer;
            this->threadContext = threadContextNew;
            this->start();
        }

        void exit() {
            this->done();
            this->areWeQuitting = true;
        }

        bool getCompletionSignal() {
            return receive(this->completionBuffer);
        }

        bool getFrame(RawFrameData* dataPackage) {

            if (!this->haveWeBooted) {
                receive(this->readyBuffer);
                this->haveWeBooted = true;
            }

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

            return false;

        }

        ~SongDecoder() {
            this->done();
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
            this->threadContext->releaseGroup();
        }

    protected:
        int audioStreamIndex{ 0 }, audioFrameCount{ 0 }, totalFileSize{ 0 }, bufferMaxSize{ 0 }, bytesRead{ 0 }, sentFrameCount{ 0 }, bytesReadTotal{ 0 };
        shared_ptr<DiscordCoreInternal::ThreadContext> threadContext{ nullptr };
        unbounded_buffer<bool>* completionBuffer{ nullptr };
        AVFrame* frame{ nullptr }, * newFrame{ nullptr };
        unbounded_buffer<vector<uint8_t>>* dataBuffer{};
        unbounded_buffer<RawFrameData> outDataBuffer{};
        AVCodecContext* audioDecodeContext{ nullptr };
        AVFormatContext* formatContext{ nullptr };
        unbounded_buffer<bool> readyBuffer{};
        SwrContext* swrContext{ nullptr };
        AVIOContext* ioContext{ nullptr };
        AVStream* audioStream{ nullptr };
        vector<uint8_t> currentBuffer{};
        AVPacket* packet{ nullptr };
        bool areWeQuitting{ false };
        bool haveWeBooted{ false };
        AVCodec* codec{ nullptr };

        void run() {
            if (!this->haveWeBooted) {
                vector<uint8_t> newVector;
                auto buffer = receive(this->dataBuffer);
                for (unsigned int x = 0; x < buffer.size(); x += 1) {
                    newVector.push_back(buffer.data()[x]);
                }
                this->currentBuffer = newVector;
                unsigned char* fileStreamBuffer = (unsigned char*)av_malloc(this->bufferMaxSize);
                if (fileStreamBuffer == nullptr) {
                    cout << "Failed to allocate filestreambuffer.\n\n";
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
                    cout << "Failed to allocate AVIOContext.\n\n";
                }

                this->formatContext = avformat_alloc_context();

                if (!this->formatContext) {
                    cout << "Could not allocate the format context.\n\n";
                    done();
                    return;
                }

                this->formatContext->pb = this->ioContext;
                this->formatContext->flags |= AVFMT_FLAG_CUSTOM_IO;

                if (avformat_open_input(&this->formatContext, "memory", nullptr, nullptr) < 0) {
                    cout << "Error opening AVFormatContext.\n\n";
                    return;
                }
                AVMediaType type = AVMediaType::AVMEDIA_TYPE_AUDIO;
                int ret = av_find_best_stream(this->formatContext, type, -1, -1, NULL, 0);
                if (ret < 0) {
                    string newString = "Could not find ";
                    newString += av_get_media_type_string(type);
                    newString += " stream in input memory stream.\n\n";
                    cout << newString;
                    done();
                    return;
                }
                else {
                    this->audioStreamIndex = ret;
                    this->audioStream = this->formatContext->streams[this->audioStreamIndex];
                    if (!this->audioStream) {
                        cout << "Could not find an audio stream.\n\n";
                        done();
                        return;
                    }

                    if (avformat_find_stream_info(this->formatContext, NULL) < 0) {
                        cout << "Could not find stream information.\n\n";
                        done();
                        return;
                    }

                    this->codec = avcodec_find_decoder(this->audioStream->codecpar->codec_id);
                    if (!this->codec) {
                        string newString = "Failed to find ";
                        newString += av_get_media_type_string(type);
                        newString += " decoder.\n\n";
                        cout << newString;
                        done();
                        return;
                    }

                    this->audioDecodeContext = avcodec_alloc_context3(this->codec);
                    if (!this->audioDecodeContext) {
                        string newString = "Failed to allocate the ";
                        newString += av_get_media_type_string(type);
                        newString += " AVCodecContext.\n\n";
                        cout << newString;
                        done();
                        return;
                    }

                    if (avcodec_parameters_to_context(this->audioDecodeContext, this->audioStream->codecpar) < 0) {
                        string newString = "Failed to copy ";
                        newString += av_get_media_type_string(type);
                        newString += " codec parameters to decoder context.\n\n";
                        cout << newString;
                        done();
                        return;
                    }

                    if (avcodec_open2(this->audioDecodeContext, this->codec, NULL) < 0) {
                        string newString = "Failed to open ";
                        newString += av_get_media_type_string(type);
                        newString += " AVCodecContext.\n\n";
                        cout << newString;
                        done();
                        return;
                    }

                    this->swrContext = swr_alloc();
                    this->swrContext = swr_alloc_set_opts(NULL, AV_CH_LAYOUT_STEREO, av_get_alt_sample_fmt(this->audioDecodeContext->sample_fmt, 0), 48000, AV_CH_LAYOUT_STEREO, this->audioDecodeContext->sample_fmt, this->audioDecodeContext->sample_rate, 0, nullptr);
                    swr_init(this->swrContext);
                    av_dump_format(this->formatContext, 0, "memory", 0);
                    send(this->readyBuffer, true);
                }
            }
            if (this->currentBuffer.size() > 0) {

                this->packet = av_packet_alloc();
                if (!this->packet) {
                    cout << "Error: Could not allocate packet\n\n";
                    done();
                    return;
                }

                this->frame = av_frame_alloc();
                if (!this->frame) {
                    cout << "Error: Could not allocate frame\n\n";
                    done();
                    return;
                }

                this->newFrame = av_frame_alloc();
                if (!this->newFrame) {
                    cout << "Error: Could not allocate new-frame\n\n";
                    done();
                    return;
                }

                while (av_read_frame(this->formatContext, this->packet) >= 0) {
                    if (this->packet->stream_index == this->audioStreamIndex) {
                        int ret = avcodec_send_packet(this->audioDecodeContext, this->packet);
                        if (ret < 0) {
                            char charString[32];
                            av_strerror(ret, charString, 32);
                            cout << "Error submitting a packet for decoding (" + to_string(ret) + "), " + charString + ".\n\n";
                            done();
                            return;
                        }
                        if (ret >= 0) {
                            ret = avcodec_receive_frame(this->audioDecodeContext, this->frame);
                            if (ret < 0) {
                                cout << "Error during decoding (" + to_string(ret) + ")\n\n";
                                done();
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
                                cout << "Return value is less than zero!\n\n";
                                this->done();
                                break;
                            }
                        }
                        else {
                            this->done();
                            break;
                        }
                    }
                    else {
                        this->done();
                        break;
                    }
                    av_packet_unref(this->packet);
                    av_frame_unref(this->frame);
                    av_frame_unref(this->newFrame);
                    this->frame = av_frame_alloc();
                    this->newFrame = av_frame_alloc();
                    this->packet = av_packet_alloc();
                    if (this->areWeQuitting) {
                        this->done();
                        break;
                    }
                }
                send(this->completionBuffer, true);
                av_packet_unref(this->packet);
                av_packet_free(&this->packet);
                av_frame_unref(this->frame);
                av_frame_free(&this->frame);
                av_frame_unref(this->newFrame);
                av_frame_free(&this->newFrame);
                cout << "Completed decoding!" << endl << endl;
                return;
            }
        }

        static int FileStreamRead(void* opaque, uint8_t* buf, int) {
            SongDecoder* stream = reinterpret_cast<SongDecoder*>(opaque);
            stream->bytesRead = 0;
            stream->currentBuffer = vector<uint8_t>();
            if (stream->bytesReadTotal >= stream->totalFileSize) {
                return AVERROR_EOF;
            }
            try {
                stream->currentBuffer = receive(stream->dataBuffer, 10000);
            }
            catch (exception&) {};
            if (stream->currentBuffer.size() > 0) {
                stream->bytesRead = (int)stream->currentBuffer.size();
                stream->bytesReadTotal += stream->bytesRead;
            }
            else {
                RawFrameData frameData;
                frameData.sampleCount = 0;
                send(stream->outDataBuffer, frameData);
                return AVERROR_EOF;
            }

            for (int x = 0; x < stream->bytesRead; x += 1) {
                buf[x] = stream->currentBuffer[x];
            }

            if (stream->ioContext->buf_ptr - stream->ioContext->buffer >= stream->totalFileSize || stream->currentBuffer.size() == 0) {
                cout << "End of file reached!\n\n";
                return AVERROR_EOF;
            }

            return stream->bytesRead;
        }
    };

}

#endif
