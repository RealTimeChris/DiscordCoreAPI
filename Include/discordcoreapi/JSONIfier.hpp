/*
*
	DiscordCoreAPI, A bot library for Discord, written in C++, and featuring explicit multithreading through the usage of custom, asynchronous C++ CoRoutines.

	Copyright 2021, 2022 Chris M. (RealTimeChris)

	This file is part of DiscordCoreAPI.
	DiscordCoreAPI is free software: you can redistribute it and/or modify it under the terms of the GNU
	General Public License as published by the Free Software Foundation, either version 3 of the License,
	or (at your option) any later version.
	DiscordCoreAPI is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without
	even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.
	You should have received a copy of the GNU General Public License along with DiscordCoreAPI.
	If not, see <https://www.gnu.org/licenses/>.

*/
/// JSONIfier.hpp - For all things related to JSON parsing etc.
/// Apr 21, 2021
/// https://discordcoreapi.com
/// \file JSONIfier.hpp

#pragma once

#include <discordcoreapi/FoundationEntities.hpp>

namespace DiscordCoreAPI {

	struct DiscordCoreAPI_Dll BatchEditGuildApplicationCommandPermissionsData;
	struct DiscordCoreAPI_Dll EditGuildApplicationCommandPermissionsData;
	struct DiscordCoreAPI_Dll EditChannelPermissionOverwritesData;
	struct DiscordCoreAPI_Dll CreateGlobalApplicationCommandData;
	struct DiscordCoreAPI_Dll CreateGuildApplicationCommandData;
	struct DiscordCoreAPI_Dll EditGlobalApplicationCommandData;
	struct DiscordCoreAPI_Dll EditGuildApplicationCommandData;
	struct DiscordCoreAPI_Dll ModifyGuildChannelPositionsData;
	struct DiscordCoreAPI_Dll StartThreadWithoutMessageData;
	struct DiscordCoreAPI_Dll CreateGuildScheduledEventData;
	struct DiscordCoreAPI_Dll ModifyGuildScheduledEventData;
	struct DiscordCoreAPI_Dll StartThreadWithoutMessageData;
	struct DiscordCoreAPI_Dll StartThreadInForumChannelData;
	struct DiscordCoreAPI_Dll ModifyGuildWelcomeScreenData;
	struct DiscordCoreAPI_Dll ModifyGuildRolePositionsData;
	struct DiscordCoreAPI_Dll CreateAutoModerationRuleData;
	struct DiscordCoreAPI_Dll ModifyAutoModerationRuleData;
	struct DiscordCoreAPI_Dll EditApplicationCommandData;
	struct DiscordCoreAPI_Dll StartThreadWithMessageData;
	struct DiscordCoreAPI_Dll AddRecipientToGroupDMData;
	struct DiscordCoreAPI_Dll CreateChannelInviteData;
	struct DiscordCoreAPI_Dll DeleteMessagesBulkData;
	struct DiscordCoreAPI_Dll CreateGuildChannelData;
	struct DiscordCoreAPI_Dll FollowNewsChannelData;
	struct DiscordCoreAPI_Dll ModifyGuildMemberData;
	struct DiscordCoreAPI_Dll ModifyGuildEmojiData;
	struct DiscordCoreAPI_Dll CreateGuildEmojiData;
	struct DiscordCoreAPI_Dll UpdateVoiceStateData;
	struct DiscordCoreAPI_Dll CreateGuildRoleData;
	struct DiscordCoreAPI_Dll BeginGuildPruneData;
	struct DiscordCoreAPI_Dll ModifyGuildRoleData;
	struct DiscordCoreAPI_Dll AddGuildMemberData;
	struct DiscordCoreAPI_Dll CreateGuildBanData;
	struct DiscordCoreAPI_Dll UpdatePresenceData;
	struct DiscordCoreAPI_Dll ModifyChannelData;
	struct DiscordCoreAPI_Dll ModifyGuildData;
	struct DiscordCoreAPI_Dll CreateGuildData;

	class DiscordCoreAPI_Dll CreateDeferredInteractionResponseData;
	class DiscordCoreAPI_Dll CreateInteractionResponseData;
	class DiscordCoreAPI_Dll EditInteractionResponseData;
	class DiscordCoreAPI_Dll CreateFollowUpMessageData;
	class DiscordCoreAPI_Dll EditFollowUpMessageData;
	class DiscordCoreAPI_Dll EditWebHookMessageData;
	class DiscordCoreAPI_Dll ExecuteWebHookData;
	class DiscordCoreAPI_Dll CreateMessageData;
	class DiscordCoreAPI_Dll EditMessageData;
	class DiscordCoreAPI_Dll EditWebHookData;

	void to_json(nlohmann::json& jsonOut, const DiscordCoreAPI::MessageReferenceData& valueNew);

	void to_json(nlohmann::json& jsonOut, const DiscordCoreAPI::AllowedMentionsData& valueIn);

	void to_json(nlohmann::json& jsonOut, const DiscordCoreAPI::AttachmentData& valueIn);

	void to_json(nlohmann::json& jsonOut, const DiscordCoreAPI::EmbedFieldData& valueIn);

	void to_json(nlohmann::json& jsonOut, const std::vector<ActionRowData>&& dataPackage);

	void to_json(nlohmann::json& jsonOut, const DiscordCoreAPI::EmbedData& valueIn);

};// namespace DiscordCoreAPI

namespace DiscordCoreInternal {

	nlohmann::json JSONIFY(const std::string& serverId, const std::string& sessionId, const std::string& token, const std::string& testString);

	nlohmann::json JSONIFY(const std::string& botToken, int64_t intents, int32_t currentShard, int32_t numberOfShards);

	nlohmann::json JSONIFY(const std::string& botToken, std::string sessionID, int32_t lastReceivedNumber);

	nlohmann::json JSONIFY(const DiscordCoreAPI::UpdateVoiceStateData& dataPackage);

	nlohmann::json JSONIFY(const DiscordCoreAPI::UpdatePresenceData& dataPackage);

	nlohmann::json JSONIFY(int32_t lastReceivedNumber);

	std::vector<uint8_t> JSONIFY(const std::string& localPort, const std::string& localIp, const std::string& encryptionMode, int32_t testValue);

	std::vector<uint8_t> JSONIFY(const VoiceConnectionData& dataPackage, const VoiceConnectInitData& dataPackage02);

	std::vector<uint8_t> JSONIFY(int32_t ssrc, int32_t delay);

	std::vector<uint8_t> JSONIFY(int64_t nonce);

	void JSONIFY(const DiscordCoreAPI::ApplicationCommandOptionData& dataPackage, nlohmann::json* pJSONData);

	std::string JSONIFY(const DiscordCoreAPI::EditGuildApplicationCommandPermissionsData& dataPackage);

	std::string JSONIFY(const DiscordCoreAPI::EditChannelPermissionOverwritesData& dataPackage);

	std::string JSONIFY(const DiscordCoreAPI::CreateGlobalApplicationCommandData& dataPackage);

	std::string JSONIFY(const DiscordCoreAPI::CreateGuildApplicationCommandData& dataPackage);

	std::string JSONIFY(const DiscordCoreAPI::EditGlobalApplicationCommandData& dataPackage);

	std::string JSONIFY(const DiscordCoreAPI::EditGuildApplicationCommandData& dataPackage);

	std::string JSONIFY(const DiscordCoreAPI::ModifyGuildChannelPositionsData& dataPackage);

	std::string JSONIFY(const DiscordCoreAPI::StartThreadWithoutMessageData& dataPackage);

	std::string JSONIFY(const DiscordCoreAPI::StartThreadInForumChannelData& dataPackage);

	std::string JSONIFY(const DiscordCoreAPI::CreateGuildScheduledEventData& dataPackage);

	std::string JSONIFY(const DiscordCoreAPI::ModifyGuildScheduledEventData& dataPackage);

	std::string JSONIFY(const DiscordCoreAPI::ModifyGuildRolePositionsData& dataPackage);

	std::string JSONIFY(const DiscordCoreAPI::CreateAutoModerationRuleData& dataPackage);

	std::string JSONIFY(const DiscordCoreAPI::ModifyAutoModerationRuleData& dataPackage);

	std::string JSONIFY(const DiscordCoreAPI::ModifyGuildWelcomeScreenData& dataPackage);

	std::string JSONIFY(const DiscordCoreAPI::StartThreadWithMessageData& dataPackage);

	std::string JSONIFY(const DiscordCoreAPI::AddRecipientToGroupDMData& dataPackage);

	std::string JSONIFY(const DiscordCoreAPI::InteractionResponseData& dataPackage);

	std::string JSONIFY(const DiscordCoreAPI::CreateChannelInviteData& dataPackage);

	std::string JSONIFY(const DiscordCoreAPI::DeleteMessagesBulkData& dataPackage);

	std::string JSONIFY(const DiscordCoreAPI::FollowNewsChannelData& dataPackage);

	std::string JSONIFY(const DiscordCoreAPI::CreateGuildEmojiData& dataPackage);

	std::string JSONIFY(const DiscordCoreAPI::ModifyGuildEmojiData& dataPackage);

	std::string JSONIFY(const DiscordCoreAPI::BeginGuildPruneData& dataPackage);

	std::string JSONIFY(const DiscordCoreAPI::CreateGuildBanData& dataPackage);

	std::string JSONIFY(const DiscordCoreAPI::AddGuildMemberData& dataPackage);

	std::string JSONIFY(DiscordCoreAPI::CreateGuildChannelData& dataPackage);

	std::string JSONIFY(const DiscordCoreAPI::ModifyGuildData& dataPackage);

	std::string JSONIFY(const DiscordCoreAPI::EditWebHookData& dataPackage);

	std::string JSONIFY(DiscordCoreAPI::ModifyGuildMemberData& dataPackage);

	std::string JSONIFY(DiscordCoreAPI::ModifyGuildRoleData& dataPackage);

	std::string JSONIFY(DiscordCoreAPI::CreateGuildRoleData& dataPackage);

	std::string JSONIFY(DiscordCoreAPI::ExecuteWebHookData& dataPackage);

	std::string JSONIFY(DiscordCoreAPI::CreateMessageData& dataPackage);

	std::string JSONIFY(DiscordCoreAPI::ModifyChannelData& channelData);

	std::string JSONIFY(DiscordCoreAPI::CreateGuildData& dataPackage);

	std::string JSONIFY(DiscordCoreAPI::EditMessageData& dataPackage);


}// namespace DiscordCoreInternal
