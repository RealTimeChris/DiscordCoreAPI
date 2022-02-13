// VoiceConnection.cpp - Source file for the voice connection class DiscordCoreAPI_Dll.
// Jul 15, 2021
// Chris M.
// https://github.com/RealTimeChris

#include "VoiceConnection.hpp"
#include "WebSocketEntities.hpp"
#include "DiscordCoreClient.hpp"
#include "CoRoutine.hpp"
#include "JSONIfier.hpp"

namespace DiscordCoreAPI {

	AudioEncrypter::AudioEncrypter() {
		if (sodium_init() == -1) {
			std::cout << "LibSodium failed to initialize!" << std::endl << std::endl;
		}
	};

	std::vector<uint8_t> AudioEncrypter::encryptSingleAudioFrame(EncodedFrameData bufferToSend, int32_t audioSSRC, std::string keys) {
		std::cout << "KEYS SIZE: " << keys.size() << std::endl;
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
		return audioDataPacket;
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
			return this->areWePlaying;
		}
	}

	bool VoiceConnection::stop() {
		if (this->areWePlaying) {
			this->areWePlaying = false;
			this->areWeStopping = true;
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
			if (!this->areWePaused) {
				sendSpeakingMessage(false);
				this->areWePaused = true;
			}
			else {
				sendSpeakingMessage(true);
				this->pauseEvent.set();
				this->areWePaused = false;
			}
		}
	}

	void VoiceConnection::reconnect() {
		this->connect(this->voiceConnectInitData);
		this->play();
	}

	void VoiceConnection::connect(DiscordCoreInternal::VoiceConnectInitData voiceConnectInitDataNew) {
		try {
			this->areWeStopping = false;
			this->doWeQuit = false;
			this->voiceConnectInitData = voiceConnectInitDataNew;
			if (!this->baseSocketAgent->voiceConnectionDataBufferMap.contains(this->voiceConnectInitData.guildId)) {
				this->baseSocketAgent->voiceConnectionDataBufferMap.insert(std::make_pair(this->voiceConnectInitData.guildId, this->voiceConnectionBuffer.get()));
			}
			if (this->voiceSocketAgent != nullptr) {
				this->voiceSocketAgent->doWeQuit = true;
				this->voiceSocketAgent->theTask.get();
			}
			this->voiceSocketAgent.reset(nullptr);
			if (this->baseSocketAgent->areWeReadyToConnectEvent.wait(10000)) {
				return;
			}
			this->voiceSocketAgent = std::make_unique<DiscordCoreInternal::VoiceSocketAgent>(this->voiceConnectInitData, this->baseSocketAgent);
			this->doWeReconnect = &this->voiceSocketAgent->doWeReconnect;
			this->voiceConnectionData = &this->voiceSocketAgent->voiceConnectionData;
			this->areWeConnectedBool = true;
			if (this->theTask == nullptr) {
				this->theTask = std::make_unique<CoRoutine<void>>(this->run());
			}
		}
		catch (...) {
			DiscordCoreAPI::reportException("VoiceConnection::connect()");
		}
	}

	void VoiceConnection::disconnect() {
		this->sendSpeakingMessage(false);
		this->areWeConnectedBool = false;
		this->areWePlaying = false;
		this->areWeStopping = true;
		this->doWeQuit = true;
		if (this->voiceSocketAgent != nullptr) {
			this->voiceSocketAgent->doWeQuit = true;
			this->voiceSocketAgent->theTask.get();
			this->voiceSocketAgent.reset(nullptr);
		}
		this->theTask->cancel();
		this->theTask->get();
		this->theTask.reset(nullptr);
		auto thePtr = getSongAPIMap()->at(this->voiceConnectInitData.guildId).get();
		if (thePtr != nullptr) {
			thePtr->onSongCompletion(getSongAPIMap()->at(this->voiceConnectInitData.guildId)->eventDelegateToken, this->voiceConnectInitData.guildId);
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

	void VoiceConnection::sendSingleAudioFrame(std::vector<uint8_t>& audioDataPacketNew) {
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
			for (uint8_t x = 0; x < 3; x += 1) {
				if (x == 0) {
					newFrame.data.push_back(0xF8);
				}
				else if (x == 1) {
					newFrame.data.push_back(0xFF);
				}
				else {
					newFrame.data.push_back(0xFE);
				}
			}
			newFrame.sampleCount = 3;
			frameData.push_back(newFrame);
		}
		for (auto& value : frameData) {
			auto newerFrame = this->audioEncrypter.encryptSingleAudioFrame(value, this->voiceConnectionData->audioSSRC, this->voiceConnectionData->secretKey);
			this->sendSingleAudioFrame(newerFrame);
		}
	}

	void VoiceConnection::sendSpeakingMessage(bool isSpeaking) {
		if (!this->doWeQuit && this->voiceSocketAgent != nullptr) {
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
			while (!this->doWeQuit && !cancelHandle.promise().isItStopped()) {
				if (!this->didWeJustConnect) {
					this->audioBuffer.clearContents();
					this->clearAudioData();
					this->didWeJustConnect = false;
				}
				if (this->playSetEvent.wait(10000)) {
					if (this->doWeQuit || cancelHandle.promise().isItStopped()) {
						break;
					}
					continue;
				};
				this->playSetEvent.reset();
			start:
				if (this->doWeQuit || cancelHandle.promise().isItStopped()) {
					this->areWePlaying = false;
					break;
				}
				this->audioData.type = AudioFrameType::Unset;
				this->audioData.encodedFrameData.data.clear();
				this->audioData.rawFrameData.data.clear();
				this->audioBuffer.tryReceive(this->audioData);
				if ((this->audioData.encodedFrameData.sampleCount == 0 && this->audioData.rawFrameData.sampleCount == 0) || this->audioData.type == AudioFrameType::Unset) {
					this->areWePlaying = false;
					goto start;
				}
				else if (this->audioData.type == AudioFrameType::Cancel) {
					this->areWePlaying = false;
					continue;
				}
				this->areWePlaying = true;
				this->sendSpeakingMessage(true);
				int64_t startingValue{ static_cast<int64_t>(std::chrono::duration_cast<std::chrono::nanoseconds>(std::chrono::system_clock::now().time_since_epoch()).count()) };
				int64_t intervalCount{ 20000000 };
				int32_t frameCounter{ 0 };
				int64_t timeCounter{ 0 };
				int64_t totalTime{ 0 };
				while ((this->audioData.rawFrameData.sampleCount != 0 || this->audioData.encodedFrameData.sampleCount != 0) && !this->areWeStopping) {
					this->areWePlaying = true;
					if (this->doWeReconnect->wait(0)) {
						this->areWeConnectedBool = false;
						this->sendSilence();
						this->sendSpeakingMessage(false);
						this->reconnect();
						this->sendSpeakingMessage(true);
						this->areWePlaying = true;
						this->doWeReconnect->set();
					}
					if (this->areWeStopping) {
						this->areWePlaying = false;
						frameCounter = 0;
						break;
					}
					if (this->areWePaused) {
						this->pauseEvent.wait(240000);
						this->pauseEvent.reset();
						this->areWePaused = false;
					}
					if (this->doWeQuit || cancelHandle.promise().isItStopped()) {
						this->clearAudioData();
						this->areWePlaying = false;
						frameCounter = 0;
						break;
					}
					frameCounter += 1;
					this->audioBuffer.tryReceive(this->audioData);
					nanoSleep(100000);
					if (this->doWeQuit || cancelHandle.promise().isItStopped()) {
						break;
					}
					if (this->audioData.type != AudioFrameType::Cancel && this->audioData.type != AudioFrameType::Unset) {
						std::vector<uint8_t> newFrame{};
						if (this->audioData.type == AudioFrameType::RawPCM) {
							auto newFrames = this->encoder->encodeSingleAudioFrame(this->audioData.rawFrameData);
							std::cout << "FRAME COUNT: " << newFrames.data.size() << std::endl;
							newFrame = this->audioEncrypter.encryptSingleAudioFrame(newFrames, this->voiceConnectionData->audioSSRC, this->voiceConnectionData->secretKey);
						}
						else {
							std::cout << "ENCODED FRAME COUNT: " << this->audioData.encodedFrameData.data.size() << std::endl;
							newFrame = this->audioEncrypter.encryptSingleAudioFrame(this->audioData.encodedFrameData, this->voiceConnectionData->audioSSRC, this->voiceConnectionData->secretKey);
						}
						nanoSleep(18000000);
						if (this->doWeQuit || cancelHandle.promise().isItStopped()) {
							break;
						}
						timeCounter = static_cast<int64_t>(std::chrono::duration_cast<std::chrono::nanoseconds>(std::chrono::system_clock::now().time_since_epoch()).count() - startingValue);
						int64_t  waitTime = intervalCount - timeCounter;
						spinLock(waitTime);
						startingValue = static_cast<int64_t>(std::chrono::duration_cast<std::chrono::nanoseconds>(std::chrono::system_clock::now().time_since_epoch()).count());
						this->sendSingleAudioFrame(newFrame);
						totalTime += static_cast<int64_t>(std::chrono::duration_cast<std::chrono::nanoseconds>(std::chrono::system_clock::now().time_since_epoch()).count() - startingValue);
						intervalCount = 20000000 - (totalTime / frameCounter);
						this->audioData.type = AudioFrameType::Unset;
						this->audioData.encodedFrameData.data.clear();
						this->audioData.rawFrameData.data.clear();
					}
					else if ((this->audioData.type == AudioFrameType::Cancel || (this->audioData.encodedFrameData.data.size() == 0 && this->audioData.rawFrameData.data.size() == 0)) && !this->areWeStopping) {
						SongCompletionEventData completionEventData{};
						completionEventData.voiceConnection = this;
						completionEventData.wasItAFail = false;
						getSongAPIMap()->at(this->voiceConnectInitData.guildId)->onSongCompletionEvent(completionEventData);
						this->areWePlaying = false;
						frameCounter = 0;
						break;
					}
				}

				this->areWePlaying = false;

				if (this->areWeStopping) {
					this->stopSetEvent.wait(5000);
					if (this->doWeQuit || cancelHandle.promise().isItStopped()) {
						this->clearAudioData();
						this->areWePlaying = false;
						frameCounter = 0;
						break;
					}
					this->clearAudioData();
					this->stopSetEvent.reset();
					this->areWeStopping = false;
				}
				this->sendSpeakingMessage(false);
				if (this->doWeQuit || cancelHandle.promise().isItStopped()) {
					this->clearAudioData();
					this->areWePlaying = false;
					frameCounter = 0;
					break;
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
		this->theTask->get();
		auto thePtr = getSongAPIMap()->at(this->voiceConnectInitData.guildId).get();
		if (thePtr != nullptr) {
			thePtr->onSongCompletion(thePtr->eventDelegateToken, this->voiceConnectInitData.guildId);
		}
		this->baseSocketAgent->voiceConnectionDataBufferMap.erase(this->voiceConnectInitData.guildId);
	}

}