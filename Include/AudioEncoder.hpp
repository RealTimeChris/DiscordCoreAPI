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
/// AudioEncoder.hpp - Header for the audio encoder class DiscordCoreAPI_Dll.
/// Aug 22, 2021
/// Chris M.
/// https://github.com/RealTimeChris/DiscordCoreAPI

#pragma once

#include <FoundationEntities>
#include <opus/opus.h>
#include <iostream>

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
        std::unique_ptr<OpusEncoder, OpusEncoderDeleter> thePtr{ nullptr , OpusEncoderDeleter{} };
    };

	class DiscordCoreAPI_Dll AudioEncoder {
	public:

        AudioEncoder();

        std::vector<AudioFrameData> encodeFrames(std::vector<RawFrameData>& rawFrames);

        EncodedFrameData encodeSingleAudioFrame(RawFrameData& inputFrame);

	protected:

		OpusEncoderWrapper encoder{ nullptr };
		const int32_t maxBufferSize{ 1276 };
		const int32_t sampleRate{ 48000 };
		const int32_t nChannels{ 2 };

	};

}
