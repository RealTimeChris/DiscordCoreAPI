// DecodingStuff.hpp - Header for the "decoding stuff".
// Jul 29, 2021
// Chris M.
// https://github.com/RealTimeChris

#pragma once

#ifndef _DECODING_STUFF_
#define _DECODING_STUFF_

#include "../pch.h"
#include "DemuxingStuff.hpp"

struct __declspec(uuid("5b0d3235-4dba-4d44-865e-8f1d0e4fd04d")) __declspec(novtable) IMemoryBufferByteAccess : ::IUnknown
{
    virtual HRESULT __stdcall GetBuffer(uint8_t** value, uint32_t* capacity) = 0;
};

namespace DiscordCoreAPI {

    vector<uint8_t> constructOpusHeader() {
        vector<uint8_t> returnVector;

        returnVector.push_back(*(__int8*)(void*)&"O");
        returnVector.push_back(*(__int8*)(void*)&"p");
        returnVector.push_back(*(__int8*)(void*)&"u");
        returnVector.push_back(*(__int8*)(void*)&"s");
        returnVector.push_back(*(__int8*)(void*)&"H");
        returnVector.push_back(*(__int8*)(void*)&"e");
        returnVector.push_back(*(__int8*)(void*)&"a");
        returnVector.push_back(*(__int8*)(void*)&"d");
        int valueOne = 1;
        returnVector.push_back(*(__int8*)(void*)&valueOne);
        valueOne = 2;
        returnVector.push_back(*(__int8*)(void*)&valueOne);
        valueOne = 0;
        returnVector.push_back(*(__int8*)(void*)&valueOne);
        valueOne = 48000;
        returnVector.push_back(*(__int8*)(void*)&valueOne >> 24);
        returnVector.push_back(*(__int8*)(void*)&valueOne >> 16);
        returnVector.push_back(*(__int8*)(void*)&valueOne >> 8);
        returnVector.push_back(*(__int8*)(void*)&valueOne);
        valueOne = 0;
        returnVector.push_back(*(__int8*)(void*)&valueOne >> 8);
        returnVector.push_back(*(__int8*)(void*)&valueOne);
        valueOne = 0;
        returnVector.push_back(*(__int8*)(void*)&"O");
        returnVector.push_back(*(__int8*)(void*)&"p");
        returnVector.push_back(*(__int8*)(void*)&"u");
        returnVector.push_back(*(__int8*)(void*)&"s");
        returnVector.push_back(*(__int8*)(void*)&"T");
        returnVector.push_back(*(__int8*)(void*)&"a");
        returnVector.push_back(*(__int8*)(void*)&"g");
        returnVector.push_back(*(__int8*)(void*)&"s");
        valueOne = 7;
        returnVector.push_back(*(__int8*)(void*)&valueOne >> 24);
        returnVector.push_back(*(__int8*)(void*)&valueOne >> 16);
        returnVector.push_back(*(__int8*)(void*)&valueOne >> 8);
        returnVector.push_back(*(__int8*)(void*)&valueOne);
        returnVector.push_back(*(__int8*)(void*)&"R");
        returnVector.push_back(*(__int8*)(void*)&"T");
        returnVector.push_back(*(__int8*)(void*)&"C");
        returnVector.push_back(*(__int8*)(void*)&"h");
        returnVector.push_back(*(__int8*)(void*)&"r");
        returnVector.push_back(*(__int8*)(void*)&"i");
        returnVector.push_back(*(__int8*)(void*)&"s");
        valueOne = 0;
        returnVector.push_back(*(__int8*)(void*)&valueOne >> 24);
        returnVector.push_back(*(__int8*)(void*)&valueOne >> 16);
        returnVector.push_back(*(__int8*)(void*)&valueOne >> 8);
        returnVector.push_back(*(__int8*)(void*)&valueOne);
        valueOne = 0;
        returnVector.push_back(*(__int8*)(void*)&valueOne >> 24);
        returnVector.push_back(*(__int8*)(void*)&valueOne >> 16);
        returnVector.push_back(*(__int8*)(void*)&valueOne >> 8);
        returnVector.push_back(*(__int8*)(void*)&valueOne);
        for (auto value : returnVector) {
            cout << "CURRENT VALUE: " << value << endl;
        }
        return returnVector;
    }

    IBuffer loadFile(hstring filePath, hstring fileName) {
        auto folder = winrt::Windows::Storage::StorageFolder::GetFolderFromPathAsync(filePath).get();
        winrt::Windows::Storage::StorageFile storageFile = folder.GetFileAsync(fileName).get();
        auto returnBuffer = winrt::Windows::Storage::FileIO::ReadBufferAsync(storageFile).get();
        return returnBuffer;
    }

    void saveFile(hstring filePath, hstring fileName, IBuffer readBuffer) {
        auto folder = winrt::Windows::Storage::StorageFolder::GetFolderFromPathAsync(filePath).get();
        winrt::Windows::Storage::StorageFile storageFile = folder.CreateFileAsync(fileName, CreationCollisionOption::ReplaceExisting).get();
        winrt::Windows::Storage::FileIO::WriteBufferAsync(storageFile, readBuffer).get();
    }

    vector<uint8_t> applyAudioData(WebAFile webaFile, IBuffer originalHeaderData) {
        vector<uint8_t> returnVector;
        for (unsigned int x = 0; x < 262; x += 1) {
            returnVector.push_back(originalHeaderData.data()[x]);
        }
        for (auto value : webaFile.segment.clusters) {
            for (auto value2 : value.simpleBlocks) {
                for (auto value3 : value2.frame) {
                    returnVector.push_back(value3);
                }
            }
        }
        return returnVector;
    }

    vector<uint8_t> decodeOpusData(WebAFile inputDecodingData, string filePath, string fileName) {
        vector<uint8_t> returnVector;

        return returnVector;

    }

    static int output_audio_frame(AVFrame* frame, AVFrame* newFrame, int* audioFrameCount, AVCodecContext* audio_dec_ctx, FILE* audio_dst_file, SwrContext* swrContext)
    {
        size_t unpadded_linesize = frame->nb_samples * av_get_bytes_per_sample((AVSampleFormat)frame->format);
        printf("audio_frame n:%d nb_samples:%d pts:%s\n", audioFrameCount++, frame->nb_samples, to_string(frame->pts), &audio_dec_ctx->time_base);
        newFrame->channel_layout = AV_CH_LAYOUT_STEREO;
        newFrame->sample_rate = 48000;
        newFrame->format = AVSampleFormat::AV_SAMPLE_FMT_FLT;
        swr_convert_frame(swrContext, newFrame, frame);
        
        cout << "CHANNEL LAYOUT: " << newFrame->channel_layout << " SAMPLE RATE: " << newFrame->sample_rate << " FORMAT: " << newFrame->format << endl;
        /* Write the raw audio data samples of the first plane. This works
         * fine for packed formats (e.g. AV_SAMPLE_FMT_S16). However,
         * most audio decoders output planar audio, which uses a separate
         * plane of audio samples for each channel (e.g. AV_SAMPLE_FMT_S16P).
         * In other words, this code will write only the first audio channel
         * in these cases.
         * You should use libswresample or libavfilter to convert the frame
         * to packed data. 
         * */
        fwrite(newFrame->data[0], 1, unpadded_linesize, audio_dst_file);
        
        return 0;
    }

    static int decode_packet(AVCodecContext* dec, const AVPacket* pkt, AVFrame* frame,AVFrame* newFrame,  int* frameCount, FILE* fileOutput, SwrContext* swrContext)
    {
        int ret = 0;
        // submit the packet to the decoder
        ret = avcodec_send_packet(dec, pkt);
        if (ret < 0) {
            fprintf(stderr, "Error submitting a packet for decoding (%s)\n", to_string(ret));
            return ret;
        }

        // get all the available frames from the decoder
        while (ret >= 0) {
            ret = avcodec_receive_frame(dec, frame);
            if (ret < 0) {
                // those two return values are special and mean there is no output
                // frame available, but there were no errors during decoding
                if (ret == AVERROR_EOF || ret == AVERROR(EAGAIN))
                    return 0;

                fprintf(stderr, "Error during decoding (%s)\n", to_string(ret));
                return ret;
            }

            // write the frame data to output file;
            ret = output_audio_frame(frame, newFrame, frameCount, dec, fileOutput, swrContext);

            av_frame_unref(frame);
            if (ret < 0)
                return ret;
        }

        return 0;
    }

    static int open_codec_context(int* stream_idx,
        AVCodecContext** dec_ctx, AVFormatContext* fmt_ctx, enum AVMediaType type, string src_filename)
    {
        int ret, stream_index;
        AVStream* st;
        const AVCodec* dec = NULL;

        ret = av_find_best_stream(fmt_ctx, type, -1, -1, NULL, 0);
        if (ret < 0) {
            fprintf(stderr, "Could not find %s stream in input file '%s'\n",
                av_get_media_type_string(type), src_filename);
            return ret;
        }
        else {
            stream_index = ret;
            st = fmt_ctx->streams[stream_index];

            /* find decoder for the stream */
            dec = avcodec_find_decoder(st->codecpar->codec_id);
            if (!dec) {
                fprintf(stderr, "Failed to find %s codec\n",
                    av_get_media_type_string(type));
                return AVERROR(EINVAL);
            }

            /* Allocate a codec context for the decoder */
            *dec_ctx = avcodec_alloc_context3(dec);
            if (!*dec_ctx) {
                fprintf(stderr, "Failed to allocate the %s codec context\n",
                    av_get_media_type_string(type));
                return AVERROR(ENOMEM);
            }

            /* Copy codec parameters from input stream to output codec context */
            if ((ret = avcodec_parameters_to_context(*dec_ctx, st->codecpar)) < 0) {
                fprintf(stderr, "Failed to copy %s codec parameters to decoder context\n",
                    av_get_media_type_string(type));
                return ret;
            }

            /* Init the decoders */
            if ((ret = avcodec_open2(*dec_ctx, dec, NULL)) < 0) {
                fprintf(stderr, "Failed to open %s codec\n",
                    av_get_media_type_string(type));
                return ret;
            }
            *stream_idx = stream_index;
        }

        return 0;
    }

    static int get_format_from_sample_fmt(const char** fmt,
        enum AVSampleFormat sample_fmt)
    {
        int i;
        struct sample_fmt_entry {
            enum AVSampleFormat sample_fmt; const char* fmt_be, * fmt_le;
        } sample_fmt_entries[] = {
            { AV_SAMPLE_FMT_U8,  "u8",    "u8"    },
            { AV_SAMPLE_FMT_S16, "s16be", "s16le" },
            { AV_SAMPLE_FMT_S32, "s32be", "s32le" },
            { AV_SAMPLE_FMT_FLT, "f32be", "f32le" },
            { AV_SAMPLE_FMT_DBL, "f64be", "f64le" },
            { AV_SAMPLE_FMT_U8P,  "u8",    "u8"    },
            { AV_SAMPLE_FMT_S16P, "s16be", "s16le" },
            { AV_SAMPLE_FMT_S32P, "s32be", "s32le" },
            { AV_SAMPLE_FMT_FLTP, "f32be", "f32le" },
            { AV_SAMPLE_FMT_DBLP, "f64be", "f64le" },

        };
        *fmt = NULL;

        for (i = 0; i < FF_ARRAY_ELEMS(sample_fmt_entries); i++) {
            struct sample_fmt_entry* entry = &sample_fmt_entries[i];
            if (sample_fmt == entry->sample_fmt) {
                *fmt = AV_NE(entry->fmt_be, entry->fmt_le);
                cout << "WERE HERE WERE HERE WERE HERE" << endl;
                return 0;
            }
        }

        fprintf(stderr,
            "sample format %s is not supported as output format\n",
            av_get_sample_fmt_name(sample_fmt));
        return -1;
    }

    int decodeAudioData(string filePath, string fileName)
    {
        static AVFormatContext* fmt_ctx = NULL;
        static AVCodecContext* audio_dec_ctx;
        static AVStream* audio_stream = NULL;
        string audio_dst_filename, src_filename;
        static FILE* audio_dst_file = NULL;


        static int  audio_stream_idx = -1;
        static AVFrame* frame = NULL;
        static AVPacket* pkt = NULL;
        static int audio_frame_count = 0;

        int ret = 0;

        if (filePath.size() <= 4) {
            fprintf(stderr, "usage: %s  input_file video_output_file audio_output_file\n"
                "API example program to show how to read frames from an input file.\n"
                "This program reads frames from a file, decodes them, and writes decoded\n"
                "video frames to a rawvideo file named video_output_file, and decoded\n"
                "audio frames to a rawaudio file named audio_output_file.\n",
                filePath);
            exit(1);
        }
        src_filename = filePath + fileName;
        string fileNameNew = fileName.substr(0, fileName.find(".m4a")) + "-decoded.wav";
        audio_dst_filename = (filePath + fileNameNew).c_str();

        /* open input file, and allocate format context */
        if (avformat_open_input(&fmt_ctx, src_filename.c_str(), NULL, NULL) < 0) {
            fprintf(stderr, "Could not open source file %s\n", src_filename);
            exit(1);
        }

        /* retrieve stream information */
        if (avformat_find_stream_info(fmt_ctx, NULL) < 0) {
            fprintf(stderr, "Could not find stream information\n");
            exit(1);
        }

        if (open_codec_context(&audio_stream_idx, &audio_dec_ctx, fmt_ctx, AVMEDIA_TYPE_AUDIO, src_filename) >= 0) {
            audio_stream = fmt_ctx->streams[audio_stream_idx];
            audio_dst_file = fopen(audio_dst_filename.c_str(), "wb");
            if (!audio_dst_file) {
                fprintf(stderr, "Could not open destination file %s\n", audio_dst_filename);
                ret = 1;
            }
        }

        /* dump input information to stderr */
        av_dump_format(fmt_ctx, 0, src_filename.c_str(), 0);

        if (!audio_stream ) {
            fprintf(stderr, "Could not find audio or video stream in the input, aborting\n");
            ret = 1;
        }

        frame = av_frame_alloc();
        if (!frame) {
            fprintf(stderr, "Could not allocate frame\n");
            ret = AVERROR(ENOMEM);
        }

        pkt = av_packet_alloc();
        if (!pkt) {
            fprintf(stderr, "Could not allocate packet\n");
            ret = AVERROR(ENOMEM);;
        }

        SwrContext* swrContext = swr_alloc();
        swrContext = swr_alloc_set_opts(NULL, AV_CH_LAYOUT_STEREO, AVSampleFormat::AV_SAMPLE_FMT_FLT, 48000, AV_CH_LAYOUT_STEREO, audio_dec_ctx->sample_fmt, audio_dec_ctx->sample_rate, 0, nullptr);
        swr_init(swrContext);
        if (audio_stream)
            printf("Demuxing audio from file '%s' into '%s'\n", src_filename, audio_dst_filename);
        int frameCount;
        AVFrame* newFrame = av_frame_alloc();
        /* read frames from the file */
        while (av_read_frame(fmt_ctx, pkt) >= 0) {
            // check if the packet belongs to a stream we are interested in, otherwise
            // skip it
            
            if (pkt->stream_index == audio_stream_idx)
                ret = decode_packet(audio_dec_ctx, pkt, frame, newFrame, &frameCount, audio_dst_file, swrContext);
            av_packet_unref(pkt);
            if (ret < 0)
                break;
        }

        /* flush the decoders */
        if (audio_dec_ctx)
            ret = decode_packet(audio_dec_ctx, NULL, frame, newFrame, &frameCount, audio_dst_file, swrContext);

        int remainingSamples = swr_get_delay(swrContext, audio_dec_ctx->sample_rate);
        cout << "REMAINING SAMPLES: " << remainingSamples << endl;


        printf("Demuxing succeeded.\n");

        if (audio_stream) {
            enum AVSampleFormat sfmt = (AVSampleFormat)newFrame->format;
            int n_channels = newFrame->channel_layout;
            const char* fmt;

            if (av_sample_fmt_is_planar((AVSampleFormat)newFrame->format)) {
                const char* packed = av_get_sample_fmt_name(sfmt);
                printf("Warning: the sample format the decoder produced is planar "
                    "(%s). This example will output the first channel only.\n",
                    packed ? packed : "?");
                sfmt = av_get_packed_sample_fmt(sfmt);
                n_channels = 1;
            }

            if ((ret = get_format_from_sample_fmt(&fmt, (AVSampleFormat)newFrame->format)) < 0)
                goto end;

            printf("Play the output audio file with the command:\n"
                "ffplay -f %s -ac %d -ar %d %s\n",
                newFrame->format, n_channels, newFrame->sample_rate,
                audio_dst_filename);
        }

    end:
        av_frame_free(&newFrame);
        av_frame_free(&frame);
        avcodec_close(audio_dec_ctx);
        avcodec_free_context(&audio_dec_ctx);
        avformat_close_input(&fmt_ctx);
        if (audio_dst_file)
            fclose(audio_dst_file);
        av_packet_free(&pkt);
        
        

        return 0;
    }
    
/*
static int open_input_file(const char* filename,
    AVFormatContext** input_format_context,
    AVCodecContext** input_codec_context)
{
    AVCodecContext* avctx;
    const AVCodec* input_codec;
    int error;

    /* Open the input file to read from it. 
    if ((error = avformat_open_input(input_format_context, filename, NULL,
        NULL)) < 0) {
        fprintf(stderr, "Could not open input file '%s' (error '%s')\n",
            filename, to_string(error));
        *input_format_context = NULL;
        return error;
    }

    /* Get information on the input file (number of streams etc.). 
    if ((error = avformat_find_stream_info(*input_format_context, NULL)) < 0) {
        fprintf(stderr, "Could not open find stream info (error '%s')\n",
            to_string(error));
        avformat_close_input(input_format_context);
        return error;
    }

    /* Make sure that there is only one stream in the input file. 
    if ((*input_format_context)->nb_streams != 1) {
        fprintf(stderr, "Expected one audio input stream, but found %d\n",
            (*input_format_context)->nb_streams);
        avformat_close_input(input_format_context);
        return AVERROR_EXIT;
    }

    /* Find a decoder for the audio stream. 
    if (!(input_codec = avcodec_find_decoder((*input_format_context)->streams[0]->codecpar->codec_id))) {
        fprintf(stderr, "Could not find input codec\n");
        avformat_close_input(input_format_context);
        return AVERROR_EXIT;
    }

    /* Allocate a new decoding context. 
    avctx = avcodec_alloc_context3(input_codec);
    if (!avctx) {
        fprintf(stderr, "Could not allocate a decoding context\n");
        avformat_close_input(input_format_context);
        return AVERROR(ENOMEM);
    }

    /* Initialize the stream parameters with demuxer information. 
    error = avcodec_parameters_to_context(avctx, (*input_format_context)->streams[0]->codecpar);
    if (error < 0) {
        avformat_close_input(input_format_context);
        avcodec_free_context(&avctx);
        return error;
    }

    /* Open the decoder for the audio stream to use it later. 
    if ((error = avcodec_open2(avctx, input_codec, NULL)) < 0) {
        fprintf(stderr, "Could not open input codec (error '%s')\n",
            to_string(error));
        avcodec_free_context(&avctx);
        avformat_close_input(input_format_context);
        return error;
    }

    /* Save the decoder context for easier access later. 
    *input_codec_context = avctx;

    return 0;
}


static int decode_packet(int* got_frame, int cached, AVPacket pkt, int audio_stream_idx, AVFrame* frame, AVCodecContext* audio_dec_ctx, int* audio_frame_count, FILE* audio_dst_file)
{
    int ret = 0;
    int decoded = pkt.size;
    *got_frame = 0;
    if (pkt.stream_index == audio_stream_idx) {
        /* decode audio frame 
        ret = avcodec_decode_audio4(audio_dec_ctx, frame, got_frame, &pkt);
        if (ret < 0) {
            fprintf(stderr, "Error decoding audio frame (%s)\n", to_string(ret));
            return ret;
        }
        /* Some audio decoders decode only part of the packet, and have to be
         * called again with the remainder of the packet data.
         * Sample: fate-suite/lossless-audio/luckynight-partial.shn
         * Also, some decoders might over-read the packet. 
        decoded = FFMIN(ret, pkt.size);
        if (*got_frame) {
            size_t unpadded_linesize = frame->nb_samples * av_get_bytes_per_sample((AVSampleFormat)frame->format);
            printf("audio_frame%s n:%d nb_samples:%d pts:%s\n",
                cached ? "(cached)" : "",
                *audio_frame_count++, frame->nb_samples,
                to_string(frame->pts), &audio_dec_ctx->time_base);
            /* Write the raw audio data samples of the first plane. This works
             * fine for packed formats (e.g. AV_SAMPLE_FMT_S16). However,
             * most audio decoders output planar audio, which uses a separate
             * plane of audio samples for each channel (e.g. AV_SAMPLE_FMT_S16P).
             * In other words, this code will write only the first audio channel
             * in these cases.
             * You should use libswresample or libavfilter to convert the frame
             * to packed data. 
            fwrite(frame->extended_data[0], 1, unpadded_linesize, audio_dst_file);
        }
    }
    /* If we use the new API with reference counting, we own the data and need
     * to de-reference it when we don't use it anymore 
        av_frame_unref(frame);
    return decoded;
}

    static int get_format_from_sample_fmt(const char** fmt,
        enum AVSampleFormat sample_fmt)
    {
        int i;
        struct sample_fmt_entry {
            enum AVSampleFormat sample_fmt; const char* fmt_be, * fmt_le;
        } sample_fmt_entries[] = {
            { AV_SAMPLE_FMT_U8,  "u8",    "u8"    },
            { AV_SAMPLE_FMT_S16, "s16be", "s16le" },
            { AV_SAMPLE_FMT_S32, "s32be", "s32le" },
            { AV_SAMPLE_FMT_FLT, "f32be", "f32le" },
            { AV_SAMPLE_FMT_DBL, "f64be", "f64le" },
        };
        *fmt = NULL;
        for (i = 0; i < FF_ARRAY_ELEMS(sample_fmt_entries); i++) {
            struct sample_fmt_entry* entry = &sample_fmt_entries[i];
            if (sample_fmt == entry->sample_fmt) {
                *fmt = AV_NE(entry->fmt_be, entry->fmt_le);
                return 0;
            }
        }
        fprintf(stderr,
            "Sample format %s not supported as output format\n",
            av_get_sample_fmt_name(sample_fmt));
        return AVERROR(EINVAL);
    }
    
    int decodeAudioData(string filePath, string fileName)
    {
        int64_t src_ch_layout = AV_CH_LAYOUT_STEREO, dst_ch_layout = AV_CH_LAYOUT_STEREO;
        int src_rate = 44100, dst_rate = 48000;
        uint8_t** src_data = NULL, ** dst_data = NULL;
        int  dst_nb_channels = 0;
        int dst_linesize;
        int src_nb_samples = 1024, dst_nb_samples, max_dst_nb_samples;
        enum AVSampleFormat src_sample_fmt = AV_SAMPLE_FMT_FLT, dst_sample_fmt = AV_SAMPLE_FMT_FLT;
        const char* dst_filename = NULL, * src_fileName;
        FILE* srcFile, * audio_dst_file;
        int dst_bufsize;
        const char* fmt;
        struct SwrContext* swr_ctx;
        double t;
        int ret;

        static AVFormatContext* fmt_ctx = NULL;
        static AVCodecContext* audio_dec_ctx;
        static AVStream* audio_stream = NULL;
        static int  audio_stream_idx = -1;
        static AVFrame* frame = av_frame_alloc();
        static AVPacket pkt;
        static int audio_frame_count = 0;
        if (filePath.size() <=2) {
            fprintf(stderr, "Usage: %s output_file\n"
                "API example program to show how to resample an audio stream with libswresample.\n"
                "This program generates a series of audio frames, resamples them to a specified "
                "output format and rate and saves them to an output file named output_file.\n",
                filePath);
            exit(1);
        }
        src_fileName = (filePath + fileName).c_str();
        open_input_file(src_fileName, &fmt_ctx, &audio_dec_ctx);
        
        string newFileName = fileName.substr(0, fileName.find(".m4a")) + "-decoded.wav";
        dst_filename = (filePath + newFileName).c_str();
        audio_dst_file = fopen(dst_filename, "wb");
        if (!audio_dst_file) {
            fprintf(stderr, "Could not open destination file %s\n", dst_filename);
            exit(1);
        }
        srcFile = fopen(src_fileName, "wb");
        if (!srcFile) {
            fprintf(stderr, "Could not open source file %s\n", src_fileName);
            exit(1);
        }
        /* create resampler context *
        swr_ctx = swr_alloc();
        if (!swr_ctx) {
            fprintf(stderr, "Could not allocate resampler context\n");
            ret = AVERROR(ENOMEM);
            goto end;
        }
        /* set options *
        av_opt_set_int(swr_ctx, "in_channel_layout", src_ch_layout, 0);
        av_opt_set_int(swr_ctx, "in_sample_rate", src_rate, 0);
        av_opt_set_sample_fmt(swr_ctx, "in_sample_fmt", src_sample_fmt, 0);
        av_opt_set_int(swr_ctx, "out_channel_layout", dst_ch_layout, 0);
        av_opt_set_int(swr_ctx, "out_sample_rate", dst_rate, 0);
        av_opt_set_sample_fmt(swr_ctx, "out_sample_fmt", dst_sample_fmt, 0);
        /* initialize the resampling context *
        if ((ret = swr_init(swr_ctx)) < 0) {
            fprintf(stderr, "Failed to initialize the resampling context\n");
            goto end;
        }
        /* allocate source and destination samples buffers */
        /* compute the number of converted samples: buffering is avoided
         * ensuring that the output buffer will contain at least all the
         * converted input samples *
        max_dst_nb_samples = dst_nb_samples =
            av_rescale_rnd(src_nb_samples, dst_rate, src_rate, AV_ROUND_UP);
        /* buffer is going to be directly written to a rawaudio file, no alignment *
        dst_nb_channels = av_get_channel_layout_nb_channels(dst_ch_layout);
        ret = av_samples_alloc_array_and_samples(&dst_data, &dst_linesize, dst_nb_channels,
            dst_nb_samples, dst_sample_fmt, 0);
        if (ret < 0) {
            fprintf(stderr, "Could not allocate destination samples\n");
            goto end;
        }
        av_init_packet(&pkt);
        pkt.data = NULL;
        pkt.size = 0;
        t = 0;
        do {
            int gotFrame = 0;
            int cached = 0;
            int audioStreamIdx = 0;
            int frameCount = 0;
            av_read_frame(fmt_ctx, &pkt);

            ret = decode_packet(&gotFrame, cached, pkt, audioStreamIdx, frame, audio_dec_ctx, &frameCount, audio_dst_file);
            /* compute destination number of samples *
            dst_nb_samples = av_rescale_rnd(swr_get_delay(swr_ctx, src_rate) +
                src_nb_samples, dst_rate, src_rate, AV_ROUND_UP);
            if (dst_nb_samples > max_dst_nb_samples) {
                av_freep(&dst_data[0]);
                ret = av_samples_alloc(dst_data, &dst_linesize, dst_nb_channels,
                    dst_nb_samples, dst_sample_fmt, 1);
                if (ret < 0)
                    break;
                max_dst_nb_samples = dst_nb_samples;
            }
            /* convert to destination format *
            ret = swr_convert(swr_ctx, dst_data, dst_nb_samples, (const uint8_t**)&frame->data, src_nb_samples);
            if (ret < 0) {
                fprintf(stderr, "Error while converting\n");
                goto end;
            }
            dst_bufsize = av_samples_get_buffer_size(&dst_linesize, dst_nb_channels,
                ret, dst_sample_fmt, 1);
            if (dst_bufsize < 0) {
                fprintf(stderr, "Could not get sample buffer size\n");
                goto end;
            }
            printf("t:%f in:%d out:%d\n", t, src_nb_samples, ret);
            fwrite(dst_data[0], 1, dst_bufsize, audio_dst_file);
        } while (pkt.size > 0);
        if ((ret = get_format_from_sample_fmt(&fmt, dst_sample_fmt)) < 0)
            goto end;
        fprintf(stderr, "Resampling succeeded. Play the output file with the command:\nffplay -f %s -channel_layout %PRId64 -channels %d -ar %d %s\n");
    end:
        fclose(audio_dst_file);
        if (src_data)
            av_freep(&src_data[0]);
        av_freep(&src_data);
        if (dst_data)
            av_freep(&dst_data[0]);
        av_freep(&dst_data);
        swr_free(&swr_ctx);
        return ret < 0;
    }
    */
}

#endif
