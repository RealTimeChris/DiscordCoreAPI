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
        BuildSongDecoderData() {};
        IBuffer dataBuffer;
        size_t totalFileSize = 0;
    };

    class SongDecoder {
    public:

        SongDecoder(){}

        SongDecoder(BuildSongDecoderData dataPackage) {
            vector<uint8_t> newVector;
            for (unsigned int x = 0; x < dataPackage.dataBuffer.Length(); x += 1) {
                newVector.push_back(dataPackage.dataBuffer.data()[x]);
            }
            this->currentBuffer = newVector;
            this->totalFileSize = (int)dataPackage.totalFileSize;
            // Define your buffer 

            // A IStream - you choose where it comes from.

            // Alloc a buffer for the stream.
            unsigned char* fileStreamBuffer = (unsigned char*)av_malloc(this->currentBuffer.size());
            if (fileStreamBuffer == nullptr) {
                // out of memory
            }

            // Get a AVContext stream
            this->ioContext = avio_alloc_context(
                fileStreamBuffer,    // Buffer
                (int)this->currentBuffer.size(),  // Buffer size
                0,                   // Buffer is only readable - set to 1 for read/write
                this,      // User (your) specified data
                FileStreamRead,      // Function - Reading Packets (see example)
                0,                  // Function - Write Packets
                0   // Function - Seek to position in stream (see example)
            );

            if (this->ioContext == nullptr) {
                // out of memory
            }

            // Allocate a AVContext
            this->formatContext = avformat_alloc_context();

            // Set up the Format Context
            this->formatContext->pb = this->ioContext;
            this->formatContext->flags |= AVFMT_FLAG_CUSTOM_IO; // we set up our own IO

            // Open "file" (open our custom IO)
            // Empty string is where filename would go. Doesn't matter since we aren't reading a file
            // NULL params are format and demuxer settings, respectively
            if (avformat_open_input(&formatContext, "", nullptr, nullptr) < 0) {
                // Error opening file
                cout << "Error opening format context." << endl;
                return;
            }
            int ret;
            AVMediaType type = AVMediaType::AVMEDIA_TYPE_AUDIO;
            ret = av_find_best_stream(this->formatContext, type, -1, -1, NULL, 0);
            if (ret < 0) {
                fprintf(stderr, "Could not find %s stream in input memory stream.\n", av_get_media_type_string(type));
                return;
            }
            else {
                this->audioStreamIndex = ret;
                this->audioStream = this->formatContext->streams[this->audioStreamIndex];
                if (!audioStream) {
                    fprintf(stderr, "Could not find collect an audio stream.\n");
                    return;
                }

                /* retrieve stream information */
                if (avformat_find_stream_info(this->formatContext, NULL) < 0) {
                    fprintf(stderr, "Could not find stream information\n");
                    return;
                }
                /* find decoder for the stream */
                this->dec = avcodec_find_decoder(this->audioStream->codecpar->codec_id);
                if (!this->dec) {
                    fprintf(stderr, "Failed to find %s codec\n",
                        av_get_media_type_string(type));
                    return;
                }

                /* Allocate a codec context for the decoder */
                this->audioDecodeContext = avcodec_alloc_context3(this->dec);
                if (!this->audioDecodeContext) {
                    fprintf(stderr, "Failed to allocate the %s codec context\n",
                        av_get_media_type_string(type));
                    return;
                }

                /* Copy codec parameters from input stream to output codec context */
                if ((ret = avcodec_parameters_to_context(this->audioDecodeContext, this->audioStream->codecpar)) < 0) {
                    fprintf(stderr, "Failed to copy %s codec parameters to decoder context\n",
                        av_get_media_type_string(type));
                    return;
                }

                /* Init the decoders */
                if ((ret = avcodec_open2(this->audioDecodeContext, this->dec, NULL)) < 0) {
                    fprintf(stderr, "Failed to open %s codec\n",
                        av_get_media_type_string(type));
                    return;
                }
                this->swrContext = swr_alloc();
                this->swrContext = swr_alloc_set_opts(NULL, AV_CH_LAYOUT_STEREO, av_get_alt_sample_fmt(this->audioDecodeContext->sample_fmt, 0), 48000, AV_CH_LAYOUT_STEREO, this->audioDecodeContext->sample_fmt, this->audioDecodeContext->sample_rate, 0, nullptr);
                swr_init(this->swrContext);

            }
            av_dump_format(this->formatContext, 0, "Memory-Input", 0);
        }

        int avio_seek(AVIOContext* avContext, int offset, int ) {
            avContext->buf_ptr = avContext->buffer + offset;
            return offset;
        }

        vector<RawFrameData> getFrames() {
            if (this->currentBuffer.size() >0){
                vector<RawFrameData> frames{};
                int ret = 0;

                this->packet = av_packet_alloc();
                if (!this->packet) {
                    fprintf(stderr, "Error: Could not allocate packet\n");
                    return frames;
                }

                this->frame = av_frame_alloc();
                if (!this->frame) {
                    fprintf(stderr, "Error: Could not allocate frame\n");
                    return frames;
                }

                this->newFrame = av_frame_alloc();
                if (!this->newFrame) {
                    fprintf(stderr, "Error: Could not allocate frame\n");
                    return frames;
                }

                // read frames from the file
                while (av_read_frame(this->formatContext, this->packet) >= 0) {

                    // check if the packet belongs to a stream we are interested in, otherwise
                    // skip it
                    if (this->packet->stream_index == this->audioStreamIndex) {
                        // submit the packet to the decoder
                        ret = avcodec_send_packet(this->audioDecodeContext, this->packet);
                        if (ret < 0) {
                            char charString[32];
                            av_strerror(ret, charString, 32);
                            fprintf(stderr, "Error submitting a packet for decoding (%s), %s.\n", to_string(ret).c_str(), charString);
                            return frames;
                        }
                        if (ret >= 0) {

                            ret = avcodec_receive_frame(this->audioDecodeContext, this->frame);
                            if (ret < 0) {
                                fprintf(stderr, "Error during decoding (%s)\n", to_string(ret).c_str());
                                return frames;
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
                            printf("Audio Frame #:%d Number of Samples:%d pts:%s\n", this->audioFrameCount, this->newFrame->nb_samples, to_string(this->newFrame->pts).c_str());
                            size_t unpadded_linesize = this->newFrame->nb_samples * av_get_bytes_per_sample((AVSampleFormat)this->newFrame->format) * 2;
                            vector<uint8_t> newVector{};
                            for (int x = 0; x < unpadded_linesize; x += 1) {
                                newVector.push_back(this->newFrame->extended_data[0][x]);
                            }

                            RawFrameData rawFrame{};
                            rawFrame.data = newVector;
                            rawFrame.sampleCount = newFrame->nb_samples;
                            frames.push_back(rawFrame);

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
                                frames.push_back(rawFrame02);
                            }
                            if (ret < 0) {
                                cout << "Return value is less than zero!" << endl << endl;
                                return frames;
                            }
                        }
                    }
                    av_packet_unref(this->packet);
                    av_frame_unref(this->frame);
                    av_frame_unref(this->newFrame);
                }
                av_packet_unref(this->packet);
                av_packet_free(&this->packet);
                av_frame_unref(this->frame);
                av_frame_free(&this->frame);
                av_frame_unref(this->newFrame);
                av_frame_free(&this->newFrame);
                return frames;
            }
            return vector<RawFrameData>();
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
        AVIOContext* ioContext{ nullptr };
        AVFormatContext* formatContext{ nullptr };
        AVCodecContext* audioDecodeContext{ nullptr };
        AVStream* audioStream{ nullptr };
        SwrContext* swrContext{ nullptr };
        AVCodec* dec{ nullptr };

        int  audioStreamIndex{ 0 }, audioFrameCount{ 0 }, totalFileSize{ 0 };
        AVFrame* frame{ nullptr }, * newFrame{ nullptr };
        AVPacket* packet{ nullptr };
        vector<uint8_t> currentBuffer{};

        static int FileStreamRead(void* opaque, uint8_t* buf, int bufSize)
        {   
            SongDecoder* stream = reinterpret_cast<SongDecoder*>(opaque);
            int bytesRead;
            if (stream->currentBuffer.size() > 0) {
                bytesRead = (int)stream->currentBuffer.size();
            }
            if (stream->ioContext->buf_ptr - stream->ioContext->buffer >= stream->totalFileSize) {
                return AVERROR_EOF;
            }

            for (int x = 0; x < bufSize; x += 1) {
                buf[x] = stream->currentBuffer[x];
            }

            return bufSize;
        }

    };
    
}

#endif
