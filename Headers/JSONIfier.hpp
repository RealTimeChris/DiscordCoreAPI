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

	DiscordCoreAPI_Dll string getIdentifyPayload(string botToken, __int32 intents);

	DiscordCoreAPI_Dll string getAddBanPayload(DiscordCoreAPI::CreateGuildBanData dataPackage);

	DiscordCoreAPI_Dll string getIsSpeakingPayload(bool isSpeaking, __int32 ssrc, __int32 delay = 0);

	DiscordCoreAPI_Dll string getPatchChannelPayload(DiscordCoreAPI::ModifyChannelData channelData);

	DiscordCoreAPI_Dll string getHeartbeatPayload(__int32 lastReceivedNumber);

	DiscordCoreAPI_Dll string getVoiceHeartbeatPayload(__int64 nonce);

	DiscordCoreAPI_Dll string getPresenceUpdatePayload(UpdatePresenceData dataPackage);

	DiscordCoreAPI_Dll string getVoiceStateUpdatePayload(UpdateVoiceStateData dataPackage);

	DiscordCoreAPI_Dll string getVoiceIdentifyPayload(DiscordCoreInternal::VoiceConnectionData dataPackage, VoiceConnectInitData dataPackage02);

	DiscordCoreAPI_Dll string getResumeVoicePayload(string serverId, string sessionId, string token);

	DiscordCoreAPI_Dll string getSelectProtocolPayload(string localPort, string localIp, string encryptionMode);

	DiscordCoreAPI_Dll string getModifyGuildMemberPayload(DiscordCoreAPI::ModifyGuildMemberData dataPackage);

	DiscordCoreAPI_Dll string getResumePayload(string botToken, string sessionID, __int32 lastReceivedNumber);

	DiscordCoreAPI_Dll string getModifyRolePayload(DiscordCoreAPI::UpdateRoleData dataPackage);

	DiscordCoreAPI_Dll string getSocketPath(string initialPayload);

	DiscordCoreAPI_Dll __int32 getHeartBeatInterval(hstring initialPayload);

	DiscordCoreAPI_Dll string getCreateMessagePayload(DiscordCoreAPI::CreateMessageData dataPackage);

	DiscordCoreAPI_Dll string getCreateMessagePayload(DiscordCoreAPI::SendDMData dataPackage);

	DiscordCoreAPI_Dll string getEditMessagePayload(DiscordCoreAPI::EditMessageData dataPackage);

	DiscordCoreAPI_Dll void addOptionsData(ApplicationCommandOptionData dataPackage, json* pJSONData);

	DiscordCoreAPI_Dll string getEditApplicationCommandPermissionsPayload(PutEditApplicationCommandPermissionsData dataPackage);

	DiscordCoreAPI_Dll string getBatchEditApplicationCommandPermissionsPayload(PutBatchEditApplicationCommandPermissionsData dataPackage);

	DiscordCoreAPI_Dll string getCreateApplicationCommandPayload(PostApplicationCommandData dataPackage);

	DiscordCoreAPI_Dll string getEditApplicationCommandPayload(PatchApplicationCommandData dataPackage);

	DiscordCoreAPI_Dll string getEditFollowUpMessagePayload(DiscordCoreAPI::EditFollowUpMessageData dataPackage);

	DiscordCoreAPI_Dll string getEditInteractionResponsePayload(DiscordCoreAPI::EditInteractionResponseData dataPackage);

	DiscordCoreAPI_Dll string getCreateDeferredInteractionResponsePayload(DiscordCoreAPI::CreateDeferredInteractionResponseData dataPackage);

	DiscordCoreAPI_Dll string getCreateInteractionResponsePayload(DiscordCoreAPI::CreateInteractionResponseData dataPackage);

	DiscordCoreAPI_Dll string getCreateRolePayload(DiscordCoreAPI::CreateRoleData dataPackage);

	DiscordCoreAPI_Dll string getUpdateRolePositionsPayload(DiscordCoreAPI::UpdateRolePositionData dataPackage);

	DiscordCoreAPI_Dll string getEditChannelPermissionOverwritesPayload(DiscordCoreAPI::EditChannelPermissionOverwritesData dataPackage);

	DiscordCoreAPI_Dll string getPostFollowUpMessagePayload(DiscordCoreAPI::CreateFollowUpMessageData dataPackage);

	DiscordCoreAPI_Dll string getDeleteMessagesBulkPayload(DiscordCoreAPI::DeleteMessagesBulkData dataPackage);
}
#endif