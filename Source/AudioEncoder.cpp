/*
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
/// AudioEncoder.cpp - Source file for the audio encoder class.
/// Aug 22, 2021
/// https://discordcoreapi.com
/// \file AudioEncoder.cpp

#include <discordcoreapi/AudioEncoder.hpp>

namespace DiscordCoreInternal {

	void OpusEncoderWrapper::OpusEncoderDeleter::operator()(OpusEncoder* other) {
		opus_encoder_destroy(other);
	}

	OpusEncoderWrapper& OpusEncoderWrapper::operator=(OpusEncoder* other) {
		this->thePtr.reset(other);
		return *this;
	}

	OpusEncoderWrapper::operator OpusEncoder*() {
		return this->thePtr.get();
	}

	AudioEncoder::AudioEncoder() {
		int32_t error;
		this->encoder = opus_encoder_create(this->sampleRate, this->nChannels, OPUS_APPLICATION_AUDIO, &error);
	}

	std::vector<DiscordCoreAPI::AudioFrameData> AudioEncoder::encodeFrames(std::vector<DiscordCoreAPI::RawFrameData> rawFrames) {
		std::vector<DiscordCoreAPI::AudioFrameData> newData{};
		newData.reserve(rawFrames.size());
		for (int32_t x = 0; x < rawFrames.size(); x++) {
			DiscordCoreAPI::AudioFrameData frameData{};
			frameData.type = DiscordCoreAPI::AudioFrameType::Encoded;
			frameData.encodedFrameData = encodeSingleAudioFrame(rawFrames[x]);
			newData.push_back(frameData);
		}
		rawFrames.clear();
		return newData;
	}

	DiscordCoreAPI::EncodedFrameData AudioEncoder::encodeSingleAudioFrame(DiscordCoreAPI::RawFrameData& inputFrame) {
		std::vector<opus_int16> newVector{};
		newVector.reserve(inputFrame.data.size() / 2);
		for (uint32_t x = 0; x < inputFrame.data.size() / 2; x++) {
			opus_int16 newValue{};
			newValue |= inputFrame.data[static_cast<uint64_t>(x) * 2] << 0;
			newValue |= inputFrame.data[static_cast<uint64_t>(x) * 2 + 1] << 8;
			newVector.push_back(newValue);
		}
		newVector.shrink_to_fit();
		std::vector<uint8_t> newBuffer{};
		newBuffer.resize(this->maxBufferSize);
		int32_t count = opus_encode(this->encoder, newVector.data(), inputFrame.sampleCount, newBuffer.data(), this->maxBufferSize);
		if (count <= 0 || count > newBuffer.size()) {
			return DiscordCoreAPI::EncodedFrameData();
		}
		DiscordCoreAPI::EncodedFrameData encodedFrame{};
		encodedFrame.data.insert(encodedFrame.data.begin(), newBuffer.begin(), newBuffer.begin() + count);
		encodedFrame.sampleCount = inputFrame.sampleCount;
		return encodedFrame;
	}
}