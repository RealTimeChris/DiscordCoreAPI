// AudioEncoder.hpp - Header for the audio encoder class.
// Aug 22, 2021
// Chris M.
// https://github.com/RealTimeChris

#pragma once

#include "IndexInitial.hpp"
#include "FoundationEntities.hpp"

namespace DiscordCoreAPI {

	class DiscordCoreAPI_Dll AudioEncoder {
	public:

		AudioEncoder();

		vector<AudioFrameData> encodeFrames(vector<RawFrameData>& rawFrames);

		EncodedFrameData encodeSingleAudioFrame(RawFrameData& inputFrame);

	protected:

		OpusEncoderWrapper encoder{ nullptr };
		const int32_t maxBufferSize{ 1276 };
		const int32_t sampleRate{ 48000 };
		const int32_t nChannels{ 2 };

	};

}
