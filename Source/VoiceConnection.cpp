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

	void OpusDecoderWrapper::OpusDecoderDeleter::operator()(OpusDecoder* other) noexcept {
		if (other) {
			opus_decoder_destroy(other);
			other = nullptr;
		}
	}

	OpusDecoderWrapper::OpusDecoderWrapper() {
		int32_t error{};
		this->data.resize(23040);
		this->ptr.reset(opus_decoder_create(48000, 2, &error));
		if (error != OPUS_OK) {
			throw std::runtime_error{ "Failed to create the Opus Decoder" };
		}
	}

	std::basic_string_view<opus_int16> OpusDecoderWrapper::decodeData(const std::basic_string_view<uint8_t> dataToDecode) {
		const int64_t sampleCount =
			opus_decode(this->ptr.get(), dataToDecode.data(), static_cast<opus_int32>(dataToDecode.length() & 0x7FFFFFFF), data.data(), 5760, 0);
		if (sampleCount > 0) {
			return std::basic_string_view<opus_int16>{ this->data.data(), static_cast<size_t>(sampleCount * 2ull) };
		} else {
			throw std::runtime_error{ "Failed to decode a user's voice payload, Reason: " + std::to_string(sampleCount) };
		}
	}

	MovingAverager::MovingAverager(size_t periodCountNew) noexcept : periodCount{ periodCountNew } {
	}

	void MovingAverager::addValue(int64_t value) {
		this->values.emplace_back(value);
		if (this->values.size() > this->periodCount) {
			this->values.pop_front();
		}
	}

	int64_t MovingAverager::collectAverage() {
		if (this->values.size() == 0) {
			return 0;
		}
		int64_t returnValue{};
		for (auto& value: this->values) {
			returnValue += value;
		}
		returnValue /= this->values.size();
		return returnValue;
	}

	VoiceUser& VoiceUser::operator=(VoiceUser&& data) noexcept {
		this->wereWeEnding.store(data.wereWeEnding.load());
		this->payloads = std::move(data.payloads);
		this->decoder = std::move(data.decoder);
		this->userId = data.userId;
		return *this;
	}

	VoiceUser::VoiceUser(VoiceUser&& data) noexcept {
		*this = std::move(data);
	}

	void VoiceUser::insertPayload(std::basic_string<uint8_t>&& data) {
		this->payloads.send(data);
	}

	std::basic_string<uint8_t> VoiceUser::extractPayload() {
		StopWatch stopWatch{ 15000us };
		std::basic_string<uint8_t> value{};
		if (this->wereWeEnding.load()) {
			this->payloads.tryReceive(value);
		} else {
			stopWatch.resetTimer();
			while (!this->payloads.tryReceive(value)) {
				if (stopWatch.hasTimePassed()) {
					break;
				}
				std::this_thread::sleep_for(1us);
			}
		}
		return value;
	}

	OpusDecoderWrapper& VoiceUser::getDecoder() {
		return this->decoder;
	}

	void VoiceUser::setEndingStatus(bool data) {
		this->wereWeEnding.store(data);
	}

	void VoiceUser::setUserId(Snowflake userIdNew) {
		this->userId = userIdNew;
	}

	bool VoiceUser::getEndingStatus() {
		return this->wereWeEnding.load();
	}

	Snowflake VoiceUser::getUserId() {
		return this->userId;
	}

	RTPPacketEncrypter::RTPPacketEncrypter(uint32_t ssrcNew, const std::vector<uint8_t>& keysNew) noexcept {
		this->keys = keysNew;
		this->ssrc = ssrcNew;
	}

	std::basic_string_view<uint8_t> RTPPacketEncrypter::encryptPacket(const AudioFrameData& audioData) noexcept {
		if (this->keys.size() > 0) {
			this->sequence++;
			this->timeStamp += audioData.sampleCount;
			constexpr uint8_t headerSize{ 12 };
			char header[headerSize]{};
			storeBits(header, this->version);
			storeBits(header + 1, this->flags);
			storeBits(header + 2, this->sequence);
			storeBits(header + 4, this->timeStamp);
			storeBits(header + 8, this->ssrc);
			uint8_t nonceForLibSodium[crypto_secretbox_NONCEBYTES]{};
			for (uint8_t x = 0; x < headerSize; ++x) {
				nonceForLibSodium[x] = header[x];
			}
			const uint64_t numOfBytes{ headerSize + audioData.data.size() + crypto_secretbox_MACBYTES };
			if (this->data.size() < numOfBytes) {
				this->data.resize(numOfBytes);
			}
			for (uint8_t x = 0; x < headerSize; ++x) {
				this->data[x] = header[x];
			}
			if (crypto_secretbox_easy(this->data.data() + headerSize, audioData.data.data(), audioData.data.size(), nonceForLibSodium,
					this->keys.data()) != 0) {
				return {};
			}
			return std::basic_string_view<uint8_t>{ this->data.data(), numOfBytes };
		}
		return {};
	}

	VoiceConnectionBridge::VoiceConnectionBridge(DiscordCoreClient* clientPtrNew, StreamType streamType, Snowflake guildIdNew)
		: DatagramSocketClient(streamType, clientPtrNew->getConfigManager().doWePrintWebSocketErrorMessages()) {
		this->clientPtr = clientPtrNew;
		this->guildId = guildIdNew;
	}

	void VoiceConnectionBridge::parseOutGoingVoiceData() noexcept {
		const std::basic_string_view<uint8_t> buffer = this->getInputBuffer();
		if (buffer.size() > 0) {
			AudioFrameData frame{};
			frame.data.insert(frame.data.begin(), buffer.begin(), buffer.end());
			frame.sampleCount = buffer.size() / 2 / 2;
			frame.type = AudioFrameType::RawPCM;
			this->clientPtr->getSongAPI(this->guildId)->audioDataBuffer.send(std::move(frame));
		} else {
			AudioFrameData newFrame{};
			newFrame.data.push_back(0xf8);
			newFrame.data.push_back(0xff);
			newFrame.data.push_back(0xfe);
			newFrame.type = AudioFrameType::Encoded;
			this->clientPtr->getSongAPI(this->guildId)->audioDataBuffer.send(std::move(newFrame));
		}
	}

	void VoiceConnectionBridge::handleAudioBuffer() noexcept {
		this->parseOutGoingVoiceData();
	}

	VoiceConnection::VoiceConnection(DiscordCoreClient* clientPtrNew, DiscordCoreInternal::WebSocketSSLShard* baseShardNew,
		std::atomic_bool* doWeQuitNew) noexcept
		: WebSocketCore(&clientPtrNew->configManager, DiscordCoreInternal::WebSocketType::Voice),
		  DatagramSocketClient(StreamType::None, clientPtrNew->configManager.doWePrintWebSocketErrorMessages()) {
		this->dataOpCode = DiscordCoreInternal::WebSocketOpCode::Op_Text;
		this->configManager = &clientPtrNew->configManager;
		this->discordCoreClient = clientPtrNew;
		this->downSampledVector.resize(23040);
		this->upSampledVector.resize(23040);
		this->baseShard = baseShardNew;
		this->doWeQuit = doWeQuitNew;
	}

	Snowflake VoiceConnection::getChannelId() noexcept {
		return this->voiceConnectInitData.channelId;
	}

	void VoiceConnection::parseIncomingVoiceData(const std::basic_string_view<uint8_t> rawDataBufferNew) noexcept {
		std::unique_lock lock{ this->voiceUserMutex };
		const uint32_t speakerSsrc{ ntohl(*reinterpret_cast<const uint32_t*>(rawDataBufferNew.data() + 8)) };
		if (this->voiceUsers.contains(speakerSsrc)) {
			if (rawDataBufferNew.size() <= 39) {
				this->voiceUsers[speakerSsrc].setEndingStatus(true);
				return;
			} else {
				this->voiceUsers[speakerSsrc].setEndingStatus(false);
			}
			if (72 <= (rawDataBufferNew[1] & 0b0111'1111) && (rawDataBufferNew[1] & 0b0111'1111) <= 76) {
				return;
			}
			this->voiceUsers[speakerSsrc].insertPayload(static_cast<std::basic_string<uint8_t>>(rawDataBufferNew));
		}
	}

	void VoiceConnection::sendVoiceData(const std::basic_string_view<uint8_t> responseData) noexcept {
		if (responseData.size() == 0) {
			if (this->configManager->doWePrintWebSocketErrorMessages()) {
				cout << shiftToBrightRed() << "Please specify voice data to send" << reset() << endl << endl;
			}
			return;
		} else {
			if (DatagramSocketClient::areWeStillConnected()) {
				DatagramSocketClient::writeData(responseData);
			}
		}
	}

	void VoiceConnection::connect(const DiscordCoreAPI::VoiceConnectInitData& initData) noexcept {
		this->voiceConnectInitData = initData;
		this->connections = std::make_unique<ConnectionPackage>();
		this->connections->currentReconnectTries = this->currentReconnectTries;
		this->connections->currentShard = this->shard[0];
		this->activeState.store(VoiceActiveState::Connecting);
		if (!this->taskThread01) {
			this->taskThread01 = std::make_unique<std::jthread>([=, this](std::stop_token stopToken) {
				this->runVoice(stopToken);
			});
		}
	}

	UnboundedMessageBlock<AudioFrameData>& VoiceConnection::getAudioBuffer() noexcept {
		return this->discordCoreClient->getSongAPI(this->voiceConnectInitData.guildId)->audioDataBuffer;
	}

	void VoiceConnection::checkForAndSendHeartBeat(const bool isImmedate) noexcept {
		if (this->heartBeatStopWatch.hasTimePassed() || isImmedate) {
			DiscordCoreAPI::Jsonifier data{};
			data["d"] = std::chrono::duration_cast<std::chrono::nanoseconds>(std::chrono::steady_clock::now().time_since_epoch()).count();
			data["op"] = 3;
			data.refreshString(DiscordCoreAPI::JsonifierSerializeType::Json);
			std::string string = this->prepMessageData(data.operator std::string(), this->dataOpCode);
			if (!this->sendMessage(string, true)) {
				return;
			}
			this->haveWeReceivedHeartbeatAck = false;
			this->heartBeatStopWatch.resetTimer();
		}
	}

	bool VoiceConnection::onMessageReceived(const std::string_view data) noexcept {
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
		if (message.op != 0) {
			switch (message.op) {
				case 2: {
					const VoiceSocketReadyData data{ value["d"] };
					this->audioSSRC = data.ssrc;
					this->voiceIp = data.ip;
					this->port = data.port;
					this->audioEncryptionMode = data.mode;
					this->connectionState.store(VoiceConnectionState::Initializing_DatagramSocket);
					break;
				}
				case 4: {
					auto arrayValue = getArray(value["d"], "secret_key");
					if (arrayValue.didItSucceed) {
						std::vector<uint8_t> secretKey{};
						for (auto iterator: arrayValue.arrayValue) {
							secretKey.push_back(static_cast<uint8_t>(iterator.get_uint64().take_value()));
						}
						this->encryptionKey = secretKey;
					}
					this->packetEncrypter = RTPPacketEncrypter{ this->audioSSRC, this->encryptionKey };
					this->connectionState.store(VoiceConnectionState::Collecting_Init_Data);
					break;
				}
				case 5: {
					const uint32_t ssrc = getUint32(value["d"], "ssrc");
					VoiceUser user{};
					user.setUserId(stoull(getString(value["d"], "user_id")));
					std::unique_lock lock{ this->voiceUserMutex };
					if (!Users::getCachedUser({ .userId = user.getUserId() }).getFlagValue(UserFlags::Bot) ||
						this->voiceConnectInitData.streamInfo.streamBotAudio) {
						if (!this->voiceUsers.contains(ssrc)) {
							this->voiceUsers[ssrc] = std::move(user);
						}
					}
					break;
				}
				case 6: {
					this->haveWeReceivedHeartbeatAck = true;
					break;
				}
				case 8: {
					this->heartBeatStopWatch =
						StopWatch{ std::chrono::milliseconds{ static_cast<uint32_t>(getFloat(value["d"], "heartbeat_interval")) } };
					this->areWeHeartBeating = true;
					this->connectionState.store(VoiceConnectionState::Sending_Identify);
					this->currentState.store(DiscordCoreInternal::WebSocketState::Authenticated);
					this->haveWeReceivedHeartbeatAck = true;
					break;
				}
				case 9: {
					this->connectionState.store(VoiceConnectionState::Initializing_DatagramSocket);
					break;
				}
				case 13: {
					const auto userId = stoull(getString(value["d"], "user_id"));
					std::unique_lock lock{ this->voiceUserMutex };
					for (auto& [key, value]: this->voiceUsers) {
						if (userId == value.getUserId()) {
							this->voiceUsers.erase(key);
							break;
						}
					}
					break;
				}
			}
		}
		return true;
	}

	void VoiceConnection::sendSingleFrame(AudioFrameData& frameData) noexcept {
		this->discordCoreClient->getSongAPI(this->voiceConnectInitData.guildId)->audioDataBuffer.send(std::move(frameData));
	}

	void VoiceConnection::sendSpeakingMessage(const bool isSpeaking) noexcept {
		DiscordCoreInternal::SendSpeakingData data{};
		if (!isSpeaking) {
			data.type = static_cast<DiscordCoreInternal::SendSpeakingType>(0);
			this->sendSilence();
			DatagramSocketClient::processIO(DiscordCoreInternal::ProcessIOType::Write_Only);
		} else {
			data.type = DiscordCoreInternal::SendSpeakingType::Microphone;
		}
		data.delay = 0;
		data.ssrc = this->audioSSRC;
		auto serializer = data.operator DiscordCoreAPI::Jsonifier();
		serializer.refreshString(JsonifierSerializeType::Json);
		std::string string = this->prepMessageData(serializer.operator std::string(), DiscordCoreInternal::WebSocketOpCode::Op_Text);
		this->sendMessage(string, true);
	}

	void VoiceConnection::runBridge(std::stop_token token) noexcept {
		StopWatch processAndSleepStopWatch{ 20000us };
		this->sendSilence();
		MovingAverager averager{ 20 };
		DatagramSocketClient::processIO(DiscordCoreInternal::ProcessIOType::Both);
		while (!token.stop_requested()) {
			processAndSleepStopWatch.resetTimer();
			if (processAndSleepStopWatch.getTotalWaitTime() - (averager.collectAverage()) >= 0) {
				std::this_thread::sleep_for(std::chrono::microseconds{ processAndSleepStopWatch.getTotalWaitTime() - (averager.collectAverage()) });
			}
			this->mixAudio();
			this->streamSocket->processIO(DiscordCoreInternal::ProcessIOType::Both);
			averager.addValue(processAndSleepStopWatch.totalTimePassed());
			while (!processAndSleepStopWatch.hasTimePassed()) {
			}
			processAndSleepStopWatch.resetTimer();
		}
	}

	void VoiceConnection::runVoice(std::stop_token stopToken) noexcept {
		StopWatch stopWatch{ 20000ms };
		StopWatch sendSilenceStopWatch{ 5000ms };
		while (!stopToken.stop_requested() && !this->doWeQuit->load() && this->activeState.load() != VoiceActiveState::Exiting) {
			switch (this->activeState.load()) {
				case VoiceActiveState::Connecting: {
					while (!stopToken.stop_requested() && this->activeState.load() == VoiceActiveState::Connecting) {
						std::this_thread::sleep_for(1ms);
						if (!stopToken.stop_requested() && VoiceConnection::areWeConnected()) {
							if (WebSocketCore::processIO(10) == DiscordCoreInternal::ProcessIOResult::Error) {
								this->onClosed();
							}
						}
						if (!stopToken.stop_requested() && VoiceConnection::areWeConnected()) {
							this->checkForAndSendHeartBeat(false);
						}
						this->checkForConnections();
					}
					break;
				}
				case VoiceActiveState::Stopped: {
					this->areWePlaying.store(false);
					this->discordCoreClient->getSongAPI(this->voiceConnectInitData.guildId)->audioDataBuffer.clearContents();
					this->clearAudioData();
					while (!stopToken.stop_requested() && this->activeState.load() == VoiceActiveState::Stopped) {
						DatagramSocketClient::processIO(DiscordCoreInternal::ProcessIOType::Both);
						if (sendSilenceStopWatch.hasTimePassed()) {
							sendSilenceStopWatch.resetTimer();
							this->sendSpeakingMessage(true);
							this->sendSpeakingMessage(false);
						}
						std::this_thread::sleep_for(1ms);
						if (!stopToken.stop_requested() && VoiceConnection::areWeConnected()) {
							if (WebSocketCore::processIO(10) == DiscordCoreInternal::ProcessIOResult::Error) {
								this->onClosed();
							}
						}
						if (!stopToken.stop_requested() && VoiceConnection::areWeConnected()) {
							this->checkForAndSendHeartBeat(false);
						}
						this->checkForConnections();
					}
					break;
				}
				case VoiceActiveState::Paused: {
					this->areWePlaying.store(false);
					while (!stopToken.stop_requested() && this->activeState.load() == VoiceActiveState::Paused) {
						DatagramSocketClient::processIO(DiscordCoreInternal::ProcessIOType::Both);
						if (sendSilenceStopWatch.hasTimePassed()) {
							sendSilenceStopWatch.resetTimer();
							this->sendSpeakingMessage(true);
							this->sendSpeakingMessage(false);
						}
						std::this_thread::sleep_for(1ms);
						if (!stopToken.stop_requested() && VoiceConnection::areWeConnected()) {
							if (WebSocketCore::processIO(10) == DiscordCoreInternal::ProcessIOResult::Error) {
								this->onClosed();
							}
						}
						if (!stopToken.stop_requested() && VoiceConnection::areWeConnected()) {
							this->checkForAndSendHeartBeat(false);
						}
						this->checkForConnections();
					}
					break;
				}
				case VoiceActiveState::Playing: {
					this->audioData.type = AudioFrameType::Encoded;
					this->audioData.data.clear();

					stopWatch.resetTimer();
					while (!stopToken.stop_requested() && !DatagramSocketClient::areWeStillConnected()) {
						if (stopWatch.hasTimePassed() || this->activeState.load() == VoiceActiveState::Exiting) {
							return;
						}
						std::this_thread::sleep_for(1ms);
						if (!stopToken.stop_requested() && VoiceConnection::areWeConnected()) {
							if (WebSocketCore::processIO(10) == DiscordCoreInternal::ProcessIOResult::Error) {
								this->onClosed();
							}
						}
						if (!stopToken.stop_requested() && VoiceConnection::areWeConnected()) {
							this->checkForAndSendHeartBeat(false);
						}
						this->checkForConnections();
					}

					DoubleTimePointNs startingValue{ std::chrono::steady_clock::now().time_since_epoch() };
					DoubleTimePointNs intervalCount{ std::chrono::nanoseconds{ 20000000 } };
					DoubleTimePointNs targetTime{ startingValue.time_since_epoch() + intervalCount.time_since_epoch() };
					int64_t frameCounter{ 0 };
					DoubleTimePointNs totalTime{ std::chrono::nanoseconds{ 0 } };

					this->sendSpeakingMessage(false);
					this->sendSpeakingMessage(true);

					while (!stopToken.stop_requested() && this->activeState.load() == VoiceActiveState::Playing) {
						this->areWePlaying.store(true);
						if (!stopToken.stop_requested() && VoiceConnection::areWeConnected()) {
							this->checkForAndSendHeartBeat(false);
						}
						this->checkForConnections();
						this->discordCoreClient->getSongAPI(this->voiceConnectInitData.guildId)->audioDataBuffer.tryReceive(this->audioData);
						if (this->audioData.data.size() == 0) {
							this->areWePlaying.store(false);
						} else {
							this->areWePlaying.store(true);
						}
						frameCounter++;
						if (this->audioData.guildMemberId != 0) {
							this->currentGuildMemberId = this->audioData.guildMemberId;
						}
						std::basic_string_view<uint8_t> newFrame{};
						bool doWeBreak{ false };
						switch (this->audioData.type) {
							case AudioFrameType::RawPCM: {
								auto encodedFrameData = this->encoder.encodeSingleAudioFrame(this->audioData);
								newFrame = this->packetEncrypter.encryptPacket(encodedFrameData);
								break;
							}
							case AudioFrameType::Encoded: {
								newFrame = this->packetEncrypter.encryptPacket(this->audioData);
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
								break;
							}
						}
						if (doWeBreak) {
							break;
						}
						auto waitTime = targetTime - std::chrono::steady_clock::now();
						if (waitTime.count() >= 18000000) {
							if (!stopToken.stop_requested() && VoiceConnection::areWeConnected()) {
								if (WebSocketCore::processIO(1) == DiscordCoreInternal::ProcessIOResult::Error) {
									this->onClosed();
								}
							}
						}
						waitTime = targetTime - std::chrono::steady_clock::now();
						nanoSleep(static_cast<uint64_t>(static_cast<double>(waitTime.count()) * 0.95f));
						waitTime = targetTime - std::chrono::steady_clock::now();
						if (waitTime.count() > 0 && waitTime.count() < 20000000) {
							spinLock(waitTime.count());
						}
						startingValue = std::chrono::steady_clock::now();
						if (newFrame.size() > 0) {
							this->sendVoiceData(newFrame);
						}
						DatagramSocketClient::processIO(DiscordCoreInternal::ProcessIOType::Both);

						this->audioData.data.clear();
						this->audioData.sampleCount = 0;
						this->audioData.type = AudioFrameType::Unset;
						totalTime += std::chrono::steady_clock::now() - startingValue;
						const auto intervalCountNew =
							DoubleTimePointNs{ std::chrono::nanoseconds{ 20000000 } - (totalTime.time_since_epoch() / frameCounter) }
								.time_since_epoch()
								.count();
						intervalCount = DoubleTimePointNs{ std::chrono::nanoseconds{ static_cast<uint64_t>(intervalCountNew) } };
						targetTime = std::chrono::steady_clock::now().time_since_epoch() + intervalCount;
					}
					break;
				}
					this->areWePlaying.store(true);
				case VoiceActiveState::Exiting: {
					return;
				}
			}
			if (stopToken.stop_requested()) {
				return;
			}
			std::this_thread::sleep_for(1ms);
		}
	};

	bool VoiceConnection::areWeCurrentlyPlaying() noexcept {
		return (this->areWePlaying.load() && this->activeState.load() == VoiceActiveState::Playing) ||
			this->activeState.load() == VoiceActiveState::Paused;
	}

	void VoiceConnection::handleAudioBuffer() noexcept {
		if (this->connectionState.load() == VoiceConnectionState::Initializing_DatagramSocket) {
		} else {
			std::basic_string_view<uint8_t> string = DatagramSocketClient::getInputBuffer();
			if (this->streamSocket && this->encryptionKey.size() > 0) {
				this->parseIncomingVoiceData(string);
			}
		}
	}

	void VoiceConnection::connectInternal() noexcept {
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
					this->currentReconnectTries++;
					this->onClosed();
					return;
				}
				this->baseUrl = this->voiceConnectionData.endPoint.substr(0, this->voiceConnectionData.endPoint.find(":"));
				this->connectionState.store(VoiceConnectionState::Initializing_WebSocket);
				this->connectInternal();
				break;
			}
			case VoiceConnectionState::Initializing_WebSocket: {
				this->currentState.store(DiscordCoreInternal::WebSocketState::Upgrading);
				if (!WebSocketCore::connect(this->baseUrl, "/?v=4", "443", this->configManager->doWePrintWebSocketErrorMessages(), false)) {
					this->currentReconnectTries++;
					this->onClosed();
					return;
				}
				this->shard[0] = 0;
				this->shard[1] = 1;
				while (this->currentState.load() != DiscordCoreInternal::WebSocketState::Collecting_Hello) {
					if (WebSocketCore::processIO(10) == DiscordCoreInternal::ProcessIOResult::Error) {
						this->currentReconnectTries++;
						this->onClosed();
						return;
					}
				}
				this->connectionState.store(VoiceConnectionState::Collecting_Hello);
				this->connectInternal();
				break;
			}
			case VoiceConnectionState::Collecting_Hello: {
				stopWatch.resetTimer();
				while (this->connectionState.load() != VoiceConnectionState::Sending_Identify) {
					if (stopWatch.hasTimePassed()) {
						this->currentReconnectTries++;
						this->onClosed();
						return;
					}
					if (WebSocketCore::processIO(100) == DiscordCoreInternal::ProcessIOResult::Error) {
						this->currentReconnectTries++;
						this->onClosed();
						return;
					}
					std::this_thread::sleep_for(1ms);
				}
				this->currentReconnectTries = 0;
				this->connectInternal();
				break;
			}
			case VoiceConnectionState::Sending_Identify: {
				this->haveWeReceivedHeartbeatAck = true;
				DiscordCoreInternal::VoiceIdentifyData data{};
				data.connectInitData = this->voiceConnectInitData;
				data.connectionData = this->voiceConnectionData;
				auto serializer = data.operator DiscordCoreAPI::Jsonifier();
				serializer.refreshString(JsonifierSerializeType::Json);
				std::string string = this->prepMessageData(serializer.operator std::string(), this->dataOpCode);
				if (!WebSocketCore::sendMessage(string, true)) {
					this->currentReconnectTries++;
					return;
				}
				this->connectionState.store(VoiceConnectionState::Collecting_Ready);
				this->connectInternal();
				break;
			}
			case VoiceConnectionState::Collecting_Ready: {
				stopWatch.resetTimer();
				while (this->connectionState.load() != VoiceConnectionState::Initializing_DatagramSocket) {
					if (stopWatch.hasTimePassed()) {
						this->currentReconnectTries++;
						this->onClosed();
						return;
					}
					if (WebSocketCore::processIO(100) == DiscordCoreInternal::ProcessIOResult::Error) {
						this->currentReconnectTries++;
						this->onClosed();
						return;
					}
					std::this_thread::sleep_for(1ms);
				}
				this->connectInternal();
				break;
			}
			case VoiceConnectionState::Initializing_DatagramSocket: {
				this->voiceConnect();
				this->connectionState.store(VoiceConnectionState::Sending_Select_Protocol);
				this->connectInternal();
				break;
			}
			case VoiceConnectionState::Sending_Select_Protocol: {
				DiscordCoreInternal::VoiceSocketProtocolPayloadData data{};
				data.voiceEncryptionMode = this->audioEncryptionMode;
				data.externalIp = this->externalIp;
				data.voicePort = this->port;
				auto serializer = data.operator DiscordCoreAPI::Jsonifier();
				serializer.refreshString(JsonifierSerializeType::Json);
				std::string string = this->prepMessageData(serializer.operator std::string(), this->dataOpCode);
				if (!WebSocketCore::sendMessage(string, true)) {
					this->currentReconnectTries++;
					return;
				}
				this->connectionState.store(VoiceConnectionState::Collecting_Session_Description);
				this->activeState.store(VoiceActiveState::Playing);
				this->connectInternal();
				break;
			}
			case VoiceConnectionState::Collecting_Session_Description: {
				stopWatch.resetTimer();
				while (this->connectionState.load() != VoiceConnectionState::Collecting_Init_Data) {
					if (stopWatch.hasTimePassed()) {
						this->currentReconnectTries++;
						this->onClosed();
						return;
					}
					if (WebSocketCore::processIO(100) == DiscordCoreInternal::ProcessIOResult::Error) {
						this->currentReconnectTries++;
						this->onClosed();
						return;
					}
					std::this_thread::sleep_for(1ms);
				}
				this->baseShard->voiceConnectionDataBuffersMap[this->voiceConnectInitData.guildId.operator size_t()]->clearContents();
				this->connectionState.store(VoiceConnectionState::Collecting_Init_Data);
				if (this->voiceConnectInitData.streamInfo.type != StreamType::None) {
					this->streamSocket = std::make_unique<VoiceConnectionBridge>(this->discordCoreClient, this->voiceConnectInitData.streamInfo.type,
						this->voiceConnectInitData.guildId);
					this->streamSocket->connect(this->voiceConnectInitData.streamInfo.address, this->voiceConnectInitData.streamInfo.port);
					this->taskThread02 = std::make_unique<std::jthread>([=, this](std::stop_token stopToken) {
						this->runBridge(stopToken);
					});
				}
				this->activeState.store(VoiceActiveState::Playing);
				this->play();
				return;
			}
		}
	}

	void VoiceConnection::clearAudioData() noexcept {
		if (this->audioData.data.size() != 0) {
			this->audioData.data.clear();
			this->audioData = AudioFrameData();
		}
		AudioFrameData frameData{};
		while (this->discordCoreClient->getSongAPI(this->voiceConnectInitData.guildId)->audioDataBuffer.tryReceive(frameData)) {
		};
	}

	bool VoiceConnection::areWeConnected() noexcept {
		return WebSocketCore::areWeStillConnected() && DatagramSocketClient::areWeStillConnected();
	}

	bool VoiceConnection::voiceConnect() noexcept {
		if (!DatagramSocketClient::areWeStillConnected()) {
			if (!DatagramSocketClient::connect(this->voiceIp, std::to_string(this->port))) {
				return false;
			} else {
				uint8_t packet[74]{};
				constexpr uint16_t val1601{ 0x01 };
				constexpr uint16_t val1602{ 70 };
				packet[0] = static_cast<uint8_t>(val1601 >> 8);
				packet[1] = static_cast<uint8_t>(val1601 >> 0);
				packet[2] = static_cast<uint8_t>(val1602 >> 8);
				packet[3] = static_cast<uint8_t>(val1602 >> 0);
				packet[4] = static_cast<uint8_t>(this->audioSSRC >> 24);
				packet[5] = static_cast<uint8_t>(this->audioSSRC >> 16);
				packet[6] = static_cast<uint8_t>(this->audioSSRC >> 8);
				packet[7] = static_cast<uint8_t>(this->audioSSRC);
				DatagramSocketClient::getInputBuffer();
				DatagramSocketClient::writeData(std::basic_string_view<uint8_t>{ packet, std::size(packet) });
				std::basic_string_view<uint8_t> inputString{};
				StopWatch stopWatch{ 5500ms };
				while (inputString.size() < 74 && !this->doWeQuit->load() && this->activeState.load() != VoiceActiveState::Exiting) {
					DatagramSocketClient::processIO(DiscordCoreInternal::ProcessIOType::Both);
					inputString = DatagramSocketClient::getInputBuffer();
					std::this_thread::sleep_for(1ms);
					if (stopWatch.hasTimePassed()) {
						return false;
					}
				}
				std::string message{};
				if (inputString.size() > 64) {
					message.insert(message.begin(), inputString.begin() + 8, inputString.begin() + 64);
				}
				const auto endLineFind = message.find('\u0000', 5);
				if (endLineFind != std::string::npos) {
					message = message.substr(0, endLineFind);
				}
				this->externalIp = message;
				this->voiceConnectionDataBuffer.clearContents();
				return true;
			}
		} else {
			return true;
		}
	}

	void VoiceConnection::sendSilence() noexcept {
		std::vector<std::basic_string<uint8_t>> frames{};
		for (size_t x = 0; x < 5; ++x) {
			AudioFrameData newFrame{};
			newFrame.data.push_back(0xf8);
			newFrame.data.push_back(0xff);
			newFrame.data.push_back(0xfe);
			frames.push_back(static_cast<std::basic_string<uint8_t>>(this->packetEncrypter.encryptPacket(newFrame)));
		}
		for (auto& value: frames) {
			this->sendVoiceData(value);
		}
	}

	void VoiceConnection::pauseToggle() noexcept {
		if (this) {
			if (this->activeState.load() == VoiceActiveState::Paused) {
				this->activeState.store(VoiceActiveState::Playing);
			} else {
				this->activeState.store(VoiceActiveState::Paused);
			}
		}
	}

	void VoiceConnection::checkForConnections() {
		if (this->connections) {
			this->connections.reset(nullptr);
			const VoiceActiveState currentState{ this->activeState.load() };
			StopWatch stopWatch{ 10000ms };
			this->connectionState.store(VoiceConnectionState::Collecting_Init_Data);
			while (this->baseShard->currentState.load() != DiscordCoreInternal::WebSocketState::Authenticated) {
				if (stopWatch.hasTimePassed() || this->activeState.load() == VoiceActiveState::Exiting) {
					return;
				}
				std::this_thread::sleep_for(1ms);
			}
			this->connectInternal();
			this->sendSpeakingMessage(true);
			this->activeState.store(currentState);
		}
	}

	void VoiceConnection::disconnect() noexcept {
		this->activeState.store(VoiceActiveState::Exiting);
		if (this->taskThread01) {
			this->taskThread01->request_stop();
			if (this->taskThread01->joinable()) {
				this->taskThread01->join();
			}
			this->taskThread01.reset(nullptr);
		}
		if (this->taskThread02) {
			this->taskThread02->request_stop();
			if (this->taskThread02->joinable()) {
				this->taskThread02->join();
			}
			this->taskThread02.reset(nullptr);
		}
		DatagramSocketClient::disconnect();
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
		WebSocketCore::ssl = nullptr;
		WebSocketCore::outputBuffer.clear();
		WebSocketCore::inputBuffer.clear();
		WebSocketCore::socket = SOCKET_ERROR;
		this->voiceUsers.clear();
		this->activeState.store(VoiceActiveState::Connecting);
		this->connectionState.store(VoiceConnectionState::Collecting_Init_Data);
		this->currentState.store(DiscordCoreInternal::WebSocketState::Disconnected);
		this->discordCoreClient->getSongAPI(this->voiceConnectInitData.guildId)->audioDataBuffer.clearContents();
	}

	void VoiceConnection::reconnect() noexcept {
		this->currentState.store(DiscordCoreInternal::WebSocketState::Disconnected);
		WebSocketCore::ssl = nullptr;
		WebSocketCore::socket = SOCKET_ERROR;
		DatagramSocketClient::disconnect();
		this->closeCode = 0;
		this->areWeHeartBeating = false;
		if (this->streamSocket) {
			this->streamSocket->connect(this->voiceConnectInitData.streamInfo.address, this->voiceConnectInitData.streamInfo.port);
		}
		this->currentReconnectTries++;
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
		opus_int32 voiceUserCount{};
		size_t decodedSize{};
		std::memset(this->upSampledVector.data(), 0b00000000, this->upSampledVector.size() * sizeof(int32_t));
		std::unique_lock lock{ this->voiceUserMutex };
		for (auto& [key, value]: this->voiceUsers) {
			std::basic_string<uint8_t> payload{ value.extractPayload() };
			if (payload.size() > 0) {
				const uint64_t headerSize{ 12 };
				const uint64_t csrcCount{ static_cast<uint64_t>(payload[0]) & 0b0000'1111 };
				const uint64_t offsetToData{ headerSize + sizeof(uint32_t) * csrcCount };
				const uint64_t encryptedDataLength{ payload.size() - offsetToData };

				if (this->decryptedDataString.size() < encryptedDataLength) {
					this->decryptedDataString.resize(encryptedDataLength);
				}

				uint8_t nonce[24]{};
				for (int32_t x = 0; x < headerSize; ++x) {
					nonce[x] = payload[x];
				}

				if (crypto_secretbox_open_easy(this->decryptedDataString.data(), payload.data() + offsetToData, encryptedDataLength, nonce,
						this->encryptionKey.data())) {
					return;
				}

				std::basic_string_view<uint8_t> newString{ this->decryptedDataString.data(), encryptedDataLength - crypto_secretbox_MACBYTES };

				if ((payload[0] >> 4) & 0b0001) {
					const uint16_t extenstionLengthInWords{ ntohs(*reinterpret_cast<const uint16_t*>(&newString[2])) };
					const size_t extensionLength{ sizeof(uint32_t) * extenstionLengthInWords };
					const size_t extensionHeaderLength{ sizeof(uint16_t) * 2 };
					newString = newString.substr(extensionHeaderLength + extensionLength);
				}

				if (newString.size() > 0) {
					auto decodedData = value.getDecoder().decodeData(newString);
					if (decodedData.size() <= 0) {
						if (this->configManager->doWePrintGeneralErrorMessages()) {
							cout << "Failed to decode user's voice payload." << std::endl;
						}
					} else {
						decodedSize = std::max(decodedSize, decodedData.size());
						voiceUserCount++;
						for (uint32_t x = 0; x < decodedData.size(); ++x) {
							this->upSampledVector[x] += decodedData[x];
						}
					}
				}
			}
		}
		if (decodedSize > 0) {
			for (int32_t x = 0; x < decodedSize; ++x) {
				this->downSampledVector[x] = this->upSampledVector[x] / voiceUserCount;
			}
			this->streamSocket->writeData(
				std::basic_string_view<uint8_t>{ reinterpret_cast<uint8_t*>(this->downSampledVector.data()), decodedSize * 2 });
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