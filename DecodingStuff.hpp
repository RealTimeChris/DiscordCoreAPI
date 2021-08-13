// DecodingStuff.hpp - Header for the "decoding stuff".
// Jul 29, 2021
// Chris M.
// https://github.com/RealTimeChris

#pragma once

#ifndef _DECODING_STUFF_
#define _DECODING_STUFF_

#include "../pch.h"
#include "DemuxingStuff.hpp"

namespace DiscordCoreAPI {

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

    class SongDecoder {
    public:
        SongDecoder(string filePathNew, string fileNameNew) {
            this->filePath = filePathNew;
            this->fileName = fileNameNew;
        }

        vector<uint8_t> decodeSong() {
            vector<string> filePathAndName = this->decodeAudioData(this->filePath, this->fileName);
            IBuffer loadedBuffer = loadFile(to_hstring(filePathAndName.at(0)), to_hstring(filePathAndName.at(1)));
            vector<uint8_t> returnVector;
            for (unsigned int x = 0; x < loadedBuffer.Length(); x += 1) {
                returnVector.push_back(loadedBuffer.data()[x]);
            }
            return returnVector;
        }

    protected:
        string filePath, fileName;
        int outputAudioFrame(AVFrame* frame, AVFrame* newFrame, int* audioFrameCount, FILE* outputFile, SwrContext* swrContext)
        {
            (*audioFrameCount)++;
            if (!swr_is_initialized(swrContext)) {
                swr_init(swrContext);
            }
            newFrame->channel_layout = AV_CH_LAYOUT_STEREO;
            newFrame->sample_rate = 48000;
            newFrame->format = AVSampleFormat::AV_SAMPLE_FMT_FLT;
            newFrame->nb_samples = frame->nb_samples;
            newFrame->pts = frame->pts;
            swr_convert_frame(swrContext, newFrame, frame);
            printf("audio_frame n:%d nb_samples:%d pts:%s\n", *audioFrameCount, newFrame->nb_samples, to_string(newFrame->pts).c_str());
            size_t unpadded_linesize = newFrame->nb_samples * av_get_bytes_per_sample((AVSampleFormat)newFrame->format) * 2;

            fwrite(newFrame->extended_data[0], 1, unpadded_linesize, outputFile);
            __int64 sampleCount = swr_get_delay(swrContext, newFrame->sample_rate);
            if (sampleCount > 0) {
                if (!swr_is_initialized(swrContext)) {
                    swr_init(swrContext);
                }
                swr_convert_frame(swrContext, newFrame, nullptr);
                fwrite(newFrame->extended_data[0], 1, (size_t)newFrame->linesize, outputFile);
                cout << "SAMPLE COUNT: " << sampleCount << endl;
            }

            return 0;
        }

        int decodePacket(AVCodecContext* dec, const AVPacket* packet, AVFrame* frame, AVFrame* newFrame, int* frameCount, FILE* fileOutput, SwrContext* swrContext)
        {
            int ret = 0;
            // submit the packet to the decoder
            ret = avcodec_send_packet(dec, packet);
            if (ret < 0) {
                fprintf(stderr, "Error submitting a packet for decoding (%s)\n", to_string(ret).c_str());
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

                    fprintf(stderr, "Error during decoding (%s)\n", to_string(ret).c_str());
                    return ret;
                }

                // write the frame data to output file;
                ret = outputAudioFrame(frame, newFrame, frameCount, fileOutput, swrContext);

                av_frame_unref(frame);
                if (ret < 0)
                    return ret;
            }

            return 0;
        }

        int openCodecContext(int* stream_idx,
            AVCodecContext** dec_ctx, AVFormatContext* formatContext, enum AVMediaType type, string sourceFilename)
        {
            int ret, stream_index;
            AVStream* st;
            const AVCodec* dec = NULL;

            ret = av_find_best_stream(formatContext, type, -1, -1, NULL, 0);
            if (ret < 0) {
                fprintf(stderr, "Could not find %s stream in input file '%s'\n",
                    av_get_media_type_string(type), sourceFilename.c_str());
                return ret;
            }
            else {
                stream_index = ret;
                st = formatContext->streams[stream_index];

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

        int getFormatFromSampleFormat(const char** format,
            enum AVSampleFormat sampleFormat)
        {
            int i;
            struct sample_fmt_entry {
                enum AVSampleFormat sampleFormat; const char* fmt_be, * fmt_le;
            } sampleFormatEntries[] = {
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
            *format = NULL;

            for (i = 0; i < FF_ARRAY_ELEMS(sampleFormatEntries); i++) {
                struct sample_fmt_entry* entry = &sampleFormatEntries[i];
                if (sampleFormat == entry->sampleFormat) {
                    *format = AV_NE(entry->fmt_be, entry->fmt_le);
                    cout << "WERE HERE WERE HERE WERE HERE" << endl;
                    return 0;
                }
            }

            fprintf(stderr,
                "sample format %s is not supported as output format\n",
                av_get_sample_fmt_name(sampleFormat));
            return -1;
        }

        vector<string> decodeAudioData(string filePathNew, string fileNameNew)
        {
            static AVFormatContext* formatContext = NULL;
            static AVCodecContext* audioDecodeContext;
            static AVStream* audioStream = NULL;
            string outputFilename, sourceFilename;
            static FILE* outputFile = NULL;

            static int  audioStreamIndex = -1;
            static AVFrame* frame = NULL;
            static AVPacket* packet = NULL;
            static int audioFrameCount = 0;

            int ret = 0;

            if (filePath.size() <= 4) {
                fprintf(stderr, "usage: %s  input_file video_output_file audio_output_file\n"
                    "API example program to show how to read frames from an input file.\n"
                    "This program reads frames from a file, decodes them, and writes decoded\n"
                    "video frames to a rawvideo file named video_output_file, and decoded\n"
                    "audio frames to a rawaudio file named audio_output_file.\n",
                    filePath.c_str());
                return vector<string>{string()};
            }
            sourceFilename = filePathNew + fileNameNew;
            string fileNameNewer = fileNameNew.substr(0, fileNameNew.find(".m4a")) + "-decoded.wav";
            outputFilename = (filePathNew + fileNameNewer).c_str();

            /* open input file, and allocate format context */
            if (avformat_open_input(&formatContext, sourceFilename.c_str(), NULL, NULL) < 0) {
                fprintf(stderr, "Could not open source file %s\n", sourceFilename.c_str());
                return vector<string>{string()};
            }

            /* retrieve stream information */
            if (avformat_find_stream_info(formatContext, NULL) < 0) {
                fprintf(stderr, "Could not find stream information\n");
                return vector<string>{string()};
            }

            if (openCodecContext(&audioStreamIndex, &audioDecodeContext, formatContext, AVMEDIA_TYPE_AUDIO, sourceFilename) >= 0) {
                audioStream = formatContext->streams[audioStreamIndex];
                fopen_s(&outputFile, outputFilename.c_str(), "wb");
                if (!outputFile) {
                    fprintf(stderr, "Could not open destination file %s\n", outputFilename.c_str());
                    ret = 1;
                }
            }

            /* dump input information to stderr */
            av_dump_format(formatContext, 0, sourceFilename.c_str(), 0);

            if (!audioStream) {
                fprintf(stderr, "Could not find audio or video stream in the input, aborting\n");
                ret = 1;
            }

            frame = av_frame_alloc();
            if (!frame) {
                fprintf(stderr, "Could not allocate frame\n");
                ret = AVERROR(ENOMEM);
            }

            packet = av_packet_alloc();
            if (!packet) {
                fprintf(stderr, "Could not allocate packet\n");
                ret = AVERROR(ENOMEM);;
            }
            cout << "SAMPLE FORMAT: " << audioDecodeContext->sample_fmt << endl;
            SwrContext* swrContext = swr_alloc();
            swrContext = swr_alloc_set_opts(NULL, AV_CH_LAYOUT_STEREO, av_get_alt_sample_fmt(audioDecodeContext->sample_fmt, 0), 48000, AV_CH_LAYOUT_STEREO, audioDecodeContext->sample_fmt, audioDecodeContext->sample_rate, 0, nullptr);
            swr_init(swrContext);
            if (audioStream)
                printf("Demuxing audio from file '%s' into '%s'\n", sourceFilename.c_str(), outputFilename.c_str());
            AVFrame* newFrame = av_frame_alloc();
            /* read frames from the file */
            while (av_read_frame(formatContext, packet) >= 0) {
                // check if the packet belongs to a stream we are interested in, otherwise
                // skip it

                if (packet->stream_index == audioStreamIndex) {
                    ret = decodePacket(audioDecodeContext, packet, frame, newFrame, &audioFrameCount, outputFile, swrContext);
                }
                av_packet_unref(packet);
                av_frame_free(&frame);
                frame = av_frame_alloc();
                av_frame_free(&newFrame);
                newFrame = av_frame_alloc();
                if (ret < 0)
                    break;
            }

            /* flush the decoders */
            if (audioDecodeContext)
                ret = decodePacket(audioDecodeContext, NULL, frame, newFrame, &audioFrameCount, outputFile, swrContext);

            printf("Demuxing succeeded.\n");

            if (audioStream) {
                int nChannels = audioDecodeContext->channels;
                const char* format;

                if ((ret = getFormatFromSampleFormat(&format, (AVSampleFormat)newFrame->format)) < 0)
                    goto end;
                cout << "Play the output audio file with the command:\n.\\ffplay -f " + to_string(to_hstring(format)) + " -ac " + to_string(nChannels) + " -ar " + to_string(newFrame->sample_rate) + " '" + outputFilename.c_str() + "'\n" << endl;
            }

        end:
            av_frame_unref(frame);
            av_frame_free(&frame);
            av_frame_unref(newFrame);
            av_frame_free(&newFrame);
            fclose(outputFile);
            avformat_free_context(formatContext);
            avcodec_free_context(&audioDecodeContext);
            av_packet_free(&packet);
            vector<string>returnVector;
            returnVector.push_back(filePath);
            returnVector.push_back(fileNameNewer);
            return returnVector;
        }

    };
    
}

#endif
