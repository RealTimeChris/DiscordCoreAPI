// VoiceConnectionStuff.hpp - Header for all of the "voice connection" stuff.
// Jul 15, 2021
// Chris M.
// https://github.com/RealTimeChris

#pragma once

#ifndef _VOICE_CONNECTION_STUFF_
#define _VOICE_CONNECTION_STUFF_

#include "../pch.h"
#include "WebSocketStuff.hpp"
#include "DemuxingStuff.hpp"
#include "DecodingStuff.hpp"

namespace DiscordCoreAPI {

	struct AudioDataChunk {
		vector<vector<uint8_t>> audioData;
	};

	class VoiceConnection : public agent {
	public:
		bool doWeQuit = false;
		bool doWeWait = true;
		bool areWePlaying = false;
		unsigned int timestamp = 0;
		unsigned short sequenceIndex = 0;
		__int32 sequenceIndexLib = 0;
		DiscordCoreInternal::VoiceConnectionData voiceConnectionData;
		shared_ptr<DiscordCoreInternal::VoiceChannelWebSocketAgent> voicechannelWebSocketAgent{ nullptr };
		shared_ptr<unbounded_buffer<AudioDataChunk>> bufferMessageBlock;
		shared_ptr<unbounded_buffer<bool>> readyBuffer;
		unbounded_buffer<bool> playPauseBuffer;
		VoiceConnection(DiscordCoreInternal::VoiceConnectionData voiceConnectionDataNew, shared_ptr<unbounded_buffer<AudioDataChunk>> bufferMessageBlockNew)
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

		void deleteFile(string filePath, string fileName) {
			auto folder = winrt::Windows::Storage::KnownFolders::GetFolderAsync(winrt::Windows::Storage::KnownFolderId::MusicLibrary).get();
			auto folder2 = folder.GetFolderFromPathAsync(to_hstring(filePath)).get();
			winrt::Windows::Storage::StorageFile storageFile = folder2.GetFileAsync(to_hstring(fileName)).get();
			storageFile.DeleteAsync().get();
		}

		void play(bool doWeBlock = false) {
			send(this->playPauseBuffer, true);
			this->doWeWait = false;
			this->start();
			if (doWeBlock) {
				wait(this);
			}
		}

		void startPlaying() {
			send(this->playPauseBuffer, true);
			this->doWeWait = false;
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
		OpusEncoder* encoder;
		int nChannels = 2;

		vector<uint8_t> encodeSingleAudioFrame(vector<uint8_t> inputFrame) {
			uint8_t* oldBuffer;
			oldBuffer = new uint8_t[inputFrame.size()];
			for (unsigned int x = 0; x < inputFrame.size(); x += 1) {
				oldBuffer[x] = inputFrame[x];
			}
			uint8_t* newBuffer;
			int bufferSize = (int)inputFrame.size() * nChannels * sizeof(float);
			newBuffer = new uint8_t[bufferSize];
			cout << "INPUT SIZE: " << inputFrame.size() << endl;
			int count = opus_encode_float(encoder, (float*)oldBuffer, 960, newBuffer, bufferSize);
			cout << "ENCODED COUNT: " << count << endl;
			vector<uint8_t> newVector;
			for (int x = 0; x< count; x += 1) {
				newVector.push_back(newBuffer[x]);
			}
			delete oldBuffer;
			oldBuffer = nullptr;
			delete newBuffer;
			newBuffer = nullptr;
			return newVector;
		}

		void sendSingleAudioQuantum(vector<uint8_t> bufferToSend) {
			constexpr int headerSize = 12;
			constexpr int nonceSize = 4;
			const uint8_t header[headerSize] = {
				0x80,
				0x78,
				static_cast<uint8_t>((this->sequenceIndex >> (8 * 1)) & 0xff),
				static_cast<uint8_t>((this->sequenceIndex >> (8 * 0)) & 0xff),
				static_cast<uint8_t>((this->timestamp >> (8 * 3)) & 0xff),
				static_cast<uint8_t>((this->timestamp >> (8 * 2)) & 0xff),
				static_cast<uint8_t>((this->timestamp >> (8 * 1)) & 0xff),
				static_cast<uint8_t>((this->timestamp >> (8 * 0)) & 0xff),
				static_cast<uint8_t>((this->voiceConnectionData.audioSSRC >> (8 * 3)) & 0xff),
				static_cast<uint8_t>((this->voiceConnectionData.audioSSRC >> (8 * 2)) & 0xff),
				static_cast<uint8_t>((this->voiceConnectionData.audioSSRC >> (8 * 1)) & 0xff),
				static_cast<uint8_t>((this->voiceConnectionData.audioSSRC >> (8 * 0)) & 0xff)
			};

			uint8_t nonceForDiscord[nonceSize] = {
				static_cast<uint8_t>((this->sequenceIndex >> (8 * 1)) & 0xff),
				static_cast<uint8_t>((this->sequenceIndex >> (8 * 0)) & 0xff)
			};

			uint8_t nonce[crypto_secretbox_NONCEBYTES] = {
				static_cast<uint8_t>((this->sequenceIndex >> (8 * 1)) & 0xff),
				static_cast<uint8_t>((this->sequenceIndex >> (8 * 0)) & 0xff)
			};

			cout << "BUFFER SIZE PRE: " << bufferToSend.size() << endl;
			const size_t numOfBytes = headerSize + bufferToSend.size() + nonceSize + crypto_secretbox_MACBYTES;
			std::vector<uint8_t> audioDataPacket(numOfBytes);
			std::memcpy(audioDataPacket.data(), header, headerSize);
			std::memcpy(audioDataPacket.data() + numOfBytes - nonceSize, nonceForDiscord, nonceSize);
			unsigned char* encryptionKeys = new unsigned char[crypto_secretbox_KEYBYTES];
			unsigned char* bufferToSendNew = new unsigned char[bufferToSend.size()];
			cout << "KEYS SIZE: " << crypto_secretbox_KEYBYTES << sizeof(encryptionKeys) * sizeof(unsigned char) << endl;
			cout << "BUFFER SIZE POST: " << numOfBytes << endl;
			for (unsigned int x = 0; x < bufferToSend.size(); x += 1) {
				bufferToSendNew[x] = bufferToSend[x];
			}
			for (unsigned int x = 0; x < this->voiceConnectionData.keys.size(); x += 1) {
				encryptionKeys[x] = (unsigned char)this->voiceConnectionData.keys[x];
			}
			if (crypto_secretbox_easy(audioDataPacket.data() + headerSize,
				bufferToSendNew, bufferToSend.size(), nonce, encryptionKeys) != 0) {
				throw exception("ENCRYPTION FAILED!");
			};

			this->voicechannelWebSocketAgent->sendVoiceData(audioDataPacket);
			this->sequenceIndex += 1;
			this->sequenceIndexLib += 1;
			this->timestamp += 20;
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
			while (!this->doWeQuit) {
				if (this->doWeWait) {
					receive(this->playPauseBuffer);
				}
				auto audioData = receive(*this->bufferMessageBlock);
				this->areWePlaying = true;

				int currentPosition = 0;
				sendSpeakingMessage(true);
				int count = 0;
				int startingCount = (int)std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
				while (this->areWePlaying) {
					count = (int)std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count() - startingCount;
					if (count >= 20) {
						startingCount = (unsigned int)std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
						//auto value = encodeSingleAudioFrame(audioData.audioData.at(currentPosition));
						//sendSingleAudioQuantum(value);
						sendSingleAudioQuantum(audioData.audioData.at(currentPosition));
						currentPosition += 1;
						if (currentPosition == audioData.audioData.size()) {
							this->areWePlaying = false;
						}
					}
				}
				sendSpeakingMessage(false);

			}
			done();
		}

		void terminate() {
			done();
			this->doWeQuit = true;
		}

	};

}
#endif