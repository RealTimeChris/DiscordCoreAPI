// FFMPegStuff.hpp - Header for the FFMPeg stuff
// Jul 19, 2021
// Chris M.
// https://github.com/RealTimeChris

#pragma once

#ifndef _FFMPEG_STUFF_
#define _FFMPEG_STUFF_

#include "../pch.h"

namespace DiscordCoreAPI {

    struct RawOpusData {
        vector<uint32_t> frameIndices;
        vector<vector<uint8_t>> frameData;
        unsigned int currentFrameIndex;
    };

    struct BufferData {
        uint8_t* ptr;
        size_t size;
    };

    static int readPacket(void* opaque, uint8_t* buffer, int bufferSize)
    {
        struct BufferData* bd = (struct BufferData*)opaque;
        bufferSize = FFMIN(bufferSize, (int)bd->size);
        printf("ptr:%p size:%zu\n", bd->ptr, bd->size);
        memcpy(buffer, bd->ptr, bufferSize);
        bd->ptr += bufferSize;
        bd->size -= bufferSize;
        return bufferSize;
    }

    static int getInputStreams(int* streamIndex, AVFormatContext* formatContext, enum AVMediaType type) {
        int ret, streamIndexValue;
        AVStream* stream;

        ret = av_find_best_stream(formatContext, type, -1, -1, NULL, 0);
        if (ret < 0) {
            cout << "Could not find %s stream in input file '%s'\n" << endl;
            return ret;
        }
        else {
            streamIndexValue = ret;
            stream = formatContext->streams[streamIndexValue];
        }

        *streamIndex = streamIndexValue;

        return 0;
    }

    RawOpusData demux(string filePath, vector<uint8_t> rawData) {
        AVStream* audioStream = nullptr;
        AVIOContext* avioContext = nullptr;
        uint8_t* avioContextBuffer, * buffer = new uint8_t[rawData.size()];
        size_t avioContextBufferSize = 4096;
        int audioStreamIndex = -1, ret = 0;
        AVFrame* frame = nullptr;
        AVPacket* packet = nullptr;
        IBuffer outputBuffer = nullptr;
        AVFormatContext* formatContext = ::avformat_alloc_context();
        BufferData bufferData;
        RawOpusData rawOpusData;

        for (unsigned int x = 0; x < rawData.size(); x+=1) {
            memcpy(buffer + x, &rawData[x], sizeof(uint8_t));
        }

        bufferData.ptr = buffer;
        bufferData.size = rawData.size();
        formatContext = avformat_alloc_context();
        if (!formatContext) {
            ret = AVERROR(ENOMEM);
            return RawOpusData();
        }
        avioContextBuffer = (uint8_t*)av_malloc(avioContextBufferSize);
        if (!avioContextBuffer) {
            ret = AVERROR(ENOMEM);
            return RawOpusData();
        }
        avioContext = avio_alloc_context(avioContextBuffer, (int)avioContextBufferSize, 0, &bufferData, &readPacket, NULL, NULL);
        if (!avioContext) {
            ret = AVERROR(ENOMEM);
            return RawOpusData();
        }
        formatContext->pb = avioContext;
        ret = avformat_open_input(&formatContext, NULL, NULL, NULL);
        if (ret < 0) {
            fprintf(stderr, "Could not open input\n");
            return RawOpusData();
        }
        ret = avformat_find_stream_info(formatContext, NULL);
        if (ret < 0) {
            fprintf(stderr, "Could not find stream information\n");
            return RawOpusData();
        }

        av_dump_format(formatContext, 0, filePath.c_str(), 0);

        if (getInputStreams(&audioStreamIndex, formatContext, AVMEDIA_TYPE_AUDIO) >= 0) {
            audioStream = formatContext->streams[audioStreamIndex];
        }

        if (!audioStream) {
            cout << "Could not find audio or video stream in the input, aborting\n" << endl;
            return RawOpusData();
        }

        frame = av_frame_alloc();
        if (!frame) {
            cout << "Could not allocate frame\n" << endl;
            return RawOpusData();
        }

        packet = av_packet_alloc();
        if (!packet) {
            cout << "Could not allocate packet\n" << endl;
            return RawOpusData();
        }

        if (audioStream) {
            printf("Demuxing audio from file '%s' into memorystream!\n", filePath.c_str());
        }

        uint32_t currentIndex = 0;
        while (1) {

            AVStream* in_stream;
            ret = av_read_frame(formatContext, packet);
            if (ret < 0) {
                break;
            }

            in_stream = formatContext->streams[currentIndex];
            if (ret < 0) {
                fprintf(stderr, "Error muxing packet\n");
                break;
            }

            rawOpusData.frameIndices.push_back(packet->buf->size);
            cout << "FRAME DURATION: " << packet->duration << endl;
            cout << "FRAME SIZE ORIGINAL: " << packet->buf->size << endl;

            vector<uint8_t> vectorNew;
            for (int x = 0; x < packet->buf->size; x += 1) {
                vectorNew.push_back(packet->buf->data[x]);
            }

            rawOpusData.frameData.push_back(vectorNew);
            av_packet_unref(packet);
            currentIndex += 1;
        }

        cout << "Demuxing succeeded.\n" << endl;

        avformat_close_input(&formatContext);
        av_packet_free(&packet);
        av_frame_free(&frame);

        return rawOpusData;
    }
    
}

#endif
