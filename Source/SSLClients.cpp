/// SSLClents.cpp - Source file for the "SSL Client" stuff.
/// Dec 12, 2021
/// Chris M.
/// https://github.com/RealTimeChris

#include "SSLClients.hpp"

namespace DiscordCoreInternal {

	HttpSSLClient::HttpSSLClient(nullptr_t) {};

	HttpSSLClient::HttpSSLClient(std::string* theInputBuffer) :
		inputBufferPtr(theInputBuffer) {
#ifdef _WIN32
		this->soundcloudCertPath = "C:/SSL/certs/SoundCloudCert.pem";
		this->defaultCertPath = "C:/SSL/certs/DiscordCert.pem";
		this->googleCertPath = "C:/SSL/certs/GoogleCert.pem";
#else 
		std::string userName = getenv("USER");
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
			if (this->context == nullptr) {
				std::cout << "SSL_CTX_new() Error: ";
				ERR_print_errors_fp(stdout);
				std::cout << std::endl;
				return false;
			}
			int32_t options = static_cast<int32_t>(SSL_CTX_get_options(this->context));
			int32_t returnValue = static_cast<int32_t>(SSL_CTX_set_options(this->context, SSL_OP_NO_COMPRESSION));
			if (returnValue != (options | SSL_OP_NO_COMPRESSION)) {
				std::cout << "SSL_CTX_set_options() Error: ";
				ERR_print_errors_fp(stdout);
				std::cout << std::endl;
				return false;
			}
			SSL_CTX_set_verify(this->context, SSL_VERIFY_PEER, nullptr);
			SSL_CTX_set_verify_depth(this->context, 4);
			returnValue = SSL_CTX_load_verify_locations(this->context, certPath.c_str(), NULL);
			if (returnValue != 1) {
				std::cout << "SSL_CTX_load_verify_locations() Error: ";
				ERR_print_errors_fp(stdout);
				std::cout << std::endl;
				return false;
			}
			returnValue = SSL_CTX_set_cipher_list(this->context, "ALL");
			if (returnValue != 1) {
				std::cout << "SSL_CTX_set_cipher_list() Error: ";
				ERR_print_errors_fp(stdout);
				std::cout << std::endl;
				return false;
			}
			returnValue = SSL_CTX_set_ciphersuites(this->context, "TLS_AES_256_GCM_SHA384:TLS_CHACHA20_POLY1305_SHA256:TLS_AES_128_GCM_SHA256");
			if (returnValue != 1) {
				std::cout << "SSL_CTX_set_ciphersuites() Error: ";
				ERR_print_errors_fp(stdout);
				std::cout << std::endl;
				return false;
			}
			this->connectionBio = BIO_new_ssl_connect(this->context);
			if (this->connectionBio == nullptr) {
				std::cout << "BIO_new_ssl_connect() Error: ";
				ERR_print_errors_fp(stdout);
				std::cout << std::endl;
				return false;
			}

			returnValue = BIO_set_conn_hostname(this->connectionBio, std::string(baseUrlNew + ":" + portNew).c_str());
			if (returnValue != 1) {
				std::cout << "BIO_set_connt_hostname() Error: ";
				ERR_print_errors_fp(stdout);
				std::cout << std::endl;
				return false;
			}
			BIO_get_ssl(this->connectionBio, &this->ssl);
			if (this->ssl == nullptr) {
				std::cout << "BIO_get_ssl() Error: ";
				ERR_print_errors_fp(stdout);
				std::cout << std::endl;
				return false;
			};
			returnValue = SSL_set_tlsext_host_name(this->ssl, std::string(baseUrlNew + ":" + portNew).c_str());
			if (returnValue != 1) {
				std::cout << "SSL_set_tlsext_host_name() Error: " << SSL_get_error(this->ssl, returnValue) << std::endl;
				ERR_print_errors_fp(stdout);
				std::cout << std::endl;
				return false;
			}
			returnValue = SSL_connect(this->ssl);
			if (returnValue != 1) {
				std::cout << "SSL_connect() Error: " << SSL_get_error(this->ssl, returnValue) << std::endl;
				ERR_print_errors_fp(stdout);
				std::cout << std::endl;
				return false;
			}
			X509* cert = SSL_get_peer_certificate(this->ssl);
			if (cert == nullptr) {
				std::cout << "SSL_get_peer_certificate() Error: " << SSL_get_error(this->ssl, returnValue) << std::endl;
				ERR_print_errors_fp(stdout);
				std::cout << std::endl;
				return false;
			}
			if (cert) {
				X509_free(cert);
				cert = nullptr;
			}
			returnValue = SSL_get_verify_result(this->ssl);
			if (returnValue != X509_V_OK) {
				std::cout << "SSL_get_verify_result() Error: " << SSL_get_error(this->ssl, returnValue) << std::endl;
				ERR_print_errors_fp(stdout);
				std::cout << std::endl;
				return false;
			}
			this->theSocket = SSL_get_fd(this->ssl);
			if (this->theSocket == INVALID_SOCKET) {
				std::cout << "SSL_get_fd() Error: " << SSL_get_error(this->ssl, returnValue) << std::endl;
				ERR_print_errors_fp(stdout);
				std::cout << std::endl;
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
		FD_ZERO(&writeSet);
		FD_ZERO(&readSet);
		if ((this->writeBuffer.size() > 0 || this->wantWrite) && !this->wantRead) {
			FD_SET(this->theSocket, &writeSet);
		}
		else {
			FD_SET(this->theSocket, &readSet);
		}
		timeval checkTime{};
		checkTime.tv_sec = 1;
		auto resultValue = select(FD_SETSIZE, &readSet, &writeSet, nullptr, &checkTime);
		if (resultValue == SOCKET_ERROR) {
			std::cout << "select() Error: " << resultValue + ", ";
#ifdef _WIN32
			std::cout << WSAGetLastError() << std::endl;
#endif
			return false;
		}
		else if (resultValue == 0) {
			return true;
		}
		if (FD_ISSET(this->theSocket, &writeSet)) {
			this->wantWrite = false;
			size_t writtenBytes{ 0 };
			if (this->ssl == nullptr) {
				std::cout << "ssl is nullptr. " << std::endl;
				return false;
			}
			int32_t returnValue = SSL_write_ex(this->ssl, this->writeBuffer.data(), static_cast<uint32_t>(this->writeBuffer.size()), &writtenBytes);
			int32_t errorValue = SSL_get_error(this->ssl, returnValue);
			switch (errorValue) {
			case SSL_ERROR_NONE: {
				this->writeBuffer.clear();
				return true;
			}
			case SSL_ERROR_ZERO_RETURN: {
				std::cout << "SSL_write_ex() Error: " << SSL_get_error(this->ssl, returnValue) << std::endl;
				ERR_print_errors_fp(stdout);
				std::cout << std::endl;
				return false;
			}
			case SSL_ERROR_WANT_READ: {
				this->wantWrite = true;
				this->writeBuffer.clear();
				return true;
			}
			case SSL_ERROR_WANT_WRITE: {
				this->wantWrite = true;
				this->writeBuffer.clear();
				return true;
			}
			default: {
				std::cout << "SSL_write_ex() Error: " << SSL_get_error(this->ssl, returnValue) << std::endl;
				ERR_print_errors_fp(stdout);
				std::cout << std::endl;
				return false;
			}
			}
		}
		else if (FD_ISSET(this->theSocket, &readSet)) {
			this->wantRead = false;
			std::vector<uint8_t>  serverToClientBuffer{};
			serverToClientBuffer.resize(this->maxBufferSize);
			size_t readBytes{ 0 };
			int32_t returnValue = SSL_read_ex(this->ssl, serverToClientBuffer.data(), static_cast<int32_t>(this->maxBufferSize), &readBytes);
			int32_t errorValue = SSL_get_error(this->ssl, returnValue);
			switch (errorValue) {
			case SSL_ERROR_NONE: {
				if (readBytes > 0) {
					this->inputBufferPtr->insert(this->inputBufferPtr->end(), serverToClientBuffer.begin(), serverToClientBuffer.begin() + readBytes);
				}
				return true;
			}
			case SSL_ERROR_ZERO_RETURN: {
				std::cout << "SSL_read_ex() Error: " << SSL_get_error(this->ssl, returnValue) << std::endl;
				ERR_print_errors_fp(stdout);
				std::cout << std::endl;
				return false;
			}
			case SSL_ERROR_WANT_READ: {
				this->wantRead = true;
				return true;
			}
			case SSL_ERROR_WANT_WRITE: {
				this->wantRead = true;
				return true;
			}
			default: {
				std::cout << "SSL_read_ex() Error: " << SSL_get_error(this->ssl, returnValue) << std::endl;
				ERR_print_errors_fp(stdout);
				std::cout << std::endl;
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
		int32_t returnValue = getaddrinfo(baseUrlNew.c_str(), portNew.c_str(), hints, resultAddress);
		if (returnValue == SOCKET_ERROR) {
			std::cout << "getaddrinfo() Error: " << returnValue + ", ";
#ifdef _WIN32
			std::cout << WSAGetLastError() << std::endl;
#endif
			return;
		}
		this->theSocket = static_cast<DiscordCoreInternal::SOCKET>(socket(resultAddress->ai_family, resultAddress->ai_socktype, resultAddress->ai_protocol));
		if (static_cast<SOCKET>(this->theSocket) == INVALID_SOCKET) {
			std::cout << "socket() Error: ";
#ifdef _WIN32
			std::cout << WSAGetLastError() << std::endl;
#endif
			return;
		}
		returnValue = connect(this->theSocket, resultAddress->ai_addr, static_cast<int32_t>(resultAddress->ai_addrlen));
		if (returnValue == SOCKET_ERROR) {
			std::cout << "connect() Error: ";
#ifdef _WIN32
			std::cout << WSAGetLastError() << std::endl;
#endif
			return;
		}
#ifdef _WIN32
		char optionValue = true;
		returnValue = setsockopt(this->theSocket, IPPROTO_TCP, TCP_NODELAY, &optionValue, sizeof(bool));
		if (returnValue == SOCKET_ERROR) {
			std::cout << "setsockopt() Error: ";
			std::cout << WSAGetLastError() << std::endl;
			return;
		}
#endif
		this->context = SSL_CTX_new(TLS_client_method());
		if (this->context == nullptr) {
			std::cout << "SSL_CTX_new() Error: ";
			ERR_print_errors_fp(stdout);
			std::cout << std::endl;
			return;
		}
		this->ssl = SSL_new(this->context);
		if (this->ssl == nullptr) {
			std::cout << "SSL_new() Error: ";
			ERR_print_errors_fp(stdout);
			std::cout << std::endl;
			return;
		}
		returnValue = SSL_set_fd(this->ssl, this->theSocket);
		if (returnValue != 1) {
			std::cout << "SSL_set_fd() Error: " << SSL_get_error(this->ssl, returnValue) << std::endl;
			ERR_print_errors_fp(stdout);
			std::cout << std::endl;
			return;
		}
		returnValue = SSL_connect(this->ssl);
		if (returnValue != 1) {
			std::cout << "SSL_connect() Error: " << SSL_get_error(this->ssl, returnValue) << std::endl;
			ERR_print_errors_fp(stdout);
			std::cout << std::endl;
			return;
		}
	};

	WebSocketSSLClient::WebSocketSSLClient(nullptr_t) {};

	std::vector<uint8_t>WebSocketSSLClient::getData() {
		std::vector<uint8_t> newVector{};
		newVector.insert(newVector.begin(), this->inputBufferPtr->begin(), this->inputBufferPtr->end());
		this->inputBufferPtr->clear();
		return newVector;
	}

	bool WebSocketSSLClient::processIO(bool print) {
		fd_set writeSet{};
		fd_set readSet{};
		int32_t nfds{ 0 };
		FD_ZERO(&writeSet);
		FD_ZERO(&readSet);
		if ((this->writeBuffer.size() > 0 || this->wantWrite) && !this->wantRead) {
			FD_SET(this->theSocket, &writeSet);
			nfds = std::max(static_cast<int>(this->theSocket), nfds);
		}
		else {
			FD_SET(this->theSocket, &readSet);
			nfds = std::max(static_cast<int>(this->theSocket), nfds);
		}
		timeval checkTime{};
		checkTime.tv_sec = 1;
		auto resultValue = select(nfds + 1, &readSet, &writeSet, nullptr, &checkTime);
		if (resultValue == SOCKET_ERROR) {
			std::cout << "select() Error: " << resultValue + ", ";
#ifdef _WIN32
			std::cout << WSAGetLastError() << std::endl;
#else
			std::cout << errno << std::endl;
#endif
			return false;
		}
		else if (resultValue == 0) {
			return true;
		}

		if (FD_ISSET(this->theSocket, &writeSet)) {
			this->wantWrite = false;
			size_t writtenBytes{ 0 };
			if (this->ssl == nullptr) {
				std::cout << "ssl is nullptr. " << std::endl;
				return false;
			}
			int32_t returnValue = SSL_write_ex(this->ssl, this->writeBuffer.data(), static_cast<uint32_t>(this->writeBuffer.size()), &writtenBytes);
			int32_t errorValue = SSL_get_error(this->ssl, returnValue);
			switch (errorValue) {
			case SSL_ERROR_NONE: {
				this->writeBuffer.clear();
				return true;
			}
			case SSL_ERROR_ZERO_RETURN: {
				std::cout << "SSL_write_ex() Error: " << SSL_get_error(this->ssl, returnValue) << std::endl;
				ERR_print_errors_fp(stdout);
				std::cout << std::endl;
				return false;
			}
			case SSL_ERROR_WANT_READ: {
				this->wantWrite = true;
				this->writeBuffer.clear();
				return true;
			}
			case SSL_ERROR_WANT_WRITE: {
				this->wantWrite = true;
				this->writeBuffer.clear();
				return true;
			}
			default: {
				std::cout << "SSL_write_ex() Error: " << SSL_get_error(this->ssl, returnValue) << std::endl;
				ERR_print_errors_fp(stdout);
				std::cout << std::endl;
				return false;
			}
			}
		}
		else if (FD_ISSET(this->theSocket, &readSet)) {
			this->wantRead = false;
			std::vector<uint8_t>  serverToClientBuffer{};
			serverToClientBuffer.resize(this->maxBufferSize);
			size_t readBytes{ 0 };
			int32_t returnValue = SSL_read_ex(this->ssl, serverToClientBuffer.data(), static_cast<int32_t>(this->maxBufferSize), &readBytes);
			int32_t errorValue = SSL_get_error(this->ssl, returnValue);
			switch (errorValue) {
			case SSL_ERROR_NONE: {
				if (readBytes > 0) {
					this->inputBufferPtr->insert(this->inputBufferPtr->end(), serverToClientBuffer.begin(), serverToClientBuffer.begin() + readBytes);
					if (print) {
						std::cout << "READ AMOUNT: " << readBytes << std::endl;
						std::string theString{};
						theString.insert(theString.begin(), this->inputBufferPtr->begin(), this->inputBufferPtr->end());
						std::cout << "THE STRING: " << theString << std::endl;
					}
					this->bytesRead += readBytes;
				}
				return true;
			}
			case SSL_ERROR_ZERO_RETURN: {
				std::cout << "SSL_read_ex() Error: " << SSL_get_error(this->ssl, returnValue) << std::endl;
#ifdef _WIN32
				std::cout << WSAGetLastError() << std::endl;
#else
				std::cout << errno << std::endl;
#endif
				ERR_print_errors_fp(stdout);
				std::cout << std::endl;
				return false;
			}
			case SSL_ERROR_WANT_READ: {
				this->wantRead = true;
				return true;
			}
			case SSL_ERROR_WANT_WRITE: {
				this->wantRead = true;
				return true;
			}
			default: {
				std::cout << "SSL_read_ex() Error: " << SSL_get_error(this->ssl, returnValue) << std::endl;
#ifdef _WIN32
				std::cout << WSAGetLastError() << std::endl;
#else
				std::cout << errno << std::endl;
#endif
				ERR_print_errors_fp(stdout);
				std::cout << std::endl;
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

		int32_t returnValue = getaddrinfo(baseUrlNew.c_str(), portNew.c_str(), hints, resultAddress);
		if (returnValue == SOCKET_ERROR) {
			std::cout << "getaddrinfo() Error: ";
#ifdef _WIN32
			std::cout << WSAGetLastError() << std::endl;
#endif
			return;
		}

		this->theSocket = static_cast<DiscordCoreInternal::SOCKET>(socket(resultAddress->ai_family, resultAddress->ai_socktype, resultAddress->ai_protocol));
		if (static_cast<SOCKET>(this->theSocket) == INVALID_SOCKET) {
			std::cout << "socket() Error: ";
#ifdef _WIN32
			std::cout << WSAGetLastError() << std::endl;
#endif
			return;
		}

		returnValue = connect(this->theSocket, resultAddress->ai_addr, static_cast<int32_t>(resultAddress->ai_addrlen));
		if (returnValue == SOCKET_ERROR) {
			std::cout << "connect() Error: ";
#ifdef _WIN32
			std::cout << WSAGetLastError() << std::endl;
#endif
			return;
		}

		this->connectionBio = BIO_new_dgram(this->theSocket, BIO_CLOSE);
		if (this->connectionBio == nullptr) {
			std::cout << "BIO_new_dgram() Error: ";
			ERR_print_errors_fp(stdout);
			std::cout << std::endl;
			return;
		}

		returnValue = BIO_ctrl(this->connectionBio, BIO_CTRL_DGRAM_SET_CONNECTED, 0, &resultAddress);
		if (returnValue == 0) {
			std::cout << "BIO_ctrl() Error: ";
			ERR_print_errors_fp(stdout);
			std::cout << std::endl;
			return;
		}
		std::cout << "WERE HERE CONNECTION!" << std::endl;
		timeval timeout{};
		timeout.tv_sec = 1;
		timeout.tv_usec = 0;
		BIO_ctrl(this->connectionBio, BIO_CTRL_DGRAM_SET_RECV_TIMEOUT, 0, &timeout);
		BIO_ctrl(this->connectionBio, BIO_CTRL_DGRAM_SET_SEND_TIMEOUT, 0, &timeout);
		return;
	}

	DatagramSocketSSLClient::DatagramSocketSSLClient(nullptr_t) {};

	bool DatagramSocketSSLClient::writeData(std::string& data) {
		size_t writtenBytes{ 0 };
		if (this->connectionBio == nullptr) {
			std::cout << "DatagramSocketSSLClient() Error: Missing connectionBio!";
			return false;
		}
		int32_t returnValue = BIO_write_ex(this->connectionBio, data.data(), static_cast<uint32_t>(data.size()), &writtenBytes);
		if (returnValue != 1) {
			std::cout << "BIO_write_ex() Error: ";
			ERR_print_errors_fp(stdout);
			std::cout << std::endl;
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
		int32_t returnValue = BIO_read_ex(this->connectionBio, serverToClientBuffer.data(), this->maxBufferSize, &readBytes);
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