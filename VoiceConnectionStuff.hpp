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

		VoiceConnection(shared_ptr<DiscordCoreInternal::ThreadContext> threadContextNew, DiscordCoreInternal::VoiceConnectInitData voiceConnectInitDataNew, map<string, shared_ptr<unbounded_buffer<AudioFrameData>>*>* sendAudioBufferMapNew, shared_ptr<DiscordCoreClientBase> discordCoreClientBaseNew, shared_ptr<DiscordCoreInternal::WebSocketConnectionAgent> websocketAgent) :
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
				this->discordCoreClientBase = discordCoreClientBaseNew;
				this->receiveAudioBufferMap = sendAudioBufferMapNew;
				this->audioDataBuffer = make_shared<unbounded_buffer<AudioFrameData>>();
				this->voicechannelWebSocketAgent = make_shared<DiscordCoreInternal::VoiceChannelWebSocketAgent>(&this->readyBuffer, websocketAgent->collectVoiceConnectionDataBuffer, websocketAgent->voiceConnectionDataBuffer, voiceConnectInitDataNew);
				this->voicechannelWebSocketAgent->start();
				receive(this->readyBuffer);
				this->voiceConnectInitData = this->voicechannelWebSocketAgent->voiceConnectInitData;
				this->voiceConnectionData = this->voicechannelWebSocketAgent->voiceConnectionData;
				this->receiveAudioBufferMap->insert_or_assign(this->voiceConnectInitData.guildId, &this->audioDataBuffer);
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

		event_token onSongCompletion(delegate<VoiceConnection*> const& handler) {
			if (!this->areWeInstantiated) {
				this->areWeInstantiated = true;
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

		bool stop() {
			if (this->areWePlaying) {
				this->areWeStopping = true;
				this->areWePlaying = false;
				this->areWeStreaming = false;
				receive(this->stopBuffer);
				bool shouldWePlay;
				while (try_receive(this->playBuffer, shouldWePlay)) {};
				send(this->stopBuffer, true);
				return true;
			}
			else {
				return false;
			}
		}

		bool skip() {
			if (this->areWePlaying){
				this->areWeSkipping = true;
				this->areWePlaying = false;
				this->areWeStreaming = false;
				receive(this->skipBuffer);
				bool shouldWePlay;
				while (try_receive(this->playBuffer, shouldWePlay)) {};
				send(this->skipBuffer, true);
				return true;
			}
			else {
				return false;
			}
		}

		void play() {
			if (this != nullptr) {
				receive(this->readyBuffer);
				if (!this->areWePlaying) {
					send(this->playBuffer, true);
				}

				if (!this->areWeWaiting) {
					this->areWeWaiting = true;
					wait(this);
					this->areWeWaiting = false;
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

		~VoiceConnection() {}

	protected:
		friend class DiscordCoreClientBase;
		friend class Guild;
		shared_ptr<DiscordCoreInternal::VoiceChannelWebSocketAgent> voicechannelWebSocketAgent{ nullptr };
		map<string, shared_ptr<unbounded_buffer<AudioFrameData>>*>* receiveAudioBufferMap{ nullptr };
		DiscordCoreInternal::VoiceConnectionData voiceConnectionData{};
		shared_ptr<unbounded_buffer<AudioFrameData>> audioDataBuffer{ nullptr };
		shared_ptr<DiscordCoreClientBase> discordCoreClientBase{ nullptr };
		DiscordCoreInternal::VoiceConnectInitData voiceConnectInitData{};
		winrt::event<delegate<VoiceConnection*>> onSongCompletionEvent;
		unbounded_buffer<bool> readyBuffer{ nullptr };
		unbounded_buffer<bool> pauseBuffer{ nullptr };
		unbounded_buffer<bool> stopBuffer{ nullptr };
		unbounded_buffer<bool> skipBuffer{ nullptr };
		unbounded_buffer<bool> playBuffer{ nullptr };
		unsigned short sequenceIndex{ 0 };
		bool areWeConnectedBool{ false };
		OpusEncoder* encoder{ nullptr };
		const int maxBufferSize{ 1276 };
		bool areWeInstantiated{ false };
		bool hasTerminateRun{ false };
		bool areWeStreaming{ false };
		bool areWeStopping{ false };
		bool areWeSkipping{ false };
		unsigned int timestamp{ 0 };
		bool areWeWaiting{ false };
		bool areWePlaying{ false };
		AudioFrameData audioData{};
		bool areWePaused{ false };
		bool doWeQuit{ false };

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
			if (this->voicechannelWebSocketAgent->voiceSocket != nullptr) {
				this->voicechannelWebSocketAgent->sendVoiceData(audioDataPacketNew);
			}
		}

		void sendSpeakingMessage(bool isSpeaking) {
			if (!this->doWeQuit) {
				this->voiceConnectionData.audioSSRC = this->voicechannelWebSocketAgent->voiceConnectionData.audioSSRC;
				string newString = DiscordCoreInternal::getIsSpeakingPayload(isSpeaking, this->voiceConnectionData.audioSSRC, 0);
				if (this->voicechannelWebSocketAgent->webSocket != nullptr) {
					this->voicechannelWebSocketAgent->sendMessage(newString);
				}
			}
		}

		void run() {
			while (!this->doWeQuit) {
				if (!this->areWePlaying) {
					send(this->readyBuffer, true);
					receive(this->playBuffer);
					this->audioDataBuffer = *this->receiveAudioBufferMap->at(this->voiceConnectInitData.guildId);
					this->audioData.type = AudioFrameType::Unset;
					this->audioData.encodedFrameData.data.clear();
					this->audioData.rawFrameData.data.clear();
				start:
					try {
						this->audioData = receive(*this->audioDataBuffer, 10000);
					}
					catch (exception&) {};
					
					if (this->audioData.encodedFrameData.sampleCount == 0 || this->audioData.rawFrameData.sampleCount == 0) {
						goto start;
					}
				}
				this->sendSpeakingMessage(true);
				long long startingValue{ (long long)chrono::duration_cast<chrono::nanoseconds>(chrono::system_clock::now().time_since_epoch()).count() };
				long long intervalCount{ 20000000 };
				long long timeCounter{ 0 };
				int frameCounter{ 0 };

				if (this->audioData.type == AudioFrameType::Encoded) {
					this->areWeStreaming = true;
					this->areWePlaying = true;
					while (this->audioData.encodedFrameData.sampleCount != 0 && !this->areWeStopping && !this->areWeSkipping) {
						if (this->areWeSkipping) {
							frameCounter = 0;
							break;
						}
						if (this->areWeStopping) {
							frameCounter = 0;
							break;
						}
						if (this->areWePaused) {
							this->areWeStreaming = false;
							receive(this->pauseBuffer);
							this->areWePaused = false;
						}
						if (this->doWeQuit) {
							this->clearAudioData();
							this->areWeStreaming = false;
							this->areWePlaying = false;
							frameCounter = 0;
				 			break;
						}
						frameCounter += 1;
						try_receive(*this->audioDataBuffer, this->audioData);
						nanosleep(100000);
						if (this->audioData.type != AudioFrameType::Cancel && this->audioData.type != AudioFrameType::Unset) {
							vector<uint8_t> newFrame = this->encryptSingleAudioFrame(this->audioData.encodedFrameData);
							nanosleep(17500000);
							timeCounter = (long long)chrono::duration_cast<chrono::nanoseconds>(chrono::system_clock::now().time_since_epoch()).count() - startingValue;
							long long  waitTime = intervalCount - timeCounter;
							spinLock(waitTime);
							startingValue = (long long)chrono::duration_cast<chrono::nanoseconds>(chrono::system_clock::now().time_since_epoch()).count();
							this->sendSingleAudioFrame(newFrame);
							this->audioData.type = AudioFrameType::Unset;
							this->audioData.encodedFrameData.data.clear();
							this->audioData.rawFrameData.data.clear();
						}
						else if (this->audioData.type == AudioFrameType::Cancel) {
							this->onSongCompletionEvent(this);
							this->areWeStreaming = false;
							this->areWePlaying = false;
							frameCounter = 0;
							break;
						}
					}
					this->areWePlaying = false;
				}
				else if (this->audioData.type == AudioFrameType::RawPCM ) {
					this->areWeStreaming = true;
					this->areWePlaying = true;
					while (this->audioData.rawFrameData.sampleCount != 0 && !this->areWeStopping && !this->areWeSkipping) {
						if (this->areWeSkipping) {
							frameCounter = 0;
							break;
						}
						if (this->areWeStopping) {
							frameCounter = 0;
							break;
						}
						if (this->areWePaused) {
							this->areWeStreaming = false;
							receive(this->pauseBuffer);
							this->areWePaused = false;
						}
						if (this->doWeQuit) {
							this->clearAudioData();
							this->areWeStreaming = false;
							this->areWePlaying = false;
							frameCounter = 0;
							break;
						}
						frameCounter += 1;
						try_receive(*this->audioDataBuffer, this->audioData);
						nanosleep(17500000);
						if (this->audioData.type != AudioFrameType::Cancel && this->audioData.type != AudioFrameType::Unset) {
							auto newFrames = encodeSingleAudioFrame(this->audioData.rawFrameData);
							vector<uint8_t> newFrame = this->encryptSingleAudioFrame(newFrames);
							timeCounter = (long long)chrono::duration_cast<chrono::nanoseconds>(chrono::system_clock::now().time_since_epoch()).count() - startingValue;
							long long  waitTime = intervalCount - timeCounter;
							spinLock(waitTime);
							startingValue = (long long)chrono::duration_cast<chrono::nanoseconds>(chrono::system_clock::now().time_since_epoch()).count();
							this->sendSingleAudioFrame(newFrame);
							this->audioData.type = AudioFrameType::Unset;
							this->audioData.encodedFrameData.data.clear();
							this->audioData.rawFrameData.data.clear();
						}
						else if (this->audioData.type == AudioFrameType::Cancel) {
							this->onSongCompletionEvent(this);
							this->areWeStreaming = false;
							this->areWePlaying = false;
							frameCounter = 0;
							break;
						}
					}
					this->areWePlaying = false;
				}
				this->areWePlaying = false;
				if (this->areWeStopping) {
					send(this->stopBuffer, true);
					receive(this->stopBuffer);
					this->areWeStopping = false;
				}
				if (this->areWeSkipping) {
					send(this->skipBuffer, true);
					receive(this->skipBuffer);
					this->areWeSkipping = false;
				}
				this->sendSpeakingMessage(false);
			}
			this->done();

		}


	};

};
#endif