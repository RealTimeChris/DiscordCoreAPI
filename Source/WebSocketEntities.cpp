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
/// WebSocketEntities.cpp - Source file for the webSocket related classes and structs.
/// May 13, 2021
/// Chris M.
/// https://github.com/RealTimeChris/DiscordCoreAPI

#include <WebSocketEntities.hpp>
#include <JSONIfier.hpp>
#include <EventManager.hpp>
#include <CommandController.hpp>
#include <DiscordCoreClient.hpp>

namespace DiscordCoreInternal
{

	BaseSocketAgent::BaseSocketAgent(std::string botToken, std::string baseUrl)
	{
		this->authKey = DiscordCoreAPI::generateX64BaseEncodedKey();
		this->state = WebSocketState::Initializing;
		this->botToken = botToken;
		this->baseUrl = baseUrl;
		this->doWeReconnect.set();
		this->theTask = std::make_unique<DiscordCoreAPI::CoRoutine<void>>(this->run());
	}

	BaseSocketAgent::BaseSocketAgent(nullptr_t){};

	void BaseSocketAgent::sendMessage(std::string &dataToSend)
	{
		try
		{
			std::lock_guard<std::mutex> accessLock{this->accessorMutex01};
			std::cout << "Sending WebSocket Message: " << std::endl
					  << dataToSend;
			this->webSocket->writeData(dataToSend);
		}
		catch (...)
		{
			DiscordCoreAPI::reportException("BaseSocketAgent::sendMessage()");
			this->onClosedExternal();
		}
	}

	DiscordCoreAPI::TSUnboundedMessageBlock<WebSocketWorkload> &BaseSocketAgent::getWorkloadTarget()
	{
		return this->webSocketWorkloadTarget;
	}

	void BaseSocketAgent::sendMessage(nlohmann::json &dataToSend)
	{
		try
		{
			DiscordCoreAPI::StopWatch stopWatch{std::chrono::milliseconds{5500}};
			while (!this->areWeConnected.load(std::memory_order_seq_cst) && !(dataToSend.contains("op") && (dataToSend.at("op") == 2 || dataToSend.at("op") == 6)))
			{
				if (stopWatch.hasTimePassed())
				{
					return;
				}
			}
			std::lock_guard<std::mutex> accessLock{this->accessorMutex01};
			std::cout << "Sending WebSocket Message: " << dataToSend.dump() << std::endl
					  << std::endl;
			std::string theVector = this->erlPacker.parseJsonToEtf(dataToSend);
			std::string out{};
			out.resize(this->maxHeaderSize);
			size_t size = this->createHeader(out.data(), theVector.size(), this->dataOpcode);
			std::string header(out.data(), size);
			std::string theVectorNew{};
			theVectorNew.insert(theVectorNew.begin(), header.begin(), header.end());
			theVectorNew.insert(theVectorNew.begin() + header.size(), theVector.begin(), theVector.end());
			this->webSocket->writeData(theVectorNew);
		}
		catch (...)
		{
			DiscordCoreAPI::reportException("BaseSocketAgent::sendMessage()");
			this->onClosedExternal();
		}
	}

	uint64_t BaseSocketAgent::createHeader(char *outBuffer, uint64_t sendlength, WebSocketOpCode opCode)
	{
		try
		{
			size_t position{0};
			int32_t indexCount{0};
			outBuffer[position++] = this->webSocketFinishBit | static_cast<unsigned char>(opCode);
			if (sendlength <= this->webSocketMaxPayloadLengthSmall)
			{
				outBuffer[position++] = static_cast<unsigned char>(sendlength);
			}
			else if (sendlength <= this->webSocketMaxPayloadLengthLarge)
			{
				outBuffer[position++] = static_cast<unsigned char>(this->webSocketPayloadLengthMagicLarge);
				indexCount = 2;
			}
			else
			{
				outBuffer[position++] = this->webSocketPayloadLengthMagicHuge;
				indexCount = 8;
			}
			for (int32_t x = indexCount - 1; x >= 0; x--)
			{
				outBuffer[position++] = static_cast<unsigned char>(sendlength >> x * 8);
			}
			outBuffer[1] |= this->webSocketMaskBit;
			outBuffer[position++] = 0;
			outBuffer[position++] = 0;
			outBuffer[position++] = 0;
			outBuffer[position++] = 0;
			return position;
		}
		catch (...)
		{
			DiscordCoreAPI::reportException("BaseSocketAgent::createHeader()");
			this->onClosedExternal();
			return uint64_t{};
		}
	}

	std::vector<std::string> BaseSocketAgent::tokenize(std::string &dataIn, std::string separator)
	{
		try
		{
			std::string::size_type value{0};
			std::vector<std::string> dataOut{};
			while ((value = dataIn.find_first_not_of(separator, value)) != std::string::npos)
			{
				auto output = dataIn.find(separator, value);
				dataOut.push_back(dataIn.substr(value, output - value));
				value = output;
			}
			return dataOut;
		}
		catch (...)
		{
			DiscordCoreAPI::reportException("BaseSocketAgent::tokenize()");
			this->onClosedExternal();
			return std::vector<std::string>{};
		}
	}

	void BaseSocketAgent::getVoiceConnectionData(VoiceConnectInitData doWeCollect)
	{
		try
		{
			this->semaphore.acquire();
			this->voiceConnectInitData = doWeCollect;
			DiscordCoreAPI::UpdateVoiceStateData dataPackage{};
			dataPackage.channelId = "";
			dataPackage.guildId = this->voiceConnectInitData.guildId;
			dataPackage.selfDeaf = this->voiceConnectInitData.selfDeaf;
			dataPackage.selfMute = this->voiceConnectInitData.selfMute;
			nlohmann::json newData = JSONIFY(dataPackage);
			this->sendMessage(newData);
			std::this_thread::sleep_for(std::chrono::milliseconds{500});
			dataPackage.channelId = this->voiceConnectInitData.channelId;
			newData = JSONIFY(dataPackage);
			this->areWeCollectingData = true;
			this->sendMessage(newData);
			while (this->areWeCollectingData)
			{
				std::this_thread::sleep_for(std::chrono::milliseconds{1});
			}
			this->semaphore.release();
		}
		catch (...)
		{
			DiscordCoreAPI::reportException("BaseSocketAgent::getVoiceConnectionData()");
			this->onClosedExternal();
		}
	}

	DiscordCoreAPI::CoRoutine<void> BaseSocketAgent::run()
	{
		try
		{
			auto cancelHandle = co_await DiscordCoreAPI::NewThreadAwaitable<void>();
			this->connect();
			while (!cancelHandle.promise().isItStopped())
			{
				if (!this->doWeReconnect.wait(0))
				{
					this->onClosedInternal();
				}
				if (this->webSocket != nullptr)
				{
					if (!this->webSocket->processIO(1000))
					{
						this->onClosedExternal();
					}
					this->handleBuffer();
				}
			}
			co_return;
		}
		catch (...)
		{
			DiscordCoreAPI::reportException("BaseSocketAgent::run()");
			this->onClosedExternal();
			co_return;
		}
	}

	void BaseSocketAgent::onMessageReceived()
	{
		try
		{
			std::string messageNew = this->webSocket->getInputBuffer();
			this->webSocket->getInputBuffer().clear();
			nlohmann::json payload{};

			try
			{
				payload = this->erlPacker.parseEtfToJson(&messageNew);
			}
			catch (...)
			{
				return;
			}

			if (this->areWeCollectingData && payload.at("t") == "VOICE_SERVER_UPDATE" && !this->serverUpdateCollected)
			{
				if (!this->serverUpdateCollected && !this->stateUpdateCollected)
				{
					this->voiceConnectionData = VoiceConnectionData();
					this->voiceConnectionData.endPoint = payload.at("d").at("endpoint").get<std::string>();
					this->voiceConnectionData.token = payload.at("d").at("token").get<std::string>();
					this->serverUpdateCollected = true;
				}
				else
				{
					this->voiceConnectionData.endPoint = payload.at("d").at("endpoint").get<std::string>();
					this->voiceConnectionData.token = payload.at("d").at("token").get<std::string>();
					if (this->voiceConnectionDataBufferMap.contains(payload.at("d").at("guild_id")))
					{
						this->voiceConnectionDataBufferMap.at(payload.at("d").at("guild_id"))->send(this->voiceConnectionData);
					}
					this->serverUpdateCollected = false;
					this->stateUpdateCollected = false;
					this->areWeCollectingData = false;
				}
			}

			if (this->areWeCollectingData && payload.at("t") == "VOICE_STATE_UPDATE" && !this->stateUpdateCollected && payload.at("d").at("member").at("user").at("id") == this->voiceConnectInitData.userId)
			{
				if (!this->stateUpdateCollected && !this->serverUpdateCollected)
				{
					this->voiceConnectionData = VoiceConnectionData();
					this->voiceConnectionData.sessionId = payload.at("d").at("session_id").get<std::string>();
					this->stateUpdateCollected = true;
				}
				else
				{
					this->voiceConnectionData.sessionId = payload.at("d").at("session_id").get<std::string>();
					if (this->voiceConnectionDataBufferMap.contains(payload.at("d").at("guild_id")))
					{
						this->voiceConnectionDataBufferMap.at(payload.at("d").at("guild_id"))->send(this->voiceConnectionData);
					}
					this->serverUpdateCollected = false;
					this->stateUpdateCollected = false;
					this->areWeCollectingData = false;
				}
			}

			if (payload.at("s") >= 0)
			{
				this->lastNumberReceived = payload.at("s");
			}

			if (payload.at("t") == "RESUMED")
			{
				this->areWeConnected.store(true, std::memory_order_seq_cst);
				this->currentReconnectTries = 0;
				this->areWeReadyToConnectEvent.set();
			}

			if (payload.at("t") == "READY")
			{
				this->areWeConnected.store(true, std::memory_order_seq_cst);
				this->sessionId = payload.at("d").at("session_id");
				this->currentReconnectTries = 0;
				this->areWeReadyToConnectEvent.set();
				this->areWeAuthenticated = true;
			}

			if (payload.at("op") == 1)
			{
				this->sendHeartBeat();
			}

			if (payload.at("op") == 7)
			{
				std::cout << "Reconnecting (Type 7)!" << std::endl
						  << std::endl;
				this->areWeResuming = true;
				this->currentReconnectTries += 1;
				this->areWeConnected.store(false, std::memory_order_seq_cst);
				this->heartbeatTimer->cancel();
				this->webSocket.reset(nullptr);
				this->connect();
			}

			if (payload.at("op") == 9)
			{
				std::cout << "Reconnecting (Type 9)!" << std::endl
						  << std::endl;
				srand(static_cast<uint32_t>(std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count()));
				this->currentReconnectTries += 1;
				int32_t numOfMsToWait = static_cast<int32_t>(1000.0f + ((static_cast<float>(rand()) / static_cast<float>(RAND_MAX)) * static_cast<float>(4000.0f)));
				std::this_thread::sleep_for(std::chrono::milliseconds{numOfMsToWait});
				if (payload.at("d") == true)
				{
					nlohmann::json identityJson = JSONIFY(this->botToken, static_cast<int32_t>(this->intentsValue));
					this->sendMessage(identityJson);
				}
				else
				{
					this->areWeConnected.store(false, std::memory_order_seq_cst);
					this->heartbeatTimer->cancel();
					this->webSocket.reset(nullptr);
					this->areWeResuming = false;
					this->areWeAuthenticated = false;
					this->connect();
				}
			}

			if (payload.at("op") == 10)
			{
				this->heartbeatInterval = payload.at("d").at("heartbeat_interval");
				DiscordCoreAPI::TimeElapsedHandler onHeartBeat = [this]()
				{
					BaseSocketAgent::sendHeartBeat();
				};
				this->heartbeatTimer = std::make_unique<DiscordCoreAPI::ThreadPoolTimer>(DiscordCoreAPI::ThreadPoolTimer::createPeriodicTimer(onHeartBeat, this->heartbeatInterval));
				if (!this->areWeAuthenticated)
				{
					nlohmann::json identityJson = JSONIFY(this->botToken, static_cast<int32_t>(this->intentsValue));
					this->sendMessage(identityJson);
				}
				if (this->areWeResuming)
				{
					std::this_thread::sleep_for(std::chrono::milliseconds{1500});
					nlohmann::json resumePayload = JSONIFY(this->botToken, this->sessionId, this->lastNumberReceived);
					this->sendMessage(resumePayload);
				}
			}

			if (payload.at("op") == 11)
			{
				this->haveWeReceivedHeartbeatAck = true;
			}

			if (payload.contains("d") && !payload.at("d").is_null() && payload.contains("t") && !payload.at("t").is_null())
			{
				WebSocketWorkload webSocketWorkload{};
				webSocketWorkload.payLoad.update(std::move(payload.at("d")));

				switch (payload.at("t"))
				{
				case "APPLICATION_COMMAND_CREATE":
					webSocketWorkload.eventType = WebSocketEventType::Application_Command_Create;
					this->webSocketWorkloadTarget.send(std::move(webSocketWorkload));
					break;

				case "APPLICATION_COMMAND_UPDATE":
					webSocketWorkload.eventType = WebSocketEventType::Application_Command_Update;
					this->webSocketWorkloadTarget.send(std::move(webSocketWorkload));
					break;

				case "APPLICATION_COMMAND_DELETE":
					webSocketWorkload.eventType = WebSocketEventType::Application_Command_Delete;
					this->webSocketWorkloadTarget.send(std::move(webSocketWorkload));
					break;

				case "CHANNEL_CREATE":
					webSocketWorkload.eventType = WebSocketEventType::Channel_Create;
					this->webSocketWorkloadTarget.send(std::move(webSocketWorkload));
					break;

				case "CHANNEL_UPDATE":
					webSocketWorkload.eventType = WebSocketEventType::Channel_Update;
					this->webSocketWorkloadTarget.send(std::move(webSocketWorkload));
					break;

				case "CHANNEL_DELETE":
					webSocketWorkload.eventType = WebSocketEventType::Channel_Delete;
					this->webSocketWorkloadTarget.send(std::move(webSocketWorkload));
					break;

				case "CHANNEL_PINS_UPDATE":
					webSocketWorkload.eventType = WebSocketEventType::Channel_Pins_Update;
					this->webSocketWorkloadTarget.send(std::move(webSocketWorkload));
					break;

				case "THREAD_CREATE":
					webSocketWorkload.eventType = WebSocketEventType::Thread_Create;
					this->webSocketWorkloadTarget.send(std::move(webSocketWorkload));
					break;

				case "THREAD_UPDATE":
					webSocketWorkload.eventType = WebSocketEventType::Thread_Update;
					this->webSocketWorkloadTarget.send(std::move(webSocketWorkload));
					break;

				case "THREAD_DELETE":
					webSocketWorkload.eventType = WebSocketEventType::Thread_Delete;
					this->webSocketWorkloadTarget.send(std::move(webSocketWorkload));
					break;

				case "THREAD_LIST_SYNC":
					webSocketWorkload.eventType = WebSocketEventType::Thread_List_Sync;
					this->webSocketWorkloadTarget.send(std::move(webSocketWorkload));
					break;

				case "THREAD_MEMBER_UPDATE":
					webSocketWorkload.eventType = WebSocketEventType::Thread_Member_Update;
					this->webSocketWorkloadTarget.send(std::move(webSocketWorkload));
					break;

				case "THREAD_MEMBERS_UPDATE":
					webSocketWorkload.eventType = WebSocketEventType::Thread_Members_Update;
					this->webSocketWorkloadTarget.send(std::move(webSocketWorkload));
					break;

				case "GUILD_CREATE":
					webSocketWorkload.eventType = WebSocketEventType::Guild_Create;
					this->webSocketWorkloadTarget.send(std::move(webSocketWorkload));
					break;

				case "GUILD_UPDATE":
					webSocketWorkload.eventType = WebSocketEventType::Guild_Update;
					this->webSocketWorkloadTarget.send(std::move(webSocketWorkload));
					break;

				case "GUILD_DELETE":
					webSocketWorkload.eventType = WebSocketEventType::Guild_Delete;
					this->webSocketWorkloadTarget.send(std::move(webSocketWorkload));
					break;

				case "GUILD_BAN_ADD":
					webSocketWorkload.eventType = WebSocketEventType::Guild_Ban_Add;
					this->webSocketWorkloadTarget.send(std::move(webSocketWorkload));
					break;

				case "GUILD_BAN_REMOVE":
					webSocketWorkload.eventType = WebSocketEventType::Guild_Ban_Remove;
					this->webSocketWorkloadTarget.send(std::move(webSocketWorkload));
					break;

				case "GUILD_EMOJIS_UPDATE":
					webSocketWorkload.eventType = WebSocketEventType::Guild_Emojis_Update;
					this->webSocketWorkloadTarget.send(std::move(webSocketWorkload));
					break;

				case "GUILD_STICKERS_UPDATE":
					webSocketWorkload.eventType = WebSocketEventType::Guild_Stickers_Update;
					this->webSocketWorkloadTarget.send(std::move(webSocketWorkload));
					break;

				case "GUILD_INTEGRATIONS_UPDATE":
					webSocketWorkload.eventType = WebSocketEventType::Guild_Integrations_Update;
					this->webSocketWorkloadTarget.send(std::move(webSocketWorkload));
					break;

				case "GUILD_MEMBER_ADD":
					webSocketWorkload.eventType = WebSocketEventType::Guild_Member_Add;
					this->webSocketWorkloadTarget.send(std::move(webSocketWorkload));
					break;

				case "GUILD_MEMBER_REMOVE":
					webSocketWorkload.eventType = WebSocketEventType::Guild_Member_Remove;
					this->webSocketWorkloadTarget.send(std::move(webSocketWorkload));
					break;

				case "GUILD_MEMBERS_CHUNK":
					webSocketWorkload.eventType = WebSocketEventType::Guild_Members_Chunk;
					this->webSocketWorkloadTarget.send(std::move(webSocketWorkload));
					break;

				case "GUILD_ROLE_CREATE":
					webSocketWorkload.eventType = WebSocketEventType::Guild_Role_Create;
					this->webSocketWorkloadTarget.send(std::move(webSocketWorkload));
					break;

				case "GUILD_ROLE_UPDATE":
					webSocketWorkload.eventType = WebSocketEventType::Guild_Role_Update;
					this->webSocketWorkloadTarget.send(std::move(webSocketWorkload));
					break;

				case "GUILD_ROLE_DELETE":
					webSocketWorkload.eventType = WebSocketEventType::Guild_Role_Delete;
					this->webSocketWorkloadTarget.send(std::move(webSocketWorkload));
					break;

				case "INTEGRATION_ROLE_CREATE":
					webSocketWorkload.eventType = WebSocketEventType::Integration_Create;
					this->webSocketWorkloadTarget.send(std::move(webSocketWorkload));
					break;

				case "INTEGRATION_ROLE_UPDATE":
					webSocketWorkload.eventType = WebSocketEventType::Integration_Update;
					this->webSocketWorkloadTarget.send(std::move(webSocketWorkload));
					break;

				case "INTEGRATION_ROLE_DELETE":
					webSocketWorkload.eventType = WebSocketEventType::Integration_Delete;
					this->webSocketWorkloadTarget.send(std::move(webSocketWorkload));
					break;

				case "INTERACTION_CREATE":
					webSocketWorkload.eventType = WebSocketEventType::Interaction_Create;
					this->webSocketWorkloadTarget.send(std::move(webSocketWorkload));
					break;

				case "INVITE_CREATE":
					webSocketWorkload.eventType = WebSocketEventType::Invite_Create;
					this->webSocketWorkloadTarget.send(std::move(webSocketWorkload));
					break;

				case "INVITE_DELETE":
					webSocketWorkload.eventType = WebSocketEventType::Invite_Delete;
					this->webSocketWorkloadTarget.send(std::move(webSocketWorkload));
					break;

				case "MESSAGE_CREATE":
					webSocketWorkload.eventType = WebSocketEventType::Message_Create;
					this->webSocketWorkloadTarget.send(std::move(webSocketWorkload));
					break;

				case "MESSAGE_UPDATE":
					webSocketWorkload.eventType = WebSocketEventType::Message_Update;
					this->webSocketWorkloadTarget.send(std::move(webSocketWorkload));
					break;

				case "MESSAGE_DELETE":
					webSocketWorkload.eventType = WebSocketEventType::Message_Delete;
					this->webSocketWorkloadTarget.send(std::move(webSocketWorkload));
					break;

				case "MESSAGE_DELETE_BULK":
					webSocketWorkload.eventType = WebSocketEventType::Message_Delete_Bulk;
					this->webSocketWorkloadTarget.send(std::move(webSocketWorkload));
					break;

				case "MESSAGE_REACTION_ADD":
					webSocketWorkload.eventType = WebSocketEventType::Message_Reaction_Add;
					this->webSocketWorkloadTarget.send(std::move(webSocketWorkload));
					break;

				case "MESSAGE_REACTION_REMOVE":
					webSocketWorkload.eventType = WebSocketEventType::Message_Delete_Bulk;
					this->webSocketWorkloadTarget.send(std::move(webSocketWorkload));
					break;

				case "MESSAGE_REACTION_REMOVE_ALL":
					webSocketWorkload.eventType = WebSocketEventType::Message_Reaction_Remove_All;
					this->webSocketWorkloadTarget.send(std::move(webSocketWorkload));
					break;

				case "PRESENCE_UPDATE":
					webSocketWorkload.eventType = WebSocketEventType::Presence_Update;
					this->webSocketWorkloadTarget.send(std::move(webSocketWorkload));
					break;

				case "STAGE_INSTANCE_CREATE":
					webSocketWorkload.eventType = WebSocketEventType::Stage_Instance_Create;
					this->webSocketWorkloadTarget.send(std::move(webSocketWorkload));
					break;

				case "STAGE_INSTANCE_UPDATE":
					webSocketWorkload.eventType = WebSocketEventType::Stage_Instance_Update;
					this->webSocketWorkloadTarget.send(std::move(webSocketWorkload));
					break;

				case "STAGE_INSTANCE_DELETE":
					webSocketWorkload.eventType = WebSocketEventType::Stage_Instance_Delete;
					this->webSocketWorkloadTarget.send(std::move(webSocketWorkload));
					break;

				case "TYPING_START":
					webSocketWorkload.eventType = WebSocketEventType::Typing_Start;
					this->webSocketWorkloadTarget.send(std::move(webSocketWorkload));
					break;

				case "USER_UPDATE":
					webSocketWorkload.eventType = WebSocketEventType::User_Update;
					this->webSocketWorkloadTarget.send(std::move(webSocketWorkload));
					break;

				case "VOICE_STATE_UPDATE":
					webSocketWorkload.eventType = WebSocketEventType::Voice_State_Update;
					this->webSocketWorkloadTarget.send(std::move(webSocketWorkload));
					break;

				case "VOICE_SERVER_UPDATE":
					webSocketWorkload.eventType = WebSocketEventType::Voice_Server_Update;
					this->webSocketWorkloadTarget.send(std::move(webSocketWorkload));
					break;

				case "WEBHOOKS_UPDATE":
					webSocketWorkload.eventType = WebSocketEventType::Webhooks_Update;
					this->webSocketWorkloadTarget.send(std::move(webSocketWorkload));
					break;
				}
			}
			std::cout << "Message received from WebSocket: " << payload.dump() << std::endl
					  << std::endl;
			return;
		}
		catch (...)
		{
			DiscordCoreAPI::reportException("BaseSocketAgent::onMessageReceived()");
			this->onClosedExternal();
			return;
		}
	}

	void BaseSocketAgent::sendHeartBeat()
	{
		try
		{
			if (this->haveWeReceivedHeartbeatAck)
			{
				nlohmann::json heartbeat = JSONIFY(this->lastNumberReceived);
				this->sendMessage(heartbeat);
				this->haveWeReceivedHeartbeatAck = false;
			}
			else
			{
				this->onClosedExternal();
			}
		}
		catch (...)
		{
			DiscordCoreAPI::reportException("BaseSocketAgent::sendHeartBeat()");
			this->onClosedExternal();
		}
	}

	void BaseSocketAgent::handleBuffer()
	{
		try
		{
			std::string newVector{};
			switch (this->state)
			{
			case WebSocketState::Initializing:
				newVector.insert(newVector.begin(), this->webSocket->getInputBuffer().begin(), this->webSocket->getInputBuffer().end());
				if (newVector.find("\r\n\r\n") != std::string::npos)
				{
					std::string headers = newVector.substr(0, newVector.find("\r\n\r\n"));
					newVector.erase(0, newVector.find("\r\n\r\n") + 4);
					std::vector<std::string> headerOut = tokenize(headers);
					if (headerOut.size())
					{
						std::string statusLine = headerOut[0];
						headerOut.erase(headerOut.begin());
						std::vector<std::string> status = tokenize(statusLine, " ");
						if (status.size() >= 3 && status[1] == "101")
						{
							this->state = WebSocketState::Connected;
							this->webSocket->getInputBuffer().clear();
							this->webSocket->getInputBuffer().insert(this->webSocket->getInputBuffer().end(), newVector.begin(), newVector.end());
							this->parseHeader();
						}
						else
						{
							return;
						}
					}
				}
				break;
			case WebSocketState::Connected:
				while (this->parseHeader())
				{
				};
				return;
			}
		}
		catch (...)
		{
			DiscordCoreAPI::reportException("BaseSocketAgent::handleBuffer()");
			this->onClosedExternal();
		}
	}

	bool BaseSocketAgent::parseHeader()
	{
		try
		{
			std::string newVector = this->webSocket->getInputBuffer();
			if (this->webSocket->getInputBuffer().size() < 4)
			{
				return false;
			}
			else
			{
				switch (static_cast<WebSocketOpCode>(this->webSocket->getInputBuffer()[0] & ~this->webSocketMaskBit))
				{
				case WebSocketOpCode::Op_Continuation:
				case WebSocketOpCode::Op_Text:
				case WebSocketOpCode::Op_Binary:
				case WebSocketOpCode::Op_Ping:
				case WebSocketOpCode::Op_Pong:
				{
					uint8_t length01 = this->webSocket->getInputBuffer()[1];
					int32_t payloadStartOffset = 2;
					if (length01 & this->webSocketMaskBit)
					{
						return false;
					}
					uint64_t length02 = length01;
					if (length01 == this->webSocketPayloadLengthMagicLarge)
					{
						if (this->webSocket->getInputBuffer().size() < 8)
						{
							return false;
						}
						uint8_t length03 = this->webSocket->getInputBuffer()[2];
						uint8_t length04 = this->webSocket->getInputBuffer()[3];
						length02 = static_cast<uint64_t>((length03 << 8) | length04);
						payloadStartOffset += 2;
					}
					else if (length01 == this->webSocketPayloadLengthMagicHuge)
					{
						if (this->webSocket->getInputBuffer().size() < 10)
						{
							return false;
						}
						length02 = 0;
						for (int32_t value = 2, shift = 56; value < 10; ++value, shift -= 8)
						{
							uint8_t length05 = static_cast<uint8_t>(this->webSocket->getInputBuffer()[value]);
							length02 |= static_cast<uint64_t>(length05) << static_cast<uint64_t>(shift);
						}
						payloadStartOffset += 8;
					}
					if (this->webSocket->getInputBuffer().size() < payloadStartOffset + length02)
					{
						return false;
					}
					else
					{
						std::string newerVector{};
						newerVector.reserve(length02);
						for (uint32_t x = payloadStartOffset; x < payloadStartOffset + length02; x += 1)
						{
							newerVector.push_back(this->webSocket->getInputBuffer()[x]);
						}
						this->webSocket->getInputBuffer() = std::move(newerVector);
						this->onMessageReceived();
						this->webSocket->getInputBuffer().insert(this->webSocket->getInputBuffer().begin(), newVector.begin() + payloadStartOffset + length02, newVector.end());
					}
					return true;
				}
				case WebSocketOpCode::Op_Close:
				{
					uint16_t close = this->webSocket->getInputBuffer()[2] & 0xff;
					close <<= 8;
					close |= this->webSocket->getInputBuffer()[3] & 0xff;
					this->closeCode = close;
					std::cout << "WebSocket Closed; Code: " << this->closeCode << std::endl;
					this->onClosedExternal();
					return false;
				}
				default:
				{
					this->closeCode = 0;
					return false;
				}
				}
			}
			return false;
		}
		catch (...)
		{
			DiscordCoreAPI::reportException("BaseSocketAgent::parseHeader()");
			this->onClosedExternal();
			return false;
		}
	}

	void BaseSocketAgent::onClosedExternal()
	{
		this->doWeReconnect.reset();
	}

	void BaseSocketAgent::onClosedInternal()
	{
		this->areWeReadyToConnectEvent.reset();
		if (this->maxReconnectTries > this->currentReconnectTries)
		{
			this->areWeConnected.store(false, std::memory_order_seq_cst);
			this->closeCode = 1000;
			this->currentReconnectTries += 1;
			this->webSocket.reset(nullptr);
			this->areWeAuthenticated = false;
			if (this->heartbeatTimer->running())
			{
				this->heartbeatTimer->cancel();
			}
			this->haveWeReceivedHeartbeatAck = true;
			this->connect();
		}
		else if (this->maxReconnectTries <= this->currentReconnectTries)
		{
			this->theTask->cancel();
		}
	}

	void BaseSocketAgent::connect()
	{
		try
		{
			this->authKey = DiscordCoreAPI::generateX64BaseEncodedKey();
			this->webSocket = std::make_unique<WebSocketSSLClient>(this->baseUrl, this->port);
			this->state = WebSocketState::Initializing;
			this->doWeReconnect.set();
			std::string sendString = "GET " + this->relativePath + " HTTP/1.1\r\nHost: " + this->baseUrl +
									 "\r\nPragma: no-cache\r\nUser-Agent: DiscordCoreAPI/1.0\r\nUpgrade: WebSocket\r\nConnection: Upgrade\r\nSec-WebSocket-Key: " +
									 this->authKey + "\r\nSec-WebSocket-Version: 13\r\n\r\n";
			this->sendMessage(sendString);
		}
		catch (...)
		{
			DiscordCoreAPI::reportException("BaseSocketAgent::connect()");
			this->onClosedExternal();
		}
	}

	BaseSocketAgent::~BaseSocketAgent()
	{
		this->theTask->cancel();
		this->heartbeatTimer->cancel();
	}

	VoiceSocketAgent::VoiceSocketAgent(VoiceConnectInitData initDataNew, BaseSocketAgent *baseBaseSocketAgentNew)
	{
		this->authKey = DiscordCoreAPI::generateX64BaseEncodedKey();
		this->baseSocketAgent = baseBaseSocketAgentNew;
		this->voiceConnectInitData = initDataNew;
		this->baseSocketAgent->voiceConnectionDataBufferMap.insert_or_assign(this->voiceConnectInitData.guildId, &this->voiceConnectionDataBuffer);
		this->baseSocketAgent->getVoiceConnectionData(this->voiceConnectInitData);
		this->doWeReconnect.set();
		this->areWeConnected.reset();
		this->theTask = std::make_unique<DiscordCoreAPI::CoRoutine<void>>(this->run());
	}

	void VoiceSocketAgent::sendVoiceData(std::string &responseData)
	{
		try
		{
			if (responseData.size() == 0)
			{
				std::cout << "Please specify voice data to send" << std::endl
						  << std::endl;
				return;
			}
			else
			{
				if (!this->voiceSocket->writeData(responseData))
				{
					this->onClosedExternal();
				}
			}
		}
		catch (...)
		{
			DiscordCoreAPI::reportException("VoiceSocketAgent::sendVoiceData()");
			this->onClosedExternal();
		}
	}

	void VoiceSocketAgent::sendMessage(std::vector<uint8_t> &dataToSend)
	{
		try
		{
			std::string newString{};
			newString.insert(newString.begin(), dataToSend.begin(), dataToSend.end());
			std::cout << "Sending Voice WebSocket Message: " << newString << std::endl
					  << std::endl;
			std::vector<char> out{};
			out.resize(this->maxHeaderSize);
			size_t size = this->createHeader(out.data(), newString.size(), this->dataOpcode);
			std::string header(out.data(), size);
			std::string theVectorNew{};
			theVectorNew.insert(theVectorNew.begin(), header.begin(), header.end());
			theVectorNew.insert(theVectorNew.begin() + header.size(), newString.begin(), newString.end());
			this->webSocket->writeData(theVectorNew);
		}
		catch (...)
		{
			DiscordCoreAPI::reportException("VoiceSocketAgent::sendMessage()");
			this->onClosedExternal();
		}
	}

	void VoiceSocketAgent::sendMessage(std::string &dataToSend)
	{
		try
		{
			std::cout << "Sending Voice WebSocket Message: " << std::endl
					  << dataToSend;
			this->webSocket->writeData(dataToSend);
		}
		catch (...)
		{
			DiscordCoreAPI::reportException("VoiceSocketAgent::sendMessage()");
			this->onClosedExternal();
		}
	}

	uint64_t VoiceSocketAgent::createHeader(char *outBuffer, uint64_t sendlength, WebSocketOpCode opCode)
	{
		try
		{
			size_t position = 0;
			outBuffer[position++] = this->webSocketFinishBit | static_cast<unsigned char>(opCode);
			if (sendlength <= this->webSocketMaxPayloadLengthSmall)
			{
				outBuffer[position++] = static_cast<unsigned char>(sendlength);
			}
			else if (sendlength <= this->webSocketMaxPayloadLengthLarge)
			{
				outBuffer[position++] = static_cast<unsigned char>(this->webSocketPayloadLengthMagicLarge);
				outBuffer[position++] = static_cast<unsigned char>(sendlength >> 8);
				outBuffer[position++] = static_cast<unsigned char>(sendlength);
			}
			else
			{
				outBuffer[position++] = this->webSocketPayloadLengthMagicHuge;
				const uint64_t length02 = sendlength;
				for (int32_t x = sizeof(uint64_t) - 1; x >= 0; x--)
				{
					outBuffer[position++] = static_cast<unsigned char>(length02 >> x * 8);
				}
			}
			outBuffer[1] |= this->webSocketMaskBit;
			outBuffer[position++] = 0;
			outBuffer[position++] = 0;
			outBuffer[position++] = 0;
			outBuffer[position++] = 0;
			return position;
		}
		catch (...)
		{
			DiscordCoreAPI::reportException("VoiceSocketAgent::createHeader()");
			this->onClosedExternal();
			return size_t{};
		}
	}

	std::vector<std::string> VoiceSocketAgent::tokenize(std::string &dataIn, std::string separator)
	{
		try
		{
			std::string::size_type value{0};
			std::vector<std::string> dataOut{};
			while ((value = dataIn.find_first_not_of(separator, value)) != std::string::npos)
			{
				auto output = dataIn.find(separator, value);
				dataOut.push_back(dataIn.substr(value, output - value));
				value = output;
			}
			return dataOut;
		}
		catch (...)
		{
			DiscordCoreAPI::reportException("VoiceSocketAgent::tokenize()");
			this->onClosedExternal();
			return std::vector<std::string>{};
		}
	}

	DiscordCoreAPI::CoRoutine<void> VoiceSocketAgent::run()
	{
		try
		{
			auto cancelHandle = co_await DiscordCoreAPI::NewThreadAwaitable<void>();
			this->connect();
			while (!cancelHandle.promise().isItStopped())
			{
				if (!this->doWeReconnect.wait(0))
				{
					this->onClosedInternal();
					co_return;
				}
				if (this->webSocket != nullptr)
				{
					if (!this->webSocket->processIO(1000))
					{
						this->onClosedExternal();
					}
				}
				if (this->voiceSocket != nullptr)
				{
					this->voiceSocket->readData(true);
				}
				this->handleBuffer();
			}
			co_return;
		}
		catch (...)
		{
			DiscordCoreAPI::reportException("VoiceSocketAgent::run()");
			this->onClosedExternal();
			co_return;
		}
	}

	void VoiceSocketAgent::onMessageReceived()
	{
		try
		{
			std::string message = this->webSocket->getInputBuffer();
			this->webSocket->getInputBuffer().clear();
			nlohmann::json payload = payload.parse(message);
			std::cout << "Message received from Voice WebSocket: " << message << std::endl
					  << std::endl;
			if (payload.contains("op"))
			{
				if (payload.at("op") == 6)
				{
					this->haveWeReceivedHeartbeatAck = true;
				};
				if (payload.at("op") == 2)
				{
					this->voiceConnectionData.audioSSRC = payload.at("d").at("ssrc").get<uint32_t>();
					this->voiceConnectionData.voiceIp = payload.at("d").at("ip").get<std::string>();
					this->voiceConnectionData.voicePort = std::to_string(payload.at("d").at("port").get<int64_t>());
					for (auto &value : payload.at("d").at("modes"))
					{
						if (value == "xsalsa20_poly1305")
						{
							this->voiceConnectionData.voiceEncryptionMode = value;
						}
					}
					this->voiceConnect();
					this->collectExternalIP();
					std::vector<uint8_t> protocolPayloadSelectString = JSONIFY(this->voiceConnectionData.voicePort, this->voiceConnectionData.externalIp, this->voiceConnectionData.voiceEncryptionMode, 0);
					if (this->webSocket != nullptr)
					{
						this->sendMessage(protocolPayloadSelectString);
					}
				}
				if (payload.at("op") == 4)
				{
					for (uint32_t x = 0; x < payload.at("d").at("secret_key").size(); x += 1)
					{
						this->voiceConnectionData.secretKey.push_back(payload.at("d").at("secret_key").at(x).get<uint8_t>());
					}
					this->areWeConnected.set();
				}
				if (payload.at("op") == 9)
				{
				};
				if (payload.at("op") == 8)
				{
					if (payload.at("d").contains("heartbeat_interval"))
					{
						this->heartbeatInterval = static_cast<int32_t>(payload.at("d").at("heartbeat_interval").get<float>());
					}
					DiscordCoreAPI::TimeElapsedHandler onHeartBeat{[&, this]() -> void
																   {
																	   VoiceSocketAgent::sendHeartBeat();
																   }};
					this->heartbeatTimer = DiscordCoreAPI::ThreadPoolTimer{DiscordCoreAPI::ThreadPoolTimer::createPeriodicTimer(onHeartBeat, this->heartbeatInterval)};
					this->haveWeReceivedHeartbeatAck = true;
					std::vector<uint8_t> identifyPayload = JSONIFY(this->voiceConnectionData, this->voiceConnectInitData);
					if (this->webSocket != nullptr)
					{
						this->sendMessage(identifyPayload);
					}
				}
			}
		}
		catch (...)
		{
			DiscordCoreAPI::reportException("VoiceSocketAgent::onMessageReceived()");
			this->onClosedExternal();
		}
	}

	void VoiceSocketAgent::collectExternalIP()
	{
		try
		{
			std::string packet{};
			packet.resize(74);
			uint16_t val1601{0x01};
			uint16_t val1602{70};
			packet[0] = static_cast<uint8_t>(val1601 >> 8);
			packet[1] = static_cast<uint8_t>(val1601 >> 0);
			packet[2] = static_cast<uint8_t>(val1602 >> 8);
			packet[3] = static_cast<uint8_t>(val1602 >> 0);
			packet[4] = static_cast<uint8_t>(this->voiceConnectionData.audioSSRC >> 24);
			packet[5] = static_cast<uint8_t>(this->voiceConnectionData.audioSSRC >> 16);
			packet[6] = static_cast<uint8_t>(this->voiceConnectionData.audioSSRC >> 8);
			packet[7] = static_cast<uint8_t>(this->voiceConnectionData.audioSSRC);
			if (!this->voiceSocket->writeData(packet))
			{
				this->onClosedExternal();
				return;
			};
			while (this->voiceSocket->getInputBuffer().size() < 74)
			{
				this->voiceSocket->readData(false);
			}
			std::string message{};
			message.insert(message.begin(), this->voiceSocket->getInputBuffer().begin() + 8, this->voiceSocket->getInputBuffer().begin() + 64);
			if (message.find('\u0000') != std::string::npos)
			{
				message = message.substr(0, message.find('\u0000', 5));
			}
			this->voiceSocket->getInputBuffer().clear();
			this->voiceConnectionData.externalIp = message;
		}
		catch (...)
		{
			DiscordCoreAPI::reportException("VoiceSocketAgent::collectExternalIP()");
			this->onClosedExternal();
		}
	}

	void VoiceSocketAgent::sendHeartBeat()
	{
		try
		{
			if (this->haveWeReceivedHeartbeatAck)
			{
				std::vector<uint8_t> heartbeatPayload = JSONIFY(std::chrono::duration_cast<std::chrono::nanoseconds>(std::chrono::system_clock::now().time_since_epoch()).count());
				if (this->webSocket != nullptr)
				{
					this->sendMessage(heartbeatPayload);
				}
				this->haveWeReceivedHeartbeatAck = false;
			}
			else
			{
				this->onClosedExternal();
			}
		}
		catch (...)
		{
			DiscordCoreAPI::reportException("VoiceSocketAgent::sendHeartBeat()");
			this->onClosedExternal();
		}
	}

	void VoiceSocketAgent::voiceConnect()
	{
		try
		{
			this->voiceSocket = std::make_unique<DatagramSocketSSLClient>(this->voiceConnectionData.voiceIp, this->voiceConnectionData.voicePort);
		}
		catch (...)
		{
			DiscordCoreAPI::reportException("VoiceSocketAgent::voiceConnect()");
			this->onClosedExternal();
		}
	}

	void VoiceSocketAgent::handleBuffer()
	{
		try
		{
			std::string newVector{};
			switch (this->state)
			{
			case WebSocketState::Initializing:
				newVector.insert(newVector.begin(), this->webSocket->getInputBuffer().begin(), this->webSocket->getInputBuffer().end());
				if (newVector.find("\r\n\r\n") != std::string::npos)
				{
					std::string headers = newVector.substr(0, newVector.find("\r\n\r\n"));
					newVector.erase(0, newVector.find("\r\n\r\n") + 4);
					std::vector<std::string> headerOut = tokenize(headers);
					if (headerOut.size())
					{
						std::string statusLine = headerOut[0];
						headerOut.erase(headerOut.begin());
						std::vector<std::string> status = tokenize(statusLine, " ");
						if (status.size() >= 3 && status[1] == "101")
						{
							this->state = WebSocketState::Connected;
							this->webSocket->getInputBuffer().clear();
							this->webSocket->getInputBuffer().insert(this->webSocket->getInputBuffer().end(), newVector.begin(), newVector.end());
							this->parseHeader();
						}
						else
						{
							return;
						}
					}
				}
				break;
			case WebSocketState::Connected:
				while (this->parseHeader())
				{
				};
				return;
			}
		}
		catch (...)
		{
			DiscordCoreAPI::reportException("VoiceSocketAgent::handleBuffer()");
			this->onClosedExternal();
		}
	}

	bool VoiceSocketAgent::parseHeader()
	{
		try
		{
			std::string newVector = this->webSocket->getInputBuffer();
			if (this->webSocket->getInputBuffer().size() < 4)
			{
				return false;
			}
			else
			{
				switch (static_cast<WebSocketOpCode>(this->webSocket->getInputBuffer()[0] & ~this->webSocketMaskBit))
				{
				case WebSocketOpCode::Op_Continuation:
				case WebSocketOpCode::Op_Text:
				case WebSocketOpCode::Op_Binary:
				case WebSocketOpCode::Op_Ping:
				case WebSocketOpCode::Op_Pong:
				{
					uint8_t length01 = this->webSocket->getInputBuffer()[1];
					int32_t payloadStartOffset = 2;
					if (length01 & this->webSocketMaskBit)
					{
						return false;
					}
					uint64_t length02 = length01;
					if (length01 == this->webSocketPayloadLengthMagicLarge)
					{
						if (this->webSocket->getInputBuffer().size() < 8)
						{
							return false;
						}
						uint8_t length03 = this->webSocket->getInputBuffer()[2];
						uint8_t length04 = this->webSocket->getInputBuffer()[3];
						length02 = static_cast<uint64_t>((length03 << 8) | length04);
						payloadStartOffset += 2;
					}
					else if (length01 == this->webSocketPayloadLengthMagicHuge)
					{
						if (this->webSocket->getInputBuffer().size() < 10)
						{
							return false;
						}
						length02 = 0;
						for (int32_t value = 2, shift = 56; value < 10; ++value, shift -= 8)
						{
							uint8_t length05 = static_cast<uint8_t>(this->webSocket->getInputBuffer()[value]);
							length02 |= static_cast<uint64_t>(length05) << static_cast<uint64_t>(shift);
						}
						payloadStartOffset += 8;
					}
					if (this->webSocket->getInputBuffer().size() < payloadStartOffset + length02)
					{
						return false;
					}
					else
					{
						std::string newerVector{};
						newerVector.reserve(length02);
						for (uint32_t x = payloadStartOffset; x < payloadStartOffset + length02; x += 1)
						{
							newerVector.push_back(this->webSocket->getInputBuffer()[x]);
						}
						this->webSocket->getInputBuffer() = std::move(newerVector);
						this->onMessageReceived();
						this->webSocket->getInputBuffer().insert(this->webSocket->getInputBuffer().begin(), newVector.begin() + payloadStartOffset + length02, newVector.end());
					}
					return true;
				}
				case WebSocketOpCode::Op_Close:
				{
					uint16_t close = this->webSocket->getInputBuffer()[2] & 0xff;
					close <<= 8;
					close |= this->webSocket->getInputBuffer()[3] & 0xff;
					this->closeCode = close;
					std::cout << "Voice WebSocket Closed; Code: " << this->closeCode << std::endl;
					this->onClosedExternal();
					return false;
				}
				default:
				{
					this->closeCode = 0;
					return false;
				}
				}
			}
			return false;
		}
		catch (...)
		{
			DiscordCoreAPI::reportException("VoiceSocketAgent::parseHeader()");
			this->onClosedExternal();
			return false;
		}
	}

	void VoiceSocketAgent::onClosedExternal()
	{
		this->doWeReconnect.reset();
	}

	void VoiceSocketAgent::onClosedInternal()
	{
		this->closeCode = 1000;
		this->voiceSocket.reset(nullptr);
		this->webSocket.reset(nullptr);
		this->heartbeatTimer.cancel();
	}

	void VoiceSocketAgent::connect()
	{
		try
		{
			this->authKey = DiscordCoreAPI::generateX64BaseEncodedKey();
			DiscordCoreAPI::waitForTimeToPass(this->voiceConnectionDataBuffer, this->voiceConnectionData, 20000);
			this->baseUrl = this->voiceConnectionData.endPoint.substr(0, this->voiceConnectionData.endPoint.find(":"));
			this->relativePath = "/?v=4";
			this->heartbeatTimer.cancel();
			this->webSocket = std::make_unique<WebSocketSSLClient>(this->baseUrl, "443");
			this->state = WebSocketState::Initializing;
			std::string sendVector = "GET " + this->relativePath + " HTTP/1.1\r\nHost: " + this->baseUrl +
									 "\r\nPragma: no-cache\r\nUser-Agent: DiscordCoreAPI/1.0\r\nUpgrade: WebSocket\r\nConnection: Upgrade\r\nSec-WebSocket-Key: " +
									 this->authKey + "\r\nSec-WebSocket-Version: 13\r\n\r\n";
			this->sendMessage(sendVector);
		}
		catch (...)
		{
			DiscordCoreAPI::reportException("VoiceSocketAgent::connect()");
			this->onClosedExternal();
		}
	}

	VoiceSocketAgent::~VoiceSocketAgent()
	{
		this->theTask->cancel();
		this->theTask.reset(nullptr);
		this->heartbeatTimer.cancel();
	};
}
