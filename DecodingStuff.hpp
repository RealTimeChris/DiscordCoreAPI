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

    /**
     * Fill dst buffer with nb_samples, generated starting from t.
     */
    static void fill_samples(double* dst, int nb_samples, int nb_channels, int sample_rate, double* t)
    {
        int i, j;
        double tincr = 1.0 / sample_rate, * dstp = dst;
        const double c = 2 * M_PI * 440.0;

        /* generate sin tone with 440Hz frequency and duplicated channels */
        for (i = 0; i < nb_samples; i++) {
            *dstp = sin(c * *t);
            for (j = 1; j < nb_channels; j++)
                dstp[j] = dstp[0];
            dstp += nb_channels;
            *t += tincr;
        }
    }

    int decodeAudioData(string filePath, string fileName) {
        cout << "WERE HERE WERE HERE WERE HERE" << endl;
        int64_t src_ch_layout = AV_CH_LAYOUT_STEREO, dst_ch_layout = AV_CH_LAYOUT_STEREO;
        int src_rate = 44100, dst_rate = 48000;
        uint8_t** src_data = NULL, ** dst_data = NULL;
        int src_nb_channels = 0, dst_nb_channels = 0;
        int src_linesize, dst_linesize;
        int src_nb_samples = 1024, dst_nb_samples, max_dst_nb_samples;
        enum AVSampleFormat src_sample_fmt = AV_SAMPLE_FMT_DBL, dst_sample_fmt = AV_SAMPLE_FMT_FLT;
        const char* dst_filename = NULL;
        FILE* dst_file;
        int dst_bufsize;
        const char* fmt;
        struct SwrContext* swr_ctx;
        double t;
        int ret;

        if (filePath.size() <= 2) {
            fprintf(stderr, "Usage: %s output_file\n"
                "API example program to show how to resample an audio stream with libswresample.\n"
                "This program generates a series of audio frames, resamples them to a specified "
                "output format and rate and saves them to an output file named output_file.\n");
            exit(1);
        }
        dst_filename = (filePath + fileName).c_str();

        dst_file = fopen(dst_filename, "wb");
        if (!dst_file) {
            fprintf(stderr, "Could not open destination file %s\n", dst_filename);
            exit(1);
        }

        /* create resampler context */
        swr_ctx = swr_alloc();
        if (!swr_ctx) {
            fprintf(stderr, "Could not allocate resampler context\n");
            ret = AVERROR(ENOMEM);
            
        }

        /* set options */
        av_opt_set_int(swr_ctx, "in_channel_layout", src_ch_layout, 0);
        av_opt_set_int(swr_ctx, "in_sample_rate", src_rate, 0);
        av_opt_set_sample_fmt(swr_ctx, "in_sample_fmt", src_sample_fmt, 0);

        av_opt_set_int(swr_ctx, "out_channel_layout", dst_ch_layout, 0);
        av_opt_set_int(swr_ctx, "out_sample_rate", dst_rate, 0);
        av_opt_set_sample_fmt(swr_ctx, "out_sample_fmt", dst_sample_fmt, 0);

        /* initialize the resampling context */
        if ((ret = swr_init(swr_ctx)) < 0) {
            fprintf(stderr, "Failed to initialize the resampling context\n");
            
        }

        /* allocate source and destination samples buffers */

        src_nb_channels = av_get_channel_layout_nb_channels(src_ch_layout);
        ret = av_samples_alloc_array_and_samples(&src_data, &src_linesize, src_nb_channels,
            src_nb_samples, src_sample_fmt, 0);
        if (ret < 0) {
            fprintf(stderr, "Could not allocate source samples\n");
            
        }

        /* compute the number of converted samples: buffering is avoided
         * ensuring that the output buffer will contain at least all the
         * converted input samples */
        max_dst_nb_samples = dst_nb_samples =
            av_rescale_rnd(src_nb_samples, dst_rate, src_rate, AV_ROUND_UP);

        /* buffer is going to be directly written to a rawaudio file, no alignment */
        dst_nb_channels = av_get_channel_layout_nb_channels(dst_ch_layout);
        ret = av_samples_alloc_array_and_samples(&dst_data, &dst_linesize, dst_nb_channels,
            dst_nb_samples, dst_sample_fmt, 0);
        if (ret < 0) {
            fprintf(stderr, "Could not allocate destination samples\n");
            
        }

        t = 0;
        do {
            /* generate synthetic audio */
            fill_samples((double*)src_data[0], src_nb_samples, src_nb_channels, src_rate, &t);

            /* compute destination number of samples */
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

            /* convert to destination format */
            ret = swr_convert(swr_ctx, dst_data, dst_nb_samples, (const uint8_t**)src_data, src_nb_samples);
            if (ret < 0) {
                fprintf(stderr, "Error while converting\n");
            }
            dst_bufsize = av_samples_get_buffer_size(&dst_linesize, dst_nb_channels,
                ret, dst_sample_fmt, 1);
            if (dst_bufsize < 0) {
                fprintf(stderr, "Could not get sample buffer size\n");
            }
            printf("t:%f in:%d out:%d\n", t, src_nb_samples, ret);
            fwrite(dst_data[0], 1, dst_bufsize, dst_file);
        } while (t < 10);
        const char* format = fmt;
        int64_t layout = dst_ch_layout;
        int nb_chans = dst_nb_channels;
        int rateNew = dst_rate;
        const char* fileNameNew = dst_filename;
        if ((get_format_from_sample_fmt(&fmt, dst_sample_fmt)) < 0)
            
        fprintf(stderr, "Resampling succeeded.Play the output file with the command : \nffplay -f %s -channel_layout %PRId64 -channels %d -ar %d %s\n", fmt, dst_ch_layout, dst_nb_channels, dst_rate, dst_filename);

    end:
        fclose(dst_file);

        if (src_data)
            av_freep(&src_data[0]);
        av_freep(&src_data);

        if (dst_data)
            av_freep(&dst_data[0]);
        av_freep(&dst_data);

        swr_free(&swr_ctx);
        return ret < 0;
    }
}

#endif
