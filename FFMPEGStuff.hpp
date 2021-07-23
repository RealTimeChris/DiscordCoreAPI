// FFMPegStuff.hpp - Header for the FFMPeg stuff
// Jul 19, 2021
// Chris M.
// https://github.com/RealTimeChris

#pragma once

#ifndef _FFMPEG_STUFF_
#define _FFMPEG_STUFF_

#include "../pch.h"

namespace DiscordCoreAPI {

    class AudioStream : public IStream
    {
    public:
        AudioStream(IBuffer audioDataNew)
        {
            _refcount = 1;
            this->audioData = audioDataNew;
        }

        ~AudioStream()
        {
        }

    public:

        virtual HRESULT STDMETHODCALLTYPE QueryInterface(REFIID iid, void** ppvObject)
        {
            if (iid == __uuidof(IUnknown)
                || iid == __uuidof(IStream)
                || iid == __uuidof(ISequentialStream))
            {
                *ppvObject = static_cast<IStream*>(this);
                AddRef();
                return S_OK;
            }
            else
                return E_NOINTERFACE;
        }

        virtual ULONG STDMETHODCALLTYPE AddRef(void)
        {
            return (ULONG)InterlockedIncrement(&_refcount);
        }

        virtual ULONG STDMETHODCALLTYPE Release(void)
        {
            ULONG res = (ULONG)InterlockedDecrement(&_refcount);
            if (res == 0)
                delete this;
            return res;
        }

        // ISequentialStream Interface  
    public:
        virtual HRESULT STDMETHODCALLTYPE Read(void* pv, ULONG cb, ULONG* pcbRead)
        {
            cout << "READ LENGTH: " << to_string(cb) << endl;
            uint8_t* readData = this->audioData.data();
            uint8_t* readDataReturn = new uint8_t[cb];
            for (unsigned int x = 0; x < cb; x += 1) {
                readDataReturn[x] = readData[x];
            }
            *pcbRead = cb;
            errno_t error = memcpy_s(pv, cb, readDataReturn, cb);
            if (error != S_OK) {
                throw exception("Failed to read memory from the AudioStream class!");
            }
            
            return  S_OK;
        }

        virtual HRESULT STDMETHODCALLTYPE Write(void const* pv, ULONG cb, ULONG* pcbWritten)
        {
            return S_OK;
        }

        // IStream Interface  
    public:
        virtual HRESULT STDMETHODCALLTYPE SetSize(ULARGE_INTEGER)
        {
            return E_NOTIMPL;
        }

        virtual HRESULT STDMETHODCALLTYPE CopyTo(IStream*, ULARGE_INTEGER, ULARGE_INTEGER*,
            ULARGE_INTEGER*)
        {
            return E_NOTIMPL;
        }

        virtual HRESULT STDMETHODCALLTYPE Commit(DWORD)
        {
            return E_NOTIMPL;
        }

        virtual HRESULT STDMETHODCALLTYPE Revert(void)
        {
            return E_NOTIMPL;
        }

        virtual HRESULT STDMETHODCALLTYPE LockRegion(ULARGE_INTEGER, ULARGE_INTEGER, DWORD)
        {
            return E_NOTIMPL;
        }

        virtual HRESULT STDMETHODCALLTYPE UnlockRegion(ULARGE_INTEGER, ULARGE_INTEGER, DWORD)
        {
            return E_NOTIMPL;
        }

        virtual HRESULT STDMETHODCALLTYPE Clone(IStream**)
        {
            return E_NOTIMPL;
        }

        virtual HRESULT STDMETHODCALLTYPE Seek(LARGE_INTEGER liDistanceToMove, DWORD dwOrigin,
            ULARGE_INTEGER* lpNewFilePointer)
        {
            DWORD dwMoveMethod;

            switch (dwOrigin)
            {
            case STREAM_SEEK_SET:
                dwMoveMethod = FILE_BEGIN;
                break;
            case STREAM_SEEK_CUR:
                dwMoveMethod = FILE_CURRENT;
                break;
            case STREAM_SEEK_END:
                dwMoveMethod = FILE_END;
                break;
            default:
                return STG_E_INVALIDFUNCTION;
                break;
            }

                return HRESULT_FROM_WIN32(GetLastError());
            return S_OK;
        }

        virtual HRESULT STDMETHODCALLTYPE Stat(STATSTG* pStatstg, DWORD grfStatFlag)
        {

            STATSTG statsStat; 
            statsStat.cbSize.QuadPart = audioData.Length();
            *pStatstg = statsStat; 
            return S_OK;
        }

    private:
        IBuffer audioData;
        LONG _refcount;
    };

    int streamRead(void* ptr, uint8_t* buf, int buf_size)
    {
        // This is your IStream
        AudioStream* stream = reinterpret_cast<AudioStream*>(ptr);
        STATSTG statsStruct;
        stream->Stat(&statsStruct, STATFLAG_NONAME);
        cout << "STREAM SIZE: " << to_string(statsStruct.cbSize.QuadPart) << endl;
        ULONG bytesRead = 0;
        HRESULT hr = stream->Read(buf, buf_size, &bytesRead);
        if (hr == S_FALSE)
            return AVERROR_EOF;  // End of file

        if (FAILED(hr))
            return -1;
        return bytesRead;
    };

    // whence: SEEK_SET, SEEK_CUR, SEEK_END (like fseek) and AVSEEK_SIZE
    int64_t streamSeek(void* ptr, int64_t pos, int whence)
    {
        // Quelle Abfragen:
        AudioStream* pStream = reinterpret_cast<AudioStream*>(ptr);

        // Seek:
        LARGE_INTEGER in = { pos };
        ULARGE_INTEGER out = { 0 };
        if (FAILED(pStream->Seek(in, whence, &out))) {
            return -1;
        }
        // Return the new position:
        return out.QuadPart;
    }

    class CustomIOContext {
    public:
        AVFormatContext* formatContext;
        AudioStream* fileStreamData;

        CustomIOContext(IBuffer audioData) {
            this->fileStreamBuffer = (unsigned char*)av_malloc(audioData.Length());
            if (this->fileStreamBuffer == NULL) {
                throw exception("Could not allocate the AudioStream buffer!");
            }
            this->fileStreamData = new AudioStream(audioData);
            this->fileStreamData->SetSize((ULARGE_INTEGER)audioData.Length());
            if (this->fileStreamData->Write(audioData.data(), audioData.Length(), NULL) != S_OK) {
                throw exception("Error writing the data to the stream of CustomIOContext!\n");
            };
            this->fileStreamData->Seek((LARGE_INTEGER)0, STREAM_SEEK_SET, nullptr);
            this->avioContext = avio_alloc_context(
                this->fileStreamBuffer,    // Buffer
                audioData.Length(),  // Buffer size
                0,                   // Buffer is only readable - set to 1 for read/write
                this->fileStreamData,      // User (your) specified data
                streamRead,      // Function - Reading Packets (see example)
                0,                // Function - Write Packets
                streamSeek// Function - Seek to position in stream (see example)
            );
            if (!this->avioContext) {
                throw exception("Failed to allocate the context!");
            }
            this->avioContext->buffer = this->fileStreamBuffer;
            this->avioContext->buffer_size = audioData.Length();
            memcpy_s(this->fileStreamBuffer, audioData.Length(), audioData.data(), audioData.Length());
            STATSTG statsStruct;
            this->fileStreamData->Stat(&statsStruct, STATFLAG_NONAME);
            cout << "STREAM SIZE: " << to_string(statsStruct.cbSize.QuadPart) << endl;
            this->formatContext = avformat_alloc_context();
            this->formatContext->flags = AVFMT_FLAG_CUSTOM_IO;
            this->formatContext->iformat = new AVInputFormat();
            this->formatContext->iformat->flags = AVFMT_NOFILE;
            int error = avformat_open_input(&this->formatContext, 0, this->formatContext->iformat, nullptr);
            if (error < 0) {
                const size_t charCount = 128;
                char* charString = new char[charCount];
                av_make_error_string(charString, charCount, error);
                cout << "Error opening the CustomIOContext: " << charString << endl << endl;
            }
        };

    protected:
        unsigned char* fileStreamBuffer;
        AVIOContext* avioContext;
    };

    static int getInputStreams(int* streamIndex,
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
        }

        *streamIndex = streamIndexValue;

        return 0;
    }

    static int output_audio_frame(AVFrame* frame, InMemoryRandomAccessStream* randomAccessStream, int* offsetIntoStream, int* audioFrameIndex) {
        size_t unpadded_linesize = frame->nb_samples * av_get_bytes_per_sample((AVSampleFormat)frame->format);
        cout << "audio_frame " + to_string(frame->nb_samples) + " samples , @ " + to_string(av_get_bytes_per_sample((AVSampleFormat)frame->format)) + " bytes per sample." << endl;
        (*audioFrameIndex)++;

        /* Write the raw audio data samples of the first plane. This works
         * fine for packed formats (e.g. AV_SAMPLE_FMT_S16). However,
         * most audio decoders output planar audio, which uses a separate
         * plane of audio samples for each channel (e.g. AV_SAMPLE_FMT_S16P).
         * In other words, this code will write only the first audio channel
         * in these cases.
         * You should use libswresample or libavfilter to convert the frame
         * to packed data. */

        DataWriter dataWriter(randomAccessStream->GetOutputStreamAt(*offsetIntoStream));
        dataWriter.UnicodeEncoding(UnicodeEncoding::Utf8);
        for (int x = 0; x < frame->linesize[0] * 2; x += 1) {
            dataWriter.WriteByte(frame->extended_data[x % 2][x / 2]);
        }
        dataWriter.StoreAsync().get();
        *offsetIntoStream = (int)unpadded_linesize;
        return 0;
    }

    int decodePacket(AVCodecContext* codecContext, const AVPacket* packet, AVFrame* frame, InMemoryRandomAccessStream* randomAccessStream, int* offsetIntoStream, int* audioFrameIndex) {
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
    /*
    IBuffer demux(IBuffer audioBuffer, string filePathName) {
        AVCodec avCodec;
        AVCodecContext* audioCodecContext = nullptr;
        AVStream* audioStream = nullptr;
        int audioStreamIndex = 0, ret = 0, offsetIntoStream = 0;
        AVFrame* frame = nullptr;
        AVPacket* packet = nullptr;
        IBuffer outputBuffer = nullptr;
        InMemoryRandomAccessStream randomAccessStream;
        InMemoryRandomAccessStream myStream;
        //CustomIOContext formatContext(audioBuffer);
        AVFormatContext* formatContext = avformat_alloc_context();

        if (avformat_open_input(&formatContext, filePathName.c_str(), nullptr, nullptr) != 0) {
            fprintf(stderr, "Could not open the file stream!\n");
            return IBuffer();
        }

        if (avformat_find_stream_info(formatContext, nullptr) < 0) {
            fprintf(stderr, "Could not find stream information\n");
            return IBuffer();
        }

        if (getInputStreams(&audioStreamIndex, &audioCodecContext, formatContext, AVMEDIA_TYPE_AUDIO, filePathName) >= 0) {
            audioStream = formatContext->streams[audioStreamIndex];
        }

        av_dump_format(formatContext, 0, "", 0);

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
        }
    end:
        avcodec_free_context(&audioCodecContext);
        avformat_close_input(&formatContext);
        av_packet_free(&packet);
        av_frame_free(&frame);

        return outputBuffer;
    }
    */
    static int openCodecContext(int* stream_idx,
        AVFormatContext* fmt_ctx, enum AVMediaType type)
    {
        int ret, stream_index;
        AVStream* st;
        AVCodecContext* dec_ctx = NULL;
        AVCodec* dec = NULL;
        AVDictionary* opts = NULL;
        ret = av_find_best_stream(fmt_ctx, type, -1, -1, NULL, 0);
        if (ret < 0) {
            fprintf(stderr, "Could not find %s stream in input file '%s'\n",
                av_get_media_type_string(type));
            return ret;
        }
        else {
            stream_index = ret;
            st = fmt_ctx->streams[stream_index];
            /* find decoder for the stream */
            dec_ctx = st->codec;
            dec = avcodec_find_decoder(dec_ctx->codec_id);
            if (!dec) {
                fprintf(stderr, "Failed to find %s codec\n",
                    av_get_media_type_string(type));
                return AVERROR(EINVAL);
            }
            /* Init the decoders, with or without reference counting */
            if ((ret = avcodec_open2(dec_ctx, dec, &opts)) < 0) {
                fprintf(stderr, "Failed to open %s codec\n",
                    av_get_media_type_string(type));
                return ret;
            }
            *stream_idx = stream_index;
        }
        return 0;
    }

    IBuffer demux(string downloadURL) {
        AVCodecContext* audioCodecContext = nullptr;
        AVStream* audioStream = nullptr;
        int audioStreamIndex = -1, ret = 0, offsetIntoStream = 0;
        AVFrame* frame = nullptr;
        AVPacket* packet = nullptr;
        IBuffer outputBuffer = nullptr;
        InMemoryRandomAccessStream randomAccessStream;
        InMemoryRandomAccessStream myStream;
        AVFormatContext* formatContext = ::avformat_alloc_context();

        int error = avformat_open_input(&formatContext, "C:\\Users\\Chris\\Downloads\\Skrillex - First Of The Year (Equinox) [Official Music Video] 1627070955308433200.webm", NULL, NULL);
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

        if (openCodecContext(&audioStreamIndex, formatContext, AVMediaType::AVMEDIA_TYPE_AUDIO) != 0) {
            cout << "Could not find open the codec context!\n" << endl;
            return IBuffer();
        }

        if (getInputStreams(&audioStreamIndex, &audioCodecContext, formatContext, AVMEDIA_TYPE_AUDIO, downloadURL) >= 0) {
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
    
}

#endif
