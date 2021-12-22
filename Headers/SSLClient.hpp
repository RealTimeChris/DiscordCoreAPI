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
		
		void toggleBlocking();

		bool readData();

		~MsgWebSocketSSLClient();

	protected:

		uint32_t fileDescriptor{ static_cast<uint32_t>(~0) };
		const uint32_t bufferSize{ 1024 * 16 };
		vector<uint8_t> outputBuffer{};
		vector<uint8_t> inputBuffer{};
		SSL_CTX* context{ nullptr };
		bool areWeBlocking{ true };
		string hostname{ "" };
		SSL* ssl{ nullptr };
		string port{ "" };
		fd_set readfds{};
	};

	class DiscordCoreAPI_Dll DatagramWebSocketSSLClient {
	public:

		friend class VoiceChannelWebSocketAgent;

		DatagramWebSocketSSLClient(string, string);

		DatagramWebSocketSSLClient(nullptr_t);

		string collectExternalIp(uint32_t);

		void writeData(vector<uint8_t>&);

		void toggleBlocking();

		bool readData(bool);

		~DatagramWebSocketSSLClient();

	protected:

		uint32_t fileDescriptor{ static_cast<uint32_t>(~0) };
		const uint32_t bufferSize{ 1024 * 16 };
		vector<uint8_t> outputBuffer{};
		vector<uint8_t> inputBuffer{};
		bool areWeBlocking{ true };
		string hostname{ "" };
		string port{ "" };
	};

}