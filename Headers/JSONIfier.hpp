// JSONifier.hpp - For all things related to JSON parsing etc.
// Apr 21, 2021
// Chris M.
// https://github.com/RealTimeChris

#pragma once

#include "IndexInitial.hpp"
#include "ChannelEntities.hpp"

namespace DiscordCoreAPI {

	struct BatchEditGuildApplicationCommandPermissionsData;
	struct EditGuildApplicationCommandPermissionsData;
	class CreateDeferredInteractionResponseData;
	struct EditChannelPermissionOverwritesData;
	struct CreateGlobalApplicationCommandData;
	struct CreateGuildApplicationCommandData;
	struct EditGlobalApplicationCommandData;
	struct EditGuildApplicationCommandData;
	struct StartThreadWithoutMessageData;
	struct CreateGuildScheduledEventData;
	struct ModifyGuildScheduledEventData;
	struct StartThreadWithoutMessageData;
	class CreateInteractionResponseData;
	struct ModifyGuildWelcomeScreenData;
	struct ModifyGuildRolePositionsData;
	struct EditApplicationCommandData;
	class EditInteractionResponseData;
	struct StartThreadWithMessageData;
	struct AddRecipientToGroupDMData;
	class CreateFollowUpMessageData;
	struct CreateChannelInviteData;
	struct DeleteMessagesBulkData;
	class EditFollowUpMessageData;
	class EditWebHookMessageData;
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
	class ExecuteWebHookData;
	struct ModifyChannelData;
	class CreateMessageData;
	struct ModifyGuildData;
	struct CreateGuildData;
	class EditMessageData;	

};

namespace DiscordCoreInternal {

	DiscordCoreAPI_Dll int32_t JSONIFY(hstring initialPayload);

	DiscordCoreAPI_Dll json JSONIFY(string botToken, int32_t intents);

	DiscordCoreAPI_Dll json JSONIFY(int32_t lastReceivedNumber);

	DiscordCoreAPI_Dll json JSONIFY(UpdatePresenceData dataPackage);

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