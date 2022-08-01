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
/// VoiceConnection.cpp - Source file for the voice connection class.
/// Jul 15, 2021
/// https://discordcoreapi.com
/// \file VoiceConnection.cpp

#include <discordcoreapi/VoiceConnection.hpp>
#include <discordcoreapi/DiscordCoreClient.hpp>

namespace DiscordCoreAPI {

	void OpusEncoderWrapper::OpusEncoderDeleter::operator()(OpusEncoder* other) {
		if (other) {
			opus_encoder_destroy(other);
			other = nullptr;
		}
	}

	OpusEncoderWrapper& OpusEncoderWrapper::operator=(OpusEncoderWrapper&& other) {
		this->thePtr.reset(other.thePtr.release());
		return *this;
	}

	OpusEncoderWrapper::OpusEncoderWrapper(OpusEncoderWrapper&& other) {
		*this = std::move(other);
	}

	OpusEncoderWrapper::OpusEncoderWrapper() {
		int32_t error{};
		this->thePtr.reset(opus_encoder_create(48000, 2, OPUS_APPLICATION_AUDIO, &error));
		if (error != OPUS_OK) {
			throw std::runtime_error{ "Failed to create the Opus Encoder" };
		}
	}

	OpusEncoderWrapper::operator OpusEncoder*() {
		return this->thePtr.get();
	}

	void OpusDecoderWrapper::OpusDecoderDeleter::operator()(OpusDecoder* other) {
		if (other) {
			opus_decoder_destroy(other);
			other = nullptr;
		}
	}

	OpusDecoderWrapper& OpusDecoderWrapper::operator=(OpusDecoderWrapper&& other) {
		this->thePtr.reset(other.thePtr.release());
		return *this;
	}

	OpusDecoderWrapper::OpusDecoderWrapper(OpusDecoderWrapper&& other) {
		*this = std::move(other);
	}

	OpusDecoderWrapper::OpusDecoderWrapper() {
		int32_t error{};
		this->thePtr.reset(opus_decoder_create(48000, 2, &error));
		if (error != OPUS_OK) {
			throw std::runtime_error{ "Failed to create the Opus Decoder" };
		}
	}

	OpusDecoderWrapper::operator OpusDecoder*() {
		return this->thePtr.get();
	}

	RTPPacket::RTPPacket(uint32_t timestampNew, uint16_t sequenceNew, uint32_t ssrcNew, const std::vector<uint8_t>& audioDataNew, const std::string& theKeysNew) {
		this->audioData = audioDataNew;
		this->timestamp = timestampNew;
		this->sequence = sequenceNew;
		this->theKeys = theKeysNew;
		this->ssrc = ssrcNew;
	}

	RTPPacket::operator std::string() {
		if (this->theKeys.size() > 0) {
			const uint8_t nonceSize{ crypto_secretbox_NONCEBYTES };
			const uint8_t headerSize{ 12 };
			const uint8_t byteSize{ 8 };
			std::string header{};
			storeBits(header, this->version);
			storeBits(header, this->flags);
			storeBits(header, this->sequence);
			storeBits(header, this->timestamp);
			storeBits(header, this->ssrc);
			std::unique_ptr<uint8_t[]> nonceForLibSodium{ std::make_unique<uint8_t[]>(nonceSize) };
			for (uint8_t x = 0; x < headerSize; x++) {
				nonceForLibSodium[x] = header[x];
			}
			for (uint8_t x = headerSize; x < nonceSize; x++) {
				nonceForLibSodium[x] = 0;
			}
			uint64_t numOfBytes{ headerSize + this->audioData.size() + crypto_secretbox_MACBYTES };
			std::unique_ptr<uint8_t[]> audioDataPacket{ std::make_unique<uint8_t[]>(numOfBytes) };
			for (uint8_t x = 0; x < headerSize; x++) {
				audioDataPacket[x] = header[x];
			}
			std::unique_ptr<uint8_t[]> encryptionKeys{ std::make_unique<uint8_t[]>(this->theKeys.size()) };
			for (uint64_t x = 0; x < this->theKeys.size(); x++) {
				encryptionKeys[x] = this->theKeys[x];
			}
			if (crypto_secretbox_easy(audioDataPacket.get() + headerSize, this->audioData.data(), this->audioData.size(), nonceForLibSodium.get(), encryptionKeys.get()) != 0) {
				return "";
			};
			std::string audioDataPacketNew{};
			audioDataPacketNew.insert(audioDataPacketNew.begin(), audioDataPacket.get(), audioDataPacket.get() + numOfBytes);
			return audioDataPacketNew;
		}
		return {};
	}

	VoiceConnection::VoiceConnection(DiscordCoreInternal::BaseSocketAgent* BaseSocketAgentNew, const DiscordCoreInternal::VoiceConnectInitData& initDataNew,
		DiscordCoreAPI::ConfigManager* configManagerNew, std::atomic_bool* doWeQuitNew, StreamType streamTypeNew, StreamInfo streamInfoNew) noexcept
		: WebSocketSSLShard(BaseSocketAgentNew->discordCoreClient, &this->connections, BaseSocketAgentNew->currentBaseSocketAgent, initDataNew.currentShard, this->doWeQuit),
		  DatagramSocketClient(true) {
		this->baseShard = BaseSocketAgentNew->sslShards[initDataNew.currentShard].get();
		this->activeState.store(VoiceActiveState::Connecting);
		WebSocketSSLShard::connections = &this->connections;
		this->baseSocketAgent = BaseSocketAgentNew;
		this->voiceConnectInitData = initDataNew;
		this->configManager = configManagerNew;
		this->theStreamInfo = streamInfoNew;
		this->streamType = streamTypeNew;
		this->doWeQuit = doWeQuitNew;
	}

	Snowflake VoiceConnection::getChannelId() noexcept {
		return this->voiceConnectInitData.channelId;
	}

	std::string VoiceConnection::encryptSingleAudioFrame(const EncodedFrameData& bufferToSend) noexcept {
		if (this->secretKeySend.size() > 0) {
			this->sequenceIndex++;
			this->timeStamp += bufferToSend.sampleCount;
			if (this->timeStamp > INT16_MAX && this->sequenceIndex < 100) {
				this->timeStamp = 0;
			}
			return RTPPacket{ this->timeStamp, this->sequenceIndex, this->audioSSRC, bufferToSend.data, this->secretKeySend };
		}
		return {};
	}

	void VoiceConnection::sendSingleAudioFrame(std::string& audioDataPacketNew) noexcept {
		if (DatagramSocketClient::areWeStillConnected()) {
			DatagramSocketClient::writeData(audioDataPacketNew);
		} else {
			this->onClosedVoice();
		}
	}

	UnboundedMessageBlock<AudioFrameData>& VoiceConnection::getAudioBuffer() noexcept {
		return this->audioDataBuffer;
	}

	void VoiceConnection::sendSingleFrame(const AudioFrameData& frameData) noexcept {
		this->audioDataBuffer.send(frameData);
	}

	void VoiceConnection::onMessageReceived() noexcept {
		try {
			std::string theMessage{};
			if (WebSocketSSLShard::processedMessages.size() > 0) {
				theMessage = WebSocketSSLShard::processedMessages.front();
				WebSocketSSLShard::processedMessages.pop();
			} else {
				return;
			}
			if (theMessage.size() > 0) {
				nlohmann::json payload = payload.parse(theMessage);
				if (this->configManager->doWePrintWebSocketSuccessMessages()) {
					cout << DiscordCoreAPI::shiftToBrightGreen() << "Message received from Voice WebSocket: " << theMessage << DiscordCoreAPI::reset() << endl << endl;
				}
				if (payload.contains("op") && !payload["op"].is_null()) {
					switch (payload["op"].get<int32_t>()) {
						case 2: {
							this->audioSSRC = payload["d"]["ssrc"].get<uint32_t>();
							this->voiceIp = payload["d"]["ip"].get<std::string>();
							this->port = std::to_string(payload["d"]["port"].get<int64_t>());
							for (auto& value: payload["d"]["modes"]) {
								if (value == "xsalsa20_poly1305") {
									this->audioEncryptionMode = value;
								}
							}
							this->connectionState.store(VoiceConnectionState::Initializing_DatagramSocket);
							break;
						}
						case 4: {
							for (uint32_t x = 0; x < payload["d"]["secret_key"].size(); x++) {
								this->secretKeySend.push_back(payload["d"]["secret_key"][x].get<uint8_t>());
							}
							if (this->targetSocket) {								
								this->targetSocket->writeData(this->secretKeySend);
							}
							this->connectionState.store(VoiceConnectionState::Collecting_Init_Data);
							break;
						}
						case 6: {
							this->haveWeReceivedHeartbeatAck = true;
							break;
						}
						case 8: {
							if (payload["d"].contains("heartbeat_interval")) {
								this->heartbeatInterval = static_cast<int32_t>(payload["d"]["heartbeat_interval"].get<float>());
								this->areWeHeartBeating = false;
								this->connectionState.store(VoiceConnectionState::Sending_Identify);
							}
							break;
						}
						case 5: {
							if (payload.contains("d") && !payload["d"].is_null()) {
								if (!payload["d"]["ssrc"].is_null()) {
									VoiceUser theUser{};
									theUser.theUserId = stoull(payload["d"]["user_id"].get<std::string>());
									this->thePayloads[payload["d"]["ssrc"].get<int32_t>()] = std ::move(theUser);
								}
							}
							break;
						}
						case 13: {
							if (payload.contains("d")&&!payload["d"].is_null()){
								if (!payload["d"]["user_id"].is_null()) {
									auto userId = stoull(payload["d"]["user_id"].get<std::string>());
									for (auto& [key, value]: this->thePayloads) {
										if (userId == value.theUserId) {
											this->thePayloads.erase(key);
											break;
										}
									}
								}
							}
						}
					}
				}
			}
		} catch (...) {
			if (this->configManager->doWePrintWebSocketErrorMessages()) {
				DiscordCoreAPI::reportException("VoiceConnection::onMessageReceived()");
			}
			this->onClosedVoice();
		}
	}

	void VoiceConnection::sendVoiceData(const std::string& responseData) noexcept {
		try {
			if (responseData.size() == 0) {
				if (this->configManager->doWePrintWebSocketErrorMessages()) {
					cout << DiscordCoreAPI::shiftToBrightRed() << "Please specify voice data to send" << DiscordCoreAPI::reset() << endl << endl;
				}
				return;
			} else {
				if (DatagramSocketClient::areWeStillConnected()) {
					std::string theData = responseData;
					DatagramSocketClient::writeData(theData);
				} else {
					this->onClosedVoice();
				}
			}
		} catch (...) {
			if (this->configManager->doWePrintWebSocketErrorMessages()) {
				DiscordCoreAPI::reportException("VoiceConnection::sendVoiceData()");
			}
			this->onClosedVoice();
		}
	}

	void VoiceConnection::sendSpeakingMessage(const bool isSpeaking) noexcept {
		DiscordCoreInternal::SendSpeakingData theData{};
		if (!isSpeaking) {
			theData.type = static_cast<DiscordCoreInternal::SendSpeakingType>(0);
			this->sendSilence();
			DatagramSocketClient::processIO(10000);
		} else {
			theData.type = DiscordCoreInternal::SendSpeakingType::Microphone;
			theData.delay = 0;
			theData.ssrc = this->audioSSRC;
			nlohmann::json newString = theData;
			std::string theString{};
			this->stringifyJsonData(newString, theString, DiscordCoreInternal::WebSocketOpCode::Op_Text);
			if (!this->sendMessage(theString, true)) {
				this->onClosedVoice();
			}
		}
	}

	void VoiceConnection::runWebSocket(std::stop_token& stopToken) noexcept {
		try {
			while (!stopToken.stop_requested() && !this->doWeQuit->load() && this->activeState.load() != VoiceActiveState::Exiting) {
				if (!stopToken.stop_requested() && WebSocketSSLShard::connections->size() > 0) {
					DiscordCoreAPI::StopWatch theStopWatch{ 10000ms };
					if (this->activeState.load() == VoiceActiveState::Connecting) {
						this->lastActiveState.store(VoiceActiveState::Stopped);
					} else {
						this->lastActiveState.store(this->activeState.load());
					}
					this->activeState.store(VoiceActiveState::Connecting);
					this->connectionState.store(VoiceConnectionState::Collecting_Init_Data);
					while (!stopToken.stop_requested() && this->baseShard->theWebSocketState.load() != DiscordCoreInternal::WebSocketSSLShardState::Authenticated) {
						if (theStopWatch.hasTimePassed() || this->activeState.load() == VoiceActiveState::Exiting) {
							return;
						}
						std::this_thread::sleep_for(1ms);
					}
					this->connectInternal();
					this->sendSpeakingMessage(false);
					this->sendSpeakingMessage(true);
					this->activeState.store(this->lastActiveState.load());
				}
				if (!stopToken.stop_requested() && this->heartbeatInterval != 0 && !this->areWeHeartBeating) {
					this->areWeHeartBeating = true;
					this->heartBeatStopWatch = DiscordCoreAPI::StopWatch{ std::chrono::milliseconds{ this->heartbeatInterval } };
				}
				if (!stopToken.stop_requested() && WebSocketSSLShard::areWeStillConnected() && this->heartBeatStopWatch.hasTimePassed() && this->areWeHeartBeating) {
					this->sendHeartBeat();
					this->heartBeatStopWatch.resetTimer();
				}
				if (!stopToken.stop_requested() && WebSocketSSLShard::areWeStillConnected()) {
					DiscordCoreInternal::SSLClient::processIO(10000);
				}
				if (!stopToken.stop_requested() && WebSocketSSLShard::areWeStillConnected() && WebSocketSSLShard::inputBuffer.size() > 0) {
					this->parseMessage(this);
				}
				if (!stopToken.stop_requested() && WebSocketSSLShard::areWeStillConnected() && this->processedMessages.size() > 0) {
					this->onMessageReceived();
				}
				std::this_thread::sleep_for(1ms);
			}
		} catch (...) {
			if (this->configManager->doWePrintWebSocketErrorMessages()) {
				DiscordCoreAPI::reportException("VoiceConnection::run()");
			}
			this->onClosedVoice();
		}
	}

	void VoiceConnection::runBridge(std::stop_token& theToken) noexcept {
		StopWatch theStopWatch{ 240ms };
		while (!theToken.stop_requested()) {
			this->targetSocket->processIO(100);
			this->parseIncomingVoiceData();
			if (theStopWatch.hasTimePassed()) {
				theStopWatch.resetTimer();
				for (int32_t x = 0; x < 12; x++) {
					this->mixAudio();
				}
			}
		}
	}

	bool VoiceConnection::collectAndProcessAMessage(VoiceConnectionState stateToWaitFor) noexcept {
		DiscordCoreAPI::StopWatch theStopWatch{ 2500ms };
		while (!this->doWeQuit->load() && this->connectionState.load() != stateToWaitFor) {
			WebSocketSSLShard::processIO(10000);
			if (!WebSocketSSLShard::areWeStillConnected()) {
				return false;
			}
			if (WebSocketSSLShard::inputBuffer.size() > 0) {
				if (!this->parseMessage(this)) {
					return false;
				}
			}
			if (this->processedMessages.size() > 0) {
				this->onMessageReceived();
				return true;
			}
			if (theStopWatch.hasTimePassed()) {
				return false;
			}
			std::this_thread::sleep_for(1ms);
		}
		return false;
	}

	void VoiceConnection::runVoice(std::stop_token& stopToken) noexcept {
		StopWatch theStopWatch{ 20000ms };
		StopWatch theSendSilenceStopWatch{ 5000ms };
		while (!stopToken.stop_requested() && !this->doWeQuit->load() && this->activeState.load() != VoiceActiveState::Exiting) {
			if (!DatagramSocketClient::areWeStillConnected()) {
				this->onClosedVoice();
			}
			switch (this->activeState.load()) {
				case VoiceActiveState::Connecting: {
					while (!stopToken.stop_requested() && this->activeState.load() == VoiceActiveState::Connecting) {
						std::this_thread::sleep_for(1ms);
					}
					break;
				}
				case VoiceActiveState::Stopped: {
					this->areWePlaying.store(false);
					this->audioDataBuffer.clearContents();
					this->clearAudioData();
					while (!stopToken.stop_requested() && this->activeState.load() == VoiceActiveState::Stopped) {
						DatagramSocketClient::processIO(1000);
						std::string theString = DatagramSocketClient::getInputBuffer();
						if (this->streamType == StreamType::Source) {
							this->theFrameQueueRaw.push(theString);
						}
						if (theSendSilenceStopWatch.hasTimePassed()) {
							theSendSilenceStopWatch.resetTimer();
							this->sendSpeakingMessage(true);
							this->sendSpeakingMessage(false);
						}
						std::this_thread::sleep_for(1ms);
					}
					break;
				}
				case VoiceActiveState::Paused: {
					this->areWePlaying.store(false);
					while (!stopToken.stop_requested() && this->activeState.load() == VoiceActiveState::Paused) {
						DatagramSocketClient::processIO(10000);
						if (theSendSilenceStopWatch.hasTimePassed()) {
							theSendSilenceStopWatch.resetTimer();
							this->sendSpeakingMessage(true);
							this->sendSpeakingMessage(false);
						}
						std::this_thread::sleep_for(1ms);
					}
					break;
				}
				case VoiceActiveState::Playing: {
					DoubleTimePointNs startingValue{ std::chrono::system_clock::now().time_since_epoch() };
					DoubleTimePointNs intervalCount{ std::chrono::nanoseconds{ 20000000 } };
					DoubleTimePointNs targetTime{ startingValue.time_since_epoch() + intervalCount.time_since_epoch() };
					int32_t frameCounter{ 0 };
					DoubleTimePointNs totalTime{ std::chrono::duration_cast<std::chrono::nanoseconds>(std::chrono::nanoseconds{ 0 }) };
					if (!this->areWePlaying.load()) {
						this->sendSpeakingMessage(true);
						this->areWePlaying.store(true);
					}

					this->audioData.type = AudioFrameType::Encoded;
					this->audioData.encodedFrameData.data.clear();
					this->audioData.rawFrameData.data.clear();

					while (!stopToken.stop_requested() && this->activeState.load() == VoiceActiveState::Playing) {
						if (this->streamType == StreamType::None || this->streamType == StreamType::Source) {
							this->audioDataBuffer.tryReceive(this->audioData);
						} else {
							this->audioDataBufferReal.tryReceive(this->audioData);
						}
						if (!this->targetSocket) {
							while (this->theFrameQueueRaw.size() > 0) {
								this->theFrameQueueRaw.pop();
							}
						}
						theStopWatch.resetTimer();
						while (!stopToken.stop_requested() && !DatagramSocketClient::areWeStillConnected()) {
							if (theStopWatch.hasTimePassed() || this->activeState.load() == VoiceActiveState::Exiting) {
								return;
							}
							std::this_thread::sleep_for(1ms);
						}
						frameCounter++;
						if (this->audioData.guildMemberId != 0) {
							this->currentGuildMemberId = this->audioData.guildMemberId;
						}
						std::string newFrame{};
						bool doWeBreak{ false };
						switch (this->audioData.type) {
							case AudioFrameType::RawPCM: {
								std::vector<RawFrameData> rawFrames{};
								rawFrames.push_back(this->audioData.rawFrameData);
								auto encodedFrameData = this->encoder.encodeFrames(rawFrames);
								newFrame = this->encryptSingleAudioFrame(encodedFrameData[0].encodedFrameData);
								break;
							}
							case AudioFrameType::Encoded: {
								newFrame = this->encryptSingleAudioFrame(this->audioData.encodedFrameData);
								break;
							}
							case AudioFrameType::Skip: {
								SongCompletionEventData completionEventData{};
								completionEventData.guild = Guilds::getCachedGuildAsync({ .guildId = this->voiceConnectInitData.guildId }).get();
								if (this->currentGuildMemberId != 0) {
									completionEventData.guildMember =
										GuildMembers::getCachedGuildMemberAsync({ .guildMemberId = this->currentGuildMemberId, .guildId = this->voiceConnectInitData.guildId })
											.get();
								}
								completionEventData.wasItAFail = false;
								getSongAPIMap()[this->voiceConnectInitData.guildId]->onSongCompletionEvent(completionEventData);
								this->areWePlaying.store(false);
								doWeBreak = true;
								break;
							}

						}
						if (doWeBreak) {
							break;
						}
						
						DatagramSocketClient::processIO(1000);
						std::string theString = DatagramSocketClient::getInputBuffer();
						if (this->streamType == StreamType::Source) {
							this->theFrameQueueRaw.push(theString);
						}
						auto waitTime = targetTime - std::chrono::system_clock::now();
						nanoSleep(static_cast<int64_t>(static_cast<double>(waitTime.count()) * 0.95f));
						waitTime = targetTime - std::chrono::system_clock::now();
						if (waitTime.count() > 0) {
							spinLock(waitTime.count());
						}
						startingValue = std::chrono::system_clock::now();
						if (newFrame.size() > 0) {
							this->sendSingleAudioFrame(newFrame);
						}
						this->audioData.encodedFrameData.data.clear();
						this->audioData.encodedFrameData.sampleCount = 0;
						this->audioData.rawFrameData.data.clear();
						this->audioData.rawFrameData.sampleCount = 0;
						this->audioData.type = AudioFrameType::Unset;
						totalTime += std::chrono::system_clock::now() - startingValue;
						auto intervalCountNew =
							DoubleTimePointNs{ std::chrono::nanoseconds{ 20000000 } - totalTime.time_since_epoch() / frameCounter }.time_since_epoch().count();
						intervalCount = DoubleTimePointNs{ std::chrono::nanoseconds{ static_cast<uint64_t>(intervalCountNew) } };
						targetTime = std::chrono::system_clock::now().time_since_epoch() + intervalCount;
						
					}
					
					break;
				}
				case VoiceActiveState::Exiting: {
					return;
				}
				default: {
					break;
				}
			}
			if (stopToken.stop_requested()) {
				return;
			}
			std::this_thread::sleep_for(1ms);
		}
	};

	void VoiceConnection::parseIncomingVoiceData() noexcept {
		if (this->streamType == StreamType::Source) {
			std::string theBuffer{};
			if (this->theFrameQueueRaw.size() > 0) {
				auto theBuffer = this->theFrameQueueRaw.front();
				this->theFrameQueueRaw.pop();
				if (theBuffer.size() > 0 && this->secretKeySend.size() > 0) {
					std::vector<uint8_t> packet{};
					packet.insert(packet.begin(), theBuffer.begin(), theBuffer.end());
					constexpr size_t headerSize{ 12 };
					if (packet.size() < headerSize) {
						return;
					}

					if (uint8_t payload_type = packet[1] & 0b0111'1111; 72 <= payload_type && payload_type <= 76) {
						return;
					}
					
					VoicePayload thePayload{};
					uint16_t theSequence{ *reinterpret_cast<uint16_t*>(packet.data() + 2) };
					theSequence = ntohs(theSequence);
					thePayload.currentSequenceMin = theSequence;
					thePayload.currentSequenceMax = theSequence + 1;
					uint32_t theTimeStamp{ *reinterpret_cast<uint32_t*>(packet.data() + 4) };
					theTimeStamp = ntohl(theTimeStamp);
					thePayload.currentTimeStampMin = theTimeStamp;
					thePayload.currentTimeStampMax = theTimeStamp + 960;
					uint32_t speakerSsrc{ *reinterpret_cast<uint32_t*>(packet.data() + 8) };
					speakerSsrc = ntohl(speakerSsrc);
					if (this->thePayloads[speakerSsrc].currentTimeStamp == 0) {
						this->thePayloads[speakerSsrc].currentTimeStamp = theTimeStamp;
					}

					std::vector<uint8_t> nonce{};
					nonce.resize(24);
					for (uint32_t x = 0; x < headerSize; x++) {
						nonce[x] = packet[x];
					}
					const size_t csrc_count = packet[0] & 0b0000'1111;
					const ptrdiff_t offset_to_data = headerSize + sizeof(uint32_t) * csrc_count;
					uint8_t* encryptedData = packet.data() + offset_to_data;
					const size_t encryptedDataLen = packet.size() - offset_to_data;
					std::vector<uint8_t> theKey{};
					theKey.insert(theKey.begin(), this->secretKeySend.begin(), this->secretKeySend.end());

					if (crypto_secretbox_open_easy(encryptedData, encryptedData, encryptedDataLen, nonce.data(), theKey.data())) {
						return;
					}

					std::string decryptedDataString{};
					decryptedDataString.insert(decryptedDataString.begin(), encryptedData, encryptedData + encryptedDataLen);

					if (const bool usesExtension [[maybe_unused]] = (packet[0] >> 4) & 0b0001) {
						size_t extLen = 0;
						uint16_t extLengthInWords{ *reinterpret_cast<uint16_t*>(decryptedDataString.data() + 2) };
						extLengthInWords = ntohs(extLengthInWords);
						extLen = sizeof(uint32_t) * extLengthInWords;
						constexpr size_t extHeaderLen = sizeof(uint16_t) * 2;
						decryptedDataString = decryptedDataString.substr(extHeaderLen + extLen);
					}
					if (decryptedDataString.size() > 0) {
						thePayload.theRawData = decryptedDataString;
						this->thePayloads[speakerSsrc].thePayloads.push(thePayload);
					}
				}
			}
		} else {
			auto theBuffer = this->targetSocket->getInputBuffer();

			AudioFrameData theFrame{};
			theFrame.encodedFrameData.data.insert(theFrame.encodedFrameData.data.begin(), theBuffer.begin(), theBuffer.end());
			theFrame.encodedFrameData.sampleCount = 960;
			theFrame.type = AudioFrameType::Encoded;
			std::string theString{};
			theString.insert(theString.begin(), theFrame.encodedFrameData.data.begin(), theFrame.encodedFrameData.data.end());
			this->audioDataBufferReal.send(theFrame);
		}
	}

	bool VoiceConnection::areWeCurrentlyPlaying() noexcept {
		if (this == nullptr) {
			return false;
		} else {
			if (this->activeState.load() == VoiceActiveState::Playing || this->activeState.load() == VoiceActiveState::Paused) {
				return true;
			} else {
				return false;
			}
		}
	}

	void VoiceConnection::disconnectInternal() noexcept {
		if (this->taskThread01) {
			this->taskThread01->request_stop();
			if (this->taskThread01->joinable()) {
				this->taskThread01->join();
			}
			this->taskThread01.reset(nullptr);
		}
		if (this->taskThread02) {
			this->taskThread02->request_stop();
			if (this->taskThread02->joinable()) {
				this->taskThread02->join();
			}
			this->taskThread02.reset(nullptr);
		}
		DatagramSocketClient::disconnect();
		WebSocketSSLShard::disconnect(false);
		auto thePtr = getSongAPIMap()[this->voiceConnectInitData.guildId].get();
		if (thePtr) {
			thePtr->onSongCompletionEvent.remove(thePtr->eventToken);
		}
		this->areWeConnectedBool.store(false);
		this->connectionState.store(VoiceConnectionState::Collecting_Init_Data);
		this->activeState.store(VoiceActiveState::Connecting);
	}

	void VoiceConnection::connectInternal() noexcept {
		auto thePtr = this->baseSocketAgent->sslShards[voiceConnectInitData.currentShard].get();
		std::unique_lock theLock{ thePtr->theMutex };
		if (this->connections.size() > 0) {
			this->connections.pop();
		}
		if (this->currentReconnectTries >= this->maxReconnectTries) {
			this->doWeQuit->store(true);
			if (this->configManager->doWePrintWebSocketErrorMessages()) {
				cout << "VoiceConnection::connectInternal() Error: Failed to connect to voice channel!" << endl << endl;
			}
		}
		switch (this->connectionState.load()) {
			case VoiceConnectionState::Collecting_Init_Data: {
				this->baseShard->voiceConnectionDataBufferMap[this->voiceConnectInitData.guildId] = &this->voiceConnectionDataBuffer;
				this->baseShard->voiceConnectionDataBufferMap[this->voiceConnectInitData.guildId]->clearContents();
				this->baseShard->getVoiceConnectionData(this->voiceConnectInitData);
				
				if (DiscordCoreAPI::waitForTimeToPass(this->voiceConnectionDataBuffer, this->voiceConnectionData, 10000)) {
					this->currentReconnectTries++;
					this->onClosedVoice();
					this->connectInternal();
					return;
				}
				this->baseUrl = this->voiceConnectionData.endPoint.substr(0, this->voiceConnectionData.endPoint.find(":"));
				this->connectionState.store(VoiceConnectionState::Initializing_WebSocket);
				this->connectInternal();
				break;
			}
			case VoiceConnectionState::Initializing_WebSocket: {
				if (!WebSocketSSLShard::connect(this->baseUrl, "443")) {
					this->currentReconnectTries++;
					this->onClosedVoice();
					this->connectInternal();
					return;
				}
				this->theWebSocketState.store(DiscordCoreInternal::WebSocketSSLShardState::Upgrading);
				std::string sendVector = "GET /?v=4 HTTP/1.1\r\nHost: " + this->baseUrl +
					"\r\nPragma: no-cache\r\nUser-Agent: DiscordCoreAPI/1.0\r\nUpgrade: WebSocket\r\nConnection: Upgrade\r\nSec-WebSocket-Key: " +
					DiscordCoreAPI::generateBase64EncodedKey() + "\r\nSec-WebSocket-Version: 13\r\n\r\n";
				this->shard[0] = 0;
				this->shard[1] = 1;
				if (!this->sendMessage(sendVector, true)) {
					this->currentReconnectTries++;
					this->onClosedVoice();
					this->connectInternal();
					return;
				}
				DiscordCoreInternal::SSLClient::processIO(200000);
				if (!this->parseConnectionHeaders(this)) {
					this->currentReconnectTries++;
					this->onClosedVoice();
					this->connectInternal();
					return;
				}
				this->connectionState.store(VoiceConnectionState::Collecting_Hello);
				this->connectInternal();
				break;
			}
			case VoiceConnectionState::Collecting_Hello: {
				if (!this->collectAndProcessAMessage(VoiceConnectionState::Sending_Identify)) {
					this->currentReconnectTries++;
					this->onClosedVoice();
					this->connectInternal();
					return;
				}
				this->currentReconnectTries = 0;
				this->connectInternal();
				break;
			}
			case VoiceConnectionState::Sending_Identify: {
				this->haveWeReceivedHeartbeatAck = true;
				DiscordCoreInternal::VoiceIdentifyData identifyData{};
				identifyData.connectInitData = this->voiceConnectInitData;
				identifyData.connectionData = this->voiceConnectionData;
				std::string sendVector{};
				this->stringifyJsonData(identifyData, sendVector, DiscordCoreInternal::WebSocketOpCode::Op_Text);
				if (!this->sendMessage(sendVector, true)) {
					this->currentReconnectTries++;
					this->onClosedVoice();
					this->connectInternal();
					return;
				}
				this->connectionState.store(VoiceConnectionState::Collecting_Ready);
				this->connectInternal();
				break;
			}
			case VoiceConnectionState::Collecting_Ready: {
				if (!this->collectAndProcessAMessage(VoiceConnectionState::Initializing_DatagramSocket)) {
					this->currentReconnectTries++;
					this->onClosedVoice();
					this->connectInternal();
					return;
				}
				this->connectInternal();
				break;
			}
			case VoiceConnectionState::Initializing_DatagramSocket: {
				if (!this->voiceConnect()) {
					this->currentReconnectTries++;
					this->onClosedVoice();
					this->connectInternal();
					return;
				}
				this->connectionState.store(VoiceConnectionState::Sending_Select_Protocol);
				this->connectInternal();
				break;
			}
			case VoiceConnectionState::Sending_Select_Protocol: {
				DiscordCoreInternal::VoiceSocketProtocolPayloadData protocolPayloadData{};
				protocolPayloadData.voiceEncryptionMode = this->audioEncryptionMode;
				protocolPayloadData.externalIp = this->externalIp;
				protocolPayloadData.voicePort = this->port;
				nlohmann::json protocolPayloadSelectString = protocolPayloadData;
				std::string sendVector{};
				this->stringifyJsonData(protocolPayloadSelectString, sendVector, DiscordCoreInternal::WebSocketOpCode::Op_Text);
				if (!this->sendMessage(sendVector, true)) {
					this->currentReconnectTries++;
					this->onClosedVoice();
					this->connectInternal();
					return;
				}
				this->connectionState.store(VoiceConnectionState::Collecting_Session_Description);
				this->connectInternal();
				break;
			}
			case VoiceConnectionState::Collecting_Session_Description: {
				if (!this->collectAndProcessAMessage(VoiceConnectionState::Collecting_Init_Data)) {
					this->currentReconnectTries++;
					this->onClosedVoice();
					this->connectInternal();
					return;
				}
				this->baseShard->voiceConnectionDataBufferMap[this->voiceConnectInitData.guildId]->clearContents();
				this->connectionState.store(VoiceConnectionState::Collecting_Init_Data);
				if (this->streamType != StreamType::None) {
					if (this->streamType == StreamType::Source) {
						this->targetSocket = std::make_unique<DatagramSocketClient>(true);
					} else {
						this->targetSocket = std::make_unique<DatagramSocketClient>(false);
					}
					this->taskThread03 = std::make_unique<std::jthread>([=, this](std::stop_token stopToken) {
						this->runBridge(stopToken);
					});
				}
				this->targetSocket->connect(this->theStreamInfo.targetAddress, this->theStreamInfo.targetPort);
				return;
			}
		}
	}

	void VoiceConnection::clearAudioData() noexcept {
		if (this->audioData.encodedFrameData.data.size() != 0 && this->audioData.rawFrameData.data.size() != 0) {
			this->audioData.encodedFrameData.data.clear();
			this->audioData.rawFrameData.data.clear();
			this->audioData = AudioFrameData();
		}
		AudioFrameData frameData{};
		while (this->audioDataBuffer.tryReceive(frameData)) {
		};
	}

	bool VoiceConnection::areWeConnected() noexcept {
		if (this == nullptr) {
			return false;
		} else {
			return this->areWeConnectedBool.load();
		}
	}

	void VoiceConnection::sendHeartBeat() noexcept {
		try {
			if (WebSocketSSLShard::areWeStillConnected() && this->haveWeReceivedHeartbeatAck) {
				nlohmann::json data{};
				data["d"] = std::chrono::duration_cast<std::chrono::nanoseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
				data["op"] = int32_t(3);
				std::string theString{};
				this->stringifyJsonData(data, theString, DiscordCoreInternal::WebSocketOpCode::Op_Text);
				if (!this->sendMessage(theString, true)) {
					this->onClosedVoice();
				}
				this->haveWeReceivedHeartbeatAck = false;
			} else {
				this->onClosedVoice();
			}
		} catch (...) {
			if (this->configManager->doWePrintWebSocketErrorMessages()) {
				DiscordCoreAPI::reportException("VoiceConnection::sendHeartBeat()");
			}
			this->onClosedVoice();
		}
	}

	void VoiceConnection::onClosedVoice() noexcept {
		this->connectionState.store(VoiceConnectionState::Collecting_Init_Data);
		if (this->activeState.load() != VoiceActiveState::Exiting && this->currentReconnectTries < this->maxReconnectTries) {
			this->reconnect();
		} else if (this->currentReconnectTries >= this->maxReconnectTries) {
			this->disconnect();
		}
	}

	bool VoiceConnection::voiceConnect() noexcept {
		try {
			if (!DatagramSocketClient::areWeStillConnected()) {
				if (!DatagramSocketClient::connect(this->voiceIp, this->port)) {
					return false;
				} else {
					std::string packet{};
					packet.resize(74);
					uint16_t val1601{ 0x01 };
					uint16_t val1602{ 70 };
					packet[0] = static_cast<uint8_t>(val1601 >> 8);
					packet[1] = static_cast<uint8_t>(val1601 >> 0);
					packet[2] = static_cast<uint8_t>(val1602 >> 8);
					packet[3] = static_cast<uint8_t>(val1602 >> 0);
					packet[4] = static_cast<uint8_t>(this->audioSSRC >> 24);
					packet[5] = static_cast<uint8_t>(this->audioSSRC >> 16);
					packet[6] = static_cast<uint8_t>(this->audioSSRC >> 8);
					packet[7] = static_cast<uint8_t>(this->audioSSRC);
					DatagramSocketClient::getInputBuffer();
					DatagramSocketClient::writeData(packet);
					std::string inputString{};
					StopWatch theStopWatch{ 2500ms };
					while (inputString.size() < 74 && !this->doWeQuit->load() && this->activeState.load() != VoiceActiveState::Exiting) {
						DatagramSocketClient::processIO(10000);
						std::string theNewString = DatagramSocketClient::getInputBuffer();
						inputString.insert(inputString.end(), theNewString.begin(), theNewString.end());
						std::this_thread::sleep_for(1ms);
						if (theStopWatch.hasTimePassed()) {
							return false;
						}
					}
					std::string message{};
					message.insert(message.begin(), inputString.begin() + 8, inputString.begin() + 64);
					auto endLineFind = message.find('\u0000', 5);
					if (endLineFind != std::string::npos) {
						message = message.substr(0, endLineFind);
					}
					this->externalIp = message;
					this->areWeConnectedBool.store(true);
					this->voiceConnectionDataBuffer.clearContents();
					return true;
				}
			} else {
				return true;
			}
		} catch (...) {
			if (this->configManager->doWePrintWebSocketErrorMessages()) {
				DiscordCoreAPI::reportException("VoiceConnection::voiceConnect()");
			}
			this->onClosedVoice();
			return false;
		}
	}

	void VoiceConnection::sendSilence() noexcept {
		EncodedFrameData newFrame{};
		newFrame.data.push_back(0xf8);
		newFrame.data.push_back(0xff);
		newFrame.data.push_back(0xfe);
		auto theFrame = this->encryptSingleAudioFrame(newFrame);
		this->sendSingleAudioFrame(theFrame);
	}

	void VoiceConnection::pauseToggle() noexcept {
		if (this) {
			if (this->activeState.load() == VoiceActiveState::Paused) {
				this->activeState.store(VoiceActiveState::Playing);
			} else {
				this->activeState.store(VoiceActiveState::Paused);
			}
		}
	}

	void VoiceConnection::disconnect() noexcept {
		std::lock_guard theLock{ this->baseSocketAgent->theMutex };
		this->baseSocketAgent->voiceConnectionsToDisconnect.push(this->voiceConnectInitData.guildId);
		this->activeState.store(VoiceActiveState::Exiting);
	}

	void VoiceConnection::reconnect() noexcept {
		DatagramSocketClient::disconnect();
		WebSocketSSLShard::disconnect(true);
		this->areWeHeartBeating = false;
		this->wantWrite = true;
		this->wantRead = false;
		this->currentReconnectTries++;
		this->areWeConnectedBool.store(false);
	}

	void VoiceConnection::mixAudio() noexcept {
		StopWatch theStopWatch{ 20ms };
		if (this->thePayloads.size() > 0) {
			int16_t voiceUserCount{};
			std::vector<opus_int16> theUpsampledVector{};
			uint64_t sampleCount{};
			for (auto& [key, value]: this->thePayloads) {
				
				if (value.thePayloads.size() > 0) {
					auto thePayload = value.thePayloads.front();
					value.thePayloads.pop();
					thePayload.decodedData.resize(23040);
					std::vector<unsigned char> theEncodedData{};
					theEncodedData.resize(1276);

					sampleCount = opus_decode(value.theDecoder, reinterpret_cast<unsigned char*>(thePayload.theRawData.data()), thePayload.theRawData.size(),
						thePayload.decodedData.data(), 5760, 0);
					if (sampleCount <= 0) {
						std::cout << "Failed to decode user's voice payload." << std::endl;
					} else {
						voiceUserCount++;
						theUpsampledVector.resize(sampleCount * 2);
						for (uint32_t x = 0; x < sampleCount * 2; x++) {
							theUpsampledVector[x] += static_cast<opus_int16>(thePayload.decodedData[x]);
						}
					}
				}
				if (value.currentTimeStamp != 0 && theStopWatch.hasTimePassed()) {
					theStopWatch.resetTimer();
					value.currentTimeStamp += 960;
				}
			}
			if (sampleCount > 0) {
				std::vector<opus_int16> theDownsampledVector{};
				theDownsampledVector.resize(sampleCount * 2);
				std::cout << "VICOE USER COUNT: " << voiceUserCount << std::endl;
				for (int32_t x = 0; x < theUpsampledVector.size(); x++) {
					theDownsampledVector[x] = static_cast<opus_int16>(theUpsampledVector[x] / voiceUserCount);
				}
				std::vector<char> theEncodedData{};
				theEncodedData.resize(1276);
				auto byteCount = opus_encode(this->theEncoder, theDownsampledVector.data(), 960, reinterpret_cast<unsigned char*>(theEncodedData.data()), 1276);
				if (byteCount <= 0) {
					std::cout << "Failed to encode user's voice payload." << std::endl;
				} else {
					std::string theFinalString{};
					theFinalString.insert(theFinalString.begin(), theEncodedData.begin(), theEncodedData.begin() + byteCount);
					this->targetSocket->writeData(theFinalString);
				}
			}
		}
	}

	void VoiceConnection::connect() noexcept {
		if (this->baseSocketAgent->sslShards.contains(this->voiceConnectInitData.currentShard) && this->baseSocketAgent->sslShards[voiceConnectInitData.currentShard]) {
			ConnectionPackage dataPackage{};
			dataPackage.currentShard = 0;
			this->connections.push(dataPackage);
			this->activeState.store(VoiceActiveState::Connecting);
			if (!this->taskThread01) {
				this->taskThread01 = std::make_unique<std::jthread>([=, this](std::stop_token stopToken) {
					this->runWebSocket(stopToken);
				});
			}

			if (!this->taskThread02) {
				this->taskThread02 = std::make_unique<std::jthread>([=, this](std::stop_token stopToken) {
					this->runVoice(stopToken);
				});
			}
		}
	}

	bool VoiceConnection::stop() noexcept {
		this->sendSpeakingMessage(false);
		this->activeState.store(VoiceActiveState::Stopped);
		return true;
	}

	bool VoiceConnection::play() noexcept {
		this->activeState.store(VoiceActiveState::Playing);
		return true;
	}

	VoiceConnection::~VoiceConnection() noexcept {
		std::lock_guard theLock{ DatagramSocketClient::theMutex };
		std::lock_guard theLock2{ WebSocketSSLShard::theMutex };
	};

};