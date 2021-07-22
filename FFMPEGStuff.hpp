// FFMPegStuff.hpp - Header for the FFMPeg stuff
// Jul 19, 2021
// Chris M.
// https://github.com/RealTimeChris

#pragma once

#ifndef _FFMPEG_STUFF_
#define _FFMPEG_STUFF_

#include "../pch.h"

namespace DiscordCoreAPI {
/*
    static int openCodecContext(int* streamIndex,
        AVCodecContext** codecContext, AVFormatContext* formatContext, enum AVMediaType type, string sourceFilename)
    {
        int ret, streamIndexValue;
        AVStream* stream;
        const AVCodec* codec = NULL;

        ret = av_find_best_stream(formatContext, type, -1, -1, NULL, 0);
        if (ret < 0) {
            cout << "Could not find %s stream in input file '%s'\n" << endl;
            return ret;
        }
        else {
            streamIndexValue = ret;
            stream = formatContext->streams[streamIndexValue];

            // find decoder for the stream 
            codec = avcodec_find_decoder(stream->codecpar->codec_id);
            if (!codec) {
                cout << "Failed to find %s codec\n" << endl;
                return AVERROR(EINVAL);
            }

            // Allocate a codec context for the decoder 
            *codecContext = avcodec_alloc_context3(codec);
            if (!*codecContext) {
                cout << "Failed to allocate the %s codec context\n" << endl;
                return AVERROR(ENOMEM);
            }

            // Copy codec parameters from input stream to output codec context 
            if ((ret = avcodec_parameters_to_context(*codecContext, stream->codecpar)) < 0) {
                cout << "Failed to copy %s codec parameters to decoder context\n" << endl;
                return ret;
            }

            // Init the decoders 
            if ((ret = avcodec_open2(*codecContext, codec, NULL)) < 0) {
                cout << "Failed to open %s codec\n" << endl;
                return ret;
            }
            *streamIndex = streamIndexValue;
        }

        return 0;
    }

    static int output_audio_frame(AVFrame* frame, InMemoryRandomAccessStream* randomAccessStream, int* offsetIntoStream, int* audioFrameIndex) {
        size_t unpadded_linesize = frame->nb_samples * av_get_bytes_per_sample((AVSampleFormat)frame->format);
        cout << "audio_frame " + to_string(frame->nb_samples) + " samples , @ " + to_string(av_get_bytes_per_sample((AVSampleFormat)frame->format)) + " bytes per sample." << endl;
        (*audioFrameIndex)++;

        // Write the raw audio data samples of the first plane. This works
         //* fine for packed formats (e.g. AV_SAMPLE_FMT_S16). However,
         //* most audio decoders output planar audio, which uses a separate
         //* plane of audio samples for each channel (e.g. AV_SAMPLE_FMT_S16P).
         //* In other words, this code will write only the first audio channel
         //* in these cases.
         //* You should use libswresample or libavfilter to convert the frame
         //* to packed data. 

        DataWriter dataWriter(randomAccessStream->GetOutputStreamAt(*offsetIntoStream));
        dataWriter.UnicodeEncoding(UnicodeEncoding::Utf8);
        for (int x = 0; x < frame->linesize[0] * 2; x += 1) {
            dataWriter.WriteByte(frame->extended_data[x % 2][x / 2]);
        }
        dataWriter.StoreAsync().get();
        *offsetIntoStream = (int)unpadded_linesize;
        return 0;
    }

    int decodePacket(AVCodecContext* codecContext, const AVPacket* packet, AVFrame* frame,  InMemoryRandomAccessStream* randomAccessStream, int* offsetIntoStream, int* audioFrameIndex) {
        cout << "WERE HERE WERE HERE" << endl;
        int ret = 0;

        // submit the packet to the decoder
        ret = avcodec_send_packet(codecContext, packet);
        if (ret < 0) {
            cout << "Error submitting a packet for decoding (%s)\n" << endl;
            return ret;
        }

        // get all the available frames from the decoder
        while (ret >= 0) {
            ret = avcodec_receive_frame(codecContext, frame);
            if (ret < 0) {
                // those two return values are special and mean there is no output
                // frame available, but there were no errors during decoding
                if (ret == AVERROR_EOF || ret == AVERROR(EAGAIN))
                    return 0;

                cout << "Error during decoding (%s)\n" << endl;
                return ret;
            }

            // write the frame data to output file
            if (codecContext->codec->type != AVMEDIA_TYPE_VIDEO) {
                ret = output_audio_frame(frame, randomAccessStream, offsetIntoStream, audioFrameIndex);
            }

            av_frame_unref(frame);
            if (ret < 0)
            {
                return ret;
            }
        }

        return 0;
    }

    int get_format_from_sample_fmt(const char** fmt,
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
            "sample format %s is not supported as output format\n",
            av_get_sample_fmt_name(sample_fmt));
        return -1;
    }

    IBuffer demux(string downloadURL) {
        AVFormatContext* formatContext = avformat_alloc_context();
        AVCodecContext* audioCodecContext = nullptr;
        AVStream* audioStream = nullptr;
        int audioStreamIndex = -1, ret = 0, offsetIntoStream = 0;
        AVFrame* frame = nullptr;
        AVPacket* packet = nullptr;
        IBuffer outputBuffer = nullptr;
        InMemoryRandomAccessStream randomAccessStream = nullptr;

        AVInputFormat inputFormat;
        inputFormat
        int error = avformat_open_input(&formatContext, "C:\\Users\\Chris\\source\\repos\\MBot-MusicHouse-Cpp\\x64\\Release\\RICH BRIAN x ZHU. x SKRILLEX x THEY.  - Working For It (Official Audio) 1626736791099322300.weba", NULL, NULL);
		if (error < 0) {
            const size_t charCount = 128;
            char* charString = new char[charCount];
            av_make_error_string(charString, charCount, error);
            fprintf(stderr, "Could not open source file %s\n", downloadURL.c_str());
            cout << charString << endl;
            return IBuffer();
		}

		if (avformat_find_stream_info(formatContext, NULL) < 0) {
            fprintf(stderr, "Could not find stream information\n");
            return IBuffer();
		}

        if (openCodecContext(&audioStreamIndex, &audioCodecContext, formatContext, AVMEDIA_TYPE_AUDIO, downloadURL) >= 0) {
            audioStream = formatContext->streams[audioStreamIndex];
        }

        av_dump_format(formatContext, 0, downloadURL.c_str(), 0);

        if (!audioStream) {
            cout << "Could not find audio or video stream in the input, aborting\n" << endl;
            return IBuffer();
        }

        frame = av_frame_alloc();
        if (!frame) {
            cout << "Could not allocate frame\n" << endl;
            return IBuffer();
        }

        packet = av_packet_alloc();
        if (!packet) {
            cout << "Could not allocate packet\n" << endl;
            return IBuffer();
        }

        if (audioStream) {
            cout << "Demuxing audio from file '%s' into '%s'\n" << endl;
        }
        
        while (av_read_frame(formatContext, packet) >= 0) {
            if (packet->stream_index == audioStreamIndex) {
                ret = decodePacket(audioCodecContext, packet, frame, &randomAccessStream, &offsetIntoStream, &audioStreamIndex);
            }
            av_packet_unref(packet);
            if (ret < 0) {
                cout << "WERE BREAKING WERE BREAKING!" << endl;
                break;
            }
        }

        DataReader dataReader(randomAccessStream.GetInputStreamAt(0));
        dataReader.LoadAsync((uint32_t)randomAccessStream.Size());
        outputBuffer = dataReader.ReadBuffer((uint32_t)randomAccessStream.Size());
        cout << "FINAL BUFFER LENGTH: " << outputBuffer.Length() << endl;
        if (audioCodecContext) {
            decodePacket(audioCodecContext, NULL, nullptr, nullptr, nullptr, nullptr);
        }

        cout << "Demuxing succeeded.\n" << endl;

        if (audioStream) {
            enum AVSampleFormat sfmt = audioCodecContext->sample_fmt;
            int n_channels = audioCodecContext->channels;
            const char* fmt;

            if (av_sample_fmt_is_planar(sfmt)) {
                const char* packed = av_get_sample_fmt_name(sfmt);
                printf("Warning: the sample format the decoder produced is planar "
                    "(%s). This example will output the first channel only.\n",
                    packed ? packed : "?");
                sfmt = av_get_packed_sample_fmt(sfmt);
                n_channels = 1;
            }

            if ((ret = get_format_from_sample_fmt(&fmt, sfmt)) < 0)
                goto end;

            printf("Play the output audio file with the command:\n"
                "ffplay -f %s -ac %d -ar %d\n",
                fmt, n_channels, audioCodecContext->sample_rate);
        }
    end:
        avcodec_free_context(&audioCodecContext);
        avformat_close_input(&formatContext);
        av_packet_free(&packet);
        av_frame_free(&frame);

        return outputBuffer;
    }
    */

}

#endif