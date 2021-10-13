// JSONifier.hpp - For all things related to JSON parsing etc.
// Apr 21, 2021
// Chris M.
// https://github.com/RealTimeChris

#pragma once

#ifndef _JSONIFIER_
#define _JSONIFIER_

#include "IndexInitial.hpp"
#include "FoundationEntities.hpp"

namespace DiscordCoreInternal {

	DiscordCoreAPI_Dll string getIdentifyPayload(string botToken, __int32 intents);

	DiscordCoreAPI_Dll string getAddBanPayload(PutGuildBanData dataPackage);

	DiscordCoreAPI_Dll string getIsSpeakingPayload(bool isSpeaking, __int32 ssrc, __int32 delay = 0);

	DiscordCoreAPI_Dll string getHeartbeatPayload(__int32 lastReceivedNumber);

	DiscordCoreAPI_Dll string getVoiceHeartbeatPayload(__int64 nonce);

	DiscordCoreAPI_Dll string getPresenceUpdatePayload(UpdatePresenceData dataPackage);

	DiscordCoreAPI_Dll string getVoiceStateUpdatePayload(UpdateVoiceStateData dataPackage);

	DiscordCoreAPI_Dll string getVoiceIdentifyPayload(DiscordCoreInternal::VoiceConnectionData dataPackage, VoiceConnectInitData dataPackage02);

	DiscordCoreAPI_Dll string getResumeVoicePayload(string serverId, string sessionId, string token);

	DiscordCoreAPI_Dll string getSelectProtocolPayload(string localPort, string localIp, string encryptionMode);

	DiscordCoreAPI_Dll string getModifyGuildMemberPayload(PatchGuildMemberData dataPackage);

	DiscordCoreAPI_Dll string getResumePayload(string botToken, string sessionID, __int32 lastReceivedNumber);

	DiscordCoreAPI_Dll string getModifyRolePayload(PatchRoleData dataPackage);

	DiscordCoreAPI_Dll string getSocketPath(string initialPayload);

	DiscordCoreAPI_Dll __int32 getHeartBeatInterval(hstring initialPayload);

	DiscordCoreAPI_Dll string getCreateMessagePayload(PostMessageData dataPackage);

	DiscordCoreAPI_Dll string getCreateMessagePayload(PostDMData dataPackage);

	DiscordCoreAPI_Dll string getEditMessagePayload(PatchMessageData dataPackage);

	DiscordCoreAPI_Dll void addOptionsData(ApplicationCommandOptionData dataPackage, json* pJSONData);

	DiscordCoreAPI_Dll string getEditApplicationCommandPermissionsPayload(PutEditApplicationCommandPermissionsData dataPackage);

	DiscordCoreAPI_Dll string getBatchEditApplicationCommandPermissionsPayload(PutBatchEditApplicationCommandPermissionsData dataPackage);

	DiscordCoreAPI_Dll string getCreateApplicationCommandPayload(PostApplicationCommandData dataPackage);

	DiscordCoreAPI_Dll string getEditApplicationCommandPayload(PatchApplicationCommandData dataPackage);

	DiscordCoreAPI_Dll string getEditFollowUpMessagePayload(PatchFollowUpMessageData dataPackage);

	DiscordCoreAPI_Dll string getEditInteractionResponsePayload(PatchInteractionResponseData dataPackage);

	DiscordCoreAPI_Dll string getCreateDeferredInteractionResponsePayload(PostDeferredInteractionResponseData dataPackage);

	DiscordCoreAPI_Dll string getCreateInteractionResponsePayload(PostInteractionResponseData dataPackage);

	DiscordCoreAPI_Dll string getCreateRolePayload(PostRoleData dataPackage);

	DiscordCoreAPI_Dll string getUpdateRolePositionsPayload(PatchRolePositionData dataPackage);

	DiscordCoreAPI_Dll string getEditChannelPermissionOverwritesPayload(PutPermissionOverwritesData dataPackage);

	DiscordCoreAPI_Dll string getPostFollowUpMessagePayload(PostFollowUpMessageData dataPackage);

	DiscordCoreAPI_Dll string getDeleteMessagesBulkPayload(DeleteMessagesBulkData dataPackage);
}
#endif