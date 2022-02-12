/// SSLClents.cpp - Source file for the "SSL Client" stuff.
/// Dec 12, 2021
/// Chris M.
/// https://github.com/RealTimeChris

#include "SSLClients.hpp"

namespace DiscordCoreInternal {

	void reportError(std::string errorPosition, int32_t errorValue) {
		std::cout << errorPosition << errorValue;
#ifdef _WIN32
		std::cout << WSAGetLastError() << std::endl;
#else
		std::cout << strerror(errno) << std::endl;
#endif
	}

	void reportSSLError(std::string errorPosition, int32_t errorValue, SSL* ssl = nullptr) {
		if (ssl != nullptr) {
			std::cout << errorPosition << SSL_get_error(ssl, errorValue) << std::endl;
		}
		else {
			std::cout << errorPosition << std::endl;
		}
		ERR_print_errors_fp(stdout);
		std::cout << std::endl;
	}

	HttpSSLClient::HttpSSLClient(nullptr_t other) {};

	HttpSSLClient::HttpSSLClient(std::string* theInputBuffer) :
		inputBufferPtr(theInputBuffer) {
#ifdef _WIN32
		this->soundcloudCertPath = "C:/SSL/certs/SoundCloudCert.pem";
		this->defaultCertPath = "C:/SSL/certs/DiscordCert.pem";
		this->googleCertPath = "C:/SSL/certs/GoogleCert.pem";
#else 
		std::string userName{ getenv("USER") };
		this->soundcloudCertPath = "/home/" + userName + "/SSL/Certs/SoundCloudCert.pem";
		this->defaultCertPath = "/home/" + userName + "/SSL/Certs/DiscordCert.pem";
		this->googleCertPath = "/home/" + userName + "/SSL/Certs/GoogleCert.pem";
#endif
	}

	bool HttpSSLClient::connect(std::string baseUrl, std::string portNew) {
		try {
			std::string baseUrlNew{};
			if (baseUrl.find(".com") != std::string::npos) {
				baseUrlNew = baseUrl.substr(baseUrl.find("https://") + std::string("https://").size(), baseUrl.find(".com") + std::string(".com").size() - std::string("https://").size());
			}
			else if (baseUrl.find(".org") != std::string::npos) {
				baseUrlNew = baseUrl.substr(baseUrl.find("https://") + std::string("https://").size(), baseUrl.find(".org") + std::string(".org").size() - std::string("https://").size());
			}

			std::string certPath{};
			if (baseUrlNew.find("soundcloud") != std::string::npos || baseUrlNew.find("sndcdn") != std::string::npos) {
				certPath = this->soundcloudCertPath;
			}
			else if (baseUrlNew.find("youtube") != std::string::npos || baseUrlNew.find("google") != std::string::npos) {
				certPath = this->googleCertPath;
			}
			else {
				certPath = this->defaultCertPath;
			}

			this->context = SSL_CTX_new(TLS_client_method());
			if (this->context = nullptr) {
				reportSSLError("SSL_CTX_new() Error: ", 0);
				return false;
			}

			auto options{ SSL_CTX_get_options(this->context) };
			if (SSL_CTX_set_options(this->context, SSL_OP_NO_COMPRESSION) != (options | SSL_OP_NO_COMPRESSION)) {
				reportSSLError("SSL_CTX_set_options() Error: ", 0);
				return false;
			}

			SSL_CTX_set_verify(this->context, SSL_VERIFY_PEER, nullptr);
			SSL_CTX_set_verify_depth(this->context, 4);
			if (!SSL_CTX_load_verify_locations(this->context, certPath.c_str(), NULL)) {
				reportSSLError("SSL_CTX_load_verify_locations() Error: ", 0);
				return false;
			}

			if (!SSL_CTX_set_cipher_list(this->context, "ALL")) {
				reportSSLError("SSL_CTX_set_cipher_list() Error: ", 0);
				return false;
			}
			
			/* Do not allow SSL 3.0, TLS 1.0 or 1.1
			 * https://www.packetlabs.net/posts/tls-1-1-no-longer-secure/
			 */
			if (!SSL_CTX_set_min_proto_version(this->context, TLS1_2_VERSION)) {
				reportSSLError("SSL_CTX_set_min_proto_version() Error: ", 0);
				return false;
			}

			if (!SSL_CTX_set_ciphersuites(this->context, "TLS_AES_256_GCM_SHA384:TLS_CHACHA20_POLY1305_SHA256:TLS_AES_128_GCM_SHA256")) {
				reportSSLError("SSL_CTX_set_ciphersuites() Error: ", 0);
				return false;
			}

			this->connectionBio = BIO_new_ssl_connect(this->context);
			if (this->connectionBio == nullptr) {
				reportSSLError("BIO_new_ssl_connect() Error: ", 0);
				return false;
			}

			if (!BIO_set_conn_hostname(this->connectionBio, std::string(baseUrlNew + ":" + portNew).c_str())) {
				reportSSLError("BIO_set_connt_hostname() Error: ", 0);
				return false;
			}

			BIO_get_ssl(this->connectionBio, &this->ssl);
			if (this->ssl == nullptr) {
				reportSSLError("BIO_get_ssl() Error: ", 0);
				return false;
			}

			auto returnValue = SSL_set_tlsext_host_name(this->ssl, baseUrlNew.c_str());
			if (returnValue != 1) {
				reportSSLError("SSL_set_tlsext_host_name() Error: ", returnValue, this->ssl);
				return false;
			}

			returnValue = SSL_connect(this->ssl);
			if (returnValue != 1) {
				reportSSLError("SSL_connect() Error: ", returnValue, this->ssl);
				return false;
			}

			X509* cert{ SSL_get_peer_certificate(this->ssl) };
			if (cert == nullptr) {
				reportSSLError("SSL_get_peer_certificate() Error: ", returnValue, this->ssl);
				return false;
			}

			if (cert) {
				X509_free(cert);
				cert = nullptr;
			}

			returnValue = SSL_get_verify_result(this->ssl);
			if (returnValue != X509_V_OK) {
				reportSSLError("SSL_get_verify_result() Error: ", returnValue, this->ssl);
				return false;
			}

			this->theSocket = SSL_get_fd(this->ssl);
			if (this->theSocket == INVALID_SOCKET) {
				reportSSLError("SSL_get_fd() Error: ", returnValue, this->ssl);
				return false;
			}
			return true;
		}
		catch (...) {
			DiscordCoreAPI::reportException("HttpConnection::connect()");
		}
		return false;
	}

	void HttpSSLClient::writeData(std::string& data) {
		this->writeBuffer.insert(this->writeBuffer.end(), data.begin(), data.end());
		data.clear();
	}

	bool HttpSSLClient::processIO() {
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
		timeval checkTime{ .tv_usec = 600000 };
		auto resultValue{ select(nfds + 1, &readSet, &writeSet, nullptr, &checkTime) };
		if (resultValue == SOCKET_ERROR) {
			reportError("select() Error: ", resultValue);
			return false;
		}
		else if (resultValue == 0) {
			return true;
		}

		if (FD_ISSET(this->theSocket, &writeSet)) {
			size_t writtenBytes{ 0 };
			auto returnValue{ SSL_write_ex(this->ssl, this->writeBuffer.data(), static_cast<uint32_t>(this->writeBuffer.size()), &writtenBytes) };
			auto errorValue{ SSL_get_error(this->ssl, returnValue) };
			switch (errorValue) {
			case SSL_ERROR_NONE: {
				this->writeBuffer.clear();
				return true;
			}
			case SSL_ERROR_ZERO_RETURN: {
				return false;
			}
			case SSL_ERROR_WANT_READ: 
				this->wantRead = true;
				[[fallthrough]];
			case SSL_ERROR_WANT_WRITE: {
				return true;
			}
			default: {
				reportSSLError("SSL_write_ex() Error: ", returnValue, this->ssl);
				return false;
			}
			}
		}
		else if (FD_ISSET(this->theSocket, &readSet)) {
			this->wantRead = false;
			std::vector<uint8_t>  serverToClientBuffer{};
			serverToClientBuffer.resize(this->maxBufferSize);
			size_t readBytes{ 0 };
			auto returnValue{ SSL_read_ex(this->ssl, serverToClientBuffer.data(), static_cast<int32_t>(this->maxBufferSize), &readBytes) };
			auto errorValue{ SSL_get_error(this->ssl, returnValue) };
			switch (errorValue) {
			case SSL_ERROR_NONE: {
				if (readBytes > 0) {
					this->inputBufferPtr->insert(this->inputBufferPtr->end(), serverToClientBuffer.begin(), serverToClientBuffer.begin() + readBytes);
				}
				return true;
			}
			case SSL_ERROR_ZERO_RETURN: {
				return false;
			}
			case SSL_ERROR_WANT_WRITE:
				[[fallthrough]];
			case SSL_ERROR_WANT_READ: {
				this->wantRead = true;
				return true;
			}
			default: {
				reportSSLError(" HttpSSLClient::processIO::SSL_read_ex() Error: ", returnValue, this->ssl);
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

		auto returnValue{ getaddrinfo(baseUrlNew.c_str(), portNew.c_str(), hints, resultAddress) };
		if (returnValue == SOCKET_ERROR) {
			reportError("getaddrinfo() Error: ", returnValue);
			return;
		}

		this->theSocket = socket(resultAddress->ai_family, resultAddress->ai_socktype, resultAddress->ai_protocol);
		if (this->theSocket == INVALID_SOCKET) {
			reportError("socket() Error: ", returnValue);
			return;
		}

		returnValue = connect(this->theSocket, resultAddress->ai_addr, static_cast<int32_t>(resultAddress->ai_addrlen));
		if (returnValue == SOCKET_ERROR) {
			reportError("connect() Error: ", returnValue);
			return;
		}

#ifdef _WIN32
		char optionValue{ true };
		returnValue = setsockopt(this->theSocket, IPPROTO_TCP, TCP_NODELAY, &optionValue, sizeof(bool));
		if (returnValue == SOCKET_ERROR) {
			reportError("setsockopt() Error: ", returnValue);
			return;
		}
#else
		int32_t optionValue{ 1 };
		returnValue = setsockopt(this->theSocket, SOL_TCP, TCP_NODELAY, &optionValue, sizeof(optionValue));
		if (returnValue == SOCKET_ERROR) {
			reportError("setsockopt() Error: ", returnValue);
			return;
		}
#endif		
		this->context = SSL_CTX_new(TLS_client_method());
		if (this->context == nullptr) {
			reportSSLError("SSL_CTX_new() Error: ", 0);
			return;
		}

		this->ssl = SSL_new(this->context);
		if (this->ssl == nullptr) {
			reportSSLError("SSL_new() Error: ", 0);
			return;
		}

		returnValue = SSL_set_fd(this->ssl, this->theSocket);
		if (returnValue != 1) {
			reportSSLError("SSL_set_fd() Error: ", returnValue, this->ssl);
			return;
		}
		
		/* SNI */
		returnValue = SSL_set_tlsext_host_name(this->ssl, baseUrlNew.c_str());
		if (returnValue != 1) {
			reportSSLError("SSL_set_tlsext_host_name() Error: ", returnValue, this->ssl);
			return;
		}

		returnValue = SSL_connect(this->ssl);
		if (returnValue != 1) {
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
		auto resultValue{ select(nfds + 1, &readSet, &writeSet, nullptr, &checkTime) };
		if (resultValue == SOCKET_ERROR) {
			reportError("select() Error: ", resultValue);
			return false;
		}
		else if (resultValue == 0) {
			return true;
		}

		if (FD_ISSET(this->theSocket, &writeSet)) {
			size_t writtenBytes{ 0 };
			auto returnValue{ SSL_write_ex(this->ssl, this->writeBuffer.data(), static_cast<uint32_t>(this->writeBuffer.size()), &writtenBytes) };
			auto errorValue{ SSL_get_error(this->ssl, returnValue) };
			switch (errorValue) {
			case SSL_ERROR_NONE: {
				this->writeBuffer.clear();
				return true;
			}
			case SSL_ERROR_ZERO_RETURN: {
				return false;
			}
			case SSL_ERROR_WANT_READ:
				this->wantRead = true;
				[[fallthrough]];
			case SSL_ERROR_WANT_WRITE: {
				return true;
			}
			default: {
				reportSSLError("SSL_write_ex() Error: ", returnValue, this->ssl);
				return false;
			}
			}
		}
		else if (FD_ISSET(this->theSocket, &readSet)) {
			this->wantRead = false;
			std::vector<uint8_t>  serverToClientBuffer{};
			serverToClientBuffer.resize(this->maxBufferSize);
			size_t readBytes{ 0 };
			auto returnValue{ SSL_read_ex(this->ssl, serverToClientBuffer.data(), static_cast<int32_t>(this->maxBufferSize), &readBytes) };
			auto errorValue{ SSL_get_error(this->ssl, returnValue) };
			switch (errorValue) {
			case SSL_ERROR_NONE: {
				if (readBytes > 0) {
					this->inputBufferPtr->insert(this->inputBufferPtr->end(), serverToClientBuffer.begin(), serverToClientBuffer.begin() + readBytes);
					this->bytesRead += readBytes;
				}
				return true;
			}
			case SSL_ERROR_ZERO_RETURN: {
				return false;
			}
			case SSL_ERROR_WANT_WRITE:
				[[fallthrough]];
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

		auto returnValue{ getaddrinfo(baseUrlNew.c_str(), portNew.c_str(), hints, resultAddress) };
		if (returnValue == SOCKET_ERROR) {
			reportError("getaddrinfo() Error: ", returnValue);
			return;
		}

		this->theSocket = socket(resultAddress->ai_family, resultAddress->ai_socktype, resultAddress->ai_protocol);
		if (this->theSocket == INVALID_SOCKET) {
			reportError("socket() Error: ", returnValue);
			return;
		}

		returnValue = connect(this->theSocket, resultAddress->ai_addr, static_cast<int32_t>(resultAddress->ai_addrlen));
		if (returnValue == SOCKET_ERROR) {
			reportError("connect() Error: ", returnValue);
			return;
		}

#ifdef _WIN32
		u_long value{ 1 };
		returnValue = ioctlsocket(this->theSocket, FIONBIO, &value);
		if (returnValue == SOCKET_ERROR) {
			reportError("ioctlsocket() Error: ", returnValue);
			return;
		}
#else
		int ofcmode{ fcntl(this->theSocket, F_GETFL, 0) };
		ofcmode |= O_NONBLOCK;
		returnValue = fcntl(this->theSocket, F_SETFL, ofcmode);
		if (returnValue == SOCKET_ERROR) {
			reportError("fcntl() Error: ", returnValue);
			return;
		}
#endif
		this->connectionBio = BIO_new_dgram(this->theSocket, BIO_CLOSE);
		if (this->connectionBio == nullptr) {
			reportSSLError("BIO_new_dgram() Error: ", 0);
			return;
		}

		returnValue = BIO_ctrl(this->connectionBio, BIO_CTRL_DGRAM_SET_CONNECTED, 0, &resultAddress);
		if (returnValue == 0) {
			reportSSLError("BIO_ctrl() Error: ", 0);
			return;
		}

		return;
	}

	DatagramSocketSSLClient::DatagramSocketSSLClient(nullptr_t) {};

	bool DatagramSocketSSLClient::writeData(std::vector<uint8_t>& data) {
		size_t writtenBytes{ 0 };
		auto returnValue{ BIO_write_ex(this->connectionBio, data.data(), static_cast<uint32_t>(data.size()), &writtenBytes) };
		if (returnValue != 1) {
			reportSSLError("BIO_write_ex() Error: ", 0);
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
		auto returnValue{ BIO_read_ex(this->connectionBio, serverToClientBuffer.data(), this->maxBufferSize, &readBytes) };
		if (returnValue == 1) {
			if (readBytes > 0) {
				this->inputBufferPtr->insert(this->inputBufferPtr->end(), serverToClientBuffer.begin(), serverToClientBuffer.begin() + readBytes);
				if (doWeClear) {
					this->inputBufferPtr->clear();
				}
			}
		}
	}
}
