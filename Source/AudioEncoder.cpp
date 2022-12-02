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
/// AudioEncoder.cpp - Source file for the audio encoder class.
/// Aug 22, 2021
/// https://discordcoreapi.com
/// \file AudioEncoder.cpp

#include <discordcoreapi/AudioEncoder.hpp>
#include <opus/opus.h>

namespace DiscordCoreInternal {

	void OpusEncoderWrapper::OpusEncoderDeleter::operator()(OpusEncoder* other) noexcept {
		if (other) {
			opus_encoder_destroy(other);
			other = nullptr;
		}
	}

	OpusEncoderWrapper::OpusEncoderWrapper() {
		int32_t error{};
		this->ptr.reset(opus_encoder_create(this->sampleRate, this->nChannels, OPUS_APPLICATION_AUDIO, &error));
		auto result = opus_encoder_ctl(this->ptr.get(), OPUS_SET_SIGNAL(OPUS_SIGNAL_MUSIC));
		if (result != OPUS_OK) {
			throw DiscordCoreAPI::DCAException{ "Failed to set the Opus signal type, Reason: " + std::string{ opus_strerror(result) } };
		}
		result = opus_encoder_ctl(this->ptr.get(), OPUS_SET_APPLICATION(OPUS_APPLICATION_AUDIO));
		if (result != OPUS_OK) {
			throw DiscordCoreAPI::DCAException{ "Failed to set the Opus application type, Reason: " + std::string{ opus_strerror(result) } };
		}
		result = opus_encoder_ctl(this->ptr.get(), OPUS_SET_BITRATE(OPUS_BITRATE_MAX));
		if (result != OPUS_OK) {
			throw DiscordCoreAPI::DCAException{ "Failed to set the Opus bitrate, Reason: " + std::string{ opus_strerror(result) } };
		}
	}

	DiscordCoreAPI::AudioFrameData OpusEncoderWrapper::encodeData(DiscordCoreAPI::AudioFrameData& inputFrame) {
		if (inputFrame.data.size() == 0) {
			return {};
		}
		if (this->encodedData.size() == 0) {
			this->encodedData.resize(this->maxBufferSize);
		}
		int32_t count = opus_encode(this->ptr.get(), reinterpret_cast<opus_int16*>(inputFrame.data.data()), inputFrame.sampleCount,
			reinterpret_cast<uint8_t*>(this->encodedData.data()), this->maxBufferSize);
		if (count <= 0) {
			return {};
		}
		DiscordCoreAPI::AudioFrameData encodedFrame{};
		encodedFrame.data.insert(encodedFrame.data.begin(), this->encodedData.begin(), this->encodedData.begin() + count);
		encodedFrame.sampleCount = inputFrame.sampleCount;
		encodedFrame.type = DiscordCoreAPI::AudioFrameType::Encoded;
		encodedFrame.guildMemberId = inputFrame.guildMemberId;
		encodedFrame.currentSize = count;
		return encodedFrame;
	}
}