/*
	MIT License

	DiscordCoreAPI, A bot library for Discord, written in C++, and featuring explicit multithreading through the usage of custom, asynchronous C++ CoRoutines.

	Copyright 2022, 2023 Chris M. (RealTimeChris)

	Permission is hereby granted, free of charge, to any person obtaining a copy
	of this software and associated documentation files (the "Software"), to deal
	in the Software without restriction, including without limitation the rights
	to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
	copies of the Software, and to permit persons to whom the Software is
	furnished to do so, subject to the following conditions:

	The above copyright notice and this permission notice shall be included in all
	copies or substantial portions of the Software.

	THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
	IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
	FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
	AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
	LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
	OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
	SOFTWARE.
*/
/// VoiceConnection.cpp - Source file for the voice connection class.
/// Jul 15, 2021
/// https://discordcoreapi.com
/// \file VoiceConnection.cpp

#include <discordcoreapi/VoiceConnection.hpp>
#include <discordcoreapi/DiscordCoreClient.hpp>
#include <discordcoreapi/Utilities/UDPConnection.hpp>
#include <discordcoreapi/Utilities/ISADetection.hpp>

namespace Jsonifier {

	template<> struct Core<DiscordCoreAPI::VoiceSessionDescriptionData> {
		using ValueType = DiscordCoreAPI::VoiceSessionDescriptionData;
		static constexpr auto parseValue = object("secret_key", &ValueType::secretKey);
	};

	template<> struct Core<DiscordCoreAPI::DiscordCoreInternal::WebSocketMessageData<DiscordCoreAPI::SpeakingData>> {
		using ValueType = DiscordCoreAPI::DiscordCoreInternal::WebSocketMessageData<DiscordCoreAPI::SpeakingData>;
		static constexpr auto parseValue = object("d", &ValueType::d);
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

	VoiceUser::VoiceUser(Snowflake userIdNew) {
		userId = userIdNew;
	}

	VoiceUser& VoiceUser::operator=(VoiceUser&& data) noexcept {
		payloads = std::move(data.payloads);
		decoder = std::move(data.decoder);
		userId = data.userId;
		return *this;
	}

	DiscordCoreInternal::OpusDecoderWrapper& VoiceUser::getDecoder() {
		return decoder;
	}

	void VoiceUser::insertPayload(std::basic_string_view<uint8_t> data) {
		payloads.writeData(data.data(), data.size());
	}

	std::basic_string_view<uint8_t> VoiceUser::extractPayload() {
		return payloads.readData();
	}

	Snowflake VoiceUser::getUserId() {
		return userId;
	}

	RTPPacketEncrypter::RTPPacketEncrypter(uint32_t ssrcNew, const std::basic_string<uint8_t>& keysNew) {
		keys = keysNew;
		ssrc = ssrcNew;
	}

	std::basic_string_view<uint8_t> RTPPacketEncrypter::encryptPacket(DiscordCoreInternal::EncoderReturnData& audioData) {
		if (keys.size() > 0) {
			++sequence;
			timeStamp += static_cast<uint32_t>(audioData.sampleCount);
			static constexpr uint8_t headerSize{ 12 };
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
				data.at(x) = static_cast<uint8_t>(header[x]);
			}
			if (crypto_secretbox_easy(data.data() + headerSize, audioData.data.data(), audioData.data.size(), nonceForLibSodium, keys.data()) != 0) {
				return {};
			}
			return std::basic_string_view<uint8_t>{ data.data(), numOfBytes };
		}
		return {};
	}

	MovingAverager::MovingAverager(uint64_t collectionCountNew) {
		collectionCount = collectionCountNew;
	}

	MovingAverager MovingAverager::operator+=(int64_t value) {
		data.emplace_front(value);
		if (data.size() >= collectionCount) {
			data.pop_back();
		}
		return *this;
	}

	MovingAverager::operator float() {
		float returnData{};
		if (data.size() > 0) {
			for (auto& value: data) {
				returnData += static_cast<float>(value);
			}
			return returnData / static_cast<float>(data.size());
		} else {
			return 0.0f;
		}
	}

	VoiceConnectionBridge::VoiceConnectionBridge(DiscordCoreClient* discordCoreClientNew, std::basic_string<uint8_t>& encryptionKeyNew,
		StreamType streamType, const std::string& baseUrlNew, const uint16_t portNew, Snowflake guildIdNew,
		std::coroutine_handle<DiscordCoreAPI::CoRoutine<void, false>::promise_type>* tokenNew)
		: UDPConnection{ baseUrlNew, portNew, streamType, tokenNew } {
		discordCoreClient = discordCoreClientNew;
		encryptionKey = encryptionKeyNew;
		guildId = guildIdNew;
		token = tokenNew;
	}

	inline void VoiceConnectionBridge::applyGainRamp(int64_t sampleCount) {
		increment = (endGain - currentGain) / static_cast<float>(sampleCount);
		for (int64_t x = 0; x < sampleCount / DiscordCoreInternal::AudioMixer::byteBlocksPerRegister; ++x) {
			DiscordCoreInternal::AudioMixer::collectSingleRegister(upSampledVector + (x * DiscordCoreInternal::AudioMixer::byteBlocksPerRegister),
				downSampledVector + (x * DiscordCoreInternal::AudioMixer::byteBlocksPerRegister), currentGain, increment);
			currentGain += increment * static_cast<float>(DiscordCoreInternal::AudioMixer::byteBlocksPerRegister);
		}
	}

	bool compareUint8Strings(std::basic_string_view<uint8_t> stringToCheck, const char* wordToCheck) {
		std::string newString{};
		auto stringLength = std::char_traits<char>::length(wordToCheck);
		if (stringLength == stringToCheck.size()) {
			newString.resize(stringToCheck.size());
			std::memcpy(newString.data(), stringToCheck.data(), stringLength);
			return wordToCheck == newString;
		}
		return false;
	}

	void VoiceConnectionBridge::parseOutgoingVoiceData() {
		std::basic_string_view<uint8_t> buffer = getInputBuffer();
		if (compareUint8Strings(buffer, "goodbye") || compareUint8Strings(buffer, "connected1") || compareUint8Strings(buffer, "connecting")) {
			return;
		}
		if (buffer.size() > 0) {
			AudioFrameData frame{};
			frame += buffer;
			frame.type = AudioFrameType::RawPCM;
			discordCoreClient->getSongAPI(guildId).audioDataBuffer.send(std::move(frame));
		}
	}

	void VoiceConnectionBridge::disconnect() {
		if (streamType != StreamType::None) {
			outputBuffer.clear();
			char newStringArray[] = { "goodbye" };
			std::basic_string<uint8_t> newString{};
			auto stringSize = std::size(newStringArray);
			newString.resize(stringSize - 1);
			std::memcpy(newString.data(), newStringArray, stringSize - 1);
			writeData(newString);
			try {
				processIO();
			} catch (const DCAException& error) {
				MessagePrinter::printError<PrintMessageType::WebSocket>(error.what());
			}
		}
		UDPConnection::disconnect();
	}

	void VoiceConnectionBridge::handleAudioBuffer() {
		parseOutgoingVoiceData();
	}

	void VoiceConnectionBridge::mixAudio() {
		opus_int32 voiceUserCountReal{};
		size_t decodedSize{};
		std::uninitialized_value_construct(upSampledVector, upSampledVector + std::size(upSampledVector));
		for (auto& [key, value]: discordCoreClient->getVoiceConnection(guildId).voiceUsers) {
			std::basic_string_view<uint8_t> payload{ value->extractPayload() };
			if (payload.size() <= 44) {
				continue;
			} else {
				static constexpr uint64_t headerSize{ 12 };
				const uint64_t csrcCount{ static_cast<uint64_t>(payload[0]) & 0b0000'1111 };
				const uint64_t offsetToData{ headerSize + sizeof(uint32_t) * csrcCount };
				const uint64_t encryptedDataLength{ payload.size() - offsetToData };

				if (decryptedDataString.size() < encryptedDataLength) {
					decryptedDataString.resize(encryptedDataLength);
				}

				uint8_t nonce[24]{};
				for (uint64_t x = 0; x < headerSize; ++x) {
					nonce[x] = payload[x];
				}

				if (crypto_secretbox_open_easy(decryptedDataString.data(), payload.data() + offsetToData, encryptedDataLength, nonce,
						encryptionKey.data())) {
					continue;
				}

				std::basic_string_view newString{ decryptedDataString.data(), encryptedDataLength - crypto_secretbox_MACBYTES };

				if (static_cast<int8_t>(payload[0] >> 4) & 0b0001) {
					uint16_t extenstionLengthInWords{};
					std::memcpy(&extenstionLengthInWords, newString.data() + 2, sizeof(int16_t));
					extenstionLengthInWords = ntohs(extenstionLengthInWords);
					static constexpr uint64_t extensionHeaderLength{ sizeof(uint16_t) * 2 };
					uint64_t extensionLength{ sizeof(uint32_t) * extenstionLengthInWords };
					newString = newString.substr(extensionHeaderLength + extensionLength);
				}

				if (newString.size() > 44) {
					std::basic_string_view<opus_int16> decodedData{};
					try {
						decodedData = value->getDecoder().decodeData(newString);
					} catch (const DCAException& error) {
						MessagePrinter::printError<PrintMessageType::WebSocket>(error.what());
					}
					if (decodedData.size() > 0) {
						decodedSize = std::max(decodedSize, decodedData.size());
						++voiceUserCountReal;
						auto newPtr = decodedData.data();
						auto newerPtr = upSampledVector;
						for (uint64_t x = 0; x < decodedData.size() / DiscordCoreInternal::AudioMixer::byteBlocksPerRegister; ++x,
									  newPtr += DiscordCoreInternal::AudioMixer::byteBlocksPerRegister,
									  newerPtr += DiscordCoreInternal::AudioMixer::byteBlocksPerRegister) {
							DiscordCoreInternal::AudioMixer::combineSamples(newerPtr, newPtr);
						}
					}
				}
			}
		}
		if (decodedSize > 0) {
			voiceUserCountAverage += voiceUserCountReal;
			endGain = 1.0f / voiceUserCountAverage;
			applyGainRamp(decodedSize);
			if (resampleVector.size() < decodedSize * 2) {
				resampleVector.resize(decodedSize * 2);
			}
			std::memcpy(resampleVector.data(), downSampledVector, decodedSize * 2);
			writeData(std::basic_string_view<uint8_t>{ resampleVector.data(), static_cast<uint64_t>(decodedSize * 2) });
			currentGain = endGain;
		}
	}

	VoiceUDPConnection::VoiceUDPConnection(const std::string& baseUrlNew, uint16_t portNew, StreamType streamType, VoiceConnection* ptrNew,
		std::coroutine_handle<DiscordCoreAPI::CoRoutine<void, false>::promise_type>* tokenNew)
		: UDPConnection{ baseUrlNew, portNew, streamType, tokenNew } {
		voiceConnection = ptrNew;
	};

	void VoiceUDPConnection::disconnect() {
		if (streamType != StreamType::None) {
			outputBuffer.clear();
			char newStringArray[] = { "goodbye" };
			std::basic_string<uint8_t> newString{};
			auto stringSize = std::size(newStringArray);
			newString.resize(stringSize - 1);
			std::memcpy(newString.data(), newStringArray, stringSize - 1);
			writeData(newString);
			try {
				processIO();
			} catch (const DCAException& error) {
				MessagePrinter::printError<PrintMessageType::WebSocket>(error.what());
			}
		}
		UDPConnection::disconnect();
	}

	VoiceConnection::VoiceConnection(DiscordCoreClient* discordCoreClientNew, DiscordCoreInternal::WebSocketClient* baseShardNew,
		std::atomic_bool* doWeQuitNew)
		: WebSocketCore(&discordCoreClientNew->configManager, DiscordCoreInternal::WebSocketType::Voice) {
		dataOpCode = DiscordCoreInternal::WebSocketOpCode::Op_Text;
		msPerPacket = 20;
		samplesPerPacket = sampleRatePerSecond / 1000 * msPerPacket;
		activeState.store(VoiceActiveState::Connecting);
		configManager = &discordCoreClientNew->configManager;
		discordCoreClient = discordCoreClientNew;
		baseShard = baseShardNew;
		doWeQuit = doWeQuitNew;
	}

	Snowflake VoiceConnection::getChannelId() {
		return voiceConnectInitData.channelId;
	}

	void VoiceConnection::parseIncomingVoiceData(std::basic_string_view<uint8_t> rawDataBufferNew) {
		if ((72 <= (static_cast<int8_t>(rawDataBufferNew[1]) & 0b0111'1111) && ((static_cast<int8_t>(rawDataBufferNew[1]) & 0b0111'1111) <= 76)) ||
			rawDataBufferNew.size() <= 44) {
			return;
		}
		uint32_t speakerSsrc{};
		std::memcpy(&speakerSsrc, rawDataBufferNew.data() + 8, sizeof(uint32_t));
		speakerSsrc = ntohl(speakerSsrc);
		if (!voiceUsers.contains(speakerSsrc)) {
			voiceUsers.emplace(speakerSsrc, makeUnique<VoiceUser>());
		}
		voiceUsers[speakerSsrc]->insertPayload(rawDataBufferNew);
	}

	void VoiceConnection::connect(const VoiceConnectInitData& initData) {
		voiceConnectInitData = initData;
		if (taskThread.getStatus() != CoRoutineStatus::Running) {
			taskThread = runVoice();
		}
	}

	UnboundedMessageBlock<AudioFrameData>& VoiceConnection::getAudioBuffer() {
		return discordCoreClient->getSongAPI(voiceConnectInitData.guildId).audioDataBuffer;
	}

	void VoiceConnection::checkForAndSendHeartBeat(const bool isImmedate) {
		if (heartBeatStopWatch.hasTimePassed() || isImmedate) {
			DiscordCoreInternal::WebSocketMessageData<uint32_t> message{};
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

	void VoiceConnection::sendSpeakingMessage(const bool isSpeaking) {
		DiscordCoreInternal::WebSocketMessageData<DiscordCoreInternal::SendSpeakingData> data{};
		data.excludedKeys.emplace("t");
		data.excludedKeys.emplace("s");
		if (!isSpeaking) {
			data.d.type = DiscordCoreInternal::SendSpeakingType::None;
			sendSilence();
			if (udpConnection.processIO() != DiscordCoreInternal::ConnectionStatus::NO_Error) {
				onClosed();
				return;
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

	bool VoiceConnection::onMessageReceived(std::string_view data) {
		DiscordCoreInternal::WebSocketMessage message{};
		MessagePrinter::printSuccess<PrintMessageType::WebSocket>("Message received from Voice WebSocket: " + std::string{ data });
		parser.parseJson<true, true>(message, data);
		switch (static_cast<VoiceSocketOpCodes>(message.op)) {
			case VoiceSocketOpCodes::Ready_Server: {
				DiscordCoreInternal::WebSocketMessageData<VoiceSocketReadyData> dataNew{};
				parser.parseJson<true, true>(dataNew, data);
				audioSSRC = dataNew.d.ssrc;
				voiceIp = dataNew.d.ip;
				port = dataNew.d.port;
				for (auto& value: dataNew.d.modes) {
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
				parser.parseJson<true, true>(dataNew, data);
				for (auto value: dataNew.d.secretKey) {
					encryptionKey.push_back(static_cast<uint8_t>(value));
				}
				packetEncrypter = RTPPacketEncrypter{ audioSSRC, encryptionKey };
				connectionState.store(VoiceConnectionState::Collecting_Init_Data);
				break;
			}
			case VoiceSocketOpCodes::Speaking: {
				DiscordCoreInternal::WebSocketMessageData<SpeakingData> dataNew{};
				parser.parseJson<true, true>(dataNew, data);
				const uint32_t ssrc = dataNew.d.ssrc;
				auto userId = dataNew.d.userId;
				UniquePtr<VoiceUser> user{ makeUnique<VoiceUser>(userId) };
				if (!Users::getCachedUser({ .userId = user->getUserId() }).getFlagValue(UserFlags::Bot) ||
					voiceConnectInitData.streamInfo.streamBotAudio) {
					if (!voiceUsers.contains(ssrc)) {
						voiceUsers.emplace(ssrc, std::move(user));
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
				parser.parseJson<true, true>(dataNew, data);
				heartBeatStopWatch = StopWatch<Milliseconds>{ Milliseconds{ static_cast<uint32_t>(dataNew.d.heartBeatInterval) } };
				areWeHeartBeating = true;
				connectionState.store(VoiceConnectionState::Sending_Identify);
				currentState = DiscordCoreInternal::WebSocketState::Authenticated;
				haveWeReceivedHeartbeatAck = true;
				break;
			}
			case VoiceSocketOpCodes::Resumed: {
				connectionState.store(VoiceConnectionState::Initializing_DatagramSocket);
				break;
			}
			case VoiceSocketOpCodes::Client_Disconnect: {
				DiscordCoreInternal::WebSocketMessageData<VoiceUserDisconnectData> dataNew{};
				parser.parseJson<true, true>(dataNew, data);
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

	void VoiceConnection::connectInternal() {
		StopWatch<std::chrono::milliseconds> stopWatch{ 10000ms };
		if (currentReconnectTries >= maxReconnectTries) {
			doWeQuit->store(true);
			return;
		}
		if (streamSocket) {
			streamSocket->inputBuffer.clear();
			streamSocket->outputBuffer.clear();
		}
		areWeHeartBeating = false;
		switch (connectionState.load()) {
			case VoiceConnectionState::Collecting_Init_Data: {
				baseShard->voiceConnectionDataBufferMap[voiceConnectInitData.guildId.operator const uint64_t&()] = &voiceConnectionDataBuffer;
				baseShard->voiceConnectionDataBufferMap[voiceConnectInitData.guildId.operator const uint64_t&()]->clearContents();
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
				currentState = DiscordCoreInternal::WebSocketState::Upgrading;
				if (!WebSocketCore::connect(baseUrl, "/?v=4", 443)) {
					++currentReconnectTries;
					onClosed();
					return;
				}
				shard[0] = 0;
				shard[1] = 1;
				while (currentState != DiscordCoreInternal::WebSocketState::Collecting_Hello && !token.promise().areWeStopped()) {
					if (WebSocketCore::tcpConnection.processIO(10) != DiscordCoreInternal::ConnectionStatus::NO_Error) {
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
				while (connectionState.load() != VoiceConnectionState::Sending_Identify && !token.promise().areWeStopped()) {
					if (stopWatch.hasTimePassed()) {
						++currentReconnectTries;
						onClosed();
						return;
					}
					if (WebSocketCore::tcpConnection.processIO(10) != DiscordCoreInternal::ConnectionStatus::NO_Error) {
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
				while (connectionState.load() != VoiceConnectionState::Initializing_DatagramSocket && !token.promise().areWeStopped()) {
					if (stopWatch.hasTimePassed()) {
						++currentReconnectTries;
						onClosed();
						return;
					}
					if (WebSocketCore::tcpConnection.processIO(100) != DiscordCoreInternal::ConnectionStatus::NO_Error) {
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
				while (connectionState.load() != VoiceConnectionState::Collecting_Init_Data && !token.promise().areWeStopped()) {
					if (stopWatch.hasTimePassed()) {
						++currentReconnectTries;
						onClosed();
						return;
					}
					if (WebSocketCore::tcpConnection.processIO(10) != DiscordCoreInternal::ConnectionStatus::NO_Error) {
						++currentReconnectTries;
						onClosed();
						return;
					}
					std::this_thread::sleep_for(1ms);
				}
				baseShard->voiceConnectionDataBufferMap[voiceConnectInitData.guildId.operator const uint64_t&()]->clearContents();
				connectionState.store(VoiceConnectionState::Collecting_Init_Data);
				activeState.store(prevActiveState.load());
				if (voiceConnectInitData.streamInfo.type != StreamType::None) {
					streamSocket = makeUnique<VoiceConnectionBridge>(discordCoreClient, encryptionKey, voiceConnectInitData.streamInfo.type,
						voiceConnectInitData.streamInfo.address, voiceConnectInitData.streamInfo.port, voiceConnectInitData.guildId, &token);
					if (streamSocket->currentStatus != DiscordCoreInternal::ConnectionStatus::NO_Error) {
						++currentReconnectTries;
						onClosed();
						return;
					}
					play();
				}
				return;
			}
		}
	}

	CoRoutine<void, false> VoiceConnection::runVoice() {
		token = co_await NewThreadAwaitable<void, false>();
		StopWatch<std::chrono::milliseconds> stopWatch{ 20000ms };
		StopWatch<std::chrono::milliseconds> sendSilenceStopWatch{ 5000ms };
		while (!token.promise().areWeStopped() && !doWeQuit->load() && activeState.load() != VoiceActiveState::Exiting) {
			try {
				switch (activeState.load()) {
					case VoiceActiveState::Connecting: {
						sendSpeakingMessage(false);
						connectInternal();
						sendSpeakingMessage(true);
						break;
					}
					case VoiceActiveState::Stopped: {
						sendSpeakingMessage(false);
						xferAudioData.clearData();
						while (!token.promise().areWeStopped() && activeState.load() == VoiceActiveState::Stopped) {
							if (udpConnection.processIO() != DiscordCoreInternal::ConnectionStatus::NO_Error) {
								++currentReconnectTries;
								onClosed();
							}
							std::this_thread::sleep_for(1ms);
							if (!token.promise().areWeStopped() && VoiceConnection::areWeConnected()) {
								if (WebSocketCore::tcpConnection.processIO(10) != DiscordCoreInternal::ConnectionStatus::NO_Error) {
									++currentReconnectTries;
									onClosed();
								}
							}
							sendSilence();
							if (!token.promise().areWeStopped() && VoiceConnection::areWeConnected()) {
								checkForAndSendHeartBeat(false);
							}
						}
						break;
					}
					case VoiceActiveState::Paused: {
						while (!token.promise().areWeStopped() && activeState.load() == VoiceActiveState::Paused) {
							if (udpConnection.processIO() != DiscordCoreInternal::ConnectionStatus::NO_Error) {
								++currentReconnectTries;
								onClosed();
							}
							std::this_thread::sleep_for(1ms);
							if (!token.promise().areWeStopped() && VoiceConnection::areWeConnected()) {
								if (WebSocketCore::tcpConnection.processIO(10) != DiscordCoreInternal::ConnectionStatus::NO_Error) {
									++currentReconnectTries;
									onClosed();
								}
							}
							sendSilence();
							if (!token.promise().areWeStopped() && VoiceConnection::areWeConnected()) {
								checkForAndSendHeartBeat(false);
							}
						}
						break;
					}
					case VoiceActiveState::Playing: {
						sendSpeakingMessage(false);
						sendSpeakingMessage(true);
						xferAudioData.clearData();

						stopWatch.resetTimer();
						while (!token.promise().areWeStopped() && (!udpConnection.areWeStillConnected())) {
							if (stopWatch.hasTimePassed() || activeState.load() == VoiceActiveState::Exiting) {
								co_return;
							}
							std::this_thread::sleep_for(1ms);
							if (!token.promise().areWeStopped() && VoiceConnection::areWeConnected()) {
								if (WebSocketCore::tcpConnection.processIO(10) != DiscordCoreInternal::ConnectionStatus::NO_Error) {
									++currentReconnectTries;
									onClosed();
								}
							}
							if (!token.promise().areWeStopped() && VoiceConnection::areWeConnected()) {
								checkForAndSendHeartBeat(false);
							}
						}

						auto targetTime{ HRClock::now() + intervalCount };

						while (!token.promise().areWeStopped() && activeState.load() == VoiceActiveState::Playing) {
							uint64_t bytesPerSample{ 4 };
							if (!token.promise().areWeStopped() && VoiceConnection::areWeConnected()) {
								checkForAndSendHeartBeat(false);
							}
							discordCoreClient->getSongAPI(voiceConnectInitData.guildId).audioDataBuffer.tryReceive(xferAudioData);
							if ((doWeSkip.load() && xferAudioData.currentSize == 0)) {
								skipInternal();
							}

							AudioFrameType frameType{ xferAudioData.type };
							uint64_t frameSize{};
							if (xferAudioData.currentSize <= 0) {
								xferAudioData.clearData();
							} else if (xferAudioData.type == AudioFrameType::RawPCM) {
								intervalCount = Nanoseconds{ static_cast<uint64_t>(static_cast<double>(xferAudioData.currentSize / bytesPerSample) /
									static_cast<double>(sampleRatePerSecond) * static_cast<double>(nsPerSecond)) };
								uint64_t framesPerSecond = 1000 / msPerPacket;
								frameSize = std::min(bytesPerSample * static_cast<uint64_t>(sampleRatePerSecond) / framesPerSecond,
									xferAudioData.data.size());
							} else {
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
										returnData.data = { xferAudioData.data.data(), static_cast<uint64_t>(xferAudioData.currentSize) };
										returnData.sampleCount = 960;
										if (returnData.data.size() != 0) {
											frame = packetEncrypter.encryptPacket(returnData);
											xferAudioData.clearData();
										}
									} catch (const DCAException& error) {
										MessagePrinter::printError<PrintMessageType::WebSocket>(error.what());
										break;
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
							int64_t minimumFreeTimeForCheckingProcessIO{ static_cast<int64_t>(static_cast<double>(intervalCount.count()) * 0.60l) };
							if (VoiceConnection::areWeConnected()) {
								if (waitTimeCount >= minimumFreeTimeForCheckingProcessIO && !token.promise().areWeStopped()) {
									if (WebSocketCore::tcpConnection.processIO(0) != DiscordCoreInternal::ConnectionStatus::NO_Error) {
										++currentReconnectTries;
										onClosed();
									}
								}

							} else {
								onClosed();
							}

							waitTime = targetTime - HRClock::now();
							waitTimeCount = static_cast<int64_t>(static_cast<double>(waitTime.count()) * 0.85l);
							if (waitTimeCount > 0) {
								nanoSleep(waitTimeCount);
							}
							waitTime = targetTime - HRClock::now();
							waitTimeCount = waitTime.count();
							if (waitTimeCount > 0 && waitTimeCount < intervalCount.count()) {
								spinLock(waitTimeCount);
							}
							if (udpConnection.areWeStillConnected()) {
								udpConnection.writeData(frame);
								if (udpConnection.processIO() != DiscordCoreInternal::ConnectionStatus::NO_Error) {
									++currentReconnectTries;
									onClosed();
								}
							} else {
								xferAudioData.clearData();
								onClosed();
							}

							targetTime = HRClock::now() + intervalCount;

							if (streamSocket) {
								streamSocket->mixAudio();
								if (streamSocket->areWeStillConnected()) {
									if (streamSocket->processIO() != DiscordCoreInternal::ConnectionStatus::NO_Error) {
										++currentReconnectTries;
										std::this_thread::sleep_for(5s);
										onClosed();
									}
								} else {
									std::this_thread::sleep_for(5s);
									onClosed();
								}
							}
						}
						break;
					}
					case VoiceActiveState::Exiting: {
						co_return;
					}
				}
				if (token.promise().areWeStopped() || activeState == VoiceActiveState::Exiting) {
					co_return;
				}
				std::this_thread::sleep_for(1ms);
			} catch (const DCAException& error) {
				MessagePrinter::printError<PrintMessageType::WebSocket>(error.what());
			}
		}
	};

	void VoiceConnection::skipInternal(uint32_t currentRecursionDepth) {
		if (currentRecursionDepth >= 10) {
			return;
		}
		++currentRecursionDepth;
		SongCompletionEventData completionEventData{};
		completionEventData.guild = Guilds::getCachedGuild({ .guildId = voiceConnectInitData.guildId });
		completionEventData.wasItAFail = wasItAFail.load();
		if (xferAudioData.guildMemberId != 0) {
			completionEventData.guildMember =
				GuildMembers::getCachedGuildMember({ .guildMemberId = xferAudioData.guildMemberId, .guildId = voiceConnectInitData.guildId });
		} else {
			return;
		}
		try {
			xferAudioData.clearData();
			if (discordCoreClient->getSongAPI(voiceConnectInitData.guildId).onSongCompletionEvent.functions.size() > 0) {
				discordCoreClient->getSongAPI(voiceConnectInitData.guildId).onSongCompletionEvent(completionEventData);
			} else {
				stop();
			}
		} catch (const DCAException& error) {
			MessagePrinter::printError<PrintMessageType::WebSocket>(error.what());
			std::this_thread::sleep_for(150ms);
			skipInternal(currentRecursionDepth);
		}
	}

	bool VoiceConnection::areWeCurrentlyPlaying() {
		return (activeState.load() == VoiceActiveState::Playing) || activeState.load() == VoiceActiveState::Paused;
	}

	void VoiceUDPConnection::handleAudioBuffer() {
		if (voiceConnection->connectionState.load() == VoiceConnectionState::Initializing_DatagramSocket) {
		} else {
			std::basic_string_view<uint8_t> string = getInputBuffer();
			if (voiceConnection->streamSocket && voiceConnection->encryptionKey.size() > 0) {
				voiceConnection->parseIncomingVoiceData(string);
			}
		}
	}

	bool VoiceConnection::areWeConnected() {
		return WebSocketCore::areWeConnected() && udpConnection.areWeStillConnected() &&
			connectionState.load() == VoiceConnectionState::Collecting_Init_Data;
	}

	bool VoiceConnection::voiceConnect() {
		udpConnection = VoiceUDPConnection{ voiceIp, port, StreamType::None, this, &token };
		if (!udpConnection.areWeStillConnected()) {
			return false;
		}
		uint8_t packet[74]{};
		static constexpr uint16_t val1601{ 0x01 };
		static constexpr uint16_t val1602{ 70 };
		packet[0] = static_cast<uint8_t>(val1601 >> 8);
		packet[1] = static_cast<uint8_t>(val1601 >> 0);
		packet[2] = static_cast<uint8_t>(val1602 >> 8);
		packet[3] = static_cast<uint8_t>(val1602 >> 0);
		packet[4] = static_cast<uint8_t>(audioSSRC >> 24);
		packet[5] = static_cast<uint8_t>(audioSSRC >> 16);
		packet[6] = static_cast<uint8_t>(audioSSRC >> 8);
		packet[7] = static_cast<uint8_t>(audioSSRC);
		udpConnection.getInputBuffer();
		udpConnection.writeData(std::basic_string_view<uint8_t>{ packet, std::size(packet) });
		std::basic_string_view<uint8_t> inputStringFirst{};
		std::basic_string<uint8_t> inputString{};

		StopWatch<std::chrono::milliseconds> stopWatch{ 5500ms };
		while (inputStringFirst.size() < 74 && !doWeQuit->load() && activeState.load() != VoiceActiveState::Exiting) {
			if (udpConnection.processIO() != DiscordCoreInternal::ConnectionStatus::NO_Error) {
				++currentReconnectTries;
				onClosed();
				return false;
			}
			inputStringFirst = udpConnection.getInputBuffer();
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
		std::basic_string_view returnString{ inputStringFirst.data() + 8, inputString.size() };
		if (externalIp.size() < returnString.size()) {
			externalIp.resize(returnString.size());
		}
		std::memcpy(externalIp.data(), returnString.data(), returnString.size());
		voiceConnectionDataBuffer.clearContents();
		return true;
	}

	void VoiceConnection::sendSilence() {
		Jsonifier::Vector<std::basic_string<uint8_t>> frames{};
		uint8_t arrayNew[3]{};
		arrayNew[0] = uint8_t{ 0xf8 };
		arrayNew[1] = uint8_t{ 0xff };
		arrayNew[2] = uint8_t{ 0xfe };
		for (uint64_t x = 0; x < 5; ++x) {
			DiscordCoreInternal::EncoderReturnData frame{};
			frame.data = std::basic_string_view<uint8_t>{ arrayNew, 3 };
			frame.sampleCount = 3;
			auto packetNew = packetEncrypter.encryptPacket(frame);
			frames.emplace_back(std::basic_string<uint8_t>{ packetNew.data(), packetNew.size() });
		}
		for (auto& value: frames) {
			udpConnection.writeData(value);
			if (udpConnection.processIO() != DiscordCoreInternal::ConnectionStatus::NO_Error) {
				++currentReconnectTries;
				onClosed();
				return;
			}
		}
	}

	bool VoiceConnection::pauseToggle() {
		if (activeState.load() == VoiceActiveState::Paused) {
			activeState.store(VoiceActiveState::Playing);
			return true;
		} else if (activeState.load() == VoiceActiveState::Playing) {
			activeState.store(VoiceActiveState::Paused);
			return true;
		} else {
			return false;
		}
	}

	void VoiceConnection::disconnect() {
		activeState.store(VoiceActiveState::Exiting);
		if (taskThread.getStatus() == CoRoutineStatus::Running) {
			taskThread.cancel();
		}
		if (streamSocket) {
			streamSocket->disconnect();
			streamSocket.reset(nullptr);
		};
		udpConnection.disconnect();
		WebSocketCore::disconnect();
		areWeHeartBeating = false;
		currentReconnectTries = 0;
		voiceUsers.clear();
		prevActiveState.store(VoiceActiveState::Stopped);
		activeState.store(VoiceActiveState::Connecting);
		connectionState.store(VoiceConnectionState::Collecting_Init_Data);
		currentState.store(DiscordCoreInternal::WebSocketState::Disconnected);
		discordCoreClient->getSongAPI(voiceConnectInitData.guildId).audioDataBuffer.clearContents();
	}

	void VoiceConnection::onClosed() {
		connectionState.store(VoiceConnectionState::Collecting_Init_Data);
		if (activeState.load() != VoiceActiveState::Exiting && currentReconnectTries < maxReconnectTries) {
			if (activeState.load() != VoiceActiveState::Connecting) {
				prevActiveState.store(activeState.load());
			}
			activeState.store(VoiceActiveState::Connecting);
			if (streamSocket) {
				streamSocket->disconnect();
			}
			udpConnection.disconnect();
		} else if (currentReconnectTries >= maxReconnectTries) {
			VoiceConnection::disconnect();
		}
	}

	bool VoiceConnection::stop() {
		activeState.store(VoiceActiveState::Stopped);
		doWeSkip.store(false);
		return true;
	}

	bool VoiceConnection::play() {
		activeState.store(VoiceActiveState::Playing);
		return true;
	}

	bool VoiceConnection::skip(bool wasItAFailNew) {
		if (activeState.load() == VoiceActiveState::Stopped) {
			return false;
		} else {
			wasItAFail.store(wasItAFailNew);
			doWeSkip.store(true);
			return true;
		}
	}
}
