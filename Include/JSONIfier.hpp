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
/// JSONIfier.hpp - For all things related to JSON parsing etc.
/// Apr 21, 2021
/// Chris M.
/// https://github.com/RealTimeChris/DiscordCoreAPI
/// \file JSONIfier.hpp

#pragma once

#include <FoundationEntities.hpp>

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

};

namespace DiscordCoreInternal {

	int32_t JSONIFY(std::string initialPayload);

	nlohmann::json JSONIFY(std::string botToken, int32_t intents, int32_t currentShard, int32_t numberOfShards);

	nlohmann::json JSONIFY(int32_t lastReceivedNumber);

	nlohmann::json JSONIFY(DiscordCoreAPI::UpdatePresenceData dataPackage);

	nlohmann::json JSONIFY(DiscordCoreAPI::UpdateVoiceStateData dataPackage);

	nlohmann::json JSONIFY(std::string serverId, std::string sessionId, std::string token, std::string testString);

	nlohmann::json JSONIFY(std::string botToken, std::string sessionID, int32_t lastReceivedNumber);

	std::vector<uint8_t> JSONIFY(int64_t nonce);

	std::vector<uint8_t> JSONIFY(std::string localPort, std::string localIp, std::string encryptionMode, int32_t testValue);

	std::vector<uint8_t> JSONIFY(DiscordCoreInternal::VoiceConnectionData dataPackage, VoiceConnectInitData dataPackage02);

	std::vector<uint8_t> JSONIFY(bool isSpeaking, int32_t ssrc, int32_t delay = 0);

	std::string JSONIFY(DiscordCoreAPI::CreateGuildBanData dataPackage);

	std::string JSONIFY(DiscordCoreAPI::ModifyChannelData channelData);

	std::string JSONIFY(DiscordCoreAPI::ModifyGuildMemberData dataPackage);

	std::string JSONIFY(DiscordCoreAPI::ModifyGuildRoleData dataPackage);

	std::string JSONIFY(DiscordCoreAPI::CreateMessageData dataPackage);

	std::string JSONIFY(DiscordCoreAPI::EditMessageData dataPackage);

	std::string JSONIFY(DiscordCoreAPI::EditGuildApplicationCommandPermissionsData dataPackage);

	std::string JSONIFY(DiscordCoreAPI::BatchEditGuildApplicationCommandPermissionsData dataPackage);

	std::string JSONIFY(DiscordCoreAPI::CreateGlobalApplicationCommandData dataPackage);

	std::string JSONIFY(DiscordCoreAPI::EditGlobalApplicationCommandData dataPackage);

	std::string JSONIFY(DiscordCoreAPI::CreateGuildApplicationCommandData dataPackage);

	std::string JSONIFY(DiscordCoreAPI::EditGuildApplicationCommandData dataPackage);

	std::string JSONIFY(DiscordCoreAPI::EditFollowUpMessageData dataPackage);

	std::string JSONIFY(DiscordCoreAPI::EditInteractionResponseData dataPackage);

	std::string JSONIFY(DiscordCoreAPI::CreateDeferredInteractionResponseData dataPackage);

	std::string JSONIFY(DiscordCoreAPI::CreateInteractionResponseData dataPackage);

	std::string JSONIFY(DiscordCoreAPI::CreateGuildRoleData dataPackage);

	std::string JSONIFY(DiscordCoreAPI::ModifyGuildRolePositionsData dataPackage);

	std::string JSONIFY(DiscordCoreAPI::EditChannelPermissionOverwritesData dataPackage);

	std::string JSONIFY(DiscordCoreAPI::CreateFollowUpMessageData dataPackage);

	std::string JSONIFY(DiscordCoreAPI::DeleteMessagesBulkData dataPackage);
	
	std::string JSONIFY(DiscordCoreAPI::CreateChannelInviteData dataPackage);

	std::string JSONIFY(DiscordCoreAPI::FollowNewsChannelData dataPackage);

	std::string JSONIFY(DiscordCoreAPI::AddRecipientToGroupDMData dataPackage);

	std::string JSONIFY(DiscordCoreAPI::StartThreadWithMessageData dataPackage);

	std::string JSONIFY(DiscordCoreAPI::StartThreadWithoutMessageData dataPackage);

	std::string JSONIFY(DiscordCoreAPI::CreateGuildEmojiData dataPackage);

	std::string JSONIFY(DiscordCoreAPI::ModifyGuildEmojiData dataPackage);

	std::string JSONIFY(DiscordCoreAPI::CreateGuildData dataPackage);

	std::string JSONIFY(DiscordCoreAPI::ModifyGuildData dataPackage);

	std::string JSONIFY(DiscordCoreAPI::CreateGuildChannelData dataPackage);

	std::string JSONIFY(DiscordCoreAPI::ModifyGuildChannelPositionsData dataPackage);

	std::string JSONIFY(DiscordCoreAPI::AddGuildMemberData dataPackage);

	std::string JSONIFY(DiscordCoreAPI::BeginGuildPruneData dataPackage);

	std::string JSONIFY(DiscordCoreAPI::ModifyGuildWelcomeScreenData dataPackage);
	
	std::string JSONIFY(DiscordCoreAPI::CreateGuildScheduledEventData dataPackage);

	std::string JSONIFY(DiscordCoreAPI::ModifyGuildScheduledEventData dataPackage);

	std::string JSONIFY(DiscordCoreAPI::ExecuteWebHookData dataPackage);

	std::string JSONIFY(DiscordCoreAPI::EditWebHookMessageData dataPackage);
}
