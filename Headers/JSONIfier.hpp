// JSONifier.hpp - For all things related to JSON parsing etc.
// Apr 21, 2021
// Chris M.
// https://github.com/RealTimeChris

#pragma once

#include "IndexInitial.hpp"
#include "FoundationEntities.hpp"

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
	struct DiscordCoreAPI_Dll ModifyGuildWelcomeScreenData;
	struct DiscordCoreAPI_Dll ModifyGuildRolePositionsData;
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

};

namespace DiscordCoreInternal {

	DiscordCoreAPI_Dll int32_t JSONIFY(string initialPayload);

	DiscordCoreAPI_Dll json JSONIFY(string botToken, int32_t intents);

	DiscordCoreAPI_Dll json JSONIFY(int32_t lastReceivedNumber);

	DiscordCoreAPI_Dll json JSONIFY(DiscordCoreAPI::UpdatePresenceData dataPackage);

	DiscordCoreAPI_Dll json JSONIFY(DiscordCoreAPI::UpdateVoiceStateData dataPackage);

	DiscordCoreAPI_Dll json JSONIFY(string serverId, string sessionId, string token, string testString);

	DiscordCoreAPI_Dll json JSONIFY(string botToken, string sessionID, int32_t lastReceivedNumber);

	DiscordCoreAPI_Dll vector<uint8_t> JSONIFY(int64_t nonce);

	DiscordCoreAPI_Dll vector<uint8_t> JSONIFY(string localPort, string localIp, string encryptionMode, int32_t testValue);

	DiscordCoreAPI_Dll vector<uint8_t> JSONIFY(DiscordCoreInternal::VoiceConnectionData dataPackage, VoiceConnectInitData dataPackage02);

	DiscordCoreAPI_Dll vector<uint8_t> JSONIFY(bool isSpeaking, int32_t ssrc, int32_t delay = 0);

	DiscordCoreAPI_Dll string JSONIFY(DiscordCoreAPI::CreateGuildBanData dataPackage);

	DiscordCoreAPI_Dll string JSONIFY(DiscordCoreAPI::ModifyChannelData channelData);

	DiscordCoreAPI_Dll string JSONIFY(DiscordCoreAPI::ModifyGuildMemberData dataPackage);

	DiscordCoreAPI_Dll string JSONIFY(DiscordCoreAPI::ModifyGuildRoleData dataPackage);

	DiscordCoreAPI_Dll string JSONIFY(DiscordCoreAPI::CreateMessageData dataPackage);

	DiscordCoreAPI_Dll string JSONIFY(DiscordCoreAPI::EditMessageData dataPackage);

	DiscordCoreAPI_Dll string JSONIFY(DiscordCoreAPI::EditGuildApplicationCommandPermissionsData dataPackage);

	DiscordCoreAPI_Dll string JSONIFY(DiscordCoreAPI::BatchEditGuildApplicationCommandPermissionsData dataPackage);

	DiscordCoreAPI_Dll string JSONIFY(DiscordCoreAPI::CreateGlobalApplicationCommandData dataPackage);

	DiscordCoreAPI_Dll string JSONIFY(DiscordCoreAPI::EditGlobalApplicationCommandData dataPackage);

	DiscordCoreAPI_Dll string JSONIFY(DiscordCoreAPI::CreateGuildApplicationCommandData dataPackage);

	DiscordCoreAPI_Dll string JSONIFY(DiscordCoreAPI::EditGuildApplicationCommandData dataPackage);

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

	DiscordCoreAPI_Dll string JSONIFY(DiscordCoreAPI::ModifyGuildWelcomeScreenData dataPackage);
	
	DiscordCoreAPI_Dll string JSONIFY(DiscordCoreAPI::CreateGuildScheduledEventData dataPackage);

	DiscordCoreAPI_Dll string JSONIFY(DiscordCoreAPI::ModifyGuildScheduledEventData dataPackage);

	DiscordCoreAPI_Dll string JSONIFY(DiscordCoreAPI::ExecuteWebHookData dataPackage);

	DiscordCoreAPI_Dll string JSONIFY(DiscordCoreAPI::EditWebHookMessageData dataPackage);
}