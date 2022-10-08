/*
	DiscordCoreAPI, A bot library for Discord, written in C++, and featuring explicit multithreading through the usage of custom, asynchronous C++ CoRoutines.

	Copyright 2021, 2022 Chris M. (RealTimeChris)

	This library is free software; you can redistribute it and/or
	modify it under the terms of the GNU Lesser General Public
	License as published by the Free Software Foundation; either
	version 2.1 of the License, or (at your option) any later version.

	This library is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
	Lesser General Public License for more details.

	You should have received a copy of the GNU Lesser General Public
	License along with this library; if not, write to the Free Software
	Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301
	USA
*/
/// AudioDecoder.hpp - Header for the audio decoder class.
/// Jul 29, 2021
/// https://discordcoreapi.com
/// \file AudioDecoder.hpp

#pragma once

#ifndef AUDIO_DECODER
	#define AUDIO_DECODER

	#include <discordcoreapi/EventEntities.hpp>
	#include <discordcoreapi/FoundationEntities.hpp>

extern "C" {
	#include <libavformat/avformat.h>
	#include <libswresample/swresample.h>
}

namespace DiscordCoreInternal {

	struct DiscordCoreAPI_Dll AVFrameWrapper {
		struct DiscordCoreAPI_Dll AVFrameDeleter {
			Void operator()(AVFrame* other);
		};

		AVFrameWrapper& operator=(AVFrame* other);

		AVFrameWrapper(AVFrame* other);

		AVFrame* operator->();

		operator AVFrame*();

		AVFrameWrapper() noexcept = default;

	  protected:
		UniquePtr<AVFrame, AVFrameDeleter> thePtr{ nullptr, AVFrameDeleter{} };
	};

	struct DiscordCoreAPI_Dll AVCodecContextWrapper {
		struct DiscordCoreAPI_Dll AVCodecContextDeleter {
			Void operator()(AVCodecContext* other);
		};

		AVCodecContextWrapper& operator=(AVCodecContext* other);

		AVCodecContextWrapper(AVCodecContext* other);

		AVCodecContext* operator->();

		operator AVCodecContext*();

		AVCodecContextWrapper() noexcept = default;

	  protected:
		UniquePtr<AVCodecContext, AVCodecContextDeleter> thePtr{ nullptr, AVCodecContextDeleter{} };
	};

	struct DiscordCoreAPI_Dll AVFormatContextWrapper01 {
		AVFormatContextWrapper01() noexcept = default;

		AVFormatContext* theContext{ nullptr };
		Bool didItInitialize{ false };
	};

	struct DiscordCoreAPI_Dll AVFormatContextWrapper {
		struct DiscordCoreAPI_Dll AVFormatContextDeleter {
			Void operator()(AVFormatContextWrapper01* other);
		};

		AVFormatContextWrapper& operator=(AVFormatContext* other);

		AVFormatContextWrapper(AVFormatContext* other);

		Bool* getBoolPtr();

		AVFormatContext* operator->();

		AVFormatContext** operator*();

		operator AVFormatContext*();

		AVFormatContextWrapper() noexcept = default;

	  protected:
		UniquePtr<AVFormatContextWrapper01, AVFormatContextDeleter> thePtr{ new AVFormatContextWrapper01{}, AVFormatContextDeleter{} };
	};

	struct DiscordCoreAPI_Dll SwrContextWrapper {
		struct DiscordCoreAPI_Dll SwrContextDeleter {
			Void operator()(SwrContext* other);
		};

		SwrContextWrapper& operator=(SwrContext* other);

		SwrContextWrapper(SwrContext* other);

		operator SwrContext*();

		SwrContextWrapper() noexcept = default;

	  protected:
		UniquePtr<SwrContext, SwrContextDeleter> thePtr{ nullptr, SwrContextDeleter{} };
	};

	struct DiscordCoreAPI_Dll AVIOContextWrapper {
		struct DiscordCoreAPI_Dll AVIOContextDeleter {
			Void operator()(AVIOContext* other);
		};

		AVIOContextWrapper& operator=(AVIOContext* other);

		AVIOContextWrapper(AVIOContext* other);

		AVIOContext* operator->();

		operator AVIOContext*();

		AVIOContextWrapper() noexcept = default;

	  protected:
		UniquePtr<AVIOContext, AVIOContextDeleter> thePtr{ nullptr, AVIOContextDeleter{} };
	};

	struct DiscordCoreAPI_Dll AVPacketWrapper {
		struct DiscordCoreAPI_Dll AVPacketDeleter {
			Void operator()(AVPacket* other);
		};

		AVPacketWrapper& operator=(AVPacket* other);

		AVPacketWrapper(AVPacket* other);

		AVPacket* operator->();

		operator AVPacket*();

		AVPacketWrapper() noexcept = default;

	  protected:
		UniquePtr<AVPacket, AVPacketDeleter> thePtr{ nullptr, AVPacketDeleter{} };
	};

	struct DiscordCoreAPI_Dll BuildAudioDecoderData {
	  public:
		DiscordCoreAPI::ConfigManager* configManager{ nullptr };
		Int64 totalFileSize{ 0 };
		Int64 bufferMaxSize{ 0 };
	};

	class DiscordCoreAPI_Dll AudioDecoder {
	  public:
		AudioDecoder(const BuildAudioDecoderData& dataPackage);

		Bool getFrame(DiscordCoreAPI::AudioFrameData& dataPackage);

		Void submitDataForDecoding(String dataToDecode);

		Bool haveWeFailed();

		Void startMe();

		~AudioDecoder();

	  protected:
		Int64 audioStreamIndex{ 0 }, bufferMaxSize{ 0 }, bytesRead{ 0 }, totalFileSize{ 0 };
		DiscordCoreAPI::UnboundedMessageBlock<DiscordCoreAPI::AudioFrameData> outDataBuffer{};
		DiscordCoreAPI::UnboundedMessageBlock<String> inputDataBuffer{};
		DiscordCoreAPI::ConfigManager* configManager{ nullptr };
		AtomicInt32 refreshTimeForBuffer{ 10000 };
		AtomicBool haveWeFailedBool{ false };
		AVCodecContextWrapper audioDecodeContext{};
		AtomicBool areWeQuitting{ false };
		AVFormatContextWrapper formatContext{};
		AVFrameWrapper frame{}, newFrame{};
		AVIOContextWrapper ioContext{};
		SwrContextWrapper swrContext{};
		String currentBuffer{};
		Bool haveWeBooted{ false };
		AVPacketWrapper packet{};
		AVStream* audioStream{};
		AVCodec* codec{};
		UniquePtr<std::jthread> taskThread{ nullptr };

		static Int32 ReadBufferData(Void* opaque, Uint8* buf, Int32);

		Void run(std::stop_token stopToken);

		Void cancelMe();
	};

}// namespace DiscordCoreAPI
#endif