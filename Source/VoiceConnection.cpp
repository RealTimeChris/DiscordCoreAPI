/*
	DiscordCoreAPI, A bot library for Discord, written in C++, and featuring explicit multithreading through the usage of custom, asynchronous C++ CoRoutines.
	Copyright 2021, 2022 Chris M. (RealTimeChris)
	This library is free software; you can redistribute it and/or
	modify it under the terms of the GNU Lesser General Public
	License as published by the Free Software Foundation; either
	version 2.1 of the License, or (at your option) any later version.
	This library is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
	Lesser General Public License for more details.
	You should have received a copy of the GNU Lesser General Public
	License along with this library; if not, write to the Free Software
	Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301
	USA
*/
/// VoiceConnection.cpp - Source file for the voice connection class.
/// Jul 15, 2021
/// https://discordcoreapi.com
/// \file VoiceConnection.cpp

#include <discordcoreapi/VoiceConnection.hpp>
#include <discordcoreapi/DiscordCoreClient.hpp>

namespace DiscordCoreAPI {

	constexpr Uint16 webSocketMaxPayloadLengthLarge{ 65535u };
	constexpr Uint8 webSocketPayloadLengthMagicLarge{ 126u };
	constexpr Uint8 webSocketPayloadLengthMagicHuge{ 127u };
	constexpr Uint8 maxHeaderSize{ sizeof(Uint64) + 2u };
	constexpr Uint8 webSocketMaxPayloadLengthSmall{ 125u };
	constexpr Uint8 webSocketFinishBit{ (1u << 7u) };
	constexpr Uint8 webSocketMaskBit{ (1u << 7u) };

	VoiceSocketReadyData::VoiceSocketReadyData(simdjson::ondemand::value jsonObjectData) {
		this->ip = getString(jsonObjectData, "ip");
		this->ssrc = getUint32(jsonObjectData, "ssrc");
		simdjson::ondemand::array theArray{};
		auto theResult = jsonObjectData["modes"].get(theArray);
		if (theResult == simdjson::error_code::SUCCESS) {
			this->mode.clear();
			for (simdjson::simdjson_result<simdjson::fallback::ondemand::value> value: theArray) {
				if (String{ value.get_string().take_value() } == "xsalsa20_poly1305") {
					this->mode = String{ value.get_string().take_value() };
				}
			}
		}
		this->port = getUint64(jsonObjectData, "port");
	}

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
		Int32 error{};
		this->thePtr.reset(nullptr);
		this->thePtr.reset(opus_decoder_create(48000, 2, &error));
		if (error != OPUS_OK) {
			throw std::runtime_error{ "Failed to create the Opus Decoder" };
		}
	}

	OpusDecoderWrapper::operator OpusDecoder*() noexcept {
		return this->thePtr.get();
	}

	RTPPacket::RTPPacket(Uint32 timestampNew, Uint16 sequenceNew, Uint32 ssrcNew, std::vector<Uint8>& audioDataNew, const String& theKeysNew) noexcept {
		this->audioData = std::move(audioDataNew);
		this->timestamp = timestampNew;
		this->sequence = sequenceNew;
		this->theKeys = theKeysNew;
		this->ssrc = ssrcNew;
	}

	RTPPacket::operator String() noexcept {
		if (this->theKeys.size() > 0) {
			const Uint8 nonceSize{ crypto_secretbox_NONCEBYTES };
			const Uint8 headerSize{ 12 };
			const Uint8 byteSize{ 8 };
			String header{};
			storeBits(header, this->version);
			storeBits(header, this->flags);
			storeBits(header, this->sequence);
			storeBits(header, this->timestamp);
			storeBits(header, this->ssrc);
			std::unique_ptr<Uint8[]> nonceForLibSodium{ std::make_unique<Uint8[]>(nonceSize) };
			for (Uint8 x = 0; x < headerSize; ++x) {
				nonceForLibSodium[x] = header[x];
			}
			for (Uint8 x = headerSize; x < nonceSize; ++x) {
				nonceForLibSodium[x] = 0;
			}
			Uint64 numOfBytes{ headerSize + this->audioData.size() + crypto_secretbox_MACBYTES };
			std::unique_ptr<Uint8[]> audioDataPacket{ std::make_unique<Uint8[]>(numOfBytes) };
			for (Uint8 x = 0; x < headerSize; ++x) {
				audioDataPacket[x] = header[x];
			}
			std::unique_ptr<Uint8[]> encryptionKeys{ std::make_unique<Uint8[]>(this->theKeys.size()) };
			for (Uint64 x = 0; x < this->theKeys.size(); ++x) {
				encryptionKeys[x] = this->theKeys[x];
			}
			if (crypto_secretbox_easy(audioDataPacket.get() + headerSize, this->audioData.data(), this->audioData.size(), nonceForLibSodium.get(), encryptionKeys.get()) != 0) {
				return "";
			};
			String audioDataPacketNew{};
			audioDataPacketNew.insert(audioDataPacketNew.begin(), audioDataPacket.get(), audioDataPacket.get() + numOfBytes);
			return audioDataPacketNew;
		}
		return {};
	}

	VoiceConnection::VoiceConnection(DiscordCoreInternal::BaseSocketAgent* BaseSocketAgentNew, DiscordCoreInternal::WebSocketSSLShard* baseShard,
		const DiscordCoreInternal::VoiceConnectInitData& initDataNew, DiscordCoreAPI::ConfigManager* configManagerNew, AtomicBool* doWeQuitNew, StreamType streamTypeNew,
		StreamInfo streamInfoNew) noexcept
		: WebSocketMessageHandler(configManagerNew, &this->theConnections, "Voice WebSocket"), DatagramSocketClient(StreamType::None) {
		this->dataOpCode = DiscordCoreInternal::WebSocketOpCode::Op_Text;
		this->discordCoreClient = BaseSocketAgentNew->discordCoreClient;
		this->activeState.store(VoiceActiveState::Connecting);
		this->baseSocketAgent = BaseSocketAgentNew;
		this->typeOfWebSocket = "Voice WebSocket";
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

	String VoiceConnection::encryptSingleAudioFrame(AudioFrameData& bufferToSend) noexcept {
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
		return this->discordCoreClient->getSongAPI(this->voiceConnectInitData.guildId)->audioDataBuffer;
	}

	void VoiceConnection::sendSingleFrame(AudioFrameData& frameData) noexcept {
		this->discordCoreClient->getSongAPI(this->voiceConnectInitData.guildId)->audioDataBuffer.send(std::move(frameData));
	}

	Bool VoiceConnection::onMessageReceived(StringView theData) noexcept {
		std::unique_lock theLock00{ this->voiceUserMutex, std::defer_lock_t{} };
		try {
			if (theData.size() > 0) {
				String theString{ theData };
				theString.reserve(theString.size() + simdjson::SIMDJSON_PADDING);
				simdjson::ondemand::parser theParser{};
				DiscordCoreInternal::WebSocketMessage theMessage{};
				simdjson::ondemand::value theValue{};
				if (theParser.iterate(theString.data(), theString.length(), theString.capacity()).get(theValue) == simdjson::error_code::SUCCESS) {
					theMessage = DiscordCoreInternal::WebSocketMessage{ theValue };
				}

				if (this->configManager->doWePrintWebSocketSuccessMessages()) {
					cout << shiftToBrightGreen() << "Message received from Voice WebSocket: " << theData << reset() << endl << endl;
				}
				if (theMessage.op != 0) {
					switch (theMessage.op) {
						case 2: {
							VoiceSocketReadyData theData{ theMessage.d };
							this->audioSSRC = theData.ssrc;
							this->voiceIp = theData.ip;
							this->port = theData.port;
							this->audioEncryptionMode = theData.mode;
							this->connectionState.store(VoiceConnectionState::Initializing_DatagramSocket);
							return true;
						}
						case 4: {
							auto theArray = getArray(theMessage.d, "secret_key");
							if (theArray.didItSucceed) {
								String theSecretKey{};
								for (auto iterator: theArray.theArray) {
									theSecretKey.push_back(static_cast<Uint8>(iterator.get_uint64().take_value()));
								}
								this->secretKeySend = theSecretKey;
							}
							this->connectionState.store(VoiceConnectionState::Collecting_Init_Data);
							return true;
						}
						case 5: {
							Uint32 ssrc = getUint32(theMessage.d, "ssrc");
							VoiceUser theUser{};
							theUser.theUserId = stoull(getString(theMessage.d, "user_id"));
							theLock00.lock();
							this->voiceUsers[ssrc] = std::move(theUser);
							return true;
						}
						case 6: {
							this->haveWeReceivedHeartbeatAck = true;
							return true;
						}
						case 8: {
							auto theHeartBeat = static_cast<Uint32>(getFloat(theMessage.d, "heartbeat_interval"));
							this->heartBeatStopWatch = StopWatch{ std::chrono::milliseconds{ theHeartBeat } };
							this->areWeHeartBeating = true;
							this->connectionState.store(VoiceConnectionState::Sending_Identify);
							this->currentState.store(DiscordCoreInternal::SSLShardState::Authenticated);
							this->haveWeReceivedHeartbeatAck = true;
							return true;
						}
						case 13: {
							auto theUserId = stoull(getString(theMessage.d, "user_id"));
							for (auto& [key, value]: this->voiceUsers) {
								if (theUserId == value.theUserId) {
									theLock00.lock();
									this->voiceUsers.erase(key);
									break;
								}
							}
							return true;
						}
					}
				}
			} else {
				return false;
			}
		} catch (...) {
			reportException("VoiceConnection::onMessageReceived()");
			this->onClosed();
			return false;
		}
		return false;
	}

	void VoiceConnection::reconnectStream() noexcept {
		this->streamSocket->connect(this->theStreamInfo.address, this->theStreamInfo.port);
	}

	void VoiceConnection::sendVoiceData(String& responseData) noexcept {
		try {
			if (responseData.size() == 0) {
				if (this->configManager->doWePrintWebSocketErrorMessages()) {
					cout << shiftToBrightRed() << "Please specify voice data to send" << reset() << endl << endl;
				}
				return;
			} else {
				if (DatagramSocketClient::areWeStillConnected()) {
					DatagramSocketClient::writeData(std::move(responseData));
				}
			}
		} catch (...) {
			if (this->configManager->doWePrintWebSocketErrorMessages()) {
				reportException("VoiceConnection::sendVoiceData()");
			}
			this->onClosed();
		}
	}

	void VoiceConnection::sendSpeakingMessage(const Bool isSpeaking) noexcept {
		DiscordCoreInternal::SendSpeakingData theData{};
		if (!isSpeaking) {
			theData.type = static_cast<DiscordCoreInternal::SendSpeakingType>(0);
			this->sendSilence();
			DatagramSocketClient::processIO(DiscordCoreInternal::ProcessIOType::Write_Only);
		} else {
			theData.type = DiscordCoreInternal::SendSpeakingType::Microphone;
			theData.delay = 0;
			theData.ssrc = this->audioSSRC;
			String theString = this->stringifyJsonData(theData, DiscordCoreInternal::WebSocketOpCode::Op_Text);
			this->sendMessage(theString, true);
		}
	}

	void VoiceConnection::runWebSocket(std::stop_token stopToken) noexcept {
		try {
			while (!stopToken.stop_requested() && !this->doWeQuit->load() && this->activeState.load() != VoiceActiveState::Exiting) {
				if (this->theConnections.size() > 0) {
					this->theConnections.pop_front();
					StopWatch theStopWatch{ 10000ms };
					if (this->activeState.load() == VoiceActiveState::Connecting) {
						this->lastActiveState.store(VoiceActiveState::Stopped);
					} else {
						this->lastActiveState.store(this->activeState.load());
					}
					this->activeState.store(VoiceActiveState::Connecting);
					this->connectionState.store(VoiceConnectionState::Collecting_Init_Data);
					while (!stopToken.stop_requested() && this->baseShard->currentState.load() != DiscordCoreInternal::SSLShardState::Authenticated) {
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
				if (!stopToken.stop_requested() && WebSocketMessageHandler::areWeStillConnected()) {
					this->checkForAndSendHeartBeat();
				}
				if (!stopToken.stop_requested() && WebSocketMessageHandler::areWeStillConnected()) {
					if (WebSocketMessageHandler::processIO(10) == DiscordCoreInternal::ProcessIOResult::Error) {
						this->onClosed();
					}
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
		Int32 timeToWaitInMs{ 20 };
		Int32 timeTakesToSleep{ 0 };
		while (!theToken.stop_requested()) {
			if (theStopWatch.hasTimePassed()) {
				theStopWatch.resetTimer();
				for (Uint32 x = 0; x < this->voiceUsers.size(); ++x) {
					DatagramSocketClient::processIO(DiscordCoreInternal::ProcessIOType::Both);
					String theString = static_cast<String>(DatagramSocketClient::getInputBuffer());
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

	Bool VoiceConnection::collectAndProcessAMessage(VoiceConnectionState stateToWaitFor) noexcept {
		StopWatch theStopWatch{ 2500ms };
		theStopWatch.resetTimer();
		while (!this->doWeQuit->load() && this->connectionState.load() != stateToWaitFor) {
			if (WebSocketMessageHandler::processIO(10) == DiscordCoreInternal::ProcessIOResult::Error) {
				this->onClosed();
				return false;
			}
			if (!WebSocketMessageHandler::areWeStillConnected()) {
				return false;
			}
			if (WebSocketMessageHandler::inputBuffer.getUsedSpace() > 0) {
				WebSocketMessageHandler::parseMessage();
				return true;
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
					this->discordCoreClient->getSongAPI(this->voiceConnectInitData.guildId)->audioDataBuffer.clearContents();
					this->clearAudioData();
					while (!stopToken.stop_requested() && this->activeState.load() == VoiceActiveState::Stopped) {
						DatagramSocketClient::processIO(DiscordCoreInternal::ProcessIOType::Both);
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
					DoubleTimePointNs startingValue{ std::chrono::steady_clock::now().time_since_epoch() };
					DoubleTimePointNs intervalCount{ std::chrono::nanoseconds{ 20000000 } };
					DoubleTimePointNs targetTime{ startingValue.time_since_epoch() + intervalCount.time_since_epoch() };
					Int64 frameCounter{ 0 };
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
						this->discordCoreClient->getSongAPI(this->voiceConnectInitData.guildId)->audioDataBuffer.tryReceive(this->audioData);
						if (!this->streamSocket) {
							while (this->theFrameQueue.size() > 0) {
								this->theFrameQueue.pop_front();
							}
						}
						frameCounter++;
						if (this->audioData.guildMemberId != 0) {
							this->currentGuildMemberId = this->audioData.guildMemberId;
						}
						String newFrame{};
						Bool doWeBreak{ false };
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
								DiscordCoreClient::getSongAPI(this->voiceConnectInitData.guildId)->onSongCompletionEvent(completionEventData);
								this->areWePlaying.store(false);
								doWeBreak = true;
								break;
							}
						}
						if (doWeBreak) {
							break;
						}
						auto waitTime = targetTime - std::chrono::steady_clock::now();
						nanoSleep(static_cast<Uint64>(static_cast<Double>(waitTime.count()) * 0.95f));
						waitTime = targetTime - std::chrono::steady_clock::now();
						if (waitTime.count() > 0 && static_cast<Uint64>(waitTime.count()) < 20000000) {
							spinLock(waitTime.count());
						}
						startingValue = std::chrono::steady_clock::now();
						if (newFrame.size() > 0) {
							this->sendVoiceData(newFrame);
						}
						if (this->streamType == StreamType::None) {
							DatagramSocketClient::processIO(DiscordCoreInternal::ProcessIOType::Both);
						} else {
							DatagramSocketClient::processIO(DiscordCoreInternal::ProcessIOType::Write_Only);
						}

						this->audioData.data.clear();
						this->audioData.sampleCount = 0;
						this->audioData.type = AudioFrameType::Unset;
						totalTime += std::chrono::steady_clock::now() - startingValue;
						auto intervalCountNew =
							DoubleTimePointNs{ std::chrono::nanoseconds{ 20000000 } - (totalTime.time_since_epoch() / frameCounter) }.time_since_epoch().count();
						intervalCount = DoubleTimePointNs{ std::chrono::nanoseconds{ static_cast<Uint64>(intervalCountNew) } };
						targetTime = std::chrono::steady_clock::now().time_since_epoch() + intervalCount;
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
				std::vector<Uint8> packet{};
				packet.insert(packet.begin(), theBuffer.theRawData.begin(), theBuffer.theRawData.end());
				theBuffer.theRawData.clear();
				constexpr Uint64 headerSize{ 12 };
				if (packet.size() < headerSize) {
					return;
				}

				if (Uint8 payloadType = packet[1] & 0b0111'1111; 72 <= payloadType && payloadType <= 76) {
					return;
				}
				Uint32 speakerSsrc{ *reinterpret_cast<Uint32*>(packet.data() + 8) };
				speakerSsrc = ntohl(speakerSsrc);

				std::vector<Uint8> nonce{};
				nonce.resize(24);
				for (Uint32 x = 0; x < headerSize; ++x) {
					nonce[x] = packet[x];
				}
				const Uint64 csrcCount = packet[0] & 0b0000'1111;
				const ptrdiff_t offsetToData = headerSize + sizeof(Uint32) * csrcCount;
				Uint8* encryptedData = packet.data() + offsetToData;
				const Uint64 encryptedDataLength = packet.size() - offsetToData;
				std::vector<Uint8> theKey{};
				theKey.insert(theKey.begin(), this->secretKeySend.begin(), this->secretKeySend.end());
				std::vector<Uint8> decryptedData{};
				decryptedData.resize(encryptedDataLength);
				if (crypto_secretbox_open_easy(decryptedData.data(), encryptedData, encryptedDataLength, nonce.data(), theKey.data())) {
					return;
				}

				String decryptedDataString{};
				decryptedDataString.insert(decryptedDataString.begin(), decryptedData.begin(), decryptedData.end());

				if ((packet[0] >> 4) & 0b0001) {
					Uint64 extLen = 0;
					Uint16 extLengthInWords{ *reinterpret_cast<Uint16*>(decryptedDataString.data() + 2) };
					extLengthInWords = ntohs(extLengthInWords);
					extLen = sizeof(Uint32) * extLengthInWords;
					constexpr Uint64 extHeaderLen = sizeof(Uint16) * 2;
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
							theBuffer.decodedData.resize(static_cast<Uint64>(sampleCount) * 2);
							this->voiceUsers[speakerSsrc].thePayloads.push(std::move(theBuffer));
						}
					}
				}
			}
		}

		String theBuffer = static_cast<String>(this->streamSocket->getInputBuffer());

		AudioFrameData theFrame{};
		theFrame.data.insert(theFrame.data.begin(), theBuffer.begin(), theBuffer.end());
		theFrame.sampleCount = 960;
		theFrame.type = AudioFrameType::Encoded;
		this->discordCoreClient->getSongAPI(this->voiceConnectInitData.guildId)->audioDataBuffer.send(std::move(theFrame));
	}

	Bool VoiceConnection::areWeCurrentlyPlaying() noexcept {
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
		VoiceConnection::disconnect();
		if (this->streamSocket && this->streamSocket->areWeStillConnected()) {
			this->streamSocket->disconnect();
		}
		if (DiscordCoreClient::getSongAPI(this->voiceConnectInitData.guildId)) {
			DiscordCoreClient::getSongAPI(this->voiceConnectInitData.guildId)
				->onSongCompletionEvent.remove(DiscordCoreClient::getSongAPI(this->voiceConnectInitData.guildId)->eventToken);
		}
		this->areWeConnectedBool.store(false);
		this->connectionState.store(VoiceConnectionState::Collecting_Init_Data);
		this->activeState.store(VoiceActiveState::Connecting);
	}

	void VoiceConnection::connectInternal() noexcept {
		StopWatch theStopWatch{ 5000ms };
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
				if (!WebSocketMessageHandler::connect(this->baseUrl, "443", this->configManager->doWePrintWebSocketErrorMessages(), false)) {
					this->currentReconnectTries++;
					this->onClosed();
					this->connectInternal();
					return;
				}
				WebSocketMessageHandler::currentState.store(DiscordCoreInternal::SSLShardState::Upgrading);
				String sendVector = "GET /?v=4 HTTP/1.1\r\nHost: " + this->baseUrl +
					"\r\nPragma: no-cache\r\nUser-Agent: DiscordCoreAPI/1.0\r\nUpgrade: WebSocket\r\nConnection: Upgrade\r\nSec-WebSocket-Key: " + generateBase64EncodedKey() +
					"\r\nSec-WebSocket-Version: 13\r\n\r\n";
				this->shard[0] = 0;
				this->shard[1] = 1;
				this->sendMessage(sendVector, true);
				while (this->currentState.load() != DiscordCoreInternal::SSLShardState::Collecting_Hello) {
					if (WebSocketMessageHandler::processIO(10) == DiscordCoreInternal::ProcessIOResult::Error) {
						this->onClosed();
						this->connectInternal();
						return;
					}
				}
				this->connectionState.store(VoiceConnectionState::Collecting_Hello);
				this->connectInternal();
				break;
			}
			case VoiceConnectionState::Collecting_Hello: {
				theStopWatch.resetTimer();
				while (this->connectionState.load() != VoiceConnectionState::Sending_Identify) {
					if (theStopWatch.hasTimePassed()) {
						this->onClosed();
						this->connectInternal();
						return;
					}
					if (WebSocketMessageHandler::processIO(10) == DiscordCoreInternal::ProcessIOResult::Error) {
						this->onClosed();
						this->connectInternal();
						return;
					}
					std::this_thread::sleep_for(1ms);
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
				String sendVector = this->stringifyJsonData(identifyData, DiscordCoreInternal::WebSocketOpCode::Op_Text);
				this->sendMessage(sendVector, true);
				this->connectionState.store(VoiceConnectionState::Collecting_Ready);
				this->connectInternal();
				break;
			}
			case VoiceConnectionState::Collecting_Ready: {
				theStopWatch.resetTimer();
				while (this->connectionState.load() != VoiceConnectionState::Initializing_DatagramSocket) {
					if (theStopWatch.hasTimePassed()) {
						this->onClosed();
						this->connectInternal();
						return;
					}
					if (WebSocketMessageHandler::processIO(10) == DiscordCoreInternal::ProcessIOResult::Error) {
						this->onClosed();
						this->connectInternal();
						return;
					}
					std::this_thread::sleep_for(1ms);
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
				String sendVector = this->stringifyJsonData(protocolPayloadData, DiscordCoreInternal::WebSocketOpCode::Op_Text);
				this->sendMessage(sendVector, true);
				this->connectionState.store(VoiceConnectionState::Collecting_Session_Description);
				this->connectInternal();
				break;
			}
			case VoiceConnectionState::Collecting_Session_Description: {
				theStopWatch.resetTimer();
				while (this->connectionState.load() != VoiceConnectionState::Collecting_Init_Data) {
					if (theStopWatch.hasTimePassed()) {
						this->onClosed();
						this->connectInternal();
						return;
					}
					if (WebSocketMessageHandler::processIO(10) == DiscordCoreInternal::ProcessIOResult::Error) {
						this->onClosed();
						this->connectInternal();
						return;
					}
					std::this_thread::sleep_for(1ms);
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
		while (this->discordCoreClient->getSongAPI(this->voiceConnectInitData.guildId)->audioDataBuffer.tryReceive(frameData)) {
		};
	}

	Bool VoiceConnection::areWeConnected() noexcept {
		if (this == nullptr) {
			return false;
		} else {
			return this->areWeConnectedBool.load();
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

	Bool VoiceConnection::voiceConnect() noexcept {
		try {
			if (!DatagramSocketClient::areWeStillConnected()) {
				if (!DatagramSocketClient::connect(this->voiceIp, std::to_string(this->port))) {
					return false;
				} else {
					String packet{};
					packet.resize(74);
					Uint16 val1601{ 0x01 };
					Uint16 val1602{ 70 };
					packet[0] = static_cast<Uint8>(val1601 >> 8);
					packet[1] = static_cast<Uint8>(val1601 >> 0);
					packet[2] = static_cast<Uint8>(val1602 >> 8);
					packet[3] = static_cast<Uint8>(val1602 >> 0);
					packet[4] = static_cast<Uint8>(this->audioSSRC >> 24);
					packet[5] = static_cast<Uint8>(this->audioSSRC >> 16);
					packet[6] = static_cast<Uint8>(this->audioSSRC >> 8);
					packet[7] = static_cast<Uint8>(this->audioSSRC);
					DatagramSocketClient::getInputBuffer();
					DatagramSocketClient::writeData(std::move(packet));
					String inputString{};
					StopWatch theStopWatch{ 2500ms };
					while (inputString.size() < 74 && !this->doWeQuit->load() && this->activeState.load() != VoiceActiveState::Exiting) {
						DatagramSocketClient::processIO(DiscordCoreInternal::ProcessIOType::Both);
						inputString = DatagramSocketClient::getInputBuffer();
						std::this_thread::sleep_for(1ms);
						if (theStopWatch.hasTimePassed()) {
							return false;
						}
					}
					String message{};
					message.insert(message.begin(), inputString.begin() + 8, inputString.begin() + 64);
					auto endLineFind = message.find('\u0000', 5);
					if (endLineFind != String::npos) {
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
		newFrame.data.emplace_back(0xf8);
		newFrame.data.emplace_back(0xff);
		newFrame.data.emplace_back(0xfe);
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
		this->baseSocketAgent->disconnectVoice(this->voiceConnectInitData.guildId);
		this->activeState.store(VoiceActiveState::Exiting);
	}

	void VoiceConnection::reconnect() noexcept {
		DatagramSocketClient::disconnect();
		if (WebSocketMessageHandler::theSocket != SOCKET_ERROR) {
			WebSocketMessageHandler::theSocket = SOCKET_ERROR;
			this->currentState.store(DiscordCoreInternal::SSLShardState::Disconnected);
			WebSocketMessageHandler::areWeConnecting.store(true);
			WebSocketMessageHandler::outputBuffer.clear();
			WebSocketMessageHandler::inputBuffer.clear();
			this->closeCode = 0;
			this->areWeHeartBeating = false;
			if (WebSocketMessageHandler::theConnections) {
				DiscordCoreAPI::ConnectionPackage theData{};
				theData.currentReconnectTries = this->currentReconnectTries;
				theData.areWeResuming = this->areWeResuming;
				theData.currentShard = this->shard[0];
				WebSocketMessageHandler::theConnections->emplace_back(theData);
			}
		}
		if (this->taskThread03) {
			this->taskThread03.reset(nullptr);
		}
		DatagramSocketClient::outputBuffer.clear();
		DatagramSocketClient::inputBuffer.clear();
		WebSocketMessageHandler::outputBuffer.clear();
		WebSocketMessageHandler::inputBuffer.clear();
		this->areWeConnectedBool.store(false);
		this->thePackage.currentShard = 0;
		this->areWeHeartBeating = false;
		this->currentReconnectTries++;
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
						for (Uint32 x = 0; x < thePayload.decodedData.size(); ++x) {
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
				for (Int32 x = 0; x < theUpsampledVector.size(); ++x) {
					theDownsampledVector[x] = static_cast<opus_int16>(theUpsampledVector[x] / voiceUserCount);
				}
				auto theEncodedData = this->theEncoder.encodeSingleAudioFrame(theDownsampledVector);
				if (theEncodedData.data.size() <= 0) {
					if (this->configManager->doWePrintGeneralErrorMessages()) {
						cout << "Failed to encode user's voice payload." << endl;
					}
				} else {
					String theFinalString{};
					theFinalString.insert(theFinalString.begin(), theEncodedData.data.data(), theEncodedData.data.data() + theEncodedData.data.size());
					this->streamSocket->writeData(theFinalString);
				}
			}
		}
	}

	void VoiceConnection::connect() noexcept {
		if (this->baseSocketAgent) {
			this->thePackage.currentShard = 1;
			this->theConnections.emplace_back(this->thePackage);
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

	Bool VoiceConnection::stop() noexcept {
		this->sendSpeakingMessage(false);
		this->activeState.store(VoiceActiveState::Stopped);
		return true;
	}

	Bool VoiceConnection::play() noexcept {
		this->activeState.store(VoiceActiveState::Playing);
		return true;
	}

	VoiceConnection::~VoiceConnection() {
		this->disconnectInternal();
	}

}