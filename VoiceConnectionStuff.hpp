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
#include "GuildStuff.hpp"
#include "DiscordCoreClientBase.hpp"

namespace DiscordCoreAPI {

	class VoiceConnection : public agent {
	public:

		VoiceConnection(shared_ptr<DiscordCoreInternal::ThreadContext> threadContextNew, DiscordCoreInternal::VoiceConnectionData voiceConnectionDataNew, shared_ptr<unbounded_buffer<AudioFrameData*>> bufferMessageBlockNew, shared_ptr<DiscordCoreClientBase> discordCoreClientBaseNew)
			: agent(*threadContextNew->scheduler) {
			if (voiceConnectionDataNew.channelId != "") {
				if (sodium_init() == -1) {
					cout << "LibSodium failed to initialize!" << endl << endl;
				}
				int error;
				this->encoder = opus_encoder_create(48000, 2, OPUS_APPLICATION_AUDIO, &error);
				if (error != OPUS_OK) {
					cout << "Failed to create Opus encoder!" << endl << endl;
				}
				this->discordCoreClientBase = discordCoreClientBaseNew;
				this->threadContext = threadContextNew;
				this->voiceConnectionData = voiceConnectionDataNew;
				this->audioDataBuffer = bufferMessageBlockNew;
				this->audioDataBufferOutput = make_shared<unbounded_buffer<AudioFrameData*>>();
				this->voicechannelWebSocketAgent = make_shared<DiscordCoreInternal::VoiceChannelWebSocketAgent>(DiscordCoreInternal::ThreadManager::getThreadContext(DiscordCoreInternal::ThreadType::Music).get(), this->voiceConnectionData, &this->readyBuffer);
				send(this->voicechannelWebSocketAgent->voiceConnectionDataBuffer, this->voiceConnectionData);
				this->voicechannelWebSocketAgent->start();
				receive(this->readyBuffer);
				this->voiceConnectionData = this->voicechannelWebSocketAgent->voiceConnectionData;
				this->areWeConnectedBool = true;
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
			shared_ptr<VoiceConnection> sharedPtr = DiscordCoreClientBase::voiceConnectionMap->at(this->voiceConnectionData.guildId);
			if (!sharedPtr->areWeInstantiated) {
				sharedPtr->areWeInstantiated = true;
				this->start();
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
				return  this->areWeConnectedBool;
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

		void seek(int percentageIntoSong) {
			if (0 <= percentageIntoSong && 100 >= percentageIntoSong) {
				send(this->seekBuffer, percentageIntoSong);
			}
			else {
				throw exception("Please, enter a valid value!");
			}
		}

		void stop() {
			if (this != nullptr) {
				this->areWePlaying = false;
				this->areWeWaitingForAudioData = true;
				this->areWeStopping = true;
				receive(this->stopBuffer);
			}
		}

		void play() {
			if (this != nullptr) {
				AudioFrameData* dataPackage;
				if (try_receive(*this->audioDataBuffer, dataPackage)) {
					if (dataPackage->type == AudioFrameType::Encoded) {
						if (dataPackage->encodedFrameData.size() > 0) {
							send(*this->audioDataBufferOutput, dataPackage);
						}
					}
					else {
						if (dataPackage->rawFrameData.size() > 0) {
							send(*this->audioDataBufferOutput, dataPackage);
						}
					}
				};
				if (!this->areWeWaiting) {
					this->areWeWaiting = true;
					wait(this);
				}
			}
		}

		void pauseToggle() {
			if (this != nullptr) {
				if (!this->areWePaused) {
					sendSpeakingMessage(false);
					this->areWePaused = true;
				}
				else {
					sendSpeakingMessage(true);
					send(this->pauseBuffer, true);
				}
			}
		}

		void disconnect() {
			if (DiscordCoreClientBase::voiceConnectionMap->contains(this->voiceConnectionData.guildId)) {
				shared_ptr<VoiceConnection> voiceConnection = DiscordCoreClientBase::voiceConnectionMap->at(this->voiceConnectionData.guildId);
				if (!voiceConnection->hasTerminateRun) {
					if (voiceConnection->areWePlaying) {
						voiceConnection->areWePlaying = false;
						voiceConnection->areWeStopping = true;
						cout << "HERE 0000000" << endl;
						receive(voiceConnection->stopBuffer);
					}
					cout << "HERE 1111111" << endl;
					if (voiceConnection->areWeWaitingForAudioData) {
						cout << "HERE 2222222" << endl;
						voiceConnection->clearAudioData();
						AudioFrameData* frameData;
						while (try_receive(*voiceConnection->audioDataBuffer, frameData)) {};
						AudioFrameData audioFrameData{};
						send(*voiceConnection->audioDataBuffer, &audioFrameData);
					}
					voiceConnection->doWeQuit = true;
					cout << "HERE 3333333" << endl;
					DiscordCoreClientBase::currentUser->updateVoiceStatus({ .guildId = voiceConnection->voiceConnectionData.guildId,.channelId = "", .selfMute = false,.selfDeaf = false });
					cout << "HERE 3333333" << endl;
					cout << "HERE 4444444" << endl;
					if (voiceConnection->threadContext->schedulerGroup != nullptr) {
						cout << "HERE 5555555" << endl;
						voiceConnection->threadContext->releaseGroup();
					}
					cout << "HERE 6666666" << endl;
					if (voiceConnection->encoder != nullptr) {
						opus_encoder_destroy(voiceConnection->encoder);
						cout << "HERE 7777777" << endl;
						voiceConnection->encoder = nullptr;
					}
					voiceConnection->hasTerminateRun = true;
					DiscordCoreClientBase::audioBuffersMap.erase(voiceConnection->voiceConnectionData.guildId);
					DiscordCoreClientBase::voiceConnectionMap->erase(voiceConnection->voiceConnectionData.guildId);
					cout << "HERE 888888" << endl;
				}
			}
		}

		~VoiceConnection() {
			this->disconnect();
		}

	protected:
		friend class Guild;
		friend class DiscordCoreClientBase;
		shared_ptr<DiscordCoreInternal::VoiceChannelWebSocketAgent> voicechannelWebSocketAgent{ nullptr };
		shared_ptr<unbounded_buffer<AudioFrameData*>> audioDataBufferOutput{ nullptr };
		shared_ptr<unbounded_buffer<AudioFrameData*>> audioDataBuffer{ nullptr };
		shared_ptr<DiscordCoreInternal::ThreadContext> threadContext{ nullptr };
		shared_ptr<DiscordCoreClientBase> discordCoreClientBase{ nullptr };
		DiscordCoreInternal::VoiceConnectionData voiceConnectionData{};
		winrt::event<delegate<>> onSongCompletionEvent;
		unbounded_buffer<bool> readyBuffer{ nullptr };
		unbounded_buffer<bool> pauseBuffer{ nullptr };
		unbounded_buffer<bool> stopBuffer{ nullptr };
		unbounded_buffer<int> seekBuffer{ nullptr };
		bool areWeWaitingForAudioData{ true };
		AudioFrameData* audioData{ nullptr };
		unsigned short sequenceIndex{ 0 };
		bool areWeConnectedBool{ false };
		OpusEncoder* encoder{ nullptr };
		const int maxBufferSize{ 1276 };
		bool areWeInstantiated{ false };
		bool hasTerminateRun{ false };
		bool areWeStopping{ false };
		unsigned int timestamp{ 0 };
		bool areWeWaiting{ false };
		bool areWePlaying{ false };
		bool areWePaused{ false };
		bool doWeQuit{ false };
		
		event_token onSongCompletionToBeWrapped(delegate<> const handler) {
			return onSongCompletionEvent.add(handler);
		}

		void clearAudioData() {
			if (this->audioData != nullptr) {
				this->audioData = nullptr;
			}
		}

		EncodedFrameData encodeSingleAudioFrame(RawFrameData inputFrame) {
			uint8_t* oldBuffer{ new uint8_t[inputFrame.data.size()] };

			for (int x = 0; x < inputFrame.data.size(); x += 1) {
				oldBuffer[x] = inputFrame.data[x];
			}

			uint8_t* newBuffer{ new uint8_t[this->maxBufferSize] };

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
			constexpr int headerSize{ 12 };
			constexpr int nonceSize{ crypto_secretbox_NONCEBYTES };
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

			size_t numOfBytes{ headerSize + bufferToSend.data.size() + crypto_secretbox_MACBYTES };
			uint8_t* audioDataPacket{ new uint8_t[numOfBytes] };
			memcpy(audioDataPacket, headerFinal, headerSize);
			unsigned char* encryptionKeys{ new unsigned char[crypto_secretbox_KEYBYTES] };
			unsigned char* bufferToSendNew{ new unsigned char[bufferToSend.data.size()] };
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
			if (this->voicechannelWebSocketAgent->voiceSocket != nullptr) {
				this->voicechannelWebSocketAgent->sendVoiceData(audioDataPacketNew);
			}
			this->sequenceIndex += 1;
			this->timestamp += (int)bufferToSend.sampleCount;
		}

		void sendSpeakingMessage(bool isSpeaking) {
			this->voiceConnectionData.audioSSRC = this->voicechannelWebSocketAgent->voiceConnectionData.audioSSRC;
			string newString = DiscordCoreInternal::getIsSpeakingPayload(isSpeaking, this->voiceConnectionData.audioSSRC, 0);
			if (this->voicechannelWebSocketAgent->webSocket != nullptr) {
				this->voicechannelWebSocketAgent->sendMessage(newString);
			}
		}

		void run() {
			cout << "WAS HAT 00000" << endl;
			while (!this->doWeQuit) {
				cout << "WAS HAT 111111" << endl;

				if (this->areWeWaitingForAudioData) {
					cout << "WAS HAT 222222" << endl;
					this->audioData = receive(*this->audioDataBufferOutput);
					cout << "WAS HAT 3333333" << endl;
					this->areWeWaitingForAudioData = false;
				}
				cout << "WAS HAT 4444444" << endl;
				this->sendSpeakingMessage(true);

				cout << "WAS HAT 5555555" << endl;
				const int intervalCount = 19800;
				int frameCounter = 0;
				int timeCounter = 0;
				int startingValue = (int)std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
				if (this->audioData != nullptr) {
					if (this->audioData->type == AudioFrameType::Encoded&&this->audioData->encodedFrameData.size()>0) {
						this->areWePlaying = true;
						cout << "WAS HAT 6666666" << endl;
						if (this->areWePlaying) {
							cout << "WAS HAT 77777777" << endl;
							cout << "WAS HAT 88888888" << endl;
							size_t encodedFrameDataSize = this->audioData->encodedFrameData.size();
							for (int x = 0; x < this->audioData->encodedFrameData.size(); x += 1) {
								cout << "WAS HAT 9999999" << endl;
								int newValue;
								if (try_receive(this->seekBuffer, newValue)) {
									x = (int)trunc(((float)newValue / (float)100) * (float)this->audioData->encodedFrameData.size());
									frameCounter = x;
								}
								if (!this->areWePlaying) {
									this->clearAudioData();
									this->areWeWaitingForAudioData = true;
									this->onSongCompletionEvent();
									frameCounter = 0;
									break;
								};
								if (this->areWePaused) {
									receive(this->pauseBuffer);
									this->areWePaused = false;
								}
								frameCounter += 1;
								if (frameCounter >= encodedFrameDataSize - 1 || this->audioData->encodedFrameData.size() == 0) {
									this->clearAudioData();
									this->areWePlaying = false;
									this->areWeWaitingForAudioData = true;
									this->onSongCompletionEvent();
									frameCounter = 0;
									break;
								}
								timeCounter = 0;
								while (timeCounter <= intervalCount) {
									timeCounter = (int)std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::system_clock::now().time_since_epoch()).count() - startingValue;
								}
								this->sendSingleAudioFrame(this->audioData->encodedFrameData[x]);
								startingValue = (int)std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
							}
						}
						else {
							this->areWeWaitingForAudioData = true;
						}
					}
					else if (this->audioData->type == AudioFrameType::RawPCM && this->audioData->rawFrameData.size() > 0) {
						this->areWePlaying = true;
						cout << "WAS HAT 6666666" << endl;
						if (this->areWePlaying) {
							cout << "WAS HAT 77777777" << endl;
							cout << "WAS HAT 88888888" << endl;
							size_t rawFrameDataSize = this->audioData->rawFrameData.size();
							for (int x = 0; x < this->audioData->rawFrameData.size(); x += 1) {
								cout << "WAS HAT 9999999" << endl;
								int newValue;
								if (try_receive(this->seekBuffer, newValue)) {
									x = (int)trunc(((float)newValue / (float)100) * (float)this->audioData->encodedFrameData.size());
									frameCounter = x;
								}
								if (!this->areWePlaying) {
									this->clearAudioData();
									this->areWeWaitingForAudioData = true;
									this->onSongCompletionEvent();
									frameCounter = 0;
									break;
								};
								if (this->areWePaused) {
									receive(this->pauseBuffer);
									this->areWePaused = false;
								}
								frameCounter += 1;
								if (frameCounter >= rawFrameDataSize - 1 || this->audioData->rawFrameData.size() == 0) {
									this->clearAudioData();
									this->areWePlaying = false;
									this->areWeWaitingForAudioData = true;
									this->onSongCompletionEvent();
									frameCounter = 0;
									break;
								}
								timeCounter = 0;
								while (timeCounter <= intervalCount) {
									timeCounter = (int)std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::system_clock::now().time_since_epoch()).count() - startingValue;
								}
								auto newVector = encodeSingleAudioFrame(this->audioData->rawFrameData[x]);
								this->sendSingleAudioFrame(newVector);
								startingValue = (int)std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
							}
						}
						else {
							this->areWeWaitingForAudioData = true;
						}
					}
					else {
						this->areWeWaitingForAudioData = true;
					}
				}
				else {
					this->areWeWaitingForAudioData = true;
				}
				if (this->areWeStopping) {
					this->clearAudioData();
					send(this->stopBuffer, true);
					this->areWeStopping = false;
				}
				this->sendSpeakingMessage(false);
			}
			this->done();
		
		}

	};

}
#endif