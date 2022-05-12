/*
*
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
/// Chris M.
/// https://discordcoreapi.com
/// \file JSONIfier.hpp

#pragma once

#include <discordcoreapi/FoundationEntities.hpp>

namespace DiscordCoreAPI {

	struct BatchEditGuildApplicationCommandPermissionsData;
	struct EditGuildApplicationCommandPermissionsData;
	struct EditChannelPermissionOverwritesData;
	struct CreateGlobalApplicationCommandData;
	struct CreateGuildApplicationCommandData;
	struct EditGlobalApplicationCommandData;
	struct EditGuildApplicationCommandData;
	struct ModifyGuildChannelPositionsData;
	struct StartThreadWithoutMessageData;
	struct CreateGuildScheduledEventData;
	struct ModifyGuildScheduledEventData;
	struct StartThreadWithoutMessageData;
	struct StartThreadInForumChannelData;
	struct ModifyGuildWelcomeScreenData;
	struct ModifyGuildRolePositionsData;
	struct EditApplicationCommandData;
	struct StartThreadWithMessageData;
	struct AddRecipientToGroupDMData;
	struct CreateChannelInviteData;
	struct DeleteMessagesBulkData;
	struct CreateGuildChannelData;
	struct FollowNewsChannelData;
	struct ModifyGuildMemberData;
	struct ModifyGuildEmojiData;
	struct CreateGuildEmojiData;
	struct UpdateVoiceStateData;
	struct CreateGuildRoleData;
	struct BeginGuildPruneData;
	struct ModifyGuildRoleData;
	struct AddGuildMemberData;
	struct CreateGuildBanData;
	struct UpdatePresenceData;
	struct ModifyChannelData;
	struct ModifyGuildData;
	struct CreateGuildData;

	class CreateDeferredInteractionResponseData;
	class CreateInteractionResponseData;
	class EditInteractionResponseData;
	class CreateFollowUpMessageData;
	class EditFollowUpMessageData;
	class EditWebHookMessageData;
	class ExecuteWebHookData;
	class CreateMessageData;
	class EditMessageData;
	class EditWebHookData;

	void to_json(nlohmann::json& jsonOut, const DiscordCoreAPI::MessageReferenceData& valueNew);

	void to_json(nlohmann::json& jsonOut, const DiscordCoreAPI::AllowedMentionsData& valueIn);

	void to_json(nlohmann::json& jsonOut, const DiscordCoreAPI::AttachmentData& valueIn);

	void to_json(nlohmann::json& jsonOut, const DiscordCoreAPI::EmbedFieldData& valueIn);

	void to_json(nlohmann::json& jsonOut, const std::vector<ActionRowData>& dataPackage);

	void to_json(nlohmann::json& jsonOut, const DiscordCoreAPI::EmbedData& valueIn);

};// namespace DiscordCoreAPI

namespace DiscordCoreInternal {

	int32_t JSONIFY(std::string initialPayload);

	nlohmann::json JSONIFY(std::string serverId, std::string sessionId, std::string token, std::string testString);

	nlohmann::json JSONIFY(std::string botToken, int64_t intents, int32_t currentShard, int32_t numberOfShards);

	nlohmann::json JSONIFY(std::string botToken, std::string sessionID, int32_t lastReceivedNumber);

	nlohmann::json JSONIFY(DiscordCoreAPI::UpdateVoiceStateData dataPackage);
	
	nlohmann::json JSONIFY(DiscordCoreAPI::UpdatePresenceData dataPackage);

	nlohmann::json JSONIFY(int32_t lastReceivedNumber);

	std::vector<uint8_t> JSONIFY(std::string localPort, std::string localIp, std::string encryptionMode, int32_t testValue);

	std::vector<uint8_t> JSONIFY(DiscordCoreInternal::VoiceConnectionData dataPackage, VoiceConnectInitData dataPackage02);

	std::vector<uint8_t> JSONIFY(int32_t ssrc, int32_t delay);

	std::vector<uint8_t> JSONIFY(int64_t nonce);

	void JSONIFY(DiscordCoreAPI::ApplicationCommandOptionData dataPackage, nlohmann::json* pJSONData);

	std::string JSONIFY(DiscordCoreAPI::EditGuildApplicationCommandPermissionsData dataPackage);

	std::string JSONIFY(DiscordCoreAPI::EditChannelPermissionOverwritesData dataPackage);

	std::string JSONIFY(DiscordCoreAPI::CreateGlobalApplicationCommandData dataPackage);

	std::string JSONIFY(DiscordCoreAPI::CreateGuildApplicationCommandData dataPackage);

	std::string JSONIFY(DiscordCoreAPI::EditGlobalApplicationCommandData dataPackage);

	std::string JSONIFY(DiscordCoreAPI::EditGuildApplicationCommandData dataPackage);

	std::string JSONIFY(DiscordCoreAPI::ModifyGuildChannelPositionsData dataPackage);

	std::string JSONIFY(DiscordCoreAPI::StartThreadWithoutMessageData dataPackage);

	std::string JSONIFY(DiscordCoreAPI::StartThreadInForumChannelData dataPackage);
	
	std::string JSONIFY(DiscordCoreAPI::CreateGuildScheduledEventData dataPackage);

	std::string JSONIFY(DiscordCoreAPI::ModifyGuildRolePositionsData dataPackage);

	std::string JSONIFY(DiscordCoreAPI::ModifyGuildScheduledEventData dataPackage);

	std::string JSONIFY(DiscordCoreAPI::ModifyGuildWelcomeScreenData dataPackage);

	std::string JSONIFY(DiscordCoreAPI::StartThreadWithMessageData dataPackage);

	std::string JSONIFY(DiscordCoreAPI::AddRecipientToGroupDMData dataPackage);
	
	std::string JSONIFY(DiscordCoreAPI::InteractionResponseData dataPackage);

	std::string JSONIFY(DiscordCoreAPI::CreateChannelInviteData dataPackage);

	std::string JSONIFY(DiscordCoreAPI::DeleteMessagesBulkData dataPackage);

	std::string JSONIFY(DiscordCoreAPI::CreateGuildChannelData dataPackage);
	
	std::string JSONIFY(DiscordCoreAPI::ModifyGuildMemberData dataPackage);	

	std::string JSONIFY(DiscordCoreAPI::FollowNewsChannelData dataPackage);	

	std::string JSONIFY(DiscordCoreAPI::CreateGuildEmojiData dataPackage);

	std::string JSONIFY(DiscordCoreAPI::CreateGuildRoleData dataPackage);

	std::string JSONIFY(DiscordCoreAPI::ModifyGuildEmojiData dataPackage);

	std::string JSONIFY(DiscordCoreAPI::ModifyGuildRoleData dataPackage);

	std::string JSONIFY(DiscordCoreAPI::BeginGuildPruneData dataPackage);

	std::string JSONIFY(DiscordCoreAPI::CreateGuildBanData dataPackage);

	std::string JSONIFY(DiscordCoreAPI::AddGuildMemberData dataPackage);

	std::string JSONIFY(DiscordCoreAPI::ExecuteWebHookData dataPackage);

	std::string JSONIFY(DiscordCoreAPI::ModifyChannelData channelData);

	std::string JSONIFY(DiscordCoreAPI::CreateMessageData dataPackage);

	std::string JSONIFY(DiscordCoreAPI::EditMessageData dataPackage);	

	std::string JSONIFY(DiscordCoreAPI::CreateGuildData dataPackage);

	std::string JSONIFY(DiscordCoreAPI::ModifyGuildData dataPackage);	

	std::string JSONIFY(DiscordCoreAPI::EditWebHookData dataPackage);


}// namespace DiscordCoreInternal
