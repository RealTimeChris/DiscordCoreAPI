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
		vector<uint8_t> audioData;
	};

	class ThreadPoolTimerNew {
	public:
		ThreadPoolTimerNew() {};
		ThreadPoolTimer threadPoolTimer{ nullptr };
	};

	class VoiceConnection : public agent {
	public:
		bool doWeQuit = false;
		bool doWeWait = true;
		bool areWePlaying = false;
		__int32 timestamp;
		__int16 sequenceIndex;
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
		int frameSize = 960, nChannels = 2;

		vector<uint8_t> encodeSingleAudioFrame(vector<uint8_t> inputFrame) {
			uint8_t* oldBuffer;
			oldBuffer = new uint8_t[inputFrame.size()];
			for (unsigned int x = 0; x < inputFrame.size(); x += 1) {
				oldBuffer[x] = inputFrame[x];
			}
			uint8_t* newBuffer;
			int bufferSize = frameSize * nChannels * sizeof(float);
			newBuffer = new uint8_t[bufferSize];

			int count = opus_encode_float(encoder, (float*)newBuffer, frameSize, newBuffer, bufferSize);
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
			cout << "AUDIO SSRC: " << this->voiceConnectionData.audioSSRC << endl;
			cout << "SEQUENCE INDEX: " << this->sequenceIndex << endl;
			cout << "TIMESTAMP: " << this->timestamp<< endl;
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

			uint8_t nonce[nonceSize] =
			{ (uint8_t)((this->sequenceIndex>> (8 * 3)) & 0xff),
				(uint8_t)((this->sequenceIndex >> (8 * 2)) & 0xff),
				(uint8_t)((this->sequenceIndex >> (8 * 1)) & 0xff),
				(uint8_t)((this->sequenceIndex >> (8 * 0)) & 0xff) };

			const size_t numOfBytes = headerSize + nonceSize + bufferToSend.size() + crypto_secretbox_MACBYTES;
			uint8_t* audioDataPacket;
			audioDataPacket = new uint8_t[numOfBytes];
			memcpy_s(audioDataPacket, headerSize, header, headerSize);

			if (crypto_secretbox_easy(audioDataPacket + headerSize,
				bufferToSend.data(), bufferToSend.size(), nonce, (unsigned char*)this->voiceConnectionData.keys.data()) != 0) {
				throw exception("ENCRYPTION FAILED!");
			};

			memcpy_s(audioDataPacket + (numOfBytes - nonceSize), nonceSize, nonce, nonceSize);
			vector<uint8_t> audioDataPacketNew;
			audioDataPacketNew.resize(numOfBytes);
			for (unsigned int x = 0; x < numOfBytes; x += 1) {
				audioDataPacketNew[x] = audioDataPacket[x];
			}

			this->sequenceIndex += 1;
			this->voicechannelWebSocketAgent->sendVoiceData(audioDataPacketNew);
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
			vector<ThreadPoolTimerNew> sendTimers;
			string playerId;
			while (!this->doWeQuit) {
				if (this->doWeWait) {
					receive(this->playPauseBuffer);
				}
				auto audioData = receive(*this->bufferMessageBlock);
				this->areWePlaying = true;

				int currentPosition = 0, currentPositionPrev = 0;
				vector<uint8_t> testVector;
				//sendSpeakingMessage(true);
				int count = 0;
				int startingCount = (int)std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
				while (this->areWePlaying) {
					count = (int)std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count() - startingCount;
					if (count >= 20) {
						startingCount = (unsigned int)std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
						currentPosition += frameSize * nChannels * sizeof(float);
						if (currentPosition > audioData.audioData.size()) {
							currentPosition = (int)audioData.audioData.size();
						}
						vector<uint8_t> newVector;
						for (int x = currentPositionPrev; x < currentPosition; x += 1) {
							newVector.push_back(audioData.audioData[x]);
						}
						vector<uint8_t> encodedFrame = encodeSingleAudioFrame(newVector);
						currentPositionPrev = currentPosition;

						sendSingleAudioQuantum(encodedFrame);
						for (auto value : encodedFrame) {
							testVector.push_back(value);
						}
						if (currentPosition >= audioData.audioData.size()) {
							this->areWePlaying = false;
						}
					}
				}
				sendSpeakingMessage(false);
				InMemoryRandomAccessStream randomStream;
				DataWriter dataWriter(randomStream.GetOutputStreamAt(0));
				dataWriter.WriteBytes(testVector);
				
				cout << "TEST VECTOR SIZE: " << testVector.size() << endl;
				dataWriter.StoreAsync().get();
				DataReader dataReader(randomStream.GetInputStreamAt(0));
				dataReader.LoadAsync((uint32_t)randomStream.Size());
				auto readBuffer = dataReader.ReadBuffer((uint32_t)randomStream.Size());
				saveFile(L"C:\\Users\\Chris\\Downloads", L"TESTFILENAME.opus", readBuffer);

			}
			cout << "END OF VOICECONNECTION::PLAY: " << endl;
			done();
		}

		void terminate() {
			done();
			this->doWeQuit = true;
		}

	};

}
#endif