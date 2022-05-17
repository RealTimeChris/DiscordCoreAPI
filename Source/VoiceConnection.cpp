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
/// VoiceConnection.cpp - Source file for the voice connection class.
/// Jul 15, 2021
/// Chris M.
/// https://discordcoreapi.com
/// \file VoiceConnection.cpp

#include <discordcoreapi/VoiceConnection.hpp>
#include <discordcoreapi/DiscordCoreClient.hpp>
#include <discordcoreapi/JSONIfier.hpp>

namespace DiscordCoreAPI {

	std::string VoiceConnection::encryptSingleAudioFrame(EncodedFrameData bufferToSend, int32_t audioSSRC, const std::string&  keys) {
		if (keys.size() > 0) {
			this->sequenceIndex += 1;
			this->timeStamp += 960;
			const int32_t nonceSize{ crypto_secretbox_NONCEBYTES };
			const int32_t headerSize{ 12 };
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
			for (uint32_t x = 0; x < headerSize; x += 1) {
				nonceForLibSodium[x] = headerFinal[x];
			}
			for (int32_t x = headerSize; x < nonceSize; x += 1) {
				nonceForLibSodium[x] = 0;
			}
			uint64_t numOfBytes{ headerSize + bufferToSend.data.size() + crypto_secretbox_MACBYTES };
			std::unique_ptr<uint8_t[]> audioDataPacket{ std::make_unique<uint8_t[]>(numOfBytes) };
			for (uint32_t x = 0; x < headerSize; x += 1) {
				audioDataPacket[x] = headerFinal[x];
			}
			std::unique_ptr<unsigned char[]> encryptionKeys{ std::make_unique<unsigned char[]>(keys.size()) };
			for (uint32_t x = 0; x < keys.size(); x += 1) {
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

	VoiceConnection::VoiceConnection(DiscordCoreInternal::BaseSocketAgent* BaseSocketAgentNew) {
		this->baseSocketAgent = BaseSocketAgentNew;
	}

	std::string VoiceConnection::getChannelId() {
		if (this && this->voiceConnectInitData.channelId != "") {
			return this->voiceConnectInitData.channelId;
		} else {
			return std::string();
		}
	}

	TSUnboundedMessageBlock<AudioFrameData>& VoiceConnection::getAudioBuffer() {
		return this->audioBuffer;
	}

	void VoiceConnection::sendSingleFrame(AudioFrameData frameData) {
		this->audioBuffer.send(frameData);
	}

	bool VoiceConnection::areWeConnected() {
		if (this == nullptr) {
			return false;
		} else {
			return this->areWeConnectedBool;
		}
	}

	bool VoiceConnection::areWeCurrentlyPlaying() {
		if (this == nullptr) {
			return false;
		} else {
			return this->areWePlaying.load();
		}
	}

	bool VoiceConnection::stop() {
		if (this->areWePlaying.load()) {
			this->areWePlaying.store(false);
			this->areWeStopping.store(true);
			this->clearAudioData();
			this->stopSetEvent.set();
			return true;
		}
		return false;
	}

	bool VoiceConnection::play() {
		if (this) {
			this->playSetEvent.set();
			return true;
		}
		return false;
	}

	void VoiceConnection::pauseToggle() {
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

	void VoiceConnection::reconnect() {
		this->connect(this->voiceConnectInitData);
		this->play();
	}

	void VoiceConnection::connect(DiscordCoreInternal::VoiceConnectInitData voiceConnectInitDataNew) {
		this->areWeConnectedBool = true;
		this->areWeStopping.store(false);
		this->stopSetEvent.set();
		this->pauseEvent.set();
		this->voiceConnectInitData = voiceConnectInitDataNew;
		if (this->voiceSocketAgent) {
			this->voiceSocketAgent.reset(nullptr);
		}
		if (!this->baseSocketAgent->areWeReadyToConnectEvent.wait(10000)) {
			return;
		}
		this->voiceSocketAgent =
			std::make_unique<DiscordCoreInternal::VoiceSocketAgent>(this->voiceConnectInitData, this->baseSocketAgent, this->baseSocketAgent->printSuccessMessages);
		this->doWeReconnect = &this->voiceSocketAgent->doWeReconnect;
		if (!this->voiceSocketAgent->areWeConnected.wait(10000)) {
			return;
		}
		this->voiceConnectionData = &this->voiceSocketAgent->voiceConnectionData;
		if (this->theTask == nullptr) {
			this->theTask = std::make_unique<std::jthread>([=, this](std::stop_token theToken) {
				this->run(theToken);
			});
		}
	}

	void VoiceConnection::disconnect() {
		this->stopSetEvent.set();
		this->playSetEvent.set();
		this->areWeConnectedBool = false;
		this->areWePlaying.store(false);
		this->areWeStopping.store(true);
		this->sendSpeakingMessage(false);
		if (this->voiceSocketAgent) {
			this->voiceSocketAgent.reset(nullptr);
		}
		if (this->theTask) {
			this->theTask->request_stop();
			if (this->theTask->joinable()) {
				this->theTask->join();
			}
			this->theTask.reset(nullptr);
		}
		auto thePtr = getSongAPIMap()[this->voiceConnectInitData.guildId].get();
		if (thePtr) {
			thePtr->onSongCompletionEvent.remove(thePtr->theToken);
		}
	}

	void VoiceConnection::clearAudioData() {
		if (this->audioData.encodedFrameData.data.size() != 0 && this->audioData.rawFrameData.data.size() != 0) {
			this->audioData.encodedFrameData.data.clear();
			this->audioData.rawFrameData.data.clear();
			this->audioData = AudioFrameData();
		}
		AudioFrameData frameData{};
		while (this->audioBuffer.tryReceive(frameData)) {
		};
	}

	void VoiceConnection::sendSingleAudioFrame(std::string& audioDataPacketNew) {
		if (this->voiceSocketAgent) {
			if (this->voiceSocketAgent->voiceSocket) {
				this->voiceSocketAgent->sendVoiceData(audioDataPacketNew);
			}
		}
	}

	void VoiceConnection::sendSilence() {
		EncodedFrameData newFrame{};
		newFrame.data.push_back(0xf8);
		newFrame.data.push_back(0xff);
		newFrame.data.push_back(0xfe);
		newFrame.sampleCount = 960;
		for (uint8_t x = 0; x < 5; x += 1) {
			auto encryptedFrame = this->encryptSingleAudioFrame(newFrame, this->voiceConnectionData->audioSSRC, this->voiceConnectionData->secretKey);
			this->sendSingleAudioFrame(encryptedFrame);
		}
	}

	void VoiceConnection::sendSpeakingMessage(bool isSpeaking) {
		if (!isSpeaking) {
			this->sendSilence();
		} else {
			if (this->voiceSocketAgent) {
				std::vector<uint8_t> newString = DiscordCoreInternal::JSONIFY(this->voiceSocketAgent->voiceConnectionData.audioSSRC, 0);
				if (this->voiceSocketAgent->webSocket) {
					this->voiceSocketAgent->sendMessage(newString);
				}
			}
		}
	}

	void VoiceConnection::run(std::stop_token theToken) {
		while (!theToken.stop_requested()) {
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
					this->reconnect();
				}
			}
			this->sendSpeakingMessage(true);
			while ((this->audioData.rawFrameData.sampleCount != 0 || this->audioData.encodedFrameData.sampleCount != 0) && !this->areWeStopping && !theToken.stop_requested()) {
				this->areWePlaying.store(true);
				if (!this->doWeReconnect->wait(0)) {
					this->areWeConnectedBool = false;
					this->sendSpeakingMessage(false);
					this->reconnect();
					this->sendSpeakingMessage(true);
					this->areWePlaying.store(true);
					this->doWeReconnect->set();
				}
				if (this->areWeStopping.load()) {
					this->areWePlaying.store(false);
					break;
				}
				this->pauseEvent.wait(240000);
				if (theToken.stop_requested()) {
					this->areWePlaying.store(false);
					return;
				}
				frameCounter += 1;
				this->audioBuffer.tryReceive(this->audioData);
				if (this->audioData.guildMemberId != "") {
					this->currentGuildMemberId = this->audioData.guildMemberId;
				}
				nanoSleep(100000);
				if (theToken.stop_requested()) {
					this->areWePlaying.store(false);
					return;
				}
				if (this->audioData.type != AudioFrameType::Unset && this->audioData.type != AudioFrameType::Skip && !this->areWeStopping) {
					std::string newFrame{};
					if (this->audioData.type == AudioFrameType::RawPCM) {
						std::vector<RawFrameData> rawFrames{};
						rawFrames.push_back(this->audioData.rawFrameData);
						auto encodedFrameData = this->encoder->encodeFrames(rawFrames);
						newFrame = this->encryptSingleAudioFrame(encodedFrameData[0].encodedFrameData, this->voiceConnectionData->audioSSRC, this->voiceConnectionData->secretKey);
					} else {
						newFrame = this->encryptSingleAudioFrame(this->audioData.encodedFrameData, this->voiceConnectionData->audioSSRC, this->voiceConnectionData->secretKey);
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
				} else if (this->audioData.type == AudioFrameType::Skip && !this->areWeStopping) {
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

	VoiceConnection::~VoiceConnection() {
		if (this->theTask) {
			this->theTask->request_stop();
			if (this->theTask->joinable()) {
				this->theTask->join();
				this->theTask.reset(nullptr);
			}
		}
	}

}