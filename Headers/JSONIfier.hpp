// JSONifier.hpp - For all things related to JSON parsing etc.
// Apr 21, 2021
// Chris M.
// https://github.com/RealTimeChris

#pragma once

#include "IndexInitial.hpp"
#include "ChannelEntities.hpp"

namespace DiscordCoreAPI {

	struct alignas(hardware_destructive_interference_size)  CreateDeferredInteractionResponseData;
	struct alignas(hardware_destructive_interference_size)  EditChannelPermissionOverwritesData;
	struct alignas(hardware_destructive_interference_size)  StartThreadWithoutMessageData;
	struct alignas(hardware_destructive_interference_size)  CreateInteractionResponseData;
	struct alignas(hardware_destructive_interference_size)  StartThreadWithoutMessageData;
	struct alignas(hardware_destructive_interference_size)  ModifyGuildRolePositionsData;
	struct alignas(hardware_destructive_interference_size)  EditInteractionResponseData;
	struct alignas(hardware_destructive_interference_size)  StartThreadWithMessageData;
	struct alignas(hardware_destructive_interference_size)  CreateFollowUpMessageData;
	struct alignas(hardware_destructive_interference_size)  AddRecipientToGroupDMData;
	struct alignas(hardware_destructive_interference_size)  EditFollowUpMessageData;
	struct alignas(hardware_destructive_interference_size)  CreateChannelInviteData;
	struct alignas(hardware_destructive_interference_size)  DeleteMessagesBulkData;
	struct alignas(hardware_destructive_interference_size)  FollowNewsChannelData;
	struct alignas(hardware_destructive_interference_size)  ModifyGuildMemberData;
	struct alignas(hardware_destructive_interference_size)  ModifyGuildEmojiData;
	struct alignas(hardware_destructive_interference_size)  CreateGuildEmojiData;
	struct alignas(hardware_destructive_interference_size)  UpdateVoiceStateData;
	struct alignas(hardware_destructive_interference_size)  CreateGuildRoleData;
	struct alignas(hardware_destructive_interference_size)  BeginGuildPruneData;
	struct alignas(hardware_destructive_interference_size)  ModifyGuildRoleData;
	struct alignas(hardware_destructive_interference_size)  AddGuildMemberData;
	struct alignas(hardware_destructive_interference_size)  CreateGuildBanData;
	struct alignas(hardware_destructive_interference_size)  CreateMessageData;
	struct alignas(hardware_destructive_interference_size)  ModifyChannelData;
	struct alignas(hardware_destructive_interference_size)  ModifyGuildData;
	struct alignas(hardware_destructive_interference_size)  EditMessageData;
	struct alignas(hardware_destructive_interference_size)  CreateGuildData;
	struct alignas(hardware_destructive_interference_size)  SendDMData;

};

namespace DiscordCoreInternal {

	DiscordCoreAPI_Dll json JSONIFY(string botToken, int32_t intents);

	DiscordCoreAPI_Dll json JSONIFY(int32_t lastReceivedNumber);

	DiscordCoreAPI_Dll json JSONIFY(UpdatePresenceData dataPackage);

	DiscordCoreAPI_Dll json JSONIFY(DiscordCoreAPI::UpdateVoiceStateData dataPackage);

	DiscordCoreAPI_Dll json JSONIFY(string serverId, string sessionId, string token, string testString);

	DiscordCoreAPI_Dll json JSONIFY(string botToken, string sessionID, int32_t lastReceivedNumber);

	DiscordCoreAPI_Dll string JSONIFY(int64_t nonce);

	DiscordCoreAPI_Dll string JSONIFY(string localPort, string localIp, string encryptionMode, int32_t testValue);

	DiscordCoreAPI_Dll string JSONIFY(DiscordCoreInternal::VoiceConnectionData dataPackage, VoiceConnectInitData dataPackage02);

	DiscordCoreAPI_Dll string JSONIFY(bool isSpeaking, int32_t ssrc, int32_t delay = 0);

	DiscordCoreAPI_Dll string JSONIFY(DiscordCoreAPI::CreateGuildBanData dataPackage);

	DiscordCoreAPI_Dll string JSONIFY(DiscordCoreAPI::ModifyChannelData channelData);

	DiscordCoreAPI_Dll string JSONIFY(DiscordCoreAPI::ModifyGuildMemberData dataPackage);

	DiscordCoreAPI_Dll string JSONIFY(DiscordCoreAPI::ModifyGuildRoleData dataPackage);

	DiscordCoreAPI_Dll string JSONIFY(string initialPayload);

	DiscordCoreAPI_Dll int32_t JSONIFY(hstring initialPayload);

	DiscordCoreAPI_Dll string JSONIFY(DiscordCoreAPI::CreateMessageData dataPackage);

	DiscordCoreAPI_Dll string JSONIFY(DiscordCoreAPI::SendDMData dataPackage);

	DiscordCoreAPI_Dll string JSONIFY(DiscordCoreAPI::EditMessageData dataPackage);

	DiscordCoreAPI_Dll void JSONIFY(DiscordCoreAPI::ApplicationCommandOptionData dataPackage, json* pJSONData);

	DiscordCoreAPI_Dll string JSONIFY(DiscordCoreAPI::PutEditApplicationCommandPermissionsData dataPackage);

	DiscordCoreAPI_Dll string JSONIFY(DiscordCoreAPI::PutBatchEditApplicationCommandPermissionsData dataPackage);

	DiscordCoreAPI_Dll string JSONIFY(DiscordCoreAPI::PostApplicationCommandData dataPackage);

	DiscordCoreAPI_Dll string JSONIFY(DiscordCoreAPI::PatchApplicationCommandData dataPackage);

	DiscordCoreAPI_Dll string JSONIFY(DiscordCoreAPI::EditFollowUpMessageData dataPackage);

	DiscordCoreAPI_Dll string JSONIFY(DiscordCoreAPI::EditInteractionResponseData dataPackage);

	DiscordCoreAPI_Dll string JSONIFY(DiscordCoreAPI::CreateDeferredInteractionResponseData dataPackage);

	DiscordCoreAPI_Dll string JSONIFY(DiscordCoreAPI::CreateInteractionResponseData dataPackage);

	DiscordCoreAPI_Dll string JSONIFY(DiscordCoreAPI::CreateGuildRoleData dataPackage);

	DiscordCoreAPI_Dll string JSONIFY(DiscordCoreAPI::ModifyGuildRolePositionsData dataPackage);

	DiscordCoreAPI_Dll string JSONIFY(DiscordCoreAPI::EditChannelPermissionOverwritesData dataPackage);

	DiscordCoreAPI_Dll string JSONIFY(DiscordCoreAPI::CreateFollowUpMessageData dataPackage);

	DiscordCoreAPI_Dll string JSONIFY(DiscordCoreAPI::DeleteMessagesBulkData dataPackage);
	
	DiscordCoreAPI_Dll string JSONIFY(DiscordCoreAPI::CreateChannelInviteData dataPackage);

	DiscordCoreAPI_Dll string JSONIFY(DiscordCoreAPI::FollowNewsChannelData dataPackage);

	DiscordCoreAPI_Dll string JSONIFY(DiscordCoreAPI::AddRecipientToGroupDMData dataPackage);

	DiscordCoreAPI_Dll string JSONIFY(DiscordCoreAPI::StartThreadWithMessageData dataPackage);

	DiscordCoreAPI_Dll string JSONIFY(DiscordCoreAPI::StartThreadWithoutMessageData dataPackage);

	DiscordCoreAPI_Dll string JSONIFY(DiscordCoreAPI::CreateGuildEmojiData dataPackage);

	DiscordCoreAPI_Dll string JSONIFY(DiscordCoreAPI::ModifyGuildEmojiData dataPackage);

	DiscordCoreAPI_Dll string JSONIFY(DiscordCoreAPI::CreateGuildData dataPackage);

	DiscordCoreAPI_Dll string JSONIFY(DiscordCoreAPI::ModifyGuildData dataPackage);

	DiscordCoreAPI_Dll string JSONIFY(DiscordCoreAPI::CreateGuildChannelData dataPackage);

	DiscordCoreAPI_Dll string JSONIFY(DiscordCoreAPI::ModifyGuildChannelPositionsData dataPackage);

	DiscordCoreAPI_Dll string JSONIFY(DiscordCoreAPI::AddGuildMemberData dataPackage);

	DiscordCoreAPI_Dll string JSONIFY(DiscordCoreAPI::BeginGuildPruneData dataPackage);
}