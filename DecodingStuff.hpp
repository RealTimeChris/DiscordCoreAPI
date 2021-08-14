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

    struct RawFrame {
        vector<uint8_t> data{};
        uint32_t sampleCount = 0;
        int64_t timePoint = 0;
    };

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

    int64_t FileStreamSeek(void* ptr, int64_t pos, int) {
        // Your custom IStream
        DataReader* stream = reinterpret_cast<DataReader*>(ptr);
        // Origin is an item from STREAM_SEEK enum.
        //   STREAM_SEEK_SET - relative to beginning of stream.
        //   STREAM_SEEK_CUR - relative to current position in stream.
        //   STREAM_SEEK_END - relative to end of stream;
        auto returnVal = stream->LoadAsync((uint32_t)pos).get();

        // Return the new position
        return returnVal;
    }

    /**
    * Reads from an IStream into FFmpeg.
    *
    * @param ptr       A pointer to the user-defined IO data structure.
    * @param buf       A buffer to read into.
    * @param buf_size  The size of the buffer buff.
    *
    * @return The number of bytes read into the buffer.
    */
    int FileStreamRead(void* ptr, uint8_t* buf, int buf_size)
    {
        // This is your IStream
        DataReader* stream = reinterpret_cast<DataReader*>(ptr);
        ULONG bytesRead = stream->LoadAsync(buf_size).get();
        if (bytesRead <= 0) {
            return AVERROR_EOF;
        };
        auto hr = stream->ReadBuffer(bytesRead);
        for (unsigned int x = 0; x < bytesRead; x += 1) {
            buf[x] = hr.data()[x];
        }

        if (!hr) {
            return -1;
        }            
        return bytesRead;
    }

    class SongDecoder {
    public:

        SongDecoder(DataReader fileStreamData) {
            // Define your buffer size
            const int FILESTREAMBUFFERSZ = 8192;

            // A IStream - you choose where it comes from.

            // Alloc a buffer for the stream.
            unsigned char* fileStreamBuffer = (unsigned char*)av_malloc(FILESTREAMBUFFERSZ);
            if (fileStreamBuffer == nullptr) {
                // out of memory
            }

            // Get a AVContext stream
            ioContext = avio_alloc_context(
                fileStreamBuffer,    // Buffer
                FILESTREAMBUFFERSZ,  // Buffer size
                0,                   // Buffer is only readable - set to 1 for read/write
                &fileStreamData,      // User (your) specified data
                FileStreamRead,      // Function - Reading Packets (see example)
                0,                   // Function - Write Packets
                FileStreamSeek       // Function - Seek to position in stream (see example)
            );
            if (ioContext == nullptr) {
                // out of memory
            }

            // Allocate a AVContext
            formatContext = avformat_alloc_context();

            // Set up the Format Context
            formatContext->pb = ioContext;
            formatContext->flags |= AVFMT_FLAG_CUSTOM_IO; // we set up our own IO

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
            ret = av_find_best_stream(formatContext, type, -1, -1, NULL, 0);
            if (ret < 0) {
                fprintf(stderr, "Could not find %s stream in input memory stream.\n", av_get_media_type_string(type));
                return;
            }
            else {
                this->audioStreamIndex = ret;
                this->audioStream = formatContext->streams[audioStreamIndex];
                if (!audioStream) {
                    fprintf(stderr, "Could not find collect an audio stream.\n");
                    return;
                }

                /* retrieve stream information */
                if (avformat_find_stream_info(formatContext, NULL) < 0) {
                    fprintf(stderr, "Could not find stream information\n");
                    return;
                }
                /* find decoder for the stream */
                dec = avcodec_find_decoder(audioStream->codecpar->codec_id);
                if (!dec) {
                    fprintf(stderr, "Failed to find %s codec\n",
                        av_get_media_type_string(type));
                    return;
                }

                /* Allocate a codec context for the decoder */
                audioDecodeContext = avcodec_alloc_context3(dec);
                if (!audioDecodeContext) {
                    fprintf(stderr, "Failed to allocate the %s codec context\n",
                        av_get_media_type_string(type));
                    return;
                }

                /* Copy codec parameters from input stream to output codec context */
                if ((ret = avcodec_parameters_to_context(audioDecodeContext, audioStream->codecpar)) < 0) {
                    fprintf(stderr, "Failed to copy %s codec parameters to decoder context\n",
                        av_get_media_type_string(type));
                    return;
                }

                /* Init the decoders */
                if ((ret = avcodec_open2(audioDecodeContext, dec, NULL)) < 0) {
                    fprintf(stderr, "Failed to open %s codec\n",
                        av_get_media_type_string(type));
                    return;
                }
                audioStreamIndex = audioStreamIndex;
                swrContext = swr_alloc();
                swrContext = swr_alloc_set_opts(NULL, AV_CH_LAYOUT_STEREO, av_get_alt_sample_fmt(audioDecodeContext->sample_fmt, 0), 48000, AV_CH_LAYOUT_STEREO, audioDecodeContext->sample_fmt, audioDecodeContext->sample_rate, 0, nullptr);
                swr_init(swrContext);

            }
            av_dump_format(formatContext, 0, "test", 0);
        }

        void collectMoreInput(IBuffer readBuffer) {
            if (ioContext->write_flag) {
                memcpy(ioContext->buffer, readBuffer.data(), readBuffer.Length());
            }
            if (avio_seek(ioContext,0, SEEK_SET) < 0) {
                cout << "Sorry, failed to seek!" << endl;
            }
        }

        vector<RawFrame> getFrames() {
            vector<RawFrame> frames;
            int ret;
            frame = av_frame_alloc();
            if (!frame) {
                fprintf(stderr, "Could not allocate frame\n");
                return vector<RawFrame>();
            }

            packet = av_packet_alloc();
            if (!packet) {
                fprintf(stderr, "Could not allocate packet\n");
                return vector<RawFrame>();
            }

            this->newFrame = av_frame_alloc();
            // read frames from the file
            while(av_read_frame(formatContext, packet) >= 0) {
                // check if the packet belongs to a stream we are interested in, otherwise
                // skip it

                if (packet->stream_index == audioStreamIndex) {
                    // submit the packet to the decoder
                    ret = avcodec_send_packet(audioDecodeContext, packet);
                    if (ret < 0) {
                        fprintf(stderr, "Error submitting a packet for decoding (%s)\n", to_string(ret).c_str());
                        return vector<RawFrame>();
                    }

                    // get all the available frames from the decoder
                    while (ret >= 0) {
                        ret = avcodec_receive_frame(audioDecodeContext, frame);
                        if (ret < 0) {
                            // those two return values are special and mean there is no output
                            // frame available, but there were no errors during decoding
                            if (ret == AVERROR_EOF || ret == AVERROR(EAGAIN))
                                return vector<RawFrame>();

                            fprintf(stderr, "Error during decoding (%s)\n", to_string(ret).c_str());
                            return vector<RawFrame>();
                        }

                        (audioFrameCount)++;
                        if (!swr_is_initialized(swrContext)) {
                            swr_init(swrContext);
                        }
                        newFrame->channel_layout = AV_CH_LAYOUT_STEREO;
                        newFrame->sample_rate = 48000;
                        newFrame->format = AVSampleFormat::AV_SAMPLE_FMT_FLT;
                        newFrame->nb_samples = frame->nb_samples;
                        newFrame->pts = frame->pts;
                        swr_convert_frame(swrContext, newFrame, frame);
                        printf("Audio Frame #:%d Number of Samples:%d pts:%s\n", audioFrameCount, newFrame->nb_samples, to_string(newFrame->pts).c_str());
                        size_t unpadded_linesize = newFrame->nb_samples * av_get_bytes_per_sample((AVSampleFormat)newFrame->format) * 2;

                        vector<uint8_t> newVector{};
                        for (int x = 0; x < unpadded_linesize; x += 1) {
                            newVector.push_back(newFrame->extended_data[0][x]);
                        }

                        RawFrame rawFrame{};
                        rawFrame.data = newVector;
                        rawFrame.timePoint = packet->pts;
                        rawFrame.sampleCount = frame->nb_samples;
                        frames.push_back(rawFrame);

                        __int64 sampleCount = swr_get_delay(swrContext, newFrame->sample_rate);
                        if (sampleCount > 0) {
                            if (!swr_is_initialized(swrContext)) {
                                swr_init(swrContext);
                            }
                            swr_convert_frame(swrContext, newFrame, nullptr);
                            vector<uint8_t> newVector02{};
                            for (int x = 0; x < *newFrame->linesize; x += 1) {
                                newVector02.push_back(*newFrame->extended_data[x]);
                            }

                            RawFrame rawFrame02{};
                            rawFrame02.data = newVector02;
                            rawFrame02.timePoint = frame->pts;
                            rawFrame02.sampleCount = frame->nb_samples;
                            frames.push_back(rawFrame02);
                        }
                        
                        if (ret < 0) {
                            return vector<RawFrame>();
                        }
                            
                    }
                }
                av_packet_unref(packet);
                av_frame_unref(frame);
                av_frame_free(&frame);
                av_frame_unref(newFrame);
                av_frame_free(&newFrame);
                return vector<RawFrame>();
            }
        }

        ~SongDecoder() {
            if (formatContext) {
                avformat_close_input(&formatContext);
            }
            if (formatContext) {
                avformat_free_context(formatContext);
            }
            if (ioContext) {
                av_free(ioContext);
            }
            if (frame) {
                av_frame_unref(frame);
                av_frame_free(&frame);
            }
            if (newFrame) {
                av_frame_unref(newFrame);
                av_frame_free(&newFrame);
            }
            if (audioDecodeContext) {
                avcodec_free_context(&audioDecodeContext);
            }
            if (packet) {
                av_packet_free(&packet);
            }
        }

    protected:
        AVIOContext* ioContext = nullptr;
        AVFormatContext* formatContext = nullptr;
        AVCodecContext* audioDecodeContext = nullptr;
        AVStream* audioStream = nullptr;
        SwrContext* swrContext = nullptr;
        AVCodec* dec = nullptr;
        int  audioStreamIndex, audioFrameIndex, audioFrameCount;
        AVFrame* frame = nullptr, * newFrame = nullptr;
        AVPacket* packet = nullptr;

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
        /*
        vector<string> decodeAudioData(string filePathNew, string fileNameNew)
        {
            
            
            cout << "SAMPLE FORMAT: " << audioDecodeContext->sample_fmt << endl;
            SwrContext* swrContext = swr_alloc();
            swrContext = swr_alloc_set_opts(NULL, AV_CH_LAYOUT_STEREO, av_get_alt_sample_fmt(audioDecodeContext->sample_fmt, 0), 48000, AV_CH_LAYOUT_STEREO, audioDecodeContext->sample_fmt, audioDecodeContext->sample_rate, 0, nullptr);
            swr_init(swrContext);
            if (audioStream)
                printf("Demuxing audio from file '%s' into '%s'\n", sourceFilename.c_str(), outputFilename.c_str());
            AVFrame* newFrame = av_frame_alloc();
            /* read frames from the file
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
                if (ret < 0) {
                    break;
                }
            }

            /* flush the decoders
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
        }
        */
        
    };
    
}

#endif
