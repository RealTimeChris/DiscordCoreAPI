/*
*
	DiscordCoreAPI, A bot library for Discord, written in C++, and featuring explicit multithreading through the usage of custom, asynchronous C++ CoRoutines.

	Copyright 2021, 2022 Chris M. (RealTimeChris)

	This file is part of DiscordCoreAPI.
	DiscordCoreAPI is free software: you can redistribute it and/or modify it under the terms of the GNU
	General Public License as published by the Free Software Foundation, either version 3 of the License,
	or (at your option) any later version.
	DiscordCoreAPI is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without
	even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.
	You should have received a copy of the GNU General Public License along with DiscordCoreAPI.
	If not, see <https://www.gnu.org/licenses/>.

*/
/// AudioDecoder.hpp - Header for the audio decoder class.
/// Jul 29, 2021
/// https://discordcoreapi.com
/// \file AudioDecoder.hpp

#pragma once

#include <discordcoreapi/EventEntities.hpp>
#include <discordcoreapi/FoundationEntities.hpp>

extern "C" {
#include <libavformat/avformat.h>
#include <libswresample/swresample.h>
}

namespace DiscordCoreInternal {

	struct DiscordCoreAPI_Dll AVFrameWrapper {
		struct DiscordCoreAPI_Dll AVFrameDeleter {
			inline void operator()(AVFrame* other) {
				if (other) {
					av_frame_unref(other);
					av_frame_free(&other);
				}
			}
		};

		inline AVFrameWrapper& operator=(AVFrame* other) {
			this->thePtr.reset(other);
			return *this;
		}

		inline AVFrame* operator->() {
			return this->thePtr.get();
		}

		inline operator AVFrame*() {
			return this->thePtr.get();
		}

		inline AVFrameWrapper(std::nullptr_t){};

	  protected:
		std::unique_ptr<AVFrame, AVFrameDeleter> thePtr{ nullptr, AVFrameDeleter{} };
	};

	struct DiscordCoreAPI_Dll AVCodecContextWrapper {
		struct DiscordCoreAPI_Dll AVCodecContextDeleter {
			inline void operator()(AVCodecContext* other) {
				if (other) {
					avcodec_free_context(&other);
				}
			}
		};

		inline AVCodecContextWrapper& operator=(AVCodecContext* other) {
			this->thePtr.reset(other);
			return *this;
		}

		inline AVCodecContext* operator->() {
			return this->thePtr.get();
		}

		inline operator AVCodecContext*() {
			return this->thePtr.get();
		}

		inline AVCodecContextWrapper(std::nullptr_t){};

	  protected:
		std::unique_ptr<AVCodecContext, AVCodecContextDeleter> thePtr{ nullptr, AVCodecContextDeleter{} };
	};

	struct DiscordCoreAPI_Dll AVFormatContextWrapper01 {
		AVFormatContextWrapper01() = default;

		AVFormatContext* theContext{ nullptr };
		bool didItInitialize{ false };
	};

	struct DiscordCoreAPI_Dll AVFormatContextWrapper {
		struct DiscordCoreAPI_Dll AVFormatContextDeleter {
			inline void operator()(AVFormatContextWrapper01* other) {
				if (other->didItInitialize) {
					avformat_close_input(&other->theContext);
				}
			}
		};

		inline AVFormatContextWrapper& operator=(AVFormatContext* other) {
			this->thePtr->theContext = other;
			return *this;
		}

		inline bool* getBoolPtr() {
			return &this->thePtr.get()->didItInitialize;
		}

		inline AVFormatContext* operator->() {
			return this->thePtr.get()->theContext;
		}

		inline AVFormatContext** operator*() {
			return &this->thePtr.get()->theContext;
		}

		inline operator AVFormatContext*() {
			return this->thePtr.get()->theContext;
		}

		inline AVFormatContextWrapper(std::nullptr_t){};

	  protected:
		std::unique_ptr<AVFormatContextWrapper01, AVFormatContextDeleter> thePtr{ new AVFormatContextWrapper01{}, AVFormatContextDeleter{} };
	};

	struct DiscordCoreAPI_Dll SwrContextWrapper {
		struct DiscordCoreAPI_Dll SwrContextDeleter {
			void operator()(SwrContext* other) {
				if (other) {
					swr_free(&other);
				}
			}
		};

		inline SwrContextWrapper& operator=(SwrContext* other) {
			this->thePtr.reset(other);
			return *this;
		}

		inline operator SwrContext*() {
			return this->thePtr.get();
		}

		inline SwrContextWrapper(std::nullptr_t){};

	  protected:
		std::unique_ptr<SwrContext, SwrContextDeleter> thePtr{ nullptr, SwrContextDeleter{} };
	};

	struct DiscordCoreAPI_Dll AVIOContextWrapper {
		struct DiscordCoreAPI_Dll AVIOContextDeleter {
			inline void operator()(AVIOContext* other) {
				if (other) {
					av_freep(&other);
				}
			}
		};

		AVIOContextWrapper& operator=(AVIOContext* other) {
			this->thePtr.reset(other);
			return *this;
		}

		AVIOContext* operator->() {
			return this->thePtr.get();
		}

		operator AVIOContext*() {
			return this->thePtr.get();
		}

		AVIOContextWrapper(std::nullptr_t){};

	  protected:
		std::unique_ptr<AVIOContext, AVIOContextDeleter> thePtr{ nullptr, AVIOContextDeleter{} };
	};

	struct DiscordCoreAPI_Dll AVPacketWrapper {
		struct DiscordCoreAPI_Dll AVPacketDeleter {
			inline void operator()(AVPacket* other) {
				if (other) {
					av_packet_free(&other);
				}
			}
		};

		inline AVPacketWrapper& operator=(AVPacket* other) {
			this->thePtr.reset(other);
			return *this;
		}

		inline AVPacket* operator->() {
			return this->thePtr.get();
		}

		inline operator AVPacket*() {
			return this->thePtr.get();
		}

		inline AVPacketWrapper(std::nullptr_t){};

	  protected:
		std::unique_ptr<AVPacket, AVPacketDeleter> thePtr{ nullptr, AVPacketDeleter{} };
	};

	struct DiscordCoreAPI_Dll AVCodecWrapper {
		struct DiscordCoreAPI_Dll AVCodecDeleter {
			inline void operator()(AVCodec*){};
		};

		inline AVCodecWrapper& operator=(AVCodec* other) {
			this->thePtr.reset(other);
			return *this;
		}

		inline operator AVCodec*() {
			return this->thePtr.get();
		}

		inline AVCodecWrapper(std::nullptr_t){};

	  protected:
		std::unique_ptr<AVCodec, AVCodecDeleter> thePtr{ nullptr, AVCodecDeleter{} };
	};

	struct DiscordCoreAPI_Dll AVStreamWrapper {
		struct DiscordCoreAPI_Dll AVStreamDeleter {
			inline void operator()(AVStream*){};
		};

		inline AVStreamWrapper& operator=(AVStream* other) {
			this->thePtr.reset(other);
			return *this;
		}

		inline AVStream* operator->() {
			return this->thePtr.get();
		}

		inline operator AVStream*() {
			return this->thePtr.get();
		}

		inline AVStreamWrapper(std::nullptr_t){};

	  protected:
		std::unique_ptr<AVStream, AVStreamDeleter> thePtr{ nullptr, AVStreamDeleter{} };
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

		void submitDataForDecoding(std::string dataToDecode);

		bool getFrame(DiscordCoreAPI::RawFrameData& dataPackage);

		bool haveWeFailed();

		void startMe();

		~AudioDecoder();

	  protected:
		int64_t audioStreamIndex{ 0 }, bufferMaxSize{ 0 }, bytesRead{ 0 }, totalFileSize{ 0 };
		DiscordCoreAPI::UnboundedMessageBlock<DiscordCoreAPI::RawFrameData> outDataBuffer{};
		DiscordCoreAPI::UnboundedMessageBlock<std::string> inputDataBuffer{};
		DiscordCoreAPI::ConfigManager* configManager{ nullptr };
		AVFrameWrapper frame{ nullptr }, newFrame{ nullptr };
		AVCodecContextWrapper audioDecodeContext{ nullptr };
		std::atomic_int32_t refreshTimeForBuffer{ 10000 };
		std::unique_ptr<std::jthread> theTask{ nullptr };
		AVFormatContextWrapper formatContext{ nullptr };
		std::atomic_bool haveWeFailedBool{ false };
		AVIOContextWrapper ioContext{ nullptr };
		SwrContextWrapper swrContext{ nullptr };
		AVStreamWrapper audioStream{ nullptr };
		AVPacketWrapper packet{ nullptr };
		AVCodecWrapper codec{ nullptr };
		std::string currentBuffer{};
		bool areWeQuitting{ false };
		bool haveWeBooted{ false };

		static int32_t FileStreamRead(void* opaque, uint8_t* buf, int32_t);

		void run(std::stop_token theToken);

		void cancelMe();
	};

}// namespace DiscordCoreAPI
