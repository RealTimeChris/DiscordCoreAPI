// AudioEncoder.hpp - Header for the audio encoder class.
// Aug 22, 2021
// Chris M.
// https://github.com/RealTimeChris

#ifndef AUDIO_ENCODER
#define AUDIO_ENCODER

#include "FoundationEntities.hpp"
#include <opus/opus.h>

namespace DiscordCoreAPI {

    struct OpusEncoderWrapper {

        struct OpusEncoderDeleter {
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

        OpusEncoderWrapper(nullptr_t) {};
    protected:
        std::unique_ptr<OpusEncoder, OpusEncoderDeleter> thePtr{ nullptr , OpusEncoderDeleter{} };
    };

	class AudioEncoder {
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
#endif