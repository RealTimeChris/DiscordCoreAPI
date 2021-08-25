// VoiceConnectionStuff.hpp - Header for all of the "voice connection" stuff.
// Jul 15, 2021
// Chris M.
// https://github.com/RealTimeChris

#pragma once

#ifndef _VOICE_CONNECTION_STUFF_
#define _VOICE_CONNECTION_STUFF_

#include "../pch.h"
#include "WebSocketStuff.hpp"
#include "DecodingStuff.hpp"
#include "EncodingStuff.hpp"
#include "GuildStuff01.hpp"
#include "DiscordCoreClientBase.hpp"

namespace DiscordCoreAPI {

	class VoiceConnection : public agent {
	public:
		VoiceConnection(shared_ptr<DiscordCoreInternal::ThreadContext> threadContextNew, DiscordCoreInternal::VoiceConnectionData voiceConnectionDataNew, shared_ptr<unbounded_buffer<AudioFrameData*>> bufferMessageBlockNew)
			: agent(*threadContextNew->scheduler) {
			if (voiceConnectionDataNew.channelId != "") {
				if (sodium_init() == -1) {
					cout << "LibSodium failed to initialize!" << endl << endl;
				}
				int error;
				this->encoder = opus_encoder_create(48000, 2, OPUS_APPLICATION_AUDIO, &error);
				if (error != OPUS_OK){
					cout << "Failed to create Opus encoder!" << endl << endl;
				}
				this->voiceConnectionData = voiceConnectionDataNew;
				this->threadContext = threadContextNew;
				this->bufferMessageBlock = bufferMessageBlockNew;
				this->voicechannelWebSocketAgent = make_shared<DiscordCoreInternal::VoiceChannelWebSocketAgent>(DiscordCoreInternal::ThreadManager::getThreadContext(DiscordCoreInternal::ThreadType::Music).get(), this->voiceConnectionData, &this->readyBuffer);
				send(this->voicechannelWebSocketAgent->voiceConnectionDataBuffer, this->voiceConnectionData);
				this->voicechannelWebSocketAgent->start();
				receive(this->readyBuffer);
				this->voiceConnectionData = this->voicechannelWebSocketAgent->voiceConnectionData;
				this->start();
			}
			else {
				throw exception("Sorry, but you need to select a proper voice channel to connect to!");
			}
		}

		string getChannelId() {
			if (this != nullptr && this->voiceConnectionData.channelId != "") {
				return this->voiceConnectionData.channelId;
			}
			else {
				return string();
			}
		}

		event_token onSongCompletion(delegate<> const& handler) {
			if (!DiscordCoreClientBase::voiceConnectionMap->at(this->voiceConnectionData.guildId)->amIInstantiated) {
				auto voiceConnection = DiscordCoreClientBase::voiceConnectionMap->at(this->voiceConnectionData.guildId);
				voiceConnection->amIInstantiated = true;
				DiscordCoreClientBase::voiceConnectionMap->insert_or_assign(this->voiceConnectionData.guildId, voiceConnection);
				return onSongCompletionEvent.add(handler);
			}
			else {
				return event_token();
			}
		}

		void onSongCompletion(event_token const& token) {
			onSongCompletionEvent.remove(token);
		}

		bool areWeConnected() {
			if (this == nullptr) {
				return false;
			}
			else {
				return true;
			}
		}

		void seek(int percentageIntoSong) {
			if (0 <= percentageIntoSong && 100 >= percentageIntoSong) {
				send(this->seekBuffer, percentageIntoSong);
			}
			else {
				throw exception("Please, enter a valid value!");
			}
		}

		void skipCurrentSong() {
			if (this != nullptr) {
				this->areWePlaying = false;
				this->areWeWaitingForAudioData = true;
				this->doWeWait = true;
				this->areWeStopping = true;
				bool receiveTrigger = receive(this->readyBuffer);
				this->clearAudioData();
			}
		}

		void stopPlaying() {
			if (this != nullptr) {
				this->areWePlaying = false;
				this->areWeWaitingForAudioData = true;
				this->doWeWait = true;
				this->areWeStopping = true;
				bool receiveTrigger = receive(this->readyBuffer);
				this->clearAudioData();
			}
		}

		bool areWeCurrentlyPlaying() {
			if (this == nullptr) {
				return false;
			}
			else {
				return this->areWePlaying;
			}
		}

		void play() {
			if (this != nullptr) {
				this->areWePlaying = false;
				this->areWeWaitingForAudioData = true;
				send(this->playPauseBuffer, true);
			}
		}

		void pauseToggle() {
			if (this != nullptr) {
				if (this->doWeWait) {
					send(this->playPauseBuffer, true);
				}
				else {
					this->areWePlaying = false;
					this->doWeWait = true;
					sendSpeakingMessage(false);
				}
			}
		}

		~VoiceConnection() {
			this->threadContext->releaseGroup();
			opus_encoder_destroy(this->encoder);
			this->terminate();
		}

	protected:
		friend class Guild;
		bool doWeQuit = false;
		bool doWeWait = true;
		bool areWePlaying = false;
		bool areWeWaitingForAudioData = true;
		bool areWeStopping = false;
		bool amIInstantiated = false;
		unsigned int timestamp = 0;
		unsigned short sequenceIndex = 0;
		DiscordCoreInternal::VoiceConnectionData voiceConnectionData;
		shared_ptr<DiscordCoreInternal::ThreadContext> threadContext{ nullptr };
		shared_ptr<DiscordCoreInternal::VoiceChannelWebSocketAgent> voicechannelWebSocketAgent{ nullptr };
		shared_ptr<unbounded_buffer<AudioFrameData*>> bufferMessageBlock{ nullptr };
		unbounded_buffer<bool> readyBuffer{ nullptr };
		unbounded_buffer<int> seekBuffer{ nullptr };
		unbounded_buffer<bool> playPauseBuffer{ nullptr };
		OpusEncoder* encoder{ nullptr };
		const int maxBufferSize = 1276;
		winrt::event<delegate<>> onSongCompletionEvent;
		AudioFrameData* audioData{ nullptr };

		event_token onSongCompletionToBeWrapped(delegate<> const handler) {
			return onSongCompletionEvent.add(handler);
		}

		void clearAudioData() {
			if (this->audioData != nullptr) {
				this->audioData = nullptr;
			}
		}

		EncodedFrameData encodeSingleAudioFrame(RawFrameData inputFrame) {
			uint8_t* oldBuffer = new uint8_t[inputFrame.data.size()];

			for (int x = 0; x < inputFrame.data.size(); x += 1) {
				oldBuffer[x] = inputFrame.data[x];
			}

			uint8_t* newBuffer = new uint8_t[this->maxBufferSize];

			int count = opus_encode_float(this->encoder, (float*)oldBuffer, inputFrame.sampleCount, newBuffer, this->maxBufferSize);
			vector<uint8_t> newVector{};
			for (int x = 0; x < count; x += 1) {
				newVector.push_back(newBuffer[x]);
			}
			EncodedFrameData encodedFrame;
			encodedFrame.data = newVector;
			encodedFrame.sampleCount = inputFrame.sampleCount;
			delete oldBuffer;
			oldBuffer = nullptr;
			delete newBuffer;
			newBuffer = nullptr;
			return encodedFrame;
		}
		
		void sendSingleAudioFrame(EncodedFrameData bufferToSend) {
			constexpr int headerSize = 12;
			constexpr int nonceSize = crypto_secretbox_NONCEBYTES;
			uint8_t header01[2]{ 0x80 , 0x78 };
			uint16_t header02[1]{ this->sequenceIndex };
			uint32_t header03[2]{ (uint32_t)this->timestamp, (uint32_t)this->voiceConnectionData.audioSSRC };

			uint8_t headerFinal[headerSize];
			headerFinal[0] = (uint8_t)header01[0];
			headerFinal[1] = (uint8_t)header01[1];
			headerFinal[2] = (uint8_t)(header02[0] >> (8 * 1));
			headerFinal[3] = (uint8_t)(header02[0] >> (8 * 0));
			headerFinal[4] = (uint8_t)(header03[0] >> (8 * 3));
			headerFinal[5] = (uint8_t)(header03[0] >> (8 * 2));
			headerFinal[6] = (uint8_t)(header03[0] >> (8 * 1));
			headerFinal[7] = (uint8_t)(header03[0] >> (8 * 0));
			headerFinal[8] = (uint8_t)(header03[1] >> (8 * 3));
			headerFinal[9] = (uint8_t)(header03[1] >> (8 * 2));
			headerFinal[10] = (uint8_t)(header03[1] >> (8 * 1));
			headerFinal[11] = (uint8_t)(header03[1] >> (8 * 0));

			uint8_t nonceForLibSodium[nonceSize];
			memcpy(nonceForLibSodium, headerFinal, headerSize);
			for (unsigned int x = headerSize; x < crypto_secretbox_NONCEBYTES; x += 1) {
				nonceForLibSodium[x] = 0;
			}

			size_t numOfBytes = headerSize + bufferToSend.data.size() + crypto_secretbox_MACBYTES;
			uint8_t* audioDataPacket = new uint8_t[numOfBytes];
			memcpy(audioDataPacket, headerFinal, headerSize);
			unsigned char* encryptionKeys = new unsigned char[crypto_secretbox_KEYBYTES];
			unsigned char* bufferToSendNew = new unsigned char[bufferToSend.data.size()];
			for (unsigned int x = 0; x < bufferToSend.data.size(); x += 1) {
				bufferToSendNew[x] = bufferToSend.data[x];
			}
			for (unsigned int x = 0; x < this->voiceConnectionData.keys.size(); x += 1) {
				encryptionKeys[x] = (unsigned char)this->voiceConnectionData.keys[x];
			}
			if (crypto_secretbox_easy(audioDataPacket + headerSize,
				bufferToSendNew, bufferToSend.data.size(), nonceForLibSodium, encryptionKeys) != 0){
				throw exception("ENCRYPTION FAILED!");
			};
			vector<uint8_t> audioDataPacketNew;
			for (unsigned int x = 0; x < numOfBytes; x += 1) {
				audioDataPacketNew.push_back(audioDataPacket[x]);
			}
			this->voicechannelWebSocketAgent->sendVoiceData(audioDataPacketNew);
			this->sequenceIndex += 1;
			this->timestamp += (int)bufferToSend.sampleCount;
		}

		void sendSpeakingMessage(bool isSpeaking) {
			this->voiceConnectionData.audioSSRC = this->voicechannelWebSocketAgent->voiceConnectionData.audioSSRC;
			string newString = DiscordCoreInternal::getIsSpeakingPayload(isSpeaking, this->voiceConnectionData.audioSSRC, 0);
			this->voicechannelWebSocketAgent->sendMessage(newString);
		}

		void run() {
			while (!this->doWeQuit) {
				
				if (this->doWeWait) {
					receive(this->playPauseBuffer);
				}

				if (this->areWeWaitingForAudioData) {
					this->audioData = receive(*this->bufferMessageBlock);
					this->areWeWaitingForAudioData = false;
				}
				this->sendSpeakingMessage(true);
				this->areWePlaying = true;
				int frameCounter = 0;
				while (this->areWePlaying) {
					int timeCounter = 0;
					int startingValue = (int)std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
					const int intervalCount = 20000;
					if (this->audioData != nullptr) {
						if (this->audioData->type == AudioFrameType::Encoded) {
							size_t encodedFrameDataSize = this->audioData->encodedFrameData.size();
							for (int x = 0; x < this->audioData->encodedFrameData.size(); x += 1) {
								timeCounter = 0;
								while (timeCounter <= intervalCount) {
									timeCounter = (int)std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::system_clock::now().time_since_epoch()).count() - startingValue;
								}
								startingValue = (int)std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
								this->sendSingleAudioFrame(this->audioData->encodedFrameData[x]);
								int newValue;
								if (try_receive(this->seekBuffer, newValue)) {
									x = (int)trunc(((float)newValue / (float)100) * (float)this->audioData->encodedFrameData.size());
									frameCounter = x;
								}
								if (!this->areWePlaying) {
									break;
								};
								frameCounter += 1;
								if (frameCounter >= encodedFrameDataSize - 1 || this->audioData->encodedFrameData.size() == 0) {
									this->clearAudioData();
									this->areWePlaying = false;
									this->areWeWaitingForAudioData = true;
									this->onSongCompletionEvent();
									frameCounter = 0;
									break;
								}
							}
						}
						else if (this->audioData->type == AudioFrameType::RawPCM) {
							size_t rawFrameDataSize = this->audioData->rawFrameData.size();
							for (int x = 0; x < this->audioData->rawFrameData.size(); x += 1) {
								timeCounter = 0;
								while (timeCounter <= intervalCount) {
									timeCounter = (int)std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::system_clock::now().time_since_epoch()).count() - startingValue;
								}
								startingValue = (int)std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
								auto newVector = encodeSingleAudioFrame(this->audioData->rawFrameData[x]);
								this->sendSingleAudioFrame(newVector);
								int newValue;
								if (try_receive(this->seekBuffer, newValue)) {
									x = (int)trunc(((float)newValue / (float)100) * (float)this->audioData->encodedFrameData.size());
									frameCounter = x;
								}
								if (!this->areWePlaying) {
									break;
								};
								frameCounter += 1;
								if (frameCounter >= rawFrameDataSize - 1 || this->audioData->rawFrameData.size() == 0) {
									this->clearAudioData();
									this->areWePlaying = false;
									this->areWeWaitingForAudioData = true;
									this->onSongCompletionEvent();
									frameCounter = 0;
									break;
								}
							}
						}
						if (this->audioData == nullptr || (this->audioData->encodedFrameData.size() == 0 && this->audioData->rawFrameData.size() == 0)) {
							this->doWeWait = false;
							this->areWePlaying = false;
							this->areWeWaitingForAudioData = true;
							break;
						}
					}
				}
				if (this->areWeStopping) {
					send(this->readyBuffer, true);
					this->areWeStopping = false;
				}
				this->sendSpeakingMessage(false);
			}
			this->done();
		}

		void terminate() {
			DiscordCoreClientBase::voiceConnectionMap->erase(this->voiceConnectionData.guildId);
			this->areWePlaying = false;
			this->doWeQuit = true;
		}

	};

}
#endif