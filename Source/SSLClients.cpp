/*
*    # License

   Copyright 2021 Chris M.

   Licensed under the Apache License, Version 2.0 (the "License");
   you may not use this file except in compliance with the License.
   You may obtain a copy of the License at

	   http://www.apache.org/licenses/LICENSE-2.0

   Unless required by applicable law or agreed to in writing, software
   distributed under the License is distributed on an "AS IS" BASIS,
   WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
   See the License for the specific language governing permissions and
   limitations under the License.
*/
/// SSLClents.cpp - Source file for the "SSL Client" stuff.
/// Dec 12, 2021
/// Chris M.
/// https://github.com/RealTimeChris/DiscordCoreAPI

#include <SSLClients.hpp>

namespace DiscordCoreInternal {

	void reportSSLError(std::string errorPosition, int32_t errorValue = 0, SSL* ssl = nullptr) {
		if (ssl != nullptr) {
			std::cout << errorPosition << SSL_get_error(ssl, errorValue) << std::endl;
		}
		else {
			std::cout << errorPosition << std::endl;
		}
		ERR_print_errors_fp(stdout);
		std::cout << std::endl;
	}

	void reportError(std::string errorPosition, int32_t errorValue) {
		std::cout << errorPosition << errorValue << ", ";
#ifdef _WIN32
		std::cout << WSAGetLastError() << std::endl;
#else
		std::cout << strerror(errno) << std::endl;
#endif
	}

	HttpSSLClient::HttpSSLClient(nullptr_t other) {};

	HttpSSLClient::HttpSSLClient(std::string* theInputBuffer) :
		inputBufferPtr(theInputBuffer) {
#ifdef _WIN32
		this->soundcloudCertPath = "C:/SSL/Certs/SoundCloudCert.pem";
		this->defaultCertPath = "C:/SSL/Certs/DiscordCert.pem";
		this->googleCertPath = "C:/SSL/Certs/GoogleCert.pem";
#else 
		std::string userName{ getenv("USER") };
		this->soundcloudCertPath = "/home/" + userName + "/SSL/Certs/SoundCloudCert.pem";
		this->defaultCertPath = "/home/" + userName + "/SSL/Certs/DiscordCert.pem";
		this->googleCertPath = "/home/" + userName + "/SSL/Certs/GoogleCert.pem";
#endif
	}

	bool HttpSSLClient::connect(std::string baseUrl, std::string portNew) {
		try {
			std::string certPath{};
			if (baseUrl.find("soundcloud") != std::string::npos || baseUrl.find("sndcdn") != std::string::npos) {
				certPath = this->soundcloudCertPath;
			}
			else if (baseUrl.find("youtube") != std::string::npos || baseUrl.find("google") != std::string::npos) {
				certPath = this->googleCertPath;
			}
			else {
				certPath = this->defaultCertPath;
			}

			if (this->context = SSL_CTX_new(TLS_client_method()); this->context == nullptr) {
				reportSSLError("SSL_CTX_new() Error: ");
				return false;
			}

			auto options{ SSL_CTX_get_options(this->context) };
			if (SSL_CTX_set_options(this->context, SSL_OP_NO_COMPRESSION) != (options | SSL_OP_NO_COMPRESSION)) {
				reportSSLError("SSL_CTX_set_options() Error: ");
				return false;
			}

			SSL_CTX_set_verify(this->context, SSL_VERIFY_PEER, nullptr);
			SSL_CTX_set_verify_depth(this->context, 4);
			if (!SSL_CTX_load_verify_locations(this->context, certPath.c_str(), NULL)) {
				reportSSLError("SSL_CTX_load_verify_locations() Error: ");
				return false;
			}

			if (!SSL_CTX_set_cipher_list(this->context, "ALL")) {
				reportSSLError("SSL_CTX_set_cipher_list() Error: ");
				return false;
			}
			
			/* Do not allow SSL 3.0, TLS 1.0 or 1.1
			 * https://www.packetlabs.net/posts/tls-1-1-no-longer-secure/
			 */
			if (!SSL_CTX_set_min_proto_version(this->context, TLS1_2_VERSION)) {
				reportSSLError("SSL_CTX_set_min_proto_version() Error: ");
				return false;
			}

			if (!SSL_CTX_set_ciphersuites(this->context, "TLS_AES_256_GCM_SHA384:TLS_CHACHA20_POLY1305_SHA256:TLS_AES_128_GCM_SHA256")) {
				reportSSLError("SSL_CTX_set_ciphersuites() Error: ");
				return false;
			}

			if (this->connectionBio = BIO_new_ssl_connect(this->context); this->connectionBio == nullptr) {
				reportSSLError("BIO_new_ssl_connect() Error: ");
				return false;
			}

			if (!BIO_set_conn_hostname(this->connectionBio, std::string(baseUrl + ":" + portNew).c_str())) {
				reportSSLError("BIO_set_connt_hostname() Error: ");
				return false;
			}

			if (BIO_get_ssl(this->connectionBio, &this->ssl); this->ssl == nullptr) {
				reportSSLError("BIO_get_ssl() Error: ");
				return false;
			}

			if (auto returnValue = SSL_set_tlsext_host_name(this->ssl, baseUrl.c_str()); !returnValue) {
				reportSSLError("SSL_set_tlsext_host_name() Error: ", returnValue, this->ssl);
				return false;
			}

			if (auto returnValue = SSL_connect(this->ssl); !returnValue) {
				reportSSLError("SSL_connect() Error: ", returnValue, this->ssl);
				return false;
			}

			if (std::unique_ptr<X509, X509Deleter> cert = std::unique_ptr<X509, X509Deleter>(SSL_get_peer_certificate(this->ssl)); cert == nullptr) {
				reportSSLError("SSL_get_peer_certificate() Error: ", 0, this->ssl);
				return false;
			}

			if (auto returnValue = SSL_get_verify_result(this->ssl); returnValue != X509_V_OK) {
				reportSSLError("SSL_get_verify_result() Error: ", returnValue, this->ssl);
				return false;
			}

			if (this->theSocket = SSL_get_fd(this->ssl); this->theSocket == INVALID_SOCKET) {
				reportSSLError("SSL_get_fd() Error: ", this->theSocket, this->ssl);
				return false;
			}
			return true;
		}
		catch (...) {
			DiscordCoreAPI::reportException("HttpSSLClient::connect()");
		}
		return false;
	}

	void HttpSSLClient::writeData(std::string& data) {
		this->writeBuffer.insert(this->writeBuffer.end(), data.begin(), data.end());
		data.clear();
	}

	bool HttpSSLClient::processIO() {
#ifdef _WIN32
		fd_set writeSet{};
		fd_set readSet{};
		int32_t nfds{ 0 };
		FD_ZERO(&writeSet);
		FD_ZERO(&readSet);

		if (this->writeBuffer.size() > 0 && !this->wantRead) {
			FD_SET(this->theSocket, &writeSet);
			nfds = this->theSocket > nfds ? this->theSocket : nfds;
		}
		else {
			FD_SET(this->theSocket, &readSet);
			nfds = this->theSocket > nfds ? this->theSocket : nfds;
		}

		timeval checkTime{ .tv_usec = 60000 };
		if (auto resultValue = select(nfds + 1, &readSet, &writeSet, nullptr, &checkTime); resultValue == SOCKET_ERROR) {
			reportError("select() Error: ", resultValue);
			return false;
		}
		else if (resultValue == 0) {
			return true;
		}
#else
		epoll_event writeEvent{}, readEvent{}, events[1]{};
		epollWrapper epoll{ nullptr };
		bool writing{ false };

		if (this->writeBuffer.size() > 0 && !this->wantRead) {
			writing = true;
			writeEvent.events = EPOLLOUT;
			writeEvent.data.fd = this->theSocket;
			if (auto resultValue = epoll_ctl(epoll, EPOLL_CTL_ADD, this->theSocket, &writeEvent); resultValue == SOCKET_ERROR) {
				reportError("epoll_ctl() Error: ", resultValue);
				return false;
			}
		}
		else {
			readEvent.events = EPOLLIN;
			readEvent.data.fd = this->theSocket;
			if (auto resultValue = epoll_ctl(epoll, EPOLL_CTL_ADD, this->theSocket, &readEvent); resultValue == SOCKET_ERROR) {
				reportError("epoll_ctl() Error: ", resultValue);
				return false;
			}
		}

		if (auto resultValue = epoll_wait(epoll, events, 1, 60); resultValue == SOCKET_ERROR) {
			reportError("epoll_wait() Error: ", resultValue);
			return false;
		}
		else if (resultValue == 0) {
			return true;
		}

#endif

#ifdef _WIN32
		if (FD_ISSET(this->theSocket, &writeSet)) {
#else
		if (writing) {
#endif
			size_t writtenBytes{ 0 };
			auto returnValue{ SSL_write_ex(this->ssl, this->writeBuffer.data(), this->writeBuffer.size(), &writtenBytes) };
			auto errorValue{ SSL_get_error(this->ssl, returnValue) };
			switch (errorValue) {
			case SSL_ERROR_NONE: {
				this->writeBuffer.clear();
				return true;
			}
			case SSL_ERROR_SYSCALL: {
				[[fallthrough]];
			}
			case SSL_ERROR_ZERO_RETURN: {
				return false;
			}
			case SSL_ERROR_WANT_READ: {
				this->wantRead = true;
				[[fallthrough]];
			}
			case SSL_ERROR_WANT_WRITE: {
				return true;
			}
			default: {
				reportSSLError("SSL_write_ex() Error: ", returnValue, this->ssl);
				return false;
			}
			}
		}
#ifdef _WIN32
		else if (FD_ISSET(this->theSocket, &readSet)) {
#else
		else {
#endif
			this->wantRead = false;
			std::vector<uint8_t>  serverToClientBuffer{};
			serverToClientBuffer.resize(this->maxBufferSize);
			size_t readBytes{ 0 };
			auto returnValue{ SSL_read_ex(this->ssl, serverToClientBuffer.data(), this->maxBufferSize, &readBytes) };
			auto errorValue{ SSL_get_error(this->ssl, returnValue) };
			switch (errorValue) {
			case SSL_ERROR_NONE: {
				if (readBytes > 0) {
					this->inputBufferPtr->insert(this->inputBufferPtr->end(), serverToClientBuffer.begin(), serverToClientBuffer.begin() + readBytes);
				}
				return true;
			}
			case SSL_ERROR_SYSCALL: {
				[[fallthrough]];
			}
			case SSL_ERROR_ZERO_RETURN: {
				return false;
			}
			case SSL_ERROR_WANT_WRITE: {
				[[fallthrough]];
			}
			case SSL_ERROR_WANT_READ: {
				this->wantRead = true;
				return true;
			}
			default: {
				reportSSLError("WebSocketSSLClient::processIO::SSL_read_ex() Error: ", returnValue, this->ssl);
				return false;
			}
			}
		}
		return true;
	}

	WebSocketSSLClient::WebSocketSSLClient(std::string baseUrlNew, std::string portNew, std::vector<uint8_t>* theInputBuffer, int64_t maxBufferSizeNew) :
		inputBufferPtr(theInputBuffer),
		maxBufferSize(maxBufferSizeNew)
	{
		addrinfoWrapper  resultAddress{ nullptr }, hints{ nullptr };
		hints->ai_family = AF_INET;
		hints->ai_socktype = SOCK_STREAM;
		hints->ai_protocol = IPPROTO_TCP;

		if (auto returnValue = getaddrinfo(baseUrlNew.c_str(), portNew.c_str(), hints, resultAddress); returnValue == SOCKET_ERROR) {
			reportError("getaddrinfo() Error: ", returnValue);
			return;
		}

		if (this->theSocket = socket(resultAddress->ai_family, resultAddress->ai_socktype, resultAddress->ai_protocol); this->theSocket == INVALID_SOCKET) {
			reportError("socket() Error: ", this->theSocket);
			return;
		}

		if (auto returnValue = connect(this->theSocket, resultAddress->ai_addr, static_cast<int32_t>(resultAddress->ai_addrlen)); returnValue == SOCKET_ERROR) {
			reportError("connect() Error: ", returnValue);
			return;
		}

#ifdef _WIN32
		char optionValue{ true };
		if (auto returnValue = setsockopt(this->theSocket, IPPROTO_TCP, TCP_NODELAY, &optionValue, sizeof(bool)); returnValue == SOCKET_ERROR) {
			reportError("setsockopt() Error: ", returnValue);
			return;
		}
#else
		int32_t optionValue{ 1 };
		if (auto returnValue = setsockopt(this->theSocket, SOL_TCP, TCP_NODELAY, &optionValue, sizeof(optionValue)); returnValue == SOCKET_ERROR) {
			reportError("setsockopt() Error: ", returnValue);
			return;
		}
#endif		

		if (this->context = SSL_CTX_new(TLS_client_method()); this->context == nullptr) {
			reportSSLError("SSL_CTX_new() Error: ");
			return;
		}

		if (this->ssl = SSL_new(this->context); this->ssl == nullptr) {
			reportSSLError("SSL_new() Error: ");
			return;
		}

		if (auto returnValue = SSL_set_fd(this->ssl, this->theSocket); !returnValue) {
			reportSSLError("SSL_set_fd() Error: ", returnValue, this->ssl);
			return;
		}

		/* SNI */
		if (auto returnValue = SSL_set_tlsext_host_name(this->ssl, baseUrlNew.c_str()); !returnValue) {
			reportSSLError("SSL_set_tlsext_host_name() Error: ", returnValue, this->ssl);
			return;
		}

		if (auto returnValue = SSL_connect(this->ssl); !returnValue) {
			reportSSLError("SSL_connect() Error: ", returnValue, this->ssl);
			return;
		}
	};

	WebSocketSSLClient::WebSocketSSLClient(nullptr_t other) {};

	std::string WebSocketSSLClient::getData() {
		std::string newVector{};
		newVector.insert(newVector.begin(), this->inputBufferPtr->begin(), this->inputBufferPtr->end());
		this->inputBufferPtr->clear();
		return newVector;
	}

	bool WebSocketSSLClient::processIO(int32_t waitTimeInMicroSeconds) {
#ifdef _WIN32
		fd_set writeSet{};
		fd_set readSet{};
		int32_t nfds{ 0 };
		FD_ZERO(&writeSet);
		FD_ZERO(&readSet);

		if (this->writeBuffer.size() > 0 && !this->wantRead) {
			FD_SET(this->theSocket, &writeSet);
			nfds = this->theSocket > nfds ? this->theSocket : nfds;
		}
		else {
			FD_SET(this->theSocket, &readSet);
			nfds = this->theSocket > nfds ? this->theSocket : nfds;
		}

		timeval checkTime{ .tv_usec = waitTimeInMicroSeconds };
		if (auto resultValue = select(nfds + 1, &readSet, &writeSet, nullptr, &checkTime); resultValue == SOCKET_ERROR) {
			reportError("select() Error: ", resultValue);
			return false;
		}
		else if (resultValue == 0) {
			return true;
		}
#else
		epoll_event writeEvent{}, readEvent{}, events[1]{};
		epollWrapper epoll{ nullptr };
		bool writing{ false };

		if (this->writeBuffer.size() > 0 && !this->wantRead) {
			writing = true;
			writeEvent.events = EPOLLOUT;
			writeEvent.data.fd = this->theSocket;
			if (auto resultValue = epoll_ctl(epoll, EPOLL_CTL_ADD, this->theSocket, &writeEvent); resultValue == SOCKET_ERROR) {
				reportError("epoll_ctl() Error: ", resultValue);
				return false;
			}
		}
		else {
			readEvent.events = EPOLLIN;
			readEvent.data.fd = this->theSocket;
			if (auto resultValue = epoll_ctl(epoll, EPOLL_CTL_ADD, this->theSocket, &readEvent); resultValue == SOCKET_ERROR) {
				reportError("epoll_ctl() Error: ", resultValue);
				return false;
			}
		}

		if (auto resultValue = epoll_wait(epoll, events, 1, waitTimeInMicroSeconds / 1000); resultValue == SOCKET_ERROR) {
			reportError("epoll_wait() Error: ", resultValue);
			return false;
		}
		else if (resultValue == 0) {
			return true;
		}

#endif

#ifdef _WIN32
		if (FD_ISSET(this->theSocket, &writeSet)) {
#else
		if (writing) {
#endif
			size_t writtenBytes{ 0 };
			auto returnValue{ SSL_write_ex(this->ssl, this->writeBuffer.data(), this->writeBuffer.size(), &writtenBytes) };
			auto errorValue{ SSL_get_error(this->ssl, returnValue) };
			switch (errorValue) {
			case SSL_ERROR_NONE: {
				this->writeBuffer.clear();
				return true;
			}
			case SSL_ERROR_SYSCALL: {
				[[fallthrough]];
			}
			case SSL_ERROR_ZERO_RETURN: {
				return false;
			}
			case SSL_ERROR_WANT_READ: {
				this->wantRead = true;
				[[fallthrough]];
			}
			case SSL_ERROR_WANT_WRITE: {
				return true;
			}
			default: {
				reportSSLError("SSL_write_ex() Error: ", returnValue, this->ssl);
				return false;
			}
			}
		}
#ifdef _WIN32
		else if (FD_ISSET(this->theSocket, &readSet)) {
#else
		else {
#endif
			this->wantRead = false;
			std::vector<uint8_t>  serverToClientBuffer{};
			serverToClientBuffer.resize(this->maxBufferSize);
			size_t readBytes{ 0 };
			auto returnValue{ SSL_read_ex(this->ssl, serverToClientBuffer.data(), this->maxBufferSize, &readBytes) };
			auto errorValue{ SSL_get_error(this->ssl, returnValue) };
			switch (errorValue) {
			case SSL_ERROR_NONE: {
				if (readBytes > 0) {
					this->inputBufferPtr->insert(this->inputBufferPtr->end(), serverToClientBuffer.begin(), serverToClientBuffer.begin() + readBytes);
					this->bytesRead += readBytes;
				}
				return true;
			}
			case SSL_ERROR_SYSCALL: {
				[[fallthrough]];
			}
			case SSL_ERROR_ZERO_RETURN: {
				return false;
			}
			case SSL_ERROR_WANT_WRITE: {
				[[fallthrough]];
			}
			case SSL_ERROR_WANT_READ: {
				this->wantRead = true;
				return true;
			}
			default: {
				reportSSLError("WebSocketSSLClient::processIO::SSL_read_ex() Error: ", returnValue, this->ssl);
				return false;
			}
			}
		}
		return true;
	}

	int64_t WebSocketSSLClient::getBytesRead() {
		return this->bytesRead;
	}
	
	DatagramSocketSSLClient::DatagramSocketSSLClient(std::string baseUrlNew, std::string portNew, std::vector<uint8_t>* theInputBuffer) :
		inputBufferPtr(theInputBuffer)
	{
		addrinfoWrapper  resultAddress{ nullptr }, hints{ nullptr };

		hints->ai_family = AF_INET;
		hints->ai_socktype = SOCK_DGRAM;
		hints->ai_protocol = IPPROTO_UDP;

		if (auto returnValue = getaddrinfo(baseUrlNew.c_str(), portNew.c_str(), hints, resultAddress); returnValue == SOCKET_ERROR) {
			reportError("getaddrinfo() Error: ", returnValue);
			return;
		}

		if (this->theSocket = socket(resultAddress->ai_family, resultAddress->ai_socktype, resultAddress->ai_protocol); this->theSocket == INVALID_SOCKET) {
			reportError("socket() Error: ", 0);
			return;
		}

		if (auto returnValue = connect(this->theSocket, resultAddress->ai_addr, static_cast<int32_t>(resultAddress->ai_addrlen)); returnValue == SOCKET_ERROR) {
			reportError("connect() Error: ", returnValue);
			return;
		}

#ifdef _WIN32
		u_long value{ 1 };
		if (auto returnValue = ioctlsocket(this->theSocket, FIONBIO, &value); returnValue == SOCKET_ERROR) {
			reportError("ioctlsocket() Error: ", returnValue);
			return;
		}
#else
		if (auto returnValue = fcntl(this->theSocket, F_SETFL, fcntl(this->theSocket, F_GETFL, 0) | O_NONBLOCK); returnValue == SOCKET_ERROR) {
			reportError("fcntl() Error: ", returnValue);
			return;
		}
#endif
		if (this->connectionBio = BIO_new_dgram(this->theSocket, BIO_CLOSE); this->connectionBio == nullptr) {
			reportSSLError("BIO_new_dgram() Error: ");
			return;
		}

		if (auto returnValue = BIO_ctrl(this->connectionBio, BIO_CTRL_DGRAM_SET_CONNECTED, 0, &resultAddress); returnValue == 0) {
			reportSSLError("BIO_ctrl() Error: ");
			return;
		}

		return;
	}

	DatagramSocketSSLClient::DatagramSocketSSLClient(nullptr_t) {};

	bool DatagramSocketSSLClient::writeData(std::vector<uint8_t>& data) {
		size_t writtenBytes{ 0 };

		if (!BIO_write_ex(this->connectionBio, data.data(), data.size(), &writtenBytes)) {
			reportSSLError("BIO_write_ex() Error: ");
			return false;
		};
		data.clear();
		return true;
	}

	std::vector<uint8_t> DatagramSocketSSLClient::getData() {
		std::vector<uint8_t> newVector{};
		newVector.insert(newVector.begin(), this->inputBufferPtr->begin(), this->inputBufferPtr->end());
		this->inputBufferPtr->clear();
		return newVector;
	}

	void DatagramSocketSSLClient::readData(bool doWeClear) {
		std::vector<uint8_t>  serverToClientBuffer{};
		serverToClientBuffer.resize(this->maxBufferSize);
		size_t readBytes{ 0 };
		if (auto returnValue = BIO_read_ex(this->connectionBio, serverToClientBuffer.data(), this->maxBufferSize, &readBytes); returnValue == 1) {
			if (readBytes > 0) {
				this->inputBufferPtr->insert(this->inputBufferPtr->end(), serverToClientBuffer.begin(), serverToClientBuffer.begin() + readBytes);
				if (doWeClear) {
					this->inputBufferPtr->clear();
				}
			}
		}
	}
}
