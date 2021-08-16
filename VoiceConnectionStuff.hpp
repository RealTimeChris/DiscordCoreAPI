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
				this->repacketizer = opus_repacketizer_create();
				if (!repacketizer) {
					cout << "Failed to create Opus repacketizer!";
				}
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
		OpusEncoder* encoder;
		OpusRepacketizer* repacketizer;
		int nChannels = 2;
		const int encodedBufferSize00 = 65536;
		uint8_t encodeBuffer[65536];

		vector<uint8_t> repacketizeSingleAudioFrame(vector<vector<uint8_t>> inputFrames) {
			int outDataSize = 0;
			int mEncFrameSize = 960;
			int mEncFrameBytes = mEncFrameSize * 4;
			
			int inDataSize = 0;
			vector<uint8_t> returnVector;
			returnVector.resize(this->encodedBufferSize00);
			for (auto value : inputFrames) {
				inDataSize += (int)value.size();
			}
			uint8_t* input = new uint8_t[inDataSize];
			if (0 == (inDataSize % mEncFrameBytes)) {
				bool isOk = true;
				size_t cur = 0;
				uint8_t* out = returnVector.data();

				memset(out, 0, sizeof(encodeBuffer));
				repacketizer = opus_repacketizer_init(repacketizer);
				for (size_t i = 0; i < (inDataSize / mEncFrameBytes); ++i) {
					int ret;
					int retval = opus_repacketizer_cat(repacketizer, out, ret);
					if (retval != OPUS_OK) {
						isOk = false;
						cout << "opus_repacketizer_cat() : {}" << endl;
						break;
					}
					out += ret;
					cur += ret;
				}
				if (isOk) {
					int ret = opus_repacketizer_out(repacketizer, returnVector.data(), 65536);
					if (ret > 0) {
						outDataSize = ret;
					}
					else {
						cout << "opus_repacketizer_out(): {}" << endl;
					}
				}
			}
			else {
				throw std::runtime_error(format("Invalid input data length: {}, must be n times of {}", inDataSize, mEncFrameBytes));
			}
			return returnVector;
		}

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
			constexpr int nonceSize = crypto_secretbox_NONCEBYTES;
			uint8_t header01[2]{ 0x80 , 0x78 };
			uint16_t header02[1]{ this->sequenceIndex };
			uint32_t header03[2]{ (uint32_t)this->timestamp, (uint32_t)this->voiceConnectionData.audioSSRC };

			cout << "HEADER 00" << std::dec << header01[0] << endl;
			cout << "HEADER 01" << std::dec << header01[1] << endl;
			cout << "HEADER 02" << std::dec << (uint16_t)header02[0] << endl;
			cout << "HEADER 04" << std::dec << (uint32_t)header03[0] << endl;
			cout << "HEADER 08" << std::dec << (uint32_t)header03[1] << endl;

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

			cout << "BUFFER SIZE PRE: " << bufferToSend.size() << endl;
			size_t numOfBytes = headerSize + bufferToSend.size() + crypto_secretbox_MACBYTES;
			uint8_t* audioDataPacket = new uint8_t[numOfBytes];
			std::memcpy(audioDataPacket, headerFinal, headerSize);
			unsigned char* encryptionKeys = new unsigned char[crypto_secretbox_KEYBYTES];
			unsigned char* bufferToSendNew = new unsigned char[bufferToSend.size()];
			cout << "KEYS SIZE: " << crypto_secretbox_KEYBYTES << sizeof(encryptionKeys) * sizeof(unsigned char) << endl;
			cout << "BUFFER SIZE POST: " << numOfBytes << endl;
			for (unsigned int x = 0; x < bufferToSend.size(); x += 1) {
				bufferToSendNew[x] = bufferToSend[x];
				cout << hex << bufferToSendNew[x];
			}
			for (unsigned int x = 0; x < this->voiceConnectionData.keys.size(); x += 1) {
				encryptionKeys[x] = (unsigned char)this->voiceConnectionData.keys[x];
			}
			if (crypto_secretbox_easy(audioDataPacket + headerSize,
				bufferToSendNew, bufferToSend.size(), nonceForLibSodium, encryptionKeys) != 0) {
				throw exception("ENCRYPTION FAILED!");
			};
			vector<uint8_t> audioDataPacketNew;
			for (unsigned int x = 0; x < numOfBytes; x += 1) {
				audioDataPacketNew.push_back(audioDataPacket[x]);
			}
			this->voicechannelWebSocketAgent->sendVoiceData(audioDataPacketNew);
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
				this->timestamp = (unsigned  int)std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
				while (this->areWePlaying) {
					count = (int)std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count() - startingCount;
					if (count >= 20) {
						startingCount = (unsigned int)std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
						auto newAudioData = repacketizeSingleAudioFrame(audioData.audioData);
						sendSingleAudioQuantum(newAudioData);
						currentPosition += 1;
						if (currentPosition == audioData.audioData.size() - 1) {
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
