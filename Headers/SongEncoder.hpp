// SongEncoder.hpp - Header for the song encoder class.
// Aug 22, 2021
// Chris M.
// https://github.com/RealTimeChris

#pragma once

#include "IndexInitial.hpp"
#include "FoundationEntities.hpp"

namespace DiscordCoreAPI {

	class DiscordCoreAPI_Dll SongEncoder {
	public:

		SongEncoder();

		concurrent_vector<AudioFrameData> encodeFrames(concurrent_vector<RawFrameData> rawFrames);

		~SongEncoder();

	protected:
		OpusEncoder* encoder{ nullptr };
		const int32_t maxBufferSize{ 1276 };
		const int32_t sampleRate{ 48000 };
		const int32_t nChannels{ 2 };

		EncodedFrameData encodeSingleAudioFrame(RawFrameData inputFrame);

	};

}
