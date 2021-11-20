// JSONifier.hpp - For all things related to JSON parsing etc.
// Apr 21, 2021
// Chris M.
// https://github.com/RealTimeChris

#pragma once

#include "IndexInitial.hpp"
#include "ChannelEntities.hpp"

namespace DiscordCoreAPI {

	struct CreateDeferredInteractionResponseData;
	struct EditChannelPermissionOverwritesData;
	struct StartThreadWithoutMessageData;
	struct CreateInteractionResponseData;
	struct StartThreadWithoutMessageData;
	struct ModifyGuildRolePositionsData;
	struct EditInteractionResponseData;
	struct StartThreadWithMessageData;
	struct CreateFollowUpMessageData;
	struct AddRecipientToGroupDMData;
	struct EditFollowUpMessageData;
	struct CreateChannelInviteData;
	struct DeleteMessagesBulkData;
	struct FollowNewsChannelData;
	struct ModifyGuildMemberData;
	struct ModifyGuildEmojiData;
	struct CreateGuildEmojiData;
	struct CreateGuildRoleData;
	struct AddGuildMemberData;
	struct CreateGuildBanData;
	struct CreateMessageData;
	struct ModifyChannelData;
	struct ModifyGuildData;
	struct EditMessageData;
	struct CreateGuildData;
	struct UpdateRoleData;
	struct SendDMData;	

};

namespace DiscordCoreInternal {

	DiscordCoreAPI_Dll json JSONIFY(string botToken, __int32 intents);

	DiscordCoreAPI_Dll json JSONIFY(__int32 lastReceivedNumber);

	DiscordCoreAPI_Dll json JSONIFY(UpdatePresenceData dataPackage);

	DiscordCoreAPI_Dll json JSONIFY(UpdateVoiceStateData dataPackage);

	DiscordCoreAPI_Dll json JSONIFY(string serverId, string sessionId, string token, string testString);

	DiscordCoreAPI_Dll json JSONIFY(string botToken, string sessionID, __int32 lastReceivedNumber);

	DiscordCoreAPI_Dll string JSONIFY(__int64 nonce);

	DiscordCoreAPI_Dll string JSONIFY(string localPort, string localIp, string encryptionMode, __int32 testValue);

	DiscordCoreAPI_Dll string JSONIFY(DiscordCoreInternal::VoiceConnectionData dataPackage, VoiceConnectInitData dataPackage02);

	DiscordCoreAPI_Dll string JSONIFY(bool isSpeaking, __int32 ssrc, __int32 delay = 0);

	DiscordCoreAPI_Dll string JSONIFY(DiscordCoreAPI::CreateGuildBanData dataPackage);

	DiscordCoreAPI_Dll string JSONIFY(DiscordCoreAPI::ModifyChannelData channelData);

	DiscordCoreAPI_Dll string JSONIFY(DiscordCoreAPI::ModifyGuildMemberData dataPackage);

	DiscordCoreAPI_Dll string JSONIFY(DiscordCoreAPI::UpdateRoleData dataPackage);

	DiscordCoreAPI_Dll string JSONIFY(string initialPayload);

	DiscordCoreAPI_Dll __int32 JSONIFY(hstring initialPayload);

	DiscordCoreAPI_Dll string JSONIFY(DiscordCoreAPI::CreateMessageData dataPackage);

	DiscordCoreAPI_Dll string JSONIFY(DiscordCoreAPI::SendDMData dataPackage);

	DiscordCoreAPI_Dll string JSONIFY(DiscordCoreAPI::EditMessageData dataPackage);

	DiscordCoreAPI_Dll void JSONIFY(ApplicationCommandOptionData dataPackage, json* pJSONData);

	DiscordCoreAPI_Dll string JSONIFY(PutEditApplicationCommandPermissionsData dataPackage);

	DiscordCoreAPI_Dll string JSONIFY(PutBatchEditApplicationCommandPermissionsData dataPackage);

	DiscordCoreAPI_Dll string JSONIFY(PostApplicationCommandData dataPackage);

	DiscordCoreAPI_Dll string JSONIFY(PatchApplicationCommandData dataPackage);

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
	
}