/*
	DiscordCoreAPI, A bot library for Discord, written in C++, and featuring explicit multithreading through the usage of custom, asynchronous C++ CoRoutines.

	Copyright 2021, 2022, 2023 Chris M. (RealTimeChris)

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

namespace DiscordCoreInternal {

	template<> struct WebSocketMessageData<DiscordCoreAPI::SpeakingData> {
		DiscordCoreAPI::SpeakingData d{};
		int64_t op{ -1 };
	};

}

namespace Jsonifier {

	template<> struct Core<DiscordCoreAPI::VoiceSessionDescriptionData> {
		using ValueType = DiscordCoreAPI::VoiceSessionDescriptionData;
		static constexpr auto parseValue = object("secret_key", &ValueType::secretKey);
	};

	template<> struct Core<DiscordCoreInternal::WebSocketMessageData<DiscordCoreAPI::SpeakingData>> {
		using ValueType = DiscordCoreInternal::WebSocketMessageData<DiscordCoreAPI::SpeakingData>;
		static constexpr auto parseValue = object("d", &ValueType::d, "op", &ValueType::op);
	};

	template<> struct Core<DiscordCoreAPI::SpeakingData> {
		using ValueType = DiscordCoreAPI::SpeakingData;
		static constexpr auto parseValue = object("ssrc", &ValueType::ssrc, "user_id", &ValueType::userId);
	};

	template<> struct Core<DiscordCoreAPI::VoiceConnectionHelloData> {
		using ValueType = DiscordCoreAPI::VoiceConnectionHelloData;
		static constexpr auto parseValue = object("heartbeat_interval", &ValueType::heartBeatInterval);
	};

	template<> struct Core<DiscordCoreAPI::VoiceUserDisconnectData> {
		using ValueType = DiscordCoreAPI::VoiceUserDisconnectData;
		static constexpr auto parseValue = object("user_id", &ValueType::userId);
	};

	template<> struct Core<DiscordCoreAPI::VoiceSocketReadyData> {
		using ValueType = DiscordCoreAPI::VoiceSocketReadyData;
		static constexpr auto parseValue =
			object("modes", &ValueType::modes, "ip", &ValueType::ip, "port", &ValueType::port, "ssrc", &ValueType::ssrc);
	};

}

namespace DiscordCoreAPI {

	VoiceUser::VoiceUser(Snowflake userIdNew) noexcept {
		userId = userIdNew;
	}

	VoiceUser& VoiceUser::operator=(VoiceUser&& data) noexcept {
		payloads = std::move(data.payloads);
		decoder = std::move(data.decoder);
		userId = data.userId;
		return *this;
	}

	DiscordCoreInternal::OpusDecoderWrapper& VoiceUser::getDecoder() noexcept {
		return decoder;
	}

	void VoiceUser::insertPayload(std::basic_string_view<uint8_t> data) noexcept {
		payloads.writeData(data);
	}

	std::basic_string_view<uint8_t> VoiceUser::extractPayload() noexcept {
		return payloads.readData();
	}

	Snowflake VoiceUser::getUserId() noexcept {
		return userId;
	}

	RTPPacketEncrypter::RTPPacketEncrypter(uint32_t ssrcNew, const std::basic_string<uint8_t>& keysNew) noexcept {
		keys = keysNew;
		ssrc = ssrcNew;
	}

	DiscordCoreInternal::OpusDecoderWrapper decoderWrapper{};

	std::basic_string_view<uint8_t> RTPPacketEncrypter::encryptPacket(DiscordCoreInternal::EncoderReturnData& audioData) noexcept {
		if (keys.size() > 0) {
			++sequence;
			timeStamp += static_cast<uint32_t>(audioData.sampleCount);
			const uint8_t headerSize{ 12 };
			char header[headerSize]{};
			DiscordCoreInternal::storeBits(header, version);
			DiscordCoreInternal::storeBits(header + 1, flags);
			DiscordCoreInternal::storeBits(header + 2, sequence);
			DiscordCoreInternal::storeBits(header + 4, timeStamp);
			DiscordCoreInternal::storeBits(header + 8, ssrc);
			uint8_t nonceForLibSodium[crypto_secretbox_NONCEBYTES]{};
			for (int8_t x = 0; x < headerSize; ++x) {
				nonceForLibSodium[x] = header[x];
			}
			const uint64_t numOfBytes{ headerSize + audioData.data.size() + crypto_secretbox_MACBYTES };
			if (data.size() < numOfBytes) {
				data.resize(numOfBytes);
			}
			for (int8_t x = 0; x < headerSize; ++x) {
				data[x] = static_cast<uint8_t>(header[x]);
			}
			if (crypto_secretbox_easy(data.data() + headerSize, audioData.data.data(), audioData.data.size(), nonceForLibSodium, keys.data()) != 0) {
				return {};
			}
			return std::basic_string_view<uint8_t>{ data.data(), numOfBytes };
		}
		return {};
	}

	MovingAverager::MovingAverager(uint64_t collectionCountNew) noexcept {
		collectionCount = collectionCountNew;
	}

	MovingAverager MovingAverager::operator+=(int64_t value) noexcept {
		values.emplace_front(value);
		if (values.size() >= collectionCount) {
			values.pop_back();
		}
		return *this;
	}

	MovingAverager::operator float() noexcept {
		float returnData{};
		if (values.size() > 0) {
			for (auto& value: values) {
				returnData += static_cast<float>(value);
			}
			return returnData / static_cast<float>(values.size());
		} else {
			return 0.0f;
		}
	}

	VoiceConnectionBridge::VoiceConnectionBridge(DiscordCoreClient* discordCoreClientNew, std::basic_string<uint8_t>& encryptionKeyNew, StreamType streamType,
		const std::string& baseUrlNew, const uint16_t portNew, Snowflake guildIdNew, std::stop_token* tokenNew)
		: UDPConnection{ baseUrlNew, portNew, streamType, discordCoreClientNew->getConfigManager().doWePrintWebSocketErrorMessages() } {
		encryptionKey = encryptionKeyNew;
		discordCoreClient = discordCoreClientNew;
		guildId = guildIdNew;
		token = tokenNew;
	}

	inline void VoiceConnectionBridge::collectEightElements(opus_int32* dataIn, opus_int16* dataOut) noexcept {
		__m256 currentSamplesNew256{ _mm256_mul_ps(_mm256_cvtepi32_ps(_mm256_load_si256(reinterpret_cast<const __m256i*>(dataIn))),
			_mm256_add_ps(_mm256_set1_ps(currentGain),
				_mm256_mul_ps(_mm256_set1_ps(increment), _mm256_set_ps(0.0f, 1.0f, 2.0f, 3.0f, 4.0f, 5.0f, 6.0f, 7.0f)))) };

		__m256i currentSamplesNewer256{ _mm256_cvtps_epi32(
			_mm256_blendv_ps(_mm256_max_ps(currentSamplesNew256, _mm256_set1_ps(static_cast<float>(std::numeric_limits<opus_int16>::min()))),
				_mm256_min_ps(currentSamplesNew256, _mm256_set1_ps(static_cast<float>(std::numeric_limits<opus_int16>::max()))),
				_mm256_cmp_ps(currentSamplesNew256, _mm256_set1_ps(0.0f), _CMP_GE_OQ))) };

		__m128i pack128{ _mm_packs_epi32(_mm256_extractf128_si256(currentSamplesNewer256, 0), _mm256_extractf128_si256(currentSamplesNewer256, 1)) };

		_mm_storeu_si128(reinterpret_cast<__m128i*>(dataOut), pack128);
	}

	inline void VoiceConnectionBridge::applyGainRamp(int64_t sampleCount) noexcept {
		increment = (endGain - currentGain) / static_cast<float>(sampleCount);
		for (int64_t x = 0; x < sampleCount / 8; ++x) {
			collectEightElements(upSampledVector.data() + (x * 8), downSampledVector.data() + (x * 8));
			currentGain += increment * 8.0f;
		}
	}

	void VoiceConnectionBridge::parseOutgoingVoiceData() noexcept {
		std::basic_string_view<uint8_t> buffer = getInputBuffer();
		if (buffer == reinterpret_cast<const uint8_t*>("goodbye")) {
			discordCoreClient->getVoiceConnection(guildId)->onClosed();
			return;
		}
		if (buffer.size() > 0) {
			AudioFrameData frame{};
			frame += buffer;
			DiscordCoreAPI::DiscordCoreClient::getSongAPI(guildId)->audioDataBuffer.send(std::move(frame));
		}
	}

	void VoiceConnectionBridge::handleAudioBuffer() noexcept {
		parseOutgoingVoiceData();
	}

	void VoiceConnectionBridge::mixAudio() noexcept {
		opus_int32 voiceUserCountReal{};
		int64_t decodedSize{};
		std::fill(upSampledVector.data(), upSampledVector.data() + upSampledVector.size(), 0);
		for (auto& [key, value]: discordCoreClient->getVoiceConnection(guildId)->voiceUsers) {
			std::basic_string_view<uint8_t> payload{ value->extractPayload() };
			if (payload.size() == 0) {
				continue;
			} else {
				const uint64_t headerSize{ 12 };
				const uint64_t csrcCount{ static_cast<uint64_t>(payload[0]) & 0b0000'1111 };
				const uint64_t offsetToData{ headerSize + sizeof(uint32_t) * csrcCount };
				const uint64_t encryptedDataLength{ payload.size() - offsetToData };

				if (decryptedDataString.size() < encryptedDataLength) {
					decryptedDataString.resize(encryptedDataLength);
				}

				uint8_t nonce[24]{};
				for (int32_t x = 0; x < headerSize; ++x) {
					nonce[x] = payload[x];
				}

				if (crypto_secretbox_open_easy(reinterpret_cast<uint8_t*>(decryptedDataString.data()),
						reinterpret_cast<const uint8_t*>(payload.data()) + offsetToData, encryptedDataLength, reinterpret_cast<uint8_t*>(nonce),
						reinterpret_cast<uint8_t*>(encryptionKey.data()))) {
					continue;
				}

				std::basic_string_view newString{ decryptedDataString.data(), encryptedDataLength - crypto_secretbox_MACBYTES };

				if (static_cast<int8_t>(payload[0] >> 4) & 0b0001) {
					const uint16_t extenstionLengthInWords{ ntohs(*reinterpret_cast<const uint16_t*>(&newString[2])) };
					const uint64_t extensionLength{ sizeof(uint32_t) * extenstionLengthInWords };
					const uint64_t extensionHeaderLength{ sizeof(uint16_t) * 2 };
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
						for (uint64_t x = 0; x < decodedData.size() / 8; ++x) {
							_mm256_storeu_si256(reinterpret_cast<__m256i*>(upSampledVector.data() + (x * 8)),
								_mm256_add_epi32(_mm256_loadu_si256(reinterpret_cast<__m256i*>(upSampledVector.data() + (x * 8))),
									_mm256_cvtepi16_epi32(_mm_loadu_si128(reinterpret_cast<const __m128i*>(decodedData.data() + (x * 8))))));
						}
					}
				}
			}
		}
		if (decodedSize > 0) {
			voiceUserCountAverage += voiceUserCountReal;
			endGain = 1.0f / voiceUserCountAverage;
			applyGainRamp(decodedSize);
			writeData(
				std::basic_string_view<uint8_t>{ reinterpret_cast<uint8_t*>(downSampledVector.data()), static_cast<uint64_t>(decodedSize * 2) });
			currentGain = endGain;
		}
	}

	VoiceUDPConnection::VoiceUDPConnection(const std::string& baseUrlNew, uint16_t portNew, DiscordCoreAPI::StreamType streamType,
		bool doWePrintErrorsNew, VoiceConnection* ptrNew)
		: UDPConnection{ baseUrlNew, portNew, streamType, doWePrintErrorsNew } {
		voiceConnection = ptrNew;
	};

	VoiceConnection::VoiceConnection(DiscordCoreClient* discordCoreClientNew, DiscordCoreInternal::WebSocketClient* baseShardNew,
		std::atomic_bool* doWeQuitNew) noexcept
		: WebSocketCore(&discordCoreClientNew->configManager, DiscordCoreInternal::WebSocketType::Voice, false) {
		dataOpCode = DiscordCoreInternal::WebSocketOpCode::Op_Text;
		msPerPacket = 20;
		samplesPerPacket = sampleRatePerSecond / 1000 * msPerPacket;
		configManager = &discordCoreClientNew->configManager;
		discordCoreClient = discordCoreClientNew;
		baseShard = baseShardNew;
		doWeQuit = doWeQuitNew;
		haveWeConstructed.store(true);
	}

	Snowflake VoiceConnection::getChannelId() noexcept {
		return voiceConnectInitData.channelId;
	}

	void VoiceConnection::parseIncomingVoiceData(std::basic_string_view<uint8_t> rawDataBufferNew) noexcept {
		if (rawDataBufferNew.size() <= 39) {
			return;
		}
		const uint32_t speakerSsrc{ ntohl(*reinterpret_cast<const uint32_t*>(rawDataBufferNew.data() + 8)) };
		if (voiceUsers.contains(speakerSsrc)) {
			if (72 <= (static_cast<int8_t>(rawDataBufferNew[1]) & 0b0111'1111) && ((static_cast<int8_t>(rawDataBufferNew[1]) & 0b0111'1111) <= 76)) {
				return;
			}
			voiceUsers[speakerSsrc]->insertPayload(rawDataBufferNew);
		}
	}

	void VoiceConnection::connect(const VoiceConnectInitData& initData) noexcept {
		voiceConnectInitData = initData;
		connections = std::make_unique<ConnectionPackage>();
		connections->currentReconnectTries = currentReconnectTries;
		connections->currentShard = shard[0];
		activeState.store(VoiceActiveState::Connecting);
		if (!taskThread) {
			taskThread = std::make_unique<std::jthread>([=, this](std::stop_token token) {
				runVoice(token);
			});
		}
		StopWatch stopWatch{ 15000us };
		while (!WebSocketCore::tcpConnection.get() || (!WebSocketCore::tcpConnection->areWeStillConnected() && !stopWatch.hasTimePassed())) {
			std::this_thread::sleep_for(1us);
		}
	}

	UnboundedMessageBlock<AudioFrameData>& VoiceConnection::getAudioBuffer() noexcept {
		return DiscordCoreAPI::DiscordCoreClient::getSongAPI(voiceConnectInitData.guildId)->audioDataBuffer;
	}

	void VoiceConnection::checkForAndSendHeartBeat(const bool isImmedate) noexcept {
		if (heartBeatStopWatch.hasTimePassed() || isImmedate) {
			DiscordCoreInternal::WebSocketMessageData<int32_t> message{};
			message.excludedKeys.emplace("t");
			message.excludedKeys.emplace("s");
			std::string string{};
			message.d = std::chrono::duration_cast<Nanoseconds>(HRClock::now().time_since_epoch()).count();
			message.op = 3;
			parser.serializeJson<true>(message, string);
			createHeader(string, dataOpCode);
			if (!sendMessage(string, true)) {
				onClosed();
				return;
			}
			if (activeState.load() == VoiceActiveState::Paused || activeState.load() == VoiceActiveState::Stopped) {
				sendSilence();
			}
			haveWeReceivedHeartbeatAck = false;
			heartBeatStopWatch.resetTimer();
		}
	}

	void VoiceConnection::sendSpeakingMessage(const bool isSpeaking) noexcept {
		DiscordCoreInternal::WebSocketMessageData<DiscordCoreInternal::SendSpeakingData> data{};
		data.excludedKeys.emplace("t");
		data.excludedKeys.emplace("s");
		if (!isSpeaking) {
			data.d.type = static_cast<DiscordCoreInternal::SendSpeakingType>(0);
			sendSilence();
			if (udpConnection) {
				try {
					udpConnection->processIO();
				} catch (...) {
					if (configManager->doWePrintWebSocketErrorMessages()) {
						reportException("VoiceConnection::sendSpeakingMessage()");
					}
				}
			}
		} else {
			data.d.type = DiscordCoreInternal::SendSpeakingType::Microphone;
		}
		data.d.delay = 0;
		data.d.ssrc = audioSSRC;
		data.op = 5;
		std::string string{};
		parser.serializeJson<true>(data, string);
		createHeader(string, dataOpCode);
		sendMessage(string, true);
	}

	void VoiceConnection::checkForConnections() noexcept {
		if (connections) {
			connections.reset(nullptr);
			currentState.store(DiscordCoreInternal::WebSocketState::Disconnected);
			if (voiceConnectInitData.streamInfo.type != StreamType::None) {
				if (streamSocket) {
					streamSocket->inputBuffer.clear();
					streamSocket->outputBuffer.clear();
				}
				discordCoreClient->getSongAPI(voiceConnectInitData.guildId)->audioDataBuffer.clearContents();
			}
			WebSocketCore::disconnect();
			if (udpConnection) {
				udpConnection->disconnect();
			}
			closeCode = 0;
			areWeHeartBeating = false;
			StopWatch stopWatch{ 10000ms };
			connectionState.store(VoiceConnectionState::Collecting_Init_Data);
			connectInternal();
			activeState.store(VoiceActiveState::Playing);
			sendSpeakingMessage(true);
		}
	}

	bool VoiceConnection::onMessageReceived(std::string_view data) noexcept {
		DiscordCoreInternal::WebSocketMessage message{};
		if (configManager->doWePrintWebSocketSuccessMessages()) {
			cout << shiftToBrightGreen() << "Message received from Voice WebSocket: " << data << reset() << endl << endl;
		}
		parser.parseJson<true>(message, data);
		switch (static_cast<VoiceSocketOpCodes>(message.op)) {
			case VoiceSocketOpCodes::Ready_Server: {
				DiscordCoreInternal::WebSocketMessageData<VoiceSocketReadyData> dataNew{};
				parser.parseJson<true>(dataNew, data);
				audioSSRC = dataNew.d.ssrc;
				voiceIp = dataNew.d.ip;
				port = dataNew.d.port;
				for (auto value: dataNew.d.modes) {
					if (value == "xsalsa20_poly1305") {
						audioEncryptionMode = value;
					}
				}
				connectionState.store(VoiceConnectionState::Initializing_DatagramSocket);
				break;
			}
			case VoiceSocketOpCodes::Session_Description: {
				DiscordCoreInternal::WebSocketMessageData<VoiceSessionDescriptionData> dataNew{};
				encryptionKey.clear();
				parser.parseJson<true>(dataNew, data);
				for (auto value: dataNew.d.secretKey) {
					encryptionKey.push_back(static_cast<uint8_t>(value));
				}
				packetEncrypter = RTPPacketEncrypter{ audioSSRC, encryptionKey };
				connectionState.store(VoiceConnectionState::Collecting_Init_Data);
				break;
			}
			case VoiceSocketOpCodes::Speaking: {
				DiscordCoreInternal::WebSocketMessageData<SpeakingData> dataNew{};
				parser.parseJson<true>(dataNew, data);
				const uint32_t ssrc = dataNew.d.ssrc;
				auto userId = dataNew.d.userId;
				std::unique_ptr<VoiceUser> user{ std::make_unique<VoiceUser>(userId) };
				if (!Users::getCachedUser({ .userId = user->getUserId() }).getFlagValue(UserFlags::Bot) ||
					voiceConnectInitData.streamInfo.streamBotAudio) {
					if (!voiceUsers.contains(ssrc)) {
						voiceUsers.emplace(std::make_pair(ssrc, std::move(user)));
					}
				}
				break;
			}
			case VoiceSocketOpCodes::Heartbeat_ACK: {
				haveWeReceivedHeartbeatAck = true;
				break;
			}
			case VoiceSocketOpCodes::Hello: {
				DiscordCoreInternal::WebSocketMessageData<VoiceConnectionHelloData> dataNew{};
				parser.parseJson<true>(dataNew, data);
				heartBeatStopWatch = StopWatch{ Milliseconds{ static_cast<uint32_t>(dataNew.d.heartBeatInterval) } };
				areWeHeartBeating = true;
				connectionState.store(VoiceConnectionState::Sending_Identify);
				currentState.store(DiscordCoreInternal::WebSocketState::Authenticated);
				haveWeReceivedHeartbeatAck = true;
				break;
			}
			case VoiceSocketOpCodes::Resumed: {
				connectionState.store(VoiceConnectionState::Initializing_DatagramSocket);
				break;
			}
			case VoiceSocketOpCodes::Client_Disconnect: {
				DiscordCoreInternal::WebSocketMessageData<VoiceUserDisconnectData> dataNew{};
				parser.parseJson<true>(dataNew, data);
				const auto userId = dataNew.d.userId;
				for (auto& [key, value]: voiceUsers) {
					if (userId == value->getUserId()) {
						voiceUsers.erase(key);
						break;
					}
				}
				break;
			}
			case VoiceSocketOpCodes::Identify: {
				[[fallthrough]];
			}
			case VoiceSocketOpCodes::Select_Protocol: {
				[[fallthrough]];
			}
			case VoiceSocketOpCodes::Heartbeat: {
				[[fallthrough]];
			}
			case VoiceSocketOpCodes::Resume: {
				break;
			}
		}
		return true;
	}

	void VoiceConnection::connectInternal() noexcept {
		StopWatch stopWatch{ 10000ms };
		if (currentReconnectTries >= maxReconnectTries) {
			doWeQuit->store(true);
			if (configManager->doWePrintWebSocketErrorMessages()) {
				cout << "VoiceConnection::connect() Error: Failed to connect to voice channel!" << endl << endl;
			}
			return;
		}
		activeState.store(VoiceActiveState::Connecting);
		switch (connectionState.load()) {
			case VoiceConnectionState::Collecting_Init_Data: {
				baseShard->voiceConnectionDataBufferMap[voiceConnectInitData.guildId.operator uint64_t()] = &voiceConnectionDataBuffer;
				baseShard->voiceConnectionDataBufferMap[voiceConnectInitData.guildId.operator uint64_t()]->clearContents();
				baseShard->getVoiceConnectionData(voiceConnectInitData);

				if (waitForTimeToPass(voiceConnectionDataBuffer, voiceConnectionData, 10000)) {
					++currentReconnectTries;
					onClosed();
					return;
				}
				baseUrl = voiceConnectionData.endPoint.substr(0, voiceConnectionData.endPoint.find(":"));
				connectionState.store(VoiceConnectionState::Initializing_WebSocket);
				connectInternal();
				break;
			}
			case VoiceConnectionState::Initializing_WebSocket: {
				currentState.store(DiscordCoreInternal::WebSocketState::Upgrading);
				if (!WebSocketCore::connect(baseUrl, "/?v=4", 443, true)) {
					++currentReconnectTries;
					onClosed();
					return;
				}
				shard[0] = 0;
				shard[1] = 1;
				while (currentState.load() != DiscordCoreInternal::WebSocketState::Collecting_Hello && !token->stop_requested()) {
					try {
						WebSocketCore::tcpConnection->processIO(10);
					} catch (...) {
						if (configManager->doWePrintWebSocketErrorMessages()) {
							reportException("VoiceConnection::connectInternal()");
						}
						++currentReconnectTries;
						onClosed();
						return;
					}
				}
				connectionState.store(VoiceConnectionState::Collecting_Hello);
				connectInternal();
				break;
			}
			case VoiceConnectionState::Collecting_Hello: {
				stopWatch.resetTimer();
				while (connectionState.load() != VoiceConnectionState::Sending_Identify && !token->stop_requested()) {
					if (stopWatch.hasTimePassed()) {
						++currentReconnectTries;
						onClosed();
						return;
					}
					try {
						WebSocketCore::tcpConnection->processIO(10);
					} catch (...) {
						if (configManager->doWePrintWebSocketErrorMessages()) {
							reportException("VoiceConnection::connectInternal()");
						}
						++currentReconnectTries;
						onClosed();
						return;
					}
					std::this_thread::sleep_for(1ms);
				}
				currentReconnectTries = 0;
				connectInternal();
				break;
			}
			case VoiceConnectionState::Sending_Identify: {
				haveWeReceivedHeartbeatAck = true;
				DiscordCoreInternal::WebSocketMessageData<DiscordCoreInternal::VoiceIdentifyData> data{};
				data.excludedKeys.emplace("t");
				data.excludedKeys.emplace("s");
				data.d.serverId = voiceConnectInitData.guildId;
				data.d.sessionId = voiceConnectionData.sessionId;
				data.d.token = voiceConnectionData.token;
				data.d.userId = voiceConnectInitData.userId;
				data.op = 0;
				data.s = 0;
				std::string string{};
				parser.serializeJson<true>(data, string);
				createHeader(string, dataOpCode);
				if (!WebSocketCore::sendMessage(string, true)) {
					++currentReconnectTries;
					onClosed();
					return;
				}
				connectionState.store(VoiceConnectionState::Collecting_Ready);
				connectInternal();
				break;
			}
			case VoiceConnectionState::Collecting_Ready: {
				stopWatch.resetTimer();
				while (connectionState.load() != VoiceConnectionState::Initializing_DatagramSocket && !token->stop_requested()) {
					if (stopWatch.hasTimePassed()) {
						++currentReconnectTries;
						onClosed();
						return;
					}
					try {
						WebSocketCore::tcpConnection->processIO(100);
					} catch (...) {
						if (configManager->doWePrintWebSocketErrorMessages()) {
							reportException("VoiceConnection::connectInternal()");
						}
						++currentReconnectTries;
						onClosed();
						return;
					}
					std::this_thread::sleep_for(1ms);
				}
				connectInternal();
				break;
			}
			case VoiceConnectionState::Initializing_DatagramSocket: {
				if (!voiceConnect()) {
					++currentReconnectTries;
					onClosed();
					return;
				}
				connectionState.store(VoiceConnectionState::Sending_Select_Protocol);
				connectInternal();
				break;
			}
			case VoiceConnectionState::Sending_Select_Protocol: {
				DiscordCoreInternal::WebSocketMessageData<DiscordCoreInternal::VoiceSocketProtocolPayloadData> data{};
				data.excludedKeys.emplace("t");
				data.excludedKeys.emplace("s");
				data.d.data.mode = audioEncryptionMode;
				data.d.data.address = externalIp;
				data.d.data.port = port;
				data.d.protocol = "udp";
				data.op = 1;
				std::string string{};
				parser.serializeJson<true>(data, string);
				createHeader(string, dataOpCode);
				if (!WebSocketCore::sendMessage(string, true)) {
					++currentReconnectTries;
					onClosed();
					return;
				}
				connectionState.store(VoiceConnectionState::Collecting_Session_Description);
				connectInternal();
				break;
			}
			case VoiceConnectionState::Collecting_Session_Description: {
				stopWatch.resetTimer();
				while (connectionState.load() != VoiceConnectionState::Collecting_Init_Data && !token->stop_requested()) {
					if (stopWatch.hasTimePassed()) {
						++currentReconnectTries;
						onClosed();
						return;
					}
					try {
						WebSocketCore::tcpConnection->processIO(10);
					} catch (...) {
						if (configManager->doWePrintWebSocketErrorMessages()) {
							reportException("VoiceConnection::connectInternal()");
						}
						++currentReconnectTries;
						onClosed();
						return;
					}
					std::this_thread::sleep_for(1ms);
				}
				baseShard->voiceConnectionDataBufferMap[voiceConnectInitData.guildId.operator uint64_t()]->clearContents();
				if (voiceConnectInitData.streamInfo.type != StreamType::None) {
					try {
						streamSocket = std::make_unique<VoiceConnectionBridge>(discordCoreClient, encryptionKey, voiceConnectInitData.streamInfo.type,
							voiceConnectInitData.streamInfo.address, voiceConnectInitData.streamInfo.port, voiceConnectInitData.guildId, token);
					} catch (...) {
						if (configManager->doWePrintWebSocketSuccessMessages()) {
							reportException("VoiceConnection::connectInternal()");
						}
						++currentReconnectTries;
						onClosed();
						return;
					}
				}
				play();
				while (!haveWeConstructed.load()) {
					std::this_thread::sleep_for(1ms);
				}
				return;
			}
		}
	}

	void VoiceConnection::runVoice(std::stop_token tokenNew) noexcept {
		StopWatch stopWatch{ 20000ms };
		token = &tokenNew;
		StopWatch sendSilenceStopWatch{ 5000ms };
		while (!token->stop_requested() && !doWeQuit->load() && activeState.load() != VoiceActiveState::Exiting) {
			switch (activeState.load()) {
				case VoiceActiveState::Connecting: {
					while (!token->stop_requested() && activeState.load() == VoiceActiveState::Connecting) {
						areWePlaying.store(false);
						std::this_thread::sleep_for(1ms);
						if (!token->stop_requested() && VoiceConnection::areWeConnected()) {
							try {
								WebSocketCore::tcpConnection->processIO(10);
							} catch (...) {
								if (configManager->doWePrintWebSocketErrorMessages()) {
									reportException("VoiceConnection::runVoice()");
								}
								++currentReconnectTries;
								onClosed();
							}
						}
						if (!token->stop_requested() && VoiceConnection::areWeConnected()) {
							checkForAndSendHeartBeat(false);
						}
						checkForConnections();
					}
					break;
				}
				case VoiceActiveState::Stopped: {
					areWePlaying.store(false);
					while (!token->stop_requested() && activeState.load() == VoiceActiveState::Stopped) {
						try {
							udpConnection->processIO();
						} catch (...) {
							if (configManager->doWePrintWebSocketErrorMessages()) {
								reportException("VoiceConnection::runVoice()");
							}
							++currentReconnectTries;
							onClosed();
						}
						std::this_thread::sleep_for(1ms);
						if (!token->stop_requested() && VoiceConnection::areWeConnected()) {
							try {
								WebSocketCore::tcpConnection->processIO(10);
							} catch (...) {
								if (configManager->doWePrintWebSocketErrorMessages()) {
									reportException("VoiceConnection::runVoice()");
								}
								++currentReconnectTries;
								onClosed();
							}
						}
						if (!token->stop_requested() && VoiceConnection::areWeConnected()) {
							checkForAndSendHeartBeat(false);
						}
						checkForConnections();
					}
					break;
				}
				case VoiceActiveState::Paused: {
					areWePlaying.store(false);
					while (!token->stop_requested() && activeState.load() == VoiceActiveState::Paused) {
						udpConnection->processIO();
						std::this_thread::sleep_for(1ms);
						if (!token->stop_requested() && VoiceConnection::areWeConnected()) {
							try {
								WebSocketCore::tcpConnection->processIO(10);
							} catch (...) {
								if (configManager->doWePrintWebSocketErrorMessages()) {
									reportException("VoiceConnection::runVoice()");
								}
								++currentReconnectTries;
								onClosed();
							}
						}
						if (!token->stop_requested() && VoiceConnection::areWeConnected()) {
							checkForAndSendHeartBeat(false);
						}
						checkForConnections();
					}
					break;
				}
				case VoiceActiveState::Playing: {
					xferAudioData.clearData();

					stopWatch.resetTimer();
					while (!token->stop_requested() && (!udpConnection || !udpConnection->areWeStillConnected())) {
						if (stopWatch.hasTimePassed() || activeState.load() == VoiceActiveState::Exiting) {
							return;
						}
						std::this_thread::sleep_for(1ms);
						if (!token->stop_requested() && VoiceConnection::areWeConnected()) {
							try {
								WebSocketCore::tcpConnection->processIO(10);
							} catch (...) {
								if (configManager->doWePrintWebSocketErrorMessages()) {
									reportException("VoiceConnection::runVoice()");
								}
								++currentReconnectTries;
								onClosed();
							}
						}
						if (!token->stop_requested() && VoiceConnection::areWeConnected()) {
							checkForAndSendHeartBeat(false);
						}
						checkForConnections();
					}

					auto targetTime{ HRClock::now() + intervalCount };

					while (!token->stop_requested() && activeState.load() == VoiceActiveState::Playing) {
						int64_t bytesPerSample{ 4 };
						if (!token->stop_requested() && VoiceConnection::areWeConnected()) {
							checkForAndSendHeartBeat(false);
						}
						checkForConnections();
						discordCoreClient->getSongAPI(voiceConnectInitData.guildId)->audioDataBuffer.tryReceive(xferAudioData);
						if ((doWeSkip.load() && xferAudioData.currentSize == 0) || doWeSkipNow.load()) {
							skipInternal();
						}

						AudioFrameType frameType{ xferAudioData.type };
						size_t frameSize{};
						if (xferAudioData.currentSize <= 0) {
							xferAudioData.clearData();
						} else if (xferAudioData.type == AudioFrameType::RawPCM) {
							intervalCount = Nanoseconds{ static_cast<uint64_t>(static_cast<double>(xferAudioData.currentSize / bytesPerSample) /
								static_cast<double>(sampleRatePerSecond) * static_cast<double>(nsPerSecond)) };
							size_t framesPerSecond = 1000 / msPerPacket;
							areWePlaying.store(true);
							frameSize = std::min(bytesPerSample * sampleRatePerSecond / framesPerSecond, xferAudioData.data.size());
						} else {
							areWePlaying.store(true);
							intervalCount = Nanoseconds{ 20000000 };
						}
						std::basic_string_view<uint8_t> frame{};
						switch (frameType) {
							case AudioFrameType::RawPCM: {
								auto encodedFrameData = encoder.encodeData(std::basic_string_view<uint8_t>(xferAudioData.data.data(), frameSize));
								xferAudioData.clearData();
								if (encodedFrameData.data.size() != 0) {
									frame = packetEncrypter.encryptPacket(encodedFrameData);
								}
								break;
							}
							case AudioFrameType::Encoded: {
								try {
									DiscordCoreInternal::EncoderReturnData returnData{};
									returnData.data = { xferAudioData.data.data(), static_cast<size_t>(xferAudioData.currentSize) };
									returnData.sampleCount = 960;
									if (returnData.data.size() != 0) {
										frame = packetEncrypter.encryptPacket(returnData);
										xferAudioData.clearData();
									}
								} catch (...) {
									reportException("VoiceConnection::runVoice()");
								}
								break;
							}
							case AudioFrameType::Unset: {
								xferAudioData.clearData();
								break;
							}
						}
						auto waitTime = targetTime - HRClock::now();
						auto waitTimeCount = waitTime.count();
						int64_t minimumFreeTimeForCheckingProcessIO{ static_cast<int64_t>(static_cast<double>(intervalCount.count()) * 0.70l) };
						try {
							if (WebSocketCore::tcpConnection && WebSocketCore::tcpConnection->areWeStillConnected()) {
								if (waitTimeCount >= minimumFreeTimeForCheckingProcessIO && !token->stop_requested() &&
									VoiceConnection::areWeConnected()) {
									WebSocketCore::tcpConnection->processIO(0);
								}

							} else {
								onClosed();
							}
						} catch (...) {
							if (configManager->doWePrintWebSocketErrorMessages()) {
								reportException("VoiceConnection::runVoice()");
							}
							++currentReconnectTries;
							onClosed();
						}

						waitTime = targetTime - HRClock::now();
						waitTimeCount = static_cast<int64_t>(static_cast<double>(waitTime.count()) * 0.90l);
						if (waitTimeCount > 0) {
							nanoSleep(waitTimeCount);
						}
						waitTime = targetTime - HRClock::now();
						waitTimeCount = waitTime.count();
						if (waitTimeCount > 0 && waitTimeCount < intervalCount.count()) {
							spinLock(waitTimeCount);
						}
						if (udpConnection && udpConnection->areWeStillConnected()) {
							udpConnection->writeData(frame);
							try {
								udpConnection->processIO();
							} catch (...) {
								if (configManager->doWePrintWebSocketErrorMessages()) {
									reportException("VoiceConnection::runVoice()");
								}
								++currentReconnectTries;
								onClosed();
							}
						} else {
							activeState.store(VoiceActiveState::Connecting);
							xferAudioData.clearData();
							onClosed();
						}

						targetTime = HRClock::now() + intervalCount;

						if (streamSocket) {
							streamSocket->mixAudio();
							try {
								if (streamSocket->areWeStillConnected()) {
									streamSocket->processIO();
								} else {
									onClosed();
								}
							} catch (...) {
								if (configManager->doWePrintWebSocketErrorMessages()) {
									reportException("VoiceConnection::runVoice()");
								}
								++currentReconnectTries;
								onClosed();
							}
						}
					}
					areWePlaying.store(false);
					break;
				}
				case VoiceActiveState::Exiting: {
					areWePlaying.store(false);
					return;
				}
			}
			if (token->stop_requested() || activeState == VoiceActiveState::Exiting) {
				areWePlaying.store(false);
				return;
			}
			std::this_thread::sleep_for(1ms);
		}
	};

	void VoiceConnection::skipInternal() noexcept {
		areWePlaying.store(false);
		SongCompletionEventData completionEventData{};
		completionEventData.guild = Guilds::getCachedGuild({ .guildId = voiceConnectInitData.guildId });
		if (xferAudioData.guildMemberId != 0) {
			completionEventData.guildMember =
				GuildMembers::getCachedGuildMember({ .guildMemberId = xferAudioData.guildMemberId, .guildId = voiceConnectInitData.guildId });
		}
		completionEventData.wasItAFail = false;
		DiscordCoreClient::getSongAPI(voiceConnectInitData.guildId)->onSongCompletionEvent(completionEventData);
		DiscordCoreClient::getSongAPI(voiceConnectInitData.guildId)->audioDataBuffer.clearContents();
		xferAudioData.clearData();
		doWeSkipNow.store(false);
		doWeSkip.store(false);
	}

	bool VoiceConnection::areWeCurrentlyPlaying() noexcept {
		return (areWePlaying.load() && activeState.load() == VoiceActiveState::Playing) || activeState.load() == VoiceActiveState::Paused;
	}

	void VoiceUDPConnection::handleAudioBuffer() noexcept {
		if (voiceConnection->connectionState.load() == VoiceConnectionState::Initializing_DatagramSocket) {
		} else {
			std::basic_string_view<uint8_t> string = getInputBuffer();
			if (voiceConnection->streamSocket && voiceConnection->encryptionKey.size() > 0) {
				voiceConnection->parseIncomingVoiceData(string);
			}
		}
	}

	bool VoiceConnection::areWeConnected() noexcept {
		return WebSocketCore::tcpConnection && WebSocketCore::tcpConnection->areWeStillConnected() && udpConnection &&
			udpConnection->areWeStillConnected();
	}

	bool VoiceConnection::voiceConnect() noexcept {
		try {
			udpConnection =
				std::make_unique<VoiceUDPConnection>(voiceIp, port, StreamType::None, configManager->doWePrintWebSocketErrorMessages(), this);
		} catch (...) {
			if (configManager->doWePrintWebSocketErrorMessages()) {
				reportException("VoiceConnection::voiceConnect()");
			}
			return false;
		}
		uint8_t packet[74]{};
		const uint16_t val1601{ 0x01 };
		const uint16_t val1602{ 70 };
		packet[0] = static_cast<uint8_t>(val1601 >> 8);
		packet[1] = static_cast<uint8_t>(val1601 >> 0);
		packet[2] = static_cast<uint8_t>(val1602 >> 8);
		packet[3] = static_cast<uint8_t>(val1602 >> 0);
		packet[4] = static_cast<uint8_t>(audioSSRC >> 24);
		packet[5] = static_cast<uint8_t>(audioSSRC >> 16);
		packet[6] = static_cast<uint8_t>(audioSSRC >> 8);
		packet[7] = static_cast<uint8_t>(audioSSRC);
		udpConnection->getInputBuffer();
		udpConnection->writeData(std::basic_string_view<uint8_t>{ packet, std::size(packet) });
		std::basic_string_view<uint8_t> inputStringFirst{};
		std::basic_string<uint8_t> inputString{};

		StopWatch stopWatch{ 5500ms };
		while (inputStringFirst.size() < 74 && !doWeQuit->load() && activeState.load() != VoiceActiveState::Exiting) {
			try {
				udpConnection->processIO();
			} catch (...) {
				if (configManager->doWePrintWebSocketErrorMessages()) {
					reportException("VoiceConnection::voiceConnect()");
				}
				return false;
			}
			inputStringFirst = udpConnection->getInputBuffer();
			std::this_thread::sleep_for(1ms);
			if (stopWatch.hasTimePassed()) {
				return false;
			}
		}
		inputString.insert(inputString.begin(), inputStringFirst.begin(), inputStringFirst.end());
		inputString = inputString.substr(8);
		const auto endLineFind = inputString.find(static_cast<uint8_t>('\u0000'), 6);
		if (endLineFind != std::string::npos) {
			inputString = inputString.substr(0, endLineFind);
		}
		externalIp = std::string{ reinterpret_cast<const char*>(inputStringFirst.data()) + 8, inputString.size() };
		voiceConnectionDataBuffer.clearContents();
		return true;
	}

	void VoiceConnection::sendSilence() noexcept {
		std::vector<std::basic_string<uint8_t>> frames{};
		uint8_t arrayNew[3]{};
		arrayNew[0] = uint8_t{ 0xf8 };
		arrayNew[1] = uint8_t{ 0xff };
		arrayNew[2] = uint8_t{ 0xfe };
		for (uint64_t x = 0; x < 5; ++x) {
			DiscordCoreInternal::EncoderReturnData frame{};
			frame.data = std::basic_string_view<uint8_t>{ arrayNew, 3 };
			frame.sampleCount = 3;
			auto packetNew = packetEncrypter.encryptPacket(frame);
			frames.push_back(std::basic_string<uint8_t>{ packetNew.data(), packetNew.size() });
		}
		for (auto& value: frames) {
			if (udpConnection) {
				try {
					udpConnection->writeData(value);
					udpConnection->processIO();
				} catch (...) {
					if (configManager->doWePrintWebSocketErrorMessages()) {
						reportException("VoiceConnection::sendSilence()");
					}
				}
			}
		}
	}

	void VoiceConnection::pauseToggle() noexcept {
		if (activeState.load() == VoiceActiveState::Paused) {
			activeState.store(VoiceActiveState::Playing);
		} else {
			activeState.store(VoiceActiveState::Paused);
		}
	}

	void VoiceConnection::disconnect() noexcept {
		activeState.store(VoiceActiveState::Exiting);
		if (taskThread) {
			taskThread->request_stop();
			if (taskThread->joinable()) {
				taskThread.reset(nullptr);
			}
		}
		std::string payload = "\x03\xE8";
		createHeader(payload, DiscordCoreInternal::WebSocketOpCode::Op_Close);
		if (WebSocketCore::tcpConnection) {
			try {
				WebSocketCore::tcpConnection->writeData(payload, true);
			} catch (...) {
				if (configManager->doWePrintWebSocketErrorMessages()) {
					reportException("VoiceConnection::disconnect()");
				}
			}
		}
		if (streamSocket) {
			streamSocket->disconnect();
			streamSocket.reset(nullptr);
		}
		if (DiscordCoreClient::getSongAPI(voiceConnectInitData.guildId)) {
			DiscordCoreClient::getSongAPI(voiceConnectInitData.guildId)
				->onSongCompletionEvent.erase(DiscordCoreClient::getSongAPI(voiceConnectInitData.guildId)->eventToken);
		}
		DiscordCoreClient::getSongAPI(voiceConnectInitData.guildId)->cancelCurrentSong();
		closeCode = 0;
		WebSocketCore::disconnect();
		areWeHeartBeating = false;
		currentReconnectTries = 0;
		voiceUsers.clear();
		activeState.store(VoiceActiveState::Connecting);
		connectionState.store(VoiceConnectionState::Collecting_Init_Data);
		currentState.store(DiscordCoreInternal::WebSocketState::Disconnected);
		discordCoreClient->getSongAPI(voiceConnectInitData.guildId)->audioDataBuffer.clearContents();
	}

	void VoiceConnection::reconnect() noexcept {
		++currentReconnectTries;
		connections = std::make_unique<ConnectionPackage>();
		connections->currentReconnectTries = currentReconnectTries;
		connections->currentShard = shard[0];
		WebSocketCore::disconnect();
		udpConnection.reset(nullptr);
	}

	void VoiceConnection::onClosed() noexcept {
		connectionState.store(VoiceConnectionState::Collecting_Init_Data);
		activeState.store(VoiceActiveState::Stopped);
		if (activeState.load() != VoiceActiveState::Exiting && currentReconnectTries < maxReconnectTries) {
			connectInternal();
		} else if (currentReconnectTries >= maxReconnectTries) {
			VoiceConnection::disconnect();
		}
	}

	bool VoiceConnection::stop() noexcept {
		sendSpeakingMessage(false);
		activeState.store(VoiceActiveState::Stopped);
		return true;
	}

	bool VoiceConnection::play() noexcept {
		activeState.store(VoiceActiveState::Playing);
		sendSpeakingMessage(false);
		sendSpeakingMessage(true);
		return true;
	}

	void VoiceConnection::skip() noexcept {
		doWeSkipNow.store(true);
	}

	VoiceConnection::~VoiceConnection() noexcept {
		disconnect();
	}

}