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

#include <discordcoreapi/FoundationEntities.hpp>
#include <opus/opus.h>

extern "C" {
#include <libavformat/avformat.h>
#include <libswresample/swresample.h>
}

namespace DiscordCoreInternal {

	struct DiscordCoreAPI_Dll OpusDecoderWrapper {
		struct DiscordCoreAPI_Dll OpusDecoderDeleter {
			void operator()(OpusDecoder*) noexcept;
		};

		OpusDecoderWrapper();

		std::basic_string_view<opus_int16> decodeData(std::string_view dataToDecode);

	  protected:
		std::unique_ptr<OpusDecoder, OpusDecoderDeleter> ptr{ nullptr, OpusDecoderDeleter{} };
		std::vector<opus_int16> data{};
	};

	struct DiscordCoreAPI_Dll AVFrameWrapper {
		struct DiscordCoreAPI_Dll AVFrameDeleter {
			void operator()(AVFrame* other);
		};

		AVFrameWrapper& operator=(AVFrame* other);

		AVFrameWrapper(AVFrame* other);

		AVFrame* operator->();

		operator AVFrame*();

		AVFrameWrapper() noexcept = default;

	  protected:
		std::unique_ptr<AVFrame, AVFrameDeleter> ptr{ nullptr, AVFrameDeleter{} };
	};

	struct DiscordCoreAPI_Dll AVCodecContextWrapper {
		struct DiscordCoreAPI_Dll AVCodecContextDeleter {
			void operator()(AVCodecContext* other);
		};

		AVCodecContextWrapper& operator=(AVCodecContext* other);

		AVCodecContextWrapper(AVCodecContext* other);

		AVCodecContext* operator->();

		operator AVCodecContext*();

		AVCodecContextWrapper() noexcept = default;

	  protected:
		std::unique_ptr<AVCodecContext, AVCodecContextDeleter> ptr{ nullptr, AVCodecContextDeleter{} };
	};

	struct DiscordCoreAPI_Dll AVFormatContextWrapper01 {
		AVFormatContextWrapper01() noexcept = default;

		AVFormatContext* theContext{ nullptr };
		bool didItInitialize{ false };
	};

	struct DiscordCoreAPI_Dll AVFormatContextWrapper {
		struct DiscordCoreAPI_Dll AVFormatContextDeleter {
			void operator()(AVFormatContextWrapper01* other);
		};

		AVFormatContextWrapper& operator=(AVFormatContext* other);

		AVFormatContextWrapper(AVFormatContext* other);

		bool* getBoolPtr();

		AVFormatContext* operator->();

		AVFormatContext** operator*();

		operator AVFormatContext*();

		AVFormatContextWrapper() noexcept = default;

	  protected:
		std::unique_ptr<AVFormatContextWrapper01, AVFormatContextDeleter> ptr{ std::make_unique<AVFormatContextWrapper01>().release(),
			AVFormatContextDeleter{} };
	};

	struct DiscordCoreAPI_Dll SwrContextWrapper {
		struct DiscordCoreAPI_Dll SwrContextDeleter {
			void operator()(SwrContext* other);
		};

		SwrContextWrapper& operator=(SwrContext* other);

		SwrContextWrapper(SwrContext* other);

		operator SwrContext*();

		SwrContextWrapper() noexcept = default;

	  protected:
		std::unique_ptr<SwrContext, SwrContextDeleter> ptr{ nullptr, SwrContextDeleter{} };
	};

	struct DiscordCoreAPI_Dll AVIOContextWrapper {
		struct DiscordCoreAPI_Dll AVIOContextDeleter {
			void operator()(AVIOContext* other);
		};

		AVIOContextWrapper& operator=(AVIOContext* other);

		AVIOContextWrapper(AVIOContext* other);

		AVIOContext* operator->();

		operator AVIOContext*();

		AVIOContextWrapper() noexcept = default;

	  protected:
		std::unique_ptr<AVIOContext, AVIOContextDeleter> ptr{ nullptr, AVIOContextDeleter{} };
	};

	struct DiscordCoreAPI_Dll AVPacketWrapper {
		struct DiscordCoreAPI_Dll AVPacketDeleter {
			void operator()(AVPacket* other);
		};

		AVPacketWrapper& operator=(AVPacket* other);

		AVPacketWrapper(AVPacket* other);

		AVPacket* operator->();

		operator AVPacket*();

		AVPacketWrapper() noexcept = default;

	  protected:
		std::unique_ptr<AVPacket, AVPacketDeleter> ptr{ nullptr, AVPacketDeleter{} };
	};

	struct DiscordCoreAPI_Dll BuildAudioDecoderData {
	  public:
		DiscordCoreAPI::ConfigManager* configManager{ nullptr };
		int64_t totalFileSize{ 0 };
		int64_t bufferMaxSize{ 0 };
	};

	class DiscordCoreAPI_Dll AudioDecoder {
	  public:
		AudioDecoder(const BuildAudioDecoderData& dataPackage);

		bool getFrame(DiscordCoreAPI::AudioFrameData& dataPackage);

		void submitDataForDecoding(std::string dataToDecode);

		bool haveWeFailed();

		void startMe();

		~AudioDecoder();

	  protected:
		int64_t audioStreamIndex{ 0 }, bufferMaxSize{ 0 }, bytesRead{ 0 }, totalFileSize{ 0 };
		DiscordCoreAPI::UnboundedMessageBlock<DiscordCoreAPI::AudioFrameData> outDataBuffer{};
		DiscordCoreAPI::UnboundedMessageBlock<std::string> inputDataBuffer{};
		DiscordCoreAPI::ConfigManager* configManager{ nullptr };
		std::atomic_int32_t refreshTimeForBuffer{ 10000 };
		std::atomic_bool haveWeFailedBool{ false };
		AVCodecContextWrapper audioDecodeContext{};
		std::atomic_bool areWeQuitting{ false };
		AVFormatContextWrapper formatContext{};
		AVFrameWrapper frame{}, newFrame{};
		AVIOContextWrapper ioContext{};
		SwrContextWrapper swrContext{};
		std::string currentBuffer{};
		bool haveWeBooted{ false };
		AVPacketWrapper packet{};
		AVStream* audioStream{};
		AVCodec* codec{};
		std::unique_ptr<std::jthread> taskThread{ nullptr };

		static int32_t ReadBufferData(void* opaque, uint8_t* buf, int32_t);

		void run(std::stop_token token);

		void cancelMe();
	};

}// namespace DiscordCoreAPI
