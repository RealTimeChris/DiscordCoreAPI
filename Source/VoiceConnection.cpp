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

	void OpusDecoderWrapper::OpusDecoderDeleter::operator()(OpusDecoder* other) noexcept {
		if (other) {
			opus_decoder_destroy(other);
			other = nullptr;
		}
	}

	OpusDecoderWrapper& OpusDecoderWrapper::operator=(OpusDecoderWrapper&& other) noexcept {
		if (this != &other) {
			this->thePtr.reset(nullptr);
			this->thePtr.reset(other.thePtr.release());
		}
		return *this;
	}

	OpusDecoderWrapper::OpusDecoderWrapper(OpusDecoderWrapper&& other) noexcept {
		*this = std::move(other);
	}

	OpusDecoderWrapper::OpusDecoderWrapper() {
		int32_t error{};
		this->thePtr.reset(nullptr);
		this->thePtr.reset(opus_decoder_create(48000, 2, &error));
		if (error != OPUS_OK) {
			throw std::runtime_error{ "Failed to create the Opus Decoder" };
		}
	}

	OpusDecoderWrapper::operator OpusDecoder*() noexcept {
		return this->thePtr.get();
	}

	RTPPacket::RTPPacket(uint32_t timestampNew, uint16_t sequenceNew, uint32_t ssrcNew, std::vector<uint8_t>& audioDataNew, const std::string& theKeysNew) noexcept {
		this->audioData = std::move(audioDataNew);
		this->timestamp = timestampNew;
		this->sequence = sequenceNew;
		this->theKeys = theKeysNew;
		this->ssrc = ssrcNew;
	}

	RTPPacket::operator std::string() noexcept {
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
			for (uint8_t x = 0; x < headerSize; ++x) {
				nonceForLibSodium[x] = header[x];
			}
			for (uint8_t x = headerSize; x < nonceSize; ++x) {
				nonceForLibSodium[x] = 0;
			}
			uint64_t numOfBytes{ headerSize + this->audioData.size() + crypto_secretbox_MACBYTES };
			std::unique_ptr<uint8_t[]> audioDataPacket{ std::make_unique<uint8_t[]>(numOfBytes) };
			for (uint8_t x = 0; x < headerSize; ++x) {
				audioDataPacket[x] = header[x];
			}
			std::unique_ptr<uint8_t[]> encryptionKeys{ std::make_unique<uint8_t[]>(this->theKeys.size()) };
			for (uint64_t x = 0; x < this->theKeys.size(); ++x) {
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

	VoiceConnection::VoiceConnection(DiscordCoreInternal::BaseSocketAgent* BaseSocketAgentNew, DiscordCoreInternal::WebSocketSSLShard* baseShard,
		const DiscordCoreInternal::VoiceConnectInitData& initDataNew, DiscordCoreAPI::ConfigManager* configManagerNew, std::atomic_bool* doWeQuitNew, StreamType streamTypeNew,
		StreamInfo streamInfoNew) noexcept
		: WebSocketSSLShard(BaseSocketAgentNew->discordCoreClient, &this->theConnections, initDataNew.currentShard, this->doWeQuit), DatagramSocketClient(StreamType::None) {
		this->activeState.store(VoiceActiveState::Connecting);
		this->baseSocketAgent = BaseSocketAgentNew;
		this->voiceConnectInitData = initDataNew;
		this->configManager = configManagerNew;
		this->theStreamInfo = streamInfoNew;
		this->streamType = streamTypeNew;
		this->doWeQuit = doWeQuitNew;
		this->baseShard = baseShard;
	}

	Snowflake VoiceConnection::getChannelId() noexcept {
		return this->voiceConnectInitData.channelId;
	}

	std::string VoiceConnection::encryptSingleAudioFrame(AudioFrameData& bufferToSend) noexcept {
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

	UnboundedMessageBlock<AudioFrameData>& VoiceConnection::getAudioBuffer() noexcept {
		return this->audioDataBuffer;
	}

	void VoiceConnection::sendSingleFrame(AudioFrameData& frameData) noexcept {
		this->audioDataBuffer.send(std::move(frameData));
	}

	bool VoiceConnection::onMessageReceived(const std::string& theData) noexcept {
		std::unique_lock theLock00{ this->voiceUserMutex, std::defer_lock_t{} };
		try {
			if (WebSocketSSLShard::inputBuffer.size() > 0) {
				nlohmann::json payload = payload.parse(theData);
				if (this->configManager->doWePrintWebSocketSuccessMessages()) {
					cout << shiftToBrightGreen() << "Message received from Voice WebSocket: " << payload << reset() << endl << endl;
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
							return true;
						}
						case 4: {
							std::string theSecretKey{};
							for (uint32_t x = 0; x < payload["d"]["secret_key"].size(); ++x) {
								theSecretKey.push_back(payload["d"]["secret_key"][x].get<uint8_t>());
							}
							this->secretKeySend = theSecretKey;
							this->connectionState.store(VoiceConnectionState::Collecting_Init_Data);
							return true;
						}
						case 5: {
							if (payload.contains("d") && !payload["d"].is_null()) {
								if (!payload["d"]["ssrc"].is_null()) {
									VoiceUser theUser{};
									theUser.theUserId = stoull(payload["d"]["user_id"].get<std::string>());
									theLock00.lock();
									this->voiceUsers[payload["d"]["ssrc"].get<int32_t>()] = std::move(theUser);
								}
							}
							return true;
						}
						case 6: {
							this->haveWeReceivedHeartbeatAck = true;
							return true;
						}
						case 8: {
							if (payload["d"].contains("heartbeat_interval")) {
								this->heartBeatStopWatch = StopWatch{ std::chrono::milliseconds{ static_cast<int32_t>(payload["d"]["heartbeat_interval"].get<float>()) } };
								this->areWeHeartBeating = true;
								this->connectionState.store(VoiceConnectionState::Sending_Identify);
							}
							return true;
						}
						case 13: {
							if (payload.contains("d") && !payload["d"].is_null()) {
								if (!payload["d"]["user_id"].is_null()) {
									auto userId = stoull(payload["d"]["user_id"].get<std::string>());
									for (auto& [key, value]: this->voiceUsers) {
										if (userId == value.theUserId) {
											theLock00.lock();
											this->voiceUsers.erase(key);
											break;
										}
									}
								}
							}
							return true;
						}
					}
				}
			}
			return false;
		} catch (...) {
			if (this->configManager->doWePrintWebSocketErrorMessages()) {
				reportException("VoiceConnection::onMessageReceived()");
			}
			this->onClosed();
			return false;
		}
	}

	void VoiceConnection::reconnectStream() noexcept {
		this->streamSocket->connect(this->theStreamInfo.address, this->theStreamInfo.port);
	}

	void VoiceConnection::sendVoiceData(std::string& responseData) noexcept {
		try {
			if (responseData.size() == 0) {
				if (this->configManager->doWePrintWebSocketErrorMessages()) {
					cout << shiftToBrightRed() << "Please specify voice data to send" << reset() << endl << endl;
				}
				return;
			} else {
				if (DatagramSocketClient::areWeStillConnected()) {
					DatagramSocketClient::writeData(std::move(responseData));
				} else {
					this->onClosed();
				}
			}
		} catch (...) {
			if (this->configManager->doWePrintWebSocketErrorMessages()) {
				reportException("VoiceConnection::sendVoiceData()");
			}
			this->onClosed();
		}
	}

	void VoiceConnection::sendSpeakingMessage(const bool isSpeaking) noexcept {
		DiscordCoreInternal::SendSpeakingData theData{};
		if (!isSpeaking) {
			theData.type = static_cast<DiscordCoreInternal::SendSpeakingType>(0);
			this->sendSilence();
			DatagramSocketClient::processIO(DiscordCoreInternal::ProcessIOType::Write_Only);
		} else {
			theData.type = DiscordCoreInternal::SendSpeakingType::Microphone;
			theData.delay = 0;
			theData.ssrc = this->audioSSRC;
			nlohmann::json newString = theData;
			std::string theString{};
			this->stringifyJsonData(&newString, theString, DiscordCoreInternal::WebSocketOpCode::Op_Text);
			if (!this->sendMessage(theString, true)) {
				this->onClosed();
			}
		}
	}

	void VoiceConnection::runWebSocket(std::stop_token stopToken) noexcept {
		try {
			while (!stopToken.stop_requested() && !this->doWeQuit->load() && this->activeState.load() != VoiceActiveState::Exiting) {
				if (!stopToken.stop_requested() && this->theConnections.size() > 0) {
					this->theConnections.pop_front();
					StopWatch theStopWatch{ 10000ms };
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
				if (!stopToken.stop_requested() && WebSocketSSLShard::areWeStillConnected() && this->heartBeatStopWatch.hasTimePassed() && this->areWeHeartBeating) {
					this->sendHeartBeat();
					this->heartBeatStopWatch.resetTimer();
				}
				if (!stopToken.stop_requested() && WebSocketSSLShard::areWeStillConnected()) {
					WebSocketSSLShard::processIO(10000);
				}
				if (!stopToken.stop_requested() && WebSocketSSLShard::areWeStillConnected() && WebSocketSSLShard::inputBuffer.size() > 0) {
					this->parseMessage(this);
				}

				std::this_thread::sleep_for(1ms);
			}
		} catch (...) {
			if (this->configManager->doWePrintWebSocketErrorMessages()) {
				reportException("VoiceConnection::run()");
			}
			this->onClosed();
		}
	}

	void VoiceConnection::runBridge(std::stop_token theToken) noexcept {
		StopWatch theStopWatch{ 20ms };
		int32_t timeToWaitInMs{ 20 };
		int32_t timeTakesToSleep{ 0 };
		while (!theToken.stop_requested()) {
			if (theStopWatch.hasTimePassed()) {
				theStopWatch.resetTimer();
				for (uint32_t x = 0; x < this->voiceUsers.size(); ++x) {
					DatagramSocketClient::processIO(DiscordCoreInternal::ProcessIOType::Both);
					std::string theString = DatagramSocketClient::getInputBuffer();
					DatagramSocketClient::getInputBuffer().clear();
					VoicePayload thePayload{};
					thePayload.theRawData.insert(thePayload.theRawData.begin(), theString.begin(), theString.end());
					this->theFrameQueue.emplace_back(thePayload);
				}
				this->streamSocket->processIO(DiscordCoreInternal::ProcessIOType::Both);
				this->parseIncomingVoiceData();
				this->streamSocket->processIO(DiscordCoreInternal::ProcessIOType::Both);
				this->mixAudio();
			}
			if (timeTakesToSleep == 0) {
				theStopWatch.resetTimer();
			}
			if (theStopWatch.totalTimePassed() + timeTakesToSleep <= timeToWaitInMs) {
				std::this_thread::sleep_for(1ms);
			}
			if (timeTakesToSleep == 0) {
				timeTakesToSleep = theStopWatch.totalTimePassed();
			}
		}
	}

	bool VoiceConnection::collectAndProcessAMessage(VoiceConnectionState stateToWaitFor) noexcept {
		StopWatch theStopWatch{ 2500ms };
		while (!this->doWeQuit->load() && this->connectionState.load() != stateToWaitFor) {
			WebSocketSSLShard::processIO(10000);
			if (!WebSocketSSLShard::areWeStillConnected()) {
				return false;
			}
			if (WebSocketSSLShard::inputBuffer.size() > 0) {
				if (!VoiceConnection::parseMessage(this)) {
					return true;
				}
			}
			if (theStopWatch.hasTimePassed()) {
				return false;
			}
			std::this_thread::sleep_for(1ms);
		}
		return false;
	}

	void VoiceConnection::runVoice(std::stop_token stopToken) noexcept {
		StopWatch theStopWatch{ 20000ms };
		StopWatch theSendSilenceStopWatch{ 5000ms };
		while (!stopToken.stop_requested() && !this->doWeQuit->load() && this->activeState.load() != VoiceActiveState::Exiting) {
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
						DatagramSocketClient::processIO(DiscordCoreInternal::ProcessIOType::Both);
						DatagramSocketClient::getInputBuffer().clear();
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
						DatagramSocketClient::processIO(DiscordCoreInternal::ProcessIOType::Both);
						DatagramSocketClient::getInputBuffer().clear();
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
					int64_t frameCounter{ 0 };
					DoubleTimePointNs totalTime{ std::chrono::nanoseconds{ 0 } };
					if (!this->areWePlaying.load()) {
						this->sendSpeakingMessage(true);
						this->areWePlaying.store(true);
					}

					this->audioData.type = AudioFrameType::Encoded;
					this->audioData.data.clear();

					theStopWatch.resetTimer();
					while (!stopToken.stop_requested() && !DatagramSocketClient::areWeStillConnected()) {
						if (theStopWatch.hasTimePassed() || this->activeState.load() == VoiceActiveState::Exiting) {
							return;
						}
						std::this_thread::sleep_for(1ms);
					}

					while (!stopToken.stop_requested() && this->activeState.load() == VoiceActiveState::Playing) {
						this->audioDataBuffer.tryReceive(this->audioData);
						if (!this->streamSocket) {
							while (this->theFrameQueue.size() > 0) {
								this->theFrameQueue.pop_front();
							}
						}
						frameCounter++;
						if (this->audioData.guildMemberId != 0) {
							this->currentGuildMemberId = this->audioData.guildMemberId;
						}
						std::string newFrame{};
						bool doWeBreak{ false };
						switch (this->audioData.type) {
							case AudioFrameType::RawPCM: {
								auto encodedFrameData = this->theEncoder.encodeSingleAudioFrame(this->audioData);
								newFrame = this->encryptSingleAudioFrame(encodedFrameData);
								break;
							}
							case AudioFrameType::Encoded: {
								newFrame = this->encryptSingleAudioFrame(this->audioData);
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
						auto waitTime = targetTime - std::chrono::system_clock::now();
						nanoSleep(static_cast<uint64_t>(static_cast<double>(waitTime.count()) * 0.95f));
						waitTime = targetTime - std::chrono::system_clock::now();
						if (waitTime.count() > 0 && static_cast<uint64_t>(waitTime.count()) < 20000000) {
							spinLock(waitTime.count());
						}
						startingValue = std::chrono::system_clock::now();
						if (newFrame.size() > 0) {
							this->sendVoiceData(newFrame);
						}
						if (this->streamType == StreamType::None) {
							DatagramSocketClient::processIO(DiscordCoreInternal::ProcessIOType::Both);
							DatagramSocketClient::getInputBuffer().clear();
						} else {
							DatagramSocketClient::processIO(DiscordCoreInternal::ProcessIOType::Write_Only);
						}

						this->audioData.data.clear();
						this->audioData.sampleCount = 0;
						this->audioData.type = AudioFrameType::Unset;
						totalTime += std::chrono::system_clock::now() - startingValue;
						auto intervalCountNew = DoubleTimePointNs{ std::chrono::nanoseconds{ 20000000 } - totalTime.time_since_epoch() / frameCounter }.time_since_epoch().count();
						intervalCount = DoubleTimePointNs{ std::chrono::nanoseconds{ static_cast<uint64_t>(intervalCountNew) } };
						targetTime = std::chrono::system_clock::now().time_since_epoch() + intervalCount;
					}
					break;
				}
				case VoiceActiveState::Exiting: {
					return;
				}
			}
			if (stopToken.stop_requested()) {
				return;
			}
			std::this_thread::sleep_for(1ms);
		}
	};

	void VoiceConnection::parseIncomingVoiceData() noexcept {
		while (this->theFrameQueue.size() > 0) {
			VoicePayload theBuffer = this->theFrameQueue.front();
			this->theFrameQueue.pop_front();
			if (theBuffer.theRawData.size() > 0 && this->secretKeySend.size() > 0) {
				std::vector<uint8_t> packet{};
				packet.insert(packet.begin(), theBuffer.theRawData.begin(), theBuffer.theRawData.end());
				theBuffer.theRawData.clear();
				constexpr size_t headerSize{ 12 };
				if (packet.size() < headerSize) {
					return;
				}

				if (uint8_t payloadType = packet[1] & 0b0111'1111; 72 <= payloadType && payloadType <= 76) {
					return;
				}
				uint32_t speakerSsrc{ *reinterpret_cast<uint32_t*>(packet.data() + 8) };
				speakerSsrc = ntohl(speakerSsrc);

				std::vector<uint8_t> nonce{};
				nonce.resize(24);
				for (uint32_t x = 0; x < headerSize; ++x) {
					nonce[x] = packet[x];
				}
				const size_t csrcCount = packet[0] & 0b0000'1111;
				const ptrdiff_t offsetToData = headerSize + sizeof(uint32_t) * csrcCount;
				uint8_t* encryptedData = packet.data() + offsetToData;
				const size_t encryptedDataLength = packet.size() - offsetToData;
				std::vector<uint8_t> theKey{};
				theKey.insert(theKey.begin(), this->secretKeySend.begin(), this->secretKeySend.end());
				std::vector<uint8_t> decryptedData{};
				decryptedData.resize(encryptedDataLength);
				if (crypto_secretbox_open_easy(decryptedData.data(), encryptedData, encryptedDataLength, nonce.data(), theKey.data())) {
					return;
				}

				std::string decryptedDataString{};
				decryptedDataString.insert(decryptedDataString.begin(), decryptedData.begin(), decryptedData.end());

				if ((packet[0] >> 4) & 0b0001) {
					size_t extLen = 0;
					uint16_t extLengthInWords{ *reinterpret_cast<uint16_t*>(decryptedDataString.data() + 2) };
					extLengthInWords = ntohs(extLengthInWords);
					extLen = sizeof(uint32_t) * extLengthInWords;
					constexpr size_t extHeaderLen = sizeof(uint16_t) * 2;
					decryptedDataString = decryptedDataString.substr(extHeaderLen + extLen);
				}
				if (decryptedDataString.size() > 0 && (decryptedDataString.size() - 16) > 0) {
					theBuffer.theRawData.insert(theBuffer.theRawData.begin(), decryptedDataString.begin(), decryptedDataString.end() - 16);
					theBuffer.decodedData.resize(23040);
					std::unique_lock theLock00{ this->voiceUserMutex };
					if (this->voiceUsers.contains(speakerSsrc)) {
						auto sampleCount = opus_decode(this->voiceUsers[speakerSsrc].theDecoder, theBuffer.theRawData.data(), static_cast<opus_int32>(theBuffer.theRawData.size()),
							theBuffer.decodedData.data(), 5760, 0);
						if (sampleCount <= 0) {
							if (this->configManager->doWePrintGeneralErrorMessages()) {
								cout << "Failed to decode user's voice payload." << std::endl;
							}
						} else {
							theBuffer.decodedData.resize(static_cast<uint64_t>(sampleCount) * 2);
							this->voiceUsers[speakerSsrc].thePayloads.push(std::move(theBuffer));
						}
					}
				}
			}
		}

		std::string theBuffer = this->streamSocket->getInputBuffer();
		this->streamSocket->getInputBuffer().clear();

		AudioFrameData theFrame{};
		theFrame.data.insert(theFrame.data.begin(), theBuffer.begin(), theBuffer.end());
		theFrame.sampleCount = 960;
		theFrame.type = AudioFrameType::Encoded;
		this->audioDataBuffer.send(std::move(theFrame));
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
		if (this->taskThread03) {
			if (this->streamSocket) {
				this->streamSocket->disconnect();
			}
			this->taskThread03->request_stop();
			if (this->taskThread03->joinable()) {
				this->taskThread03->join();
			}
			this->taskThread03.reset(nullptr);
		}

		DatagramSocketClient::disconnect();
		WebSocketSSLShard::disconnect(false);
		if (this->streamSocket && this->streamSocket->areWeStillConnected()) {
			this->streamSocket->disconnect();
		}
		auto thePtr = getSongAPIMap()[this->voiceConnectInitData.guildId].get();
		if (thePtr) {
			thePtr->onSongCompletionEvent.remove(thePtr->eventToken);
		}
		this->areWeConnectedBool.store(false);
		this->connectionState.store(VoiceConnectionState::Collecting_Init_Data);
		this->activeState.store(VoiceActiveState::Connecting);
	}

	void VoiceConnection::connectInternal() noexcept {
		if (this->thePackage.currentShard == -1) {
			return;
		}
		if (this->currentReconnectTries >= this->maxReconnectTries) {
			this->doWeQuit->store(true);
			if (this->configManager->doWePrintWebSocketErrorMessages()) {
				cout << "VoiceConnection::connectInternal() Error: Failed to connect to voice channel!" << endl << endl;
			}
			return;
		}
		switch (this->connectionState.load()) {
			case VoiceConnectionState::Collecting_Init_Data: {
				this->baseShard->voiceConnectionDataBufferMap[this->voiceConnectInitData.guildId] = &this->voiceConnectionDataBuffer;
				this->baseShard->voiceConnectionDataBufferMap[this->voiceConnectInitData.guildId]->clearContents();
				this->baseShard->getVoiceConnectionData(this->voiceConnectInitData);

				if (waitForTimeToPass(this->voiceConnectionDataBuffer, this->voiceConnectionData, 10000)) {
					this->currentReconnectTries++;
					this->onClosed();
					this->connectInternal();
					return;
				}
				this->baseUrl = this->voiceConnectionData.endPoint.substr(0, this->voiceConnectionData.endPoint.find(":"));
				this->connectionState.store(VoiceConnectionState::Initializing_WebSocket);
				this->connectInternal();
				break;
			}
			case VoiceConnectionState::Initializing_WebSocket: {
				if (WebSocketSSLShard::connect(this->baseUrl, "443", this->configManager->doWePrintWebSocketErrorMessages()) != DiscordCoreInternal::ConnectionResult::No_Error) {
					this->currentReconnectTries++;
					this->onClosed();
					this->connectInternal();
					return;
				}
				this->theWebSocketState.store(DiscordCoreInternal::WebSocketSSLShardState::Upgrading);
				std::string sendVector = "GET /?v=4 HTTP/1.1\r\nHost: " + this->baseUrl +
					"\r\nPragma: no-cache\r\nUser-Agent: DiscordCoreAPI/1.0\r\nUpgrade: WebSocket\r\nConnection: Upgrade\r\nSec-WebSocket-Key: " + generateBase64EncodedKey() +
					"\r\nSec-WebSocket-Version: 13\r\n\r\n";
				this->shard[0] = 0;
				this->shard[1] = 1;
				if (!this->sendMessage(sendVector, true)) {
					this->currentReconnectTries++;
					this->onClosed();
					this->connectInternal();
					return;
				}
				WebSocketSSLShard::processIO(200000);
				if (!this->parseConnectionHeaders(this)) {
					this->currentReconnectTries++;
					this->onClosed();
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
					this->onClosed();
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
				nlohmann::json theData{ identifyData };
				this->stringifyJsonData(&theData[0], sendVector, DiscordCoreInternal::WebSocketOpCode::Op_Text);
				if (!this->sendMessage(sendVector, true)) {
					this->currentReconnectTries++;
					this->onClosed();
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
					this->onClosed();
					this->connectInternal();
					return;
				}
				this->connectInternal();
				break;
			}
			case VoiceConnectionState::Initializing_DatagramSocket: {
				if (!this->voiceConnect()) {
					this->currentReconnectTries++;
					this->onClosed();
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
				this->stringifyJsonData(&protocolPayloadSelectString, sendVector, DiscordCoreInternal::WebSocketOpCode::Op_Text);
				if (!this->sendMessage(sendVector, true)) {
					this->currentReconnectTries++;
					this->onClosed();
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
					this->onClosed();
					this->connectInternal();
					return;
				}
				this->baseShard->voiceConnectionDataBufferMap[this->voiceConnectInitData.guildId]->clearContents();
				this->connectionState.store(VoiceConnectionState::Collecting_Init_Data);
				if (this->streamType != StreamType::None) {
					this->streamSocket = std::make_unique<DatagramSocketClient>(this->streamType);
					if (this->taskThread03) {
						this->taskThread03.reset(nullptr);
					}
					this->taskThread03 = std::make_unique<std::jthread>([=, this](std::stop_token stopToken) {
						this->runBridge(stopToken);
					});
					this->streamSocket->connect(this->theStreamInfo.address, this->theStreamInfo.port);
				}
				this->thePackage.currentShard = -1;
				return;
			}
		}
	}

	void VoiceConnection::clearAudioData() noexcept {
		if (this->audioData.data.size() != 0) {
			this->audioData.data.clear();
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
				this->stringifyJsonData(&data, theString, DiscordCoreInternal::WebSocketOpCode::Op_Text);
				if (!this->sendMessage(theString, true)) {
					this->onClosed();
				}
				this->haveWeReceivedHeartbeatAck = false;
			} else {
				this->onClosed();
			}
		} catch (...) {
			if (this->configManager->doWePrintWebSocketErrorMessages()) {
				reportException("VoiceConnection::sendHeartBeat()");
			}
			this->onClosed();
		}
	}

	void VoiceConnection::onClosed() noexcept {
		this->connectionState.store(VoiceConnectionState::Collecting_Init_Data);
		if (this->activeState.load() != VoiceActiveState::Exiting && this->currentReconnectTries < this->maxReconnectTries) {
			this->reconnect();
		} else if (this->currentReconnectTries >= this->maxReconnectTries) {
			VoiceConnection::disconnect();
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
					DatagramSocketClient::writeData(std::move(packet));
					std::string inputString{};
					StopWatch theStopWatch{ 2500ms };
					while (inputString.size() < 74 && !this->doWeQuit->load() && this->activeState.load() != VoiceActiveState::Exiting) {
						DatagramSocketClient::processIO(DiscordCoreInternal::ProcessIOType::Both);
						inputString.insert(inputString.end(), DatagramSocketClient::getInputBuffer().data(),
							DatagramSocketClient::getInputBuffer().data() + DatagramSocketClient::getInputBuffer().size());
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
				reportException("VoiceConnection::voiceConnect()");
			}
			this->onClosed();
			return false;
		}
	}

	void VoiceConnection::sendSilence() noexcept {
		AudioFrameData newFrame{};
		newFrame.data.push_back(0xf8);
		newFrame.data.push_back(0xff);
		newFrame.data.push_back(0xfe);
		auto theFrame = this->encryptSingleAudioFrame(newFrame);
		if (theFrame.size() > 0) {
			this->sendVoiceData(theFrame);
		}
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
		std::unique_lock theLock{ this->baseSocketAgent->theConnectDisconnectMutex };
		this->baseSocketAgent->voiceConnectionsToDisconnect.push_back(this->voiceConnectInitData.guildId);
		this->activeState.store(VoiceActiveState::Exiting);
	}

	void VoiceConnection::reconnect() noexcept {
		DatagramSocketClient::disconnect();
		WebSocketSSLShard::disconnect(true);
		if (this->taskThread03) {
			this->taskThread03.reset(nullptr);
		}
		this->areWeHeartBeating = false;
		this->wantWrite = true;
		this->wantRead = false;
		this->currentReconnectTries++;
		this->areWeConnectedBool.store(false);
		this->thePackage.currentShard = 0;
	}

	void VoiceConnection::mixAudio() noexcept {
		if (this->voiceUsers.size() > 0) {
			opus_int32 voiceUserCount{};
			std::vector<opus_int32> theUpsampledVector{};
			for (auto& [key, value]: this->voiceUsers) {
				if (value.thePayloads.size() > 0) {
					std::unique_lock theLock00{ this->voiceUserMutex };
					VoicePayload thePayload = value.thePayloads.front();
					if (value.thePayloads.size() > 0) {
						value.thePayloads.pop();
					}
					theLock00.unlock();
					if (thePayload.decodedData.size() > 0) {
						voiceUserCount++;
						if (theUpsampledVector.size() == 0) {
							theUpsampledVector.resize(thePayload.decodedData.size());
						}
						for (uint32_t x = 0; x < thePayload.decodedData.size(); ++x) {
							if (thePayload.decodedData[x] >= INT16_MAX) {
								theUpsampledVector[x] += INT16_MAX;
							} else if (thePayload.decodedData[x] <= INT16_MIN) {
								theUpsampledVector[x] += INT16_MIN;
							} else {
								theUpsampledVector[x] += static_cast<opus_int32>(thePayload.decodedData[x]);
							}
						}
					}
				}
			}
			if (theUpsampledVector.size() > 0) {
				std::vector<opus_int16> theDownsampledVector{};
				theDownsampledVector.resize(theUpsampledVector.size());
				for (int32_t x = 0; x < theUpsampledVector.size(); ++x) {
					theDownsampledVector[x] = static_cast<opus_int16>(theUpsampledVector[x] / voiceUserCount);
				}
				auto theEncodedData = this->theEncoder.encodeSingleAudioFrame(theDownsampledVector);
				if (theEncodedData.data.size() <= 0) {
					if (this->configManager->doWePrintGeneralErrorMessages()) {
						cout << "Failed to encode user's voice payload." << endl;
					}
				} else {
					std::string theFinalString{};
					theFinalString.insert(theFinalString.begin(), theEncodedData.data.data(), theEncodedData.data.data() + theEncodedData.data.size());
					this->streamSocket->writeData(theFinalString);
				}
			}
		}
	}

	void VoiceConnection::connect() noexcept {
		if (this->baseSocketAgent) {
			this->thePackage.currentShard = 1;
			this->theConnections.push_back(this->thePackage);
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

};