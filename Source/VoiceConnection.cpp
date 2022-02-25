/*
*    # License

   Copyright 2021 Chris M.

   Licensed under the Apache License, Version 2.0 (the "License");
   you may not use this file except in compliance with the License.
   You may obtain a copy of the License at

	   http://www.apache.org/licenses/LICENSE-2.0

   Unless required by applicable law or agreed to in writing, software
   distributed under the License is distributed on an "AS IS" BASIS,
   WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
   See the License for the specific language governing permissions and
   limitations under the License.
*/
/// VoiceConnection.cpp - Source file for the voice connection class.
/// Jul 15, 2021
/// Chris M.
/// https://github.com/RealTimeChris/DiscordCoreAPI
/// \file VoiceConnection.cpp

#include <VoiceConnection.hpp>
#include <WebSocketEntities.hpp>
#include <DiscordCoreClient.hpp>
#include <CoRoutine.hpp>
#include <JSONIfier.hpp>

namespace DiscordCoreAPI {

	AudioEncrypter::AudioEncrypter() {
		if (sodium_init() == -1) {
			std::cout << "LibSodium failed to initialize!" << std::endl << std::endl;
		}
	};

	std::string AudioEncrypter::encryptSingleAudioFrame(EncodedFrameData bufferToSend, int32_t audioSSRC, std::string keys) {
		if (keys.size() > 0) {
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
			std::vector<uint8_t> nonceForLibSodium{};
			nonceForLibSodium.resize(nonceSize);
			for (uint32_t x = 0; x < headerSize; x += 1) {
				nonceForLibSodium[x] = headerFinal[x];
			}
			for (int32_t x = headerSize; x < nonceSize; x += 1) {
				nonceForLibSodium[x] = 0;
			}
			uint64_t numOfBytes{ headerSize + bufferToSend.data.size() + crypto_secretbox_MACBYTES };
			std::vector<uint8_t> audioDataPacket{};
			audioDataPacket.resize(numOfBytes);
			for (uint32_t x = 0; x < headerSize; x += 1) {
				audioDataPacket[x] = headerFinal[x];
			}
			std::vector<unsigned char> encryptionKeys{};
			encryptionKeys.resize(keys.size());
			for (uint32_t x = 0; x < keys.size(); x += 1) {
				encryptionKeys[x] = keys[x];
			}
			if (crypto_secretbox_easy(audioDataPacket.data() + headerSize, bufferToSend.data.data(), bufferToSend.data.size(), nonceForLibSodium.data(), encryptionKeys.data()) != 0) {
				throw std::runtime_error("ENCRYPTION FAILED!");
			};
			bufferToSend.data.clear();
			this->sequenceIndex += 1;
			this->timeStamp += static_cast<int32_t>(bufferToSend.sampleCount);
			audioDataPacket.shrink_to_fit();
			std::string audioDataPacketNew{};
			audioDataPacketNew.insert(audioDataPacketNew.begin(), audioDataPacket.begin(), audioDataPacket.end());
			return audioDataPacketNew;
		}
		return std::string{};
	}

	VoiceConnection::VoiceConnection(DiscordCoreInternal::BaseSocketAgent* BaseSocketAgentNew) {
		this->baseSocketAgent = BaseSocketAgentNew;
	}

	std::string VoiceConnection::getChannelId() {
		if (this != nullptr && this->voiceConnectInitData.channelId != "") {
			return this->voiceConnectInitData.channelId;
		}
		else {
			return std::string();
		}
	}

	TSUnboundedMessageBlock<AudioFrameData>& VoiceConnection::getAudioBuffer() {
		return this->audioBuffer;
	}

	void VoiceConnection::sendSingleFrame(AudioFrameData frameData) {
		this->audioBuffer.send(std::move(frameData));
	}

	bool VoiceConnection::areWeConnected() {
		if (this == nullptr) {
			return false;
		}
		else {
			return this->areWeConnectedBool;
		}
	}

	bool VoiceConnection::areWeCurrentlyPlaying() {
		if (this == nullptr) {
			return false;
		}
		else {
			return this->areWePlaying.load(std::memory_order_seq_cst);
		}
	}

	bool VoiceConnection::stop() {
		if (this->areWePlaying.load(std::memory_order_seq_cst)) {
			this->areWePlaying.store(false, std::memory_order_seq_cst);
			this->areWeStopping.store(true, std::memory_order_seq_cst);
			this->clearAudioData();
			this->stopSetEvent.set();
			return true;
		}
		return false;
	}

	bool VoiceConnection::play() {
		if (this != nullptr) {
			this->playSetEvent.set();
			return true;
		}
		return false;
	}

	void VoiceConnection::pauseToggle() {
		if (this != nullptr) {
			if (this->pauseEvent.checkStatus()) {
				sendSpeakingMessage(false);
				this->pauseEvent.reset();
			}
			else {
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
		try {
			this->areWeConnectedBool = true;
			this->areWeStopping.store(false, std::memory_order_seq_cst);
			this->stopSetEvent.set();
			this->pauseEvent.set();
			this->voiceConnectInitData = voiceConnectInitDataNew;
			if (this->voiceSocketAgent != nullptr) {
				this->voiceSocketAgent->theTask->cancel();
				this->voiceSocketAgent->theTask.reset(nullptr);
				this->voiceSocketAgent.reset(nullptr);
			}
			if (!this->baseSocketAgent->areWeReadyToConnectEvent.wait(10000)) {
				return;
			}
			this->voiceSocketAgent = std::make_unique<DiscordCoreInternal::VoiceSocketAgent>(this->voiceConnectInitData, this->baseSocketAgent);
			this->doWeReconnect = &this->voiceSocketAgent->doWeReconnect;
			if (!this->voiceSocketAgent->areWeConnected.wait(10000)){
				return;
			}
			this->voiceConnectionData = &this->voiceSocketAgent->voiceConnectionData;
			if (this->theTask == nullptr) {
				this->theTask = std::make_unique<CoRoutine<void>>(this->run());
			}
		}
		catch (...) {
			DiscordCoreAPI::reportException("VoiceConnection::connect()");
		}
	}

	void VoiceConnection::disconnect() {
		this->stopSetEvent.set();
		this->playSetEvent.set();
		this->areWeConnectedBool = false;
		this->areWePlaying.store(false, std::memory_order_seq_cst);
		this->areWeStopping.store(true, std::memory_order_seq_cst);
		if (this->voiceSocketAgent != nullptr) {
			this->voiceSocketAgent->theTask->cancel();
			this->voiceSocketAgent->theTask.reset(nullptr);
			this->voiceSocketAgent.reset(nullptr);
		}
		this->sendSpeakingMessage(false);
		this->theTask->cancel();
		this->theTask.reset(nullptr);
		auto thePtr = getSongAPIMap()[this->voiceConnectInitData.guildId].get();
		if (thePtr != nullptr) {
			getSongAPIMap()[this->voiceConnectInitData.guildId]->onSongCompletionEvent = std::function<CoRoutine<void>(SongCompletionEventData)>{};
		}
	}

	void VoiceConnection::clearAudioData() {
		if (this->audioData.encodedFrameData.data.size() != 0 && this->audioData.rawFrameData.data.size() != 0) {
			this->audioData.encodedFrameData.data.clear();
			this->audioData.rawFrameData.data.clear();
			this->audioData = AudioFrameData();
		}
		AudioFrameData frameData{};
		while (this->audioBuffer.tryReceive(frameData)) {};
	}

	void VoiceConnection::sendSingleAudioFrame(std::string& audioDataPacketNew) {
		if (this->voiceSocketAgent != nullptr) {
			if (this->voiceSocketAgent->voiceSocket != nullptr) {
				this->voiceSocketAgent->sendVoiceData(audioDataPacketNew);
			}
		}
	}

	void VoiceConnection::sendSilence() {
		std::vector<EncodedFrameData> frameData{};
		for (uint32_t x = 0; x < 5; x += 1) {
			EncodedFrameData newFrame{};
			newFrame.data.push_back(0xF8);
			newFrame.data.push_back(0xFF);
			newFrame.data.push_back(0xFE);
			newFrame.sampleCount = 3;
			frameData.push_back(newFrame);
		}
		for (auto& value : frameData) {
			DiscordCoreAPI::EventWaiter frameWaiter{};
			frameWaiter.reset();
			frameWaiter.wait(20);
			auto newerFrame = this->audioEncrypter.encryptSingleAudioFrame(value, this->voiceConnectionData->audioSSRC, this->voiceConnectionData->secretKey);
			this->sendSingleAudioFrame(newerFrame);
		}
	}

	void VoiceConnection::sendSpeakingMessage(bool isSpeaking) {
		if (this->voiceSocketAgent != nullptr) {
			this->voiceConnectionData->audioSSRC = this->voiceSocketAgent->voiceConnectionData.audioSSRC;
			std::vector<uint8_t> newString = DiscordCoreInternal::JSONIFY(isSpeaking, this->voiceConnectionData->audioSSRC, 0);
			if (this->voiceSocketAgent->webSocket != nullptr) {
				this->voiceSocketAgent->sendMessage(newString);
			}
		}
	}

	CoRoutine<void> VoiceConnection::run() {
		auto cancelHandle = co_await NewThreadAwaitable<void>();
		try {
			while (!cancelHandle.promise().isItStopped()) {
				if (!this->didWeJustConnect) {
					this->audioBuffer.clearContents();
					this->clearAudioData();
					this->didWeJustConnect = false;
				}
				if (!this->playSetEvent.wait(10000)) {
					if (cancelHandle.promise().isItStopped()) {
						this->areWePlaying.store(false, std::memory_order_seq_cst);
						co_return;
					}
					continue;
				};
				this->playSetEvent.reset();
			start:
				if (cancelHandle.promise().isItStopped()) {
					this->areWePlaying.store(false, std::memory_order_seq_cst);
					co_return;
				}
				this->audioData.type = AudioFrameType::Unset;
				this->audioData.encodedFrameData.data.clear();
				this->audioData.rawFrameData.data.clear();
				this->audioBuffer.tryReceive(this->audioData);
				if ((this->audioData.encodedFrameData.sampleCount == 0 && this->audioData.rawFrameData.sampleCount == 0) || this->audioData.type == AudioFrameType::Unset) {
					this->areWePlaying.store(false, std::memory_order_seq_cst);
					goto start;
				}
				this->areWePlaying.store(true, std::memory_order_seq_cst);
				int64_t startingValue{ std::chrono::duration_cast<std::chrono::nanoseconds, int64_t>(std::chrono::system_clock::now().time_since_epoch()).count() };
				int64_t intervalCount{ 20000000 };
				int32_t frameCounter{ 0 };
				int64_t totalTime{ 0 };
				this->sendSpeakingMessage(true);
				while ((this->audioData.rawFrameData.sampleCount != 0 || this->audioData.encodedFrameData.sampleCount != 0) && !this->areWeStopping && !cancelHandle.promise().isItStopped()) {
					this->areWePlaying.store(true, std::memory_order_seq_cst);
					if (!this->doWeReconnect->wait(0)) {
						this->areWeConnectedBool = false;
						this->sendSilence();
						this->sendSpeakingMessage(false);
						this->reconnect();
						this->sendSpeakingMessage(true);
						this->areWePlaying.store(true, std::memory_order_seq_cst);
						this->doWeReconnect->set();
					}
					if (this->areWeStopping.load(std::memory_order_seq_cst)) {
						this->areWePlaying.store(false, std::memory_order_seq_cst);
						break;
					}
					this->pauseEvent.wait(240000);
					if (cancelHandle.promise().isItStopped()) {
						this->areWePlaying.store(false, std::memory_order_seq_cst);
						co_return;
					}
					frameCounter += 1;
					this->audioBuffer.tryReceive(this->audioData);
					if (this->audioData.guildMemberId != "") {
						this->currentGuildMemberId = this->audioData.guildMemberId;
					}
					nanoSleep(100000);
					if (cancelHandle.promise().isItStopped()) {
						this->areWePlaying.store(false, std::memory_order_seq_cst);
						co_return;
					}
					if (this->audioData.type != AudioFrameType::Unset && this->audioData.type != AudioFrameType::Skip && !this->areWeStopping) {
						std::string newFrame{};
						if (this->audioData.type == AudioFrameType::RawPCM) {
							auto encodedFrameData = this->encoder->encodeSingleAudioFrame(this->audioData.rawFrameData);
							newFrame = this->audioEncrypter.encryptSingleAudioFrame(encodedFrameData, this->voiceConnectionData->audioSSRC, this->voiceConnectionData->secretKey);
						}
						else {
							newFrame = this->audioEncrypter.encryptSingleAudioFrame(this->audioData.encodedFrameData, this->voiceConnectionData->audioSSRC, this->voiceConnectionData->secretKey);
						}
						if (newFrame.size() == 0) {
							continue;
						}
						nanoSleep(18000000);
						if (cancelHandle.promise().isItStopped()) {
							this->areWePlaying.store(false, std::memory_order_seq_cst);
							co_return;
						}
						auto timeCounter = static_cast<int64_t>(std::chrono::duration_cast<std::chrono::nanoseconds>(std::chrono::system_clock::now().time_since_epoch()).count() - startingValue);
						auto waitTime = intervalCount - timeCounter;
						spinLock(waitTime);
						startingValue = static_cast<int64_t>(std::chrono::duration_cast<std::chrono::nanoseconds>(std::chrono::system_clock::now().time_since_epoch()).count());
						this->sendSingleAudioFrame(newFrame);
						totalTime += static_cast<int64_t>(std::chrono::duration_cast<std::chrono::nanoseconds>(std::chrono::system_clock::now().time_since_epoch()).count() - startingValue);
						intervalCount = 20000000 - (totalTime / frameCounter);
						this->audioData.type = AudioFrameType::Unset;
						this->audioData.encodedFrameData.data.clear();
						this->audioData.rawFrameData.data.clear();
					}
					else if (this->audioData.type == AudioFrameType::Skip && !this->areWeStopping) {
						SongCompletionEventData completionEventData{};
						completionEventData.guild = Guilds::getCachedGuildAsync({ .guildId = this->voiceConnectInitData.guildId }).get();
						completionEventData.guildMember = GuildMembers::getCachedGuildMemberAsync({ .guildMemberId = this->currentGuildMemberId,.guildId = this->voiceConnectInitData.guildId }).get();
						completionEventData.wasItAFail = false;
						getSongAPIMap()[this->voiceConnectInitData.guildId]->onSongCompletionEvent(completionEventData);
						break;
					}
				}
				this->areWePlaying.store(false, std::memory_order_seq_cst);
				this->sendSpeakingMessage(false);
				this->clearAudioData();

				if (this->areWeStopping.load(std::memory_order_seq_cst)) {
					this->stopSetEvent.wait(5000);
					this->stopSetEvent.reset();
					this->areWeStopping.store(false, std::memory_order_seq_cst);
				}
				
				if (cancelHandle.promise().isItStopped()) {
					co_return;
				}
			}
		}
		catch (...) {
			reportException("VoiceConnection::run()");
		}
		co_return;
	};

	VoiceConnection::~VoiceConnection() {
		this->theTask->cancel();
		this->theTask.reset(nullptr);
	}

}