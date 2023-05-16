/*
	DiscordCoreAPI, A bot library for Discord, written in C++, and featuring explicit multithreading through the usage of custom, asynchronous C++ CoRoutines.

	Copyright 2021, 2022, 2023 Chris M. (RealTimeChris)

	This library is free software; you can redistribute it and/or
	modify it under the terms of the GNU Lesser General Public
	License as published by the Free Software Foundation; either
	version 2.1 of the License, or (at your option) any later version.

	This library is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
	Lesser General Public License for more details.

	You should have received a copy of the GNU Lesser General Public
	License along with this library; if not, write to the Free Software
	Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301
	USA
*/
/// EventManager.hpp - Header for the event manager class.
/// Jun 7, 2021
/// https://discordcoreapi.com
/// \file EventManager.cpp

#include <discordcoreapi/EventManager.hpp>
#include <discordcoreapi/CoRoutine.hpp>
#include <discordcoreapi/CommandController.hpp>
#include <discordcoreapi/DiscordCoreClient.hpp>
#include <discordcoreapi/JsonSpecializations.hpp>

namespace Jsonifier {

	template<> struct Core<DiscordCoreAPI::VoiceStateData> {
		using OTy = DiscordCoreAPI::VoiceStateData;
		static constexpr auto parseValue = object("channel_id", &OTy::channelId, "guild_id", &OTy::guildId, "session_id", &OTy::sessionId,
			"self_deaf", &OTy::selfDeaf, "self_mute", &OTy::selfMute, "self_stream", &OTy::selfStream, "self_video", &OTy::selfVideo, "suppress",
			&OTy::suppress, "deaf", &OTy::deaf, "mute", &OTy::mute, "request_to_speak_timestamp", &OTy::requestToSpeakTimestamp, "user_id", &OTy::id);
	};

	template<> struct Core<DiscordCoreAPI::VoiceServerUpdateData> {
		using OTy = DiscordCoreAPI::VoiceServerUpdateData;
		static constexpr auto parseValue = object("endpoint", &OTy::endpoint, "guild_id", &OTy::guildId, "token", &OTy::token);
	};
}

namespace DiscordCoreAPI {

	EventData<GuildCacheData>::EventData(Jsonifier::JsonifierCore& parser, std::string_view data, DiscordCoreClient* client) {
		parser.parseJson<true>(*this, data);
		value.discordCoreClient = client;
		for (auto& valueNew: value.roles) {
			Roles::insertRole(std::move(valueNew));
		}
		for (auto& valueNew: value.channels) {
			Channels::insertChannel(std::move(valueNew));
		}
		for (auto& valueNew: value.voiceStates) {
			valueNew.guildId = value.id;
		}
		for (auto& valueNew: value.members) {
			valueNew.guildId = value.id;
			GuildMembers::insertGuildMember(std::move(valueNew));
		}
		if (Guilds::doWeCacheGuilds()) {
			if (Guilds::doWeCacheGuilds()) {
				Guilds::insertGuild(value);
			}
		}
		for (auto& valueNew: value.members) {
			UserData newUser{ valueNew.user };
			Users::insertUser(std::move(newUser));
		}
		for (auto& value: value.voiceStates) {
			GuildMembers::insertVoiceState(std::move(value));
		}
	};

	EventData<InteractionData>::EventData(Jsonifier::JsonifierCore& parser, std::string_view data, DiscordCoreClient* clientPtr) {
		parser.parseJson<true>(*this, data);
		if (value.user.id == 0) {
			value.user = Users::getCachedUser(GetUserData{ .userId = value.member.user.id });
		}
		std::unique_ptr<InputEventData> eventData{ std::make_unique<InputEventData>(value) };
		switch (value.type) {
			case InteractionType::Application_Command: {
				eventData->responseType = InputEventResponseType::Unset;
				*eventData->interactionData = value;
				std::unique_ptr<CommandData> commandData{ std::make_unique<CommandData>(*eventData) };
				commandData->discordCoreClient = eventData->getGuildData().discordCoreClient;
				clientPtr->getCommandController().checkForAndRunCommand(*commandData);
				break;
			}
			case InteractionType::Message_Component: {
				switch (value.data.componentType) {
					case ComponentType::Button: {
						eventData->responseType = InputEventResponseType::Unset;
						*eventData->interactionData = value;
						for (auto& value: ButtonCollector::buttonInteractionBuffersMap) {
						}
						if (ButtonCollector::buttonInteractionBuffersMap.contains(eventData->getChannelData().id + eventData->getMessageData().id)) {
							ButtonCollector::buttonInteractionBuffersMap[eventData->getChannelData().id + eventData->getMessageData().id]->send(
								value);
						}
						ButtonCollector::buttonInteractionEventsMap.operator()(*eventData->interactionData);
						break;
					}
					case ComponentType::Channel_Select:
					case ComponentType::Mentionable_Select:
					case ComponentType::Role_Select:
					case ComponentType::User_Select:
					case ComponentType::String_Select: {
						eventData->responseType = InputEventResponseType::Unset;
						*eventData->interactionData = value;
						if (SelectMenuCollector::selectMenuInteractionBuffersMap.contains(
								eventData->getChannelData().id + eventData->getMessageData().id)) {
							SelectMenuCollector::selectMenuInteractionBuffersMap[eventData->getChannelData().id + eventData->getMessageData().id]
								->send(value);
						}
						SelectMenuCollector::selectMenuInteractionEventsMap.operator()(*eventData->interactionData);
						break;
					}
				}
				break;
			}
			case InteractionType::Modal_Submit: {
				eventData->responseType = InputEventResponseType::Unset;
				*eventData->interactionData = value;
				if (ModalCollector::modalInteractionBuffersMap.contains(eventData->getChannelData().id)) {
					ModalCollector::modalInteractionBuffersMap[eventData->getChannelData().id]->send(eventData->getInteractionData());
					ModalCollector::modalInteractionEventsMap.operator()(*eventData->interactionData);
				}
				break;
			}
			case InteractionType::Application_Command_Autocomplete: {
				eventData->responseType = InputEventResponseType::Unset;
				*eventData->interactionData = value;
				//std::unique_ptr<OnAutoCompleteEntryData> autocompleteEntryData{ std::make_unique<OnAutoCompleteEntryData>(data, dataReal) };
				//autocompleteEntryData->inputEvent = *eventData;
				//clientPtr->getEventManager().onAutoCompleteEntry(*autocompleteEntryData);
				break;
			}
		}
	}

	EventData<PresenceUpdateData>::EventData(Jsonifier::JsonifierCore& parser, std::string_view data) {
		parser.parseJson<true>(*this, data);
	};

	EventData<VoiceStateData>::EventData(Jsonifier::JsonifierCore& parser, std::string_view data, DiscordCoreInternal::WebSocketClient* sslShard) {
		parser.parseJson<true>(*this, data);
		sslShard->voiceConnectionData.sessionId = value.sessionId;
		std::cout << "USER ID: " << value.id << std::endl;
		std::cout << "SHARD USER ID: " << sslShard->userId << std::endl;
		if (sslShard->areWeCollectingData && !sslShard->stateUpdateCollected && !sslShard->serverUpdateCollected &&

			value.id == sslShard->userId) {
			sslShard->voiceConnectionData = DiscordCoreInternal::VoiceConnectionData{};
			sslShard->voiceConnectionData.sessionId = value.sessionId;
			sslShard->stateUpdateCollected = true;
		} else if (sslShard->areWeCollectingData && !sslShard->stateUpdateCollected) {
			sslShard->voiceConnectionData.sessionId = value.sessionId;
			if (sslShard->voiceConnectionDataBufferMap.contains(value.guildId.operator uint64_t())) {
				sslShard->voiceConnectionDataBufferMap[value.guildId.operator uint64_t()]->send(sslShard->voiceConnectionData);
				sslShard->serverUpdateCollected = false;
				sslShard->stateUpdateCollected = false;
				sslShard->areWeCollectingData = false;
			}
		}
		GuildMembers::insertVoiceState(value);
	};

	EventData<VoiceServerUpdateData>::EventData(Jsonifier::JsonifierCore& parser, std::string_view data,
		DiscordCoreInternal::WebSocketClient* sslShard) {
		parser.parseJson<true>(*this, data);
		if (sslShard->areWeCollectingData && !sslShard->serverUpdateCollected && !sslShard->stateUpdateCollected) {
			sslShard->voiceConnectionData = DiscordCoreInternal::VoiceConnectionData{};
			sslShard->voiceConnectionData.endPoint = value.endpoint;
			sslShard->voiceConnectionData.token = value.token;
			sslShard->serverUpdateCollected = true;
		} else if (sslShard->areWeCollectingData && !sslShard->serverUpdateCollected) {
			sslShard->voiceConnectionData.endPoint = value.endpoint;
			sslShard->voiceConnectionData.token = value.token;
			if (sslShard->voiceConnectionDataBufferMap.contains(value.guildId.operator uint64_t())) {
				sslShard->voiceConnectionDataBufferMap[value.guildId.operator uint64_t()]->send(sslShard->voiceConnectionData);
				sslShard->serverUpdateCollected = false;
				sslShard->stateUpdateCollected = false;
				sslShard->areWeCollectingData = false;
			}
		}
	};

	/*
	OnInputEventCreationData::OnInputEventCreationData(Jsonifier::JsonifierCore& dataReal, std::string_view dataToParse) {
		dataReal.parseJson<true>(inputEventData , dataToParse);
	}
	/*
	OnApplicationCommandPermissionsUpdateData::OnApplicationCommandPermissionsUpdateData(DiscordCoreInternal::WebSocketMessage& message,
		Jsonifier::JsonifierCore& data) {
		permissionData = message.processJsonMessage<DiscordCoreAPI::GuildApplicationCommandPermissionsData>(data);
	}

	OnAutoModerationRuleCreationData::OnAutoModerationRuleCreationData(DiscordCoreInternal::WebSocketMessage& message,
		Jsonifier::JsonifierCore& data) {
		theRule = message.processJsonMessage<AutoModerationRule>(data);
	}

	OnAutoModerationRuleUpdateData::OnAutoModerationRuleUpdateData(Jsonifier::JsonifierCore& dataReal, std::string_view dataToParse) {
		theRule = message.processJsonMessage<AutoModerationRule>(data);
	}

	OnAutoModerationRuleDeletionData::OnAutoModerationRuleDeletionData(DiscordCoreInternal::WebSocketMessage& message,
		Jsonifier::JsonifierCore& data) {
		theRule = message.processJsonMessage<AutoModerationRule>(data);
	}

	OnAutoModerationActionExecutionData::OnAutoModerationActionExecutionData(DiscordCoreInternal::WebSocketMessage& message,
		Jsonifier::JsonifierCore& data) {
		data = message.processJsonMessage<AutoModerationActionExecutionEventData>(data);
	}

	OnChannelCreationData::OnChannelCreationData(Jsonifier::JsonifierCore& dataReal, std::string_view dataToParse) {
		channel = std::make_unique<ChannelData>(data.processJsonMessage<ChannelData>(dataReal));
		if (Channels::doWeCacheChannels()) {
			GuildData value{};
			value.id = channel->guildId;
			if (Guilds::getCache().contains(value)) {
				Guilds::getCache()[value].channels.emplace_back(channel->id);
			}
			if (Channels::doWeCacheChannels()) {
				Channels::insertChannel(*channel);
			}
		}
	}

	OnChannelCreationData& OnChannelCreationData::operator=(const OnChannelCreationData& other) {
		*channel = *other.channel;
		return *this;
	}

	OnChannelCreationData::OnChannelCreationData(const OnChannelCreationData& other) {
		*this = other;
	}
	
	OnChannelUpdateData::OnChannelUpdateData(Jsonifier::JsonifierCore& dataReal, std::string_view dataToParse) {
		channel = std::make_unique<ChannelData>(data.processJsonMessage<ChannelData>(dataReal));
		if (Channels::doWeCacheChannels()) {
			Channels::insertChannel(*channel);
		}
	}

	OnChannelUpdateData& OnChannelUpdateData::operator=(const OnChannelUpdateData& other) {
		*channel = *other.channel;
		return *this;
	}

	OnChannelUpdateData::OnChannelUpdateData(const OnChannelUpdateData& other) {
		*this = other;
	}

	OnChannelDeletionData::OnChannelDeletionData(Jsonifier::JsonifierCore& dataReal, std::string_view dataToParse) {
		channel = std::make_unique<ChannelData>(data.processJsonMessage<ChannelData>(dataReal));
		if (Channels::doWeCacheChannels()) {
			GuildData value{};
			value.id = channel->guildId;
			if (Guilds::getCache().contains(value)) {
				for (uint64_t x = 0; x < Guilds::getCache()[value].channels.size(); ++x) {
					if (Guilds::getCache()[value].channels[x] == channel->id) {
						Guilds::getCache()[value].channels.erase(Guilds::getCache()[value].channels.begin() + x);
					}
				}
			}
			if (Channels::doWeCacheChannels()) {
				Channels::removeChannel(channel->id);
			}
		}
	}

	OnChannelDeletionData& OnChannelDeletionData::operator=(const OnChannelDeletionData& other) {
		*channel = *other.channel;
		return *this;
	}

	OnChannelDeletionData::OnChannelDeletionData(const OnChannelDeletionData& other) {
		*this = other;
	}

	OnChannelPinsUpdateData::OnChannelPinsUpdateData(Jsonifier::JsonifierCore& dataReal, std::string_view dataToParse) {
		dataPackage = data.processJsonMessage<ChannelPinsUpdateEventData>(dataReal);
	}

	OnThreadCreationData::OnThreadCreationData(Jsonifier::JsonifierCore& dataReal, std::string_view dataToParse) {
		value = data.processJsonMessage<Thread>(dataReal);
		GuildData value{};
		value.id = value.guildId;
		if (Guilds::getCache().contains(value)) {
			Guilds::getCache()[value].threads.emplace_back(value.id);
		}
	}

	OnThreadUpdateData::OnThreadUpdateData(Jsonifier::JsonifierCore& dataReal, std::string_view dataToParse) {
		value = data.processJsonMessage<Thread>(dataReal);
	}
	
	OnThreadDeletionData::OnThreadDeletionData(Jsonifier::JsonifierCore& dataReal,   std::string_view dataToParse) {
		dataReal.parseJson<true>(value, dataToParse);
		GuildData value{};
		value.id = value.guildId;
		if (Guilds::getCache().contains(value)) {
			for (uint64_t x = 0; x < Guilds::getCache()[value].threads.size(); ++x) {
				if (Guilds::getCache()[value].threads[x] == value.id) {
					Guilds::getCache()[value].threads.erase(Guilds::getCache()[value].threads.begin() + x);
				}
			}
		}
	}

	OnThreadListSyncData::OnThreadListSyncData(Jsonifier::JsonifierCore& dataReal, std::string_view dataToParse) {
		dataReal.parseJson<true>(value, dataToParse);
	}

	OnThreadMemberUpdateData::OnThreadMemberUpdateData(Jsonifier::JsonifierCore& dataReal,  std::string_view dataToParse) {
		dataReal.parseJson<true>(value, dataToParse);
	}

	OnThreadMembersUpdateData::OnThreadMembersUpdateData(Jsonifier::JsonifierCore& dataReal,   std::string_view dataToParse) {
		dataReal.parseJson<true>(value, dataToParse);
	}

	OnGuildCreationData::OnGuildCreationData(Jsonifier::JsonifierCore& dataReal, std::string_view stringData, DiscordCoreClient*client) {
		value = std::make_unique<GuildData>();
		dataReal.parseJson<true>(*this, stringData);
		value->discordCoreClient = client;
		for (auto& value: value->roles) {
			std::cout << "ROLE ID: " << value.operator std::string() << std::endl;
		}
		for (auto& value: value->channels) {
			std::cout << "CHANNEL ID: " << value.operator std::string() << std::endl;
		}
		for (auto& value: value->members) {
			std::cout << "GUILD MEMBER ID: " << value.userIdBase.id.operator std::string() << std::endl;
		}
		std::cout << "GUILD ID: " << value->id << std::endl;
		if (Guilds::doWeCacheGuilds()) {
			if (Guilds::doWeCacheGuilds()) {
				Guilds::insertGuild(*value);
			}
		}
	}

	OnGuildCreationData& OnGuildCreationData::operator=(const OnGuildCreationData& other) {
		*value = *other.value;
		return *this;
	}

	OnGuildCreationData::OnGuildCreationData(const OnGuildCreationData& other) {
		*this = other;
	}

	OnGuildUpdateData::OnGuildUpdateData(Jsonifier::JsonifierCore& dataReal, std::string_view stringData, DiscordCoreClient* client) {
		value = std::make_unique<GuildData>();
		dataReal.parseJson<true>(*value, stringData);
		value->discordCoreClient = client;
		if (Guilds::doWeCacheGuilds()) {
			Guilds::insertGuild(*value);
		}
	}

	OnGuildUpdateData& OnGuildUpdateData::operator=(const OnGuildUpdateData& other) {
		*value = *other.value;
		return *this;
	}

	OnGuildUpdateData::OnGuildUpdateData(const OnGuildUpdateData& other) {
		*this = other;
	}

	OnGuildDeletionData::OnGuildDeletionData(Jsonifier::JsonifierCore& dataReal, std::string_view stringData, DiscordCoreClient* client) {
		value = std::make_unique<GuildData>();
		dataReal.parseJson<true>(*value, stringData);
		value->discordCoreClient = client;
		if (Guilds::doWeCacheGuilds()) {
			Guilds::removeGuild(value->id);
		}
		for (auto& value: value->members) {
			GuildMemberData guildMember =
				GuildMembers::getCachedGuildMember({ .guildMemberId = value.userIdBase.id.operator std::string(), .guildId = value->id });
			GuildMembers::removeGuildMember(guildMember);
		}
		for (auto& value: value->channels) {
			Channels::removeChannel(value.operator DiscordCoreAPI::Snowflake &());
		}
		for (auto& value: value->roles) {
			Roles::removeRole(value.operator DiscordCoreAPI::Snowflake &());
		}
	}

	OnGuildDeletionData& OnGuildDeletionData::operator=(const OnGuildDeletionData& other) {
		*value = *other.value;
		return *this;
	}

	OnGuildDeletionData::OnGuildDeletionData(const OnGuildDeletionData& other) {
		*this = other;
	}
	/*
	OnGuildBanAddData::OnGuildBanAddData(Jsonifier::JsonifierCore& dataReal, std::string_view dataToParse) {
		*this = data.processJsonMessage<OnGuildBanAddData>(dataReal);
	}

	OnGuildBanRemoveData::OnGuildBanRemoveData(Jsonifier::JsonifierCore& dataReal, std::string_view dataToParse) {
		*this = data.processJsonMessage<OnGuildBanRemoveData>(dataReal);
	}

	OnGuildEmojisUpdateData::OnGuildEmojisUpdateData(Jsonifier::JsonifierCore& dataReal, std::string_view dataToParse) {
		updateData = data.processJsonMessage<GuildEmojisUpdateEventData>(dataReal);
		GuildData value{};
		value.id = updateData.guildId;
		if (Guilds::getCache().contains(value)) {
			Guilds::getCache()[value].emoji.clear();
			for (auto& value: updateData.emojis) {
				Guilds::getCache()[value].emoji.emplace_back(value.id);
			}
		}
	}

	OnGuildStickersUpdateData::OnGuildStickersUpdateData(Jsonifier::JsonifierCore& dataReal, std::string_view dataToParse) {
		updateData = data.processJsonMessage<GuildStickersUpdateEventData>(dataReal);
		GuildData value{};
		value.id = updateData.guildId;
		if (Guilds::getCache().contains(value)) {
			Guilds::getCache()[value].stickers.clear();
			for (auto& value: updateData.stickers) {
				Guilds::getCache()[value].stickers.emplace_back(value.id);
			}
		}
	}

	OnGuildIntegrationsUpdateData::OnGuildIntegrationsUpdateData(Jsonifier::JsonifierCore& dataReal, std::string_view dataToParse) {
		*this = data.processJsonMessage<OnGuildIntegrationsUpdateData>(dataReal);
		
	}

	OnGuildMemberAddData::OnGuildMemberAddData(Jsonifier::JsonifierCore& dataReal, std::string_view dataToParse,
		DiscordCoreClient* client) {
		guildMember = std::make_unique<GuildMemberData>(data.processJsonMessage<GuildMemberData>(dataReal));
		discordCoreClient = client;
		if (GuildMembers::doWeCacheGuildMembers()) {
			GuildMembers::insertGuildMember(*guildMember);
			GuildData value{};
			value.id = guildMember->guildId;
			if (Guilds::getCache().contains(value)) {
				++Guilds::getCache()[value].memberCount;
			}
		}
	}

	OnGuildMemberAddData& OnGuildMemberAddData::operator=(const OnGuildMemberAddData& other) {
		discordCoreClient = other.discordCoreClient;
		*guildMember = *other.guildMember;
		return *this;
	}

	OnGuildMemberAddData::OnGuildMemberAddData(const OnGuildMemberAddData& other) {
		*this = other;
	}

	OnGuildMemberRemoveData::OnGuildMemberRemoveData(Jsonifier::JsonifierCore& dataReal, std::string_view dataToParse,
		DiscordCoreClient* client) {
		*this = data.processJsonMessage<OnGuildMemberRemoveData>(dataReal);
		GuildMember guildMember = GuildMembers::getCachedGuildMember({ .guildMemberId = user->id, .guildId = guildId });
		if (GuildMembers::doWeCacheGuildMembers()) {
			GuildData value{};
			value.id = guildId;
			GuildMembers::removeGuildMember(guildMember);
			if (Guilds::getCache().contains(value)) {
				for (uint64_t x = 0; x < Guilds::getCache()[value].members.size(); ++x) {
					if (Guilds::getCache()[value].members[x] == user->id) {
						--Guilds::getCache()[value].memberCount;
						Guilds::getCache()[value].members.erase(Guilds::getCache()[value].members.begin() + x);
					}
				}
			}
		}
	}

	OnGuildMemberRemoveData& OnGuildMemberRemoveData::operator=(const OnGuildMemberRemoveData& other) {
		discordCoreClient = other.discordCoreClient;
		guildId = other.guildId;
		*user = *other.user;
		return *this;
	}

	OnGuildMemberRemoveData::OnGuildMemberRemoveData(const OnGuildMemberRemoveData& other) {
		*this = other;
	}

	OnGuildMemberUpdateData::OnGuildMemberUpdateData(Jsonifier::JsonifierCore& dataReal, std::string_view dataToParse,
		DiscordCoreClient* client) {
		guildMember = std::make_unique<GuildMemberData>(data.processJsonMessage<GuildMemberData>(dataReal));
		if (GuildMembers::doWeCacheGuildMembers()) {
			GuildMembers::insertGuildMember(*guildMember);
		}
	}

	OnGuildMemberUpdateData& OnGuildMemberUpdateData::operator=(const OnGuildMemberUpdateData& other) {
		*guildMember = *other.guildMember;
		return *this;
	}

	OnGuildMemberUpdateData::OnGuildMemberUpdateData(const OnGuildMemberUpdateData& other) {
		*this = other;
	}

	OnGuildMembersChunkData::OnGuildMembersChunkData(Jsonifier::JsonifierCore& dataReal, std::string_view dataToParse) {
		chunkEventData = data.processJsonMessage<GuildMembersChunkEventData>(dataReal);
	}

	OnRoleCreationData::OnRoleCreationData(Jsonifier::JsonifierCore& dataReal, std::string_view dataToParse) {
		*this = data.processJsonMessage<OnRoleCreationData>(dataReal);
		GuildData value{};
		value.id = guildId;
		if (Guilds::getCache().contains(value)) {
			Guilds::getCache()[value].roles.emplace_back(role->id);
		}
		if (Roles::doWeCacheRoles()) {
			Roles::insertRole(*role);
		}
	}

	OnRoleCreationData& OnRoleCreationData::operator=(const OnRoleCreationData& other) {
		guildId = other.guildId;
		*role = *other.role;
		return *this;
	}

	OnRoleCreationData::OnRoleCreationData(const OnRoleCreationData& other) {
		*this = other;
	}

	OnRoleUpdateData::OnRoleUpdateData(Jsonifier::JsonifierCore& dataReal, std::string_view dataToParse) {
		*this = data.processJsonMessage<OnRoleUpdateData>(dataReal);
		if (Roles::doWeCacheRoles()) {
			Roles::insertRole(*role);
		}
	}

	OnRoleUpdateData& OnRoleUpdateData::operator=(const OnRoleUpdateData& other) {
		guildId = other.guildId;
		*role = *other.role;
		return *this;
	}

	OnRoleUpdateData::OnRoleUpdateData(const OnRoleUpdateData& other) {
		*this = other;
	}

	OnRoleDeletionData::OnRoleDeletionData(Jsonifier::JsonifierCore& dataReal, std::string_view dataToParse) {
		*this = data.processJsonMessage<OnRoleDeletionData>(dataReal);
		GuildData value{};
		value.id = guildId;
		if (Roles::doWeCacheRoles()) {
			Roles::removeRole(role->id);
			if (Guilds::getCache().contains(value)) {
				for (uint64_t x = 0; x < Guilds::getCache()[value].roles.size(); ++x) {
					if (Guilds::getCache()[value].roles[x] == role->id) {
						Guilds::getCache()[value].roles.erase(Guilds::getCache()[value].roles.begin() + x);
					}
				}
			}
		}
	}

	OnRoleDeletionData& OnRoleDeletionData::operator=(const OnRoleDeletionData& other) {
		guildId = other.guildId;
		*role = *other.role;
		return *this;
	}

	OnRoleDeletionData::OnRoleDeletionData(const OnRoleDeletionData& other) {
		*this = other;
	}

	OnUserUpdateData& OnUserUpdateData::operator=(const OnUserUpdateData& other) {
		*user = *other.user;
		return *this;
	}

	OnUserUpdateData::OnUserUpdateData(const OnUserUpdateData& other) {
		*this = other;
	}

	OnGuildScheduledEventCreationData::OnGuildScheduledEventCreationData(DiscordCoreInternal::WebSocketMessage& data,
		Jsonifier::JsonifierCore& dataReal) {
		guildScheduledEvent = data.processJsonMessage<GuildScheduledEventData>(dataReal);
		GuildData value{};
		value.id = guildScheduledEvent.guildId;
		if (Guilds::getCache().contains(value)) {
			Guilds::getCache()[value].guildScheduledEvents.emplace_back(guildScheduledEvent.id);
		}
	}

	OnGuildScheduledEventUpdateData::OnGuildScheduledEventUpdateData(DiscordCoreInternal::WebSocketMessage& data,
		Jsonifier::JsonifierCore& dataReal) {
		guildScheduledEvent = data.processJsonMessage<GuildScheduledEventData>(dataReal);
	}

	OnGuildScheduledEventDeletionData::OnGuildScheduledEventDeletionData(DiscordCoreInternal::WebSocketMessage& data,
		Jsonifier::JsonifierCore& dataReal) {
		guildScheduledEvent = data.processJsonMessage<GuildScheduledEventData>(dataReal);
		GuildData value{};
		value.id = guildScheduledEvent.guildId;
		if (Guilds::getCache().contains(value)) {
			for (uint64_t x = 0; x < Guilds::getCache()[value].guildScheduledEvents.size(); ++x) {
				if (Guilds::getCache()[value].guildScheduledEvents[x] == guildScheduledEvent.id) {
					Guilds::getCache()[value].guildScheduledEvents.erase(Guilds::getCache()[value].guildScheduledEvents.begin() + x);
				}
			}
		}
	}

	OnGuildScheduledEventUserAddData::OnGuildScheduledEventUserAddData(DiscordCoreInternal::WebSocketMessage& data,
		Jsonifier::JsonifierCore& dataReal) {
		*this = data.processJsonMessage<OnGuildScheduledEventUserAddData>(dataReal);
	}

	OnGuildScheduledEventUserRemoveData::OnGuildScheduledEventUserRemoveData(DiscordCoreInternal::WebSocketMessage& data,
		Jsonifier::JsonifierCore& dataReal) {
		*this = data.processJsonMessage<OnGuildScheduledEventUserRemoveData>(dataReal);
	}

	OnIntegrationCreationData::OnIntegrationCreationData(Jsonifier::JsonifierCore& dataReal, std::string_view dataToParse) {
		*this = data.processJsonMessage<OnIntegrationCreationData>(dataReal);
	}

	OnIntegrationUpdateData::OnIntegrationUpdateData(Jsonifier::JsonifierCore& dataReal, std::string_view dataToParse) {
		*this = data.processJsonMessage<OnIntegrationUpdateData>(dataReal);
	}

	OnIntegrationDeletionData::OnIntegrationDeletionData(Jsonifier::JsonifierCore& dataReal, std::string_view dataToParse) {
		*this = data.processJsonMessage<OnIntegrationDeletionData>(dataReal);
	}
	
	OnInviteCreationData::OnInviteCreationData(Jsonifier::JsonifierCore& dataReal, std::string_view dataToParse) {
		invite = data.processJsonMessage<InviteData>(dataReal);
	}

	OnInviteDeletionData::OnInviteDeletionData(Jsonifier::JsonifierCore& dataReal, std::string_view dataToParse) {
		*this = data.processJsonMessage<OnInviteDeletionData>(dataReal);
	}

	OnMessageCreationData::OnMessageCreationData(Jsonifier::JsonifierCore& dataReal, std::string_view dataToParse) {
		message = data.processJsonMessage<Message>(dataReal);
		for (auto& [key, dValue]: MessageCollector::objectsBuffersMap) {
			dValue->send(message);
		}
	}

	OnMessageUpdateData::OnMessageUpdateData(Jsonifier::JsonifierCore& dataReal, std::string_view dataToParse) {
		messageNew = data.processJsonMessage<Message>(dataReal);
		for (auto& [key, dValue]: MessageCollector::objectsBuffersMap) {
			dValue->send(messageNew);
		}
	}

	OnMessageDeletionData::OnMessageDeletionData(Jsonifier::JsonifierCore& dataReal, std::string_view dataToParse) {
		*this = data.processJsonMessage<OnMessageDeletionData>(dataReal);
	}

	OnMessageDeleteBulkData::OnMessageDeleteBulkData(Jsonifier::JsonifierCore& dataReal, std::string_view dataToParse) {
		*this = data.processJsonMessage<OnMessageDeleteBulkData>(dataReal);
	}

	OnReactionAddData::OnReactionAddData(Jsonifier::JsonifierCore& dataReal, std::string_view dataToParse) {
		reaction = data.processJsonMessage<Reaction>(dataReal);
	}

	OnReactionRemoveData::OnReactionRemoveData(Jsonifier::JsonifierCore& dataReal, std::string_view dataToParse) {
		reactionRemoveData = data.processJsonMessage<ReactionRemoveData>(dataReal);
	}

	OnReactionRemoveAllData::OnReactionRemoveAllData(Jsonifier::JsonifierCore& dataReal, std::string_view dataToParse) {
		*this = data.processJsonMessage<OnReactionRemoveAllData>(dataReal);
	}

	OnReactionRemoveEmojiData::OnReactionRemoveEmojiData(Jsonifier::JsonifierCore& dataReal, std::string_view dataToParse) {
		*this = data.processJsonMessage<OnReactionRemoveEmojiData>(dataReal);
	}
	
	OnPresenceUpdateData::OnPresenceUpdateData(Jsonifier::JsonifierCore& dataReal, std::string_view dataToParse) {
		dataReal.parseJson<true>(*this, dataToParse);
	}
	/*
	OnStageInstanceCreationData::OnStageInstanceCreationData(Jsonifier::JsonifierCore& dataReal, std::string_view dataToParse) {
		stageInstance = data.processJsonMessage<StageInstance>(dataReal);
		GuildData value{};
		value.id = stageInstance.guildId;
		if (Guilds::getCache().contains(value)) {
			Guilds::getCache()[value].stageInstances.emplace_back(stageInstance.id);
		}
	}

	OnStageInstanceUpdateData::OnStageInstanceUpdateData(Jsonifier::JsonifierCore& dataReal, std::string_view dataToParse) {
		stageInstance = data.processJsonMessage<StageInstance>(dataReal);
	}

	OnStageInstanceDeletionData::OnStageInstanceDeletionData(Jsonifier::JsonifierCore& dataReal, std::string_view dataToParse) {
		stageInstance = data.processJsonMessage<StageInstance>(dataReal);
		GuildData value{};
		value.id = stageInstance.guildId;
		if (Guilds::getCache().contains(value)) {
			for (uint64_t x = 0; x < Guilds::getCache()[value].stageInstances.size(); ++x) {
				if (Guilds::getCache()[value].stageInstances[x] == stageInstance.id) {
					Guilds::getCache()[value].stageInstances.erase(Guilds::getCache()[value].stageInstances.begin() + x);
				}
			}
		}
	}

	OnTypingStartData::OnTypingStartData(Jsonifier::JsonifierCore& dataReal, std::string_view dataToParse) {
		typingStartData = data.processJsonMessage<TypingStartData>(dataReal);
	}

	OnUserUpdateData::OnUserUpdateData(Jsonifier::JsonifierCore& dataReal, std::string_view dataToParse) {
		user = std::make_unique<UserData>(data.processJsonMessage<UserData>(dataReal));
		if (Users::doWeCacheUsers()) {
			Users::insertUser(*user);
		}
	}

	OnWebhookUpdateData::OnWebhookUpdateData(Jsonifier::JsonifierCore& dataReal, std::string_view dataToParse) {
		*this = data.processJsonMessage<OnWebhookUpdateData>(dataReal);
	}
	
	OnAutoCompleteEntryData::OnAutoCompleteEntryData(Jsonifier::JsonifierCore& dataReal, std::string_view dataToParse) {
		dataReal.parseJson<true>(inputEvent, dataToParse);
	}
	/*
	DiscordCoreInternal::EventDelegateToken EventManager::onApplicationCommandsPermissionsUpdate(
		DiscordCoreInternal::EventDelegate<CoRoutine<void>, OnApplicationCommandPermissionsUpdateData> handler) {
		return onApplicationCommandPermissionsUpdateEvent.add(std::move(handler));
	}

	void EventManager::onApplicationCommandsPermissionsUpdate(DiscordCoreInternal::EventDelegateToken& token) {
		return onApplicationCommandPermissionsUpdateEvent.erase(token);
	}

	DiscordCoreInternal::EventDelegateToken EventManager::onAutoModerationRuleCreation(
		DiscordCoreInternal::EventDelegate<CoRoutine<void>, OnAutoModerationRuleCreationData> handler) {
		return onAutoModerationRuleCreationEvent.add(std::move(handler));
	}

	void EventManager::onAutoModerationRuleCreation(DiscordCoreInternal::EventDelegateToken& token) {
		return onAutoModerationRuleCreationEvent.erase(token);
	}

	DiscordCoreInternal::EventDelegateToken EventManager::onAutoModerationRuleUpdate(
		DiscordCoreInternal::EventDelegate<CoRoutine<void>, OnAutoModerationRuleUpdateData> handler) {
		return onAutoModerationRuleUpdateEvent.add(std::move(handler));
	}

	void EventManager::onAutoModerationRuleUpdate(DiscordCoreInternal::EventDelegateToken& token) {
		return onAutoModerationRuleUpdateEvent.erase(token);
	}

	DiscordCoreInternal::EventDelegateToken EventManager::onAutoModerationRuleDeletion(
		DiscordCoreInternal::EventDelegate<CoRoutine<void>, OnAutoModerationRuleDeletionData> handler) {
		return onAutoModerationRuleDeletionEvent.add(std::move(handler));
	}

	void EventManager::onAutoModerationRuleDeletion(DiscordCoreInternal::EventDelegateToken& token) {
		return onAutoModerationRuleDeletionEvent.erase(token);
	}

	DiscordCoreInternal::EventDelegateToken EventManager::onAutoModerationActionExecution(
		DiscordCoreInternal::EventDelegate<CoRoutine<void>, OnAutoModerationActionExecutionData> handler) {
		return onAutoModerationActionExecutionEvent.add(std::move(handler));
	}

	void EventManager::onAutoModerationActionExecution(DiscordCoreInternal::EventDelegateToken& token) {
		return onAutoModerationActionExecutionEvent.erase(token);
	}

	DiscordCoreInternal::EventDelegateToken EventManager::onAutoCompleteEntry(
		DiscordCoreInternal::EventDelegate<CoRoutine<void>, OnAutoCompleteEntryData> handler) {
		return onAutoCompleteEntryEvent.add(std::move(handler));
	}

	void EventManager::onAutoCompleteEntry(DiscordCoreInternal::EventDelegateToken& token) {
		return onInputEventCreationEvent.erase(token);
	}

	DiscordCoreInternal::EventDelegateToken EventManager::onInputEventCreation(
		DiscordCoreInternal::EventDelegate<CoRoutine<void>, OnInputEventCreationData> handler) {
		return onInputEventCreationEvent.add(std::move(handler));
	}

	void EventManager::onInputEventCreation(DiscordCoreInternal::EventDelegateToken& token) {
		return onInputEventCreationEvent.erase(token);
	}

	DiscordCoreInternal::EventDelegateToken EventManager::onChannelCreation(
		DiscordCoreInternal::EventDelegate<CoRoutine<void>, OnChannelCreationData> handler) {
		return onChannelCreationEvent.add(std::move(handler));
	}

	void EventManager::onChannelCreation(DiscordCoreInternal::EventDelegateToken& token) {
		onChannelCreationEvent.erase(token);
	}

	DiscordCoreInternal::EventDelegateToken EventManager::onChannelUpdate(
		DiscordCoreInternal::EventDelegate<CoRoutine<void>, OnChannelUpdateData> handler) {
		return onChannelUpdateEvent.add(std::move(handler));
	}

	void EventManager::onChannelUpdate(DiscordCoreInternal::EventDelegateToken& token) {
		onChannelUpdateEvent.erase(token);
	}

	DiscordCoreInternal::EventDelegateToken EventManager::onChannelDeletion(
		DiscordCoreInternal::EventDelegate<CoRoutine<void>, OnChannelDeletionData> handler) {
		return onChannelDeletionEvent.add(std::move(handler));
	}

	void EventManager::onChannelDeletion(DiscordCoreInternal::EventDelegateToken& token) {
		onChannelDeletionEvent.erase(token);
	}

	DiscordCoreInternal::EventDelegateToken EventManager::onChannelPinsUpdate(
		DiscordCoreInternal::EventDelegate<CoRoutine<void>, OnChannelPinsUpdateData> handler) {
		return onChannelPinsUpdateEvent.add(std::move(handler));
	}

	void EventManager::onChannelPinsUpdate(DiscordCoreInternal::EventDelegateToken& token) {
		return onChannelPinsUpdateEvent.erase(token);
	}

	DiscordCoreInternal::EventDelegateToken EventManager::onThreadCreation(
		DiscordCoreInternal::EventDelegate<CoRoutine<void>, OnThreadCreationData> handler) {
		return onThreadCreationEvent.add(std::move(handler));
	}

	void EventManager::onThreadCreation(DiscordCoreInternal::EventDelegateToken& token) {
		return onThreadCreationEvent.erase(token);
	}

	DiscordCoreInternal::EventDelegateToken EventManager::onThreadUpdate(
		DiscordCoreInternal::EventDelegate<CoRoutine<void>, OnThreadUpdateData> handler) {
		return onThreadUpdateEvent.add(std::move(handler));
	}

	void EventManager::onThreadUpdate(DiscordCoreInternal::EventDelegateToken& token) {
		return onThreadUpdateEvent.erase(token);
	}

	DiscordCoreInternal::EventDelegateToken EventManager::onThreadDeletion(
		DiscordCoreInternal::EventDelegate<CoRoutine<void>, OnThreadDeletionData> handler) {
		return onThreadDeletionEvent.add(std::move(handler));
	}

	void EventManager::onThreadDeletion(DiscordCoreInternal::EventDelegateToken& token) {
		return onThreadDeletionEvent.erase(token);
	}

	DiscordCoreInternal::EventDelegateToken EventManager::onThreadListSync(
		DiscordCoreInternal::EventDelegate<CoRoutine<void>, OnThreadListSyncData> handler) {
		return onThreadListSyncEvent.add(std::move(handler));
	}

	void EventManager::onThreadListSync(DiscordCoreInternal::EventDelegateToken& token) {
		return onThreadListSyncEvent.erase(token);
	}

	DiscordCoreInternal::EventDelegateToken EventManager::onThreadMemberUpdate(
		DiscordCoreInternal::EventDelegate<CoRoutine<void>, OnThreadMemberUpdateData> handler) {
		return onThreadMemberUpdateEvent.add(std::move(handler));
	}

	void EventManager::onThreadMemberUpdate(DiscordCoreInternal::EventDelegateToken& token) {
		return onThreadMemberUpdateEvent.erase(token);
	}

	DiscordCoreInternal::EventDelegateToken EventManager::onThreadMembersUpdate(
		DiscordCoreInternal::EventDelegate<CoRoutine<void>, OnThreadMembersUpdateData> handler) {
		return onThreadMembersUpdateEvent.add(std::move(handler));
	}

	void EventManager::onThreadMembersUpdate(DiscordCoreInternal::EventDelegateToken& token) {
		return onThreadMembersUpdateEvent.erase(token);
	}

	DiscordCoreInternal::EventDelegateToken EventManager::onGuildCreation(
		DiscordCoreInternal::EventDelegate<CoRoutine<void>, OnGuildCreationData> handler) {
		return onGuildCreationEvent.add(std::move(handler));
	}

	void EventManager::onGuildCreation(DiscordCoreInternal::EventDelegateToken& token) {
		onGuildCreationEvent.erase(token);
	}

	DiscordCoreInternal::EventDelegateToken EventManager::onGuildUpdate(
		DiscordCoreInternal::EventDelegate<CoRoutine<void>, OnGuildUpdateData> handler) {
		return onGuildUpdateEvent.add(std::move(handler));
	}

	void EventManager::onGuildUpdate(DiscordCoreInternal::EventDelegateToken& token) {
		onGuildUpdateEvent.erase(token);
	}

	DiscordCoreInternal::EventDelegateToken EventManager::onGuildDeletion(
		DiscordCoreInternal::EventDelegate<CoRoutine<void>, OnGuildDeletionData> handler) {
		return onGuildDeletionEvent.add(std::move(handler));
	}

	void EventManager::onGuildDeletion(DiscordCoreInternal::EventDelegateToken& token) {
		onGuildDeletionEvent.erase(token);
	}

	DiscordCoreInternal::EventDelegateToken EventManager::onGuildBanAdd(
		DiscordCoreInternal::EventDelegate<CoRoutine<void>, OnGuildBanAddData> handler) {
		return onGuildBanAddEvent.add(std::move(handler));
	}

	void EventManager::onGuildBanAdd(DiscordCoreInternal::EventDelegateToken& token) {
		onGuildBanAddEvent.erase(token);
	}

	DiscordCoreInternal::EventDelegateToken EventManager::onGuildBanRemove(
		DiscordCoreInternal::EventDelegate<CoRoutine<void>, OnGuildBanRemoveData> handler) {
		return onGuildBanRemoveEvent.add(std::move(handler));
	}

	void EventManager::onGuildBanRemove(DiscordCoreInternal::EventDelegateToken& token) {
		onGuildBanRemoveEvent.erase(token);
	}

	DiscordCoreInternal::EventDelegateToken EventManager::onGuildEmojisUpdate(
		DiscordCoreInternal::EventDelegate<CoRoutine<void>, OnGuildEmojisUpdateData> handler) {
		return onGuildEmojisUpdateEvent.add(std::move(handler));
	}

	void EventManager::onGuildEmojisUpdate(DiscordCoreInternal::EventDelegateToken& token) {
		onGuildEmojisUpdateEvent.erase(token);
	}

	DiscordCoreInternal::EventDelegateToken EventManager::onGuildStickersUpdate(
		DiscordCoreInternal::EventDelegate<CoRoutine<void>, OnGuildStickersUpdateData> handler) {
		return onGuildStickersUpdateEvent.add(std::move(handler));
	}

	void EventManager::onGuildStickersUpdate(DiscordCoreInternal::EventDelegateToken& token) {
		onGuildStickersUpdateEvent.erase(token);
	}

	DiscordCoreInternal::EventDelegateToken EventManager::onGuildIntegrationsUpdate(
		DiscordCoreInternal::EventDelegate<CoRoutine<void>, OnGuildIntegrationsUpdateData> handler) {
		return onGuildIntegrationsUpdateEvent.add(std::move(handler));
	}

	void EventManager::onGuildIntegrationsUpdate(DiscordCoreInternal::EventDelegateToken& token) {
		onGuildIntegrationsUpdateEvent.erase(token);
	}

	DiscordCoreInternal::EventDelegateToken EventManager::onGuildMembersChunk(
		DiscordCoreInternal::EventDelegate<CoRoutine<void>, OnGuildMembersChunkData> handler) {
		return onGuildMembersChunkEvent.add(std::move(handler));
	}

	void EventManager::onGuildMembersChunk(DiscordCoreInternal::EventDelegateToken& token) {
		onGuildMembersChunkEvent.erase(token);
	}

	DiscordCoreInternal::EventDelegateToken EventManager::onGuildMemberAdd(
		DiscordCoreInternal::EventDelegate<CoRoutine<void>, OnGuildMemberAddData> handler) {
		return onGuildMemberAddEvent.add(std::move(handler));
	}

	void EventManager::onGuildMemberAdd(DiscordCoreInternal::EventDelegateToken& token) {
		onGuildMemberAddEvent.erase(token);
	}

	DiscordCoreInternal::EventDelegateToken EventManager::onGuildMemberRemove(
		DiscordCoreInternal::EventDelegate<CoRoutine<void>, OnGuildMemberRemoveData> handler) {
		return onGuildMemberRemoveEvent.add(std::move(handler));
	}

	void EventManager::onGuildMemberRemove(DiscordCoreInternal::EventDelegateToken& token) {
		onGuildMemberRemoveEvent.erase(token);
	}

	DiscordCoreInternal::EventDelegateToken EventManager::onGuildMemberUpdate(
		DiscordCoreInternal::EventDelegate<CoRoutine<void>, OnGuildMemberUpdateData> handler) {
		return onGuildMemberUpdateEvent.add(std::move(handler));
	}

	void EventManager::onGuildMemberUpdate(DiscordCoreInternal::EventDelegateToken& token) {
		onGuildMemberUpdateEvent.erase(token);
	}

	DiscordCoreInternal::EventDelegateToken EventManager::onRoleCreation(
		DiscordCoreInternal::EventDelegate<CoRoutine<void>, OnRoleCreationData> handler) {
		return onRoleCreationEvent.add(std::move(handler));
	}

	void EventManager::onRoleCreation(DiscordCoreInternal::EventDelegateToken& token) {
		onRoleCreationEvent.erase(token);
	}

	DiscordCoreInternal::EventDelegateToken EventManager::onRoleUpdate(
		DiscordCoreInternal::EventDelegate<CoRoutine<void>, OnRoleUpdateData> handler) {
		return onRoleUpdateEvent.add(std::move(handler));
	}

	void EventManager::onRoleUpdate(DiscordCoreInternal::EventDelegateToken& token) {
		onRoleUpdateEvent.erase(token);
	}

	DiscordCoreInternal::EventDelegateToken EventManager::onRoleDeletion(
		DiscordCoreInternal::EventDelegate<CoRoutine<void>, OnRoleDeletionData> handler) {
		return onRoleDeletionEvent.add(std::move(handler));
	}

	void EventManager::onRoleDeletion(DiscordCoreInternal::EventDelegateToken& token) {
		onRoleDeletionEvent.erase(token);
	}

	DiscordCoreInternal::EventDelegateToken EventManager::onGuildScheduledEventCreation(
		DiscordCoreInternal::EventDelegate<CoRoutine<void>, OnGuildScheduledEventCreationData> handler) {
		return onGuildScheduledEventCreationEvent.add(std::move(handler));
	}

	void EventManager::onGuildScheduledEventCreation(DiscordCoreInternal::EventDelegateToken& token) {
		onGuildScheduledEventCreationEvent.erase(token);
	}

	DiscordCoreInternal::EventDelegateToken EventManager::onGuildScheduledEventUpdate(
		DiscordCoreInternal::EventDelegate<CoRoutine<void>, OnGuildScheduledEventUpdateData> handler) {
		return onGuildScheduledEventUpdateEvent.add(std::move(handler));
	}

	void EventManager::onGuildScheduledEventUpdate(DiscordCoreInternal::EventDelegateToken& token) {
		onGuildScheduledEventUpdateEvent.erase(token);
	}

	DiscordCoreInternal::EventDelegateToken EventManager::onGuildScheduledEventDeletion(
		DiscordCoreInternal::EventDelegate<CoRoutine<void>, OnGuildScheduledEventDeletionData> handler) {
		return onGuildScheduledEventDeletionEvent.add(std::move(handler));
	}

	void EventManager::onGuildScheduledEventDeletion(DiscordCoreInternal::EventDelegateToken& token) {
		onGuildScheduledEventDeletionEvent.erase(token);
	}

	DiscordCoreInternal::EventDelegateToken EventManager::onGuildScheduledEventUserAdd(
		DiscordCoreInternal::EventDelegate<CoRoutine<void>, OnGuildScheduledEventUserAddData> handler) {
		return onGuildScheduledEventUserAddEvent.add(std::move(handler));
	}

	void EventManager::onGuildScheduledEventUserAdd(DiscordCoreInternal::EventDelegateToken& token) {
		onGuildScheduledEventUserAddEvent.erase(token);
	}

	DiscordCoreInternal::EventDelegateToken EventManager::onGuildScheduledEventUserRemove(
		DiscordCoreInternal::EventDelegate<CoRoutine<void>, OnGuildScheduledEventUserRemoveData> handler) {
		return onGuildScheduledEventUserRemoveEvent.add(std::move(handler));
	}

	void EventManager::onGuildScheduledEventUserRemove(DiscordCoreInternal::EventDelegateToken& token) {
		onGuildScheduledEventUserRemoveEvent.erase(token);
	}

	DiscordCoreInternal::EventDelegateToken EventManager::onIntegrationCreation(
		DiscordCoreInternal::EventDelegate<CoRoutine<void>, OnIntegrationCreationData> handler) {
		return onIntegrationCreationEvent.add(std::move(handler));
	}

	void EventManager::onIntegrationCreation(DiscordCoreInternal::EventDelegateToken& token) {
		onIntegrationCreationEvent.erase(token);
	}

	DiscordCoreInternal::EventDelegateToken EventManager::onIntegrationUpdate(
		DiscordCoreInternal::EventDelegate<CoRoutine<void>, OnIntegrationUpdateData> handler) {
		return onIntegrationUpdateEvent.add(std::move(handler));
	}

	void EventManager::onIntegrationUpdate(DiscordCoreInternal::EventDelegateToken& token) {
		onIntegrationUpdateEvent.erase(token);
	}

	DiscordCoreInternal::EventDelegateToken EventManager::onIntegrationDeletion(
		DiscordCoreInternal::EventDelegate<CoRoutine<void>, OnIntegrationDeletionData> handler) {
		return onIntegrationDeletionEvent.add(std::move(handler));
	}

	void EventManager::onIntegrationDeletion(DiscordCoreInternal::EventDelegateToken& token) {
		onIntegrationDeletionEvent.erase(token);
	}

	DiscordCoreInternal::EventDelegateToken EventManager::onInteractionCreation(
		DiscordCoreInternal::EventDelegate<CoRoutine<void>, OnInteractionCreationData> handler) {
		return onInteractionCreationEvent.add(std::move(handler));
	}

	void EventManager::onInteractionCreation(DiscordCoreInternal::EventDelegateToken& token) {
		onInteractionCreationEvent.erase(token);
	}

	DiscordCoreInternal::EventDelegateToken EventManager::onInviteCreation(
		DiscordCoreInternal::EventDelegate<CoRoutine<void>, OnInviteCreationData> handler) {
		return onInviteCreationEvent.add(std::move(handler));
	}

	void EventManager::onInviteCreation(DiscordCoreInternal::EventDelegateToken& token) {
		onInviteCreationEvent.erase(token);
	}

	DiscordCoreInternal::EventDelegateToken EventManager::onInviteDeletion(
		DiscordCoreInternal::EventDelegate<CoRoutine<void>, OnInviteDeletionData> handler) {
		return onInviteDeletionEvent.add(std::move(handler));
	}

	void EventManager::onInviteDeletion(DiscordCoreInternal::EventDelegateToken& token) {
		onInviteDeletionEvent.erase(token);
	}

	DiscordCoreInternal::EventDelegateToken EventManager::onMessageCreation(
		DiscordCoreInternal::EventDelegate<CoRoutine<void>, OnMessageCreationData> handler) {
		return onMessageCreationEvent.add(std::move(handler));
	}

	void EventManager::onMessageCreation(DiscordCoreInternal::EventDelegateToken& token) {
		onMessageCreationEvent.erase(token);
	}

	DiscordCoreInternal::EventDelegateToken EventManager::onMessageUpdate(
		DiscordCoreInternal::EventDelegate<CoRoutine<void>, OnMessageUpdateData> handler) {
		return onMessageUpdateEvent.add(std::move(handler));
	}

	void EventManager::onMessageUpdate(DiscordCoreInternal::EventDelegateToken& token) {
		onMessageUpdateEvent.erase(token);
	}

	DiscordCoreInternal::EventDelegateToken EventManager::onMessageDeletion(
		DiscordCoreInternal::EventDelegate<CoRoutine<void>, OnMessageDeletionData> handler) {
		return onMessageDeletionEvent.add(std::move(handler));
	}

	void EventManager::onMessageDeletion(DiscordCoreInternal::EventDelegateToken& token) {
		onMessageDeletionEvent.erase(token);
	}

	DiscordCoreInternal::EventDelegateToken EventManager::onMessageDeleteBulk(
		DiscordCoreInternal::EventDelegate<CoRoutine<void>, OnMessageDeleteBulkData> handler) {
		return onMessageDeleteBulkEvent.add(std::move(handler));
	}

	void EventManager::onMessageDeleteBulk(DiscordCoreInternal::EventDelegateToken& token) {
		onMessageDeleteBulkEvent.erase(token);
	}

	DiscordCoreInternal::EventDelegateToken EventManager::onReactionAdd(
		DiscordCoreInternal::EventDelegate<CoRoutine<void>, OnReactionAddData> handler) {
		return onReactionAddEvent.add(std::move(handler));
	}

	void EventManager::onReactionAdd(DiscordCoreInternal::EventDelegateToken& token) {
		onReactionAddEvent.erase(token);
	}

	DiscordCoreInternal::EventDelegateToken EventManager::onReactionRemove(
		DiscordCoreInternal::EventDelegate<CoRoutine<void>, OnReactionRemoveData> handler) {
		return onReactionRemoveEvent.add(std::move(handler));
	}

	void EventManager::onReactionRemove(DiscordCoreInternal::EventDelegateToken& token) {
		onReactionRemoveEvent.erase(token);
	}

	DiscordCoreInternal::EventDelegateToken EventManager::onReactionRemoveAll(
		DiscordCoreInternal::EventDelegate<CoRoutine<void>, OnReactionRemoveAllData> handler) {
		return onReactionRemoveAllEvent.add(std::move(handler));
	}

	void EventManager::onReactionRemoveAll(DiscordCoreInternal::EventDelegateToken& token) {
		onReactionRemoveAllEvent.erase(token);
	}

	DiscordCoreInternal::EventDelegateToken EventManager::onReactionRemoveEmoji(
		DiscordCoreInternal::EventDelegate<CoRoutine<void>, OnReactionRemoveEmojiData> handler) {
		return onReactionRemoveEmojiEvent.add(std::move(handler));
	}

	void EventManager::onReactionRemoveEmoji(DiscordCoreInternal::EventDelegateToken& token) {
		onReactionRemoveEmojiEvent.erase(token);
	}

	DiscordCoreInternal::EventDelegateToken EventManager::onPresenceUpdate(
		DiscordCoreInternal::EventDelegate<CoRoutine<void>, OnPresenceUpdateData> handler) {
		return onPresenceUpdateEvent.add(std::move(handler));
	}

	void EventManager::onPresenceUpdate(DiscordCoreInternal::EventDelegateToken& token) {
		onPresenceUpdateEvent.erase(token);
	}

	DiscordCoreInternal::EventDelegateToken EventManager::onStageInstanceCreation(
		DiscordCoreInternal::EventDelegate<CoRoutine<void>, OnStageInstanceCreationData> handler) {
		return onStageInstanceCreationEvent.add(std::move(handler));
	}

	void EventManager::onStageInstanceCreation(DiscordCoreInternal::EventDelegateToken& token) {
		onStageInstanceCreationEvent.erase(token);
	}

	DiscordCoreInternal::EventDelegateToken EventManager::onStageInstanceUpdate(
		DiscordCoreInternal::EventDelegate<CoRoutine<void>, OnStageInstanceUpdateData> handler) {
		return onStageInstanceUpdateEvent.add(std::move(handler));
	}

	void EventManager::onStageInstanceUpdate(DiscordCoreInternal::EventDelegateToken& token) {
		onStageInstanceUpdateEvent.erase(token);
	}

	DiscordCoreInternal::EventDelegateToken EventManager::onStageInstanceDeletion(
		DiscordCoreInternal::EventDelegate<CoRoutine<void>, OnStageInstanceDeletionData> handler) {
		return onStageInstanceDeletionEvent.add(std::move(handler));
	}

	void EventManager::onStageInstanceDeletion(DiscordCoreInternal::EventDelegateToken& token) {
		onStageInstanceDeletionEvent.erase(token);
	}

	DiscordCoreInternal::EventDelegateToken EventManager::onTypingStart(
		DiscordCoreInternal::EventDelegate<CoRoutine<void>, OnTypingStartData> handler) {
		return onTypingStartEvent.add(std::move(handler));
	}

	void EventManager::onTypingStart(DiscordCoreInternal::EventDelegateToken& token) {
		onTypingStartEvent.erase(token);
	}

	DiscordCoreInternal::EventDelegateToken EventManager::onUserUpdate(
		DiscordCoreInternal::EventDelegate<CoRoutine<void>, OnUserUpdateData> handler) {
		return onUserUpdateEvent.add(std::move(handler));
	}

	void EventManager::onUserUpdate(DiscordCoreInternal::EventDelegateToken& token) {
		onUserUpdateEvent.erase(token);
	}

	DiscordCoreInternal::EventDelegateToken EventManager::onVoiceStateUpdate(
		DiscordCoreInternal::EventDelegate<CoRoutine<void>, OnVoiceStateUpdateData> handler) {
		return onVoiceStateUpdateEvent.add(std::move(handler));
	}

	void EventManager::onVoiceStateUpdate(DiscordCoreInternal::EventDelegateToken& token) {
		onVoiceStateUpdateEvent.erase(token);
	}

	DiscordCoreInternal::EventDelegateToken EventManager::onVoiceServerUpdate(
		DiscordCoreInternal::EventDelegate<CoRoutine<void>, OnVoiceServerUpdateData> handler) {
		return onVoiceServerUpdateEvent.add(std::move(handler));
	}

	void EventManager::onVoiceServerUpdate(DiscordCoreInternal::EventDelegateToken& token) {
		onVoiceServerUpdateEvent.erase(token);
	}

	DiscordCoreInternal::EventDelegateToken EventManager::onWebhookUpdate(
		DiscordCoreInternal::EventDelegate<CoRoutine<void>, OnWebhookUpdateData> handler) {
		return onWebhookUpdateEvent.add(std::move(handler));
	}

	void EventManager::onWebhookUpdate(DiscordCoreInternal::EventDelegateToken& token) {
		onWebhookUpdateEvent.erase(token);
	}*/

};