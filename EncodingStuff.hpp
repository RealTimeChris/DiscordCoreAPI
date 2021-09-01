// EncodingStuff.hpp - Header for the "encoding stuff".
// Aug 22, 2021
// Chris M.
// https://github.com/RealTimeChris

#pragma once

#ifndef _ENCODING_STUFF_
#define _ENCODING_STUFF_

#include "../pch.h"
#include "DiscordCoreClient02.hpp"

namespace DiscordCoreAPI {

	class SongEncoder {
	public:
        SongEncoder() {
			int error;
			this->encoder = opus_encoder_create(this->sampleRate, this->nChannels, OPUS_APPLICATION_AUDIO, &error);
			if (error != OPUS_OK) {
				cout << "Failed to create Opus encoder!";
			}
        }

		vector<AudioFrameData> encodeFrames(vector<RawFrameData> rawFrames) {
			vector<AudioFrameData> newData;
			for (int x = 0; x < rawFrames.size(); x += 1) {
				AudioFrameData frameData;
				frameData.type = AudioFrameType::Encoded;
				frameData.frameStatus = FrameStatus::Running;
				frameData.encodedFrameData = encodeSingleAudioFrame(rawFrames[x]);
				newData.push_back(frameData);
			}
			rawFrames.clear();
			return newData;
		}

		~SongEncoder() {
			opus_encoder_destroy(this->encoder);
		}

	protected:
		OpusEncoder* encoder{ nullptr };
		const int nChannels{ 2 };
		const int maxBufferSize{ 1276 };
		const int sampleRate{ 48000 };

		EncodedFrameData encodeSingleAudioFrame(RawFrameData inputFrame) {
			int sampleCount = inputFrame.sampleCount;
			uint8_t* oldBuffer = new uint8_t[inputFrame.data.size()];
			for (int x = 0; x < inputFrame.data.size(); x += 1) {
				oldBuffer[x] = inputFrame.data[x];
			}
			
			uint8_t* newBuffer = new uint8_t[this->maxBufferSize];
			
			int count = opus_encode_float(this->encoder, (float*)oldBuffer, sampleCount, newBuffer, this->maxBufferSize);
			vector<uint8_t> newVector{};
			for (int x = 0; x < count; x += 1) {
				newVector.push_back(newBuffer[x]);
			}
			EncodedFrameData encodedFrame;
			encodedFrame.data = newVector;
			encodedFrame.sampleCount = sampleCount;
			delete oldBuffer;
			oldBuffer = nullptr;
			delete newBuffer;
			newBuffer = nullptr;
			return encodedFrame;
		}
	};

}

#endif