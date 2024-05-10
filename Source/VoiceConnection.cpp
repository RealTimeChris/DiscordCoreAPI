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

#include <discordcoreapi/Utilities/ISADetection.hpp>
#include <discordcoreapi/VoiceConnection.hpp>
#include <discordcoreapi/DiscordCoreClient.hpp>
#include <discordcoreapi/Utilities/UDPConnection.hpp>

namespace jsonifier {

	template<> struct core<discord_core_api::voice_session_description_data> {
		using value_type				 = discord_core_api::voice_session_description_data;
		static constexpr auto parseValue = createValue("secret_key", &value_type::secretKey);
	};

	template<> struct core<discord_core_api::discord_core_internal::websocket_message_data<discord_core_api::speaking_data>> {
		using value_type				 = discord_core_api::discord_core_internal::websocket_message_data<discord_core_api::speaking_data>;
		static constexpr auto parseValue = createValue("d", &value_type::d);
	};

	template<> struct core<discord_core_api::speaking_data> {
		using value_type				 = discord_core_api::speaking_data;
		static constexpr auto parseValue = createValue("ssrc", &value_type::ssrc, "user_id", &value_type::userId);
	};

	template<> struct core<discord_core_api::voice_connection_hello_data> {
		using value_type				 = discord_core_api::voice_connection_hello_data;
		static constexpr auto parseValue = createValue("heartbeat_interval", &value_type::heartBeatInterval);
	};

	template<> struct core<discord_core_api::voice_user_disconnect_data> {
		using value_type				 = discord_core_api::voice_user_disconnect_data;
		static constexpr auto parseValue = createValue("user_id", &value_type::userId);
	};

	template<> struct core<discord_core_api::voice_socket_ready_data> {
		using value_type				 = discord_core_api::voice_socket_ready_data;
		static constexpr auto parseValue = createValue("modes", &value_type::modes, "ip", &value_type::ip, "port", &value_type::port, "ssrc", &value_type::ssrc);
	};

}

namespace discord_core_api {

	static DCA_INLINE thread_local discord_core_internal::audio_mixer audioMixer{};

	voice_user::voice_user(snowflake userIdNew) {
		userId = userIdNew;
	}

	voice_user& voice_user::operator=(voice_user&& other) noexcept {
		payloads = std::move(other.payloads);
		decoder	 = std::move(other.decoder);
		userId	 = other.userId;
		return *this;
	}

	discord_core_internal::opus_decoder_wrapper& voice_user::getDecoder() {
		return decoder;
	}

	void voice_user::insertPayload(jsonifier::string_view_base<uint8_t> data) {
		payloads.writeData(data.data(), data.size());
	}

	jsonifier::string_view_base<uint8_t> voice_user::extractPayload() {
		return payloads.readData();
	}

	snowflake voice_user::getUserId() {
		return userId;
	}

	rtp_packet_encrypter::rtp_packet_encrypter(uint32_t ssrcNew, const jsonifier::string_base<uint8_t>& keysNew) {
		keys = keysNew;
		ssrc = ssrcNew;
	}

	jsonifier::string_view_base<uint8_t> rtp_packet_encrypter::encryptPacket(discord_core_internal::encoder_return_data& audioData) {
		if (keys.size() > 0) {
			++sequence;
			timeStamp += static_cast<uint32_t>(audioData.sampleCount);
			static constexpr uint8_t headerSize{ 12 };
			static constexpr uint8_t version{ 0x80 };
			static constexpr uint8_t flags{ 0x78 };
			uint8_t header[headerSize]{};
			discord_core_internal::storeBits(header, version);
			discord_core_internal::storeBits(header + 1, flags);
			discord_core_internal::storeBits(header + 2, sequence);
			discord_core_internal::storeBits(header + 4, timeStamp);
			discord_core_internal::storeBits(header + 8, ssrc);
			uint8_t nonceForLibSodium[crypto_secretbox_NONCEBYTES]{};
			for (int8_t x = 0; x < headerSize; ++x) {
				nonceForLibSodium[x] = header[x];
			}
			const uint64_t numOfBytes{ headerSize + audioData.data.size() + crypto_secretbox_MACBYTES };
			if (data.size() < numOfBytes) {
				data.resize(numOfBytes);
			}
			for (uint8_t x = 0; x < headerSize; ++x) {
				data.at(x) = static_cast<uint8_t>(header[x]);
			}
			if (crypto_secretbox_easy(data.data() + headerSize, audioData.data.data(), audioData.data.size(), nonceForLibSodium, keys.data()) != 0) {
				return {};
			}
			return jsonifier::string_view_base<uint8_t>{ data.data(), numOfBytes };
		}
		return {};
	}

	moving_averager::moving_averager(uint64_t collectionCountNew) {
		collectionCount = collectionCountNew;
	}

	moving_averager moving_averager::operator+=(int64_t value) {
		values.emplace_front(value);
		if (values.size() >= collectionCount) {
			values.pop_back();
		}
		return *this;
	}

	moving_averager::operator float() {
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

	voice_connection_bridge::voice_connection_bridge(unordered_map<uint64_t, unique_ptr<voice_user>>* voiceUsersPtrNew, jsonifier::string_base<uint8_t>& encryptionKeyNew,
		stream_type streamType, const jsonifier::string& baseUrlNew, const uint16_t portNew, snowflake guildIdNew,
		std::coroutine_handle<discord_core_api::co_routine<void, false>::promise_type>* tokenNew)
		: udp_connection{ baseUrlNew, portNew, streamType, tokenNew } {
		voiceUsersPtr = voiceUsersPtrNew;
		encryptionKey = encryptionKeyNew;
		guildId		  = guildIdNew;
		token		  = tokenNew;
	}

	DCA_INLINE void voice_connection_bridge::applyGainRamp(int64_t sampleCount) {
		increment = (endGain - currentGain) / static_cast<float>(sampleCount);
		for (int64_t x = 0; x < sampleCount / audioMixer.byteBlocksPerRegister; ++x) {
			audioMixer.collectSingleRegister(upSampledVector.data() + (x * audioMixer.byteBlocksPerRegister), downSampledVector.data() + (x * audioMixer.byteBlocksPerRegister),
				currentGain, increment);
			currentGain += increment * static_cast<float>(audioMixer.byteBlocksPerRegister);
		}
	}

	bool compareUint8Strings(jsonifier::string_view_base<uint8_t> stringToCheck, const char* wordToCheck) {
		jsonifier::string newString{};
		auto stringLength = std::char_traits<char>::length(wordToCheck);
		if (stringLength == stringToCheck.size()) {
			newString.resize(stringToCheck.size());
			std::memcpy(newString.data(), stringToCheck.data(), stringLength);
			return wordToCheck == newString;
		}
		return false;
	}

	void voice_connection_bridge::parseOutgoingVoiceData() {
		jsonifier::string_view_base<uint8_t> buffer = getInputBuffer();
		if (compareUint8Strings(buffer, "goodbye") || compareUint8Strings(buffer, "connected1") || compareUint8Strings(buffer, "connecting")) {
			return;
		}
		if (buffer.size() > 0) {
			audio_frame_data frame{};
			frame += buffer;
			frame.type = audio_frame_type::raw_pcm;
			discord_core_client::getInstance()->getSongAPI(guildId).audioDataBuffer.send(std::move(frame));
		}
	}

	void voice_connection_bridge::disconnect() {
		if (streamType != stream_type::none) {
			outputBuffer.clear();
			static constexpr char newStringArray[]{ "goodbye" };
			jsonifier::string_base<uint8_t> newString{};
			static constexpr auto stringSize = std::size(newStringArray);
			newString.resize(stringSize - 1);
			std::memcpy(newString.data(), newStringArray, stringSize - 1);
			writeData(newString);
			try {
				processIO();
			} catch (const dca_exception& error) {
				message_printer::printError<print_message_type::websocket>(error.what());
			}
		}
		udp_connection::disconnect();
	}

	void voice_connection_bridge::handleAudioBuffer() {
		parseOutgoingVoiceData();
	}

	void voice_connection_bridge::mixAudio() {
		opus_int32 voiceUserCountReal{};
		int64_t decodedSize{};
		std::fill(upSampledVector.data(), upSampledVector.data() + upSampledVector.size(), 0);
		for (auto& [key, value]: *voiceUsersPtr) {
			jsonifier::string_view_base<uint8_t> payload{ value->extractPayload() };
			if (payload.size() <= 44) {
				continue;
			} else {
				static constexpr uint64_t headerSize{ 12 };
				const uint64_t csrcCount{ static_cast<uint64_t>(payload.at(0)) & 0b0000'1111 };
				const uint64_t offsetToData{ headerSize + sizeof(uint32_t) * csrcCount };
				const uint64_t encryptedDataLength{ payload.size() - offsetToData };

				if (decryptedDataString.size() < encryptedDataLength) {
					decryptedDataString.resize(encryptedDataLength);
				}

				uint8_t nonce[24]{};
				for (uint64_t x = 0; x < headerSize; ++x) {
					nonce[x] = payload[x];
				}

				if (crypto_secretbox_open_easy(decryptedDataString.data(), payload.data() + offsetToData, encryptedDataLength, nonce, encryptionKey.data())) {
					continue;
				}

				jsonifier::string_view_base newString{ decryptedDataString.data(), encryptedDataLength - crypto_secretbox_MACBYTES };

				if (static_cast<int8_t>(payload[0] >> 4) & 0b0001) {
					uint16_t extenstionLengthInWords{};
					std::memcpy(&extenstionLengthInWords, newString.data() + 2, sizeof(int16_t));
					extenstionLengthInWords = ntohs(extenstionLengthInWords);
					const uint64_t extensionLength{ sizeof(uint32_t) * extenstionLengthInWords };
					const uint64_t extensionHeaderLength{ sizeof(uint16_t) * 2 };
					newString = newString.substr(extensionHeaderLength + extensionLength);
				}

				if (newString.size() > 44) {
					jsonifier::string_view_base<opus_int16> decodedData{};
					try {
						decodedData = value->getDecoder().decodeData(newString);
					} catch (const dca_exception& error) {
						message_printer::printError<print_message_type::websocket>(error.what());
					}
					if (decodedData.size() > 0) {
						decodedSize = static_cast<int64_t>(std::max(static_cast<uint64_t>(decodedSize), decodedData.size()));
						++voiceUserCountReal;
						auto newPtr	  = decodedData.data();
						auto newerPtr = upSampledVector.data();
						for (uint64_t x = 0; x < decodedData.size() / audioMixer.byteBlocksPerRegister;
							 ++x, newPtr += audioMixer.byteBlocksPerRegister, newerPtr += audioMixer.byteBlocksPerRegister) {
							audioMixer.combineSamples(newPtr, newerPtr);
						}
					}
				}
			}
		}
		if (decodedSize > 0) {
			voiceUserCountAverage += voiceUserCountReal;
			endGain = 1.0f / voiceUserCountAverage;
			applyGainRamp(decodedSize);
			if (resampleVector.size() < static_cast<uint64_t>(decodedSize) * 2) {
				resampleVector.resize(static_cast<uint64_t>(decodedSize) * 2);
			}
			std::memcpy(resampleVector.data(), downSampledVector.data(), static_cast<uint64_t>(decodedSize) * 2);
			writeData(jsonifier::string_view_base<uint8_t>{ resampleVector.data(), static_cast<uint64_t>(decodedSize * 2) });
			currentGain = endGain;
		}
	}

	voice_udpconnection::voice_udpconnection(const jsonifier::string& baseUrlNew, uint16_t portNew, stream_type streamType, voice_connection* ptrNew,
		std::coroutine_handle<discord_core_api::co_routine<void, false>::promise_type>* tokenNew)
		: udp_connection{ baseUrlNew, portNew, streamType, tokenNew } {
		voiceConnection = ptrNew;
	};

	void voice_udpconnection::disconnect() {
		if (streamType != stream_type::none) {
			outputBuffer.clear();
			static constexpr char newStringArray[] = { "goodbye" };
			jsonifier::string_base<uint8_t> newString{};
			auto stringSize = std::size(newStringArray);
			newString.resize(stringSize - 1);
			std::memcpy(newString.data(), newStringArray, stringSize - 1);
			writeData(newString);
			try {
				processIO();
			} catch (const dca_exception& error) {
				message_printer::printError<print_message_type::websocket>(error.what());
			}
		}
		udp_connection::disconnect();
	}

	voice_connection::voice_connection(discord_core_internal::websocket_client* baseShardNew, std::atomic_bool* doWeQuitNew)
		: websocket_core(&discord_core_client::getInstance()->configManager, discord_core_internal::websocket_type::voice) {
		dataOpCode		 = discord_core_internal::websocket_op_code::Op_Text;
		msPerPacket		 = 20;
		samplesPerPacket = sampleRatePerSecond / 1000 * msPerPacket;
		configManager	 = &discord_core_client::getInstance()->configManager;
		baseShard		 = baseShardNew;
		doWeQuit		 = doWeQuitNew;
	}

	snowflake voice_connection::getChannelId() {
		return voiceConnectInitData.channelId;
	}

	void voice_connection::parseIncomingVoiceData(jsonifier::string_view_base<uint8_t> rawDataBufferNew) {
		if ((72 <= (static_cast<int8_t>(rawDataBufferNew.at(1)) & 0b0111'1111) && ((static_cast<int8_t>(rawDataBufferNew.at(1)) & 0b0111'1111) <= 76)) ||
			rawDataBufferNew.size() <= 44) {
			return;
		}
		uint32_t speakerSsrc{};
		std::memcpy(&speakerSsrc, rawDataBufferNew.data() + 8, sizeof(uint32_t));
		speakerSsrc = ntohl(speakerSsrc);
		if (!voiceUsers.contains(speakerSsrc)) {
			voiceUsers.emplace(speakerSsrc, makeUnique<voice_user>());
		}
		voiceUsers[speakerSsrc]->insertPayload(rawDataBufferNew);
	}

	void voice_connection::connect(const voice_connect_init_data& initData) {
		voiceConnectInitData = initData;
		prevActiveState.store(voice_active_state::stopped, std::memory_order_release);
		activeState.store(voice_active_state::connecting, std::memory_order_release);
		if (taskThread.getStatus() != co_routine_status::running) {
			taskThread = runVoice();
		} else {
			activeState.store(voice_active_state::connecting, std::memory_order_release);
		}
	}

	unbounded_message_block<audio_frame_data>& voice_connection::getAudioBuffer() {
		return discord_core_client::getInstance()->getSongAPI(voiceConnectInitData.guildId).audioDataBuffer;
	}

	void voice_connection::checkForAndSendHeartBeat(const bool isImmedate) {
		if (heartBeatStopWatch.hasTimeElapsed() || isImmedate) {
			discord_core_internal::websocket_message_data<uint32_t> message{};
			message.jsonifierExcludedKeys.emplace("T");
			message.jsonifierExcludedKeys.emplace("s");
			jsonifier::string_base<uint8_t> string{};
			message.d  = static_cast<uint32_t>(std::chrono::duration_cast<nanoseconds>(sys_clock::now().time_since_epoch()).count());
			message.op = 3;
			parser.serializeJson(message, string);
			createHeader(string, dataOpCode);
			if (!sendMessage(string, true)) {
				onClosed();
				return;
			}
			if (activeState.load(std::memory_order_acquire) == voice_active_state::paused || activeState.load(std::memory_order_acquire) == voice_active_state::stopped) {
				sendSilence();
			}
			haveWeReceivedHeartbeatAck = false;
			heartBeatStopWatch.reset();
		}
	}

	void voice_connection::sendSpeakingMessage(const bool isSpeaking) {
		discord_core_internal::websocket_message_data<discord_core_internal::send_speaking_data> data{};
		data.jsonifierExcludedKeys.emplace("T");
		data.jsonifierExcludedKeys.emplace("s");
		if (!isSpeaking) {
			data.d.type = discord_core_internal::send_speaking_type::None;
			sendSilence();
			if (udpConnection.processIO() != discord_core_internal::connection_status::NO_Error) {
				onClosed();
				return;
			}
		} else {
			data.d.type = discord_core_internal::send_speaking_type::Microphone;
		}
		data.d.delay = 0;
		data.d.ssrc	 = audioSSRC;
		data.op		 = 5;
		jsonifier::string_base<uint8_t> string{};
		parser.serializeJson(data, string);
		createHeader(string, dataOpCode);
		sendMessage(string, true);
	}

	bool voice_connection::onMessageReceived(jsonifier::string_view_base<uint8_t> data) {
		discord_core_internal::websocket_message message{};
		message_printer::printSuccess<print_message_type::websocket>("message received from voice websocket: " + jsonifier::string{ data });
		parser.parseJson(message, data);
		switch (static_cast<voice_socket_op_codes>(message.op)) {
			case voice_socket_op_codes::Ready_Server: {
				discord_core_internal::websocket_message_data<voice_socket_ready_data> dataNew{};
				parser.parseJson(dataNew, data);
				audioSSRC = dataNew.d.ssrc;
				voiceIp	  = dataNew.d.ip;
				port	  = dataNew.d.port;
				for (auto& value: dataNew.d.modes) {
					if (value == "xsalsa20_poly1305") {
						audioEncryptionMode = value;
					}
				}
				connectionState.store(voice_connection_state::Initializing_DatagramSocket, std::memory_order_release);
				break;
			}
			case voice_socket_op_codes::Session_Description: {
				discord_core_internal::websocket_message_data<voice_session_description_data> dataNew{};
				encryptionKey.clear();
				parser.parseJson(dataNew, data);
				for (auto& value: dataNew.d.secretKey) {
					encryptionKey.emplace_back(static_cast<uint8_t>(value));
				}
				packetEncrypter = rtp_packet_encrypter{ audioSSRC, encryptionKey };
				connectionState.store(voice_connection_state::Collecting_Init_Data, std::memory_order_release);
				break;
			}
			case voice_socket_op_codes::speaking: {
				discord_core_internal::websocket_message_data<speaking_data> dataNew{};
				parser.parseJson(dataNew, data);
				const uint32_t ssrc = dataNew.d.ssrc;
				auto userId			= dataNew.d.userId;
				unique_ptr<voice_user> user{ makeUnique<voice_user>(userId) };
				if (voiceConnectInitData.streamInfo.type != stream_type::none &&
					(voiceConnectInitData.streamInfo.streamBotAudio || !users::getCachedUser({ .userId = user->getUserId() }).getFlagValue(user_flags::Bot))) {
					if (!voiceUsers.contains(ssrc)) {
						voiceUsers.emplace(ssrc, std::move(user));
					}
				}
				break;
			}
			case voice_socket_op_codes::Heartbeat_ACK: {
				haveWeReceivedHeartbeatAck = true;
				break;
			}
			case voice_socket_op_codes::hello: {
				discord_core_internal::websocket_message_data<voice_connection_hello_data> dataNew{};
				parser.parseJson(dataNew, data);
				heartBeatStopWatch = stop_watch<milliseconds>{ dataNew.d.heartBeatInterval };
				heartBeatStopWatch.reset();
				areWeHeartBeating = true;
				connectionState.store(voice_connection_state::Sending_Identify, std::memory_order_release);
				currentState.store(discord_core_internal::websocket_state::authenticated, std::memory_order_release);
				haveWeReceivedHeartbeatAck = true;
				break;
			}
			case voice_socket_op_codes::resumed: {
				connectionState.store(voice_connection_state::Initializing_DatagramSocket, std::memory_order_release);
				break;
			}
			case voice_socket_op_codes::Client_Disconnect: {
				discord_core_internal::websocket_message_data<voice_user_disconnect_data> dataNew{};
				parser.parseJson(dataNew, data);
				const auto userId = dataNew.d.userId;
				for (auto& [key, value]: voiceUsers) {
					if (userId == value->getUserId()) {
						voiceUsers.erase(key);
						break;
					}
				}
				break;
			}
			case voice_socket_op_codes::identify: {
				[[fallthrough]];
			}
			case voice_socket_op_codes::Select_Protocol: {
				[[fallthrough]];
			}
			case voice_socket_op_codes::heartbeat: {
				[[fallthrough]];
			}
			case voice_socket_op_codes::resume: {
				break;
			}
		}
		return true;
	}

	void voice_connection::connectInternal() {
		stop_watch<milliseconds> stopWatch{ 10000ms };
		stopWatch.reset();
		if (currentReconnectTries >= maxReconnectTries) {
			doWeQuit->store(true, std::memory_order_release);
			return;
		}
		if (streamSocket) {
			streamSocket->inputBuffer.clear();
			streamSocket->outputBuffer.clear();
		}
		areWeHeartBeating = false;
		switch (connectionState.load(std::memory_order_acquire)) {
			case voice_connection_state::Collecting_Init_Data: {
				baseShard->voiceConnectionDataBufferMap[voiceConnectInitData.guildId.operator const uint64_t&()] = &voiceConnectionDataBuffer;
				baseShard->voiceConnectionDataBufferMap[voiceConnectInitData.guildId.operator const uint64_t&()]->clearContents();
				baseShard->getVoiceConnectionData(voiceConnectInitData);

				if (waitForTimeToPass(voiceConnectionDataBuffer, voiceConnectionData, 10000)) {
					onClosed();
					return;
				}
				baseUrl = voiceConnectionData.endPoint.substr(0, voiceConnectionData.endPoint.find(":"));
				connectionState.store(voice_connection_state::Initializing_WebSocket, std::memory_order_release);
				connectInternal();
				break;
			}
			case voice_connection_state::Initializing_WebSocket: {
				currentState.store(discord_core_internal::websocket_state::upgrading, std::memory_order_release);
				if (!websocket_core::connect(baseUrl, "/?v=4", 443)) {
					onClosed();
					return;
				}
				shard.at(0) = 0;
				shard.at(1) = 1;
				while (currentState.load(std::memory_order_acquire) != discord_core_internal::websocket_state::Collecting_Hello && !token.promise().stopRequested()) {
					if (websocket_core::tcpConnection.processIO(10) != discord_core_internal::connection_status::NO_Error) {
						onClosed();
						return;
					}
				}
				connectionState.store(voice_connection_state::Collecting_Hello, std::memory_order_release);
				connectInternal();
				break;
			}
			case voice_connection_state::Collecting_Hello: {
				stopWatch.reset();
				while (connectionState.load(std::memory_order_acquire) != voice_connection_state::Sending_Identify && !token.promise().stopRequested()) {
					if (stopWatch.hasTimeElapsed()) {
						onClosed();
						return;
					}
					if (websocket_core::tcpConnection.processIO(10) != discord_core_internal::connection_status::NO_Error) {
						onClosed();
						return;
					}
					std::this_thread::sleep_for(1ms);
				}
				currentReconnectTries = 0;
				connectInternal();
				break;
			}
			case voice_connection_state::Sending_Identify: {
				haveWeReceivedHeartbeatAck = true;
				discord_core_internal::websocket_message_data<discord_core_internal::voice_identify_data> data{};
				data.jsonifierExcludedKeys.emplace("T");
				data.jsonifierExcludedKeys.emplace("s");
				data.d.serverId	 = voiceConnectInitData.guildId.operator jsonifier::string();
				data.d.sessionId = voiceConnectionData.sessionId;
				data.d.token	 = voiceConnectionData.token;
				data.d.userId	 = voiceConnectInitData.userId;
				data.op			 = 0;
				data.s			 = 0;
				jsonifier::string_base<uint8_t> string{};
				parser.serializeJson(data, string);
				createHeader(string, dataOpCode);
				if (!websocket_core::sendMessage(string, true)) {
					onClosed();
					return;
				}
				connectionState.store(voice_connection_state::Collecting_Ready, std::memory_order_release);
				connectInternal();
				break;
			}
			case voice_connection_state::Collecting_Ready: {
				stopWatch.reset();
				while (connectionState.load(std::memory_order_acquire) != voice_connection_state::Initializing_DatagramSocket && !token.promise().stopRequested()) {
					if (stopWatch.hasTimeElapsed()) {
						onClosed();
						return;
					}
					if (websocket_core::tcpConnection.processIO(100) != discord_core_internal::connection_status::NO_Error) {
						onClosed();
						return;
					}
					std::this_thread::sleep_for(1ms);
				}
				connectInternal();
				break;
			}
			case voice_connection_state::Initializing_DatagramSocket: {
				if (!voiceConnect()) {
					onClosed();
					return;
				}
				connectionState.store(voice_connection_state::Sending_Select_Protocol, std::memory_order_release);
				connectInternal();
				break;
			}
			case voice_connection_state::Sending_Select_Protocol: {
				discord_core_internal::websocket_message_data<discord_core_internal::voice_socket_protocol_payload_data> data{};
				data.jsonifierExcludedKeys.emplace("T");
				data.jsonifierExcludedKeys.emplace("s");
				data.d.data.mode	= audioEncryptionMode;
				data.d.data.address = externalIp;
				data.d.data.port	= port;
				data.op				= 1;
				jsonifier::string_base<uint8_t> string{};
				parser.serializeJson(data, string);
				createHeader(string, dataOpCode);
				if (!websocket_core::sendMessage(string, true)) {
					;
					onClosed();
					return;
				}
				connectionState.store(voice_connection_state::Collecting_Session_Description, std::memory_order_release);
				connectInternal();
				break;
			}
			case voice_connection_state::Collecting_Session_Description: {
				stopWatch.reset();
				while (connectionState.load(std::memory_order_acquire) != voice_connection_state::Collecting_Init_Data && !token.promise().stopRequested()) {
					if (stopWatch.hasTimeElapsed()) {
						onClosed();
						return;
					}
					if (websocket_core::tcpConnection.processIO(10) != discord_core_internal::connection_status::NO_Error) {
						onClosed();
						return;
					}
					std::this_thread::sleep_for(1ms);
				}
				baseShard->voiceConnectionDataBufferMap[voiceConnectInitData.guildId.operator const uint64_t&()]->clearContents();
				connectionState.store(voice_connection_state::Collecting_Init_Data, std::memory_order_release);
				activeState.store(prevActiveState.load(std::memory_order_acquire), std::memory_order_release);
				if (voiceConnectInitData.streamInfo.type != stream_type::none) {
					streamSocket = makeUnique<voice_connection_bridge>(&voiceUsers, encryptionKey, voiceConnectInitData.streamInfo.type, voiceConnectInitData.streamInfo.address,
						voiceConnectInitData.streamInfo.port, voiceConnectInitData.guildId, &token);
					if (streamSocket->currentStatus != discord_core_internal::connection_status::NO_Error) {
						onClosed();
						return;
					}
					play();
				}
				return;
			}
		}
	}

	co_routine<void, false> voice_connection::runVoice() {
		token = co_await newThreadAwaitable<void, false>();
		stop_watch<milliseconds> stopWatch{ 20000ms };
		stopWatch.reset();
		stop_watch<milliseconds> sendSilenceStopWatch{ 5000ms };
		sendSilenceStopWatch.reset();
		while (!token.promise().stopRequested() && !doWeQuit->load(std::memory_order_acquire) && activeState.load(std::memory_order_acquire) != voice_active_state::exiting) {
			try {
				switch (activeState.load(std::memory_order_acquire)) {
					case voice_active_state::connecting: {
						connectInternal();
						sendSpeakingMessage(false);
						break;
					}
					case voice_active_state::stopped: {
						sendSpeakingMessage(false);
						xferAudioData.clearData();
						while (!token.promise().stopRequested() && activeState.load(std::memory_order_acquire) == voice_active_state::stopped) {
							if (udpConnection.processIO() != discord_core_internal::connection_status::NO_Error) {
								onClosed();
							}
							std::this_thread::sleep_for(1ms);
							if (!token.promise().stopRequested() && voice_connection::areWeConnected()) {
								if (websocket_core::tcpConnection.processIO(10) != discord_core_internal::connection_status::NO_Error) {
									onClosed();
								} else if (!websocket_core::areWeConnected()) {
									onClosed();
								}
							} else {
								onClosed();
							}
							if (!token.promise().stopRequested() && voice_connection::areWeConnected()) {
								checkForAndSendHeartBeat(false);
							}
						}
						break;
					}
					case voice_active_state::paused: {
						sendSpeakingMessage(false);
						while (!token.promise().stopRequested() && activeState.load(std::memory_order_acquire) == voice_active_state::paused) {
							if (udpConnection.processIO() != discord_core_internal::connection_status::NO_Error) {
								onClosed();
							}
							std::this_thread::sleep_for(1ms);
							if (!token.promise().stopRequested() && voice_connection::areWeConnected()) {
								if (websocket_core::tcpConnection.processIO(10) != discord_core_internal::connection_status::NO_Error) {
									onClosed();
								} else if (!websocket_core::areWeConnected()) {
									onClosed();
								}
							} else {
								onClosed();
							}
							if (!token.promise().stopRequested() && voice_connection::areWeConnected()) {
								checkForAndSendHeartBeat(false);
							}
						}
						break;
					}
					case voice_active_state::playing: {
						sendSpeakingMessage(false);
						sendSpeakingMessage(true);
						sendSilence();
						xferAudioData.clearData();

						auto targetTime{ sys_clock::now() + intervalCount };

						while (!token.promise().stopRequested() && activeState.load(std::memory_order_acquire) == voice_active_state::playing) {
							int64_t bytesPerSample{ 4 };
							if (!token.promise().stopRequested() && voice_connection::areWeConnected()) {
								checkForAndSendHeartBeat(false);
							}
							discord_core_client::getInstance()->getSongAPI(voiceConnectInitData.guildId).audioDataBuffer.tryReceive(xferAudioData);
							if ((doWeSkip.load(std::memory_order_acquire) && xferAudioData.currentSize == 0)) {
								skipInternal();
							}

							audio_frame_type frameType{ xferAudioData.type };
							uint64_t frameSize{};
							if (xferAudioData.currentSize <= 0) {
								xferAudioData.clearData();
							} else if (xferAudioData.type == audio_frame_type::raw_pcm) {
								intervalCount			 = nanoseconds{ static_cast<uint64_t>(static_cast<double>(xferAudioData.currentSize / bytesPerSample) /
									   static_cast<double>(sampleRatePerSecond) * static_cast<double>(nsPerSecond)) };
								uint64_t framesPerSecond = 1000 / static_cast<uint64_t>(msPerPacket);
								frameSize				 = std::min(bytesPerSample * sampleRatePerSecond / framesPerSecond, xferAudioData.data.size());
							} else {
								intervalCount = nanoseconds{ 20000000 };
							}
							jsonifier::string_view_base<uint8_t> frame{};
							switch (frameType) {
								case audio_frame_type::raw_pcm: {
									auto encodedFrameData = encoder.encodeData(jsonifier::string_view_base<uint8_t>(xferAudioData.data.data(), frameSize));
									xferAudioData.clearData();
									if (encodedFrameData.data.size() != 0) {
										frame = packetEncrypter.encryptPacket(encodedFrameData);
									}
									break;
								}
								case audio_frame_type::encoded: {
									try {
										discord_core_internal::encoder_return_data returnData{};
										returnData.data		   = { xferAudioData.data.data(), static_cast<uint64_t>(xferAudioData.currentSize) };
										returnData.sampleCount = 960;
										if (returnData.data.size() != 0) {
											frame = packetEncrypter.encryptPacket(returnData);
											xferAudioData.clearData();
										}
									} catch (const dca_exception& error) {
										message_printer::printError<print_message_type::websocket>(error.what());
										break;
									}
									break;
								}
								case audio_frame_type::Unset: {
									xferAudioData.clearData();
									break;
								}
							}
							auto waitTime	   = targetTime - sys_clock::now();
							auto waitTimeCount = waitTime.count();
							int64_t minimumFreeTimeForCheckingProcessIO{ static_cast<int64_t>(static_cast<double>(intervalCount.count()) * 0.60l) };
							if (voice_connection::areWeConnected()) {
								if (waitTimeCount >= minimumFreeTimeForCheckingProcessIO && !token.promise().stopRequested()) {
									if (websocket_core::tcpConnection.processIO(0) != discord_core_internal::connection_status::NO_Error) {
										onClosed();
									}
								}

							} else {
								onClosed();
							}

							waitTime	  = targetTime - sys_clock::now();
							waitTimeCount = static_cast<int64_t>(static_cast<double>(waitTime.count()) * 0.95l);
							if (waitTimeCount > 0) {
								nanoSleep(waitTimeCount);
							}
							waitTime	  = targetTime - sys_clock::now();
							waitTimeCount = waitTime.count();
							if (waitTimeCount > 0 && waitTimeCount < intervalCount.count()) {
								spinLock(static_cast<uint64_t>(waitTimeCount));
							}
							if (udpConnection.areWeStillConnected()) {
								udpConnection.writeData(frame);
								if (udpConnection.processIO() != discord_core_internal::connection_status::NO_Error) {
									onClosed();
								}
							} else {
								xferAudioData.clearData();
								onClosed();
							}

							targetTime = sys_clock::now() + intervalCount;

							if (streamSocket) {
								streamSocket->mixAudio();
								if (streamSocket->areWeStillConnected()) {
									if (streamSocket->processIO() != discord_core_internal::connection_status::NO_Error) {
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
					case voice_active_state::exiting: {
						co_return;
					}
				}
				if (token.promise().stopRequested() || activeState == voice_active_state::exiting) {
					co_return;
				}
				std::this_thread::sleep_for(1ms);
			} catch (const dca_exception& error) {
				message_printer::printError<print_message_type::websocket>(error.what());
			}
		}
	};

	void voice_connection::skipInternal(uint32_t currentRecursionDepth) {
		if (currentRecursionDepth >= 10) {
			stop();
			return;
		}
		++currentRecursionDepth;
		song_completion_event_data completionEventData{};
		completionEventData.guildId		  = voiceConnectInitData.guildId;
		completionEventData.wasItAFail	  = wasItAFail.load(std::memory_order_acquire);
		completionEventData.guildMemberId = currentUserId;
		try {
			xferAudioData.clearData();
			if (discord_core_client::getInstance()->getSongAPI(voiceConnectInitData.guildId).onSongCompletionEvent.functions.size() > 0) {
				discord_core_client::getInstance()->getSongAPI(voiceConnectInitData.guildId).onSongCompletionEvent(completionEventData);
			} else {
				stop();
			}
		} catch (const dca_exception& error) {
			message_printer::printError<print_message_type::websocket>(error.what());
			std::this_thread::sleep_for(150ms);
			skipInternal(currentRecursionDepth);
		}
	}

	bool voice_connection::areWeCurrentlyPlaying() {
		return (activeState.load(std::memory_order_acquire) == voice_active_state::playing) || activeState.load(std::memory_order_acquire) == voice_active_state::paused;
	}

	void voice_udpconnection::handleAudioBuffer() {
		if (voiceConnection->connectionState.load(std::memory_order_acquire) == voice_connection_state::Initializing_DatagramSocket) {
		} else {
			jsonifier::string_view_base<uint8_t> string = getInputBuffer();
			if (voiceConnection->streamSocket && voiceConnection->encryptionKey.size() > 0) {
				voiceConnection->parseIncomingVoiceData(string);
			}
		}
	}

	bool voice_connection::areWeConnected() {
		return websocket_core::areWeConnected() && udpConnection.areWeStillConnected();
	}

	bool voice_connection::voiceConnect() {
		udpConnection = voice_udpconnection{ voiceIp, port, stream_type::none, this, &token };
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
		udpConnection.writeData(jsonifier::string_view_base<uint8_t>{ packet, std::size(packet) });
		jsonifier::string_view_base<uint8_t> inputStringFirst{};
		jsonifier::string_base<uint8_t> inputString{};

		stop_watch<milliseconds> stopWatch{ 5500ms };
		stopWatch.reset();
		while (inputStringFirst.size() < 74 && !doWeQuit->load(std::memory_order_acquire) && activeState.load(std::memory_order_acquire) != voice_active_state::exiting) {
			if (udpConnection.processIO() != discord_core_internal::connection_status::NO_Error) {
				onClosed();
				return false;
			}
			inputStringFirst = udpConnection.getInputBuffer();
			std::this_thread::sleep_for(1ms);
			if (stopWatch.hasTimeElapsed()) {
				return false;
			}
		}
		inputString.insert(inputString.begin(), inputStringFirst.begin(), inputStringFirst.end());
		inputString			   = inputString.substr(8);
		const auto endLineFind = inputString.find(static_cast<uint8_t>('\u0000'), static_cast<uint64_t>(6));
		if (endLineFind != jsonifier::string::npos) {
			inputString = inputString.substr(0, endLineFind);
		}
		jsonifier::string_view_base returnString{ inputStringFirst.data() + 8, inputString.size() };
		if (externalIp.size() < returnString.size()) {
			externalIp.resize(returnString.size());
		}
		std::memcpy(externalIp.data(), returnString.data(), returnString.size());
		voiceConnectionDataBuffer.clearContents();
		return true;
	}

	void voice_connection::sendSilence() {
		jsonifier::vector<jsonifier::string_base<uint8_t>> frames{};
		static constexpr uint8_t arrayNew[3]{ 0xf8, 0xff, 0xfe };
		for (uint64_t x = 0; x < 5; ++x) {
			discord_core_internal::encoder_return_data frame{};
			frame.data		  = jsonifier::string_view_base<uint8_t>{ arrayNew, 3 };
			frame.sampleCount = 3;
			auto packetNew	  = packetEncrypter.encryptPacket(frame);
			frames.emplace_back(jsonifier::string_base<uint8_t>{ packetNew.data(), packetNew.size() });
		}
		for (auto& value: frames) {
			udpConnection.writeData(value);
			if (udpConnection.processIO() != discord_core_internal::connection_status::NO_Error) {
				onClosed();
				return;
			}
		}
	}

	bool voice_connection::pauseToggle() {
		if (activeState.load(std::memory_order_acquire) == voice_active_state::paused) {
			activeState.store(voice_active_state::playing, std::memory_order_release);
			return true;
		} else if (activeState.load(std::memory_order_acquire) == voice_active_state::playing) {
			activeState.store(voice_active_state::paused, std::memory_order_release);
			return true;
		} else {
			return false;
		}
	}

	void voice_connection::disconnect() {
		activeState.store(voice_active_state::exiting, std::memory_order_release);
		if (taskThread.getStatus() == co_routine_status::running) {
			taskThread.cancel();
		}
		if (streamSocket) {
			streamSocket->disconnect();
			streamSocket.reset();
		};
		udpConnection.disconnect();
		websocket_core::disconnect();
		areWeHeartBeating	  = false;
		currentReconnectTries = 0;
		voiceUsers.clear();
		prevActiveState.store(voice_active_state::stopped, std::memory_order_release);
		activeState.store(voice_active_state::connecting, std::memory_order_release);
		connectionState.store(voice_connection_state::Collecting_Init_Data, std::memory_order_release);
		currentState.store(discord_core_internal::websocket_state::disconnected, std::memory_order_release);
	}

	void voice_connection::onClosed() {
		connectionState.store(voice_connection_state::Collecting_Init_Data, std::memory_order_release);
		if (activeState.load(std::memory_order_acquire) != voice_active_state::exiting && currentReconnectTries < maxReconnectTries) {
			if (activeState.load(std::memory_order_acquire) != voice_active_state::connecting) {
				prevActiveState.store(activeState.load(std::memory_order_acquire), std::memory_order_release);
			}
			websocket_core::disconnect();
			++currentReconnectTries;
			activeState.store(voice_active_state::connecting, std::memory_order_release);
			if (streamSocket) {
				streamSocket->disconnect();
			}
			udpConnection.disconnect();
		} else if (currentReconnectTries >= maxReconnectTries) {
			activeState.store(voice_active_state::exiting, std::memory_order_release);
		}
	}

	bool voice_connection::stop() {
		activeState.store(voice_active_state::stopped, std::memory_order_release);
		doWeSkip.store(false, std::memory_order_release);
		return true;
	}

	bool voice_connection::play() {
		activeState.store(voice_active_state::playing, std::memory_order_release);
		return true;
	}

	bool voice_connection::skip(bool wasItAFailNew) {
		if (activeState.load(std::memory_order_acquire) == voice_active_state::stopped) {
			return false;
		} else {
			wasItAFail.store(wasItAFailNew, std::memory_order_release);
			doWeSkip.store(true, std::memory_order_release);
			return true;
		}
	}
}
