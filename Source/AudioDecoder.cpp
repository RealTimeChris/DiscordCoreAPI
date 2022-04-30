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
/// Chris M.
/// https://discordcoreapi.com
/// \file AudioDecoder.cpp

#include <discordcoreapi/AudioDecoder.hpp>

namespace DiscordCoreAPI {

	AudioDecoder::AudioDecoder(BuildAudioDecoderData dataPackage, bool doWePrintNew) {
		this->bufferMaxSize = dataPackage.bufferMaxSize;
		this->totalFileSize = dataPackage.totalFileSize;
		this->doWePrint = doWePrintNew;
	}

	void AudioDecoder::submitDataForDecoding(std::string dataToDecode) {
		this->inputDataBuffer.send(dataToDecode);
	}

	bool AudioDecoder::getFrame(RawFrameData& dataPackage) {
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
		this->refreshTimeForBuffer.store(10, std::memory_order_seq_cst);
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
		return this->haveWeFailedBool.load(std::memory_order_seq_cst);
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
		if (stream->areWeQuitting) {
			RawFrameData frameData{};
			frameData.sampleCount = 0;
			stream->outDataBuffer.send(frameData);
			stream->areWeQuitting = true;
			return AVERROR_EOF;
		}
		if (waitForTimeToPass(stream->inputDataBuffer, stream->currentBuffer, stream->refreshTimeForBuffer.load(std::memory_order_seq_cst))) {
			RawFrameData frameData{};
			frameData.sampleCount = 0;
			stream->outDataBuffer.send(frameData);
			stream->areWeQuitting = true;
			return AVERROR_EOF;
		}
		if (stream->currentBuffer.size() > 0) {
			stream->bytesRead = stream->currentBuffer.size();
		} else {
			RawFrameData frameData{};
			frameData.sampleCount = 0;
			stream->outDataBuffer.send(frameData);
			stream->areWeQuitting = true;
			return AVERROR_EOF;
		}
		for (int32_t x = 0; x < stream->bytesRead; x += 1) {
			buf[x] = stream->currentBuffer[x];
		}
		if (stream->ioContext->buf_ptr - stream->ioContext->buffer >= stream->totalFileSize) {
			RawFrameData frameData{};
			frameData.sampleCount = 0;
			stream->outDataBuffer.send(frameData);
			stream->areWeQuitting = true;
			return AVERROR_EOF;
		}
		return static_cast<int32_t>(stream->bytesRead);
	}

	void AudioDecoder::run(std::stop_token theToken) {
		try {
			if (!this->haveWeBooted) {
				unsigned char* fileStreamBuffer = static_cast<unsigned char*>(av_malloc(this->bufferMaxSize));
				if (fileStreamBuffer == nullptr) {
					this->haveWeFailedBool.store(true, std::memory_order_seq_cst);
					throw std::runtime_error("Failed to allocate filestreambuffer.");
				}

				this->ioContext = avio_alloc_context(fileStreamBuffer, static_cast<int32_t>(this->bufferMaxSize), 0, this, &AudioDecoder::FileStreamRead, 0, 0);

				if (this->ioContext == nullptr) {
					this->haveWeFailedBool.store(true, std::memory_order_seq_cst);
					throw std::runtime_error("Failed to allocate AVIOContext.");
				}

				this->formatContext = avformat_alloc_context();

				if (!this->formatContext) {
					this->haveWeFailedBool.store(true, std::memory_order_seq_cst);
					throw std::runtime_error("Could not allocate the format context.");
				}

				this->formatContext->pb = this->ioContext;
				this->formatContext->flags |= AVFMT_FLAG_CUSTOM_IO;
				if (avformat_open_input(*this->formatContext, "memory", nullptr, nullptr) < 0) {
					this->haveWeFailedBool.store(true, std::memory_order_seq_cst);
					throw std::runtime_error("Error opening AVFormatContext.");
				}
				*this->formatContext.getBoolPtr() = true;
				AVMediaType type = AVMediaType::AVMEDIA_TYPE_AUDIO;
				int32_t ret = av_find_best_stream(this->formatContext, type, -1, -1, NULL, 0);
				if (ret < 0) {
					std::string newString = "Could not find ";
					newString += av_get_media_type_string(type);
					newString += " stream in input memory stream.";
					this->haveWeFailedBool.store(true, std::memory_order_seq_cst);
					throw std::runtime_error(newString.c_str());
				}

				else {
					this->audioStreamIndex = ret;
					this->audioStream = this->formatContext->streams[this->audioStreamIndex];
					if (!this->audioStream) {
						this->haveWeFailedBool.store(true, std::memory_order_seq_cst);
						throw std::runtime_error("Could not find an audio stream.");
					}

					if (avformat_find_stream_info(this->formatContext, NULL) < 0) {
						this->haveWeFailedBool.store(true, std::memory_order_seq_cst);
						throw std::runtime_error("Could not find stream information.");
					}

					this->codec = avcodec_find_decoder(this->audioStream->codecpar->codec_id);
					if (!this->codec) {
						std::string newString = "Failed to find ";
						newString += av_get_media_type_string(type);
						newString += " decoder.";
						this->haveWeFailedBool.store(true, std::memory_order_seq_cst);
						throw std::runtime_error(newString.c_str());
					}

					this->audioDecodeContext = avcodec_alloc_context3(this->codec);
					if (!this->audioDecodeContext) {
						std::string newString = "Failed to allocate the ";
						newString += av_get_media_type_string(type);
						newString += " AVCodecContext.";
						this->haveWeFailedBool.store(true, std::memory_order_seq_cst);
						throw std::runtime_error(newString.c_str());
					}

					if (avcodec_parameters_to_context(this->audioDecodeContext, this->audioStream->codecpar) < 0) {
						std::string newString = "Failed to copy ";
						newString += av_get_media_type_string(type);
						newString += " codec parameters to decoder context.";
						this->haveWeFailedBool.store(true, std::memory_order_seq_cst);
						throw std::runtime_error(newString.c_str());
					}

					if (avcodec_open2(this->audioDecodeContext, this->codec, NULL) < 0) {
						std::string newString = "Failed to open ";
						newString += av_get_media_type_string(type);
						newString += " AVCodecContext.";
						this->haveWeFailedBool.store(true, std::memory_order_seq_cst);
						throw std::runtime_error(newString.c_str());
					}

					this->swrContext = swr_alloc_set_opts(NULL, AV_CH_LAYOUT_STEREO, AVSampleFormat::AV_SAMPLE_FMT_S16, 48000, AV_CH_LAYOUT_STEREO,
						this->audioDecodeContext->sample_fmt, this->audioDecodeContext->sample_rate, 0, nullptr);
					swr_init(this->swrContext);
					if (this->doWePrint) {
						av_dump_format(this->formatContext, 0, "memory", 0);
					}
				}

				this->haveWeBooted = true;
			}

			if (this->currentBuffer.size() > 0) {
				this->packet = av_packet_alloc();
				if (!this->packet) {
					this->haveWeFailedBool.store(true, std::memory_order_seq_cst);
					throw std::runtime_error("Error: Could not allocate packet");
				}

				this->frame = av_frame_alloc();
				if (!this->frame) {
					this->haveWeFailedBool.store(true, std::memory_order_seq_cst);
					throw std::runtime_error("Error: Could not allocate frame");
				}

				this->newFrame = av_frame_alloc();
				if (!this->newFrame) {
					this->haveWeFailedBool.store(true, std::memory_order_seq_cst);
					throw std::runtime_error("Error: Could not allocate new-frame");
				}

				while (true) {
					if (theToken.stop_requested() || this->areWeQuitting || av_read_frame(this->formatContext, this->packet) != 0) {
						break;
					}
					if (this->packet->stream_index == this->audioStreamIndex) {
						int32_t ret = avcodec_send_packet(this->audioDecodeContext, this->packet);
						if (ret < 0) {
							char charString[32];
							av_strerror(ret, charString, 32);
							std::string newString = "Error submitting a packet for decoding (" + std::to_string(ret) + "), " + charString + ".";
							this->haveWeFailedBool.store(true, std::memory_order_seq_cst);
							throw std::runtime_error(newString.c_str());
						}
						if (ret >= 0) {
							ret = avcodec_receive_frame(this->audioDecodeContext, this->frame);
							if (ret < 0) {
								std::string newString = "Error during decoding (" + std::to_string(ret) + ")";
								this->haveWeFailedBool.store(true, std::memory_order_seq_cst);
								throw std::runtime_error(newString.c_str());
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
							int32_t unpadded_linesize =
								this->newFrame->nb_samples * av_get_bytes_per_sample(static_cast<AVSampleFormat>(this->newFrame->format)) * 2;
							RawFrameData rawFrame{};
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
								RawFrameData rawFrame02{};
								rawFrame02.data.resize(*this->newFrame->linesize);
								for (int32_t x = 0; x < *this->newFrame->linesize; x += 1) {
									rawFrame02.data[x] = this->newFrame->extended_data[0][x];
								}
								rawFrame02.sampleCount = newFrame->nb_samples;
								this->outDataBuffer.send(rawFrame02);
							}
							if (ret < 0 || newFrame->nb_samples == 0) {
								this->haveWeFailedBool.store(true, std::memory_order_seq_cst);
								throw std::runtime_error("Return value is less than zero!");
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
				if (this->doWePrint) {
					std::cout << shiftToBrightGreen() << "Completed decoding!" << std::endl << reset() << std::endl;
				}
				return;
			}
			return;
		} catch (...) {
			this->haveWeFailedBool.store(true, std::memory_order_seq_cst);
			reportException("AudioDecoder::run");
			return;
		}
	}

	AudioDecoder::~AudioDecoder() {
		this->cancelMe();
	}

};