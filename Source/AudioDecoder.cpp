/*
*    # License

   Copyright 2021 Chris M.

   Licensed under the Apache License, Version 2.0 (the "License");
   you may not use this file except in compliance with the License.
   You may obtain a copy of the License at

       http://www.apache.org/licenses/LICENSE-2.0

   Unless required by applicable law or agreed to in writing, software
   distributed under the License is distributed on an "AS IS" BASIS,
   WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
   See the License for the specific language governing permissions and
   limitations under the License.
*/
/// AudioDecoder.cpp - Source file for the audio decoder class.
/// Jul 29, 2021
/// Chris M.
/// https://github.com/RealTimeChris/DiscordCoreAPI
/// \file AudioDecoder.cpp

#include <AudioDecoder.hpp>

namespace DiscordCoreAPI {

    AudioDecoder::AudioDecoder(BuildAudioDecoderData dataPackage, bool doWePrintNew) {
        this->bufferMaxSize = dataPackage.bufferMaxSize;
        this->totalFileSize = dataPackage.totalFileSize;
        this->doWePrint = doWePrintNew;
    }

    void AudioDecoder::submitDataForDecoding(std::string dataToDecode) {
        this->inputDataBuffer.send(dataToDecode);
    }

    bool AudioDecoder::getFrame(RawFrameData& dataPackage) {
        if (!this->areWeQuitting) {
            if (this->outDataBuffer.tryReceive(dataPackage)) {
                if (dataPackage.sampleCount != -1) {
                    return true;
                }
            }
        }
        return false;
    }

    void AudioDecoder::cancelMe() {
        this->refreshTimeForBuffer.store(10, std::memory_order_seq_cst);
        this->inputDataBuffer.clearContents();
        this->inputDataBuffer.send(std::string());
        this->inputDataBuffer.send(std::string());
        this->inputDataBuffer.send(std::string());
        this->inputDataBuffer.send(std::string());
        this->inputDataBuffer.send(std::string());
        this->areWeQuitting = true;
        this->theTask->cancel();
    }

    bool AudioDecoder::haveWeFailed() {
        return this->haveWeFailedBool.load(std::memory_order_seq_cst);
    }

    void AudioDecoder::startMe() {
        this->theTask = std::make_unique<CoRoutine<void>>(this->run());
    };

    int32_t AudioDecoder::FileStreamRead(void* opaque, uint8_t* buf, int32_t) {
        AudioDecoder* stream = static_cast<AudioDecoder*>(opaque);
        stream->bytesRead = 0;
        stream->currentBuffer = std::string();
        if (stream->areWeQuitting) {
            RawFrameData frameData{};
            frameData.sampleCount = 0;
            stream->outDataBuffer.send(std::move(frameData));
            stream->areWeQuitting = true;
            return AVERROR_EOF;
        }
        if (waitForTimeToPass(stream->inputDataBuffer, stream->currentBuffer, stream->refreshTimeForBuffer.load(std::memory_order_seq_cst))){
            RawFrameData frameData{};
            frameData.sampleCount = 0;
            stream->outDataBuffer.send(std::move(frameData));
            stream->areWeQuitting = true;
            return AVERROR_EOF;
        }
        if (stream->currentBuffer.size() > 0) {
            stream->bytesRead = stream->currentBuffer.size();
        }
        else {
            RawFrameData frameData{};
            frameData.sampleCount = 0;
            stream->outDataBuffer.send(std::move(frameData));
            stream->areWeQuitting = true;
            return AVERROR_EOF;
        }
        for (int32_t x = 0; x < stream->bytesRead; x += 1) {
            buf[x] = stream->currentBuffer[x];
        }
        if (stream->ioContext->buf_ptr - stream->ioContext->buffer >= stream->totalFileSize){
            RawFrameData frameData{};
            frameData.sampleCount = 0;
            stream->outDataBuffer.send(std::move(frameData));
            stream->areWeQuitting = true;
            return AVERROR_EOF;
        }
        return static_cast<int32_t>(stream->bytesRead);
    }

    CoRoutine<void> AudioDecoder::run() {
        auto cancelHandle = co_await NewThreadAwaitable<void>();
        try {
            if (!this->haveWeBooted) {
                unsigned char* fileStreamBuffer = static_cast<unsigned char*>(av_malloc(this->bufferMaxSize));
                if (fileStreamBuffer == nullptr) {
                    this->haveWeFailedBool.store(true, std::memory_order_seq_cst);
                    throw std::runtime_error("Failed to allocate filestreambuffer.\n\n");
                }

                this->ioContext = avio_alloc_context(
                    fileStreamBuffer,
                    static_cast<int32_t>(this->bufferMaxSize),
                    0,
                    this,
                    &AudioDecoder::FileStreamRead,
                    0,
                    0
                );

                if (this->ioContext == nullptr) {
                    this->haveWeFailedBool.store(true, std::memory_order_seq_cst);
                    throw std::runtime_error("Failed to allocate AVIOContext.\n\n");
                }

                this->formatContext = avformat_alloc_context();

                if (!this->formatContext) {
                    this->haveWeFailedBool.store(true, std::memory_order_seq_cst);
                    throw std::runtime_error("Could not allocate the format context.\n\n");
                }

                this->formatContext->pb = this->ioContext;
                this->formatContext->flags |= AVFMT_FLAG_CUSTOM_IO;
                if (avformat_open_input(*this->formatContext, "memory", nullptr, nullptr) < 0) {
                    this->haveWeFailedBool.store(true, std::memory_order_seq_cst);
                    throw std::runtime_error("Error opening AVFormatContext.\n\n");
                }
                *this->formatContext.getBoolPtr() = true;
                AVMediaType type = AVMediaType::AVMEDIA_TYPE_AUDIO;
                int32_t ret = av_find_best_stream(this->formatContext, type, -1, -1, NULL, 0);
                if (ret < 0) {
                    std::string newString = "Could not find ";
                    newString += av_get_media_type_string(type);
                    newString += " stream in input memory stream.\n\n";
                    this->haveWeFailedBool.store(true, std::memory_order_seq_cst);
                    throw std::runtime_error(newString.c_str());
                }
                
                else {
                    this->audioStreamIndex = ret;
                    this->audioStream = this->formatContext->streams[this->audioStreamIndex];
                    if (!this->audioStream) {
                        this->haveWeFailedBool.store(true, std::memory_order_seq_cst);
                        throw std::runtime_error("Could not find an audio stream.\n\n");
                    }

                    if (avformat_find_stream_info(this->formatContext, NULL) < 0) {
                        this->haveWeFailedBool.store(true, std::memory_order_seq_cst);
                        throw std::runtime_error("Could not find stream information.\n\n");
                    }

                    this->codec = avcodec_find_decoder(this->audioStream->codecpar->codec_id);
                    if (!this->codec) {
                        std::string newString = "Failed to find ";
                        newString += av_get_media_type_string(type);
                        newString += " decoder.\n\n";
                        this->haveWeFailedBool.store(true, std::memory_order_seq_cst);
                        throw std::runtime_error(newString.c_str());
                    }

                    this->audioDecodeContext = avcodec_alloc_context3(this->codec);
                    if (!this->audioDecodeContext) {
                        std::string newString = "Failed to allocate the ";
                        newString += av_get_media_type_string(type);
                        newString += " AVCodecContext.\n\n";
                        this->haveWeFailedBool.store(true, std::memory_order_seq_cst);
                        throw std::runtime_error(newString.c_str());
                    }

                    if (avcodec_parameters_to_context(this->audioDecodeContext, this->audioStream->codecpar) < 0) {
                        std::string newString = "Failed to copy ";
                        newString += av_get_media_type_string(type);
                        newString += " codec parameters to decoder context.\n\n";
                        this->haveWeFailedBool.store(true, std::memory_order_seq_cst);
                        throw std::runtime_error(newString.c_str());
                    }

                    if (avcodec_open2(this->audioDecodeContext, this->codec, NULL) < 0) {
                        std::string newString = "Failed to open ";
                        newString += av_get_media_type_string(type);
                        newString += " AVCodecContext.\n\n";
                        this->haveWeFailedBool.store(true, std::memory_order_seq_cst);
                        throw std::runtime_error(newString.c_str());
                    }

                    this->swrContext = swr_alloc_set_opts(NULL, AV_CH_LAYOUT_STEREO, AVSampleFormat::AV_SAMPLE_FMT_S16, 48000, AV_CH_LAYOUT_STEREO, this->audioDecodeContext->sample_fmt, this->audioDecodeContext->sample_rate, 0, nullptr);
                    swr_init(this->swrContext);
                    if (this->doWePrint) {
                        av_dump_format(this->formatContext, 0, "memory", 0);
                    }
                }

                this->haveWeBooted = true;
            }

            if (this->currentBuffer.size() > 0) {
                this->packet = av_packet_alloc();
                if (!this->packet) {
                    this->haveWeFailedBool.store(true, std::memory_order_seq_cst);
                    throw std::runtime_error("Error: Could not allocate packet\n\n");
                }

                this->frame = av_frame_alloc();
                if (!this->frame) {
                    this->haveWeFailedBool.store(true, std::memory_order_seq_cst);
                    throw std::runtime_error("Error: Could not allocate frame\n\n");
                }

                this->newFrame = av_frame_alloc();
                if (!this->newFrame) {
                    this->haveWeFailedBool.store(true, std::memory_order_seq_cst);
                    throw std::runtime_error("Error: Could not allocate new-frame\n\n");
                }

                while (true) {
                    if (cancelHandle.promise().isItStopped() || this->areWeQuitting || av_read_frame(this->formatContext, this->packet) != 0) {
                        break;
                    }
                    if (this->packet->stream_index == this->audioStreamIndex) {
                        int32_t ret = avcodec_send_packet(this->audioDecodeContext, this->packet);
                        if (ret < 0) {
                            char charString[32];
                            av_strerror(ret, charString, 32);
                            std::string newString = "Error submitting a packet for decoding (" + std::to_string(ret) + "), " + charString + ".\n\n";
                            this->haveWeFailedBool.store(true, std::memory_order_seq_cst);
                            throw std::runtime_error(newString.c_str());
                        }
                        if (ret >= 0) {
                            ret = avcodec_receive_frame(this->audioDecodeContext, this->frame);
                            if (ret < 0) {
                                std::string newString = "Error during decoding (" + std::to_string(ret) + ")\n\n";
                                this->haveWeFailedBool.store(true, std::memory_order_seq_cst);
                                throw std::runtime_error(newString.c_str());
                            }

                            if (!swr_is_initialized(this->swrContext)) {
                                swr_init(this->swrContext);
                            }
                            this->newFrame->channel_layout = AV_CH_LAYOUT_STEREO;
                            this->newFrame->sample_rate = 48000;
                            this->newFrame->format = static_cast<int32_t>(AVSampleFormat::AV_SAMPLE_FMT_S16);
                            this->newFrame->nb_samples = frame->nb_samples;
                            this->newFrame->pts = frame->pts;
                            swr_convert_frame(this->swrContext, this->newFrame, this->frame);
                            int32_t unpadded_linesize = this->newFrame->nb_samples * av_get_bytes_per_sample((AVSampleFormat)this->newFrame->format) * 2;
                            std::vector<uint8_t> newVector{};
                            newVector.resize(unpadded_linesize);
                            for (int32_t x = 0; x < unpadded_linesize; x += 1) {
                                newVector[x] = this->newFrame->extended_data[0][x];
                            }
                            RawFrameData rawFrame{};
                            rawFrame.data = newVector;
                            rawFrame.sampleCount = newFrame->nb_samples;
                            this->outDataBuffer.send(std::move(rawFrame));
                            int64_t sampleCount = swr_get_delay(this->swrContext, this->newFrame->sample_rate);
                            if (sampleCount > 0) {
                                if (!swr_is_initialized(this->swrContext)) {
                                    swr_init(this->swrContext);
                                }
                                swr_convert_frame(this->swrContext, this->newFrame, nullptr);
                                std::vector<uint8_t> newVector02{};
                                newVector02.resize(*this->newFrame->linesize);
                                for (int32_t x = 0; x < *this->newFrame->linesize; x += 1) {
                                    newVector02[x] = this->newFrame->extended_data[0][x];
                                }
                                RawFrameData rawFrame02{};
                                rawFrame02.data = newVector02;
                                rawFrame02.sampleCount = newFrame->nb_samples;
                                this->outDataBuffer.send(std::move(rawFrame02));
                            }
                            if (ret < 0 || newFrame->nb_samples == 0) {
                                this->haveWeFailedBool.store(true, std::memory_order_seq_cst);
                                throw std::runtime_error("Return value is less than zero!\n\n");
                            }
                        }
                        else {
                            break;
                        }
                    }
                    else {
                        break;
                    }
                    this->frame = av_frame_alloc();
                    this->newFrame = av_frame_alloc();
                    this->packet = av_packet_alloc();
                    if (cancelHandle.promise().isItStopped() || this->areWeQuitting) {
                        break;
                    }
                }
                if (this->doWePrint) {
                    std::cout << shiftToBrightGreen() << "Completed decoding!" << std::endl << reset() << std::endl;
                }
                co_return;
            }
            co_return;
        }
        catch (...) {
            this->haveWeFailedBool.store(true, std::memory_order_seq_cst);
            reportException("AudioDecoder::run");
            co_return;
        }
    }

    AudioDecoder::~AudioDecoder() {
        this->cancelMe();
    }

};