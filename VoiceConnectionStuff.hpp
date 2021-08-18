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

namespace DiscordCoreAPI {

	class VoiceConnection : public agent {
	public:
		VoiceConnection(DiscordCoreInternal::VoiceConnectionData voiceConnectionDataNew, shared_ptr<unbounded_buffer<vector<RawFrame>>> bufferMessageBlockNew)
			: agent(*DiscordCoreInternal::ThreadManager::getThreadContext().get()->scheduler) {
			if (voiceConnectionDataNew.channelId != "") {
				if (sodium_init() == -1) {
					cout << "LibSodium failed to initialize!" << endl << endl;
				}
				this->voiceConnectionData = voiceConnectionDataNew;
				this->bufferMessageBlock = bufferMessageBlockNew;
				this->readyBuffer = make_shared<unbounded_buffer<bool>>();
				this->voicechannelWebSocketAgent = make_shared<DiscordCoreInternal::VoiceChannelWebSocketAgent>(DiscordCoreInternal::ThreadManager::getThreadContext().get(), this->voiceConnectionData, this->readyBuffer);
				send(this->voicechannelWebSocketAgent->voiceConnectionDataBuffer, this->voiceConnectionData);
				this->voicechannelWebSocketAgent->start();
				receive(*this->readyBuffer);
				this->voiceConnectionData = this->voicechannelWebSocketAgent->voiceConnectionData;
				int error;
				encoder = opus_encoder_create(48000, 2, OPUS_APPLICATION_AUDIO, &error);
				if (error != OPUS_OK) {
					cout << "Failed to create Opus encoder!";
				}
			}
			else {
				throw exception("Sorry, but you need to select a proper voice channel to connect to!");
			}
		}

		string getChannelId() {
			return this->voiceConnectionData.channelId;
		}

		event_token onSongCompletion(delegate<> const& handler) {
			return onSongCompletionEvent.add(handler);
		}

		bool areWeConnected() {
			if (this == nullptr) {
				return false;
			}
			else {
				return true;
			}
		}

		void onSongCompletion(event_token const& token) {
			onSongCompletionEvent.remove(token);
		}

		void stopPlaying() {
			this->areWePlaying = false;
			this->areWeWaitingForAudioData = true;
		}

		bool areWeCurrentlyPlaying() {
			return this->areWePlaying;
		}

		void play(bool doWeBlock = false) {
			send(this->playPauseBuffer, true);
			this->doWeWait = true;
			this->areWePlaying = false;
			this->areWeWaitingForAudioData = true;
			this->start();
			if (doWeBlock) {
				wait(this);
			}
		}

		void pausePlaying() {
			this->doWeWait = true;
		}

		~VoiceConnection() {
			opus_encoder_destroy(encoder);
			this->terminate();
		}

	protected:
		friend class Guild;
		bool doWeQuit = false;
		bool doWeWait = true;
		bool areWePlaying = false;
		bool areWeConnectedBool = false;
		bool areWeWaitingForAudioData = true;
		unsigned int timestamp = 0;
		unsigned short sequenceIndex = 0;
		__int32 sequenceIndexLib = 0;
		DiscordCoreInternal::VoiceConnectionData voiceConnectionData;
		shared_ptr<DiscordCoreInternal::VoiceChannelWebSocketAgent> voicechannelWebSocketAgent{ nullptr };
		shared_ptr<unbounded_buffer<vector<RawFrame>>> bufferMessageBlock;
		shared_ptr<unbounded_buffer<bool>> readyBuffer;
		unbounded_buffer<bool> playPauseBuffer;
		OpusEncoder* encoder;
		int nChannels = 2;
		winrt::event<delegate<>> onSongCompletionEvent;

		EncodedFrame encodeSingleAudioFrame(RawFrame inputFrame) {
			uint8_t* oldBuffer;
			oldBuffer = new uint8_t[inputFrame.data.size()];
			for (unsigned int x = 0; x < inputFrame.data.size(); x += 1) {
				oldBuffer[x] = inputFrame.data[x];
			}
			uint8_t* newBuffer;
			int bufferSize = (int)inputFrame.data.size() * nChannels * sizeof(float);
			newBuffer = new uint8_t[bufferSize];
			int count = opus_encode_float(encoder, (float*)oldBuffer, 960, newBuffer, bufferSize);
			vector<uint8_t> newVector{};
			for (int x = 0; x< count; x += 1) {
				newVector.push_back(newBuffer[x]);
			}
			EncodedFrame encodedFrame;
			encodedFrame.data = newVector;
			encodedFrame.sampleCount = inputFrame.sampleCount;
			delete oldBuffer;
			oldBuffer = nullptr;
			delete newBuffer;
			newBuffer = nullptr;
			return encodedFrame;
		}
		
		void sendSingleAudioFrame(EncodedFrame bufferToSend) {
			constexpr int headerSize = 12;
			constexpr int nonceSize = crypto_secretbox_NONCEBYTES;
			uint8_t header01[2]{ 0x80 , 0x78 };
			uint16_t header02[1]{ this->sequenceIndex };
			uint32_t header03[2]{ (uint32_t)this->timestamp, (uint32_t)this->voiceConnectionData.audioSSRC };

			uint8_t headerFinal[headerSize];
			headerFinal[0] = header01[0];
			headerFinal[1] = header01[1];
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
			std::memcpy(audioDataPacket, headerFinal, headerSize);
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
			this->sequenceIndexLib += 1;
			this->timestamp += (int)bufferToSend.sampleCount;
		}

		void sendSpeakingMessage(bool isSpeaking) {
			this->voiceConnectionData.audioSSRC = this->voicechannelWebSocketAgent->voiceConnectionData.audioSSRC;
			string newString = DiscordCoreInternal::getIsSpeakingPayload(isSpeaking, this->voiceConnectionData.audioSSRC, 0);
			this->voicechannelWebSocketAgent->sendMessage(newString);
		}

		void saveFile(hstring filePath, hstring fileName, IBuffer readBuffer) {
			auto folder = winrt::Windows::Storage::StorageFolder::GetFolderFromPathAsync(filePath).get();
			winrt::Windows::Storage::StorageFile storageFile = folder.CreateFileAsync(fileName, CreationCollisionOption::ReplaceExisting).get();
			winrt::Windows::Storage::FileIO::WriteBufferAsync(storageFile, readBuffer).get();
		}

		IBuffer loadFile(hstring filePath, hstring fileName) {
			auto folder = winrt::Windows::Storage::StorageFolder::GetFolderFromPathAsync(filePath).get();
			winrt::Windows::Storage::StorageFile storageFile = folder.GetFileAsync(fileName).get();
			auto returnBuffer = winrt::Windows::Storage::FileIO::ReadBufferAsync(storageFile).get();
			return returnBuffer;
		}

		void run() {
			this->areWeConnectedBool = true;
			while (!this->doWeQuit) {
				if (this->doWeWait) {
					receive(this->playPauseBuffer);
				}
				vector<RawFrame> audioData;
				if (this->areWeWaitingForAudioData) {
					audioData = receive(*this->bufferMessageBlock);
					this->areWeWaitingForAudioData = false;
				}
				
				this->areWePlaying = true;
				sendSpeakingMessage(true);
				int frameCounter = 0;
				while (this->areWePlaying) {

					int timeCounter = 0;
					int startingValue = (int)std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
					int intervalCount = 20;
					for (auto value : audioData) {
						while (timeCounter < intervalCount) {
							timeCounter = (int)std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count() - startingValue;
						}
						timeCounter = 0;
						startingValue = (int)std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
						auto newVectorNew = encodeSingleAudioFrame(value);
						sendSingleAudioFrame(newVectorNew);
						frameCounter += 1;
						if (frameCounter >= audioData.size() - 1) {
							this->areWePlaying = false;
							this->areWeWaitingForAudioData = true;
							this->onSongCompletionEvent();
							frameCounter = 0;
						}
						if (this->areWePlaying == false) {
							break;
						}
					}
					
				}
				sendSpeakingMessage(false);
			}
			this->areWeConnectedBool = false;
			done();
		}

		void terminate() {
			done();
			this->areWePlaying = false;
			this->doWeQuit = true;
		}

	};

}
#endif