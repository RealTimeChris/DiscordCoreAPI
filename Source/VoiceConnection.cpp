/*
*
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

	VoiceConnection::VoiceConnection(DiscordCoreInternal::BaseSocketAgent* BaseSocketAgentNew, const DiscordCoreInternal::VoiceConnectInitData& initDataNew,
		DiscordCoreAPI::ConfigManager* configManagerNew) noexcept {
		this->theBaseShard = BaseSocketAgentNew->theClients[initDataNew.currentShard].get();
		this->baseSocketAgent = BaseSocketAgentNew;
		this->voiceConnectInitData = initDataNew;
		this->configManager = configManagerNew;
	}

	uint64_t VoiceConnection::getChannelId() noexcept {
		if (this && this->voiceConnectInitData.channelId != 0) {
			return this->voiceConnectInitData.channelId;
		} else {
			return Snowflake();
		}
	}

	void VoiceConnection::stringifyJsonData(const nlohmann::json& dataToSend, std::string& theString, DiscordCoreInternal::WebSocketOpCode theOpCode) noexcept {
		std::string theVector{};
		std::string header{};
		theVector = dataToSend.dump();
		this->createHeader(header, theVector.size(), theOpCode);
		std::string theVectorNew{};
		theVectorNew.insert(theVectorNew.begin(), header.begin(), header.end());
		theVectorNew.insert(theVectorNew.begin() + header.size(), theVector.begin(), theVector.end());
		theString = theVectorNew;
	}

	std::string VoiceConnection::encryptSingleAudioFrame(EncodedFrameData& bufferToSend, int32_t audioSSRC, const std::string& keys) noexcept {
		if (keys.size() > 0) {
			this->sequenceIndex++;
			this->timeStamp += 960;
			const int64_t nonceSize{ crypto_secretbox_NONCEBYTES };
			const int64_t headerSize{ 12 };
			const uint8_t byteSize{ 8 };
			uint8_t headerFinal[headerSize]{};
			headerFinal[0] = 0x80;
			headerFinal[1] = 0x78;
			headerFinal[2] = static_cast<uint8_t>(this->sequenceIndex >> (byteSize * 1));
			headerFinal[3] = static_cast<uint8_t>(this->sequenceIndex >> (byteSize * 0));
			headerFinal[4] = static_cast<uint8_t>(this->timeStamp >> (byteSize * 3));
			headerFinal[5] = static_cast<uint8_t>(this->timeStamp >> (byteSize * 2));
			headerFinal[6] = static_cast<uint8_t>(this->timeStamp >> (byteSize * 1));
			headerFinal[7] = static_cast<uint8_t>(this->timeStamp >> (byteSize * 0));
			headerFinal[8] = static_cast<uint8_t>(audioSSRC >> (byteSize * 3));
			headerFinal[9] = static_cast<uint8_t>(audioSSRC >> (byteSize * 2));
			headerFinal[10] = static_cast<uint8_t>(audioSSRC >> (byteSize * 1));
			headerFinal[11] = static_cast<uint8_t>(audioSSRC >> (byteSize * 0));
			uint8_t nonceForLibSodium[nonceSize]{};
			for (uint64_t x = 0; x < headerSize; x++) {
				nonceForLibSodium[x] = headerFinal[x];
			}
			for (int64_t x = headerSize; x < nonceSize; x++) {
				nonceForLibSodium[x] = 0;
			}
			uint64_t numOfBytes{ headerSize + bufferToSend.data.size() + crypto_secretbox_MACBYTES };
			std::unique_ptr<uint8_t[]> audioDataPacket{ std::make_unique<uint8_t[]>(numOfBytes) };
			for (uint64_t x = 0; x < headerSize; x++) {
				audioDataPacket[x] = headerFinal[x];
			}
			std::unique_ptr<uint8_t[]> encryptionKeys{ std::make_unique<uint8_t[]>(keys.size()) };
			for (uint64_t x = 0; x < keys.size(); x++) {
				encryptionKeys[x] = keys[x];
			}
			if (crypto_secretbox_easy(audioDataPacket.get() + headerSize, bufferToSend.data.data(), bufferToSend.data.size(), nonceForLibSodium, encryptionKeys.get()) != 0) {
				return "";
			};
			bufferToSend.data.clear();
			std::string audioDataPacketNew{};
			audioDataPacketNew.insert(audioDataPacketNew.begin(), audioDataPacket.get(), audioDataPacket.get() + numOfBytes);
			return audioDataPacketNew;
		}
		return std::string{};
	}

	void VoiceConnection::createHeader(std::string& outBuffer, uint64_t sendLength, DiscordCoreInternal::WebSocketOpCode opCode) noexcept {
		try {
			outBuffer.push_back(static_cast<uint8_t>(opCode) | DiscordCoreInternal::webSocketFinishBit);

			int32_t indexCount{ 0 };
			if (sendLength <= DiscordCoreInternal::webSocketMaxPayloadLengthSmall) {
				outBuffer.push_back(static_cast<uint8_t>(sendLength));
				indexCount = 0;
			} else if (sendLength <= DiscordCoreInternal::webSocketMaxPayloadLengthLarge) {
				outBuffer.push_back(DiscordCoreInternal::webSocketPayloadLengthMagicLarge);
				indexCount = 2;
			} else {
				outBuffer.push_back(DiscordCoreInternal::webSocketPayloadLengthMagicHuge);
				indexCount = 8;
			}
			for (int32_t x = indexCount - 1; x >= 0; x--) {
				outBuffer.push_back(static_cast<uint8_t>(sendLength >> x * 8));
			}

			outBuffer[1] |= DiscordCoreInternal::webSocketMaskBit;
			outBuffer.push_back(0);
			outBuffer.push_back(0);
			outBuffer.push_back(0);
			outBuffer.push_back(0);
		} catch (...) {
			if (this->configManager->doWePrintWebSocketErrorMessages()) {
				DiscordCoreAPI::reportException("VoiceConnection::createHeader()");
			}
		}
	}

	void VoiceConnection::parseHeadersAndMessage(DiscordCoreInternal::WebSocketSSLShard* theShard) noexcept {
		if (theShard) {
			if (theShard->theState == DiscordCoreInternal::WebSocketState::Connecting01) {
				std::string newVector = theShard->getInputBuffer();
				if (newVector.find("\r\n\r\n") != std::string::npos) {
					newVector.erase(0, newVector.find("\r\n\r\n") + 4);
					theShard->inputBuffer.clear();
					theShard->inputBuffer.insert(theShard->inputBuffer.end(), newVector.begin(), newVector.end());
					theShard->theState = DiscordCoreInternal::WebSocketState::Connecting02;
				}
			}
			if (theShard->inputBuffer.size() < 4) {
				return;
			}
			theShard->dataOpCode = static_cast<DiscordCoreInternal::WebSocketOpCode>(theShard->inputBuffer[0] & ~DiscordCoreInternal::webSocketFinishBit);
			switch (theShard->dataOpCode) {
				case DiscordCoreInternal::WebSocketOpCode::Op_Continuation:
					[[fallthrough]];
				case DiscordCoreInternal::WebSocketOpCode::Op_Text:
					[[fallthrough]];
				case DiscordCoreInternal::WebSocketOpCode::Op_Binary:
					[[fallthrough]];
				case DiscordCoreInternal::WebSocketOpCode::Op_Ping:
					[[fallthrough]];
				case DiscordCoreInternal::WebSocketOpCode::Op_Pong: {
					uint8_t length01 = theShard->inputBuffer[1];
					theShard->messageOffset = 2;
					if (length01 & DiscordCoreInternal::webSocketMaskBit) {
						return;
					}
					theShard->messageLength = length01;
					if (length01 == DiscordCoreInternal::webSocketPayloadLengthMagicLarge) {
						if (theShard->inputBuffer.size() < 8) {
							return;
						}
						uint8_t length03 = theShard->inputBuffer[2];
						uint8_t length04 = theShard->inputBuffer[3];
						theShard->messageLength = static_cast<uint64_t>((length03 << 8) | length04);
						theShard->messageOffset += 2;
					} else if (length01 == DiscordCoreInternal::webSocketPayloadLengthMagicHuge) {
						if (theShard->inputBuffer.size() < 10) {
							return;
						}
						theShard->messageLength = 0;
						for (int64_t x = 2, shift = 56; x < 10; ++x, shift -= 8) {
							uint8_t lengthNew = static_cast<uint8_t>(theShard->inputBuffer[x]);
							theShard->messageLength |= static_cast<uint64_t>((lengthNew & static_cast<uint64_t>(0xff)) << static_cast<uint64_t>(shift));
						}
						theShard->messageOffset += 8;
					}
					if (theShard->inputBuffer.size() < static_cast<uint64_t>(theShard->messageOffset) + static_cast<uint64_t>(theShard->messageLength)) {
						return;
					} else {
						std::string finalMessage{};
						finalMessage.insert(finalMessage.begin(), theShard->inputBuffer.begin() + theShard->messageOffset,
							theShard->inputBuffer.begin() + theShard->messageOffset + theShard->messageLength);
						theShard->processedMessages.push(finalMessage);
						theShard->inputBuffer.erase(theShard->inputBuffer.begin(), theShard->inputBuffer.begin() + theShard->messageOffset + theShard->messageLength);
						return;
					}
				}
				case DiscordCoreInternal::WebSocketOpCode::Op_Close: {
					uint16_t close = theShard->inputBuffer[2] & 0xff;
					close <<= 8;
					close |= theShard->inputBuffer[3] & 0xff;
					theShard->closeCode = static_cast<DiscordCoreInternal::WebSocketCloseCode>(close);
					theShard->inputBuffer.erase(theShard->inputBuffer.begin(), theShard->inputBuffer.begin() + 4);
					if (this->configManager->doWePrintWebSocketErrorMessages()) {
						std::cout << DiscordCoreAPI::shiftToBrightRed()
								  << "Voice WebSocket " + this->theClients[0]->shard.dump() + " Closed; Code: " << +static_cast<uint16_t>(this->theClients[0]->closeCode)
								  << DiscordCoreAPI::reset() << std::endl
								  << std::endl;
					}
					this->onClosed();
				}
				default: {
				}
			}
		}
	}

	void VoiceConnection::sendSingleAudioFrame(std::string& audioDataPacketNew) noexcept {
		if (this->voiceSocket && this->voiceSocket->areWeStillConnected()) {
			this->voiceSocket->writeData(audioDataPacketNew);
		}
	}

	UnboundedMessageBlock<AudioFrameData>& VoiceConnection::getAudioBuffer() noexcept {
		return this->audioBuffer;
	}

	void VoiceConnection::sendSingleFrame(const AudioFrameData& frameData) noexcept {
		this->audioBuffer.send(frameData);
	}

	void VoiceConnection::onMessageReceived(const std::string& theMessage) noexcept {
		try {
			if (this->theClients.contains(0) && this->theClients[0]) {
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
							this->voiceConnect();
							this->collectExternalIP();
							DiscordCoreInternal::VoiceSocketProtocolPayloadData protocolPayloadData{};
							protocolPayloadData.externalIp = this->voiceConnectionData.externalIp;
							protocolPayloadData.voiceEncryptionMode = this->voiceConnectionData.voiceEncryptionMode;
							protocolPayloadData.voicePort = this->voiceConnectionData.voicePort;
							nlohmann::json protocolPayloadSelectString = protocolPayloadData;
							this->sendMessage(protocolPayloadSelectString);
							this->theClients[0]->theState = DiscordCoreInternal::WebSocketState::Connected;
							break;
						}
						case 4: {
							for (uint32_t x = 0; x < payload["d"]["secret_key"].size(); x++) {
								this->voiceConnectionData.secretKey.push_back(payload["d"]["secret_key"][x].get<uint8_t>());
							}
							break;
						}
						case 6: {
							this->theClients[0]->haveWeReceivedHeartbeatAck = true;
							break;
						}
						case 8: {
							if (payload["d"].contains("heartbeat_interval")) {
								this->heartbeatInterval = static_cast<int32_t>(payload["d"]["heartbeat_interval"].get<float>());
								this->theClients[0]->areWeHeartBeating = false;
							}
							this->theClients[0]->haveWeReceivedHeartbeatAck = true;
							DiscordCoreInternal::VoiceIdentifyData identifyData{};
							identifyData.connectInitData = this->voiceConnectInitData;
							identifyData.connectionData = this->voiceConnectionData;
							nlohmann::json identityDataFinal = identifyData;
							this->sendMessage(identityDataFinal);
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

	void VoiceConnection::sendMessage(const nlohmann::json& dataToSend) noexcept {
		try {
			if (this->theClients.contains(0) && this->theClients[0]) {
				if (this->configManager->doWePrintWebSocketSuccessMessages()) {
					std::cout << DiscordCoreAPI::shiftToBrightBlue() << "Sending Voice WebSocket Message: " << dataToSend.dump() << DiscordCoreAPI::reset() << std::endl
							  << std::endl;
				}
				DiscordCoreAPI::StopWatch theStopWatch{ 1000ms };
				while (!this->theClients[0]->areWeConnected01.load()) {
					std::this_thread::sleep_for(1ms);
					if (theStopWatch.hasTimePassed()) {
						return;
					}
				}
				std::string theString{};
				this->stringifyJsonData(dataToSend, theString, DiscordCoreInternal::WebSocketOpCode::Op_Text);
				theStopWatch.resetTimer(5000);
				bool didWeWrite{ false };
				do {
					if (theStopWatch.hasTimePassed()) {
						break;
					}
					didWeWrite = this->theClients[0]->writeData(theString, false);
				} while (!didWeWrite);
				if (!didWeWrite) {
					this->onClosed();
				}
			}
		} catch (...) {
			if (this->configManager->doWePrintWebSocketErrorMessages()) {
				DiscordCoreAPI::reportException("VoiceConnection::sendMessage()");
			}
			this->onClosed();
		}
	}

	void VoiceConnection::sendVoiceData(std::string& responseData) noexcept {
		try {
			if (responseData.size() == 0) {
				if (this->configManager->doWePrintWebSocketErrorMessages()) {
					std::cout << DiscordCoreAPI::shiftToBrightRed() << "Please specify voice data to send" << DiscordCoreAPI::reset() << std::endl << std::endl;
				}
				return;
			} else {
				if (this->voiceSocket && this->voiceSocket->areWeStillConnected()) {
					this->voiceSocket->writeData(responseData);
				}
			}
		} catch (...) {
			if (this->configManager->doWePrintWebSocketErrorMessages()) {
				DiscordCoreAPI::reportException("VoiceConnection::sendVoiceData()");
			}
			this->onClosed();
		}
	}

	void VoiceConnection::sendMessage(std::string& dataToSend) noexcept {
		try {
			if (this->theClients.contains(0) && this->theClients[0]) {
				if (this->configManager->doWePrintWebSocketSuccessMessages()) {
					std::cout << DiscordCoreAPI::shiftToBrightBlue() << "Sending Voice WebSocket Message: " << dataToSend << DiscordCoreAPI::reset() << std::endl << std::endl;
				}
				DiscordCoreAPI::StopWatch theStopWatch{ 1000ms };
				while (!this->theClients[0]->areWeConnected01.load()) {
					std::this_thread::sleep_for(1ms);
					if (theStopWatch.hasTimePassed()) {
						return;
					}
				}
				theStopWatch.resetTimer(5000);
				bool didWeWrite{ false };
				do {
					if (theStopWatch.hasTimePassed()) {
						break;
					}
					didWeWrite = this->theClients[0]->writeData(dataToSend, false);
				} while (!didWeWrite);
				if (!didWeWrite) {
					this->onClosed();
				}
			}
		} catch (...) {
			if (this->configManager->doWePrintWebSocketErrorMessages()) {
				DiscordCoreAPI::reportException("VoiceConnection::sendMessage()");
			}
			this->onClosed();
		}
	}

	void VoiceConnection::sendSpeakingMessage(bool isSpeaking) noexcept {
		if (!isSpeaking) {
			this->sendSilence();
		} else {
			if (this->voiceSocket) {
				DiscordCoreInternal::SendSpeakingData theData{};
				theData.delay = 0;
				theData.ssrc = this->voiceConnectionData.audioSSRC;
				nlohmann::json newString = theData;
				this->sendMessage(newString);
			}
		}
	}

	void VoiceConnection::runWebSocket(std::stop_token theToken) noexcept {
		try {
			while (!theToken.stop_requested() && !Globals::doWeQuit.load() && !this->doWeDisconnect.load()) {
				if (this->connections.size() > 0) {
					DiscordCoreAPI::StopWatch theStopWatch{ 10000ms };
					while (!this->theBaseShard->areWeConnected01.load()) {
						if (theStopWatch.hasTimePassed()) {
							return;
						}
						std::this_thread::sleep_for(1ms);
					}
					this->theBaseShard->voiceConnectionDataBufferMap[this->voiceConnectInitData.guildId] = &this->voiceConnectionDataBuffer;
					this->baseSocketAgent->getVoiceConnectionData(this->voiceConnectInitData, this->theBaseShard);
					this->webSocketConnect();
					this->sendSpeakingMessage(false);
					this->sendSpeakingMessage(true);
					this->play();
					this->connections.pop();
				}
				if (this->heartbeatInterval != 0 && !this->theClients[0]->areWeHeartBeating) {
					this->theClients[0]->areWeHeartBeating = true;
					this->theClients[0]->heartBeatStopWatch = DiscordCoreAPI::StopWatch{ std::chrono::milliseconds{ this->heartbeatInterval } };
				}
				if (!theToken.stop_requested() && this->theClients[0]->heartBeatStopWatch.hasTimePassed() && this->theClients[0]->areWeHeartBeating) {
					this->sendHeartBeat();
					this->theClients[0]->heartBeatStopWatch.resetTimer();
				}
				if (!theToken.stop_requested() && this->theClients[0]->areWeStillConnected() && !Globals::doWeQuit.load()) {
					DiscordCoreInternal::WebSocketSSLShard::processIO(this->theClients, 1000);
				}
				if (!theToken.stop_requested() && this->theClients.contains(0) && this->theClients[0] && !Globals::doWeQuit.load()) {
					this->parseHeadersAndMessage(this->theClients[0].get());
					if (this->theClients.contains(0) && this->theClients[0] && this->theClients[0]->processedMessages.size() > 0) {
						std::string theMessage = this->theClients[0]->processedMessages.front();
						if (theMessage.size() > 0) {
							this->onMessageReceived(theMessage);
						}
						this->theClients[0]->processedMessages.pop();
					}
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

	void VoiceConnection::runVoice(std::stop_token theToken) noexcept {
		while (!theToken.stop_requested()) {
			std::this_thread::sleep_for(1ms);
			if (!this->didWeJustConnect) {
				this->audioBuffer.clearContents();
				this->clearAudioData();
				this->didWeJustConnect = false;
			}
			if (!this->playSetEvent.wait(10000)) {
				if (theToken.stop_requested()) {
					this->areWePlaying.store(false);
					return;
				}
				continue;
			};
			this->playSetEvent.reset();
		start:
			std::this_thread::sleep_for(1ms);
			if (theToken.stop_requested()) {
				this->areWePlaying.store(false);
				return;
			}
			this->audioData.type = AudioFrameType::Unset;
			this->audioData.encodedFrameData.data.clear();
			this->audioData.rawFrameData.data.clear();
			this->audioBuffer.tryReceive(this->audioData);
			if ((this->audioData.encodedFrameData.sampleCount == 0 && this->audioData.rawFrameData.sampleCount == 0) || this->audioData.type == AudioFrameType::Unset) {
				this->areWePlaying.store(false);
				goto start;
			}
			this->areWePlaying.store(true);
			int64_t startingValue{ std::chrono::duration_cast<std::chrono::nanoseconds, int64_t>(std::chrono::system_clock::now().time_since_epoch()).count() };
			int64_t intervalCount{ 20000000 };
			int32_t frameCounter{ 0 };
			int64_t totalTime{ 0 };
			if (this->disconnectStartTime != 0) {
				int64_t currentTime = static_cast<int64_t>(std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count());
				if (currentTime - this->disconnectStartTime >= 60000) {
					this->onClosed();
					this->connect();
					this->play();
				}
			}
			this->sendSpeakingMessage(true);
			while (
				(this->audioData.rawFrameData.sampleCount != 0 || this->audioData.encodedFrameData.sampleCount != 0) && !this->areWeStopping.load() && !theToken.stop_requested()) {
				if (!theToken.stop_requested() && this->voiceSocket && this->voiceSocket->areWeStillConnected()) {
					this->voiceSocket->processIO();
					this->voiceSocket->getInputBuffer();
				}
				while (!this->voiceSocket && !this->doWeDisconnect.load()) {
					std::this_thread::sleep_for(1ms);
				}
				while (!this->voiceSocket->areWeConnected.load() && !this->doWeDisconnect.load()) {
					std::this_thread::sleep_for(1ms);
				}
				this->areWePlaying.store(true);
				if (this->areWeStopping.load()) {
					this->areWePlaying.store(false);
					break;
				}
				this->pauseEvent.wait(240000);
				if (theToken.stop_requested()) {
					this->areWePlaying.store(false);
					return;
				}
				frameCounter++;
				this->audioBuffer.tryReceive(this->audioData);
				if (this->audioData.guildMemberId != 0) {
					this->currentGuildMemberId = this->audioData.guildMemberId;
				}
				nanoSleep(100000);
				if (theToken.stop_requested()) {
					this->areWePlaying.store(false);
					return;
				}
				if (this->audioData.type != AudioFrameType::Unset && this->audioData.type != AudioFrameType::Skip && !this->areWeStopping.load()) {
					std::string newFrame{};
					if (this->audioData.type == AudioFrameType::RawPCM) {
						std::vector<RawFrameData> rawFrames{};
						rawFrames.push_back(this->audioData.rawFrameData);
						auto encodedFrameData = this->encoder->encodeFrames(rawFrames);
						newFrame = this->encryptSingleAudioFrame(encodedFrameData[0].encodedFrameData, this->voiceConnectionData.audioSSRC, this->voiceConnectionData.secretKey);
					} else {
						newFrame = this->encryptSingleAudioFrame(this->audioData.encodedFrameData, this->voiceConnectionData.audioSSRC, this->voiceConnectionData.secretKey);
					}
					if (newFrame.size() == 0) {
						continue;
					}
					nanoSleep(18000000);
					if (theToken.stop_requested()) {
						this->areWePlaying.store(false);
						return;
					}
					auto timeCounter =
						static_cast<int64_t>(std::chrono::duration_cast<std::chrono::nanoseconds>(std::chrono::system_clock::now().time_since_epoch()).count() - startingValue);
					auto waitTime = intervalCount - timeCounter;
					spinLock(waitTime);
					startingValue = static_cast<int64_t>(std::chrono::duration_cast<std::chrono::nanoseconds>(std::chrono::system_clock::now().time_since_epoch()).count());
					this->sendSingleAudioFrame(newFrame);
					totalTime +=
						static_cast<int64_t>(std::chrono::duration_cast<std::chrono::nanoseconds>(std::chrono::system_clock::now().time_since_epoch()).count() - startingValue);
					intervalCount = 20000000 - (totalTime / frameCounter);
					this->audioData.type = AudioFrameType::Unset;
					this->audioData.encodedFrameData.data.clear();
					this->audioData.rawFrameData.data.clear();
				} else if (this->audioData.type == AudioFrameType::Skip && !this->areWeStopping.load()) {
					SongCompletionEventData completionEventData{};
					completionEventData.guild = Guilds::getCachedGuildAsync({ .guildId = this->voiceConnectInitData.guildId }).get();
					completionEventData.guildMember =
						GuildMembers::getCachedGuildMemberAsync({ .guildMemberId = this->currentGuildMemberId, .guildId = this->voiceConnectInitData.guildId }).get();
					completionEventData.wasItAFail = false;
					getSongAPIMap()[this->voiceConnectInitData.guildId]->onSongCompletionEvent(completionEventData);
					break;
				}
			}
			this->areWePlaying.store(false);
			this->disconnectStartTime = static_cast<int64_t>(std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count());
			this->sendSpeakingMessage(false);
			this->clearAudioData();
			if (this->areWeStopping.load()) {
				this->stopSetEvent.wait(5000);
				this->stopSetEvent.reset();
				this->areWeStopping.store(false);
			}

			if (theToken.stop_requested()) {
				return;
			}
		}
	};

	bool VoiceConnection::areWeCurrentlyPlaying() noexcept {
		if (this == nullptr) {
			return false;
		} else {
			return this->areWePlaying.load();
		}
	}

	void VoiceConnection::collectExternalIP() noexcept {
		try {
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
			this->voiceSocket->writeData(packet);
			std::this_thread::sleep_for(100ms);
			std::string inputString{};
			while (inputString.size() < 74) {
				this->voiceSocket->processIO();
				std::string theNewString = this->voiceSocket->getInputBuffer();
				inputString.insert(inputString.end(), theNewString.begin(), theNewString.end());
				std::this_thread::sleep_for(1ms);
			}
			std::string message{};
			message.insert(message.begin(), inputString.begin() + 8, inputString.begin() + 64);
			if (message.find('\u0000') != std::string::npos) {
				message = message.substr(0, message.find('\u0000', 5));
			}
			this->voiceConnectionData.externalIp = message;
			this->voiceSocket->areWeConnected.store(true);
			this->areWeConnectedBool.store(true);
			this->voiceConnectionDataBuffer.clearContents();
		} catch (...) {
			if (this->configManager->doWePrintWebSocketErrorMessages()) {
				DiscordCoreAPI::reportException("VoiceConnection::collectExternalIP()");
			}
			this->onClosed();
		}
	}

	void VoiceConnection::webSocketConnect() noexcept {
		try {
			this->voiceConnectionData = DiscordCoreInternal::VoiceConnectionData{};
			DiscordCoreAPI::waitForTimeToPass(this->voiceConnectionDataBuffer, this->voiceConnectionData, 20000);
			this->baseUrl = this->voiceConnectionData.endPoint.substr(0, this->voiceConnectionData.endPoint.find(":"));
			auto theClient = std::make_unique<DiscordCoreInternal::WebSocketSSLShard>(&this->connections, 0, 0, this->configManager);
			if (!theClient->connect(this->baseUrl, "443")) {
				this->onClosed();
			}
			std::string sendVector = "GET /?v=4 HTTP/1.1\r\nHost: " + this->baseUrl +
				"\r\nPragma: no-cache\r\nUser-Agent: DiscordCoreAPI/1.0\r\nUpgrade: WebSocket\r\nConnection: Upgrade\r\nSec-WebSocket-Key: " +
				DiscordCoreAPI::generateBase64EncodedKey() + "\r\nSec-WebSocket-Version: 13\r\n\r\n";
			this->theClients[0] = std::move(theClient);
			this->theClients[0]->areWeConnected01.store(true);
			bool didWeWrite{ false };
			DiscordCoreAPI::StopWatch theStopWatch{ 10000ms };
			do {
				if (theStopWatch.hasTimePassed()) {
					break;
				}
				didWeWrite = this->theClients[0]->writeData(sendVector, true);
			} while (!didWeWrite);
			if (!didWeWrite) {
				this->onClosed();
			}
			theStopWatch.resetTimer();
			DiscordCoreInternal::WebSocketSSLShard::processIO(this->theClients);
			while (!Globals::doWeQuit.load()) {
				if (this->theClients[0]->theState == DiscordCoreInternal::WebSocketState::Connected) {
					break;
				}
				if (this->theClients.contains(0) && this->theClients[0]->inputBuffer.size() > 0) {
					this->parseHeadersAndMessage(this->theClients[0].get());
				}
				if (this->theClients.contains(0) && this->theClients[0]->processedMessages.size() > 0) {
					std::string theMessage = this->theClients[0]->processedMessages.front();
					this->theClients[0]->processedMessages.pop();
					this->onMessageReceived(theMessage);
				}
				DiscordCoreInternal::WebSocketSSLShard::processIO(this->theClients);
				std::this_thread::sleep_for(1ms);
				if (this->theClients.contains(0) && theStopWatch.hasTimePassed()) {
					this->theClients[0]->disconnect();
					this->onClosed();
					return;
				}
			}
		} catch (...) {
			if (this->configManager->doWePrintWebSocketErrorMessages()) {
				DiscordCoreAPI::reportException("VoiceConnection::connect()");
			}
			this->onClosed();
		}
	}

	void VoiceConnection::clearAudioData() noexcept {
		if (this->audioData.encodedFrameData.data.size() != 0 && this->audioData.rawFrameData.data.size() != 0) {
			this->audioData.encodedFrameData.data.clear();
			this->audioData.rawFrameData.data.clear();
			this->audioData = AudioFrameData();
		}
		AudioFrameData frameData{};
		while (this->audioBuffer.tryReceive(frameData)) {
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
			if (this->theClients[0] && this->theClients[0]->areWeStillConnected() && this->theClients[0]->haveWeReceivedHeartbeatAck) {
				nlohmann::json data{};
				data["d"] = std::chrono::duration_cast<std::chrono::nanoseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
				data["op"] = int32_t(3);
				this->sendMessage(data);
				this->theClients[0]->haveWeReceivedHeartbeatAck = false;
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

	void VoiceConnection::voiceConnect() noexcept {
		try {
			this->voiceSocket = std::make_unique<DiscordCoreInternal::DatagramSocketSSLClient>();
			if (!this->voiceSocket->connect(this->voiceConnectionData.voiceIp, this->voiceConnectionData.voicePort)) {
				this->onClosed();
			}
		} catch (...) {
			if (this->configManager->doWePrintWebSocketErrorMessages()) {
				DiscordCoreAPI::reportException("VoiceConnection::voiceConnect()");
			}
			this->onClosed();
		}
	}

	void VoiceConnection::sendSilence() noexcept {
		EncodedFrameData newFrame{};
		newFrame.data.push_back(0xf8);
		newFrame.data.push_back(0xff);
		newFrame.data.push_back(0xfe);
		auto theFrame = this->encryptSingleAudioFrame(newFrame, this->voiceConnectionData.audioSSRC, this->voiceConnectionData.secretKey);
		this->sendSingleAudioFrame(theFrame);
	}

	void VoiceConnection::pauseToggle() noexcept {
		if (this) {
			if (this->pauseEvent.checkStatus()) {
				sendSpeakingMessage(false);
				this->pauseEvent.reset();
			} else {
				sendSpeakingMessage(true);
				this->pauseEvent.set();
			}
		}
	}

	void VoiceConnection::disconnect() noexcept {
		this->stopSetEvent.set();
		this->playSetEvent.set();
		this->areWeConnectedBool.store(false);
		this->theClients[0]->disconnect();
		this->voiceSocket->disconnect();
		this->doWeDisconnect.store(true);
		this->areWePlaying.store(false);
		this->areWeStopping.store(true);
		this->sendSpeakingMessage(false);
		if (this->voiceSocket) {
			this->voiceSocket.reset(nullptr);
		}
		if (this->theTask02) {
			this->theTask02->request_stop();
			if (this->theTask02->joinable()) {
				this->theTask02->join();
			}
			this->theTask02.reset(nullptr);
		}
		if (this->theTask01) {
			this->theTask01->request_stop();
			if (this->theTask01->joinable()) {
				this->theTask01->join();
			}
			this->theTask01.reset(nullptr);
		}
		auto thePtr = getSongAPIMap()[this->voiceConnectInitData.guildId].get();
		if (thePtr) {
			thePtr->onSongCompletionEvent.remove(thePtr->theToken);
		}
	}

	void VoiceConnection::onClosed() noexcept {
		if (this->theClients[0] && this->areWeConnectedBool.load()) {
			this->theClients[0]->disconnect();
			if (this->voiceSocket) {
				this->voiceSocket->disconnect();
				this->voiceSocket->areWeConnected.store(false);
			}
			this->areWeConnectedBool.store(false);
			this->areWePlaying.store(true);
			this->theClients[0]->areWeHeartBeating = false;
			this->theClients[0]->areWeConnected01.store(false);
		}
	}

	void VoiceConnection::connect() noexcept {
		if (this->baseSocketAgent->theClients.contains(this->voiceConnectInitData.currentShard) && this->baseSocketAgent->theClients[voiceConnectInitData.currentShard]) {
			this->areWeStopping.store(false);
			this->stopSetEvent.set();
			this->pauseEvent.set();
			if (!this->theTask01) {
				this->theTask01 = std::make_unique<std::jthread>([=, this](std::stop_token theToken) {
					this->runWebSocket(theToken);
				});
			}
			if (!this->theTask02) {
				this->theTask02 = std::make_unique<std::jthread>([=, this](std::stop_token theToken) {
					this->runVoice(theToken);
				});
			}
			ConnectionPackage dataPackage{};
			dataPackage.currentShard = 0;
			this->connections.push(dataPackage);
			return;
		}
		return;
	}

	bool VoiceConnection::stop() noexcept {
		if (this->areWePlaying.load()) {
			this->areWePlaying.store(false);
			this->areWeStopping.store(true);
			this->clearAudioData();
			this->stopSetEvent.set();
			return true;
		}
		return false;
	}

	bool VoiceConnection::play() noexcept {
		if (this) {
			this->playSetEvent.set();
			return true;
		}
		return false;
	}

	VoiceConnection::~VoiceConnection() noexcept {
		if (this->theTask01) {
			this->theTask01->request_stop();
			if (this->theTask01->joinable()) {
				this->theTask01->join();
				this->theTask01.reset(nullptr);
			}
		}
		if (this->theTask02) {
			this->theTask02->request_stop();
			if (this->theTask02->joinable()) {
				this->theTask02->join();
				this->theTask02.reset(nullptr);
			}
		}
	};

}