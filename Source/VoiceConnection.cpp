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

	namespace Globals {
		extern std::atomic_bool doWeQuit;
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
		return std::string{};
	}

	VoiceConnection::VoiceConnection() noexcept : WebSocketMessageHandler(nullptr) {
	}

	VoiceConnection::VoiceConnection(DiscordCoreInternal::BaseSocketAgent* BaseSocketAgentNew, const DiscordCoreInternal::VoiceConnectInitData& initDataNew,
		DiscordCoreAPI::ConfigManager* configManagerNew) noexcept
		: WebSocketMessageHandler(configManagerNew) {
		this->baseShard = BaseSocketAgentNew->sslShards[initDataNew.currentShard].get();
		this->activeState.store(VoiceActiveState::Connecting);
		this->baseSocketAgent = BaseSocketAgentNew;
		this->voiceConnectInitData = initDataNew;
		this->configManager = configManagerNew;
	}

	Snowflake VoiceConnection::getChannelId() noexcept {
		return this->voiceConnectInitData.channelId;
	}

	std::string VoiceConnection::encryptSingleAudioFrame(const EncodedFrameData& bufferToSend) noexcept {
		if (this->voiceConnectionData.secretKey.size() > 0) {
			this->sequenceIndex++;
			this->timeStamp += 960;
			return RTPPacket{ this->timeStamp, this->sequenceIndex, this->voiceConnectionData.audioSSRC, bufferToSend.data, this->voiceConnectionData.secretKey };
		}
		return std::string{};
	}

	void VoiceConnection::sendSingleAudioFrame(std::string& audioDataPacketNew) noexcept {
		if (this->datagramSocket && this->datagramSocket->areWeStillConnected()) {
			this->datagramSocket->writeData(audioDataPacketNew);
		} else {
			this->onClosed();
		}
	}

	UnboundedMessageBlock<AudioFrameData>& VoiceConnection::getAudioBuffer() noexcept {
		return this->audioDataBuffer;
	}

	void VoiceConnection::sendSingleFrame(const AudioFrameData& frameData) noexcept {
		this->audioDataBuffer.send(frameData);
	}

	void VoiceConnection::onMessageReceived(DiscordCoreInternal::WebSocketSSLShard* theShard) noexcept {
		try {
			std::string theMessage{};
			if (theShard->processedMessages.size() > 0) {
				theMessage = theShard->processedMessages.front();
				theShard->processedMessages.pop();
			} else {
				return;
			}
			if (this->webSocketShard && theMessage.size() > 0) {
				nlohmann::json payload = payload.parse(theMessage);
				if (this->configManager->doWePrintWebSocketSuccessMessages()) {
					std::cout << DiscordCoreAPI::shiftToBrightGreen() << "Message received from Voice WebSocket: " << theMessage << DiscordCoreAPI::reset() << std::endl
							  << std::endl;
				}
				if (payload.contains("op") && !payload["op"].is_null()) {
					switch (payload["op"].get<int32_t>()) {
						case 2: {
							this->voiceConnectionData.audioSSRC = payload["d"]["ssrc"].get<uint32_t>();
							this->voiceConnectionData.voiceIp = payload["d"]["ip"].get<std::string>();
							this->voiceConnectionData.voicePort = std::to_string(payload["d"]["port"].get<int64_t>());
							for (auto& value: payload["d"]["modes"]) {
								if (value == "xsalsa20_poly1305") {
									this->voiceConnectionData.voiceEncryptionMode = value;
								}
							}
							this->connectionState.store(VoiceConnectionState::Initializing_DatagramSocket);
							break;
						}
						case 4: {
							for (uint32_t x = 0; x < payload["d"]["secret_key"].size(); x++) {
								this->voiceConnectionData.secretKey.push_back(payload["d"]["secret_key"][x].get<uint8_t>());
							}
							this->connectionState.store(VoiceConnectionState::Collecting_Init_Data);
							break;
						}
						case 6: {
							this->webSocketShard->haveWeReceivedHeartbeatAck = true;
							break;
						}
						case 8: {
							if (payload["d"].contains("heartbeat_interval")) {
								this->heartbeatInterval = static_cast<int32_t>(payload["d"]["heartbeat_interval"].get<float>());
								this->webSocketShard->areWeHeartBeating = false;
								this->connectionState.store(VoiceConnectionState::Sending_Identify);
							}
							break;
						}
					}
				}
			}
		} catch (...) {
			if (this->configManager->doWePrintWebSocketErrorMessages()) {
				DiscordCoreAPI::reportException("VoiceConnection::onMessageReceived()");
			}
			this->onClosed();
		}
	}

	void VoiceConnection::sendVoiceData(const std::string& responseData) noexcept {
		try {
			if (responseData.size() == 0) {
				if (this->configManager->doWePrintWebSocketErrorMessages()) {
					std::cout << DiscordCoreAPI::shiftToBrightRed() << "Please specify voice data to send" << DiscordCoreAPI::reset() << std::endl << std::endl;
				}
				return;
			} else {
				if (this->datagramSocket && this->datagramSocket->areWeStillConnected()) {
					std::string theData = responseData;
					this->datagramSocket->writeData(theData);
				} else {
					this->onClosed();
				}
			}
		} catch (...) {
			if (this->configManager->doWePrintWebSocketErrorMessages()) {
				DiscordCoreAPI::reportException("VoiceConnection::sendVoiceData()");
			}
			this->onClosed();
		}
	}

	bool VoiceConnection::sendMessage(const std::string& dataToSend, bool priority) noexcept {
		try {
			if (this->webSocketShard) {
				if (this->configManager->doWePrintWebSocketSuccessMessages()) {
					std::cout << DiscordCoreAPI::shiftToBrightBlue() << "Sending Voice WebSocket Message: " << dataToSend << DiscordCoreAPI::reset() << std::endl << std::endl;
				}
				DiscordCoreAPI::StopWatch theStopWatch{ 2000ms };
				bool didWeWrite{ false };
				do {
					if (theStopWatch.hasTimePassed()) {
						return false;
					}
					didWeWrite = this->webSocketShard->writeData(dataToSend, priority);
				} while (!didWeWrite);
				return true;
			} else {
				return false;
			}
		} catch (...) {
			if (this->configManager->doWePrintWebSocketErrorMessages()) {
				DiscordCoreAPI::reportException("VoiceConnection::sendMessage()");
			}
			this->onClosed();
			return false;
		}
	}

	void VoiceConnection::sendSpeakingMessage(const bool isSpeaking) noexcept {
		DiscordCoreInternal::SendSpeakingData theData{};
		if (!isSpeaking) {
			theData.type = static_cast<DiscordCoreInternal::SendSpeakingType>(0);
			this->sendSilence();
		} else {
			theData.type = DiscordCoreInternal::SendSpeakingType::Priority_And_Voice;
			theData.delay = 0;
			theData.ssrc = this->voiceConnectionData.audioSSRC;
			nlohmann::json newString = theData;
			std::string theString{};
			this->stringifyJsonData(newString, theString, DiscordCoreInternal::WebSocketOpCode::Op_Text);
			if (!this->sendMessage(theString, true)) {
				this->onClosed();
			}
		}
	}

	void VoiceConnection::runWebSocket(std::stop_token stopToken) noexcept {
		try {
			while (!stopToken.stop_requested() && !Globals::doWeQuit.load() && this->activeState.load() != VoiceActiveState::Exiting) {
				if (!stopToken.stop_requested() && this->connections.size() > 0) {
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
				if (!stopToken.stop_requested() && this->heartbeatInterval != 0 && !this->webSocketShard->areWeHeartBeating) {
					this->webSocketShard->areWeHeartBeating = true;
					this->webSocketShard->heartBeatStopWatch = DiscordCoreAPI::StopWatch{ std::chrono::milliseconds{ this->heartbeatInterval } };
				}
				if (!stopToken.stop_requested() && this->webSocketShard->areWeStillConnected() && this->webSocketShard->heartBeatStopWatch.hasTimePassed() &&
					this->webSocketShard->areWeHeartBeating) {
					this->sendHeartBeat();
					this->webSocketShard->heartBeatStopWatch.resetTimer();
				}
				if (!stopToken.stop_requested() && this->webSocketShard->areWeStillConnected()) {
					this->webSocketShard->processIO(10000);
				}
				if (!stopToken.stop_requested() && this->webSocketShard->areWeStillConnected() && this->webSocketShard->inputBuffer.size() > 0) {
					this->parseMessage(this->webSocketShard.get());
				}
				if (!stopToken.stop_requested() && this->webSocketShard->areWeStillConnected() && this->webSocketShard->processedMessages.size() > 0) {
					this->onMessageReceived(this->webSocketShard.get());
				}
				std::this_thread::sleep_for(1ms);
			}
		} catch (...) {
			if (this->configManager->doWePrintWebSocketErrorMessages()) {
				DiscordCoreAPI::reportException("VoiceConnection::run()");
			}
			this->onClosed();
		}
	}

	bool VoiceConnection::collectAndProcessAMessage(VoiceConnectionState stateToWaitFor) noexcept {
		DiscordCoreAPI::StopWatch theStopWatch{ 2500ms };
		while (!Globals::doWeQuit.load() && this->connectionState.load() != stateToWaitFor) {
			this->webSocketShard->processIO(100000);
			if (!this->webSocketShard->areWeStillConnected()) {
				return false;
			}
			if (this->webSocketShard->inputBuffer.size() > 0) {
				this->parseMessage(this->webSocketShard.get());
			}
			if (this->webSocketShard->processedMessages.size() > 0) {
				this->onMessageReceived(this->webSocketShard.get());
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
		while (!this->datagramSocket) {
			if (theStopWatch.hasTimePassed() || this->activeState.load() == VoiceActiveState::Exiting) {
				return;
			}
			std::this_thread::sleep_for(1ms);
		}
		while (!stopToken.stop_requested() && !Globals::doWeQuit.load() && this->activeState.load() != VoiceActiveState::Exiting) {
			if (!this->datagramSocket->areWeStillConnected()) {
				this->onClosed();
			}
			switch (this->activeState.load()) {
				case VoiceActiveState::Connecting: {
					while (!stopToken.stop_requested() && this->activeState.load() == VoiceActiveState::Connecting) {
						std::this_thread::sleep_for(1ms);
					}
					break;
				}
				case VoiceActiveState::Stopped: {
					this->audioDataBuffer.clearContents();
					this->clearAudioData();
					while (!stopToken.stop_requested() && this->activeState.load() == VoiceActiveState::Stopped) {
						std::this_thread::sleep_for(1ms);
					}
					break;
				}
				case VoiceActiveState::Paused: {
					while (!stopToken.stop_requested() && this->activeState.load() == VoiceActiveState::Paused) {
						std::this_thread::sleep_for(1ms);
					}
					break;
				}
				case VoiceActiveState::Playing: {
					this->audioData.type = AudioFrameType::Unset;
					this->audioData.encodedFrameData.data.clear();
					this->audioData.rawFrameData.data.clear();
					this->audioDataBuffer.tryReceive(this->audioData);
					DoubleTimePointNs startingValue{ std::chrono::system_clock::now().time_since_epoch() };
					DoubleTimePointNs intervalCount{ std::chrono::nanoseconds{ 20000000 } };
					DoubleTimePointNs targetTime{ startingValue.time_since_epoch() + intervalCount.time_since_epoch() };
					int32_t frameCounter{ 0 };
					DoubleTimePointNs totalTime{ std::chrono::duration_cast<std::chrono::nanoseconds>(std::chrono::nanoseconds{ 0 }) };
					if (this->disconnectStartTime.count() != 0) {
						DoubleMilliSecond currentTime = std::chrono::system_clock::now().time_since_epoch();
						if ((currentTime - this->disconnectStartTime).count() >= 60000) {
							this->connect();
							this->play();
						}
					}
					if (!this->areWePlaying.load()) {
						this->sendSpeakingMessage(true);
						this->areWePlaying.store(true);
					}
					while (!stopToken.stop_requested() && this->activeState.load() == VoiceActiveState::Playing) {
						theStopWatch.resetTimer();
						while (!stopToken.stop_requested() && !this->datagramSocket->areWeStillConnected()) {
							if (theStopWatch.hasTimePassed() || this->activeState.load() == VoiceActiveState::Exiting) {
								return;
							}
							std::this_thread::sleep_for(1ms);
						}
						frameCounter++;
						if (this->audioData.guildMemberId != 0) {
							this->currentGuildMemberId = this->audioData.guildMemberId;
						}
						if (this->audioData.type != AudioFrameType::Unset && this->audioData.type != AudioFrameType::Skip) {
							std::string newFrame{};
							if (this->audioData.type == AudioFrameType::RawPCM) {
								std::vector<RawFrameData> rawFrames{};
								rawFrames.push_back(this->audioData.rawFrameData);
								auto encodedFrameData = this->encoder->encodeFrames(rawFrames);
								newFrame = this->encryptSingleAudioFrame(encodedFrameData[0].encodedFrameData);
							} else {
								newFrame = this->encryptSingleAudioFrame(this->audioData.encodedFrameData);
							}
							if (newFrame.size() == 0) {
								continue;
							}
							auto waitTime = targetTime - std::chrono::system_clock::now();
							if (waitTime.count() > 500000 && !stopToken.stop_requested() && this->datagramSocket->areWeStillConnected()) {
								this->datagramSocket->readData();
								this->datagramSocket->getInputBuffer();
							}
							waitTime = targetTime - std::chrono::system_clock::now();
							nanoSleep(static_cast<int64_t>(static_cast<float>(waitTime.count()) * 0.95f));
							waitTime = targetTime - std::chrono::system_clock::now();
							if (waitTime.count() > 0) {
								spinLock(waitTime.count());
							}
							startingValue = std::chrono::system_clock::now();
							this->sendSingleAudioFrame(newFrame);
							totalTime += std::chrono::system_clock::now() - startingValue;
							auto intervalCountNew =
								DoubleTimePointNs{ std::chrono::nanoseconds{ 20000000 } - totalTime.time_since_epoch() / frameCounter }.time_since_epoch().count();
							intervalCount = DoubleTimePointNs{ std::chrono::nanoseconds{ static_cast<uint64_t>(intervalCountNew) } };
							targetTime = std::chrono::system_clock::now().time_since_epoch() + intervalCount;
							this->audioData.type = AudioFrameType::Unset;
							this->audioData.encodedFrameData.data.clear();
							this->audioData.rawFrameData.data.clear();
						} else if (this->audioData.type == AudioFrameType::Skip) {
							SongCompletionEventData completionEventData{};
							completionEventData.guild = Guilds::getCachedGuildAsync({ .guildId = this->voiceConnectInitData.guildId }).get();
							if (this->currentGuildMemberId != 0) {
								completionEventData.guildMember =
									GuildMembers::getCachedGuildMemberAsync({ .guildMemberId = this->currentGuildMemberId, .guildId = this->voiceConnectInitData.guildId }).get();
							}
							completionEventData.wasItAFail = false;
							getSongAPIMap()[this->voiceConnectInitData.guildId]->onSongCompletionEvent(completionEventData);
							break;
						} else {
							break;
						}
						this->datagramSocket->getInputBuffer();
						this->audioDataBuffer.tryReceive(this->audioData);
					}
					this->datagramSocket->getInputBuffer();
					this->disconnectStartTime = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch());
					this->areWePlaying.store(true);
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
		}
	};

	bool VoiceConnection::areWeCurrentlyPlaying() noexcept {
		if (this == nullptr) {
			return false;
		} else {
			bool areWePlaying{};
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
		if (this->datagramSocket) {
			this->datagramSocket->disconnect();
		}
		if (this->webSocketShard) {
			this->webSocketShard->disconnect(false);
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
		auto thePtr = this->baseSocketAgent->sslShards[voiceConnectInitData.currentShard].get();
		std::unique_lock theLock{ thePtr->theMutex };
		if (this->connections.size() > 0) {
			this->connections.pop();
		}
		if (this->currentReconnectTries >= this->maxReconnectTries) {
			Globals::doWeQuit.store(true);
			if (this->configManager->doWePrintWebSocketErrorMessages()) {
				std::cout << "VoiceConnection::connectInternal() Error: Failed to connect to voice channel!" << std::endl << std::endl;
			}
		}
		switch (this->connectionState.load()) {
			case VoiceConnectionState::Collecting_Init_Data: {
				this->voiceConnectionData = DiscordCoreInternal::VoiceConnectionData{};
				this->baseShard->voiceConnectionDataBufferMap[this->voiceConnectInitData.guildId] = &this->voiceConnectionDataBuffer;
				this->baseShard->voiceConnectionDataBufferMap[this->voiceConnectInitData.guildId]->clearContents();
				this->baseSocketAgent->getVoiceConnectionData(this->voiceConnectInitData, this->baseShard);

				if (DiscordCoreAPI::waitForTimeToPass(this->voiceConnectionDataBuffer, this->voiceConnectionData, 10000)) {
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
				if (this->webSocketShard) {
					this->webSocketShard->disconnect(true);
				}
				auto theClient = std::make_unique<DiscordCoreInternal::WebSocketSSLShard>(&this->connections, 0, 0, this->configManager);
				if (!theClient->connect(this->baseUrl, "443")) {
					this->webSocketShard = std::move(theClient);
					this->currentReconnectTries++;
					this->onClosed();
					this->connectInternal();
					return;
				}
				theClient->theWebSocketState.store(DiscordCoreInternal::WebSocketSSLShardState::Upgrading);
				std::string sendVector = "GET /?v=4 HTTP/1.1\r\nHost: " + this->baseUrl +
					"\r\nPragma: no-cache\r\nUser-Agent: DiscordCoreAPI/1.0\r\nUpgrade: WebSocket\r\nConnection: Upgrade\r\nSec-WebSocket-Key: " +
					DiscordCoreAPI::generateBase64EncodedKey() + "\r\nSec-WebSocket-Version: 13\r\n\r\n";
				this->webSocketShard = std::move(theClient);
				this->webSocketShard->shard[0] = 0;
				this->webSocketShard->shard[1] = 1;
				if (!this->sendMessage(sendVector, true)) {
					this->currentReconnectTries++;
					this->onClosed();
					this->connectInternal();
					return;
				}
				for (uint32_t x = 0; x < 5; x++) {
					this->webSocketShard->processIO(100000);
				}
				if (!this->parseConnectionHeaders(this->webSocketShard.get())) {
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
				this->webSocketShard->haveWeReceivedHeartbeatAck = true;
				DiscordCoreInternal::VoiceIdentifyData identifyData{};
				identifyData.connectInitData = this->voiceConnectInitData;
				identifyData.connectionData = this->voiceConnectionData;
				std::string sendVector{};
				this->stringifyJsonData(identifyData, sendVector, DiscordCoreInternal::WebSocketOpCode::Op_Text);
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
				protocolPayloadData.externalIp = this->voiceConnectionData.externalIp;
				protocolPayloadData.voiceEncryptionMode = this->voiceConnectionData.voiceEncryptionMode;
				protocolPayloadData.voicePort = this->voiceConnectionData.voicePort;
				nlohmann::json protocolPayloadSelectString = protocolPayloadData;
				std::string sendVector{};
				this->stringifyJsonData(protocolPayloadSelectString, sendVector, DiscordCoreInternal::WebSocketOpCode::Op_Text);
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
			if (this->webSocketShard->areWeStillConnected() && this->webSocketShard->haveWeReceivedHeartbeatAck) {
				nlohmann::json data{};
				data["d"] = std::chrono::duration_cast<std::chrono::nanoseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
				data["op"] = int32_t(3);
				std::string theString{};
				this->stringifyJsonData(data, theString, DiscordCoreInternal::WebSocketOpCode::Op_Text);
				if (!this->sendMessage(theString, true)) {
					this->onClosed();
				}
				this->webSocketShard->haveWeReceivedHeartbeatAck = false;
			} else {
				this->onClosed();
			}
		} catch (...) {
			if (this->configManager->doWePrintWebSocketErrorMessages()) {
				DiscordCoreAPI::reportException("VoiceConnection::sendHeartBeat()");
			}
			this->onClosed();
		}
	}

	bool VoiceConnection::voiceConnect() noexcept {
		try {
			if (!this->datagramSocket) {
				this->datagramSocket = std::make_unique<DiscordCoreInternal::DatagramSocketClient>();
			}
			if (!this->datagramSocket->areWeStillConnected()) {
				if (!this->datagramSocket->connect(this->voiceConnectionData.voiceIp, this->voiceConnectionData.voicePort)) {
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
					packet[4] = static_cast<uint8_t>(this->voiceConnectionData.audioSSRC >> 24);
					packet[5] = static_cast<uint8_t>(this->voiceConnectionData.audioSSRC >> 16);
					packet[6] = static_cast<uint8_t>(this->voiceConnectionData.audioSSRC >> 8);
					packet[7] = static_cast<uint8_t>(this->voiceConnectionData.audioSSRC);
					this->datagramSocket->getInputBuffer();
					this->datagramSocket->writeData(packet);
					std::string inputString{};
					StopWatch theStopWatch{ 2500ms };
					while (inputString.size() < 74 && !Globals::doWeQuit.load() && this->activeState.load() != VoiceActiveState::Exiting) {
						this->datagramSocket->readData();
						std::string theNewString = this->datagramSocket->getInputBuffer();
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
					this->voiceConnectionData.externalIp = message;
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
			this->onClosed();
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
				sendSpeakingMessage(true);
				this->activeState.store(VoiceActiveState::Playing);
			} else {
				sendSpeakingMessage(false);
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
		if (this->datagramSocket) {
			this->datagramSocket->disconnect();
		}
		if (this->webSocketShard) {
			this->webSocketShard->disconnect(true);
			this->webSocketShard->areWeHeartBeating = false;
			this->webSocketShard->wantWrite = true;
			this->webSocketShard->wantRead = false;
		}
		this->currentReconnectTries++;
		this->areWeConnectedBool.store(false);
	}

	void VoiceConnection::onClosed() noexcept {
		this->connectionState.store(VoiceConnectionState::Collecting_Init_Data);
		if (this->activeState.load() != VoiceActiveState::Exiting && this->currentReconnectTries < this->maxReconnectTries) {
			this->reconnect();
		} else if (this->currentReconnectTries >= this->maxReconnectTries) {
			this->disconnect();
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
		if (this->taskThread02) {
			this->taskThread02->request_stop();
			if (this->taskThread02->joinable()) {
				this->taskThread02->join();
				this->taskThread02.reset(nullptr);
			}
		}

		if (this->taskThread01) {
			this->taskThread01->request_stop();
			if (this->taskThread01->joinable()) {
				this->taskThread01->join();
				this->taskThread01.reset(nullptr);
			}
		}
	};

}