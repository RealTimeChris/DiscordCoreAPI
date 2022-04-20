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

};// namespace DiscordCoreAPI

namespace DiscordCoreInternal {

	nlohmann::json JSONIFY(std::string botToken, int32_t intents, int32_t currentShard, int32_t numberOfShards);

	nlohmann::json JSONIFY(int32_t lastReceivedNumber);

	nlohmann::json JSONIFY(DiscordCoreAPI::UpdatePresenceData dataPackage);

	nlohmann::json JSONIFY(DiscordCoreAPI::UpdateVoiceStateData dataPackage);

	nlohmann::json JSONIFY(std::string serverId, std::string sessionId, std::string token, std::string testString);

	nlohmann::json JSONIFY(std::string botToken, std::string sessionID, int32_t lastReceivedNumber);

	nlohmann::json JSONIFY(DiscordCoreAPI::CreateGlobalApplicationCommandData dataPackage);

	nlohmann::json JSONIFY(DiscordCoreAPI::CreateGuildApplicationCommandData dataPackage);

	nlohmann::json JSONIFY(DiscordCoreAPI::CreateGuildBanData dataPackage);

	nlohmann::json JSONIFY(DiscordCoreAPI::ModifyChannelData channelData);

	nlohmann::json JSONIFY(DiscordCoreAPI::ModifyGuildMemberData dataPackage);

	nlohmann::json JSONIFY(DiscordCoreAPI::ModifyGuildRoleData dataPackage);

	nlohmann::json JSONIFY(DiscordCoreAPI::CreateMessageData dataPackage);

	nlohmann::json JSONIFY(DiscordCoreAPI::EditMessageData dataPackage);

	nlohmann::json JSONIFY(DiscordCoreAPI::EditGuildApplicationCommandPermissionsData dataPackage);

	nlohmann::json JSONIFY(DiscordCoreAPI::BatchEditGuildApplicationCommandPermissionsData dataPackage);

	nlohmann::json JSONIFY(DiscordCoreAPI::EditGlobalApplicationCommandData dataPackage);

	nlohmann::json JSONIFY(DiscordCoreAPI::EditGuildApplicationCommandData dataPackage);

	nlohmann::json JSONIFY(DiscordCoreAPI::EditFollowUpMessageData dataPackage);

	nlohmann::json JSONIFY(DiscordCoreAPI::EditInteractionResponseData dataPackage);

	nlohmann::json JSONIFY(DiscordCoreAPI::CreateDeferredInteractionResponseData dataPackage);

	nlohmann::json JSONIFY(DiscordCoreAPI::CreateInteractionResponseData dataPackage);

	nlohmann::json JSONIFY(DiscordCoreAPI::CreateGuildRoleData dataPackage);

	nlohmann::json JSONIFY(DiscordCoreAPI::ModifyGuildRolePositionsData dataPackage);

	nlohmann::json JSONIFY(DiscordCoreAPI::EditChannelPermissionOverwritesData dataPackage);

	nlohmann::json JSONIFY(DiscordCoreAPI::CreateFollowUpMessageData dataPackage);

	nlohmann::json JSONIFY(DiscordCoreAPI::DeleteMessagesBulkData dataPackage);

	nlohmann::json JSONIFY(DiscordCoreAPI::CreateChannelInviteData dataPackage);

	nlohmann::json JSONIFY(DiscordCoreAPI::FollowNewsChannelData dataPackage);

	nlohmann::json JSONIFY(DiscordCoreAPI::AddRecipientToGroupDMData dataPackage);

	nlohmann::json JSONIFY(DiscordCoreAPI::StartThreadWithMessageData dataPackage);

	nlohmann::json JSONIFY(DiscordCoreAPI::StartThreadWithoutMessageData dataPackage);

	nlohmann::json JSONIFY(DiscordCoreAPI::CreateGuildEmojiData dataPackage);

	nlohmann::json JSONIFY(DiscordCoreAPI::ModifyGuildEmojiData dataPackage);

	nlohmann::json JSONIFY(DiscordCoreAPI::CreateGuildData dataPackage);

	nlohmann::json JSONIFY(DiscordCoreAPI::ModifyGuildData dataPackage);

	nlohmann::json JSONIFY(DiscordCoreAPI::CreateGuildChannelData dataPackage);

	nlohmann::json JSONIFY(DiscordCoreAPI::ModifyGuildChannelPositionsData dataPackage);

	nlohmann::json JSONIFY(DiscordCoreAPI::AddGuildMemberData dataPackage);

	nlohmann::json JSONIFY(DiscordCoreAPI::BeginGuildPruneData dataPackage);

	nlohmann::json JSONIFY(DiscordCoreAPI::ModifyGuildWelcomeScreenData dataPackage);

	nlohmann::json JSONIFY(DiscordCoreAPI::CreateGuildScheduledEventData dataPackage);

	nlohmann::json JSONIFY(DiscordCoreAPI::ModifyGuildScheduledEventData dataPackage);

	nlohmann::json JSONIFY(DiscordCoreAPI::ExecuteWebHookData dataPackage);

	nlohmann::json JSONIFY(DiscordCoreAPI::EditWebHookMessageData dataPackage);

	nlohmann::json JSONIFY(int64_t nonce);

	nlohmann::json JSONIFY(std::string localPort, std::string localIp, std::string encryptionMode, int32_t testValue);

	nlohmann::json JSONIFY(DiscordCoreInternal::VoiceConnectionData dataPackage, VoiceConnectInitData dataPackage02);

	nlohmann::json JSONIFY(bool isSpeaking, int32_t ssrc, int32_t delay = 0);
}// namespace DiscordCoreInternal
