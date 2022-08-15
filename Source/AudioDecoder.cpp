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
/// AudioDecoder.cpp - Source file for the audio decoder class.
/// Jul 29, 2021
/// https://discordcoreapi.com
/// \file AudioDecoder.cpp

#include <discordcoreapi/AudioDecoder.hpp>

namespace DiscordCoreInternal {

	void AVFrameWrapper::AVFrameDeleter::operator()(AVFrame* other) {
		if (other) {
			av_frame_unref(other);
			av_frame_free(&other);
		}
	}

	AVFrameWrapper& AVFrameWrapper::operator=(AVFrame* other) {
		this->thePtr.reset(other);
		return *this;
	}

	AVFrameWrapper::AVFrameWrapper(AVFrame* other) {
		*this = other;
	}

	AVFrame* AVFrameWrapper::operator->() {
		return this->thePtr.get();
	}

	AVFrameWrapper::operator AVFrame*() {
		return this->thePtr.get();
	}

	void AVCodecContextWrapper::AVCodecContextDeleter::operator()(AVCodecContext* other) {
		if (other) {
			avcodec_free_context(&other);
		}
	}

	AVCodecContextWrapper& AVCodecContextWrapper::operator=(AVCodecContext* other) {
		this->thePtr.reset(other);
		return *this;
	}

	AVCodecContextWrapper::AVCodecContextWrapper(AVCodecContext* other) {
		*this = other;
	}

	AVCodecContext* AVCodecContextWrapper::operator->() {
		return this->thePtr.get();
	}

	AVCodecContextWrapper::operator AVCodecContext*() {
		return this->thePtr.get();
	}

	void AVFormatContextWrapper::AVFormatContextDeleter::operator()(AVFormatContextWrapper01* other) {
		if (other->didItInitialize) {
			avformat_free_context(other->theContext);
		}
	}

	AVFormatContextWrapper& AVFormatContextWrapper::operator=(AVFormatContext* other) {
		this->thePtr->theContext = other;
		return *this;
	}

	AVFormatContextWrapper::AVFormatContextWrapper(AVFormatContext* other) {
		*this = other;
	}

	bool* AVFormatContextWrapper::getBoolPtr() {
		return &this->thePtr.get()->didItInitialize;
	}

	AVFormatContext* AVFormatContextWrapper::operator->() {
		return this->thePtr.get()->theContext;
	}

	AVFormatContext** AVFormatContextWrapper::operator*() {
		return &this->thePtr.get()->theContext;
	}

	AVFormatContextWrapper::operator AVFormatContext*() {
		return this->thePtr.get()->theContext;
	}

	void SwrContextWrapper::SwrContextDeleter::operator()(SwrContext* other) {
		if (other) {
			swr_free(&other);
		}
	}

	SwrContextWrapper& SwrContextWrapper::operator=(SwrContext* other) {
		this->thePtr.reset(other);
		return *this;
	}

	SwrContextWrapper::SwrContextWrapper(SwrContext* other) {
		*this = other;
	}

	SwrContextWrapper::operator SwrContext*() {
		return this->thePtr.get();
	}

	void AVIOContextWrapper::AVIOContextDeleter::operator()(AVIOContext* other) {
		if (other) {
			if (other->buffer) {
				av_freep(&other->buffer);
			}
			avio_context_free(&other);
		}
	}

	AVIOContextWrapper& AVIOContextWrapper::operator=(AVIOContext* other) {
		this->thePtr.reset(other);
		return *this;
	}

	AVIOContextWrapper::AVIOContextWrapper(AVIOContext* other) {
		*this = other;
	}

	AVIOContext* AVIOContextWrapper::operator->() {
		return this->thePtr.get();
	}

	AVIOContextWrapper::operator AVIOContext*() {
		return this->thePtr.get();
	}

	void AVPacketWrapper::AVPacketDeleter::operator()(AVPacket* other) {
		if (other) {
			av_packet_free(&other);
		}
	}

	AVPacketWrapper& AVPacketWrapper::operator=(AVPacket* other) {
		this->thePtr.reset(other);
		return *this;
	}

	AVPacketWrapper::AVPacketWrapper(AVPacket* other) {
		*this = other;
	}

	AVPacket* AVPacketWrapper::operator->() {
		return this->thePtr.get();
	}

	AVPacketWrapper::operator AVPacket*() {
		return this->thePtr.get();
	}

	AudioDecoder::AudioDecoder(const BuildAudioDecoderData& dataPackage) {
		this->configManager = dataPackage.configManager;
		this->bufferMaxSize = dataPackage.bufferMaxSize;
		this->totalFileSize = dataPackage.totalFileSize;
	}

	bool AudioDecoder::getFrame(DiscordCoreAPI::AudioFrameData& dataPackage) {
		if (!this->areWeQuitting.load()) {
			if (this->outDataBuffer.tryReceive(dataPackage)) {
				if (dataPackage.sampleCount != -1) {
					return true;
				}
			}
		}
		return false;
	}

	void AudioDecoder::submitDataForDecoding(std::string dataToDecode) {
		this->inputDataBuffer.send(std::move(dataToDecode));
	}

	bool AudioDecoder::haveWeFailed() {
		return this->haveWeFailedBool.load();
	}

	void AudioDecoder::startMe() {
		this->taskThread = std::make_unique<std::jthread>([=, this](std::stop_token stopToken) {
			this->run(stopToken);
		});
	};

	int32_t AudioDecoder::FileStreamRead(void* opaque, uint8_t* buf, int32_t) {
		AudioDecoder* stream = static_cast<AudioDecoder*>(opaque);
		stream->bytesRead = 0;
		stream->currentBuffer = std::string();
		DiscordCoreAPI::AudioFrameData frameData{};
		if (stream->areWeQuitting.load()) {
			frameData.sampleCount = -5;
			stream->outDataBuffer.send(std::move(frameData));
			stream->areWeQuitting.store(true);
			return AVERROR_EOF;
		}
		if (DiscordCoreAPI::waitForTimeToPass(stream->inputDataBuffer, stream->currentBuffer, stream->refreshTimeForBuffer.load())) {
			frameData.sampleCount = -5;
			stream->outDataBuffer.send(std::move(frameData));
			stream->areWeQuitting.store(true);
			return AVERROR_EOF;
		}
		if (stream->currentBuffer.size() > 0) {
			stream->bytesRead = stream->currentBuffer.size();
		} else {
			frameData.sampleCount = -5;
			stream->outDataBuffer.send(std::move(frameData));
			stream->areWeQuitting.store(true);
			return AVERROR_EOF;
		}
		for (int32_t x = 0; x < stream->bytesRead; x++) {
			buf[x] = stream->currentBuffer[x];
		}
		if (stream->ioContext->buf_ptr - stream->ioContext->buffer >= stream->totalFileSize) {
			frameData.sampleCount = -5;
			stream->outDataBuffer.send(std::move(frameData));
			stream->areWeQuitting.store(true);
			return static_cast<int32_t>(stream->bytesRead);
		}
		return static_cast<int32_t>(stream->bytesRead);
	}

	void AudioDecoder::run(std::stop_token stopToken) {
		if (!this->haveWeBooted) {
			auto theBuffer = static_cast<uint8_t*>(av_malloc(this->bufferMaxSize));
			if (theBuffer == nullptr) {
				this->haveWeFailedBool.store(true);
				if (this->configManager->doWePrintFFMPEGErrorMessages()) {
					cout << DiscordCoreAPI::shiftToBrightRed() << "AudioDecoder::run() Error: Failed to allocate filestreambuffer." << DiscordCoreAPI::reset() << endl << endl;
				}
				return;
			}

			this->ioContext = avio_alloc_context(theBuffer, static_cast<int32_t>(this->bufferMaxSize - 1), 0, this, &AudioDecoder::FileStreamRead, 0, 0);

			if (this->ioContext == nullptr) {
				this->haveWeFailedBool.store(true);
				if (this->configManager->doWePrintFFMPEGErrorMessages()) {
					cout << DiscordCoreAPI::shiftToBrightRed() << "AudioDecoder::run() Error: Failed to allocate AVIOContext." << DiscordCoreAPI::reset() << endl << endl;
				}
				return;
			}

			this->formatContext = avformat_alloc_context();

			if (!this->formatContext) {
				this->haveWeFailedBool.store(true);
				if (this->configManager->doWePrintFFMPEGErrorMessages()) {
					cout << DiscordCoreAPI::shiftToBrightRed() << "AudioDecoder::run() Error: Could not allocate the format context." << DiscordCoreAPI::reset() << endl << endl;
				}
				return;
			}

			this->formatContext->pb = this->ioContext;
			this->formatContext->flags |= AVFMT_FLAG_CUSTOM_IO;
			if (avformat_open_input(*this->formatContext, "memory", nullptr, nullptr) < 0) {
				this->haveWeFailedBool.store(true);
				if (this->configManager->doWePrintFFMPEGErrorMessages()) {
					cout << DiscordCoreAPI::shiftToBrightRed() << "AudioDecoder::run() Error: Failed to open the AVFormatContext." << DiscordCoreAPI::reset() << endl << endl;
				}
				return;
			}
			*this->formatContext.getBoolPtr() = true;
			AVMediaType type = AVMediaType::AVMEDIA_TYPE_AUDIO;
			int32_t ret = av_find_best_stream(this->formatContext, type, -1, -1, NULL, 0);
			if (ret < 0) {
				std::string newString = "AudioDecoder::run() Error: Could not find ";
				newString += av_get_media_type_string(type);
				newString += " stream in input memory stream.";
				this->haveWeFailedBool.store(true);
				if (this->configManager->doWePrintFFMPEGErrorMessages()) {
					cout << DiscordCoreAPI::shiftToBrightRed() << newString << DiscordCoreAPI::reset() << endl << endl;
				}
				return;
			} else {
				this->audioStreamIndex = ret;
				this->audioStream = this->formatContext->streams[this->audioStreamIndex];
				if (!this->audioStream) {
					this->haveWeFailedBool.store(true);
					if (this->configManager->doWePrintFFMPEGErrorMessages()) {
						cout << DiscordCoreAPI::shiftToBrightRed() << "AudioDecoder::run() Error: Could not find an audio stream." << DiscordCoreAPI::reset() << endl << endl;
					}
					return;
				}

				if (avformat_find_stream_info(this->formatContext, NULL) < 0) {
					this->haveWeFailedBool.store(true);
					if (this->configManager->doWePrintFFMPEGErrorMessages()) {
						cout << DiscordCoreAPI::shiftToBrightRed() << "AudioDecoder::run() Error: Could not find stream information." << DiscordCoreAPI::reset() << endl << endl;
					}
					return;
				}

				this->codec = avcodec_find_decoder(this->audioStream->codecpar->codec_id);
				if (!this->codec) {
					std::string newString = "AudioDecoder::run() Error: Failed to find ";
					newString += av_get_media_type_string(type);
					newString += " decoder.";
					this->haveWeFailedBool.store(true);
					if (this->configManager->doWePrintFFMPEGErrorMessages()) {
						cout << DiscordCoreAPI::shiftToBrightRed() << newString << DiscordCoreAPI::reset() << endl << endl;
					}
					return;
				}

				this->audioDecodeContext = avcodec_alloc_context3(this->codec);
				if (!this->audioDecodeContext) {
					std::string newString = "AudioDecoder::run() Error: Failed to allocate the ";
					newString += av_get_media_type_string(type);
					newString += " AVCodecContext.";
					this->haveWeFailedBool.store(true);
					if (this->configManager->doWePrintFFMPEGErrorMessages()) {
						cout << DiscordCoreAPI::shiftToBrightRed() << newString << DiscordCoreAPI::reset() << endl << endl;
					}
					return;
				}

				if (avcodec_parameters_to_context(this->audioDecodeContext, this->audioStream->codecpar) < 0) {
					std::string newString = "AudioDecoder::run() Error: Failed to copy ";
					newString += av_get_media_type_string(type);
					newString += " codec parameters to decoder context.";
					this->haveWeFailedBool.store(true);
					if (this->configManager->doWePrintFFMPEGErrorMessages()) {
						cout << DiscordCoreAPI::shiftToBrightRed() << newString << DiscordCoreAPI::reset() << endl << endl;
					}
					return;
				}

				if (avcodec_open2(this->audioDecodeContext, this->codec, NULL) < 0) {
					std::string newString = "AudioDecoder::run() Error: Failed to open ";
					newString += av_get_media_type_string(type);
					newString += " AVCodecContext.";
					this->haveWeFailedBool.store(true);
					if (this->configManager->doWePrintFFMPEGErrorMessages()) {
						cout << DiscordCoreAPI::shiftToBrightRed() << newString << DiscordCoreAPI::reset() << endl << endl;
					}
					return;
				}

				this->swrContext = swr_alloc_set_opts(NULL, AV_CH_LAYOUT_STEREO, AVSampleFormat::AV_SAMPLE_FMT_S16, 48000, AV_CH_LAYOUT_STEREO,
					this->audioDecodeContext->sample_fmt, this->audioDecodeContext->sample_rate, 0, nullptr);
				swr_init(this->swrContext);
				if (this->configManager->doWePrintFFMPEGSuccessMessages()) {
					av_dump_format(this->formatContext, 0, "memory", 0);
				}
			}

			this->haveWeBooted = true;
		}

		if (this->currentBuffer.size() > 0) {
			this->packet = av_packet_alloc();
			if (!this->packet) {
				this->haveWeFailedBool.store(true);
				if (this->configManager->doWePrintFFMPEGErrorMessages()) {
					cout << DiscordCoreAPI::shiftToBrightRed() << "AudioDecoder::run() Error: Could not allocate packet" << DiscordCoreAPI::reset() << endl << endl;
				}
				return;
			}

			this->frame = av_frame_alloc();
			if (!this->frame) {
				this->haveWeFailedBool.store(true);
				if (this->configManager->doWePrintFFMPEGErrorMessages()) {
					cout << DiscordCoreAPI::shiftToBrightRed() << "AudioDecoder::run() Error: Could not allocate frame" << DiscordCoreAPI::reset() << endl << endl;
				}
				return;
			}

			this->newFrame = av_frame_alloc();
			if (!this->newFrame) {
				this->haveWeFailedBool.store(true);
				if (this->configManager->doWePrintFFMPEGErrorMessages()) {
					cout << DiscordCoreAPI::shiftToBrightRed() << "AudioDecoder::run() Error: Could not allocate new-frame" << DiscordCoreAPI::reset() << endl << endl;
				}
				return;
			}

			while (!stopToken.stop_requested() && !this->areWeQuitting.load() && av_read_frame(this->formatContext, this->packet) == 0) {
				if (this->packet->stream_index == this->audioStreamIndex) {
					int32_t returnValue = avcodec_send_packet(this->audioDecodeContext, this->packet);
					if (returnValue < 0) {
						char charString[32];
						av_strerror(returnValue, charString, 32);
						std::string newString = "AudioDecoder::run() Error: Issue submitting a packet for decoding (" + std::to_string(returnValue) + "), " + charString + ".";
						this->haveWeFailedBool.store(true);
						if (this->configManager->doWePrintFFMPEGErrorMessages()) {
							cout << DiscordCoreAPI::shiftToBrightRed() << newString << DiscordCoreAPI::reset() << endl << endl;
						}
						return;
					}
					if (returnValue >= 0) {
						returnValue = avcodec_receive_frame(this->audioDecodeContext, this->frame);
						if (returnValue < 0) {
							std::string newString = "AudioDecoder::run() Error: Issue during decoding (" + std::to_string(returnValue) + ")";
							this->haveWeFailedBool.store(true);
							if (this->configManager->doWePrintFFMPEGErrorMessages()) {
								cout << DiscordCoreAPI::shiftToBrightRed() << newString << DiscordCoreAPI::reset() << endl << endl;
							}
							return;
						}

						if (!swr_is_initialized(this->swrContext)) {
							swr_init(this->swrContext);
						}
						this->newFrame->channel_layout = AV_CH_LAYOUT_STEREO;
						this->newFrame->sample_rate = 48000;
						this->newFrame->format = AVSampleFormat::AV_SAMPLE_FMT_S16;
						this->newFrame->nb_samples = frame->nb_samples;
						this->newFrame->pts = frame->pts;
						swr_convert_frame(this->swrContext, this->newFrame, this->frame);
						int32_t unpadded_linesize = this->newFrame->nb_samples * av_get_bytes_per_sample(static_cast<AVSampleFormat>(this->newFrame->format)) * 2;
						DiscordCoreAPI::AudioFrameData rawFrame{};
						rawFrame.type = DiscordCoreAPI ::AudioFrameType::RawPCM;
						rawFrame.data.resize(unpadded_linesize);
						for (int32_t x = 0; x < unpadded_linesize; x++) {
							rawFrame.data[x] = this->newFrame->extended_data[0][x];
						}
						rawFrame.sampleCount = newFrame->nb_samples;
						this->outDataBuffer.send(std::move(rawFrame));
						int64_t sampleCount = swr_get_delay(this->swrContext, this->newFrame->sample_rate);
						if (sampleCount > 0) {
							if (!swr_is_initialized(this->swrContext)) {
								swr_init(this->swrContext);
							}
							swr_convert_frame(this->swrContext, this->newFrame, nullptr);
							DiscordCoreAPI::AudioFrameData rawFrame02{};
							rawFrame02.type = DiscordCoreAPI ::AudioFrameType::RawPCM;
							rawFrame02.data.resize(*this->newFrame->linesize);
							for (int32_t x = 0; x < *this->newFrame->linesize; x++) {
								rawFrame02.data[x] = this->newFrame->extended_data[0][x];
							}
							rawFrame02.sampleCount = newFrame->nb_samples;
							this->outDataBuffer.send(std::move(rawFrame02));
						}
						if (returnValue < 0 || newFrame->nb_samples == 0) {
							this->haveWeFailedBool.store(true);
							if (this->configManager->doWePrintFFMPEGErrorMessages()) {
								cout << DiscordCoreAPI::shiftToBrightRed() << "AudioDecoder::run() Error: Return value is less than zero!" << DiscordCoreAPI::reset() << endl
									 << endl;
							}
							return;
						}
					} else {
						break;
					}
				} else {
					break;
				}
				this->frame = av_frame_alloc();
				this->newFrame = av_frame_alloc();
				this->packet = av_packet_alloc();
				if (stopToken.stop_requested() || this->areWeQuitting.load()) {
					break;
				}
			}
			if (this->configManager->doWePrintFFMPEGSuccessMessages()) {
				cout << DiscordCoreAPI::shiftToBrightGreen() << "Completed decoding!" << endl << DiscordCoreAPI::reset() << endl << endl;
			}
		}
		return;
	}

	void AudioDecoder::cancelMe() {
		this->refreshTimeForBuffer.store(10);
		this->inputDataBuffer.clearContents();
		this->inputDataBuffer.send(std::string());
		this->inputDataBuffer.send(std::string());
		this->inputDataBuffer.send(std::string());
		this->inputDataBuffer.send(std::string());
		this->inputDataBuffer.send(std::string());
		this->areWeQuitting.store(true);
	}

	AudioDecoder::~AudioDecoder() {
		this->cancelMe();
	}

};