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

    struct BuildSongEncoderData {
    public:
		BuildSongEncoderData() {};
		vector<RawFrameData> rawFrames;
		size_t totalFileSize = 0;
    };

	class SongEncoder {
	public:
        SongEncoder(BuildSongEncoderData dataPackage) {
			this->rawFrames = dataPackage.rawFrames;
			int error;
			this->encoder = opus_encoder_create(48000, 2, OPUS_APPLICATION_AUDIO, &error);
			if (error != OPUS_OK) {
				cout << "Failed to create Opus encoder!";
			}
        }

		AudioFrameData encodeSong() {
			vector<EncodedFrameData> newVector;
			for (int x = 0; x < this->rawFrames.size(); x += 1) {
				newVector.push_back(encodeSingleAudioFrame(this->rawFrames[x]));
			}
			this->rawFrames.clear();
			AudioFrameData newData;
			newData.type = AudioFrameType::Encoded;
			newData.encodedFrameData = newVector;
			return newData;
		}

		~SongEncoder() {
			opus_encoder_destroy(this->encoder);
		}

	protected:
		vector<RawFrameData> rawFrames;
		int nChannels = 2;
		OpusEncoder* encoder;
		int frameSize = 960;

		EncodedFrameData encodeSingleAudioFrame(RawFrameData inputFrame) {
			uint8_t* oldBuffer;
			oldBuffer = new uint8_t[inputFrame.data.size()];
			for (int x = 0; x < inputFrame.data.size(); x += 1) {
				oldBuffer[x] = inputFrame.data[x];
			}
			uint8_t* newBuffer;
			int bufferSize = 1276;
			newBuffer = new uint8_t[bufferSize];

			int count = opus_encode_float(this->encoder, (float*)oldBuffer, inputFrame.sampleCount, newBuffer, bufferSize);
			vector<uint8_t> newVector{};
			for (int x = 0; x < count; x += 1) {
				newVector.push_back(newBuffer[x]);
			}
			EncodedFrameData encodedFrame;
			encodedFrame.data = newVector;
			encodedFrame.sampleCount = inputFrame.sampleCount;
			delete oldBuffer;
			oldBuffer = nullptr;
			delete newBuffer;
			newBuffer = nullptr;
			return encodedFrame;
		}
	};

}

#endif