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

namespace DiscordCoreAPI {

	struct AudioDataChunk {
		IBuffer audioData;
		string playerId;
		unsigned __int64 audioBitrate;
		__int64 totalByteSize = 1;
		__int64 remainingBytes;
	};

	class ThreadPoolTimerNew {
	public:
		ThreadPoolTimerNew() {};
		ThreadPoolTimer threadPoolTimer{ nullptr };
	};

	class VoiceConnection : public agent {
	public:
		bool doWeQuit = false;
		uint16_t sequenceIndex = 0;
		uint32_t timestamp = 0;
		DiscordCoreInternal::VoiceConnectionData voiceConnectionData;
		shared_ptr<DiscordCoreInternal::VoiceChannelWebSocketAgent> voicechannelWebSocketAgent{ nullptr };
		shared_ptr<unbounded_buffer<AudioDataChunk>> bufferMessageBlock;
		shared_ptr<unbounded_buffer<bool>> readyBuffer;
		VoiceConnection(DiscordCoreInternal::VoiceConnectionData voiceConnectionDataNew, shared_ptr<unbounded_buffer<AudioDataChunk>> bufferMessageBlockNew) {
			if (voiceConnectionDataNew.channelId != "") {
				this->voiceConnectionData = voiceConnectionDataNew;
				this->bufferMessageBlock = bufferMessageBlockNew;
				this->readyBuffer = make_shared<unbounded_buffer<bool>>();
				this->voicechannelWebSocketAgent = make_shared<DiscordCoreInternal::VoiceChannelWebSocketAgent>(DiscordCoreInternal::ThreadManager::getThreadContext().get(), this->voiceConnectionData, this->readyBuffer);
				send(this->voicechannelWebSocketAgent->voiceConnectionDataBuffer, this->voiceConnectionData);
				this->voicechannelWebSocketAgent->start();
			}
			else {
				throw exception("Sorry, but you need to select a proper voice channel to connect to!");
			}
		}

		bool waitForReadyConnectedStatus() {
			return receive(*this->readyBuffer);
		}

		void sendSingleAudioPacket(IBuffer bufferToSend) {
			if (sodium_init() == -1) {
				cout << "LibSodium failed to initialize!" << endl << endl;
				this->terminate();
			}
			this->voiceConnectionData = this->voicechannelWebSocketAgent->voiceConnectionData;
			constexpr int headerSize = 12;
			constexpr int nonceSize = 4;
			const unsigned int frameSize = bufferToSend.Length() / 2;
			this->timestamp += 10;

			const uint8_t header[headerSize] = {
				0x80,
				0x78,
				(uint8_t)((this->sequenceIndex >> (8 * 1)) & 0xff),
				(uint8_t)((this->sequenceIndex >> (8 * 0)) & 0xff),
				(uint8_t)((this->timestamp >> (8 * 3)) & 0xff),
				(uint8_t)((this->timestamp >> (8 * 2)) & 0xff),
				(uint8_t)((this->timestamp >> (8 * 1)) & 0xff),
				(uint8_t)((this->timestamp >> (8 * 0)) & 0xff),
				(uint8_t)((this->voiceConnectionData.audioSSRC >> (8 * 3)) & 0xff),
				(uint8_t)((this->voiceConnectionData.audioSSRC >> (8 * 2)) & 0xff),
				(uint8_t)((this->voiceConnectionData.audioSSRC >> (8 * 1)) & 0xff),
				(uint8_t)((this->voiceConnectionData.audioSSRC >> (8 * 0)) & 0xff),
			};

			uint8_t nonce[nonceSize] =
			{ (uint8_t)((this->sequenceIndex >> (8 * 1)) & 0xff),
				(uint8_t)((this->sequenceIndex >> (8 * 0)) & 0xff) };

			const size_t numOfBytes = sizeof(header) + nonceSize + bufferToSend.Length() + crypto_secretbox_MACBYTES;
			uint8_t* audioDataPacket;
			audioDataPacket = new uint8_t[numOfBytes];
			std::memcpy(audioDataPacket, header, sizeof(header));

			if (crypto_secretbox_easy(audioDataPacket + sizeof(header),
				bufferToSend.data(), bufferToSend.Length(), nonce, (unsigned char*)this->voiceConnectionData.keys.data()) != 0) {
				throw exception("ENCRYPTION FAILED!");
			};

			memcpy_s(audioDataPacket + (numOfBytes - nonceSize), nonceSize, nonce, nonceSize);
			vector<uint8_t> audioDataPacketNew;
			audioDataPacketNew.resize(numOfBytes);
			for (unsigned int x = 0; x < numOfBytes; x += 1) {
				audioDataPacketNew[x] = audioDataPacket[x];
			}
			cout << endl;

			this->voicechannelWebSocketAgent->sendVoiceData(audioDataPacketNew);
			this->sequenceIndex += 1;
		}

		void sendSpeakingMessage(bool isSpeaking) {
			this->voiceConnectionData.audioSSRC = this->voicechannelWebSocketAgent->voiceConnectionData.audioSSRC;
			string newString = DiscordCoreInternal::getIsSpeakingPayload(isSpeaking, this->voiceConnectionData.audioSSRC, 0);
			this->voicechannelWebSocketAgent->sendMessage(newString);
		}

		void cleanupAndSwitchPlayerId(int* counter, __int64* totalByteSize, __int64* bytesRemaining, __int64* startingTime, vector<ThreadPoolTimerNew>* sendTimers, string* playerId, AudioDataChunk audioData) {
			*counter = 0;
			*totalByteSize = audioData.totalByteSize;
			*bytesRemaining = audioData.totalByteSize;
			*playerId = audioData.playerId;
			*startingTime = std::chrono::duration_cast<std::chrono::nanoseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
			for (auto value : *sendTimers) {
				value.threadPoolTimer.Cancel();
			}
		}

		void deleteFile(string filePath, string fileName) {
			auto folder = Windows::Storage::KnownFolders::GetFolderAsync(winrt::Windows::Storage::KnownFolderId::MusicLibrary).get();
			auto folder2 = folder.GetFolderFromPathAsync(to_hstring(filePath)).get();
			winrt::Windows::Storage::StorageFile storageFile = folder2.CreateFileAsync(to_hstring(fileName), CreationCollisionOption::ReplaceExisting).get();
			storageFile.DeleteAsync().get();
		}

		void run() {
			AudioDataChunk audioData;
			vector<ThreadPoolTimerNew> sendTimers;
			string playerId;
			int counter = 0;
			__int64 totalByteSize = 0;
			__int64 bytesRemaining = -1;
			int bufferIndex = 0;
			vector<IBuffer> bufferVector(2);
			__int64 startingTime = std::chrono::duration_cast<std::chrono::nanoseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
			while (doWeQuit == false) {
				try {
					if (bytesRemaining == -1) {
						sendSpeakingMessage(true);
					}
					if (bytesRemaining == 0) {
						sendSpeakingMessage(false);
					}
					audioData = receive(bufferMessageBlock.get(), 20000);
					if (audioData.playerId != playerId) {
						cleanupAndSwitchPlayerId(&counter, &totalByteSize, &bytesRemaining, &startingTime, &sendTimers, &playerId, audioData);
						this->timestamp = (int)startingTime;
					}
					if (counter == 0 && bufferIndex < 2) {
						bufferVector[bufferIndex] = audioData.audioData;
						bufferIndex += 1;
					}
					__int64 currentPosition = totalByteSize - bytesRemaining;
					if (audioData.audioData.Length() > 0) {
						IBuffer buffer = demuxWebA(audioData.audioData, currentPosition, totalByteSize);
						TimerElapsedHandler onSendTime = [=, this](ThreadPoolTimer timer) {
							if (buffer.Length() > 0) {
								VoiceConnection::sendSingleAudioPacket(buffer);
							}
						};
						if (buffer != nullptr) {
							__int64 lengthInNs = (__int64)((float)buffer.Length() / (float)audioData.audioBitrate / (float)2 * 1000000000);
							ThreadPoolTimer timerNew(nullptr);
							__int64 currentTime = std::chrono::duration_cast<std::chrono::nanoseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
							__int64 timeDifference = currentTime - startingTime;
							sendTimers.resize(sendTimers.size() + 1);
							sendTimers[sendTimers.size() - 1].threadPoolTimer = timerNew.CreateTimer(onSendTime, TimeSpan((counter * lengthInNs) - (timeDifference) / 100));
						}
					}
				}
				catch (exception& e) {
					cout << "Out of time: " << e.what() << endl;
					break;
				}
			}
			string newString;
			for (unsigned int x = 0; x < bufferVector[0].Length(); x += 1) {
				if (bufferVector[0].data()[x] == bufferVector[1].data()[x]) {
					newString += bufferVector[0].data()[x];
				}
				else {
					newString += to_string(0);
				}
			}
			cout << "END OF VOICECONNECTION::PLAY: " << newString << endl;
			done();
		}

		void play(bool doWeBlock = false) {
			this->start();
			if (doWeBlock) {
				wait(this);
			}
		}

		void terminate() {
			done();
			this->doWeQuit = true;
			this->voicechannelWebSocketAgent->terminate();
		}

		~VoiceConnection() {
			this->terminate();
		}
	};

}
#endif