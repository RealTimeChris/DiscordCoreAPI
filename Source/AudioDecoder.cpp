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
/// AudioDecoder.cpp - Source file for the audio decoder class.
/// Jul 29, 2021
/// https://discordcoreapi.com
/// \file AudioDecoder.cpp

#include <discordcoreapi/Utilities.hpp>
#include <discordcoreapi/AudioDecoder.hpp>
#include <opus/opus.h>

namespace DiscordCoreInternal {

	void OpusDecoderWrapper::OpusDecoderDeleter::operator()(OpusDecoder* other) noexcept {
		if (other) {
			opus_decoder_destroy(other);
			other = nullptr;
		}
	}

	OpusDecoderWrapper::OpusDecoderWrapper() {
		int32_t error{};
		data.resize(23040);
		ptr.reset(opus_decoder_create(48000, 2, &error));
		if (error != OPUS_OK) {
			throw DiscordCoreAPI::DCAException{ "Failed to create the Opus decoder, Reason: " + std::string{ opus_strerror(error) } };
		}
	}

	std::basic_string_view<opus_int16> OpusDecoderWrapper::decodeData(std::basic_string_view<uint8_t> dataToDecode) {
		const int64_t sampleCount =
			opus_decode(ptr.get(), dataToDecode.data(), static_cast<opus_int32>(dataToDecode.length() & 0x7FFFFFFF), data.data(), 5760, 0);
		if (sampleCount > 0) {
			return std::basic_string_view<opus_int16>{ data.data(), static_cast<size_t>(sampleCount * 2ull) };
		} else {
			throw DiscordCoreAPI::DCAException{ "Failed to decode a user's voice payload, Reason: " + std::string{ opus_strerror(sampleCount) } };
		}
	}
}
