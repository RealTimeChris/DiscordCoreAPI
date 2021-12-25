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

		DatagramWebSocketSSLClient(string hostName, string post);

		string collectExternalIp(uint32_t audioSSRC);

		void writeData(vector<uint8_t>& dataToWrite);

		DatagramWebSocketSSLClient(nullptr_t);

		bool readData(bool doWeClear);

		int64_t getBytesWritten();

		vector<uint8_t> getData();

		int64_t getBytesRead();

		void toggleBlocking();

		~DatagramWebSocketSSLClient();

	protected:

		uint32_t fileDescriptor{ static_cast<uint32_t>(~0) };
		const uint32_t bufferSize{ 1024 * 16 };
		vector<uint8_t> inputBuffer{};
		bool areWeBlocking{ true };
		int64_t bytesWritten{ 0 };
		int64_t bytesRead{ 0 };
		string hostname{ "" };
		string port{ "" };
	};

	class DiscordCoreAPI_Dll StreamWebSocketSSLClient {
	public:

		friend class VoiceChannelWebSocketAgent;

		StreamWebSocketSSLClient(string hostName, string post);

		string collectExternalIp(uint32_t audioSSRC);

		void writeData(vector<uint8_t>& dataToWrite);

		StreamWebSocketSSLClient(nullptr_t);

		bool readData(bool doWeClear);

		int64_t getBytesWritten();

		vector<uint8_t> getData();

		int64_t getBytesRead();

		void toggleBlocking();

		~StreamWebSocketSSLClient();

	protected:

		uint32_t fileDescriptor{ static_cast<uint32_t>(~0) };
		const uint32_t bufferSize{ 1024 * 16 };
		vector<char> inputBuffer{};
		bool areWeBlocking{ true };
		int64_t bytesWritten{ 0 };
		int64_t bytesRead{ 0 };
		string hostname{ "" };
		string port{ "" };
	};

}