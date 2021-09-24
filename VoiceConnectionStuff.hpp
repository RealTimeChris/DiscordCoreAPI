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

namespace DiscordCoreAPI {
	
	class VoiceConnection : DiscordCoreInternal::ThreadContext, agent {
	public:

		friend class DiscordCoreClientBase;
		friend class DiscordCoreClient;
		friend class SoundCloudAPI;
		friend class YouTubeAPI;
		friend class SongAPI;
		friend class Guild;

		VoiceConnection(shared_ptr<DiscordCoreInternal::ThreadContext> threadContextNew, DiscordCoreInternal::VoiceConnectInitData voiceConnectInitDataNew, map<string, shared_ptr<unbounded_buffer<AudioFrameData>>>* sendAudioBufferMapNew, 
			shared_ptr<DiscordCoreInternal::WebSocketConnectionAgent> websocketAgentNew, string guildIdNew, map<string, shared_ptr<VoiceConnection>>* voiceConnectionMapNew) :
			ThreadContext(*DiscordCoreInternal::ThreadManager::getThreadContext(DiscordCoreInternal::ThreadType::Music).get()),
			agent(*threadContextNew->scheduler->scheduler)
		{
			if (voiceConnectInitDataNew.channelId != "") {
				if (sodium_init() == -1) {
					cout << "LibSodium failed to initialize!" << endl << endl;
				}
				int error;
				this->encoder = opus_encoder_create(48000, 2, OPUS_APPLICATION_AUDIO, &error);
				if (error != OPUS_OK) {
					cout << "Failed to create Opus encoder!" << endl << endl;
				}
				this->receiveAudioBufferMap = sendAudioBufferMapNew;
				VoiceConnection::voiceConnectionMap = voiceConnectionMapNew;
				this->guildId = guildIdNew;
				this->guildIdNew = guildIdNew;
				if (VoiceConnection::receiveAudioBufferMap->contains(guildIdNew)) {
					this->audioDataBuffer = VoiceConnection::receiveAudioBufferMap->at(guildIdNew);
				}
				else {
					this->audioDataBuffer = make_shared<unbounded_buffer<AudioFrameData>>();
				}
				this->voiceChannelWebSocketAgent = make_shared<DiscordCoreInternal::VoiceChannelWebSocketAgent>(this->connectionReadyEvent, voiceConnectInitDataNew, websocketAgentNew, &this->doWeReconnect);
				this->voiceChannelWebSocketAgent->start();
				this->websocketAgent = websocketAgentNew;
				if (this->connectionReadyEvent->wait(10000) != 0) {
					this->connectionReadyEvent->reset();
					return;
				}
				this->connectionReadyEvent->reset();
				this->onSongCompletionEvent = new winrt::event<delegate<SongCompletionEventData>>();
				this->voiceConnectInitData = this->voiceChannelWebSocketAgent->voiceConnectInitData;
				this->voiceConnectionData = this->voiceChannelWebSocketAgent->voiceConnectionData;
				this->receiveAudioBufferMap->insert_or_assign(this->voiceConnectInitData.guildId, this->audioDataBuffer);
				this->areWeConnectedBool = true;
			}
			else {
				throw exception("Sorry, but you need to select a proper voice channel to connect to!");
			}
		}

		string getChannelId() {
			if (this != nullptr && this->voiceConnectInitData.channelId != "") {
				return this->voiceConnectInitData.channelId;
			}
			else {
				return string();
			}
		}

		event_token onSongCompletion(delegate<SongCompletionEventData> const& handler) {
			if (!this->areWeInstantiated) {
				this->areWeInstantiated = true;
				this->start();
				return onSongCompletionEvent->add(handler);
			}
			else {
				return event_token();
			}
		}

		void onSongCompletion(event_token const& token) {
			onSongCompletionEvent->remove(token);
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

		bool stop() {
			if (this->areWePlaying) {
				this->areWeStopping = true;
				this->areWePlaying = false;
				if (this->stopWaitEvent->wait(1000) != 0) {
					this->stopWaitEvent->reset();
					return false;
				}
				this->clearAudioData();
				this->stopWaitEvent->reset();
				this->stopSetEvent->set();
				return true;
			}
			else {
				return false;
			}
		}

		bool skip() {
			if (this->areWePlaying){
				this->areWeStopping= true;
				this->areWePlaying = false;
				if (this->stopWaitEvent->wait(1000) != 0) {
					this->stopWaitEvent->reset();
					return false;
				}
				this->clearAudioData();
				this->stopWaitEvent->reset();
				this->stopSetEvent->set();
				return true;
			}
			else {
				return false;
			}
		}

		bool play() {
			if (this != nullptr) {
				if (this->playWaitEvent->wait(2000) != 0) {
					this->playWaitEvent->reset();
					return false;
				};
				this->playWaitEvent->reset();
				this->playSetEvent->set();
				if (!this->areWeWaiting) {
					this->areWeWaiting = true;
					wait(this);
					this->areWeWaiting = false;
				}
				return true;
			}
			return false;
		}

		void pauseToggle() {
			if (this != nullptr) {
				if (!this->areWePaused) {
					sendSpeakingMessage(false);
					this->areWePaused = true;
				}
				else {
					sendSpeakingMessage(true);
					this->pauseEvent->set();
					this->pauseEvent->reset();
				}
			}
		}

		static void reconnect(string guildId) {
			if (voiceConnectionMap->contains(guildId)) {
				auto voiceConnectionData = VoiceConnection::voiceConnectionMap->at(guildId)->voiceConnectInitData;
				auto webSocketAgent = VoiceConnection::voiceConnectionMap->at(guildId)->websocketAgent;
				auto audioBuffersMap = VoiceConnection::voiceConnectionMap->at(guildId)->receiveAudioBufferMap;
				auto onSongCompletionEvent = VoiceConnection::voiceConnectionMap->at(guildId)->onSongCompletionEvent;
				voiceConnectionMap->erase(guildId);
				voiceConnectionMap->insert_or_assign(guildId, make_shared<VoiceConnection>(DiscordCoreInternal::ThreadManager::getThreadContext(DiscordCoreInternal::ThreadType::Music).get(), voiceConnectionData, audioBuffersMap, webSocketAgent, guildId, VoiceConnection::voiceConnectionMap));
				voiceConnectionMap->at(guildId)->onSongCompletionEvent = onSongCompletionEvent;
				voiceConnectionMap->at(guildId)->sendSpeakingMessage(true);
				voiceConnectionMap->at(guildId).get()->start();
				voiceConnectionMap->at(guildId).get()->play();
			}
		}

		~VoiceConnection() {}

	protected:

		static map<string, shared_ptr<VoiceConnection>>* voiceConnectionMap;

		shared_ptr<DiscordCoreInternal::VoiceChannelWebSocketAgent> voiceChannelWebSocketAgent{ nullptr };
		map<string, shared_ptr<unbounded_buffer<AudioFrameData>>>* receiveAudioBufferMap{ nullptr };
		shared_ptr<DiscordCoreInternal::WebSocketConnectionAgent> websocketAgent{ nullptr };
		winrt::event<delegate<SongCompletionEventData>>* onSongCompletionEvent {};
		shared_ptr<unbounded_buffer<AudioFrameData>> audioDataBuffer{ nullptr };
		concurrency::event* connectionReadyEvent {new concurrency::event()};
		concurrency::event* disconnectionEvent {new concurrency::event()};
		DiscordCoreInternal::VoiceConnectInitData voiceConnectInitData{};
		DiscordCoreInternal::VoiceConnectionData voiceConnectionData{};
		concurrency::event* playWaitEvent {new concurrency::event()};
		concurrency::event* stopWaitEvent {new concurrency::event()};
		concurrency::event* playSetEvent {new concurrency::event()};
		concurrency::event* stopSetEvent {new concurrency::event()};
		concurrency::event* pauseEvent {new concurrency::event()};
		unsigned short sequenceIndex{ 0 };
		bool areWeConnectedBool{ false };
		OpusEncoder* encoder{ nullptr };
		const int maxBufferSize{ 1276 };
		bool areWeInstantiated{ false };
		bool hasTerminateRun{ false };
		bool areWeStopping{ false };
		bool doWeReconnect{ false };
		unsigned int timestamp{ 0 };
		bool areWeWaiting{ false };
		bool areWePlaying{ false };
		AudioFrameData audioData{};
		bool areWePaused{ false };
		string guildIdNew{ "" };
		bool doWeQuit{ false };
		string guildId{ "" };

		void clearAudioData() {
			if (this->audioData.encodedFrameData.data.size() != 0 && this->audioData.rawFrameData.data.size() != 0) {
				this->audioData.encodedFrameData.data.clear();
				this->audioData.rawFrameData.data.clear();
				this->audioData = AudioFrameData();
			}
			AudioFrameData frameData{};
			while (try_receive(*this->audioDataBuffer, frameData)) {
				frameData.encodedFrameData.data.clear();
				frameData.rawFrameData.data.clear();
			};
		}

		EncodedFrameData encodeSingleAudioFrame(RawFrameData inputFrame) {
			uint8_t* oldBuffer{ new uint8_t[inputFrame.data.size()] };

			for (int x = 0; x < inputFrame.data.size(); x += 1) {
				oldBuffer[x] = inputFrame.data[x];
			}

			uint8_t* newBuffer{ new uint8_t[this->maxBufferSize] };

			int count = opus_encode_float(this->encoder, (float*)oldBuffer, inputFrame.sampleCount, newBuffer, this->maxBufferSize);
			EncodedFrameData encodedFrame;
			for (int x = 0; x < count; x += 1) {
				encodedFrame.data.push_back(newBuffer[x]);
			}
			encodedFrame.sampleCount = inputFrame.sampleCount;
			delete oldBuffer;
			oldBuffer = nullptr;
			delete newBuffer;
			newBuffer = nullptr;
			return encodedFrame;
		}

		vector<uint8_t> encryptSingleAudioFrame(EncodedFrameData bufferToSend) {
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
				bufferToSendNew, bufferToSend.data.size(), nonceForLibSodium, encryptionKeys) != 0) {
				throw exception("ENCRYPTION FAILED!");
			};
			vector<uint8_t> audioDataPacketNew;
			for (unsigned int x = 0; x < numOfBytes; x += 1) {
				audioDataPacketNew.push_back(audioDataPacket[x]);
			}

			delete bufferToSendNew;
			delete encryptionKeys;
			delete audioDataPacket;
			bufferToSend.data.clear();
			this->sequenceIndex += 1;
			this->timestamp += (int)bufferToSend.sampleCount;
			return audioDataPacketNew;
		}

		void sendSingleAudioFrame(vector<uint8_t> audioDataPacketNew) {
			if (this->voiceChannelWebSocketAgent->voiceSocket != nullptr) {
				this->voiceChannelWebSocketAgent->sendVoiceData(audioDataPacketNew);
			}
		}

		void sendSpeakingMessage(bool isSpeaking) {
			if (!this->doWeQuit) {
				this->voiceConnectionData.audioSSRC = this->voiceChannelWebSocketAgent->voiceConnectionData.audioSSRC;
				string newString = DiscordCoreInternal::getIsSpeakingPayload(isSpeaking, this->voiceConnectionData.audioSSRC, 0);
				if (this->voiceChannelWebSocketAgent->webSocket != nullptr) {
					this->voiceChannelWebSocketAgent->sendMessage(newString);
				}
			}
		}

		void run() {
			try {
				cout << "WERE HERE 0000" << endl;
				while (!this->doWeQuit) {
					cout << "WERE HERE 1111" << endl;
					if (!this->areWePlaying) {
						cout << "WERE HERE 2222" << endl;
						this->playWaitEvent->set();
						this->audioDataBuffer = this->receiveAudioBufferMap->at(this->voiceConnectInitData.guildId);
						this->audioData.type = AudioFrameType::Unset;
						this->audioData.encodedFrameData.data.clear();
						this->audioData.rawFrameData.data.clear();
						if (this->playSetEvent->wait(10000) != 0) {
							cout << "WERE HERE 3333" << endl;
							this->playSetEvent->reset();
							continue;
						};
						this->playSetEvent->reset();
					start:
						if (this->doWeQuit) {
							this->done();
							break;
						}
						StopWatch stopWatch(10000);
						while (!this->doWeQuit) {
							concurrency::wait(100);
							if (try_receive(this->audioDataBuffer.get(), this->audioData) || stopWatch.hasTimePassed()) {
								stopWatch.resetTimer();
								cout << "WERE HERE 4444" << endl;
								break;
							};
						}
						if (this->audioData.encodedFrameData.sampleCount == 0 || this->audioData.rawFrameData.sampleCount == 0 || this->audioData.type == AudioFrameType::Unset) {
							cout << "WERE HERE 5555" << endl;
							goto start;
						}
						else if (this->audioData.type == AudioFrameType::Cancel) {
							cout << "WERE HERE 6666" << endl;
							continue;
						}
					}
					cout << "WERE HERE 7777" << endl;
					this->sendSpeakingMessage(true);
					long long startingValue{ (long long)chrono::duration_cast<chrono::nanoseconds>(chrono::high_resolution_clock::now().time_since_epoch()).count() };
					long long intervalCount{ 20000000 };
					long long startingValueForCalc{ 0 };
					long long timeCounter{ 0 };
					long long totalTime{ 0 };
					int frameCounter{ 0 };

					if (this->audioData.type == AudioFrameType::Encoded) {
						cout << "WERE HERE 8888" << endl;
						this->areWePlaying = true;
						while (this->audioData.encodedFrameData.sampleCount != 0 && !this->areWeStopping) {
							if (this->doWeReconnect) {
								VoiceConnection::reconnect(this->guildId);
								this->done();
								return;
							}
							if (this->areWeStopping) {
								this->areWePlaying = false;
								frameCounter = 0;
								break;
							}
							if (this->areWePaused) {
								this->pauseEvent->wait();
								this->areWePaused = false;
							}
							if (this->doWeQuit) {
								this->clearAudioData();
								this->areWePlaying = false;
								frameCounter = 0;
								break;
							}
							frameCounter += 1;
							try_receive(*this->audioDataBuffer, this->audioData);
							nanoSleep(100000);
							if (this->audioData.type != AudioFrameType::Cancel && this->audioData.type != AudioFrameType::Unset) {
								vector<uint8_t> newFrame = this->encryptSingleAudioFrame(this->audioData.encodedFrameData);
								nanoSleep(18000000);
								timeCounter = (long long)chrono::duration_cast<chrono::nanoseconds>(chrono::high_resolution_clock::now().time_since_epoch()).count() - startingValue;
								long long  waitTime = intervalCount - timeCounter;
								spinLock(waitTime);
								startingValue = (long long)chrono::duration_cast<chrono::nanoseconds>(chrono::high_resolution_clock::now().time_since_epoch()).count();
								startingValueForCalc = (long long)chrono::duration_cast<chrono::nanoseconds>(chrono::high_resolution_clock::now().time_since_epoch()).count();
								this->sendSingleAudioFrame(newFrame);
								totalTime += (long long)chrono::duration_cast<chrono::nanoseconds>(chrono::high_resolution_clock::now().time_since_epoch()).count() - startingValueForCalc;
								intervalCount = 20000000 - (totalTime / frameCounter);
								this->audioData.type = AudioFrameType::Unset;
								this->audioData.encodedFrameData.data.clear();
								this->audioData.rawFrameData.data.clear();
							}
							else if (this->audioData.type == AudioFrameType::Cancel) {
								SongCompletionEventData completionEventData{};
								completionEventData.voiceConnection = this;
								completionEventData.isThisAReplay = false;
								(*this->onSongCompletionEvent)(completionEventData);
								this->areWePlaying = false;
								frameCounter = 0;
								break;
							}
						}
						this->areWePlaying = false;
					}
					else if (this->audioData.type == AudioFrameType::RawPCM) {
						this->areWePlaying = true;
						while (this->audioData.encodedFrameData.sampleCount != 0 && !this->areWeStopping) {
							if (this->doWeReconnect) {
								VoiceConnection::reconnect(this->guildId);
								this->done();
								return;
							}
							if (this->areWeStopping) {
								this->areWePlaying = false;
								frameCounter = 0;
								break;
							}
							if (this->areWePaused) {
								this->pauseEvent->wait();
								this->areWePaused = false;
							}
							if (this->doWeQuit) {
								this->clearAudioData();
								this->areWePlaying = false;
								frameCounter = 0;
								break;
							}
							frameCounter += 1;
							try_receive(*this->audioDataBuffer, this->audioData);
							nanoSleep(100000);
							if (this->audioData.type != AudioFrameType::Cancel && this->audioData.type != AudioFrameType::Unset) {
								auto newFrames = encodeSingleAudioFrame(this->audioData.rawFrameData);
								vector<uint8_t> newFrame = this->encryptSingleAudioFrame(newFrames);
								nanoSleep(18000000);
								timeCounter = (long long)chrono::duration_cast<chrono::nanoseconds>(chrono::high_resolution_clock::now().time_since_epoch()).count() - startingValue;
								long long  waitTime = intervalCount - timeCounter;
								spinLock(waitTime);
								startingValue = (long long)chrono::duration_cast<chrono::nanoseconds>(chrono::high_resolution_clock::now().time_since_epoch()).count();
								startingValueForCalc = (long long)chrono::duration_cast<chrono::nanoseconds>(chrono::high_resolution_clock::now().time_since_epoch()).count();
								this->sendSingleAudioFrame(newFrame);
								totalTime += (long long)chrono::duration_cast<chrono::nanoseconds>(chrono::high_resolution_clock::now().time_since_epoch()).count() - startingValueForCalc;
								intervalCount = 20000000 - (totalTime / frameCounter);
								this->audioData.type = AudioFrameType::Unset;
								this->audioData.encodedFrameData.data.clear();
								this->audioData.rawFrameData.data.clear();
							}
							else if (this->audioData.type == AudioFrameType::Cancel) {
								SongCompletionEventData completionEventData{};
								completionEventData.voiceConnection = this;
								completionEventData.isThisAReplay = false;
								(*this->onSongCompletionEvent)(completionEventData);
								this->areWePlaying = false;
								frameCounter = 0;
								break;
							}
						}
					}
					this->areWePlaying = false;
					if (this->areWeStopping) {
						this->stopWaitEvent->set();
						this->stopSetEvent->wait(5000);
						this->clearAudioData();
						this->stopSetEvent->reset();
						this->areWeStopping = false;
					}
					this->sendSpeakingMessage(false);
				}
			}
			catch (...) {
				rethrowException("VoiceConnection::run() Error: ");
			}
			this->done();
		}
	};
	map<string, shared_ptr<VoiceConnection>>* VoiceConnection::voiceConnectionMap{ new map<string, shared_ptr<VoiceConnection>>() };
};
#endif