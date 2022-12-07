/*
	DiscordCoreAPI, A bot library for Discord, written in C++, and featuring explicit multithreading through the usage of custom, asynchronous C++ CoRoutines.

	Copyright 2021, 2022 Chris M. (RealTimeChris)

	This library is free software; you can redistribute it and/or
	modify it under the terms of the GNU Lesser General Public
	License as published by the Free Software Foundation; either
	version 2.1 of the License, or (at your option) any later version.

	This library is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
	Lesser General Public License for more details.

	You should have received a copy of the GNU Lesser General Public
	License along with this library; if not, write to the Free Software
	Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301
	USA
*/
/// VoiceConnection.cpp - Source file for the voice connection class.
/// Jul 15, 2021
/// https://discordcoreapi.com
/// \file VoiceConnection.cpp

#include <discordcoreapi/VoiceConnection.hpp>
#include <discordcoreapi/DiscordCoreClient.hpp>

namespace DiscordCoreAPI {

	VoiceSocketReadyData::VoiceSocketReadyData(simdjson::ondemand::value jsonObjectData) {
		this->ip = getString(jsonObjectData, "ip");
		this->ssrc = getUint32(jsonObjectData, "ssrc");
		simdjson::ondemand::array arrayValue{};
		if (jsonObjectData["modes"].get(arrayValue) == simdjson::error_code::SUCCESS) {
			this->mode.clear();
			for (simdjson::simdjson_result<simdjson::ondemand::value> value: arrayValue) {
				if (std::string{ value.get_string().take_value() } == "xsalsa20_poly1305") {
					this->mode = std::string{ value.get_string().take_value() };
				}
			}
		}
		this->port = getUint64(jsonObjectData, "port");
	}

	VoiceUser::VoiceUser(Snowflake userIdNew) noexcept {
		this->userId = userIdNew;
	}

	VoiceUser& VoiceUser::operator=(VoiceUser&& data) noexcept {
		this->payloads = std::move(data.payloads);
		this->decoder = std::move(data.decoder);
		this->userId = data.userId;
		return *this;
	}

	VoiceUser::VoiceUser(VoiceUser&& data) noexcept {
		*this = std::move(data);
	}

	DiscordCoreInternal::OpusDecoderWrapper& VoiceUser::getDecoder() noexcept {
		return this->decoder;
	}

	void VoiceUser::insertPayload(std::basic_string_view<std::byte> data) noexcept {
		if (this->payloads.getFreeSpace() == 0) {
			this->payloads.getCurrentTail()->clear();
			this->payloads.modifyReadOrWritePosition(DiscordCoreInternal::RingBufferAccessType::Read, 1);
		}
		if (data.size() <= this->payloads.getCurrentHead()->getFreeSpace()) {
			std::copy(data.data(), data.data() + data.size(), this->payloads.getCurrentHead()->getCurrentHead());
			this->payloads.getCurrentHead()->modifyReadOrWritePosition(DiscordCoreInternal::RingBufferAccessType::Write, data.size());
			this->payloads.modifyReadOrWritePosition(DiscordCoreInternal::RingBufferAccessType::Write, 1);
		}
	}

	std::basic_string_view<std::byte> VoiceUser::extractPayload() noexcept {
		std::basic_string_view<std::byte> string{};
		if (this->payloads.getUsedSpace() > 0 && this->payloads.getCurrentTail()->getUsedSpace() > 0) {
			string = std::basic_string_view<std::byte>{ this->payloads.getCurrentTail()->getCurrentTail(),
				this->payloads.getCurrentTail()->getUsedSpace() };
			this->payloads.getCurrentTail()->clear();
			this->payloads.modifyReadOrWritePosition(DiscordCoreInternal::RingBufferAccessType::Read, 1);
		}
		return string;
	}

	Snowflake VoiceUser::getUserId() noexcept {
		return this->userId;
	}

	RTPPacketEncrypter::RTPPacketEncrypter(uint32_t ssrcNew, const std::basic_string<std::byte>& keysNew) noexcept {
		this->keys = keysNew;
		this->ssrc = ssrcNew;
	}

	std::basic_string_view<std::byte> RTPPacketEncrypter::encryptPacket(DiscordCoreInternal::EncoderReturnData& audioData) noexcept {
		if (this->keys.size() > 0) {
			++this->sequence;
			this->timeStamp += static_cast<uint32_t>(audioData.sampleCount);
			const uint8_t headerSize{ 12 };
			char header[headerSize]{};
			storeBits(header, this->version);
			storeBits(header + 1, this->flags);
			storeBits(header + 2, this->sequence);
			storeBits(header + 4, this->timeStamp);
			storeBits(header + 8, this->ssrc);
			uint8_t nonceForLibSodium[crypto_secretbox_NONCEBYTES]{};
			for (int8_t x = 0; x < headerSize; ++x) {
				nonceForLibSodium[x] = header[x];
			}
			const uint64_t numOfBytes{ headerSize + audioData.data.size() + crypto_secretbox_MACBYTES };
			if (this->data.size() < numOfBytes) {
				this->data.resize(numOfBytes);
			}
			for (int8_t x = 0; x < headerSize; ++x) {
				this->data[x] = static_cast<std::byte>(header[x]);
			}
			if (crypto_secretbox_easy(reinterpret_cast<unsigned char*>(this->data.data()) + headerSize,
					reinterpret_cast<const unsigned char*>(audioData.data.data()), audioData.data.size(), nonceForLibSodium,
					reinterpret_cast<unsigned char*>(this->keys.data())) != 0) {
				return {};
			}
			return std::basic_string_view<std::byte>{ this->data.data(), numOfBytes };
		}
		return {};
	}

	MovingAverager::MovingAverager(size_t collectionCountNew) noexcept {
		this->collectionCount = collectionCountNew;
	}

	void MovingAverager::insertValue(int64_t value) noexcept {
		this->values.emplace_front(value);
		if (this->values.size() >= this->collectionCount) {
			this->values.pop_back();
		}
	}

	double MovingAverager::getCurrentValue() noexcept {
		double returnValue{};
		if (this->values.size() > 0) {
			for (auto& value: this->values) {
				returnValue += static_cast<double>(value);
			}
			return returnValue / static_cast<double>(this->values.size());
		} else {
			return 0.0f;
		}
	}

	VoiceConnectionBridge::VoiceConnectionBridge(DiscordCoreClient* clientPtrNew, StreamType streamType, Snowflake guildIdNew)
		: UDPConnection(streamType, clientPtrNew->getConfigManager().doWePrintWebSocketErrorMessages()) {
		this->clientPtr = clientPtrNew;
		this->guildId = guildIdNew;
	}

	void VoiceConnectionBridge::parseOutgoingVoiceData() noexcept {
		std::basic_string_view<std::byte> buffer = this->getInputBuffer();
		if (buffer.size() > 0) {
			AudioFrameData frame{};
			frame += buffer;
			this->clientPtr->getSongAPI(this->guildId)->audioDataBuffer.send(std::move(frame));
		}
	}

	void VoiceConnectionBridge::handleAudioBuffer() noexcept {
		this->parseOutgoingVoiceData();
	}

	VoiceConnection::VoiceConnection(DiscordCoreClient* clientPtrNew, DiscordCoreInternal::WebSocketClient* baseShardNew,
		std::atomic_bool* doWeQuitNew) noexcept
		: WebSocketCore(&clientPtrNew->configManager, DiscordCoreInternal::WebSocketType::Voice),
		  UDPConnection(StreamType::None, clientPtrNew->configManager.doWePrintWebSocketErrorMessages()) {
		this->dataOpCode = DiscordCoreInternal::WebSocketOpCode::Op_Text;
		this->configManager = &clientPtrNew->configManager;
		this->msPerPacket = 20;
		this->samplesPerPacket = sampleRatePerSecond / 1000 * msPerPacket;
		this->discordCoreClient = clientPtrNew;
		this->downSampledVector.resize(23040);
		this->upSampledVector.resize(23040);
		this->baseShard = baseShardNew;
		this->doWeQuit = doWeQuitNew;
	}

	Snowflake VoiceConnection::getChannelId() noexcept {
		return this->voiceConnectInitData.channelId;
	}

	void VoiceConnection::applyGainRamp(int64_t sampleCount) noexcept {
		const double increment = (this->currentGain - this->previousGain) / static_cast<double>(sampleCount);
		for (int64_t x = 0; x < sampleCount / 4; ++x) {
			__m256d currentSampleRaw =
				_mm256_set_pd(static_cast<double>(this->upSampledVector[(x * 4) + 3]), static_cast<double>(this->upSampledVector[(x * 4) + 2]),
					static_cast<double>(this->upSampledVector[(x * 4) + 1]), static_cast<double>(this->upSampledVector[x * 4]));
			currentSampleRaw = _mm256_mul_pd(currentSampleRaw,
				_mm256_set_pd(this->currentGain + increment * 4, this->currentGain + increment * 3, this->currentGain + increment * 2,
					this->currentGain + increment));
			__m256d comparisonSampleMin = _mm256_set1_pd(static_cast<double>(std::numeric_limits<opus_int16>::min()));
			__m256d comparisonSampleMax = _mm256_set1_pd(static_cast<double>(std::numeric_limits<opus_int16>::max()));
			__m256d comparisonSampleZero = _mm256_set1_pd(0.0l);
			__m256d compareGreaterThanZero = _mm256_cmp_pd(currentSampleRaw, comparisonSampleZero, _CMP_GE_OQ);
			__m256d newSample = _mm256_blendv_pd(_mm256_max_pd(currentSampleRaw, comparisonSampleMin),
				_mm256_min_pd(currentSampleRaw, comparisonSampleMax), compareGreaterThanZero);
			double newSamples[4]{};
			_mm256_store_pd(newSamples, newSample);
			this->downSampledVector[x * 4] = static_cast<opus_int16>(newSamples[0]);
			this->downSampledVector[(x * 4) + 1] = static_cast<opus_int16>(newSamples[1]);
			this->downSampledVector[(x * 4) + 2] = static_cast<opus_int16>(newSamples[2]);
			this->downSampledVector[(x * 4) + 3] = static_cast<opus_int16>(newSamples[3]);
			this->currentGain += increment * 4;
		}
	}

	void VoiceConnection::parseIncomingVoiceData(std::basic_string_view<std::byte> rawDataBufferNew) noexcept {
		if (rawDataBufferNew.size() <= 39) {
			return;
		}
		const uint32_t speakerSsrc{ ntohl(*reinterpret_cast<const uint32_t*>(rawDataBufferNew.data() + 8)) };
		if (this->voiceUsers.contains(speakerSsrc)) {
			if (72 <= (static_cast<int8_t>(rawDataBufferNew[1]) & 0b0111'1111) && ((static_cast<int8_t>(rawDataBufferNew[1]) & 0b0111'1111) <= 76)) {
				return;
			}
			this->voiceUsers[speakerSsrc]->insertPayload(rawDataBufferNew);
		}
	}

	void VoiceConnection::connect(const VoiceConnectInitData& initData) noexcept {
		this->voiceConnectInitData = initData;
		this->connections = std::make_unique<ConnectionPackage>();
		this->connections->currentReconnectTries = this->currentReconnectTries;
		this->connections->currentShard = this->shard[0];
		this->activeState.store(VoiceActiveState::Connecting);
		if (!this->taskThread01) {
			this->taskThread01 = std::make_unique<std::jthread>([=, this](std::stop_token token) {
				this->runVoice(token);
			});
		}
	}

	UnboundedMessageBlock<AudioFrameData>& VoiceConnection::getAudioBuffer() noexcept {
		return this->discordCoreClient->getSongAPI(this->voiceConnectInitData.guildId)->audioDataBuffer;
	}

	void VoiceConnection::checkForAndSendHeartBeat(const bool isImmedate) noexcept {
		if (this->heartBeatStopWatch.hasTimePassed() || isImmedate) {
			Jsonifier data{};
			data["d"] = std::chrono::duration_cast<Nanoseconds>(HRClock::now().time_since_epoch()).count();
			data["op"] = 3;
			data.refreshString(JsonifierSerializeType::Json);
			std::string string{ data.operator std::string() };
			this->createHeader(string, this->dataOpCode);
			if (!this->sendMessage(string, true)) {
				this->onClosed();
				return;
			}
			this->haveWeReceivedHeartbeatAck = false;
			this->heartBeatStopWatch.resetTimer();
		}
	}

	bool VoiceConnection::onMessageReceived(std::string_view data) noexcept {
		std::string string{ data };
		string.reserve(string.size() + simdjson::SIMDJSON_PADDING);
		DiscordCoreInternal::WebSocketMessage message{};
		simdjson::ondemand::value value{};
		if (this->parser.iterate(string.data(), string.length(), string.capacity()).get(value) == simdjson::error_code::SUCCESS) {
			message = DiscordCoreInternal::WebSocketMessage{ value };
		}

		if (this->configManager->doWePrintWebSocketSuccessMessages()) {
			cout << shiftToBrightGreen() << "Message received from Voice WebSocket: " << data << reset() << endl << endl;
		}
		switch (static_cast<VoiceSocketOpCodes>(message.op)) {
			case VoiceSocketOpCodes::Ready_Server: {
				const VoiceSocketReadyData data{ value["d"] };
				this->audioSSRC = data.ssrc;
				this->voiceIp = data.ip;
				this->port = data.port;
				this->audioEncryptionMode = data.mode;
				this->connectionState.store(VoiceConnectionState::Initializing_DatagramSocket);
				break;
			}
			case VoiceSocketOpCodes::Session_Description: {
				auto arrayValue = getArray(value["d"], "secret_key");
				if (arrayValue.didItSucceed) {
					std::basic_string<std::byte> secretKey{};
					for (simdjson::ondemand::array_iterator iterator = arrayValue.arrayValue.begin(); iterator != arrayValue.arrayValue.end();
						 ++iterator) {
						secretKey.push_back(static_cast<std::byte>(iterator.operator*().get_uint64().take_value()));
					}
					this->encryptionKey = secretKey;
				}
				this->packetEncrypter = RTPPacketEncrypter{ this->audioSSRC, this->encryptionKey };
				this->connectionState.store(VoiceConnectionState::Collecting_Init_Data);
				break;
			}
			case VoiceSocketOpCodes::Speaking: {
				const uint32_t ssrc = getUint32(value["d"], "ssrc");
				std::unique_ptr<VoiceUser> user{ std::make_unique<VoiceUser>(stoull(getString(value["d"], "user_id"))) };
				if (!Users::getCachedUser({ .userId = user->getUserId() }).getFlagValue(UserFlags::Bot) ||
					this->voiceConnectInitData.streamInfo.streamBotAudio) {
					if (!this->voiceUsers.contains(ssrc)) {
						this->voiceUsers.emplace(std::make_pair(ssrc, std::move(user)));
					}
				}
				break;
			}
			case VoiceSocketOpCodes::Heartbeat_ACK: {
				this->haveWeReceivedHeartbeatAck = true;
				break;
			}
			case VoiceSocketOpCodes::Hello: {
				this->heartBeatStopWatch = StopWatch{ Milliseconds{ static_cast<uint32_t>(getFloat(value["d"], "heartbeat_interval")) } };
				this->areWeHeartBeating = true;
				this->connectionState.store(VoiceConnectionState::Sending_Identify);
				this->currentState.store(DiscordCoreInternal::WebSocketState::Authenticated);
				this->haveWeReceivedHeartbeatAck = true;
				break;
			}
			case VoiceSocketOpCodes::Resumed: {
				this->connectionState.store(VoiceConnectionState::Initializing_DatagramSocket);
				break;
			}
			case VoiceSocketOpCodes::Client_Disconnect: {
				const auto userId = stoull(getString(value["d"], "user_id"));
				for (auto& [key, value]: this->voiceUsers) {
					if (userId == value->getUserId()) {
						this->voiceUsers.erase(key);
						break;
					}
				}
				break;
			}
		}
		return true;
	}

	void VoiceConnection::sendSpeakingMessage(const bool isSpeaking) noexcept {
		DiscordCoreInternal::SendSpeakingData data{};
		if (!isSpeaking) {
			data.type = static_cast<DiscordCoreInternal::SendSpeakingType>(0);
			this->sendSilence();
			UDPConnection::processIO(DiscordCoreInternal::ProcessIOType::Write_Only);
		} else {
			data.type = DiscordCoreInternal::SendSpeakingType::Microphone;
		}
		data.delay = 0;
		data.ssrc = this->audioSSRC;
		auto serializer = data.operator Jsonifier();
		serializer.refreshString(JsonifierSerializeType::Json);
		std::string string{ serializer.operator std::string() };
		this->createHeader(string, this->dataOpCode);
		this->sendMessage(string, true);
	}

	void VoiceConnection::runVoice(std::stop_token token) noexcept {
		StopWatch stopWatch{ 20000ms };
		StopWatch sendSilenceStopWatch{ 5000ms };
		while (!token.stop_requested() && !this->doWeQuit->load() && this->activeState.load() != VoiceActiveState::Exiting) {
			switch (this->activeState.load()) {
				case VoiceActiveState::Connecting: {
					while (!token.stop_requested() && this->activeState.load() == VoiceActiveState::Connecting) {
						std::this_thread::sleep_for(1ms);
						if (!token.stop_requested() && VoiceConnection::areWeConnected()) {
							if (WebSocketCore::processIO(10) == DiscordCoreInternal::ProcessIOResult::Error) {
								this->onClosed();
							}
						}
						if (!token.stop_requested() && VoiceConnection::areWeConnected()) {
							this->checkForAndSendHeartBeat(false);
						}
						this->checkForConnections(token);
					}
					break;
				}
				case VoiceActiveState::Stopped: {
					this->areWePlaying.store(false);
					this->discordCoreClient->getSongAPI(this->voiceConnectInitData.guildId)->audioDataBuffer.clearContents();
					this->clearAudioData();
					while (!token.stop_requested() && this->activeState.load() == VoiceActiveState::Stopped) {
						UDPConnection::processIO(DiscordCoreInternal::ProcessIOType::Both);
						if (sendSilenceStopWatch.hasTimePassed()) {
							sendSilenceStopWatch.resetTimer();
							this->sendSpeakingMessage(true);
							this->sendSpeakingMessage(false);
						}
						std::this_thread::sleep_for(1ms);
						if (!token.stop_requested() && VoiceConnection::areWeConnected()) {
							if (WebSocketCore::processIO(10) == DiscordCoreInternal::ProcessIOResult::Error) {
								this->onClosed();
							}
						}
						if (!token.stop_requested() && VoiceConnection::areWeConnected()) {
							this->checkForAndSendHeartBeat(false);
						}
						this->checkForConnections(token);
					}
					break;
				}
				case VoiceActiveState::Paused: {
					this->areWePlaying.store(false);
					while (!token.stop_requested() && this->activeState.load() == VoiceActiveState::Paused) {
						UDPConnection::processIO(DiscordCoreInternal::ProcessIOType::Both);
						if (sendSilenceStopWatch.hasTimePassed()) {
							sendSilenceStopWatch.resetTimer();
							this->sendSpeakingMessage(true);
							this->sendSpeakingMessage(false);
						}
						std::this_thread::sleep_for(1ms);
						if (!token.stop_requested() && VoiceConnection::areWeConnected()) {
							if (WebSocketCore::processIO(10) == DiscordCoreInternal::ProcessIOResult::Error) {
								this->onClosed();
							}
						}
						if (!token.stop_requested() && VoiceConnection::areWeConnected()) {
							this->checkForAndSendHeartBeat(false);
						}
						this->checkForConnections(token);
					}
					break;
				}
				case VoiceActiveState::Playing: {
					this->xferAudioData.type = AudioFrameType::Unset;
					this->xferAudioData.data.clear();

					stopWatch.resetTimer();
					while (!token.stop_requested() && !UDPConnection::areWeStillConnected()) {
						if (stopWatch.hasTimePassed() || this->activeState.load() == VoiceActiveState::Exiting) {
							return;
						}
						std::this_thread::sleep_for(1ms);
						if (!token.stop_requested() && VoiceConnection::areWeConnected()) {
							if (WebSocketCore::processIO(10) == DiscordCoreInternal::ProcessIOResult::Error) {
								this->onClosed();
							}
						}
						if (!token.stop_requested() && VoiceConnection::areWeConnected()) {
							this->checkForAndSendHeartBeat(false);
						}
						this->checkForConnections(token);
					}

					auto targetTime{ HRClock::now() + this->intervalCount };
					this->sendSpeakingMessage(false);
					this->sendSpeakingMessage(true);

					while (!token.stop_requested() && this->activeState.load() == VoiceActiveState::Playing) {
						int64_t bytesPerSample{ 4 };
						if (!token.stop_requested() && VoiceConnection::areWeConnected()) {
							this->checkForAndSendHeartBeat(false);
						}
						this->checkForConnections(token);
						this->discordCoreClient->getSongAPI(this->voiceConnectInitData.guildId)->audioDataBuffer.tryReceive(this->xferAudioData);
						AudioFrameType frameType{ this->xferAudioData.type };
						if (this->xferAudioData.currentSize % 480 != 0 || this->xferAudioData.currentSize == 0) {
							this->areWePlaying.store(false);
							this->xferAudioData.clearData();
						} else {
							this->intervalCount =
								Nanoseconds{ static_cast<uint64_t>(static_cast<double>(this->xferAudioData.currentSize / bytesPerSample) /
									static_cast<double>(this->sampleRatePerSecond) * static_cast<double>(this->nsPerSecond)) };
							this->areWePlaying.store(true);
							this->audioData += this->xferAudioData;
							this->currentGuildMemberId = this->xferAudioData.guildMemberId;
						}
						std::basic_string_view<std::byte> frame{};
						bool doWeBreak{};
						switch (frameType) {
							case AudioFrameType::RawPCM: {
								if (this->audioData.getCurrentTail()->getUsedSpace() >=
									static_cast<uint64_t>(this->samplesPerPacket * bytesPerSample)) {
									auto encodedFrameData =
										this->encoder.encodeData(this->audioData.readData(this->samplesPerPacket * bytesPerSample));
									if (encodedFrameData.data.size() != 0) {
										frame = this->packetEncrypter.encryptPacket(encodedFrameData);
									}
								}
								break;
							}
							case AudioFrameType::Skip: {
								SongCompletionEventData completionEventData{};
								completionEventData.guild = Guilds::getCachedGuild({ .guildId = this->voiceConnectInitData.guildId });
								if (this->currentGuildMemberId != 0) {
									completionEventData.guildMember = GuildMembers::getCachedGuildMember(
										{ .guildMemberId = this->currentGuildMemberId, .guildId = this->voiceConnectInitData.guildId });
								}
								completionEventData.wasItAFail = false;
								DiscordCoreClient::getSongAPI(this->voiceConnectInitData.guildId)->onSongCompletionEvent(completionEventData);
								this->areWePlaying.store(false);
								doWeBreak = true;
								this->xferAudioData.type = AudioFrameType::Unset;
								this->xferAudioData.clearData();
								this->audioData.clear();
								break;
							}
							case AudioFrameType::Unset: {
								this->sendSilence();
								break;
							}
						}
						if (doWeBreak) {
							continue;
						}
						auto waitTime = targetTime - HRClock::now();
						auto waitTimeCount = waitTime.count();
						int64_t minimumFreeTimeForCheckingProcessIO{ static_cast<int64_t>(static_cast<double>(this->intervalCount.count()) * 0.70l) };
						if (waitTimeCount >= minimumFreeTimeForCheckingProcessIO && !token.stop_requested() && VoiceConnection::areWeConnected()) {
							if (WebSocketCore::processIO(1) == DiscordCoreInternal::ProcessIOResult::Error) {
								this->onClosed();
							}
						}

						waitTime = targetTime - HRClock::now();
						waitTimeCount = static_cast<int64_t>(static_cast<double>(waitTime.count()) * 0.90l);
						if (waitTimeCount > 0) {
							nanoSleep(waitTimeCount);
						}
						waitTime = targetTime - HRClock::now();
						waitTimeCount = waitTime.count();
						if (waitTimeCount > 0 && waitTimeCount < this->intervalCount.count()) {
							spinLock(waitTimeCount);
						}
						if (frame.size() > 0) {
							this->xferAudioData.clearData();
							UDPConnection::writeData(frame);
							if (UDPConnection::processIO(DiscordCoreInternal::ProcessIOType::Both) == DiscordCoreInternal::ProcessIOResult::Error) {
								this->onClosed();
							}
						} else {
							if (UDPConnection::processIO(DiscordCoreInternal::ProcessIOType::Read_Only) ==
								DiscordCoreInternal::ProcessIOResult::Error) {
								this->onClosed();
							}
						}

						targetTime = HRClock::now() + this->intervalCount;

						if (this->streamSocket && this->streamSocket->areWeStillConnected()) {
							this->mixAudio();
							if (this->streamSocket->processIO(DiscordCoreInternal::ProcessIOType::Both) ==
								DiscordCoreInternal::ProcessIOResult::Error) {
								this->onClosed();
							}
						}
					}
					break;
				}
				case VoiceActiveState::Exiting: {
					return;
				}
			}
			if (token.stop_requested() || this->activeState == VoiceActiveState::Exiting) {
				return;
			}
			std::this_thread::sleep_for(1ms);
		}
	};

	bool VoiceConnection::areWeCurrentlyPlaying() noexcept {
		return (this->areWePlaying.load() && this->activeState.load() == VoiceActiveState::Playing) ||
			this->activeState.load() == VoiceActiveState::Paused;
	}

	void VoiceConnection::checkForConnections(std::stop_token token) noexcept {
		if (this->connections) {
			this->connections.reset(nullptr);
			this->currentState.store(DiscordCoreInternal::WebSocketState::Disconnected);
			WebSocketCore::ssl = nullptr;
			if (this->voiceConnectInitData.streamInfo.type != StreamType::None) {
				WebSocketCore::outputBuffer.clear();
				WebSocketCore::inputBuffer.clear();
				UDPConnection::inputBuffer.clear();
				UDPConnection::outputBuffer.clear();
				if (this->streamSocket) {
					this->streamSocket->inputBuffer.clear();
					this->streamSocket->outputBuffer.clear();
				}
				this->discordCoreClient->getSongAPI(this->voiceConnectInitData.guildId)->audioDataBuffer.clearContents();
			}
			WebSocketCore::socket = SOCKET_ERROR;
			UDPConnection::disconnect();
			this->areWeConnecting.store(true);
			this->closeCode = 0;
			this->areWeHeartBeating = false;
			StopWatch stopWatch{ 10000ms };
			this->connectionState.store(VoiceConnectionState::Collecting_Init_Data);
			while (this->baseShard->currentState.load() != DiscordCoreInternal::WebSocketState::Authenticated && !token.stop_requested()) {
				if (stopWatch.hasTimePassed() || this->activeState.load() == VoiceActiveState::Exiting) {
					return;
				}
				std::this_thread::sleep_for(1ms);
			}
			this->connectInternal(token);
			this->sendSpeakingMessage(true);
			this->activeState.store(VoiceActiveState::Playing);
		}
	}

	void VoiceConnection::handleAudioBuffer() noexcept {
		if (this->connectionState.load() == VoiceConnectionState::Initializing_DatagramSocket) {
		} else {
			std::basic_string_view<std::byte> string = UDPConnection::getInputBuffer();
			if (this->streamSocket && this->encryptionKey.size() > 0) {
				this->parseIncomingVoiceData(string);
			}
		}
	}

	void VoiceConnection::connectInternal(std::stop_token token) noexcept {
		StopWatch stopWatch{ 10000ms };
		if (this->currentReconnectTries >= this->maxReconnectTries) {
			this->doWeQuit->store(true);
			if (this->configManager->doWePrintWebSocketErrorMessages()) {
				cout << "VoiceConnection::connect() Error: Failed to connect to voice channel!" << endl << endl;
			}
			return;
		}
		switch (this->connectionState.load()) {
			case VoiceConnectionState::Collecting_Init_Data: {
				this->baseShard->voiceConnectionDataBuffersMap[this->voiceConnectInitData.guildId.operator size_t()] =
					&this->voiceConnectionDataBuffer;
				this->baseShard->voiceConnectionDataBuffersMap[this->voiceConnectInitData.guildId.operator size_t()]->clearContents();
				this->baseShard->getVoiceConnectionData(this->voiceConnectInitData);

				if (waitForTimeToPass(this->voiceConnectionDataBuffer, this->voiceConnectionData, 10000)) {
					++this->currentReconnectTries;
					this->onClosed();
					return;
				}
				this->baseUrl = this->voiceConnectionData.endPoint.substr(0, this->voiceConnectionData.endPoint.find(":"));
				this->connectionState.store(VoiceConnectionState::Initializing_WebSocket);
				this->connectInternal(token);
				break;
			}
			case VoiceConnectionState::Initializing_WebSocket: {
				this->currentState.store(DiscordCoreInternal::WebSocketState::Upgrading);
				if (!WebSocketCore::connect(this->baseUrl, "/?v=4", 443, this->configManager->doWePrintWebSocketErrorMessages(), false)) {
					++this->currentReconnectTries;
					this->onClosed();
					return;
				}
				this->shard[0] = 0;
				this->shard[1] = 1;
				while (this->currentState.load() != DiscordCoreInternal::WebSocketState::Collecting_Hello && !token.stop_requested()) {
					if (WebSocketCore::processIO(10) == DiscordCoreInternal::ProcessIOResult::Error) {
						++this->currentReconnectTries;
						this->onClosed();
						return;
					}
				}
				this->connectionState.store(VoiceConnectionState::Collecting_Hello);
				this->connectInternal(token);
				break;
			}
			case VoiceConnectionState::Collecting_Hello: {
				stopWatch.resetTimer();
				while (this->connectionState.load() != VoiceConnectionState::Sending_Identify && !token.stop_requested()) {
					if (stopWatch.hasTimePassed()) {
						++this->currentReconnectTries;
						this->onClosed();
						return;
					}
					if (WebSocketCore::processIO(100) == DiscordCoreInternal::ProcessIOResult::Error) {
						++this->currentReconnectTries;
						this->onClosed();
						return;
					}
					std::this_thread::sleep_for(1ms);
				}
				this->currentReconnectTries = 0;
				this->connectInternal(token);
				break;
			}
			case VoiceConnectionState::Sending_Identify: {
				this->haveWeReceivedHeartbeatAck = true;
				DiscordCoreInternal::VoiceIdentifyData data{};
				data.connectInitData = this->voiceConnectInitData;
				data.connectionData = this->voiceConnectionData;
				auto serializer = data.operator Jsonifier();
				serializer.refreshString(JsonifierSerializeType::Json);
				std::string string{ serializer.operator std::string() };
				this->createHeader(string, this->dataOpCode);
				if (!WebSocketCore::sendMessage(string, true)) {
					++this->currentReconnectTries;
					this->onClosed();
					return;
				}
				this->connectionState.store(VoiceConnectionState::Collecting_Ready);
				this->connectInternal(token);
				break;
			}
			case VoiceConnectionState::Collecting_Ready: {
				stopWatch.resetTimer();
				while (this->connectionState.load() != VoiceConnectionState::Initializing_DatagramSocket && !token.stop_requested()) {
					if (stopWatch.hasTimePassed()) {
						++this->currentReconnectTries;
						this->onClosed();
						return;
					}
					if (WebSocketCore::processIO(100) == DiscordCoreInternal::ProcessIOResult::Error) {
						++this->currentReconnectTries;
						this->onClosed();
						return;
					}
					std::this_thread::sleep_for(1ms);
				}
				this->connectInternal(token);
				break;
			}
			case VoiceConnectionState::Initializing_DatagramSocket: {
				if (!this->voiceConnect()) {
					++this->currentReconnectTries;
					this->onClosed();
					return;
				}
				this->connectionState.store(VoiceConnectionState::Sending_Select_Protocol);
				this->connectInternal(token);
				break;
			}
			case VoiceConnectionState::Sending_Select_Protocol: {
				DiscordCoreInternal::VoiceSocketProtocolPayloadData data{};
				data.voiceEncryptionMode = this->audioEncryptionMode;
				data.externalIp = this->externalIp;
				data.voicePort = this->port;
				auto serializer = data.operator Jsonifier();
				serializer.refreshString(JsonifierSerializeType::Json);
				std::string string{ serializer.operator std::string() };
				this->createHeader(string, this->dataOpCode);
				if (!WebSocketCore::sendMessage(string, true)) {
					++this->currentReconnectTries;
					this->onClosed();
					return;
				}
				this->connectionState.store(VoiceConnectionState::Collecting_Session_Description);
				this->connectInternal(token);
				break;
			}
			case VoiceConnectionState::Collecting_Session_Description: {
				stopWatch.resetTimer();
				while (this->connectionState.load() != VoiceConnectionState::Collecting_Init_Data && !token.stop_requested()) {
					if (stopWatch.hasTimePassed()) {
						++this->currentReconnectTries;
						;
						this->onClosed();
						return;
					}
					if (WebSocketCore::processIO(100) == DiscordCoreInternal::ProcessIOResult::Error) {
						++this->currentReconnectTries;
						;
						this->onClosed();
						return;
					}
					std::this_thread::sleep_for(1ms);
				}
				this->baseShard->voiceConnectionDataBuffersMap[this->voiceConnectInitData.guildId.operator size_t()]->clearContents();
				this->connectionState.store(VoiceConnectionState::Collecting_Init_Data);
				if (this->voiceConnectInitData.streamInfo.type != StreamType::None) {
					if (!this->streamSocket) {
						this->streamSocket = std::make_unique<VoiceConnectionBridge>(this->discordCoreClient,
							this->voiceConnectInitData.streamInfo.type, this->voiceConnectInitData.guildId);
					}
					if (!this->streamSocket->connect(this->voiceConnectInitData.streamInfo.address, this->voiceConnectInitData.streamInfo.port, false,
							token)) {
						++this->currentReconnectTries;
						this->onClosed();
						return;
					}
				}
				this->areWeConnecting.store(false);
				this->activeState.store(VoiceActiveState::Playing);
				this->play();
				return;
			}
		}
	}

	void VoiceConnection::clearAudioData() noexcept {
		if (this->xferAudioData.data.size() != 0) {
			this->xferAudioData.clearData();
		}
		this->discordCoreClient->getSongAPI(this->voiceConnectInitData.guildId)->audioDataBuffer.clearContents();
	}

	bool VoiceConnection::areWeConnected() noexcept {
		return WebSocketCore::areWeStillConnected() && UDPConnection::areWeStillConnected();
	}

	bool VoiceConnection::voiceConnect() noexcept {
		if (!UDPConnection::areWeStillConnected() && UDPConnection::connect(this->voiceIp, this->port, false)) {
			std::byte packet[74]{};
			const uint16_t val1601{ 0x01 };
			const uint16_t val1602{ 70 };
			packet[0] = static_cast<std::byte>(val1601 >> 8);
			packet[1] = static_cast<std::byte>(val1601 >> 0);
			packet[2] = static_cast<std::byte>(val1602 >> 8);
			packet[3] = static_cast<std::byte>(val1602 >> 0);
			packet[4] = static_cast<std::byte>(this->audioSSRC >> 24);
			packet[5] = static_cast<std::byte>(this->audioSSRC >> 16);
			packet[6] = static_cast<std::byte>(this->audioSSRC >> 8);
			packet[7] = static_cast<std::byte>(this->audioSSRC);
			UDPConnection::getInputBuffer();
			UDPConnection::writeData(std::basic_string_view<std::byte>{ packet, std::size(packet) });
			std::basic_string_view<std::byte> inputStringFirst{};
			std::basic_string<std::byte> inputString{};

			StopWatch stopWatch{ 5500ms };
			while (inputStringFirst.size() < 74 && !this->doWeQuit->load() && this->activeState.load() != VoiceActiveState::Exiting) {
				UDPConnection::processIO(DiscordCoreInternal::ProcessIOType::Both);
				inputStringFirst = UDPConnection::getInputBuffer();
				std::this_thread::sleep_for(1ms);
				if (stopWatch.hasTimePassed()) {
					return false;
				}
			}
			inputString.insert(inputString.begin(), inputStringFirst.begin(), inputStringFirst.end());
			inputString = inputString.substr(8);
			const auto endLineFind = inputString.find(static_cast<std::byte>('\u0000'), 6);
			if (endLineFind != std::string::npos) {
				inputString = inputString.substr(0, endLineFind);
			}
			this->externalIp = std::string{ reinterpret_cast<const char*>(inputStringFirst.data()) + 8, inputString.size() };
			this->voiceConnectionDataBuffer.clearContents();
			return true;
		} else {
			return false;
		}
	}

	void VoiceConnection::sendSilence() noexcept {
		std::vector<std::basic_string<std::byte>> frames{};
		std::byte arrayNew[3]{};
		arrayNew[0] = std::byte{ 0xf8 };
		arrayNew[1] = std::byte{ 0xff };
		arrayNew[2] = std::byte{ 0xfe };
		for (size_t x = 0; x < 5; ++x) {
			DiscordCoreInternal::EncoderReturnData frame{};
			frame.data = arrayNew;
			frame.sampleCount = 3;
			auto packetNew = this->packetEncrypter.encryptPacket(frame);
			frames.push_back(std::basic_string<std::byte>{ packetNew.data(), packetNew.size() });
		}
		for (auto& value: frames) {
			UDPConnection::writeData(value);
			UDPConnection::processIO(DiscordCoreInternal::ProcessIOType::Both);
		}
	}

	void VoiceConnection::pauseToggle() noexcept {
		if (this->activeState.load() == VoiceActiveState::Paused) {
			this->activeState.store(VoiceActiveState::Playing);
		} else {
			this->activeState.store(VoiceActiveState::Paused);
		}
	}

	void VoiceConnection::disconnect() noexcept {
		this->activeState.store(VoiceActiveState::Exiting);
		std::string payload = "\x03\xE8";
		this->createHeader(payload, DiscordCoreInternal::WebSocketOpCode::Op_Close);
		WebSocketCore::writeData(payload, true);
		UDPConnection::disconnect();
		WebSocketCore::ssl = nullptr;
		WebSocketCore::outputBuffer.clear();
		WebSocketCore::inputBuffer.clear();
		WebSocketCore::socket = SOCKET_ERROR;
		if (this->taskThread01) {
			this->taskThread01->request_stop();
			if (this->taskThread01->joinable()) {
				this->taskThread01->join();
				this->taskThread01.reset(nullptr);
			}
		}
		if (this->streamSocket) {
			this->streamSocket->disconnect();
		}
		if (DiscordCoreClient::getSongAPI(this->voiceConnectInitData.guildId)) {
			DiscordCoreClient::getSongAPI(this->voiceConnectInitData.guildId)
				->onSongCompletionEvent.remove(DiscordCoreClient::getSongAPI(this->voiceConnectInitData.guildId)->eventToken);
		}
		this->closeCode = 0;
		this->areWeHeartBeating = false;
		this->currentReconnectTries = 0;
		this->voiceUsers.clear();
		this->activeState.store(VoiceActiveState::Connecting);
		this->areWeConnecting.store(true);
		this->connectionState.store(VoiceConnectionState::Collecting_Init_Data);
		this->currentState.store(DiscordCoreInternal::WebSocketState::Disconnected);
		this->discordCoreClient->getSongAPI(this->voiceConnectInitData.guildId)->audioDataBuffer.clearContents();
	}

	void VoiceConnection::reconnect() noexcept {
		++this->currentReconnectTries;
		this->connections = std::make_unique<ConnectionPackage>();
		this->connections->currentReconnectTries = this->currentReconnectTries;
		this->connections->currentShard = this->shard[0];
	}

	void VoiceConnection::onClosed() noexcept {
		this->connectionState.store(VoiceConnectionState::Collecting_Init_Data);
		if (this->activeState.load() != VoiceActiveState::Exiting && this->currentReconnectTries < this->maxReconnectTries) {
			this->reconnect();
		} else if (this->currentReconnectTries >= this->maxReconnectTries) {
			VoiceConnection::disconnect();
		}
	}

	void VoiceConnection::mixAudio() noexcept {
		opus_int32 voiceUserCountReal{};
		int64_t decodedSize{};
		std::fill(this->upSampledVector.data(), this->upSampledVector.data() + this->upSampledVector.size(), 0);
		for (auto& [key, value]: this->voiceUsers) {
			UDPConnection::processIO(DiscordCoreInternal::ProcessIOType::Read_Only);
			std::basic_string_view<std::byte> payload{ value->extractPayload() };
			if (payload.size() == 0) {
				continue;
			} else {
				const uint64_t headerSize{ 12 };
				const uint64_t csrcCount{ static_cast<uint64_t>(payload[0]) & 0b0000'1111 };
				const uint64_t offsetToData{ headerSize + sizeof(uint32_t) * csrcCount };
				const uint64_t encryptedDataLength{ payload.size() - offsetToData };

				if (this->decryptedDataString.size() < encryptedDataLength) {
					this->decryptedDataString.resize(encryptedDataLength);
				}

				std::byte nonce[24]{};
				for (int32_t x = 0; x < headerSize; ++x) {
					nonce[x] = payload[x];
				}

				if (crypto_secretbox_open_easy(reinterpret_cast<uint8_t*>(this->decryptedDataString.data()),
						reinterpret_cast<const uint8_t*>(payload.data()) + offsetToData, encryptedDataLength, reinterpret_cast<uint8_t*>(nonce),
						reinterpret_cast<uint8_t*>(this->encryptionKey.data()))) {
					continue;
				}

				std::basic_string_view newString{ this->decryptedDataString.data(), encryptedDataLength - crypto_secretbox_MACBYTES };

				if (static_cast<int8_t>(payload[0] >> 4) & 0b0001) {
					const uint16_t extenstionLengthInWords{ ntohs(*reinterpret_cast<const uint16_t*>(&newString[2])) };
					const size_t extensionLength{ sizeof(uint32_t) * extenstionLengthInWords };
					const size_t extensionHeaderLength{ sizeof(uint16_t) * 2 };
					newString = newString.substr(extensionHeaderLength + extensionLength);
				}

				if (newString.size() > 0) {
					std::basic_string_view<opus_int16> decodedData{};
					try {
						decodedData = value->getDecoder().decodeData(newString);
					} catch (...) {
						reportException("VoiceConnection::mixAudio()");
					}
					if (decodedData.size() > 0) {
						decodedSize = std::max(decodedSize, static_cast<int64_t>(decodedData.size()));
						++voiceUserCountReal;
						for (int32_t x = 0; x < decodedData.size(); ++x) {
							this->upSampledVector[x] += static_cast<opus_int32>(decodedData[x]);
						}
					}
				}
			}
		}
		if (decodedSize > 0) {
			this->voiceUserCountAverage.insertValue(voiceUserCountReal);
			auto currentPreviousGain = 1.0f / this->voiceUserCountAverage.getCurrentValue();
			this->currentGain = currentPreviousGain;
			this->applyGainRamp(decodedSize);
			this->streamSocket->writeData(std::basic_string_view<std::byte>{ reinterpret_cast<std::byte*>(this->downSampledVector.data()),
				static_cast<size_t>(decodedSize * 2) });
			this->previousGain = currentPreviousGain;
		}
	}

	bool VoiceConnection::stop() noexcept {
		this->sendSpeakingMessage(false);
		this->activeState.store(VoiceActiveState::Stopped);
		return true;
	}

	bool VoiceConnection::play() noexcept {
		this->activeState.store(VoiceActiveState::Playing);
		return true;
	}

	VoiceConnection::~VoiceConnection() {
		this->disconnect();
	}

}