/*
*
	Copyright 2021, 2022 Chris M (RealTimeChris).

	This file is part of DiscordCoreAPI.
	DiscordCoreAPI is free software: you can redistribute it and/or modify it under the terms of the GNU
	General Public License as published by the Free Software Foundation, either version 3 of the License,
	or (at your option) any later version.
	DiscordCoreAPI is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without
	even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.
	You should have received a copy of the GNU General Public License along with DiscordCoreAPI.
	If not, see <https://www.gnu.org/licenses/>.

*/
/// AudioEncoder.hpp - Header for the audio encoder class.
/// Aug 22, 2021
/// Chris M.
/// https://discordcoreapi.com
/// \file AudioEncoder.hpp

#pragma once

#include <discordcoreapi/FoundationEntities.hpp>
#include <opus/opus.h>

namespace DiscordCoreAPI {

	struct DiscordCoreAPI_Dll OpusEncoderWrapper {
		struct DiscordCoreAPI_Dll OpusEncoderDeleter {
			void operator()(OpusEncoder* other) {
				opus_encoder_destroy(other);
			}
		};

		OpusEncoderWrapper& operator=(OpusEncoder* other) {
			this->thePtr.reset(other);
			return *this;
		}

		operator OpusEncoder*() {
			return this->thePtr.get();
		}

		OpusEncoderWrapper(nullptr_t other) {
			*this = other;
		}

	  protected:
		std::unique_ptr<OpusEncoder, OpusEncoderDeleter> thePtr{ nullptr, OpusEncoderDeleter{} };
	};

	class DiscordCoreAPI_Dll AudioEncoder {
	  public:
		AudioEncoder();

		std::vector<AudioFrameData> encodeFrames(std::vector<RawFrameData>& rawFrames);

	  protected:
		EncodedFrameData encodeSingleAudioFrame(RawFrameData& inputFrame);

		OpusEncoderWrapper encoder{ nullptr };
		const int32_t maxBufferSize{ 1276 };
		const int32_t sampleRate{ 48000 };
		const int32_t nChannels{ 2 };
	};

}// namespace DiscordCoreAPI
