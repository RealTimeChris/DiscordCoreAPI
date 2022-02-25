/*
*    # License

   Copyright 2021 Chris M.

   Licensed under the Apache License, Version 2.0 (the "License");
   you may not use this file except in compliance with the License.
   You may obtain a copy of the License at

	   http://www.apache.org/licenses/LICENSE-2.0

   Unless required by applicable law or agreed to in writing, software
   distributed under the License is distributed on an "AS IS" BASIS,
   WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
   See the License for the specific language governing permissions and
   limitations under the License.
*/
/// AudioEncoder.cpp - Source file for the audio encoder class.
/// Aug 22, 2021
/// Chris M.
/// https://github.com/RealTimeChris/DiscordCoreAPI
/// \file AudioEncoder.cpp

#include <AudioEncoder.hpp>

namespace DiscordCoreAPI {

	AudioEncoder::AudioEncoder() {
		int32_t error;
		this->encoder = opus_encoder_create(this->sampleRate, this->nChannels, OPUS_APPLICATION_AUDIO, &error);
		if (error != OPUS_OK) {
			std::cout << "Failed to create Opus encoder!";
		}
	}

	std::vector<AudioFrameData> AudioEncoder::encodeFrames(std::vector<RawFrameData>& rawFrames) {
		std::vector<AudioFrameData> newData{};
		newData.reserve(rawFrames.size());
		for (int32_t x = 0; x < rawFrames.size(); x += 1) {
			AudioFrameData frameData{};
			frameData.type = AudioFrameType::Encoded;
			frameData.encodedFrameData = encodeSingleAudioFrame(rawFrames[x]);
			newData.push_back(frameData);
		}
		rawFrames.clear();
		return newData;
	}

	EncodedFrameData AudioEncoder::encodeSingleAudioFrame(RawFrameData& inputFrame) {
		std::vector<opus_int16> newVector{};
		newVector.reserve(inputFrame.data.size() / 2);
		for (uint32_t x = 0; x < inputFrame.data.size() / 2; x += 1) {
			opus_int16 newValue{};
			newValue |= inputFrame.data[static_cast<uint64_t>(x) * 2] << 0;
			newValue |= inputFrame.data[static_cast<uint64_t>(x) * 2 + 1] << 8;
			newVector.push_back(newValue);
		}
		newVector.shrink_to_fit();
		std::vector<uint8_t> newBuffer{};
		newBuffer.reserve(this->maxBufferSize);
		int32_t count = opus_encode(this->encoder, newVector.data(), inputFrame.sampleCount, newBuffer.data(), this->maxBufferSize);
		if (count <= 0) {
			return EncodedFrameData();
		}
		EncodedFrameData encodedFrame{};
		encodedFrame.data.insert(encodedFrame.data.begin(), newBuffer.begin(), newBuffer.begin() + count);
		encodedFrame.sampleCount = inputFrame.sampleCount;
		return encodedFrame;
	}
}