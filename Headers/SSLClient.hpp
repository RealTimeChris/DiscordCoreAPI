/// SSLClent.hpp - Header file for the "SSL Client" stuff.
/// Dec 12, 2021
/// Chris M.
/// https://github.com/RealTimeChris

#pragma once

#include "IndexInitial.hpp"
#include "FoundationEntities.hpp"

namespace DiscordCoreInternal {

	class DiscordCoreAPI_Dll MsgWebSocketSSLClient {
	public:

		friend class VoiceChannelWebSocketAgent;
		friend class VoiceChannelWebSocketAgent;
		friend class MsgWebSocketAgent;
		
		MsgWebSocketSSLClient(string, string);

		MsgWebSocketSSLClient(nullptr_t);

		void writeData(vector<uint8_t>&);

		void writeData(string&);

		void readData();

		~MsgWebSocketSSLClient();

	protected:

		uint32_t fileDescriptor{ static_cast<uint32_t>(~0) };
		const uint32_t bufferSize{ 1024 * 16 };
		fd_set readfds{}, writefds{}, efds{};
		vector<uint8_t> outputBuffer{};
		vector<uint8_t> inputBuffer{};
		SSL_CTX* context{ nullptr };
		string hostname{ "" };
		SSL* ssl{ nullptr };
		string port{ "" };
	};

	class DiscordCoreAPI_Dll DatagramWebSocketSSLClient {
	public:

		friend class VoiceChannelWebSocketAgent;

		DatagramWebSocketSSLClient(string, string);

		DatagramWebSocketSSLClient(nullptr_t);

		string collectExternalIp(uint32_t);

		void writeData(vector<uint8_t>&);

		void readData();

		~DatagramWebSocketSSLClient();

	protected:

		uint32_t fileDescriptor{ static_cast<uint32_t>(~0) };
		const uint32_t bufferSize{ 1024 * 16 };
		vector<uint8_t> outputBuffer{};
		vector<uint8_t> inputBuffer{};
		string hostname{ "" };
		string port{ "" };
	};

}