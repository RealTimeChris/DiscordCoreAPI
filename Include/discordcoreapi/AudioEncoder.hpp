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
/// AudioEncoder.hpp - Header for the audio encoder class.
/// Aug 22, 2021
/// https://discordcoreapi.com
/// \file AudioEncoder.hpp

#ifndef AUDIO_ENCODER
#define AUDIO_ENCODER

#include <discordcoreapi/FoundationEntities.hpp>
#include <opus/opus.h>

namespace DiscordCoreAPI {

	struct DiscordCoreAPI_Dll OpusEncoderWrapper {
		struct DiscordCoreAPI_Dll OpusEncoderDeleter {
			void operator()(OpusEncoder*) noexcept;
		};

		OpusEncoderWrapper& operator=(OpusEncoderWrapper&&) noexcept;

		OpusEncoderWrapper(OpusEncoderWrapper&&) noexcept;

		OpusEncoderWrapper(OpusEncoder*) noexcept;

		OpusEncoderWrapper() noexcept;

		operator OpusEncoder*() noexcept;

	  protected:
		std::unique_ptr<OpusEncoder, OpusEncoderDeleter> thePtr{ nullptr, OpusEncoderDeleter{} };
	};

	class DiscordCoreAPI_Dll AudioEncoder {
	  public:
		AudioEncoder();

		DiscordCoreAPI::AudioFrameData encodeSingleAudioFrame(DiscordCoreAPI::AudioFrameData& inputFrame);

		DiscordCoreAPI::AudioFrameData encodeSingleAudioFrame(std::vector<opus_int16>& inputFrame);

	  protected:
		const Int32 maxBufferSize{ 1276 };
		const Int32 sampleRate{ 48000 };
		OpusEncoderWrapper encoder{};
		const Int32 nChannels{ 2 };
	};

}// namespace DiscordCoreAPI
#endif