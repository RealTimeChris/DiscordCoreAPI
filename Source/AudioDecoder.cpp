/*
*
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

	AudioDecoder::AudioDecoder(const BuildAudioDecoderData& dataPackage) {
		this->doWePrintSuccessMessages = dataPackage.doWePrintSuccessMessages;
		this->doWePrintErrorMessages = dataPackage.doWePrintErrorMessages;
		this->bufferMaxSize = dataPackage.bufferMaxSize;
		this->totalFileSize = dataPackage.totalFileSize;
	}

	void AudioDecoder::submitDataForDecoding(std::string dataToDecode) {
		this->inputDataBuffer.send(dataToDecode);
	}

	bool AudioDecoder::getFrame(DiscordCoreAPI::RawFrameData& dataPackage) {
		if (!this->areWeQuitting) {
			if (this->outDataBuffer.tryReceive(dataPackage)) {
				if (dataPackage.sampleCount != -1) {
					return true;
				}
			}
		}
		return false;
	}

	void AudioDecoder::cancelMe() {
		this->refreshTimeForBuffer.store(10);
		this->inputDataBuffer.clearContents();
		this->inputDataBuffer.send(std::string());
		this->inputDataBuffer.send(std::string());
		this->inputDataBuffer.send(std::string());
		this->inputDataBuffer.send(std::string());
		this->inputDataBuffer.send(std::string());
		this->areWeQuitting = true;
		if (this->theTask) {
			this->theTask->request_stop();
			if (this->theTask->joinable()) {
				this->theTask->join();
			}
		}
	}

	bool AudioDecoder::haveWeFailed() {
		return this->haveWeFailedBool.load();
	}

	void AudioDecoder::startMe() {
		this->theTask = std::make_unique<std::jthread>([=, this](std::stop_token theToken) {
			this->run(theToken);
		});
	};

	int32_t AudioDecoder::FileStreamRead(void* opaque, uint8_t* buf, int32_t) {
		AudioDecoder* stream = static_cast<AudioDecoder*>(opaque);
		stream->bytesRead = 0;
		stream->currentBuffer = std::string();
		DiscordCoreAPI::RawFrameData frameData{};
		if (stream->areWeQuitting) {
			frameData.sampleCount = 0;
			stream->outDataBuffer.send(frameData);
			stream->areWeQuitting = true;
			return AVERROR_EOF;
		}
		if (DiscordCoreAPI::waitForTimeToPass(stream->inputDataBuffer, stream->currentBuffer, stream->refreshTimeForBuffer.load())) {
			frameData.sampleCount = 0;
			stream->outDataBuffer.send(frameData);
			stream->areWeQuitting = true;
			return AVERROR_EOF;
		}
		if (stream->currentBuffer.size() > 0) {
			stream->bytesRead = stream->currentBuffer.size();
		} else {
			frameData.sampleCount = 0;
			stream->outDataBuffer.send(frameData);
			stream->areWeQuitting = true;
			return AVERROR_EOF;
		}
		for (int32_t x = 0; x < stream->bytesRead; x += 1) {
			buf[x] = stream->currentBuffer[x];
		}
		if (stream->ioContext->buf_ptr - stream->ioContext->buffer >= stream->totalFileSize) {
			frameData.sampleCount = 0;
			stream->outDataBuffer.send(frameData);
			stream->areWeQuitting = true;
			return AVERROR_EOF;
		}
		return static_cast<int32_t>(stream->bytesRead);
	}

	void AudioDecoder::run(std::stop_token theToken) {
		if (!this->haveWeBooted) {
			unsigned char* fileStreamBuffer = static_cast<unsigned char*>(av_malloc(this->bufferMaxSize));
			if (fileStreamBuffer == nullptr) {
				this->haveWeFailedBool.store(true);
				if (this->doWePrintErrorMessages) {
					std::cout << DiscordCoreAPI::shiftToBrightRed() << "Failed to allocate filestreambuffer." << DiscordCoreAPI::reset() << std::endl << std::endl;
				}
				return;
			}

			this->ioContext = avio_alloc_context(fileStreamBuffer, static_cast<int32_t>(this->bufferMaxSize), 0, this, &AudioDecoder::FileStreamRead, 0, 0);

			if (this->ioContext == nullptr) {
				this->haveWeFailedBool.store(true);
				if (this->doWePrintErrorMessages) {
					std::cout << DiscordCoreAPI::shiftToBrightRed() << "Failed to allocate AVIOContext." << DiscordCoreAPI::reset() << std::endl << std::endl;
				}
				return;
			}

			this->formatContext = avformat_alloc_context();

			if (!this->formatContext) {
				this->haveWeFailedBool.store(true);
				if (this->doWePrintErrorMessages) {
					std::cout << DiscordCoreAPI::shiftToBrightRed() << "Could not allocate the format context." << DiscordCoreAPI::reset() << std::endl << std::endl;
				}
				return;
			}

			this->formatContext->pb = this->ioContext;
			this->formatContext->flags |= AVFMT_FLAG_CUSTOM_IO;
			if (avformat_open_input(*this->formatContext, "memory", nullptr, nullptr) < 0) {
				this->haveWeFailedBool.store(true);
				if (this->doWePrintErrorMessages) {
					std::cout << DiscordCoreAPI::shiftToBrightRed() << "Error opening AVFormatContext." << DiscordCoreAPI::reset() << std::endl << std::endl;
				}
				return;
			}
			*this->formatContext.getBoolPtr() = true;
			AVMediaType type = AVMediaType::AVMEDIA_TYPE_AUDIO;
			int32_t ret = av_find_best_stream(this->formatContext, type, -1, -1, NULL, 0);
			if (ret < 0) {
				std::string newString = "Could not find ";
				newString += av_get_media_type_string(type);
				newString += " stream in input memory stream.";
				this->haveWeFailedBool.store(true);
				if (this->doWePrintErrorMessages) {
					std::cout << DiscordCoreAPI::shiftToBrightRed() << newString << DiscordCoreAPI::reset() << std::endl << std::endl;
				}
				return;
			} else {
				this->audioStreamIndex = ret;
				this->audioStream = this->formatContext->streams[this->audioStreamIndex];
				if (!this->audioStream) {
					this->haveWeFailedBool.store(true);
					if (this->doWePrintErrorMessages) {
						std::cout << DiscordCoreAPI::shiftToBrightRed() << "Could not find an audio stream." << DiscordCoreAPI::reset() << std::endl << std::endl;
					}
					return;
				}

				if (avformat_find_stream_info(this->formatContext, NULL) < 0) {
					this->haveWeFailedBool.store(true);
					if (this->doWePrintErrorMessages) {
						std::cout << DiscordCoreAPI::shiftToBrightRed() << "Could not find stream information." << DiscordCoreAPI::reset() << std::endl << std::endl;
					}
					return;
				}

				this->codec = avcodec_find_decoder(this->audioStream->codecpar->codec_id);
				if (!this->codec) {
					std::string newString = "Failed to find ";
					newString += av_get_media_type_string(type);
					newString += " decoder.";
					this->haveWeFailedBool.store(true);
					if (this->doWePrintErrorMessages) {
						std::cout << DiscordCoreAPI::shiftToBrightRed() << newString << DiscordCoreAPI::reset() << std::endl << std::endl;
					}
					return;
				}

				this->audioDecodeContext = avcodec_alloc_context3(this->codec);
				if (!this->audioDecodeContext) {
					std::string newString = "Failed to allocate the ";
					newString += av_get_media_type_string(type);
					newString += " AVCodecContext.";
					this->haveWeFailedBool.store(true);
					if (this->doWePrintErrorMessages) {
						std::cout << DiscordCoreAPI::shiftToBrightRed() << newString << DiscordCoreAPI::reset() << std::endl << std::endl;
					}
					return;
				}

				if (avcodec_parameters_to_context(this->audioDecodeContext, this->audioStream->codecpar) < 0) {
					std::string newString = "Failed to copy ";
					newString += av_get_media_type_string(type);
					newString += " codec parameters to decoder context.";
					this->haveWeFailedBool.store(true);
					if (this->doWePrintErrorMessages) {
						std::cout << DiscordCoreAPI::shiftToBrightRed() << newString << DiscordCoreAPI::reset() << std::endl << std::endl;
					}
					return;
				}

				if (avcodec_open2(this->audioDecodeContext, this->codec, NULL) < 0) {
					std::string newString = "Failed to open ";
					newString += av_get_media_type_string(type);
					newString += " AVCodecContext.";
					this->haveWeFailedBool.store(true);
					if (this->doWePrintErrorMessages) {
						std::cout << DiscordCoreAPI::shiftToBrightRed() << newString << DiscordCoreAPI::reset() << std::endl << std::endl;
					}
					return;
				}

				this->swrContext = swr_alloc_set_opts(NULL, AV_CH_LAYOUT_STEREO, AVSampleFormat::AV_SAMPLE_FMT_S16, 48000, AV_CH_LAYOUT_STEREO,
					this->audioDecodeContext->sample_fmt, this->audioDecodeContext->sample_rate, 0, nullptr);
				swr_init(this->swrContext);
				if (this->doWePrintSuccessMessages) {
					av_dump_format(this->formatContext, 0, "memory", 0);
				}
			}

			this->haveWeBooted = true;
		}

		if (this->currentBuffer.size() > 0) {
			this->packet = av_packet_alloc();
			if (!this->packet) {
				this->haveWeFailedBool.store(true);
				if (this->doWePrintErrorMessages) {
					std::cout << DiscordCoreAPI::shiftToBrightRed() << "Error: Could not allocate packet" << DiscordCoreAPI::reset() << std::endl << std::endl;
				}
				return;
			}

			this->frame = av_frame_alloc();
			if (!this->frame) {
				this->haveWeFailedBool.store(true);
				if (this->doWePrintErrorMessages) {
					std::cout << DiscordCoreAPI::shiftToBrightRed() << "Error: Could not allocate frame" << DiscordCoreAPI::reset() << std::endl << std::endl;
				}
				return;
			}

			this->newFrame = av_frame_alloc();
			if (!this->newFrame) {
				this->haveWeFailedBool.store(true);
				if (this->doWePrintErrorMessages) {
					std::cout << DiscordCoreAPI::shiftToBrightRed() << "Error: Could not allocate new-frame" << DiscordCoreAPI::reset() << std::endl << std::endl;
				}
				return;
			}

			while (true) {
				if (theToken.stop_requested() || this->areWeQuitting || av_read_frame(this->formatContext, this->packet) != 0) {
					return;
				}
				if (this->packet->stream_index == this->audioStreamIndex) {
					int32_t resultValue = avcodec_send_packet(this->audioDecodeContext, this->packet);
					if (resultValue < 0) {
						char charString[32];
						av_strerror(resultValue, charString, 32);
						std::string newString = "Error submitting a packet for decoding (" + std::to_string(resultValue) + "), " + charString + ".";
						this->haveWeFailedBool.store(true);
						if (this->doWePrintErrorMessages) {
							std::cout << DiscordCoreAPI::shiftToBrightRed() << newString << DiscordCoreAPI::reset() << std::endl << std::endl;
						}
						return;
					}
					if (resultValue >= 0) {
						resultValue = avcodec_receive_frame(this->audioDecodeContext, this->frame);
						if (resultValue < 0) {
							std::string newString = "Error during decoding (" + std::to_string(resultValue) + ")";
							this->haveWeFailedBool.store(true);
							if (this->doWePrintErrorMessages) {
								std::cout << DiscordCoreAPI::shiftToBrightRed() << newString << DiscordCoreAPI::reset() << std::endl << std::endl;
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
						DiscordCoreAPI::RawFrameData rawFrame{};
						rawFrame.data.resize(unpadded_linesize);
						for (int32_t x = 0; x < unpadded_linesize; x += 1) {
							rawFrame.data[x] = this->newFrame->extended_data[0][x];
						}
						rawFrame.sampleCount = newFrame->nb_samples;
						this->outDataBuffer.send(rawFrame);
						int64_t sampleCount = swr_get_delay(this->swrContext, this->newFrame->sample_rate);
						if (sampleCount > 0) {
							if (!swr_is_initialized(this->swrContext)) {
								swr_init(this->swrContext);
							}
							swr_convert_frame(this->swrContext, this->newFrame, nullptr);
							DiscordCoreAPI::RawFrameData rawFrame02{};
							rawFrame02.data.resize(*this->newFrame->linesize);
							for (int32_t x = 0; x < *this->newFrame->linesize; x += 1) {
								rawFrame02.data[x] = this->newFrame->extended_data[0][x];
							}
							rawFrame02.sampleCount = newFrame->nb_samples;
							this->outDataBuffer.send(rawFrame02);
						}
						if (resultValue < 0 || newFrame->nb_samples == 0) {
							this->haveWeFailedBool.store(true);
							if (this->doWePrintErrorMessages) {
								std::cout << DiscordCoreAPI::shiftToBrightRed() << "Return value is less than zero!" << DiscordCoreAPI::reset() << std::endl << std::endl;
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
				if (theToken.stop_requested() || this->areWeQuitting) {
					break;
				}
			}
			if (this->doWePrintSuccessMessages) {
				std::cout << DiscordCoreAPI::shiftToBrightGreen() << "Completed decoding!" << std::endl << DiscordCoreAPI::reset() << std::endl << std::endl;
			}
		}
		return;
	}

	AudioDecoder::~AudioDecoder() {
		this->cancelMe();
	}

};