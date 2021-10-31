// JSONifier.hpp - For all things related to JSON parsing etc.
// Apr 21, 2021
// Chris M.
// https://github.com/RealTimeChris

#pragma once

#ifndef _JSONIFIER_
#define _JSONIFIER_

#include "IndexInitial.hpp"
#include "FoundationEntities.hpp"
#include "ChannelEntities.hpp"

namespace DiscordCoreAPI {

	struct CreateDeferredInteractionResponseData;
	struct CreateInteractionResponseData;
	struct EditInteractionResponseData;
	struct CreateFollowUpMessageData;
	struct EditFollowUpMessageData;
	struct DeleteMessagesBulkData;
	struct UpdateRolePositionData;
	struct ModifyGuildMemberData;
	struct CreateGuildBanData;
	struct CreateMessageData;	
	struct EditMessageData;
	struct UpdateRoleData;
	struct CreateRoleData;
	struct SendDMData;	

};

namespace DiscordCoreInternal {

	DiscordCoreAPI_Dll string JSONIFY(string botToken, __int32 intents);

	DiscordCoreAPI_Dll string JSONIFY(DiscordCoreAPI::CreateGuildBanData dataPackage);

	DiscordCoreAPI_Dll string JSONIFY(bool isSpeaking, __int32 ssrc, __int32 delay = 0);

	DiscordCoreAPI_Dll string JSONIFY(DiscordCoreAPI::ModifyChannelData channelData);

	DiscordCoreAPI_Dll string JSONIFY(__int32 lastReceivedNumber);

	DiscordCoreAPI_Dll string JSONIFY(__int64 nonce);

	DiscordCoreAPI_Dll string JSONIFY(UpdatePresenceData dataPackage);

	DiscordCoreAPI_Dll string JSONIFY(UpdateVoiceStateData dataPackage);

	DiscordCoreAPI_Dll string JSONIFY(DiscordCoreInternal::VoiceConnectionData dataPackage, VoiceConnectInitData dataPackage02);

	DiscordCoreAPI_Dll string JSONIFY(string serverId, string sessionId, string token, string testString);

	DiscordCoreAPI_Dll string JSONIFY(string localPort, string localIp, string encryptionMode, __int32 testValue);

	DiscordCoreAPI_Dll string JSONIFY(DiscordCoreAPI::ModifyGuildMemberData dataPackage);

	DiscordCoreAPI_Dll string JSONIFY(string botToken, string sessionID, __int32 lastReceivedNumber);

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

	DiscordCoreAPI_Dll string JSONIFY(DiscordCoreAPI::CreateRoleData dataPackage);

	DiscordCoreAPI_Dll string JSONIFY(DiscordCoreAPI::UpdateRolePositionData dataPackage);

	DiscordCoreAPI_Dll string JSONIFY(DiscordCoreAPI::EditChannelPermissionOverwritesData dataPackage);

	DiscordCoreAPI_Dll string JSONIFY(DiscordCoreAPI::CreateFollowUpMessageData dataPackage);

	DiscordCoreAPI_Dll string JSONIFY(DiscordCoreAPI::DeleteMessagesBulkData dataPackage);
}
#endif