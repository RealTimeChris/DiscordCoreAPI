// SongEncoder.hpp - Header for the song encoder class.
// Aug 22, 2021
// Chris M.
// https://github.com/RealTimeChris

#pragma once

#ifndef _SONG_ENCODER_
#define _SONG_ENCODER_

#include "IndexInitial.hpp"

namespace DiscordCoreAPI {

	class DiscordCoreAPI_Dll SongEncoder {
	public:

		SongEncoder() {
			__int32 error;
			this->encoder = opus_encoder_create(this->sampleRate, this->nChannels, OPUS_APPLICATION_AUDIO, &error);
			if (error != OPUS_OK) {
				cout << "Failed to create Opus encoder!";
			}
		}

		vector<AudioFrameData> encodeFrames(vector<RawFrameData> rawFrames) {
			vector<AudioFrameData> newData;
			for (__int32 x = 0; x < rawFrames.size(); x += 1) {
				AudioFrameData frameData;
				frameData.type = AudioFrameType::Encoded;
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
		const __int32 maxBufferSize{ 1276 };
		const __int32 sampleRate{ 48000 };
		const __int32 nChannels{ 2 };

		EncodedFrameData encodeSingleAudioFrame(RawFrameData inputFrame) {
			vector<unsigned __int8> oldBuffer{};

			for (__int32 x = 0; x < inputFrame.data.size(); x += 1) {
				oldBuffer.push_back(inputFrame.data[x]);
			}

			vector<unsigned __int8> newBuffer{};
			newBuffer.resize(this->maxBufferSize);

			__int32 count = opus_encode_float(this->encoder, (float*)oldBuffer.data(), inputFrame.sampleCount, newBuffer.data(), this->maxBufferSize);
			EncodedFrameData encodedFrame{};
			for (__int32 x = 0; x < count; x += 1) {
				encodedFrame.data.push_back(newBuffer[x]);
			}
			encodedFrame.sampleCount = inputFrame.sampleCount;
			return encodedFrame;
		}

	};

}

#endif