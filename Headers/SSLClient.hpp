/// SSLClent.hpp - Header file for the "SSL Client" stuff.
/// Dec 12, 2021
/// Chris M.
/// https://github.com/RealTimeChris

#pragma once

#include "IndexInitial.hpp"
#include "FoundationEntities.hpp"

namespace DiscordCoreInternal {

	struct DiscordCoreAPI_Dll MsgWebSocketSSLContext {

		SSL_CTX* context{ nullptr };

		SSL* ssl{ nullptr };

	};

	class DiscordCoreAPI_Dll MsgWebSocketSSLClient {
	public:

		friend class MsgWebSocketAgent;

		MsgWebSocketSSLClient(string, string);

		void writeData(vector<uint8_t>&);

		void writeData(string&);

		uint64_t getBytesOut();

		uint64_t getBytesIn();

		void readData();

		~MsgWebSocketSSLClient();

	protected:

		uint32_t fileDescriptor{ static_cast<uint32_t>(~0) };
		const uint32_t bufferSize{ 1024 * 16 };
		fd_set readfds{}, writefds{}, efds{};
		size_t clientToServerOffset{ 0 };
		vector<uint8_t> outputBuffer{};
		vector<uint8_t> inputBuffer{};
		MsgWebSocketSSLContext ssl{};
		uint64_t bytesOut{ 0 };
		uint64_t bytesIn{ 0 };
		string hostname{ "" };
		string cipher{ "" };
		string port{ "" };
	};

}