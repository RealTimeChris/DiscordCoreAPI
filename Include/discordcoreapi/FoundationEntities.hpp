/*
	DiscordCoreAPI, A bot library for Discord, written in C++, and featuring explicit multithreading through the usage of custom, asynchronous C++ CoRoutines.

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
/// FoundationEntities.hpp - Header for all of the Discord/Support API data
/// structures. May 12, 2021 Chris M.
/// https://discordcoreapi.com
/// \file FoundationEntities.hpp

#pragma once

#include <discordcoreapi/Utilities.hpp>

namespace DiscordCoreInternal {

	using Snowflake = uint64_t;

	struct DiscordCoreAPI_Dll VoiceConnectInitData {
		int32_t currentShard{};
		bool selfDeaf{ false };
		bool selfMute{ false };
		Snowflake channelId{};
		Snowflake guildId{};
		Snowflake userId{};
	};

	struct DiscordCoreAPI_Dll VoiceConnectionData {
		std::string voiceEncryptionMode{};
		std::string externalIp{};
		uint32_t audioSSRC{ 0 };
		std::string secretKey{};
		std::string voicePort{};
		std::string sessionId{};
		std::string endPoint{};
		std::string voiceIp{};
		std::string token{};
	};

	struct DiscordCoreAPI_Dll WebSocketResumeData {
		int32_t lastNumberReceived{};
		std::string sessionId{};
		std::string botToken{};

		operator nlohmann::json();
	};

	struct DiscordCoreAPI_Dll WebSocketIdentifyData {
		UpdatePresenceData presence{};
		int32_t largeThreshold{};
		int32_t numberOfShards{};
		int32_t currentShard{};
		std::string botToken{};
		int64_t intents{};

		operator nlohmann::json();
	};

	struct DiscordCoreAPI_Dll VoiceSocketProtocolPayloadData {
		std::string voiceEncryptionMode{};
		std::string externalIp{};
		std::string voicePort{};

		operator nlohmann::json();
	};

	struct DiscordCoreAPI_Dll VoiceIdentifyData {
		VoiceConnectInitData connectInitData{};
		VoiceConnectionData connectionData{};

		operator nlohmann::json();
	};

	enum class SendSpeakingType : int8_t {
		Microphone = 1 << 0,
		Soundshare = 1 << 1,
		Priority = 1 << 2,
		Priority_And_Voice = Microphone | Priority,
	};

	struct DiscordCoreAPI_Dll SendSpeakingData {
		SendSpeakingType type{};
		int32_t delay{};
		int32_t ssrc{};

		operator nlohmann::json();
	};

	enum class HttpsWorkloadClass : int8_t { Get = 0, Put = 1, Post = 2, Patch = 3, Delete = 4 };

	enum class PayloadType : int8_t { Application_Json = 1, Multipart_Form = 2 };

	enum class HttpsWorkloadType : uint8_t {
		Unset = 0,
		Get_Global_Application_Commands = 1,
		Post_Global_Application_Command = 2,
		Get_Global_Application_Command = 3,
		Patch_Global_Application_Command = 4,
		Delete_Global_Application_Command = 5,
		Bulk_Put_Global_Application_Commands = 6,
		Get_Guild_Application_Commands = 7,
		Post_Guild_Application_Command = 8,
		Get_Guild_Application_Command = 9,
		Patch_Guild_Application_Command = 10,
		Delete_Guild_Application_Command = 11,
		Bulk_Put_Guild_Application_Commands = 12,
		Get_Guild_Application_Commands_Permissions = 13,
		Get_Guild_Application_Command_Permissions = 14,
		Put_Guild_Application_Command_Permissions = 15,
		Batch_Put_Guild_Application_Command_Permissions = 16,
		Post_Interaction_Response = 17,
		Get_Interaction_Response = 18,
		Patch_Interaction_Response = 19,
		Delete_Interaction_Response = 20,
		Post_Followup_Message = 21,
		Get_Followup_Message = 22,
		Patch_Followup_Message = 23,
		Delete_Followup_Message = 24,
		Get_Guild_Audit_Logs = 25,
		Get_Channel = 26,
		Patch_Channel = 27,
		Delete_Channel = 28,
		Get_Messages = 29,
		Get_Message = 30,
		Post_Message = 31,
		Crosspost_Message = 32,
		Put_Reaction = 33,
		Delete_Own_Reaction = 34,
		Delete_User_Reaction = 35,
		Get_Reactions = 36,
		Delete_All_Reactions = 37,
		Delete_Reactions_By_Emoji = 38,
		Patch_Message = 39,
		Delete_Message = 40,
		Delete_Message_Old = 41,
		Bulk_Delete_Messages = 42,
		Put_Channel_Permission_Overwrites = 43,
		Get_Channel_Invites = 44,
		Post_Channel_Invite = 45,
		Delete_Channel_Permission_Overwrites = 46,
		Post_Follow_News_Channel = 47,
		Post_Trigger_Typing_Indicator = 48,
		Get_Pinned_Messages = 49,
		Put_Pin_Message = 50,
		Delete_Pin_Message = 51,
		Put_Recipient_To_Group_Dm = 52,
		Delete_Recipient_From_Group_Dm = 53,
		Post_Thread_With_Message = 54,
		Post_Thread_Without_Message = 55,
		Put_Self_In_Thread = 56,
		Put_Thread_Member = 57,
		Delete_Self_From_Thread = 58,
		Delete_Thread_Member = 59,
		Get_Thread_Member = 60,
		Get_Thread_Members = 61,
		Get_Active_Threads = 62,
		Get_Public_Archived_Threads = 63,
		Get_Private_Archived_Threads = 64,
		Get_Joined_Private_Archived_Threads = 65,
		Get_Emoji_List = 66,
		Get_Guild_Emoji = 67,
		Post_Guild_Emoji = 68,
		Patch_Guild_Emoji = 69,
		Delete_Guild_Emoji = 70,
		Post_Guild = 71,
		Get_Guild = 72,
		Get_Guild_Preview = 73,
		Patch_Guild = 74,
		Delete_Guild = 75,
		Get_Guild_Channels = 76,
		Post_Guild_Channel = 77,
		Patch_Guild_Channel_Positions = 78,
		Get_Guild_Active_Threads = 79,
		Get_Guild_Member = 80,
		Get_Guild_Members = 81,
		Get_Search_Guild_Members = 82,
		Put_Guild_Member = 83,
		Patch_Guild_Member = 84,
		Patch_Current_Guild_Member = 85,
		Put_Guild_Member_Role = 86,
		Delete_Guild_Member_Role = 87,
		Delete_Guild_Member = 88,
		Get_Guild_Bans = 89,
		Get_Guild_Ban = 90,
		Put_Guild_Ban = 91,
		Delete_Guild_Ban = 92,
		Get_Guild_Roles = 93,
		Post_Guild_Role = 94,
		Patch_Guild_Role_Positions = 95,
		Patch_Guild_Role = 96,
		Delete_Guild_Role = 97,
		Get_Guild_Prune_Count = 98,
		Post_Guild_Prune = 99,
		Get_Guild_Voice_Regions = 100,
		Get_Guild_Invites = 101,
		Get_Guild_Integrations = 102,
		Delete_Guild_Integration = 103,
		Get_Guild_Widget_Settings = 104,
		Patch_Guild_Widget = 105,
		Get_Guild_Widget = 106,
		Get_Vanity_Invite = 107,
		Get_Guild_Widget_Image = 108,
		Get_Guild_Welcome_Screen = 109,
		Patch_Guild_Welcome_Screen = 110,
		Patch_Current_User_Voice_State = 111,
		Patch_User_Voice_State = 112,
		Get_Guild_Scheduled_Events = 113,
		Post_Guild_Scheduled_Event = 114,
		Get_Guild_Scheduled_Event = 115,
		Patch_Guild_Scheduled_Event = 116,
		Delete_Guild_Scheduled_Event = 117,
		Get_Guild_Scheduled_Event_Users = 118,
		Get_Guild_Template = 119,
		Post_Guild_From_Guild_Template = 120,
		Get_Guild_Templates = 121,
		Post_Guild_Template = 122,
		Put_Guild_Template = 123,
		Patch_Guild_Template = 124,
		Delete_Guild_Template = 125,
		Get_Invite = 126,
		Delete_Invite = 127,
		Post_Stage_Instance = 128,
		Get_Stage_Instance = 129,
		Patch_Stage_Instance = 130,
		Delete_Stage_Instance = 131,
		Get_Sticker = 132,
		Get_Nitro_Sticker_Packs = 133,
		Get_Guild_Stickers = 134,
		Post_Guild_Sticker = 135,
		Patch_Guild_Sticker = 136,
		Delete_Guild_Sticker = 137,
		Get_Current_User = 138,
		Get_User = 139,
		Patch_Current_User = 140,
		Get_Current_User_Guilds = 141,
		Delete_Leave_Guild = 142,
		Post_Create_User_Dm = 143,
		Get_User_Connections = 144,
		Get_Voice_Regions = 145,
		Post_Webhook = 146,
		Get_Channel_Webhooks = 147,
		Get_Guild_Webhooks = 148,
		Get_Webhook = 149,
		Get_Webhook_With_Token = 150,
		Patch_Webhook = 151,
		Patch_Webhook_With_Token = 152,
		Delete_Webhook = 153,
		Delete_Webhook_With_Token = 154,
		Post_Execute_Webhook = 155,
		Get_Webhook_Message = 156,
		Patch_Webhook_Message = 157,
		Delete_Webhook_Message = 158,
		Get_Application_Info = 159,
		Get_Authorization_Info = 160,
		Get_Gateway_Bot = 161,
		Post_Thread_In_Forum_Channel = 162,
		Get_Auto_Moderation_Rules = 163,
		Get_Auto_Moderation_Rule = 164,
		Post_Auto_Moderation_Rule = 165,
		Patch_Auto_Moderation_Rule = 166,
		Delete_Auto_Moderation_Rule = 167,
		YouTubeGetSearchResults = 168,
		SoundCloudGetSearchResults = 169,
		LAST = 170
	};

	class DiscordCoreAPI_Dll HttpsWorkloadData {
	  public:
		friend class HttpsClient;

		static std::unordered_map<HttpsWorkloadType, std::unique_ptr<std::atomic_int64_t>> workloadIdsExternal;
		static std::unordered_map<HttpsWorkloadType, std::unique_ptr<std::atomic_int64_t>> workloadIdsInternal;
		static std::mutex accessMutex;

		mutable std::unordered_map<std::string, std::string> headersToInsert{};
		PayloadType payloadType{ PayloadType::Application_Json };
		std::atomic_int64_t thisWorkerId{ 0 };
		HttpsWorkloadClass workloadClass{};
		mutable std::string baseUrl{};
		std::string relativePath{};
		std::string callStack{};
		std::string content{};

		HttpsWorkloadData& operator=(const HttpsWorkloadData& other);

		HttpsWorkloadData(const HttpsWorkloadData& other);

		HttpsWorkloadData& operator=(HttpsWorkloadData& other);

		HttpsWorkloadData(HttpsWorkloadData& other);

		HttpsWorkloadData(DiscordCoreInternal::HttpsWorkloadType theType);

	  protected:
		static int64_t incrementAndGetWorkloadId(HttpsWorkloadType workloadType);

		HttpsWorkloadType workloadType{};
	};
}

/// The main namespace for this library. \brief The main namespace for this
/// library.
namespace DiscordCoreAPI {

	/**
	 * \addtogroup foundation_entities
	 * @{
	*/

	template<typename ObjectType> class DataParser {
	  public:
		virtual void parseObject(const nlohmann::json&, ObjectType*) = 0;

		virtual ~DataParser() = default;
	};

	/// For ids of DiscordEntities. \brief For ids of DiscordEntities.
	using Snowflake = uint64_t;

	struct DiscordCoreAPI_Dll ConnectionPackage {
		std::unordered_map<Snowflake, DiscordCoreAPI::UnboundedMessageBlock<DiscordCoreInternal::VoiceConnectionData>*> voiceConnectionDataBufferMap{};
		int32_t currentBaseSocketAgent{ 0 };
		int32_t currentReconnectTries{ 0 };
		int32_t currentShard{ 0 };
	};

	/// Timeout durations for the timeout command. \brief Timeout durations for the timeout command.
	enum class TimeoutDurations : int16_t {
		None = 0,///< None - remove timeout.
		Minute = 1,///< 1 Minute timeout.
		Five_Minutes = 5,///< 5 Minute timeout.
		Ten_Minutes = 10,///< 10 Minute timeout.
		Hour = 60,///< 1 Hour timeout.
		Day = 1440,///< 1 Day timeout.
		Week = 10080///< 1 Week timeout.
	};

	/// Base class for all Discord entities. \brief Base class for all Discord entities.
	class DiscordCoreAPI_Dll DiscordEntity {
	  public:
		Snowflake id{};///< The identifier "snowflake" of the given entity.
		/// Converts the snowflake-id into a time and date stamp. \brief Converts the
		/// snowflake-id into a time and date stamp. \returns std::string A
		/// std::string containing the timestamp.
		std::string getCreatedAtTimestamp(TimeFormat timeFormat);

		virtual ~DiscordEntity() = default;
	};

	/// Role tags data. \brief Role tags data.
	struct DiscordCoreAPI_Dll RoleTagsData : public DataParser<RoleTagsData> {
		std::string premiumSubscriber{};///< Are they a premium subscriber?
		std::string integrationId{};///< What is the integration id?
		std::string botId{};///< What is the bot id?

		RoleTagsData() = default;

		RoleTagsData& operator=(const nlohmann::json& jsonObjectData);

		RoleTagsData(const nlohmann::json& jsonObjectData);

		virtual ~RoleTagsData() = default;

	  protected:
		void parseObject(const nlohmann::json& jsonObjectData, RoleTagsData* pDataStructure);
	};

	/// User flags. \brief User flags.
	enum class UserFlags : int32_t {
		Staff = 1 << 0,///< Discord Employee.
		Partner = 1 << 1,///< Partnered Server Owner.
		Hypesquad = 1 << 2,///< HypeSquad Events Member.
		Bug_Hunter_Level_1 = 1 << 3,///< Bug Hunter Level 1.
		Hypesquad_Online_House_1 = 1 << 6,///< House Bravery Member.
		Hypesquad_Online_House_2 = 1 << 7,///< House Brilliance Member.
		Hypesquad_Online_House_3 = 1 << 8,///< House Balance Member.
		Premium_Early_Suppoerter = 1 << 9,///< Early Nitro Supporter.
		Team_Pseudo_User = 1 << 10,///< User is a team.
		Bug_Hunter_Level_2 = 1 << 14,///< Bug Hunter Level 2.
		Verified_Bot = 1 << 16,///< Verified Bot.
		Verified_Developer = 1 << 17,///< Early Verified Bot Developer.
		Certified_Moderator = 1 << 18,///< Discord Certified Moderator.
		Bot_Http_Interactions = 1 << 19,///< Bot uses only HTTP interactions and is shown in the online member list.
		Bot = 1 << 20,///< Is it a bot?
		MFAEnabled = 1 << 21,///< Is MFA enabled?
		System = 1 << 22,///< Is it a system integration?
		Verified = 1 << 23///< Is it verified?
	};

	/// Premium types denote the level of premium a user has. \brief Premium types denote the level of premium a user has.
	enum class PremiumType : int8_t {
		None = 0,///< None.
		Nitro_Classic = 1,///< Nitro classic.
		Nitro = 2///< Nitro.
	};

	/// Data structure representing a single User. \brief Data structure representing a single User.
	class DiscordCoreAPI_Dll UserData : public DiscordEntity, public DataParser<UserData> {
	  public:
		StringWrapper discriminator{};///< The user's 4-digit discord-tag	identify.
		StringWrapper userName{};///< The user's userName, not unique across the platform	identify.
		StringWrapper avatar{};///< The user's avatar hash.
		int32_t flags{};///< The public flags on a user' s account.

		UserData() = default;

		UserData& operator=(const nlohmann::json& jsonObjectData);

		UserData(const nlohmann::json& jsonObjectData);

		virtual ~UserData() = default;

	  protected:
		void parseObject(const nlohmann::json& jsonObjectData, UserData* pDataStructure);
	};

	/// Attachment data. \brief Attachment data.
	class DiscordCoreAPI_Dll AttachmentData : public DiscordEntity, public DataParser<AttachmentData> {
	  public:
		std::string contentType{};///< Type of content for the attachment.
		std::string description{};///< A description of the attachment.
		bool ephemeral{ false };///< Whether it was an ephemeral response.
		std::string filename{};///< The file name of the attachment.
		std::string proxyUrl{};///< The proxy url for the attachment.
		int32_t height{ 0 };///< The height of the attachment.
		int32_t width{ 0 };///< The width of the attachment.
		int32_t size{ 0 };///< The size of the attachment.
		std::string url{};///< The url for the attachment.

		AttachmentData() = default;

		operator nlohmann::json();

		AttachmentData& operator=(const nlohmann::json& jsonObjectData);

		AttachmentData(const nlohmann::json& jsonObjectData);

		virtual ~AttachmentData() = default;

	  protected:
		void parseObject(const nlohmann::json& jsonObjectData, AttachmentData* pDataStructure);
	};

	/// Sticker format types. \brief Sticker format types.
	enum class StickerFormatType : int8_t {
		Png = 1,///< Png.
		Apng = 2,///< Apng.
		Lottie = 3///< Lottie
	};

	/// Embed footer data. \brief Embed footer data.
	struct DiscordCoreAPI_Dll EmbedFooterData : public DataParser<EmbedFooterData> {
		std::string proxyIconUrl{};///< Proxy icon url.
		std::string iconUrl{};///< Icon url.
		std::string text{};///< Footer text.
		EmbedFooterData() = default;

		EmbedFooterData& operator=(const nlohmann::json& jsonObjectData);

		EmbedFooterData(const nlohmann::json& jsonObjectData);

		virtual ~EmbedFooterData() = default;

	  protected:
		void parseObject(const nlohmann::json& jsonObjectData, EmbedFooterData* pDataStructure);
	};

	/// Embed image data. \brief Embed image data.
	struct DiscordCoreAPI_Dll EmbedImageData : public DataParser<EmbedImageData> {
		std::string proxyUrl{};///< Proxy url.
		int32_t height{ 0 };///< Image height.
		int32_t width{ 0 };///< Image width.
		std::string url{};///< Image url.

		EmbedImageData() = default;

		EmbedImageData& operator=(const nlohmann::json& jsonObjectData);

		EmbedImageData(const nlohmann::json& jsonObjectData);

		virtual ~EmbedImageData() = default;

	  protected:
		void parseObject(const nlohmann::json& jsonObjectData, EmbedImageData* pDataStructure);
	};

	/// Embed thumbnail data. \brief Embed thumbnail data.
	struct DiscordCoreAPI_Dll EmbedThumbnailData : public DataParser<EmbedThumbnailData> {
		std::string proxyUrl{};///< Proxy url.
		int32_t height{ 0 };///< Image height.
		int32_t width{ 0 };///< Image width.
		std::string url{};///< Image url.

		EmbedThumbnailData() = default;

		EmbedThumbnailData& operator=(const nlohmann::json& jsonObjectData);

		EmbedThumbnailData(const nlohmann::json& jsonObjectData);

		virtual ~EmbedThumbnailData() = default;

	  protected:
		void parseObject(const nlohmann::json& jsonObjectData, EmbedThumbnailData* pDataStructure);
	};

	/// Embed video data. \brief Embed video data.
	struct DiscordCoreAPI_Dll EmbedVideoData : public DataParser<EmbedVideoData> {
		std::string proxyUrl{};///< Proxy url.
		int32_t height{ 0 };///< Image height.
		int32_t width{ 0 };///< Image width.
		std::string url{};///< Image url.

		EmbedVideoData() = default;

		EmbedVideoData& operator=(const nlohmann::json& jsonObjectData);

		EmbedVideoData(const nlohmann::json& jsonObjectData);

		virtual ~EmbedVideoData() = default;

	  protected:
		void parseObject(const nlohmann::json& jsonObjectData, EmbedVideoData* pDataStructure);
	};

	/// Embed provider data. \brief Embed provider data.
	struct DiscordCoreAPI_Dll EmbedProviderData : public DataParser<EmbedProviderData> {
		std::string name{};///< Name.
		std::string url{};///< Url.

		EmbedProviderData() = default;

		EmbedProviderData& operator=(const nlohmann::json& jsonObjectData);

		EmbedProviderData(const nlohmann::json& jsonObjectData);

		virtual ~EmbedProviderData() = default;

	  protected:
		void parseObject(const nlohmann::json& jsonObjectData, EmbedProviderData* pDataStructure);
	};

	/// Embed author data. \brief Embed author data.
	struct DiscordCoreAPI_Dll EmbedAuthorData : public DataParser<EmbedAuthorData> {
		std::string proxyIconUrl{};///< Proxy icon url.
		std::string iconUrl{};///< Icon url.
		std::string name{};///< Name.
		std::string url{};///< Url.

		EmbedAuthorData() = default;

		EmbedAuthorData& operator=(const nlohmann::json& jsonObjectData);

		EmbedAuthorData(const nlohmann::json& jsonObjectData);

		virtual ~EmbedAuthorData() = default;

	  protected:
		void parseObject(const nlohmann::json& jsonObjectData, EmbedAuthorData* pDataStructure);
	};

	/// Embed field data. \brief Embed field data.
	struct DiscordCoreAPI_Dll EmbedFieldData : public DataParser<EmbedFieldData> {
		bool Inline{ false };///< Is the field inline with the rest of them?
		std::string value{};///< The text on the field.
		std::string name{};///< The title of the field.

		EmbedFieldData() = default;

		operator nlohmann::json();

		EmbedFieldData& operator=(const nlohmann::json& jsonObjectData);

		EmbedFieldData(const nlohmann::json& jsonObjectData);

		virtual ~EmbedFieldData() = default;

	  protected:
		void parseObject(const nlohmann::json& jsonObjectData, EmbedFieldData* pDataStructure);
	};

	/// Embed types. \brief Embed types.
	enum class EmbedType : int8_t {
		Rich = 0,///< Rich.
		Image = 1,///< Image.
		Video = 2,///< Video.
		Gifv = 3,///< Gifv.
		Article = 4,///< Article.
		link = 5///< Link.
	};

	/// Embed data. \brief Embed data.
	class DiscordCoreAPI_Dll EmbedData : public DataParser<EmbedData> {
	  public:
		std::string hexColorValue{ "000000" };///< Hex color value of the embed.
		std::vector<EmbedFieldData> fields{};///< Array of embed fields.
		EmbedThumbnailData thumbnail{};///< Embed thumbnail data.
		EmbedProviderData provider{};///< Embed provider data.
		std::string description{};///< Description of the embed.
		EmbedFooterData footer{};///< Embed footer data.
		EmbedAuthorData author{};///< Embed author data.
		std::string timestamp{};///< Timestamp to be placed on the embed.
		EmbedImageData image{};///< Embed image data.
		EmbedVideoData video{};///< Embed video data.
		std::string title{};///< Title of the embed.
		std::string type{};///< Type of the embed.
		std::string url{};///< Url for the embed.

		EmbedData() = default;

		operator nlohmann::json();

		EmbedData& operator=(const nlohmann::json& jsonObjectData);

		EmbedData(const nlohmann::json& jsonObjectData);

		/// Sets the author's name and avatar for the embed. \brief Sets the author's name and avatar for the embed.
		/// \param authorName The author's name.
		/// \param authorAvatarUrl The url to their avatar.
		/// \returns EmbedData& A reference to this embed.
		EmbedData& setAuthor(const std::string& authorName, const std::string& authorAvatarUrl = "");

		/// Sets the footer's values for the embed. \brief Sets the footer's values for the embed.
		/// \param footerText The footer's text.
		/// \param footerIconUrlText Url to the footer's icon.
		/// \returns EmbedData& A reference to this embed.
		EmbedData& setFooter(const std::string& footerText, const std::string& footerIconUrlText = "");

		/// Sets the timestamp on the embed. \brief Sets the timestamp on the embed.
		/// \param timeStamp The timestamp to be set.
		/// \returns EmbedData& A reference to this embed.
		EmbedData& setTimeStamp(const std::string& timeStamp);

		/// Adds a field to the embed. \brief Adds a field to the embed.
		/// \param name The title of the embed field.
		/// \param value The contents of the embed field.
		/// \param Inline Is it inline with the rest of the fields on the embed?
		/// \returns EmbedData& A reference to this embed.
		EmbedData& addField(const std::string& name, const std::string& value, bool Inline = true);

		/// Sets the description (the main contents) of the embed. \brief Sets the description (the main contents) of the embed.
		/// \param descriptionNew The contents of the description to set.
		/// \returns EmbedData& A reference to this embed.
		EmbedData& setDescription(const std::string& descriptionNew);

		/// Sets the color of the embed, by applying a hex-color value. \brief Sets the color of the embed, by applying a hex-color value.
		/// \param hexColorValueNew A string containing a hex-number value (Between 0x00 0xFFFFFF).
		/// \returns EmbedData& A reference to this embed.
		EmbedData& setColor(const std::string& hexColorValueNew);

		/// Sets the thumbnail of the embed. \brief Sets the thumbnail of the embed.
		/// \param thumbnailUrl The url to the thumbnail to be used.
		/// \returns EmbedData& A reference to this embed.
		EmbedData& setThumbnail(const std::string& thumbnailUrl);

		/// Sets the title of the embed. \brief Sets the title of the embed.
		/// \param titleNew A string containing the desired title.
		/// \returns EmbedData& A reference to this embed.
		EmbedData& setTitle(const std::string& titleNew);

		/// Sets the image of the embed. \brief Sets the image of the embed.
		/// \param imageUrl The url of the image to be set on the embed.
		/// \returns EmbedData& A reference to this embed.
		EmbedData& setImage(const std::string& imageUrl);

		virtual ~EmbedData() = default;

	  protected:
		void parseObject(const nlohmann::json& jsonObjectData, EmbedData* pDataStructure);
	};

	/// Message reference data.\brief Message reference data.
	struct DiscordCoreAPI_Dll MessageReferenceData : public DataParser<MessageReferenceData> {
		bool failIfNotExists{ false };///< Fail if the Message doesn't exist?
		Snowflake messageId{};///< Id of the Message to reference.
		Snowflake channelId{};///< Id of the Channel that the referenced Message was sent in.
		Snowflake guildId{};///< Id of the Guild that the referenced Message was sent in.

		MessageReferenceData() = default;

		operator nlohmann::json();

		MessageReferenceData& operator=(const nlohmann::json& jsonObjectData);

		MessageReferenceData(const nlohmann::json& jsonObjectData);

		virtual ~MessageReferenceData() = default;

	  protected:
		void parseObject(const nlohmann::json& jsonObjectData, MessageReferenceData* pDataStructure);
	};

	enum class MediaType : int8_t { png = 0, gif = 1, jpeg = 2, mpeg = 3, mp3 = 4 };

	/// Data representing a file to be sent via multipart-form data. \brief Data representing a file to be sent via multipart-form data.
	struct DiscordCoreAPI_Dll File {
		std::string fileName{};///< The name of the file.
		std::string data{};///< The data of the file.
	};

	/// Channel types. \brief Channel types.
	enum class ChannelType : int8_t {
		Guild_Text = 0,///< Guild text.
		Dm = 1,///< Direct-Message.
		Guild_Voice = 2,/// Guild voice.
		Group_Dm = 3,///< Group direct-Message.
		Guild_Category = 4,///< Guild category.
		Guild_News = 5,///< Guild news.
		Guild_Store = 6,///< Guild store.
		Guild_News_Thread = 10,///< Guild news Thread.
		Guild_Public_Thread = 11,///< Guild public Thread.
		Guild_Private_Thread = 12,///< Guild private Thread.
		Guild_Stage_Voice = 13,///< Guild stage-voice.
		Guild_Directory = 14,///< The channel in a hub containing the listed servers.
		Guild_Forum = 15///< A channel that can only contain threads.
	};

	/// Meta data for a Thread type of Channel. \brief Meta data for a Thread type of Channel.
	struct DiscordCoreAPI_Dll ThreadMetadataData : public DataParser<ThreadMetadataData> {
		TimeStamp<std::chrono::milliseconds> archiveTimestamp{};///< (Where applicable) the time at which this Thread was archived.
		int32_t autoArchiveDuration{ 0 };///< How int64_t before archiving this Thread.
		bool invitable{ false };///< The id of the individual who archived this Thread.
		bool archived{ false };///< Whether or not this Thread is currently archived.
		bool locked{ false };///< Whether or not this Thread is currently locked.

		ThreadMetadataData() = default;

		ThreadMetadataData& operator=(const nlohmann::json& jsonObjectData);

		ThreadMetadataData(const nlohmann::json& other);

		virtual ~ThreadMetadataData() = default;

	  protected:
		void parseObject(const nlohmann::json& jsonObjectData, ThreadMetadataData* pDataStructure);
	};

	/// Data for a single member of a Thread. \brief Data for a single member of a Thread.
	class DiscordCoreAPI_Dll ThreadMemberData : public DiscordEntity, public DataParser<ThreadMemberData> {
	  public:
		TimeStamp<std::chrono::milliseconds> joinTimestamp{};///< The time at which the member joined this Thread.
		int32_t flags{ 0 };///< Flags.
		Snowflake userId{};///< The User's id.

		ThreadMemberData() = default;

		ThreadMemberData& operator=(const nlohmann::json& jsonObjectData);

		ThreadMemberData(const nlohmann::json& other);

		virtual ~ThreadMemberData() = default;

	  protected:
		void parseObject(const nlohmann::json& jsonObjectData, ThreadMemberData* pDataStructure);
	};

	class DiscordCoreAPI_Dll ThreadMemberDataVector : public DataParser<ThreadMemberDataVector> {
	  public:
		ThreadMemberDataVector() = default;

		operator std::vector<ThreadMemberData>();

		ThreadMemberDataVector& operator=(const nlohmann::json& jsonObjectData);

		ThreadMemberDataVector(const nlohmann::json& jsonObjectData);

		virtual ~ThreadMemberDataVector() = default;

	  protected:
		std::vector<ThreadMemberData> theThreadMemberDatas{};

		void parseObject(const nlohmann::json& jsonObjectData, ThreadMemberDataVector* pDataStructure);
	};

	/// Thread types. \brief Thread types.
	enum class ThreadType : int8_t {
		Guild_News_Thread = 10,///< Guild news Thread.
		Guild_Public_Thread = 11,///< Guild public Thread.
		Guild_Private_Thread = 12///< Guild private Thread.
	};

	/// Automatic Thread archiving durations. \brief Automatic Thread archiving durations.
	enum class ThreadAutoArchiveDuration : int16_t {
		Shortest = 60,///< Shortest.
		Short = 1440,///< Short.
		Long = 4320,///< Long.
		Longest = 10080///< Longest.
	};

	enum class GuildMemberFlags : int8_t { Pending = 1 << 0, Deaf = 1 << 1, Mute = 1 << 2 };

	struct DiscordCoreAPI_Dll GuildMemberId {
		GuildMemberId() = default;
		Snowflake guildMemberId{};
		Snowflake guildId{};
	};

	bool operator==(const GuildMemberId lhs, const GuildMemberId rhs);

	bool operator<(const GuildMemberId& lhs, const GuildMemberId& rhs);

	/// Data structure representing a single GuildMember. \brief Data structure representing a single GuildMember.
	/// Data structure representing a single Guild. \brief Data structure representing a single Guild.
	class DiscordCoreAPI_Dll GuildMemberData : public DiscordEntity, public DataParser<GuildMemberData> {
	  public:
		TimeStamp<std::chrono::milliseconds> joinedAt{};///< When they joined the Guild.
		std::vector<Snowflake> roles{};///< The Guild roles that they have.
		Permissions permissions{};///< Their base-level Permissions in the Guild.
		StringWrapper userAvatar{};///< This GuildMember's User Avatar.
		StringWrapper userName{};///< This GuildMember's UserName.
		Snowflake guildId{};///< The current Guild's id.
		StringWrapper nick{};///< Their nick/display name.
		int8_t flags{ 0 };///< GuildMember flags.

		GuildMemberData() = default;

		GuildMemberData& operator=(const nlohmann::json& jsonObjectData);

		GuildMemberData(const nlohmann::json& jsonObjectData);

		void insertUser(UserData theUser);

		virtual ~GuildMemberData() = default;

	  protected:
		void parseObject(const nlohmann::json& jsonObjectData, GuildMemberData* pDataStructure);
	};

	/// Voice state data. \brief Voice state data.
	struct DiscordCoreAPI_Dll VoiceStateData : public DataParser<VoiceStateData> {
		TimeStamp<std::chrono::milliseconds> requestToSpeakTimestamp{ "" };///< The time at which the User requested to speak.
		GuildMemberData member{};///< The Guild member id this voice state is for.
		StringWrapper sessionId{};///< The session id for this voice state.
		bool selfStream{ false };///< Whether this User is streaming using "Go Live".
		bool selfVideo{ false };///< Whether this User's camera is enabled.
		bool selfDeaf{ false };///< Whether this User is locally deafened.
		bool selfMute{ false };///< Whether this User is locally muted.
		bool suppress{ false };///< Whether this User is muted by the current User.
		Snowflake channelId{};///< The Channel id this User is connected to.
		bool deaf{ false };///< Whether this User is deafened by the server.
		bool mute{ false };///< Whether this User is muted by the server.
		Snowflake guildId{};///< The Guild id this voice state is for.
		Snowflake userId{};///< The User id this voice state is for.

		VoiceStateData() = default;

		VoiceStateData& operator=(const nlohmann::json& jsonObjectData);

		VoiceStateData(const nlohmann::json& jsonObjectData);

		virtual ~VoiceStateData() = default;

	  protected:
		void parseObject(const nlohmann::json& jsonObjectData, VoiceStateData* pDataStructure);
	};

	/// Permission overwrites types. \brief Permission overwrites types.
	enum class PermissionOverwritesType : int8_t {
		Role = 0,///< Role.
		User = 1///< User.
	};

	/// A Permission overwrite, for a given Channel. \brief A Permission overwrite, for a given Channel.
	class DiscordCoreAPI_Dll OverWriteData : public DiscordEntity, public DataParser<OverWriteData> {
	  public:
		PermissionOverwritesType type{};///< Role or User type.
		Snowflake channelId{};///< Channel id for which Channel this overwrite belongs to.
		Permissions allow{};///< Collection of Permissions to allow.
		Permissions deny{};///< Collection of Permissions to deny.

		OverWriteData() = default;

		OverWriteData& operator=(const nlohmann::json& jsonObjectData);

		OverWriteData(const nlohmann::json& jsonObjectData);

		virtual ~OverWriteData() = default;

	  protected:
		void parseObject(const nlohmann::json& jsonObjectData, OverWriteData* pDataStructure);
	};

	enum class ChannelFlags : int8_t { NSFW = 1 << 0 };

	/// Data structure representing a single Channel. \brief Data structure representing a single Channel.
	class DiscordCoreAPI_Dll ChannelData : public DiscordEntity, public DataParser<ChannelData> {
	  public:
		std::unordered_map<Snowflake, OverWriteData> permissionOverwrites{};///< Permission overwrites for the given Channel.
		ChannelType type{ ChannelType::Dm };///< The type of the Channel.
		int32_t memberCount{ 0 };///< Count of members active in the Channel.
		int32_t position{ 0 };///< The position of the Channel, in the Guild's Channel list.
		StringWrapper name{};///< Name of the Channel.
		Snowflake parentId{};///< Id of the Channel's parent Channel/category.
		Snowflake ownerId{};///< Id of the Channel's owner.
		Snowflake guildId{};///< Id of the Channel's Guild, if applicable.
		int8_t flags{ 0 };///< Channel flags combined as a bitfield.

		ChannelData() = default;

		ChannelData& operator=(const nlohmann::json& jsonObjectData);

		ChannelData(const nlohmann::json& other);

		virtual ~ChannelData() = default;

	  protected:
		void parseObject(const nlohmann::json& jsonObjectData, ChannelData* pDataStructure);
	};

	/// Data representing an active Thread. \brief Data representing an active Thread.
	struct DiscordCoreAPI_Dll ActiveThreadsData : public DataParser<ActiveThreadsData> {
		std::vector<ThreadMemberData> members{};
		std::vector<ChannelData> threads{};
		bool hasMore{ false };

		ActiveThreadsData() = default;

		ActiveThreadsData& operator=(const nlohmann::json& jsonObjectData);

		ActiveThreadsData(const nlohmann::json& jsonObjectData);

		virtual ~ActiveThreadsData() = default;

	  protected:
		void parseObject(const nlohmann::json& jsonObjectData, ActiveThreadsData* pDataStructure);
	};

	/// Data representing an archived Thread. \brief Data representing an archived Thread.
	struct DiscordCoreAPI_Dll ArchivedThreadsData : public DataParser<ArchivedThreadsData> {
		std::vector<ThreadMemberData> members{};
		std::vector<ChannelData> threads{};
		bool hasMore{ false };

		ArchivedThreadsData() = default;

		ArchivedThreadsData& operator=(const nlohmann::json& jsonObjectData);

		ArchivedThreadsData(const nlohmann::json& jsonObjectData);

		virtual ~ArchivedThreadsData() = default;

	  protected:
		void parseObject(const nlohmann::json& jsonObjectData, ArchivedThreadsData* pDataStructure);
	};

	enum class RoleFlags : int8_t { Mentionable = 1 << 0, Managed = 1 << 1, Hoist = 1 << 2 };

	/// Data structure representing a single Role. \brief Data structure representing a single Role.
	class DiscordCoreAPI_Dll RoleData : public DiscordEntity, public DataParser<RoleData> {
	  public:
		StringWrapper unicodeEmoji{};///< Emoji representing the Role.
		Permissions permissions{};///< The Role's base Guild Permissions.
		int32_t position{ 0 };///< Its position amongst the rest of the Guild's roles.
		StringWrapper name{};///< The Role's name.
		int32_t color{ 0 };///< The Role's color.
		int8_t flags{ 0 };///< Role flags.

		RoleData() = default;

		RoleData& operator=(const nlohmann::json& jsonObjectData);

		RoleData(const nlohmann::json& jsonObjectData);

		virtual ~RoleData() = default;

	  protected:
		void parseObject(const nlohmann::json& jsonObjectData, RoleData* pDataStructure);
	};

	/// Application command-option types. \brief Application command-option types.
	enum class ApplicationCommandOptionType : int8_t {
		Sub_Command = 1,///< Sub-command.
		Sub_Command_Group = 2,///< Sub-command group.
		String = 3,///< String.
		Integer = 4,///< Integer.
		Boolean = 5,///< Boolean.
		User = 6,///< User.
		Channel = 7,///< Channel.
		Role = 8,///< Role.
		Mentionable = 9,///< Mentionable.
		Number = 10,///< Number.
		Attachment = 11///< Attachment.
	};

	/// Application command permission-types. \brief Application command permission-types.
	enum class ApplicationCommandPermissionType : int8_t {
		Role = 1,///< Role.
		User = 2,///< User.
		Channel = 3///< Channel.
	};

	/// Event types for auto-moderation. \brief Event types for auto-moderation.
	enum class EventType : int8_t {
		Message_Send = 1,///< When a member sends or edits a message in the guild.
	};

	/// Trigger types for auto-moderation. \brief Trigger types for auto-moderation.
	enum class TriggerType : int8_t {
		Keyword = 1,///< Check if content contains words from a user defined list of keywords.
		Harmful_Link = 2,///< Check if content contains any harmful links.
		Spam = 3,///< Check if content represents generic spam.
		Keyword_Preset = 4///< Check if content contains words from internal pre-defined wordsets.
	};

	/// Keyword preset types for auto-moderation. \brief Keyword preset types for auto-moderation.
	enum class KeywordPresetType : int8_t {
		Profanity = 1,///< Words that may be considered forms of swearing or cursing.
		Sexual_Content = 2,///< Words that refer to sexually explicit behavior or activity
		Slurs = 3///< Personal insults or words that may be considered hate speech.
	};

	/// Action types for auto-moderation. \brief Action types for auto-moderation.
	enum class ActionType : int8_t {
		Block_Message = 1,///< Blocks the content of a message according to the rule.
		Send_Alert_Message = 2,///< Logs user content to a specified channel.
		Timeout = 3///< Timeout user for a specified duration.
	};

	/// Action metadata for auto-moderation-rules. \brief Action metadata for auto-moderation-rules.
	struct DiscordCoreAPI_Dll ActionMetaData : public DataParser<ActionMetaData> {
		Snowflake channelId{};///< Channel to which user content should be logged.
		int64_t durationSeconds{};///< Timeout duration in seconds.

		ActionMetaData() = default;

		ActionMetaData& operator=(const nlohmann::json& jsonObjectData);

		ActionMetaData(const nlohmann::json& jsonObjectData);

		virtual ~ActionMetaData() = default;

	  protected:
		void parseObject(const nlohmann::json& jsonObjectData, ActionMetaData* pDataStructure);
	};

	/// Trigger metadata for auto-moderation-rules. \brief Trigger metadata for auto-moderation-rules.
	struct DiscordCoreAPI_Dll TriggerMetaData : public DataParser<TriggerMetaData> {
		std::vector<std::string> keywordFilter{};///< Substrings which will be searched for in content.
		std::vector<KeywordPresetType> presets{};///< The internally pre-defined wordsets which will be searched for in content.

		TriggerMetaData() = default;

		TriggerMetaData& operator=(const nlohmann::json& jsonObjectData);

		TriggerMetaData(const nlohmann::json& jsonObjectData);

		virtual ~TriggerMetaData() = default;

	  protected:
		void parseObject(const nlohmann::json& jsonObjectData, TriggerMetaData* pDataStructure);
	};

	/// For representing a single auto-moderation-rule-action. \brief For representing a single auto-moderation-rule-action.
	struct DiscordCoreAPI_Dll ActionData : public DataParser<ActionData> {
		ActionType type{};///< The type of action.
		ActionMetaData metadata{};///< Additional metadata needed during execution for this specific action type.

		ActionData() = default;

		ActionData& operator=(const nlohmann::json& jsonObjectData);

		ActionData(const nlohmann::json& jsonObjectData);

		virtual ~ActionData() = default;

	  protected:
		void parseObject(const nlohmann::json& jsonObjectData, ActionData* pDataStructure);
	};

	/// Represents an auto-moderation-rule. \brief Represents an auto-moderation-rule.
	class DiscordCoreAPI_Dll AutoModerationRuleData : public DiscordEntity, public DataParser<AutoModerationRuleData> {
	  public:
		std::vector<Snowflake> exemptChannels{};///< The channel ids that should not be affected by the rule(Maximum of 50).
		std::vector<Snowflake> exemptRoles{};///< The role ids that should not be affected by the rule(Maximum of 20).
		std::vector<ActionData> actions{};///< Actions which will execute when the rule is triggered.
		TriggerMetaData triggerMetaData{};///< The rule trigger metadata actions array of action objects the.
		TriggerType triggerType{};///< The rule trigger type.
		EventType eventType{};///< The rule event type.
		Snowflake creatorId{};///< The user which first created this rule.
		Snowflake guildId{};///< The guild which this rule belongs to.
		std::string name{};///< The rule name.
		bool enabled{};///< Whether the rule is enabled.

		AutoModerationRuleData() = default;

		AutoModerationRuleData& operator=(const nlohmann::json& jsonObjectData);

		AutoModerationRuleData(const nlohmann::json& jsonObjectData);

		virtual ~AutoModerationRuleData() = default;

	  protected:
		void parseObject(const nlohmann::json& jsonObjectData, AutoModerationRuleData* pDataStructure);
	};

	/// Permissions data for an ApplicationCommand. \brief Permissions data for an ApplicationCommand.
	class DiscordCoreAPI_Dll ApplicationCommandPermissionData : public DiscordEntity, public DataParser<ApplicationCommandPermissionData> {
	  public:
		ApplicationCommandPermissionType type{ ApplicationCommandPermissionType::Role };///< The type of Permission.
		bool permission{ false };///< Whether the Permission is active or not.

		ApplicationCommandPermissionData() = default;

		ApplicationCommandPermissionData& operator=(const nlohmann::json& jsonObjectData);

		ApplicationCommandPermissionData(const nlohmann::json& jsonObjectData);

		virtual ~ApplicationCommandPermissionData() = default;

	  protected:
		void parseObject(const nlohmann::json& jsonObjectData, ApplicationCommandPermissionData* pDataStructure);
	};

	/// Represents the Permissions for accessing an ApplicationCommand from within a Guild. \brief Represents the Permissions for accessing an ApplicationCommand from within a Guild.
	class DiscordCoreAPI_Dll GuildApplicationCommandPermissionsData : public DiscordEntity, public DataParser<GuildApplicationCommandPermissionsData> {
	  public:
		std::vector<ApplicationCommandPermissionData> permissions{};///< The Permissions.
		Snowflake applicationId{};///< The application's id.
		Snowflake guildId{};///< The Guild's id.

		GuildApplicationCommandPermissionsData() = default;

		GuildApplicationCommandPermissionsData& operator=(const nlohmann::json& jsonObjectData);

		GuildApplicationCommandPermissionsData(const nlohmann::json& jsonObjectData);

		virtual ~GuildApplicationCommandPermissionsData() = default;

	  protected:
		void parseObject(const nlohmann::json& jsonObjectData, GuildApplicationCommandPermissionsData* pDataStructure);
	};

	class DiscordCoreAPI_Dll GuildApplicationCommandPermissionsDataVector : public DataParser<GuildApplicationCommandPermissionsDataVector> {
	  public:
		std::vector<GuildApplicationCommandPermissionsData> theGuildApplicationCommandPermissionsDatas{};

		GuildApplicationCommandPermissionsDataVector() = default;

		GuildApplicationCommandPermissionsDataVector& operator=(const nlohmann::json& jsonObjectData);

		GuildApplicationCommandPermissionsDataVector(const nlohmann::json& jsonObjectData);

		virtual ~GuildApplicationCommandPermissionsDataVector() = default;

	  protected:
		void parseObject(const nlohmann::json& jsonObjectData, GuildApplicationCommandPermissionsDataVector* pDataStructure);
	};

	/// Data structure representing a single emoji. \brief Data structure representing a single emoji.
	class DiscordCoreAPI_Dll EmojiData : public DiscordEntity, public DataParser<EmojiData> {
	  public:
		std::wstring unicodeName{ L"" };///< What is its unicode name?
		std::vector<RoleData> roles{};///< Roles that are allowed to use this emoji.
		bool requireColons{ false };///< Require colons to render it?
		bool available{ true };///< Is it available to be used?
		bool animated{ false };///< Is it animated?
		bool managed{ false };///< Is it managed?
		StringWrapper name{};///< What is its name?
		UserData user{};///< User that created this emoji.

		EmojiData() = default;

		EmojiData& operator=(const nlohmann::json& jsonObjectData);

		EmojiData(const nlohmann::json& jsonObjectData);

		virtual ~EmojiData() = default;

	  protected:
		void parseObject(const nlohmann::json& jsonObjectData, EmojiData* pDataStructure);
	};

	class DiscordCoreAPI_Dll EmojiDataVector : public DataParser<EmojiDataVector> {
	  public:
		std::vector<EmojiData> theEmojiDatas{};

		EmojiDataVector() = default;

		EmojiDataVector& operator=(const nlohmann::json& jsonObjectData);

		EmojiDataVector(const nlohmann::json& jsonObjectData);

		virtual ~EmojiDataVector() = default;

	  protected:
		void parseObject(const nlohmann::json& jsonObjectData, EmojiDataVector* pDataStructure);
	};

	/// For updating/modifying a given Channel's properties. \brief For updating/modifying a given Channel's properties.
	struct DiscordCoreAPI_Dll UpdateChannelData {
		std::unordered_map<Snowflake, OverWriteData> permissionOverwrites{};
		int32_t defaultAutoArchiveDuration{ 10080 };
		int32_t videoQualityMode{ 1 };
		int32_t rateLimitPerUser{ 0 };
		int32_t bitrate{ 48000 };
		std::string parentId{};
		std::string rtcRgion{};
		int32_t userLimit{ 0 };
		int32_t position{ 0 };
		std::string topic{};
		std::string name{};
		ChannelType type{};
		bool nsfw{ false };
	};

	/// Data structure representing a single reaction. \brief/// Data structure representing a single reaction.
	class DiscordCoreAPI_Dll ReactionData : public DiscordEntity, public DataParser<ReactionData> {
	  public:
		GuildMemberData member{};///< The GuildMember who placed the reaction.
		Snowflake channelId{};///< The id of the Channel where it was placed.
		Snowflake messageId{};///< The id of the Message upon which it was placed.
		Snowflake guildId{};///< The id of the Guild where it was placed.
		int32_t count{ 0 };///< The number of times this particular emoji was placed as a reaction to the given Message.
		EmojiData emoji{};///< The emoji that was placed as a reaction.
		Snowflake userId{};///< The id of the User who placed the reaction.
		bool me{ false };///< Whether or not I (The bot) placed it.

		ReactionData() = default;

		ReactionData& operator=(const nlohmann::json& jsonObjectData);

		ReactionData(const nlohmann::json& jsonObjectData);

		virtual ~ReactionData() = default;

	  protected:
		void parseObject(const nlohmann::json& jsonObjectData, ReactionData* pDataStructure);
	};

	/// Structure representing Voice Region Data. \brief Structure representing Voice Region Data.
	struct DiscordCoreAPI_Dll VoiceRegionData : public DataParser<VoiceRegionData> {
		bool deprecated{ false };///< Whether this is a deprecated voice region(avoid switching to these).
		bool optimal{ false };///< True for a single server that is closest to the current User's client.
		bool custom{ false };///< Whether this is a custom voice region(used for events / etc).
		std::string name{};///< Name of the region.
		Snowflake id{};///< Unique ID for the region.

		VoiceRegionData() = default;

		VoiceRegionData& operator=(const nlohmann::json& jsonObjectData);

		VoiceRegionData(const nlohmann::json& jsonObjectData);

		virtual ~VoiceRegionData() = default;

	  protected:
		void parseObject(const nlohmann::json& jsonObjectData, VoiceRegionData* pDataStructure);
	};

	class DiscordCoreAPI_Dll VoiceRegionDataVector : public DataParser<VoiceRegionDataVector> {
	  public:
		VoiceRegionDataVector() = default;

		operator std::vector<VoiceRegionData>() {
			return this->theVoiceRegionDatas;
		}

		VoiceRegionDataVector& operator=(const nlohmann::json& jsonObjectData);

		VoiceRegionDataVector(const nlohmann::json& jsonObjectData);

		virtual ~VoiceRegionDataVector() = default;

	  protected:
		std::vector<VoiceRegionData> theVoiceRegionDatas{};

		void parseObject(const nlohmann::json& jsonObjectData, VoiceRegionDataVector* pDataStructure);
	};

	/// Message activity types. \brief Message activity types.
	enum class MessageActivityType : int8_t {
		Join = 1,///< Join.
		Spectate = 2,///< Spectate.
		Listen = 3,///< Listen.
		Join_Request = 5///< Join-request.
	};

	/// Message activity data. \brief Message activity data.
	struct DiscordCoreAPI_Dll MessageActivityData : public DataParser<MessageActivityData> {
		MessageActivityType type{ MessageActivityType::Join };///< Message activity type.
		std::string partyId{};///< Party id.

		MessageActivityData() = default;

		MessageActivityData& operator=(const nlohmann::json& jsonObjectData);

		MessageActivityData(const nlohmann::json& jsonObjectData);

		virtual ~MessageActivityData() = default;

		void parseObject(const nlohmann::json& jsonObjectData, MessageActivityData* pDataStructure);
	};

	/// Ban data. \brief Ban data.
	struct DiscordCoreAPI_Dll BanData : public DataParser<BanData> {
		bool failedDueToPerms{ false };///< Failed due to perms?
		std::string reason{};///< Reason for the ban.
		UserData user{};///< User that was banned.

		BanData() = default;

		BanData& operator=(const nlohmann::json& jsonObjectData);

		BanData(const nlohmann::json& jsonObjectData);

		virtual ~BanData() = default;

	  protected:
		void parseObject(const nlohmann::json& jsonObjectData, BanData* pDataStructure);
	};

	class DiscordCoreAPI_Dll BanDataVector : public DataParser<BanDataVector> {
	  public:
		BanDataVector() = default;

		operator std::vector<BanData>();

		BanDataVector& operator=(const nlohmann::json& jsonObjectData);

		BanDataVector(const nlohmann::json& jsonObjectData);

		virtual ~BanDataVector() = default;

	  protected:
		std::vector<BanData> theBanDatas{};

		void parseObject(const nlohmann::json& jsonObjectData, BanDataVector* pDataStructure);
	};

	/// Team members object data. \brief Team members object data.
	struct DiscordCoreAPI_Dll TeamMembersObjectData : public DataParser<TeamMembersObjectData> {
		std::vector<Permissions> permissions{};///< Permissions for the team.
		int32_t membershipState{ 0 };///< Current state.
		std::string teamId{};///< Id of the current team.
		UserData user{};///< User data of the current User.

		TeamMembersObjectData() = default;

		TeamMembersObjectData& operator=(const nlohmann::json& jsonObjectData);

		TeamMembersObjectData(const nlohmann::json& jsonObjectData);

		virtual ~TeamMembersObjectData() = default;

	  protected:
		void parseObject(const nlohmann::json& jsonObjectData, TeamMembersObjectData* pDataStructure);
	};

	/// For updating the current voice state. \brief For updating the current voice state.
	struct DiscordCoreAPI_Dll UpdateVoiceStateData {
		bool selfMute{ false };///< Whether or not we self-mute ourselves.
		bool selfDeaf{ false };///< Whether or not we self-deafen ourselves.
		Snowflake channelId{};///< Id of the desired voice Channel. Leave blank to disconnect.
		Snowflake guildId{};///< The id of the Guild fo which we would like to establish a voice connection.

		operator nlohmann::json();
	};

	/// Team object data. \brief Team object data.
	class DiscordCoreAPI_Dll TeamObjectData : public DiscordEntity, public DataParser<TeamObjectData> {
	  public:
		std::vector<TeamMembersObjectData> members{};///< Array of team members object data.
		std::string ownerUserId{};///< User id of the team owner.
		std::string icon{};///< Icon for the team.

		TeamObjectData() = default;

		TeamObjectData& operator=(const nlohmann::json& jsonObjectData);

		TeamObjectData(const nlohmann::json& jsonObjectData);

		virtual ~TeamObjectData() = default;

	  protected:
		void parseObject(const nlohmann::json& jsonObjectData, TeamObjectData* pDataStructure);
	};

	/// Application flags, for the ApplicationData structure.
	enum class ApplicationFlags : int32_t {
		Gateway_Presence = 1 << 12,///< Intent required for bots in 100 or more servers to receive presence_update events.
		Gateway_Presence_Limited = 1 << 13,///< Intent required for bots in under 100 servers to receive presence_update events, found in Bot Settings.
		Gateway_Guild_Members = 1 << 14,///< Intent required for bots in 100 or more servers to receive member-related events like guild_member_add.
		Gateway_Guild_Members_Limited = 1 << 15,///< Intent required for bots in under 100 servers to receive member-related events like guild_member_add, found in Bot Settings.
		Verificatino_Pending_Guild_Limit = 1 << 16,///< Indicates unusual growth of an app that prevents verification
		Embedded = 1 << 17,///< Indicates if an app is embedded within the Discord client (currently unavailable publicly)
		Gateway_Message_Content = 1 << 18,///< Intent required for bots in 100 or more servers to receive message content
		Gateway_Message_Content_Limited = 1 << 19///< Intent required for bots in under 100 servers to receive message content, found in Bot Settings};
	};

	/// Install params data, for application data. \brief Install params data, for application data.
	struct DiscordCoreAPI_Dll InstallParamsData : public DataParser<InstallParamsData> {
		std::vector<std::string> scopes{};///< The scopes to add the application to the server with.
		std::string permissions{};///< The permissions to request for the bot role.

		InstallParamsData() = default;

		InstallParamsData& operator=(const nlohmann::json& jsonObjectData);

		InstallParamsData(const nlohmann::json& jsonObjectData);

		virtual ~InstallParamsData() = default;

	  protected:
		void parseObject(const nlohmann::json& jsonObjectData, InstallParamsData* pDataStructure);
	};

	/// Application data. \brief Application data.
	class DiscordCoreAPI_Dll ApplicationData : public DiscordEntity, public DataParser<ApplicationData> {
	  public:
		std::vector<std::string> rpcOrigins{};///< Array of RPC origin strings.
		bool botRequireCodeGrant{ false };///< Does the bot require a code grant?
		std::vector<std::string> tags{};///< Up to 5 tags describing the content and functionality of the application install_params.
		std::string termsOfServiceUrl{};///< Terms of service Url.
		std::string privacyPolicyUrl{};///< Privacy policy Url.
		ApplicationFlags flags{ 0 };///< Application flags.
		InstallParamsData params{};///< Settings for the application's default in-app authorization link, if enabled std::string customInstallUrl{};
		std::string primarySkuId{};///< Primary SKU Id.
		std::string description{};///< Description of the application.
		std::string coverImage{};///< The cover image.
		bool botPublic{ false };///< Is the bot public?
		std::string verifyKey{};///< The verification key.
		std::string summary{};///< Summary of the application.
		TeamObjectData team{};///< Team object data.
		Snowflake guildId{};///< Guild id.
		std::string slug{};///< Sluhg.
		std::string name{};///< Application's name.
		std::string icon{};///< Application's icon.
		UserData owner{};///< Application's owner.

		ApplicationData() = default;

		ApplicationData& operator=(const nlohmann::json& jsonObjectData);

		ApplicationData(const nlohmann::json& jsonObjectData);

		virtual ~ApplicationData() = default;

	  protected:
		void parseObject(const nlohmann::json& jsonObjectData, ApplicationData* pDataStructure);
	};

	/// Authorization info structure. \brief Authorization info structure.
	struct DiscordCoreAPI_Dll AuthorizationInfoData : public DataParser<AuthorizationInfoData> {
		std::vector<std::string> scopes{};///< Array of strings - the scopes the User has authorized the application for.
		ApplicationData application{};///< Partial application object the current application.
		std::string expires{};///< When the access token expires.
		UserData user{};/// The User who has authorized, if the User has authorized with the identify scope.

		AuthorizationInfoData() = default;

		AuthorizationInfoData& operator=(const nlohmann::json& jsonObjectData);

		AuthorizationInfoData(const nlohmann::json& jsonObjectData);

		virtual ~AuthorizationInfoData() = default;

	  protected:
		void parseObject(const nlohmann::json& jsonObjectData, AuthorizationInfoData* pDataStructure);
	};

	/// Account data. \brief Account data.
	class DiscordCoreAPI_Dll AccountData : public DiscordEntity, public DataParser<AccountData> {
	  public:
		std::string name{};///< Name of the account.

		AccountData() = default;

		AccountData& operator=(const nlohmann::json& jsonObjectData);

		AccountData(const nlohmann::json& jsonObjectData);

		virtual ~AccountData() = default;

	  protected:
		void parseObject(const nlohmann::json& jsonObjectData, AccountData* pDataStructure);
	};

	/// Guild Widget Data. \brief Guild Widget Data.
	struct DiscordCoreAPI_Dll GuildWidgetData : public DataParser<GuildWidgetData> {
		bool enabled{ false };///< Whether the widget is enabled.
		Snowflake channelId{};///< The widget Channel id.

		GuildWidgetData() = default;

		GuildWidgetData& operator=(const nlohmann::json& jsonObjectData);

		GuildWidgetData(const nlohmann::json& jsonObjectData);

		virtual ~GuildWidgetData() = default;

	  protected:
		void parseObject(const nlohmann::json& jsonObjectData, GuildWidgetData* pDataStructure);
	};

	/// Get Guild Widget Data. \brief Get Guild Widget Data.
	struct DiscordCoreAPI_Dll GetGuildWidgetObjectData : public DiscordEntity {
		std::vector<ChannelData> channels{};///< Voice and stage channels which are accessible by everyone.
		std::vector<UserData> members{};///< Special widget user objects that includes users presence (Limit 100).
		std::string instantInvite{};///< Instant invite for the guilds specified widget invite channel.
		int32_t presence_count{ 0 };///< Number of online members in this guild.
		std::string name{};///< Guild name (2-100 characters).
	};

	/// Widget style options. \brief Widget style options.
	enum class WidgetStyleOptions : int8_t {
		Shield = 0,///< Shield
		Banner1 = 1,///< Banner1
		Banner2 = 2,///< Banner2
		Banner3 = 3,///< Banner3
		Banner4 = 4///< Banner4
	};

	/// Guild widget image data. \brief Guild widget image data.
	struct DiscordCoreAPI_Dll GuildWidgetImageData : public DataParser<GuildWidgetImageData> {
		std::string url{};

		GuildWidgetImageData() = default;

		GuildWidgetImageData& operator=(const nlohmann::json& jsonObjectData);

		GuildWidgetImageData(const nlohmann::json& jsonObjectData);

		virtual ~GuildWidgetImageData() = default;

	  protected:
		void parseObject(const nlohmann::json& jsonObjectData, GuildWidgetImageData* pDataStructure);
	};

	/// Integration data. \brief Integration data.
	class DiscordCoreAPI_Dll IntegrationData : public DiscordEntity, public DataParser<IntegrationData> {
	  public:
		TimeStamp<std::chrono::milliseconds> syncedAt{};///< Time it was last synced at.
		int32_t expireGracePeriod{ 0 };///< How int64_t before the integration expires.
		ApplicationData application{};///< Application data.
		int32_t subscriberCount{ 0 };///< Number of current subscribers.
		bool enableEmoticons{ true };///< Emoticons enabled?
		int32_t expireBehavior{ 0 };///< What to do upon expiry.
		bool enabled{ false };///< Enabled?
		bool syncing{ false };///< Is it syncing?
		AccountData account{};///< Account data.
		bool revoked{ false };///< Has it been revoked?
		std::string name{};///< Name of the integration.
		std::string type{};///< Type of integration.
		Snowflake roleId{};///< Role Id.
		UserData user{};///< User data for the integration.

		IntegrationData() = default;

		IntegrationData& operator=(const nlohmann::json& jsonObjectData);

		IntegrationData(const nlohmann::json& jsonObjectData);

		virtual ~IntegrationData() = default;

	  protected:
		void parseObject(const nlohmann::json& jsonObjectData, IntegrationData* pDataStructure);
	};

	class DiscordCoreAPI_Dll IntegrationDataVector : public DataParser<IntegrationDataVector> {
	  public:
		IntegrationDataVector() = default;

		operator std::vector<IntegrationData>() {
			return this->theIntegrationDatas;
		}

		IntegrationDataVector& operator=(const nlohmann::json& jsonObjectData);

		IntegrationDataVector(const nlohmann::json& jsonObjectData);

		virtual ~IntegrationDataVector() = default;

	  protected:
		std::vector<IntegrationData> theIntegrationDatas{};

		void parseObject(const nlohmann::json& jsonObjectData, IntegrationDataVector* pDataStructure);
	};

	/// Audit log events. \brief Audit log events.
	enum class AuditLogEvent : uint8_t {
		Guild_Update = 1,///< Guild update.
		Channel_Create = 10,///< Channel create.
		Channel_Update = 11,///< Channel update.
		Channel_Delete = 12,///< Channel delete.
		Channel_Overwrite_Create = 13,///< Channel overwrite create.
		Channel_Overwrite_Update = 14,///< Channel overwrite update.
		Channel_Overwrite_Delete = 15,///< Channel overwrite delete.
		Member_Kick = 20,///< Member kick.
		Member_Prune = 21,///< Member prune.
		Member_Ban_Add = 22,///< Member ban add.
		Member_Ban_Remove = 23,///< Member ban remove.
		Member_Update = 24,///< Member update.
		Member_Role_Update = 25,///< Member role update.
		Member_Move = 26,///< Member move.
		Member_Disconnect = 27,///< Member disconnect.
		Bot_Add = 28,///< Bot add.
		Role_Create = 30,///< Role create.
		Role_Update = 31,///< Role update.
		Role_Delete = 32,///< Role delete.
		Invite_Create = 40,///< Invite create.
		Invite_Update = 41,///< Invite update.
		Invite_Delete = 42,///< Invite delete.
		Webhook_Create = 50,///< Webhook create.
		Webhook_Update = 51,///< Webhook update.
		Webhook_Delete = 52,///< Webhook delete.
		Emoji_Create = 60,///< Emoji create.
		Emoji_Update = 61,///< Emoji update.
		Emoji_Delete = 62,///< Emoji delete.
		Message_Delete = 72,///< Message delete.
		Message_Bulk_Delete = 73,///< Message bulk delete.
		Message_Pin = 74,///< Message pin.
		Message_Unpin = 75,///< Message unpin.
		Integration_Create = 80,///< Integration create.
		Integration_Update = 81,///< Integration update.
		Integration_Delete = 82,///< Integration delete.
		Stage_Instance_Create = 83,///< Stage-Instance create.
		Stage_Instance_Update = 84,///< Stage-Instance update.
		Stage_Instance_Delete = 85,///< Stage-Instance delete.
		Sticker_Create = 90,///< Sticker create.
		Sticker_Update = 91,///< Sticker update.
		Sticker_Delete = 92,///< Sticker delete.
		Guild_Scheduled_Event_Create = 100,///< Guild-scheduled-event create.
		Guild_Scheduled_Event_Update = 101,///< Guild-scheduled-event update.
		Guild_Scheduled_Event_Delete = 102,///< Guild-scheduled-event delete.
		Thread_Create = 110,///< Thread create.
		Thread_Update = 111,///< Thread update.
		Thread_Delete = 112,///< Thread delete.
		Application_Command_Permission_Update = 121,///< Permissions were updated for a command.
		Auto_Moderation_Rule_Create = 140,///< Auto Moderation rule was created.
		Auto_Moderation_Rule_Update = 141,///< Auto Moderation rule was updated.
		Auto_Moderation_Rule_Delete = 142,///< Auto Moderation rule was deleted.
		Auto_Moderation_Block_Message = 143///< Message was blocked by AutoMod (according to a rule).
	};

	/// Audit log entry info data \brief Audit log entry info data.
	class DiscordCoreAPI_Dll OptionalAuditEntryInfoData : public DiscordEntity, public DataParser<OptionalAuditEntryInfoData> {
	  public:
		std::string deleteMemberDays{};///< Number of days for which the member's Messages were deleted.
		std::string membersRemoved{};///< Number of members that were removed upon a prune.
		Snowflake applicationId{};///< ID of the app whose permissions were targeted APPLICATION_COMMAND_PERMISSION_UPDATE.
		std::string roleName{};///< Role name.
		Snowflake channelId{};///< Channel Id.
		Snowflake messageId{};///< Message Id.
		std::string count{};///< Count.
		std::string type{};///< Type.

		OptionalAuditEntryInfoData() = default;

		OptionalAuditEntryInfoData& operator=(const nlohmann::json& jsonObjectData);

		OptionalAuditEntryInfoData(const nlohmann::json& jsonObjectData);

		virtual ~OptionalAuditEntryInfoData() = default;

	  protected:
		void parseObject(const nlohmann::json& jsonObjectData, OptionalAuditEntryInfoData* pDataStructure);
	};

	/// Audit log change data. \brief Audit log change data.
	struct DiscordCoreAPI_Dll AuditLogChangeData : public DataParser<AuditLogChangeData> {
		nlohmann::json newValue{};///< New value.
		nlohmann::json oldValue{};///< Old value.
		std::string key{};///< The key of the audit log change.

		AuditLogChangeData() = default;

		AuditLogChangeData& operator=(const nlohmann::json& jsonObjectData);

		AuditLogChangeData(const nlohmann::json& jsonObjectData);

		virtual ~AuditLogChangeData() = default;

	  protected:
		void parseObject(const nlohmann::json& jsonObjectData, AuditLogChangeData* pDataStructure);
	};

	/// Guild prune count data. \brief Guild prune count data.
	struct DiscordCoreAPI_Dll GuildPruneCountData : public DataParser<GuildPruneCountData> {
		int32_t count{ 0 };

		GuildPruneCountData() = default;

		GuildPruneCountData& operator=(const nlohmann::json& jsonObjectData);

		GuildPruneCountData(const nlohmann::json& jsonObjectData);

		virtual ~GuildPruneCountData() = default;

	  protected:
		void parseObject(const nlohmann::json& jsonObjectData, GuildPruneCountData* pDataStructure);
	};

	/// Audit log entry data. \brief Audit log entry data.
	class DiscordCoreAPI_Dll AuditLogEntryData : public DiscordEntity, public DataParser<AuditLogEntryData> {
	  public:
		TimeStamp<std::chrono::milliseconds> createdTimeStamp{ "" };///< Time at which this entry was created.
		std::vector<AuditLogChangeData> changes{};///< Array of audit log change data.
		OptionalAuditEntryInfoData options{};///< Audit log entry info data.
		AuditLogEvent actionType{};///< Audit log action type.
		std::string reason{};///< The reason that was entered for the given change.
		Snowflake targetId{};///< Id of the target User.
		Snowflake userId{};///< Id of the executing User.

		AuditLogEntryData() = default;

		AuditLogEntryData& operator=(const nlohmann::json& jsonObjectData);

		AuditLogEntryData(const nlohmann::json& jsonObjectData);

		virtual ~AuditLogEntryData() = default;

	  protected:
		void parseObject(const nlohmann::json& jsonObjectData, AuditLogEntryData* pDataStructure);
	};

	/// Party data. \brief Party data.
	class DiscordCoreAPI_Dll PartyData : public DiscordEntity, public DataParser<PartyData> {
	  public:
		std::vector<int32_t> size{ 0, 0 };///< The size of the party.

		PartyData() = default;

		PartyData& operator=(const nlohmann::json& jsonObjectData);

		PartyData(const nlohmann::json& jsonObjectData);

		virtual ~PartyData() = default;

	  protected:
		void parseObject(const nlohmann::json& jsonObjectData, PartyData* pDataStructure);
	};

	/// Assets data. \brief Party data.
	struct DiscordCoreAPI_Dll AssetsData : public DataParser<AssetsData> {
		StringWrapper largeImage{};///< Keyname of an asset to display.
		StringWrapper smallImage{};///< Keyname of an asset to display.
		StringWrapper largeText{};///< Hover text for the large image.
		StringWrapper smallText{};///< Hover text for the small image.

		AssetsData() = default;

		AssetsData& operator=(const nlohmann::json& jsonObjectData);

		AssetsData(const nlohmann::json& jsonObjectData);

		virtual ~AssetsData() = default;

	  protected:
		void parseObject(const nlohmann::json& jsonObjectData, AssetsData* pDataStructure);
	};

	/// Secrets data. \brief Secrets data.
	struct DiscordCoreAPI_Dll SecretsData : public DataParser<SecretsData> {
		StringWrapper spectate{};///< Unique hash for the given match context.
		StringWrapper match{};///< Unique hash for Spectate button.
		StringWrapper join{};///< Unique hash for chat invitesand Ask to Join.

		SecretsData() = default;

		SecretsData& operator=(const nlohmann::json& jsonObjectData);

		SecretsData(const nlohmann::json& jsonObjectData);

		virtual ~SecretsData() = default;

	  protected:
		void parseObject(const nlohmann::json& jsonObjectData, SecretsData* pDataStructure);
	};

	/// Timestamp data. \brief Timestamp data.
	struct DiscordCoreAPI_Dll TimestampData : public DataParser<TimestampData> {
		int64_t start{ 0 };///< Unix timestamp - Send this to have an "elapsed" timer.
		int64_t end{ 0 };///< Unix timestamp - send this to have a "remaining" timer.

		TimestampData() = default;

		TimestampData& operator=(const nlohmann::json& jsonObjectData);

		TimestampData(const nlohmann::json& jsonObjectData);

		virtual ~TimestampData() = default;

	  protected:
		void parseObject(const nlohmann::json& jsonObjectData, TimestampData* pDataStructure);
	};

	/// Button data. \brief Button data.
	struct DiscordCoreAPI_Dll ButtonData {
		StringWrapper label{};///< Visible label of the button.
		StringWrapper url{};///< Url to display on the button.
	};

	/// Activity types. \brief Activity types.
	enum class ActivityType : int8_t {
		Game = 0,///< Game.
		Streaming = 1,///< Streaming.
		Listening = 2,///< Listening.
		Watching = 3,///< Watching.
		Custom = 4,///< Custom.
		Competing = 5///< Competing.
	};

	/// Activity data. \brief Activity data.
	struct DiscordCoreAPI_Dll ActivityData : public DataParser<ActivityData> {
		TimestampData timestamps{};///< Timestamp data.
		Snowflake applicationId{};///< Application id for the current application.
		StringWrapper details{};///< Details about the activity.
		int32_t createdAt{ 0 };///< Timestamp of when the activity began.
		bool instance{ false };///< Whether this activity is an instanced context, like a match.
		StringWrapper state{};///< The player's current party status.
		SecretsData secrets{};///< Secrets data.
		ButtonData buttons{};///< Button Data.
		StringWrapper name{};///< Name of the activity.
		ActivityType type{};///< Activity data.
		AssetsData assets{};///< Assets data.
		StringWrapper url{};///< Url associated with the activity.
		int32_t flags{ 0 };///< Flags.
		EmojiData emoji{};///< Emoji associated with the activity.
		PartyData party{};///< Party data.

		ActivityData() = default;

		ActivityData& operator=(const nlohmann::json& jsonObjectData);

		ActivityData(const nlohmann::json& jsonObjectData);

		virtual ~ActivityData() = default;

	  protected:
		void parseObject(const nlohmann::json& jsonObjectData, ActivityData* pDataStructure);
	};

	/// Client status data. \brief Client status data.
	struct DiscordCoreAPI_Dll ClientStatusData : public DataParser<ClientStatusData> {
		StringWrapper desktop{};///< Desktop name.
		StringWrapper mobile{};///< Mobile name.
		StringWrapper web{};///< Web link.

		ClientStatusData() = default;

		ClientStatusData& operator=(const nlohmann::json& jsonObjectData);

		ClientStatusData(const nlohmann::json& jsonObjectData);

		virtual ~ClientStatusData() = default;

	  protected:
		void parseObject(const nlohmann::json& jsonObjectData, ClientStatusData* pDataStructure);
	};

	/// Premium tier levels. \brief Premium tier levels.
	enum class PremiumTier : int8_t {
		None = 0,///< None.
		Tier_1 = 1,///< Tier 1.
		Tier_2 = 2,///< Tier 2.
		Tier_3 = 3///< Tier 3.
	};

	/// Default Message notification levels. \brief Default Message notification
	/// levels.
	enum class DefaultMessageNotificationLevel : int8_t {
		All_Messages = 0,///< All messages.
		Only_Mentions = 1///< Only mentions.
	};

	/// Explicit content filter levels. \brief Explicit content filter levels.
	enum class ExplicitContentFilterLevel : int8_t {
		Disabled = 0,///< Disabled.
		Members_Without_Roles = 1,///< Members without roles.
		All_Members = 2///< All members.
	};

	/// MFA levels. \brief MFA levels.
	enum class MFALevel : int8_t {
		None = 0,///< None.
		Elevated = 1///< Elevated.
	};

	/// Verification levels. \brief/// Verification levels.
	enum class VerificationLevel : int8_t {
		None = 0,///< None.
		Low = 1,///< Low.
		Medium = 2,///< Medium.
		High = 3,///< High.
		Very_High = 4///< Very high.
	};

	/// Welcome screen Channel data. \brief Welcome screen Channel data.
	struct DiscordCoreAPI_Dll WelcomeScreenChannelData : public DataParser<WelcomeScreenChannelData> {
		std::string description{};///< Description of the welcome Channel.
		std::string emojiName{};///< Emoji name for the Channel.
		Snowflake channelId{};///< Id of the welcome Channel.
		Snowflake emojiId{};///< Emoji id for the Channel.

		WelcomeScreenChannelData() = default;

		WelcomeScreenChannelData& operator=(const nlohmann::json& jsonObjectData);

		WelcomeScreenChannelData(const nlohmann::json& jsonObjectData);

		virtual ~WelcomeScreenChannelData() = default;

	  protected:
		void parseObject(const nlohmann::json& jsonObjectData, WelcomeScreenChannelData* pDataStructure);
	};

	/// Welcome screen data. \brief Welcome screen data.
	struct DiscordCoreAPI_Dll WelcomeScreenData : public DataParser<WelcomeScreenData> {
		std::vector<WelcomeScreenChannelData> welcomeChannels{};///< Welcome screen Channel data.
		std::string description{};///< Description of the welcome screen.

		WelcomeScreenData() = default;

		WelcomeScreenData& operator=(const nlohmann::json& jsonObjectData);

		WelcomeScreenData(const nlohmann::json& jsonObjectData);

		virtual ~WelcomeScreenData() = default;

	  protected:
		void parseObject(const nlohmann::json& jsonObjectData, WelcomeScreenData* pDataStructure);
	};

	/// Presence update data. \brief Presence update data.
	struct DiscordCoreAPI_Dll PresenceUpdateData : public DataParser<PresenceUpdateData> {
		std::vector<ActivityData> activities{};///< Array of activities.
		ClientStatusData clientStatus{};///< Current client status.
		StringWrapper status{};///< Status of the current presence.
		Snowflake guildId{};///< Guild id for the current presence.
		UserData user{};///< User data for the current presence.

		PresenceUpdateData() = default;

		PresenceUpdateData& operator=(const nlohmann::json& jsonObjectData);

		PresenceUpdateData(const nlohmann::json& jsonObjectData);

		virtual ~PresenceUpdateData() = default;

	  protected:
		void parseObject(const nlohmann::json& jsonObjectData, PresenceUpdateData* pDataStructure);
	};

	/// Stage instance privacy levels. \brief Stage instance privacy levels.
	enum class StageInstancePrivacyLevel : int8_t {
		Public = 1,///< Public.
		Guild_Only = 2///< Guild only.
	};

	/// Stage instance data. \brief Stage instance data.
	class DiscordCoreAPI_Dll StageInstanceData : public DiscordEntity, public DataParser<StageInstanceData> {
	  public:
		StageInstancePrivacyLevel privacyLevel{ 0 };///< Privacy level of the Channel.
		bool discoverableDisabled{ false };///< Is it discoverable?
		Snowflake channelId{};///< The Channel's id.
		std::string topic{};///< The topic of the StageInstance.
		Snowflake guildId{};///< The Guild id for which the Channel exists in.

		StageInstanceData() = default;

		StageInstanceData& operator=(const nlohmann::json& jsonObjectData);

		StageInstanceData(const nlohmann::json& jsonObjectData);

		virtual ~StageInstanceData() = default;

	  protected:
		void parseObject(const nlohmann::json& jsonObjectData, StageInstanceData* pDataStructure);
	};

	/// Sticker types. \brief Sticker types.
	enum class StickerType : int8_t {
		Standard = 1,///< Standard.
		Guild = 2///< Guild.
	};

	enum class StickerFlags : int8_t { Available = 1 << 0 };

	/// Data representing a single Sticker. \brief Data representing a single Sticker.
	class DiscordCoreAPI_Dll StickerData : public DiscordEntity, public DataParser<StickerData> {
	  public:
		StickerFormatType formatType{};///< Format type.
		std::string description{};///< Description of the Sticker.
		int8_t stickerFlags{ 0 };///< Sticker flags.
		int32_t nsfwLevel{ 0 };///< NSFW warning level.
		int32_t sortValue{ 0 };///< Where in the stack of stickers it resides.
		std::string packId{};///< Pack id of the Sticker.
		std::string asset{};///< Asset value for the Sticker.
		std::string name{};///< The Sticker's name.
		std::string tags{};///< Tags for the Sticker to use.
		StickerType type{};///< The type of Sticker.
		Snowflake guildId{};///< The Guild id for which the Sticker exists in.
		UserData user{};///< The User that uploaded the Guild Sticker.

		StickerData() = default;

		StickerData& operator=(const nlohmann::json& jsonObjectData);

		StickerData(const nlohmann::json& jsonObjectData);

		virtual ~StickerData() = default;

	  protected:
		void parseObject(const nlohmann::json& jsonObjectData, StickerData* pDataStructure);
	};

	/// Data representing a single Guild preview. \brief Data representing a single Guild preview.
	struct DiscordCoreAPI_Dll GuildPreviewData : public DataParser<GuildPreviewData> {
		int32_t approximatePresenceCount{ 0 };
		std::vector<std::string> features{};
		std::vector<StickerData> stickers{};
		int32_t approximateMemberCount{ 0 };
		std::vector<EmojiData> emojis{};
		std::string discoverySplash{};
		std::string description{};
		std::string splash{};
		std::string name{};
		std::string icon{};
		Snowflake id{};

		GuildPreviewData() = default;

		GuildPreviewData& operator=(const nlohmann::json& jsonObjectData);

		GuildPreviewData(const nlohmann::json& jsonObjectData);

		virtual ~GuildPreviewData() = default;

	  protected:
		void parseObject(const nlohmann::json& jsonObjectData, GuildPreviewData* pDataStructure);
	};

	/// Afk timeout durations. \brief Afk timeout durations.
	enum class AfkTimeOutDurations : int16_t {
		Shortest = 60,///< Shortest.
		Short = 300,///< Short.
		Medium = 900,///< Medium.
		Long = 1800,///< Long.
		Longest = 3600///< Longest.
	};

	/// Guild NSFW level. \brief Guild NSFW level.
	enum class GuildNSFWLevel : int8_t {
		Default = 0,///< Default.
		Explicit = 1,///< Explicit.
		Safe = 2,///< Safe.
		Age_Restricted = 3///< Age restricted.
	};

	/// System channel flags. \brief System channel flags.
	enum class SystemChannelFlags : int8_t {
		Suppress_Join_Notifications = 1 << 0,///< Suppress member join notifications.
		Suppress_Premium_Subscriptions = 1 << 1,///< Suppress server boost notifications.
		Suppress_Guild_Reminder_Notifications = 1 << 2,///< Suppress server setup tips.
		Suppress_Join_Notification_Replies = 1 << 3///< Hide member join sticker reply buttons.
	};

	/// Guild flags. \brief Guild flags.
	enum class GuildFlags : int8_t {
		WidgetEnabled = 1 << 0,///< Widget enabled.
		Unavailable = 1 << 1,///< Unavailable.
		Owner = 1 << 2,///< Owner.
		Large = 1 << 3,///< Large.
		Premium_Progress_Bar_Enabled = 1 << 4///< Premium progress bar enabled
	};

	/// Data structure representing a single Guild. \brief Data structure representing a single Guild.
	class DiscordCoreAPI_Dll GuildData : public DiscordEntity, public DataParser<GuildData> {
	  public:
		std::unordered_map<Snowflake, PresenceUpdateData> presences{};///< Map of presences for each GuildMember.
		std::unordered_map<Snowflake, VoiceStateData> voiceStates{};///< Map of Guild-member voice-states.
		TimeStamp<std::chrono::milliseconds> joinedAt{};///< When the bot joined this Guild.
		DiscordCoreClient* discordCoreClient{ nullptr };///< A pointer to the DiscordCoreClient.
		VoiceConnection* voiceConnectionPtr{ nullptr };///< A pointer to the VoiceConnection, if present.
		std::vector<StringWrapper> features{};///< List of Guild features.
		std::vector<Snowflake> channels{};///< Array of Guild channels.
		std::vector<Snowflake> members{};///< Array of GuildMembers.
		std::vector<Snowflake> roles{};///< Array of Guild roles.
		int32_t memberCount{ 0 };///< Member count.
		StringWrapper icon{};///< Url to the Guild's icon.
		StringWrapper name{};///< The Guild's name.
		Snowflake ownerId{};///< User id of the Guild's owner.
		int8_t flags{ 0 };///< Guild flags.

		GuildData() = default;

		GuildData& operator=(const nlohmann::json& jsonObjectData);

		GuildData(const nlohmann::json& jsonObjectData);

		VoiceConnection* connectToVoice(const Snowflake guildMemberId, const Snowflake channelId = 0, bool selfDeaf = false, bool selfMute = false);

		void insertGuildMember(GuildMemberData theData);

		void insertChannel(ChannelData theData);

		void insertRole(RoleData theData);

		bool areWeConnected();

		void initialize();

		void disconnect();

		virtual ~GuildData() = default;

	  protected:
		void parseObject(const nlohmann::json& jsonObjectData, GuildData* pDataStructure);
	};

	class DiscordCoreAPI_Dll GuildDataVector : public DataParser<GuildDataVector> {
	  public:
		friend class Guilds;

		GuildDataVector() = default;

		operator std::vector<GuildData>();

		GuildDataVector& operator=(const nlohmann::json& jsonObjectData);

		GuildDataVector(const nlohmann::json& jsonObjectData);

		virtual ~GuildDataVector() = default;

	  protected:
		std::vector<GuildData> theGuildDatas{};

		void parseObject(const nlohmann::json& jsonObjectData, GuildDataVector* pDataStructure);
	};

	/// Guild scheduled event privacy levels. \brief Guild scheduled event privacy levels.
	enum class GuildScheduledEventPrivacyLevel : int8_t {
		Public = 1,///< Public.
		Guild_Only = 2///< Guild only.
	};

	/// GuildScheduledEventStatus. \brief GuildScheduledEventStatus.
	enum class GuildScheduledEventStatus : int8_t {
		Scheduled = 1,///< Scheduled.
		Active = 2,///< Active.
		Completed = 3,///< Completed.
		Canceled = 4///< Cancelled.
	};

	/// Guild scheduled event entity types. \brief Guild scheduled event entity types.
	enum class GuildScheduledEventEntityType : int8_t {
		None = 0,///< None.
		State_Instance = 1,///< Stage instance.
		Voice = 2,///< Voice.
		External = 3///< External.
	};

	/// Guild scheduled event entity metadata. \brief Guild scheduled event entity metadata.
	struct DiscordCoreAPI_Dll GuildScheduledEventMetadata : public DataParser<GuildScheduledEventMetadata> {
		std::string location{};

		GuildScheduledEventMetadata() = default;

		GuildScheduledEventMetadata& operator=(const nlohmann::json& jsonObjectData);

		GuildScheduledEventMetadata(const nlohmann::json& jsonObjectData);

		virtual ~GuildScheduledEventMetadata() = default;

	  protected:
		void parseObject(const nlohmann::json& jsonObjectData, GuildScheduledEventMetadata* pDataStructure);
	};

	/// Data representing a Guild Scheduled Event. \brief Data representing a Guild Scheduled Event.
	class DiscordCoreAPI_Dll GuildScheduledEventData : public DiscordEntity, public DataParser<GuildScheduledEventData> {
	  public:
		GuildScheduledEventPrivacyLevel privacyLevel{};///< The privacy level of the scheduled event.
		GuildScheduledEventMetadata entityMetadata{};///< Additional metadata for the Guild scheduled event.
		GuildScheduledEventEntityType entityType{};///< The type of the scheduled event.
		GuildScheduledEventStatus status{};///< The status of the scheduled event.
		std::string scheduledStartTime{};///< The time the scheduled event will start.
		std::string scheduledEndTime{};///< The time the scheduled event will end, required if entity_type is External.
		std::string description{};///< The description of the scheduled event(1 - 1000 characters.
		uint32_t userCount{ 0 };///< The number of users subscribed to the scheduled event.
		std::string creatorId{};///< The id of the User that created the scheduled event *.
		std::string entityId{};///< The id of an entity associated with a Guild scheduled event.
		Snowflake channelId{};///< The Channel id in which the scheduled event will be hosted, or null if scheduled entity type is External.
		Snowflake guildId{};///< The Guild id which the scheduled event belongs to.
		std::string name{};///< The name of the scheduled event(1 - 100 characters).
		UserData creator{};///< The User that created the scheduled event.

		GuildScheduledEventData() = default;

		GuildScheduledEventData& operator=(const nlohmann::json& jsonObjectData);

		GuildScheduledEventData(const nlohmann::json& jsonObjectData);

		virtual ~GuildScheduledEventData() = default;

	  protected:
		void parseObject(const nlohmann::json& jsonObjectData, GuildScheduledEventData* pDataStructure);
	};

	/// Data representing a single GuildScheduledEventUser. \brief Data representing a single GuildScheduledEventUser.
	struct DiscordCoreAPI_Dll GuildScheduledEventUserData : public DataParser<GuildScheduledEventUserData> {
		std::string guildScheduledEventId{};///< The scheduled event id which the User subscribed to/
		GuildMemberData member{};///< Guild member data for this User for the Guild which this event belongs to, if any.
		UserData user{};///< User which subscribed to an event.

		GuildScheduledEventUserData() = default;

		GuildScheduledEventUserData& operator=(const nlohmann::json& jsonObjectData);

		GuildScheduledEventUserData(const nlohmann::json& jsonObjectData);

		virtual ~GuildScheduledEventUserData() = default;

	  protected:
		void parseObject(const nlohmann::json& jsonObjectData, GuildScheduledEventUserData* pDataStructure);
	};

	class DiscordCoreAPI_Dll GuildScheduledEventUserDataVector : public DataParser<GuildScheduledEventUserDataVector> {
	  public:
		GuildScheduledEventUserDataVector() = default;

		operator std::vector<GuildScheduledEventUserData>();

		GuildScheduledEventUserDataVector& operator=(const nlohmann::json& jsonObjectData);

		GuildScheduledEventUserDataVector(const nlohmann::json& jsonObjectData);

		virtual ~GuildScheduledEventUserDataVector() = default;

	  protected:
		std::vector<GuildScheduledEventUserData> theGuildScheduledEventUserDatas{};

		void parseObject(const nlohmann::json& jsonObjectData, GuildScheduledEventUserDataVector* pDataStructure);
	};

	/// Invite data. \brief Invite data.
	struct DiscordCoreAPI_Dll InviteData : public DataParser<InviteData> {
		TimeStamp<std::chrono::milliseconds> createdAt{};///< Time it was created at.
		TimeStamp<std::chrono::milliseconds> expiresAt{};///< When the invite expires.
		GuildScheduledEventData guildScheduledEvent{};///< Scheduled Guild event.
		int32_t approximatePresenceCount{ 0 };///< Approximate presence count.
		ApplicationData targetApplication{};///< Application data.
		int32_t approximateMemberCount{ 0 };///< Approximate member count.
		StageInstanceData stageInstance{};///< Stage instance data.
		int32_t targetType{ 0 };///< Target type.
		bool temporary{ false };///< Is it temporary?
		UserData targetUser{};///< Target User of the invite.
		ChannelData channel{};///< Channel data of the Channel that the invite is for.
		int32_t maxUses{ 0 };///< Max number of uses.
		int32_t maxAge{ 0 };///< Maximum age of the invite.
		Snowflake guildId{};///< The Guild this invite is for.
		std::string code{};///< Unique invite code.
		UserData inviter{};///< The User who created the invite.
		GuildData guild{};///< Guild data of the Channel that the invite is for.
		int32_t uses{ 0 };///< The current number of uses.

		InviteData() = default;

		InviteData& operator=(const nlohmann::json& jsonObjectData);

		InviteData(const nlohmann::json& jsonObjectData);

		virtual ~InviteData() = default;

	  protected:
		void parseObject(const nlohmann::json& jsonObjectData, InviteData* pDataStructure);
	};

	class DiscordCoreAPI_Dll InviteDataVector : public DataParser<InviteDataVector> {
	  public:
		InviteDataVector() = default;

		operator std::vector<InviteData>();

		InviteDataVector& operator=(const nlohmann::json& jsonObjectData);

		InviteDataVector(const nlohmann::json& jsonObjectData);

		virtual ~InviteDataVector() = default;

	  protected:
		std::vector<InviteData> theInviteDatas{};

		void parseObject(const nlohmann::json& jsonObjectData, InviteDataVector* pDataStructure);
	};

	/// Represents a Guild Template. \brief Represents a Guild Template.
	struct DiscordCoreAPI_Dll GuildTemplateData : public DataParser<GuildTemplateData> {
		GuildData serializedSourceGuild{};///< The Guild snapshot this template contains.
		std::string sourceGuildId{};///< The ID of the Guild this template is based on.
		std::string description{};///< The description for the template.
		uint32_t usageCount{ 0 };///< Number of times this template has been used.
		std::string creatorId{};///< The ID of the User who created the template.
		std::string createdAt{};///< When this template was created.
		std::string updatedAt{};///< When this template was last synced to the source Guild.
		bool isDirty{ false };///< Whether the template has unsynced changes.
		std::string code{};///< The template code(unique ID).
		std::string name{};///< Template name.
		UserData creator{};///< The User who created the template.

		GuildTemplateData() = default;

		GuildTemplateData& operator=(const nlohmann::json& jsonObjectData);

		GuildTemplateData(const nlohmann::json& jsonObjectData);

		virtual ~GuildTemplateData() = default;

	  protected:
		void parseObject(const nlohmann::json& jsonObjectData, GuildTemplateData* pDataStructure);
	};

	class DiscordCoreAPI_Dll GuildTemplateDataVector : public DataParser<GuildTemplateDataVector> {
	  public:
		GuildTemplateDataVector() = default;

		operator std::vector<GuildTemplateData>();

		GuildTemplateDataVector& operator=(const nlohmann::json& jsonObjectData);

		GuildTemplateDataVector(const nlohmann::json& jsonObjectData);

		virtual ~GuildTemplateDataVector() = default;

	  protected:
		std::vector<GuildTemplateData> theGuildTemplateDatas{};

		void parseObject(const nlohmann::json& jsonObjectData, GuildTemplateDataVector* pDataStructure);
	};

	/// Invite target types. \brief Invite target types.
	enum class InviteTargetTypes : int8_t {
		Stream = 1,///< Stream.
		Embedded_Application = 2///< Embedded application.
	};

	/// WebHook types. \brief WebHook types.
	enum class WebHookType : int8_t {
		Incoming = 1,///< Incoming.
		Channel_Follower = 2,///< Channel follower.
		Application = 3///< Application.
	};

	/// WebHook data. \brief WebHook data.
	class DiscordCoreAPI_Dll WebHookData : public DiscordEntity, public DataParser<WebHookData> {
	  public:
		ChannelData sourceChannel{};///< Channel for which th WebHook was issued.
		Snowflake applicationId{};///< Application id.
		GuildData sourceGuild{};///< Source Guild id.
		WebHookType type{ 0 };///< Type of WebHook.
		std::string avatar{};///< Avatar of the WebHook.
		Snowflake channelId{};///< Channel id for which the WebHook was issued.
		std::string token{};///< Token of the WebHook.
		Snowflake guildId{};///< Guild id for which the WebHook was issued.
		std::string name{};///< Name of the WebHook.
		std::string url{};///< Url of the WebHook.
		UserData user{};///< User which create the WebHook.

		WebHookData() = default;

		WebHookData& operator=(const nlohmann::json& jsonObjectData);

		WebHookData(const nlohmann::json& jsonObjectData);

		virtual ~WebHookData() = default;

	  protected:
		void parseObject(const nlohmann::json& jsonObjectData, WebHookData* pDataStructure);
	};

	class DiscordCoreAPI_Dll WebHookDataVector : public DataParser<WebHookDataVector> {
	  public:
		WebHookDataVector() = default;

		operator std::vector<WebHookData>();

		WebHookDataVector& operator=(const nlohmann::json& jsonObjectData);

		WebHookDataVector(const nlohmann::json& jsonObjectData);
		virtual ~WebHookDataVector() = default;


	  protected:
		std::vector<WebHookData> theWebHookDatas{};

		void parseObject(const nlohmann::json& jsonObjectData, WebHookDataVector* pDataStructure);
	};

	/// Audit log data. \brief Audit log data.
	class DiscordCoreAPI_Dll AuditLogData : public DataParser<AuditLogData> {
	  public:
		std::vector<GuildScheduledEventData> guildScheduledEvents{};///< Array of guild scheduled event objects.
		std::vector<AutoModerationRuleData> autoModerationRules{};///< List of auto moderation rules referenced in the audit log.
		std::vector<AuditLogEntryData> auditLogEntries{};///< Array of audit log entry objects.
		std::vector<IntegrationData> integrations{};///< Array of partial integration objects.
		std::vector<WebHookData> webhooks{};///< Array of webhook objects.
		std::vector<ChannelData> threads{};///< Array of thread-specific channel objects.
		std::vector<UserData> users{};///< Array of user objects.

		auto getAuditLogData(const Snowflake& userIdOfChanger, AuditLogEvent auditLogType);

		auto getAuditLogData(AuditLogEvent auditLogType, const Snowflake& userIdOfTarget);

		AuditLogData() = default;

		AuditLogData& operator=(const nlohmann::json& jsonObjectData);

		AuditLogData(const nlohmann::json& jsonObjectData);

		virtual ~AuditLogData() = default;

	  protected:
		void parseObject(const nlohmann::json& jsonObjectData, AuditLogData* pDataStructure);
	};

	/// For removing a reaction. \brief For removing a reaction.
	struct DiscordCoreAPI_Dll ReactionRemoveData : public DataParser<ReactionRemoveData> {
		Snowflake channelId{};
		Snowflake messageId{};
		Snowflake guildId{};
		Snowflake userId{};
		EmojiData emoji{};

		ReactionRemoveData() = default;

		ReactionRemoveData& operator=(const nlohmann::json& jsonObjectData);

		ReactionRemoveData(const nlohmann::json& jsonObjectData);

		virtual ~ReactionRemoveData() = default;

	  protected:
		void parseObject(const nlohmann::json& jsonObjectData, ReactionRemoveData* pDataStructure);
	};

	/// For storing Interaction-related values. \brief For storing Interaction-related values.
	struct DiscordCoreAPI_Dll InteractionPackageData {
		std::string interactionToken{};
		Snowflake applicationId{};
		Snowflake interactionId{};
	};

	/// For storing Message-related values. \brief For storing Message-related values.
	struct DiscordCoreAPI_Dll MessagePackageData {
		Snowflake channelId{};
		Snowflake messageId{};
	};

	/// Data structure representing an ApplicationCommand's option choice. \brief Data structure representing an ApplicationCommand's option choice.
	class DiscordCoreAPI_Dll ApplicationCommandOptionChoiceData : public DataParser<ApplicationCommandOptionChoiceData> {
	  public:
		std::unordered_map<std::string, std::string> nameLocalizations{};///< Dictionary with keys in available locales Localization dictionary for the name field.
		nlohmann::json value{};///< The value of the option.
		std::string name{};///< The name of the current choice.

		ApplicationCommandOptionChoiceData() = default;

		operator nlohmann::json();

		ApplicationCommandOptionChoiceData& operator=(const nlohmann::json& jsonObjectData);

		ApplicationCommandOptionChoiceData(const nlohmann::json& jsonObjectData);

		virtual ~ApplicationCommandOptionChoiceData() = default;

	  protected:
		void parseObject(const nlohmann::json& jsonObjectData, ApplicationCommandOptionChoiceData* pDataStructure);
	};

	/// Data structure representing an ApplicationCommand's option. \brief Data structure representing an ApplicationCommand's option.
	struct DiscordCoreAPI_Dll ApplicationCommandOptionData : public DataParser<ApplicationCommandOptionData> {
		std::unordered_map<std::string, std::string> descriptionLocalizations{};///< Dictionary for the description localizations field.
		std::unordered_map<std::string, std::string> nameLocalizations{};///< Dictionary for the name localizations field.
		std::vector<ApplicationCommandOptionChoiceData> choices{};///< A vector of possible choices for the current ApplicationCommand option.
		std::vector<ApplicationCommandOptionData> options{};///< A vector of possible options for the current ApplicationCommand option.
		std::vector<ChannelType> channelTypes{};///< Set when the ApplicationCommand option type is set to Channel.
		ApplicationCommandOptionType type{};///< The type of command option.
		bool autocomplete{ false };///< If autocomplete interactions are enabled for this STRING, INTEGER, or NUMBER type option.
		std::string description{};///< A description of the current ApplicationCommand option.
		bool required{ false };///< If the parameter is required or optional-- default false.
		int32_t minValue{ 0 };///< If the option is an INTEGER or NUMBER type, the minimum value permitted.
		int32_t maxValue{ 0 };///< If the option is an INTEGER or NUMBER type, the maximum value permitted.
		std::string name{};///< Name of the current ApplicationCommand option.

		ApplicationCommandOptionData() = default;

		operator nlohmann::json();

		ApplicationCommandOptionData& operator=(const nlohmann::json& jsonObjectData);

		ApplicationCommandOptionData(const nlohmann::json& jsonObjectData);

		virtual ~ApplicationCommandOptionData() = default;

	  protected:
		void parseObject(const nlohmann::json& jsonObjectData, ApplicationCommandOptionData* pDataStructure);
	};

	/// Representing "TypingStart" data. \brief Representing "TypingStart" data.
	struct DiscordCoreAPI_Dll TypingStartData : public DataParser<TypingStartData> {
		GuildMemberData member{};
		int32_t timestamp{ 0 };
		Snowflake channelId{};
		Snowflake guildId{};
		Snowflake userId{};

		TypingStartData() = default;

		TypingStartData& operator=(const nlohmann::json& jsonObjectData);

		TypingStartData(const nlohmann::json& jsonObjectData);

		virtual ~TypingStartData() = default;

	  protected:
		void parseObject(const nlohmann::json& jsonObjectData, TypingStartData* pDataStructure);
	};

	struct DiscordCoreAPI_Dll YouTubeFormat {
		std::string signatureCipher{};
		std::string audioSampleRate{};
		int32_t averageBitrate{ 0 };
		int64_t contentLength{ 0 };
		std::string audioQuality{};
		std::string downloadUrl{};
		std::string signature{};
		std::string mimeType{};
		std::string quality{};
		std::string codecs{};
		std::string aitags{};
		int32_t bitrate{ 0 };
		int32_t height{ 0 };
		int32_t width{ 0 };
		int32_t itag{ 0 };
		int32_t fps{ 0 };
	};

	/// YouTube format data. \brief YouTube format data.
	struct DiscordCoreAPI_Dll YouTubeFormatVector : public DataParser<YouTubeFormatVector> {
		YouTubeFormatVector() = default;

		operator std::vector<YouTubeFormat>();

		YouTubeFormatVector& operator=(const nlohmann::json& jsonObjectData);

		YouTubeFormatVector(const nlohmann::json& jsonObjectData);

		virtual ~YouTubeFormatVector() = default;

	  protected:
		std::vector<YouTubeFormat> theFormats{};

		void parseObject(const nlohmann::json& jsonObjectData, YouTubeFormatVector* pDataStructure);
	};

	/// Application command types. \brief Application command types.
	enum class ApplicationCommandType : int8_t {
		Chat_Input = 1,///< Chat input.
		User = 2,///< User.
		Message = 3///< Message.
	};

	/// User command Interaction data. \brief User command Interaction data.
	struct DiscordCoreAPI_Dll UserCommandInteractionData : public DataParser<UserCommandInteractionData> {
		Snowflake targetId{};///< The target User's id.

		UserCommandInteractionData() = default;

		UserCommandInteractionData& operator=(const nlohmann::json& jsonObjectData);

		UserCommandInteractionData(const nlohmann::json& jsonObjectData);

		virtual ~UserCommandInteractionData() = default;

	  protected:
		void parseObject(const nlohmann::json& jsonObjectData, UserCommandInteractionData* pDataStructure);
	};

	/// Message command interacction data. \brief Message command interacction data.
	struct DiscordCoreAPI_Dll MessageCommandInteractionData : public DataParser<MessageCommandInteractionData> {
		Snowflake targetId{};///< The target Message's id.

		MessageCommandInteractionData() = default;

		MessageCommandInteractionData& operator=(const nlohmann::json& jsonObjectData);

		MessageCommandInteractionData(const nlohmann::json& jsonObjectData);

		virtual ~MessageCommandInteractionData() = default;

	  protected:
		void parseObject(const nlohmann::json& jsonObjectData, MessageCommandInteractionData* pDataStructure);
	};

	/// Component types. \brief Component types.
	enum class ComponentType : int8_t {
		ActionRow = 1,///< A container for other components.
		Button = 2,///< A button object.
		SelectMenu = 3,///< A select menu for picking from choices.
		TextInput = 4///< A text input object
	};

	/// Component Interaction data. \brief Component Interaction data.
	struct DiscordCoreAPI_Dll ComponentInteractionData : public DataParser<ComponentInteractionData> {
		std::vector<std::string> values{};///< The values of the components.
		ComponentType componentType{};///< The type of component.
		std::string customId{};///< The custom id of the Interaction entity.

		ComponentInteractionData() = default;

		ComponentInteractionData& operator=(const nlohmann::json& jsonObjectData);

		ComponentInteractionData(const nlohmann::json& jsonObjectData);

		virtual ~ComponentInteractionData() = default;

	  protected:
		void parseObject(const nlohmann::json& jsonObjectData, ComponentInteractionData* pDataStructure);
	};

	/// Modal interaction data, for inputs from text modals. \brief Modal interaction data, for inputs from text modals.
	struct DiscordCoreAPI_Dll ModalInteractionData : public DataParser<ModalInteractionData> {
		std::string customIdSmall{};///< The custom id of a particular modal input.
		std::string customId{};///< The custom id of the Interaction entity.
		std::string value{};///< The input value of the modal.

		ModalInteractionData() = default;

		ModalInteractionData& operator=(const nlohmann::json& jsonObjectData);

		ModalInteractionData(const nlohmann::json& jsonObjectData);

		virtual ~ModalInteractionData() = default;

	  protected:
		void parseObject(const nlohmann::json& jsonObjectData, ModalInteractionData* pDataStructure);
	};

	/// Allowable mentions for a Message. \brief Allowable mentions for a Message.
	class DiscordCoreAPI_Dll AllowedMentionsData : public DataParser<AllowedMentionsData> {
	  public:
		std::vector<std::string> parse{};///< A vector of allowed mention types to parse from the content.
		std::vector<std::string> roles{};///< Array of role_ids to mention (Max size of 100)
		std::vector<std::string> users{};///< Array of user_ids to mention (Max size of 100)
		bool repliedUser{ false };///< For replies, whether to mention the author of the Message being replied to (default false).

		AllowedMentionsData() = default;

		operator nlohmann::json();

		AllowedMentionsData& operator=(const nlohmann::json& jsonObjectData);

		AllowedMentionsData(const nlohmann::json& jsonObjectData);

		virtual ~AllowedMentionsData() = default;

	  protected:
		void parseObject(const nlohmann::json& jsonObjectData, AllowedMentionsData* pDataStructure);
	};

	/// Interaction types. \brief Interaction types.
	enum class InteractionType : int8_t {
		Ping = 1,///< Ping.
		Application_Command = 2,///< Application command.
		Message_Component = 3,///< Message component.
		Application_Command_Autocomplete = 4,///< Application command autocomplete.
		Modal_Submit = 5///< Modal submission.
	};

	/// Represents a single selection from a select-menu. \brief Represents a single selection from a select-menu.
	class DiscordCoreAPI_Dll SelectOptionData : public DataParser<SelectOptionData> {
	  public:
		std::string description{};///< Description of the select-menu-option.
		bool _default{ false };///< Is it the default option?
		std::string label{};///< A visible label for the select-menu-option.
		std::string value{};///< A value for identifying the option.
		EmojiData emoji{};///< An optional emoji to put on it.

		SelectOptionData() = default;

		SelectOptionData& operator=(const nlohmann::json& jsonObjectData);

		SelectOptionData(const nlohmann::json& jsonObjectData);

		virtual ~SelectOptionData() = default;

	  protected:
		void parseObject(const nlohmann::json& jsonObjectData, SelectOptionData* pDataStructure);
	};

	/// Button styles. \brief Button styles.
	enum class ButtonStyle : int8_t {
		Primary = 1,///< Primary.
		Success = 3,///< Success.
		Secondary = 2,///< Secondary.
		Danger = 4,///< Danger.
		Link = 5///< Link.
	};

	/// Represents a single Message-component. \brief Represents a single Message-component.
	struct DiscordCoreAPI_Dll ComponentData {
		std::vector<SelectOptionData> options{};///< Aray of select options the choices in the select, max 25.
		std::string placeholder{};///< Custom placeholder text if nothing is selected, max 100 characters.
		std::string customId{};///< A developer-defined identifier for the component, max 100 characters.
		int32_t minValues{ 0 };///< The minimum number of items that must be chosen; default 1, min 0, max 25.
		int32_t maxValues{ 0 };///< The maximum number of items that can be chosen; default 1, max 25.
		bool disabled{ false };///< Whether the component is disabled, default false.
		int32_t minLength{ 0 };///< The minimum input length for a text input.
		int32_t maxLength{ 0 };///< The maximum input length for a text input.
		bool required{ false };///< Whether this component is required to be filled.
		ComponentType type{};///< Integer component type.
		std::string label{};///< The label for this component.
		std::string value{};///< A pre-filled value for this component.
		std::string title{};///< Url, for url types.
		int32_t style{};///< One of ButtonStyle, or TextInputStyle.
		std::string url{};///< Url, for url types.
		EmojiData emoji{};///< Emoji name, id, and animated.
	};

	/// Action row data of Message components. \brief Action row data of Message components.
	class DiscordCoreAPI_Dll ActionRowData : public DataParser<ActionRowData> {
	  public:
		std::vector<ComponentData> components{};///< Array of components to make up the action-row.

		ActionRowData() = default;

		ActionRowData& operator=(const nlohmann::json& jsonObjectData);

		operator nlohmann::json();

		ActionRowData(const nlohmann::json& jsonObjectData);

		virtual ~ActionRowData() = default;

	  protected:
		void parseObject(const nlohmann::json& jsonObjectData, ActionRowData* pDataStructure);
	};

	/// Interaction callback types. \brief Interaction callback types.
	enum class InteractionCallbackType : int8_t {
		Pong = 1,///< ACK a Ping.
		Channel_Message_With_Source = 4,///< Respond to an interaction with a message.
		Deferred_Channel_Message_With_Source = 5,///< ACK an interaction and edit a response later, the user sees a loading state.
		Deferred_Update_Message = 6,///< For components, ACK an interaction and edit the original message later; the user does not see a loading state.
		Update_Message = 7,///< For components, edit the message the component was attached to.
		Application_Command_Autocomplete_Result = 8,///< Respond to an autocomplete interaction with suggested choices.
		Modal = 9///< Respond to an interaction with a popup modal.
	};

	/// Interaction ApplicationCommand callback data. \brief Interaction ApplicationCommand callback data.
	struct DiscordCoreAPI_Dll InteractionCallbackData {
		std::vector<ApplicationCommandOptionChoiceData> choices{};///< Autocomplete choices(max of 25 choices).
		std::vector<AttachmentData> attachments{};///< Array of partial attachment objects attachment objects with filename and description.
		std::vector<ActionRowData> components{};///< Message components.
		AllowedMentionsData allowedMentions{};///< Allowed mentions data.
		std::vector<EmbedData> embeds{};///< Message embeds.
		std::vector<File> files{};///< Files for uploading.
		std::string customId{};///< A developer-defined identifier for the component, max 100 characters.
		std::string content{};///< Message content.
		std::string title{};///< The title of the popup modal.
		int32_t flags{ 0 };///< Flags.
		bool tts{ false };///< Is it TTS?
	};

	/// Data structure representing an ApplicationCommand. \brief Data structure representing an ApplicationCommand.
	class DiscordCoreAPI_Dll ApplicationCommandData : public DiscordEntity, public DataParser<ApplicationCommandData> {
	  public:
		std::unordered_map<std::string, std::string> descriptionLocalizations{};///< Dictionary with keys in available locales Localization dictionary for name field.
		std::unordered_map<std::string, std::string> nameLocalizations{};///< Dictionary with keys in available locales Localization dictionary for name field.
		std::vector<ApplicationCommandOptionData> options{};///< A vector of possible options for the current ApplicationCommand.
		std::string defaultMemberPermissions{};///< Set of permissions represented as a bit set all
		ApplicationCommandType type{};///< The type of ApplicationCommand.
		bool dmPermission{ false };///< Indicates whether the command is available in DMs with the app, only for globally - scoped commands.
		std::string description{};///< A description of the current ApplicationCommand.
		Snowflake applicationId{};///< The current application id.
		std::string version{};///< An autoincremented version.
		std::string name{};///< Name of the current ApplicationCommand.
		Snowflake guildId{};///< (Where applicable) a Guild id for which guild to assign this ApplicationCommand to.

		ApplicationCommandData() = default;

		ApplicationCommandData& operator=(const nlohmann::json& jsonObjectData);

		ApplicationCommandData(const nlohmann::json& jsonObjectData);

		virtual ~ApplicationCommandData() = default;

	  protected:
		void parseObject(const nlohmann::json& jsonObjectData, ApplicationCommandData* pDataStructure);
	};

	/// Channel mention data. \brief Channel mention data.
	class DiscordCoreAPI_Dll ChannelMentionData : public DiscordEntity, public DataParser<ChannelMentionData> {
	  public:
		Snowflake guildId{};///< The id of the Guild where it took place.
		std::string name{};///< The name of the Channel that was mentioned.
		ChannelType type{};///< The type of Channel that was mentioned.

		ChannelMentionData() = default;

		ChannelMentionData& operator=(const nlohmann::json& jsonObjectData);

		ChannelMentionData(const nlohmann::json& jsonObjectData);

		virtual ~ChannelMentionData() = default;

	  protected:
		void parseObject(const nlohmann::json& jsonObjectData, ChannelMentionData* pDataStructure);
	};

	/// Data for when some Channel pins are updated. \brief Data for when some Channel pins are updated.
	struct DiscordCoreAPI_Dll ChannelPinsUpdateEventData : public DataParser<ChannelPinsUpdateEventData> {
		TimeStamp<std::chrono::milliseconds> lastPinTimestamp{ "" };///< The time of the last pinned Message.
		Snowflake channelId{};///< The id of the Channel within which the Message was pinned.
		Snowflake guildId{};///< The id of the Guild within which the Message was pinned.

		ChannelPinsUpdateEventData() = default;

		ChannelPinsUpdateEventData& operator=(const nlohmann::json& jsonObjectData);

		ChannelPinsUpdateEventData(const nlohmann::json& jsonObjectData);

		virtual ~ChannelPinsUpdateEventData() = default;

	  protected:
		void parseObject(const nlohmann::json& jsonObjectData, ChannelPinsUpdateEventData* pDataStructre);
	};

	/// Data for when threads are synced. \brief Data for when threads are synced.
	struct DiscordCoreAPI_Dll ThreadListSyncData : public DataParser<ThreadListSyncData> {
		std::vector<ThreadMemberData> members{};///< Array of members that are a part of the Thread.
		std::vector<std::string> channelIds{};///< The parent Channel ids whose threads are being synced. If omitted, then threads were synced for entire Guild.
		std::vector<ChannelData> threads{};///< All active threads in the given channels that the current User can access.
		Snowflake guildId{};///< The id of the Guild for which the threads are being synced.

		ThreadListSyncData() = default;

		ThreadListSyncData& operator=(const nlohmann::json& jsonObjectData);

		ThreadListSyncData(const nlohmann::json& jsonObjectData);

		virtual ~ThreadListSyncData() = default;

	  protected:
		void parseObject(const nlohmann::json& jsonObjectData, ThreadListSyncData* pDataStructure);
	};

	/// Represents a Thread-members-update. \brief Represents a Thread-members-update.
	class DiscordCoreAPI_Dll ThreadMembersUpdateData : public DiscordEntity, public DataParser<ThreadMembersUpdateData> {
	  public:
		std::vector<ThreadMemberData> addedMembers{};///< New members added to the Thread.
		std::vector<std::string> removedMemberIds{};///< Members who have been removed.
		int32_t memberCount{ 0 };///< Number of Guild-members in the Thread.
		Snowflake guildId{};///< Guild id of the Thread.

		ThreadMembersUpdateData() = default;

		ThreadMembersUpdateData& operator=(const nlohmann::json& jsonObjectData);

		ThreadMembersUpdateData(const nlohmann::json& jsonObjectData);

		virtual ~ThreadMembersUpdateData() = default;

	  protected:
		void parseObject(const nlohmann::json& jsonObjectData, ThreadMembersUpdateData* pDataStructure);
	};

	/// Message Interaction data. \brief Message Interaction data.
	class DiscordCoreAPI_Dll MessageInteractionData : public DiscordEntity, public DataParser<MessageInteractionData> {
	  public:
		GuildMemberData member{};
		InteractionType type{};
		std::string name{};
		UserData user{};

		MessageInteractionData() = default;

		MessageInteractionData& operator=(const nlohmann::json& jsonObjectData);

		MessageInteractionData(const nlohmann::json& jsonObjectData);

		virtual ~MessageInteractionData() = default;

	  protected:
		void parseObject(const nlohmann::json& jsonObjectData, MessageInteractionData* pDataStructure);
	};

	/// Message types. \brief Message types.
	enum class MessageType : int8_t {
		Default = 0,///< Default.
		Recipient_Add = 1,///< Recipient add.
		Recipient_Remove = 2,///< Recipient remove.
		Call = 3,///< Call.
		Channel_Name_Change = 4,///< Channel name change.
		Channel_Icon_Change = 5,///< Channel icon change.
		Channel_Pinned_Message = 6,///< Channel pinned Message.
		Guild_Member_Join = 7,///< Guild memeber join.
		User_Premium_Guild_Subscription = 8,///< User premium Guild subscription.
		User_Premium_Guild_Subscription_Tier_1 = 9,///< User premium Guild subscription tier 1.
		User_Premium_Guild_Subscription_Tier_2 = 10,///< User premium Guild subscription tier 2.
		User_Premium_Guild_Subscription_Tier_3 = 11,///< User premium Guild subscription tier 3.
		Channel_Follow_Add = 12,///< Channel follow add.
		Guild_Discovery_Disqualified = 14,///< Guild discovery disqualified.
		Guild_Discovery_Requalified = 15,///< Guild discovery requalified.
		Guild_Discovery_Grace_Period_Initial_Warning = 16,///< Guild discovery grade period initial warning.
		Guild_Discovery_Grace_Period_Final_Warning = 17,///< Guild discovery grade period final warning.
		Thread_Created = 18,///< Thread created.
		Reply = 19,///< Reply.
		Chat_Input_Command = 20,///< Chat input command.
		Thread_Starter_Message = 21,///< Thread starter Message.
		Guild_Invite_Reminder = 22,///< Guild invite reminder.
		Context_Menu_Command = 23///< Context menu command.
	};

	/// Message flags. \brief Message flags.
	enum class MessageFlags : uint8_t {
		Crossposted = 1 << 0,///< Crossposted.
		Is_Crosspost = 1 << 1,///< Is crosspost.
		Suppress_Embeds = 1 << 2,///< Supress embeds.
		Source_Message_Deleted = 1 << 3,///< Source Message deleted.
		Urgent = 1 << 4,///< Urgent.
		Has_Thread = 1 << 5,///< Has Thread.
		Ephemeral = 1 << 6,///< Ephemeral.
		Loading = 1 << 7///< Loading.
	};

	/// Sticker item types. \brief Sticker item types.
	enum class StickerItemType : int8_t {
		Png = 1,///< Png.
		Apng = 2,///< Apng.
		Lottie = 3///< Lottie.
	};

	/// Represents a forum thread message. \brief Represents a forum thread message.
	struct DiscordCoreAPI_Dll ForumThreadMessageData {
		std::vector<AttachmentData> attachments{};///< Array of partial attachment objects attachment objects with filename.
		std::vector<ActionRowData> components{};///< Array of message component objects the components to include with the message.
		AllowedMentionsData allowedMentions{};///< Allowed mention object allowed mentions for the message.
		std::vector<std::string> stickerIds{};///< Array of snowflakes IDs of up to 3 stickers in the server to send in the message.
		std::vector<EmbedData> embeds{};///< Array of embed objects	embedded rich content (up to 6000 characters).
		std::vector<File> files{};///< File contents the contents of the file being sent one of content, file, embed(s), sticker_ids.
		std::string content{};///< The message contents (up to 2000 characters).
		int32_t flags{ 0 };///< Flags to be set for the message.
	};

	/// Message Sticker item data. \brief Message Sticker item data.
	class DiscordCoreAPI_Dll StickerItemData : public DiscordEntity, public DataParser<StickerItemData> {
	  public:
		StickerItemType formatType{};///< Message Sticker item type.
		std::string name{};///< The name of the Sticker.

		StickerItemData() = default;

		StickerItemData& operator=(const nlohmann::json& jsonObjectData);

		StickerItemData(const nlohmann::json& jsonObjectData);

		virtual ~StickerItemData() = default;

	  protected:
		void parseObject(const nlohmann::json& jsonObjectData, StickerItemData* pDataStructure);
	};

	/// The core of a Message's data structure. \brief The core of a Message's data structure.
	class DiscordCoreAPI_Dll MessageDataOld : public DiscordEntity, public DataParser<MessageDataOld> {
	  public:
		TimeStamp<std::chrono::milliseconds> editedTimestamp{};///< The time at which it was edited.
		std::vector<ChannelMentionData> mentionChannels{};///< array of Channel mention data.
		TimeStamp<std::chrono::milliseconds> timestamp{};///< The timestamp of when the Message was created.
		std::vector<StickerItemData> stickerItems{};///< Array of Message Sticker item data.
		std::vector<AttachmentData> attachments{};///< Array of attachment data.
		MessageReferenceData messageReference{};///< Message reference data.
		std::vector<std::string> mentionRoles{};///< Vector of "mention roles" ids.
		std::vector<ActionRowData> components{};///< Array of action row data.
		std::vector<ReactionData> reactions{};//< Array of reaction data.
		MessageInteractionData interaction{};///< Message Interaction data.
		std::vector<StickerData> stickers{};///< Array of Message Sticker data.
		std::vector<UserData> mentions{};///< Array of User data, for individual's that were mentioned.
		std::vector<EmbedData> embeds{};///< Array of Message embeds.
		MessageActivityData activity{};///< Message activity data.
		bool mentionEveryone{ false };///< Does the Message mention everyone?
		ApplicationData application{};///< Application data.
		Snowflake applicationId{};///< Application id.
		GuildMemberData member{};///< The author's Guild member data.
		std::string content{};///< The Message's content.
		Snowflake channelId{};///< The Channel it was sent in.
		Snowflake webHookId{};///< WebHook id of the Message, if applicable.
		bool pinned{ false };///< Is it pinned?
		ChannelData thread{};///< The Thread that the Message was sent in, if applicable.
		std::string nonce{};///< Nonce.
		int32_t flags{ 0 };///< Flags.
		Snowflake guildId{};///< The id of the Guild the Message was sent in.
		MessageType type{};///< Message type.
		UserData author{};///< The author's User data.
		bool tts{ false };///< Is it a text-to-speech Message?

		MessageDataOld() = default;

		MessageDataOld& operator=(const nlohmann::json& jsonObjectData);

		MessageDataOld(const nlohmann::json& jsonObjectData);

		virtual ~MessageDataOld() = default;

	  protected:
		void parseObject(const nlohmann::json& jsonObjectData, MessageDataOld* pDataStructure);
	};

	/// Data structure representing a single Message. \brief Data structure representing a single Message.
	class DiscordCoreAPI_Dll MessageData : public MessageDataOld, public DataParser<MessageData> {
	  public:
		std::unique_ptr<MessageDataOld> referencedMessage{ std::make_unique<MessageDataOld>() };///< The referenced Message, to reply to.

		MessageData& operator=(const MessageData& other);

		MessageData(const MessageData& other);

		MessageData() = default;

		MessageData& operator=(const nlohmann::json& jsonObjectData);

		MessageData(const nlohmann::json& jsonObjectData);

		virtual ~MessageData() = default;

	  protected:
		void parseObject(const nlohmann::json& jsonObjectData, MessageData* pDataStructure);
	};

	/// Resolved data. \brief Resolved data.
	struct DiscordCoreAPI_Dll ResolvedData {
		std::unordered_map<Snowflake, AttachmentData> attachments{};///< Map of Snowflakes to attachment objects the ids and attachment objects.
		std::unordered_map<Snowflake, GuildMemberData> members{};///< Map full of GuildMemeberData.
		std::unordered_map<Snowflake, MessageData> messages{};///< Map full of messageData->
		std::unordered_map<Snowflake, ChannelData> channels{};///< Map full of ChannelData.
		std::unordered_map<Snowflake, UserData> users{};///< Map full of UserData.
		std::unordered_map<Snowflake, RoleData> roles{};///< Map full of RoleData.
	};

	/// Represents a Sticker pack. \brief Represents a Sticker pack.
	struct DiscordCoreAPI_Dll StickerPackData : public DataParser<StickerPackData> {
		std::vector<StickerData> stickers{};///< Array of Sticker objects	the stickers in the pack.
		std::string coverStickerId{};///< Id of a Sticker in the pack which is shown as the pack's icon.
		std::string bannerAssetId{};///< Id of the Sticker pack's banner image.
		std::string description{};///< Description of the Sticker pack.
		std::string skuId{};///< Id of the pack's SKU.
		std::string name{};///< Name of the Sticker pack.
		std::string Id{};///< Id of the Sticker pack.

		StickerPackData() = default;

		StickerPackData& operator=(const nlohmann::json& jsonObjectData);

		StickerPackData(const nlohmann::json& jsonObjectData);

		virtual ~StickerPackData() = default;

	  protected:
		void parseObject(const nlohmann::json& jsonObjectData, StickerPackData* pDataStructure);
	};

	class DiscordCoreAPI_Dll StickerPackDataVector : public DataParser<StickerPackDataVector> {
	  public:
		StickerPackDataVector() = default;

		operator std::vector<StickerPackData>();

		StickerPackDataVector& operator=(const nlohmann::json& jsonObjectData);

		StickerPackDataVector(const nlohmann::json& jsonObjectData);

		virtual ~StickerPackDataVector() = default;

	  protected:
		std::vector<StickerPackData> theStickerPackDatas{};

		void parseObject(const nlohmann::json& jsonObjectData, StickerPackDataVector* pDataStructure);
	};

	/// Connection visibility types. \brief Connection visibility types.
	enum class ConnectionVisibilityTypes : int8_t {
		None = 0,///< None.
		Everyone = 1///< Everyone.
	};

	/// Represents a single User Connection. \brief Represents a single User Connection.
	struct DiscordCoreAPI_Dll ConnectionData : public DataParser<ConnectionData> {
		std::vector<IntegrationData> integrations{};///< An array of partial server integrations.
		ConnectionVisibilityTypes visibility{};///< Visibility of this connection.
		bool showActivity{ false };///< Whether activities related to this connection will be shown in presence updates.
		bool friendSync{ false };///< Whether friend sync is enabled for this connection.
		bool verified{ false };///< Whether the connection is verified.
		bool revoked{ false };///< Whether the connection is revoked.
		std::string name{};///< The userName of the connection account.
		std::string type{};///< The service of the connection(twitch, youtube).
		Snowflake id{};///< Id of the connection account.

		ConnectionData() = default;

		ConnectionData& operator=(const nlohmann::json& jsonObjectData);

		ConnectionData(const nlohmann::json& jsonObjectData);

		virtual ~ConnectionData() = default;

	  protected:
		void parseObject(const nlohmann::json& jsonObjectData, ConnectionData* pDataStructure);
	};

	class DiscordCoreAPI_Dll ConnectionDataVector : public DataParser<ConnectionDataVector> {
	  public:
		ConnectionDataVector() = default;

		operator std::vector<ConnectionData>();

		ConnectionDataVector& operator=(const nlohmann::json& jsonObjectData);

		ConnectionDataVector(const nlohmann::json& jsonObjectData);

		virtual ~ConnectionDataVector() = default;

	  protected:
		std::vector<ConnectionData> theConnectionDatas{};

		void parseObject(const nlohmann::json& jsonObjectData, ConnectionDataVector* pDataStructure);
	};

	/// ApplicationCommand Interaction data option. \brief ApplicationCommand Interaction data option.
	struct DiscordCoreAPI_Dll ApplicationCommandInteractionDataOption : public DataParser<ApplicationCommandInteractionDataOption> {
		std::vector<ApplicationCommandInteractionDataOption> options{};///< ApplicationCommand Interaction data options.
		ApplicationCommandOptionType type{};///< The type of ApplicationCommand options.
		std::string valueString{};///< The value if it's a string.
		bool valueBool{ false };///< the value if it's a bool.
		int32_t valueInt{ 0 };///< The value if it's an int32_t.
		bool focused{ false };///< 	True if this option is the currently focused option for autocomplete.
		std::string name{};///< The name of the current option.

		ApplicationCommandInteractionDataOption() = default;

		ApplicationCommandInteractionDataOption& operator=(const nlohmann::json& jsonObjectData);

		ApplicationCommandInteractionDataOption(const nlohmann::json& jsonObjectData);

		virtual ~ApplicationCommandInteractionDataOption() = default;

	  protected:
		void parseObject(const nlohmann::json& jsonObjectData, ApplicationCommandInteractionDataOption* pDataStructure);
	};

	/// ApplicationCommand Interaction data. \brief ApplicationCommand Interaction data.
	class DiscordCoreAPI_Dll ApplicationCommandInteractionData : public DiscordEntity, public DataParser<ApplicationCommandInteractionData> {
	  public:
		std::vector<ApplicationCommandInteractionDataOption> options{};///< ApplicationCommand Interaction data options.
		ApplicationCommandType type{};///< The type of ApplicationCommand.
		ResolvedData resolved{};///< Resolved data.
		Snowflake guildId{};///< The guild that the command took place in.
		std::string name{};///< The name of the command.

		ApplicationCommandInteractionData() = default;

		ApplicationCommandInteractionData& operator=(const nlohmann::json& jsonObjectData);

		ApplicationCommandInteractionData(const nlohmann::json& jsonObjectData);

		virtual ~ApplicationCommandInteractionData() = default;

	  protected:
		void parseObject(const nlohmann::json& jsonObjectData, ApplicationCommandInteractionData* pDataStructure);
	};

	/// Interaction data data. \brief Interaction data data.
	struct DiscordCoreAPI_Dll InteractionDataData : public DataParser<InteractionDataData> {
		ApplicationCommandInteractionData applicationCommandData{};///< ApplicationCommand Interaction data.
		MessageCommandInteractionData messageInteractionData{};///< Message command Interaction data.
		UserCommandInteractionData userInteractionData{};///< User command Interaction data.
		ComponentInteractionData componentData{};///< Component Interaction data.
		ModalInteractionData modalData{};///< Modal Interaction data.

		InteractionDataData() = default;

		InteractionDataData& operator=(const nlohmann::json& jsonObjectData);

		InteractionDataData(const nlohmann::json& jsonObjectData);

		virtual ~InteractionDataData() = default;

	  protected:
		void parseObject(const nlohmann::json& jsonObjectData, InteractionDataData* pDataStructure);
	};

	/// Interaction data. \brief Interaction data.
	class DiscordCoreAPI_Dll InteractionData : public DiscordEntity, public DataParser<InteractionData> {
	  public:
		std::string appPermissions{};///< Bitwise set of permissions the app or bot has within the channel the interaction was sent from.
		InteractionDataData data{};///< The Interaction's data.
		std::string guildLocale{};///< The guild's preferred locale, if invoked in a guild.
		Snowflake applicationId{};///< The application's id.
		nlohmann::json rawData{};///< The Interaction's raw data.
		GuildMemberData member{};///< The data of the Guild member who sent the Interaction, if applicable.
		InteractionType type{};///< The type of Interaction.
		MessageData message{};///< The Message that the Interaction came through on, if applicable.
		Snowflake channelId{};///< The Channel the Interaction was sent in.
		int32_t version{ 0 };///< The Interaction version.
		std::string locale{};///< The selected language of the invoking user.
		std::string token{};///< The Interaction token.
		Snowflake guildId{};///< The Guild id of the Guild it was sent in.
		UserData user{};///< The User data of the sender of the Interaction.

		InteractionData() = default;

		InteractionData& operator=(const nlohmann::json& jsonObjectData);

		InteractionData(const nlohmann::json& jsonObjectData);

		virtual ~InteractionData() = default;

	  protected:
		void parseObject(const nlohmann::json& jsonObjectData, InteractionData* pDataStructure);
	};

	/// Data from the SessionStart info. \brief Data from the SessionStart info.
	struct DiscordCoreAPI_Dll SessionStartData : public DataParser<SessionStartData> {
		uint32_t maxConcurrency{ 0 };///< The number of identify requests allowed per 5 seconds.
		uint32_t resetAfter{ 0 };///< The number of std::chrono::milliseconds after which the limit resets.
		uint32_t remaining{ 0 };///< The remaining number of session starts the current User is allowed.
		uint32_t total{ 0 };///< The total number of session starts the current User is allowed.

		SessionStartData() = default;

		SessionStartData& operator=(const nlohmann::json& jsonObjectData);

		SessionStartData(const nlohmann::json& jsonObjectData);

		virtual ~SessionStartData() = default;

	  protected:
		void parseObject(const nlohmann::json& jsonObjectData, SessionStartData* pDataStructure);
	};

	/// Data from the GetGatewatBot endpoint. \brief Data from the GetGatewatBot endpoint.
	struct DiscordCoreAPI_Dll GatewayBotData : public DataParser<GatewayBotData> {
		SessionStartData sessionStartLimit{};///< Information on the current session start limit.
		uint32_t shards{ 0 };///< The recommended number of shards to use when connecting.
		std::string url{};///< The WSS Url that can be used for connecting to the gateway.

		GatewayBotData() = default;

		GatewayBotData& operator=(const nlohmann::json& jsonObjectData);

		GatewayBotData(const nlohmann::json& jsonObjectData);

		virtual ~GatewayBotData() = default;

	  protected:
		void parseObject(const nlohmann::json& jsonObjectData, GatewayBotData* pDataStructure);
	};

	/// Text input style for modals. \brief Text input style for modals.
	enum class TextInputStyle : int8_t {
		Short = 1,///< A single-line input.
		Paragraph = 2///< A multi-line input.
	};

	/// Data representing a Guild Emoji Update event. \brief Data representing a Guild Emoji Update event.
	struct DiscordCoreAPI_Dll GuildEmojisUpdateEventData : public DataParser<GuildEmojisUpdateEventData> {
		std::vector<EmojiData> emojis{};
		Snowflake guildId{};

		GuildEmojisUpdateEventData() = default;

		GuildEmojisUpdateEventData& operator=(const nlohmann::json& jsonObjectData);

		GuildEmojisUpdateEventData(const nlohmann::json& jsonObjectData);

		virtual ~GuildEmojisUpdateEventData() = default;

	  protected:
		void parseObject(const nlohmann::json& jsonObjectData, GuildEmojisUpdateEventData* pDataStructure);
	};

	/// Data representing a Guild Sticker Update event. \brief Data representing a Guild Stickers Update event.
	struct DiscordCoreAPI_Dll GuildStickersUpdateEventData : public DataParser<GuildStickersUpdateEventData> {
		std::vector<StickerData> stickers{};
		Snowflake guildId{};

		GuildStickersUpdateEventData() = default;

		GuildStickersUpdateEventData& operator=(const nlohmann::json& jsonObjectData);

		GuildStickersUpdateEventData(const nlohmann::json& jsonObjectData);

		virtual ~GuildStickersUpdateEventData() = default;

	  protected:
		void parseObject(const nlohmann::json& jsonObjectData, GuildStickersUpdateEventData* pDataStructure);
	};

	struct DiscordCoreAPI_Dll GuildMembersChunkEventData : public DataParser<GuildMembersChunkEventData> {
		std::vector<PresenceUpdateData> presences{};
		std::vector<GuildMemberData> members{};
		std::vector<std::string> notFound{};
		int32_t chunkIndex{ 0 };
		int32_t chunkCount{ 0 };
		std::string nonce{};
		Snowflake guildId{};

		GuildMembersChunkEventData() = default;

		GuildMembersChunkEventData& operator=(const nlohmann::json& jsonObjectData);

		GuildMembersChunkEventData(const nlohmann::json& jsonObjectData);

		virtual ~GuildMembersChunkEventData() = default;

	  protected:
		void parseObject(const nlohmann::json& jsonObjectData, GuildMembersChunkEventData* pDataStructure);
	};

	/// Data representing an input-event, which is any Message or Interaction that is coming into the bot as an input. \brief Data representing an input-event, which is any Message or Interaction that is coming into the bot as an input.
	class DiscordCoreAPI_Dll InputEventData {
	  public:
		friend struct OnInteractionCreationData;
		friend struct BaseFunctionArguments;

		friend class DiscordCoreInternal::WebSocketSSLShard;
		friend class DiscordCoreInternal::BaseSocketAgent;
		friend class RespondToInputEventData;
		friend class DiscordCoreClient;
		friend class EventHandler;
		friend class CommandData;
		friend class InputEvents;

		InputEventResponseType responseType{};///< The type of response that this input value represents.

		InputEventData& operator=(InputEventData&& other) noexcept;

		InputEventData(InputEventData&& other) noexcept;

		InputEventData& operator=(const InputEventData& other) noexcept;

		InputEventData(const InputEventData& other) noexcept;

		InputEventData& operator=(InputEventData& other) noexcept;

		InputEventData(InputEventData& other) noexcept;

		InputEventData& operator=(InteractionData& other) noexcept;

		InputEventData(InteractionData& interactionData) noexcept;

		InputEventData() = default;

		/// Returns the userName of the last User to trigger this input-event. \brief Returns the userName of the last User to trigger this input-event.
		/// \returns std::string A string containing the User name.
		std::string getUserName();

		/// Gets the avatar Url of the last User to trigger this input-event. \brief Gets the avatar Url of the last User to trigger this input-event.
		/// \returns std::string A string containing the avatar Url.
		std::string getAvatarUrl();

		/// Returns the Message embeds that are on the Message, if applicable. \brief Returns the Message embeds that are on the Message, if applicable.
		/// \returns std::vector A vector containing the EmbedData.
		std::vector<EmbedData> getEmbeds();

		/// Returns the Message components that are on the Message, if applicable. \brief Returns the Message components that are on the Message, if applicable.
		/// \returns std::vector A vector containing ActionRowData.
		std::vector<ActionRowData> getComponents();

		/// Returns the User id of the last requester of this input-event. \brief Returns the User id of the last requester of this input-event.
		/// \returns Snowflake A Snowflake containing the author's id.
		Snowflake getAuthorId();

		/// Returns the Interaction id, if appplicable, of this input-event. \brief Returns the Interaction id, if appplicable, of this input-event.
		/// \returns Snowflake A Snowflake containing the Interaction id.
		Snowflake getInteractionId();

		/// Returns the application id. \brief Returns the application id.
		/// \returns Snowflake A Snowflake containing the application id.
		Snowflake getApplicationId();

		/// Returns the Channel id of this input-event. \brief Returns the Channel id of this input-event.
		/// \returns Snowflake A Snowflake containing the Channel id.
		Snowflake getChannelId();

		/// Returns the Interaction token, if applicable, of this input-event. \brief Returns the Interaction token, if applicable, of this input-event.
		/// \returns std::string A string containing the Interaction token.
		std::string getInteractionToken();

		/// Returns the Guild id, of this input-event. \brief Returns the Guild id, of this input-event.
		/// \returns Snowflake A Snowflake containing the Guild id.
		Snowflake getGuildId();

		/// Returns the Message id, if applicable, of this input-event. \brief Returns the Message id, if applicable, of this input-event.
		/// \returns Snowflake A Snowflake containing the Message id.
		Snowflake getMessageId();

		/// Returns the Interaction data, if applicable, of this input-event. \brief Returns the InteractionData, if applicable, of this input-event.
		/// \returns InteractionData An InteractionData structure.
		InteractionData getInteractionData();

		/// Returns the Message data, if applicable, of this input-event. \brief Returns the Message data, if applicable, of this input-event.
		/// \returns MessageData A MessageData structure.
		MessageData getMessageData();

		virtual ~InputEventData() = default;

	  protected:
		std::unique_ptr<InteractionData> interactionData{ std::make_unique<InteractionData>() };
	};

	/// \brief Data for responding to an input-event.
	class DiscordCoreAPI_Dll RespondToInputEventData {
	  public:
		friend struct DeleteInteractionResponseData;
		friend struct DeleteFollowUpMessageData;
		friend struct InteractionResponseData;

		friend class CreateEphemeralInteractionResponseData;
		friend class CreateDeferredInteractionResponseData;
		friend class CreateEphemeralFollowUpMessageData;
		friend class CreateInteractionResponseData;
		friend class EditInteractionResponseData;
		friend class CreateFollowUpMessageData;
		friend class EditFollowUpMessageData;
		friend class CreateMessageData;
		friend class EditMessageData;
		friend class InputEvents;
		friend class SendDMData;

		RespondToInputEventData& operator=(InteractionData& dataPackage);

		RespondToInputEventData(InteractionData& dataPackage);

		RespondToInputEventData& operator=(InputEventData& dataPackage);

		RespondToInputEventData(InputEventData& dataPackage);

		/// Adds a button to the response Message. \brief Adds a button to the response Message.
		/// \param disabled Whether the button is active or not.
		/// \param customIdNew A custom id to give for identifying the button.
		/// \param buttonLabel A visible label for the button.
		/// \param buttonStyle The style of the button.
		/// \param emojiName An emoji name, if desired.
		/// \param emojiId An emoji id, if desired.
		/// \param url A url, if applicable.
		/// \returns RespondToInputEventData& A reference to this data structure.
		RespondToInputEventData& addButton(bool disabled, const std::string& customIdNew, const std::string& buttonLabel, ButtonStyle buttonStyle,
			const std::string& emojiName = "", Snowflake emojiId = 0, const std::string& url = "");

		/// Adds a select-menu to the response Message. \brief Adds a select-menu to the response Message.
		/// \param disabled Whether the select-menu is active or not.
		/// \param customIdNew A custom id to give for identifying the select-menu.
		/// \param options A vector of select-menu-options to offer.
		/// \param placeholder Custom placeholder text if nothing is selected, max 100 characters.
		/// \param maxValues Maximum number of selections that are possible.
		/// \param minValues Minimum required number of selections that are required.
		/// \returns RespondToInputEventData& A reference to this data structure.
		RespondToInputEventData& addSelectMenu(bool disabled, const std::string& customIdNew, std::vector<SelectOptionData> options, const std::string& placeholder,
			int32_t maxValues, int32_t minValues);

		/// Adds a modal to the response Message. \brief Adds a modal to the response Message.
		/// \param topTitleNew A title for the modal.
		/// \param topCustomIdNew A custom id to give for the modal.
		/// \param titleNew A title for the modal's individual input.
		/// \param customIdNew A custom id to give for the modal's individual input.
		/// \param required Is it a required response?
		/// \param minLength Minimum length.
		/// \param maxLength Maximum length.
		/// \param inputStyle The input style.
		/// \param label A label for the modal.
		/// \param placeholder A placeholder for the modal.
		/// \returns RespondToInputEventData& A reference to this data structure.
		RespondToInputEventData& addModal(const std::string& topTitleNew, const std::string& topCustomIdNew, const std::string& titleNew, const std::string& customIdNew,
			bool required, int32_t minLength, int32_t maxLength, TextInputStyle inputStyle, const std::string& label = "", const std::string& placeholder = "");

		/// Adds a file to the current collection of files for this message response. \brief Adds a file to the current collection of files for this message response.
		/// \param theFile The file to be added.
		/// \returns RespondToInputEventData& A reference to this data structure.
		RespondToInputEventData& addFile(File theFile);

		/// For setting the allowable mentions in a response. \brief For setting the allowable mentions in a response.
		/// \param dataPackage An AllowedMentionsData structure.
		/// \returns RespondToInputEventData& A reference to this data structure.
		RespondToInputEventData& addAllowedMentions(AllowedMentionsData dataPackage);

		/// For setting the type of response to make. \brief For setting the type of response to make.
		/// \param typeNew An InputEventResponseType.
		/// \returns RespondToInputEventData& A reference to this data structure.
		RespondToInputEventData& setResponseType(InputEventResponseType typeNew);

		/// For setting the components in a response. \brief For setting the components in a response.
		/// \param dataPackage An ActionRowData structure.
		/// \returns RespondToInputEventData& A reference to this data structure.
		RespondToInputEventData& addComponentRow(ActionRowData dataPackage);

		/// For setting the embeds in a response. \brief For setting the embeds in a response.
		/// \param dataPackage An EmbedData structure.
		/// \returns RespondToInputEventData& A reference to this data structure.
		RespondToInputEventData& addMessageEmbed(EmbedData dataPackage);

		/// For setting the Message content in a response. \brief For setting the Message content in a response.
		/// \param dataPackage A string, containing the content.
		/// \returns RespondToInputEventData& A reference to this data structure.
		RespondToInputEventData& addContent(const std::string& dataPackage);

		/// For setting the tts status of a response. \brief For setting the tts status of a response.
		/// \param enabledTTs A bool.
		/// \returns RespondToInputEventData& A reference to this data structure.
		RespondToInputEventData& setTTSStatus(bool enabledTTs);

		/// For setting the choices of an autocomplete response. \brief For setting the choices of an autocomplete response.
		/// \param theValue An nlohmann::json value that is either a float, int32_t or a string.
		/// \param theName A string for the name of the choice.
		/// \param theNameLocalizations A std::unordered_map<std::string, std::string> for the name localizations.
		/// \returns RespondToInputEventData& A reference to this data structure.
		RespondToInputEventData& setAutoCompleteChoice(nlohmann::json theValue, const std::string& theName,
			std::unordered_map<std::string, std::string> theNameLocalizations = std::unordered_map<std::string, std::string>{});

		/// For setting the direct-Message User target of a response. \brief For setting the direct-Message User target of a response.
		/// \param targetUserIdNew A string, containing the target User's id.
		/// \returns RespondToInputEventData& A reference to this data structure.
		RespondToInputEventData& setTargetUserID(const Snowflake& targetUserIdNew);

	  protected:
		std::vector<ApplicationCommandOptionChoiceData> choices{};
		std::vector<ActionRowData> components{};
		AllowedMentionsData allowedMentions{};
		std::vector<EmbedData> embeds{};
		std::string interactionToken{};
		InputEventResponseType type{};///< The type of response to make.
		InteractionType eventType{};
		std::vector<File> files{};
		Snowflake interactionId{};
		Snowflake applicationId{};
		Snowflake targetUserId{};
		std::string customId{};
		std::string content{};
		Snowflake channelId{};
		Snowflake messageId{};
		std::string title{};
		int32_t flags{ 0 };
		bool tts{ false };
	};

	/// Message response base, for responding to messages. \brief Message response base, for responding to messages.
	class DiscordCoreAPI_Dll MessageResponseBase {
	  public:
		/// Adds a button to the response Message. \brief Adds a button to the response Message.
		/// \param disabled Whether the button is active or not.
		/// \param customIdNew A custom id to give for identifying the button.
		/// \param buttonLabel A visible label for the button.
		/// \param buttonStyle The style of the button.
		/// \param emojiName An emoji name, if desired.
		/// \param emojiId An emoji id, if desired.
		/// \param url A url, if applicable.
		/// \returns MessageResponseBase& A reference to this data structure.
		MessageResponseBase& addButton(bool disabled, const std::string& customIdNew, const std::string& buttonLabel, ButtonStyle buttonStyle, const std::string& emojiName = "",
			Snowflake emojiId = 0, const std::string& url = "");

		/// Adds a select-menu to the response Message. \brief Adds a select-menu to the response Message.
		/// \param disabled Whether the select-menu is active or not.
		/// \param customIdNew A custom id to give for identifying the select-menu.
		/// \param options A vector of select-menu-options to offer.
		/// \param placeholder Custom placeholder text if nothing is selected, max 100 characters.
		/// \param maxValues Maximum number of selections that are possible.
		/// \param minValues Minimum required number of selections that are required.
		/// \returns MessageResponseBase& A reference to this data structure.
		MessageResponseBase& addSelectMenu(bool disabled, const std::string& customIdNew, std::vector<SelectOptionData> options, const std::string& placeholder, int32_t maxValues,
			int32_t minValues);

		/// Adds a modal to the response Message. \brief Adds a modal to the response Message.
		/// \param topTitleNew A title for the modal.
		/// \param topCustomIdNew A custom id to give for the modal.
		/// \param titleNew A title for the modal's individual input.
		/// \param customIdNew A custom id to give for the modal's individual input.
		/// \param required Is it a required response?
		/// \param minLength Minimum length.
		/// \param maxLength Maximum length.
		/// \param inputStyle The input style.
		/// \param label A label for the modal.
		/// \param placeholder A placeholder for the modal.
		/// \returns MessageResponseBase& A reference to this data structure.
		MessageResponseBase& addModal(const std::string& topTitleNew, const std::string& topCustomIdNew, const std::string& titleNew, const std::string& customIdNew, bool required,
			int32_t minLength, int32_t maxLength, TextInputStyle inputStyle, const std::string& label = "", const std::string& placeholder = "");

		/// Adds a file to the current collection of files for this message response. \brief Adds a file to the current collection of files for this message response.
		/// \param theFile The file to be added.
		/// \returns MessageResponseBase& A reference to this data structure.
		MessageResponseBase& addFile(File theFile);

		/// For setting the allowable mentions in a response. \brief For setting the allowable mentions in a response.
		/// \param dataPackage An AllowedMentionsData structure.
		/// \returns MessageResponseBase& A reference to this data structure.
		MessageResponseBase& addAllowedMentions(AllowedMentionsData dataPackage);

		/// For setting the components in a response. \brief For setting the components in a response.
		/// \param dataPackage An ActionRowData structure.
		/// \returns MessageResponseBase& A reference to this data structure.
		MessageResponseBase& addComponentRow(ActionRowData dataPackage);

		/// For setting the embeds in a response. \brief For setting the embeds in a response.
		/// \param dataPackage An EmbedData structure.
		/// \returns MessageResponseBase& A reference to this data structure.
		MessageResponseBase& addMessageEmbed(EmbedData dataPackage);

		/// For setting the Message content in a response. \brief For setting the Message content in a response.
		/// \param dataPackage A string, containing the content.
		/// \returns MessageResponseBase& A reference to this data structure.
		MessageResponseBase& addContent(const std::string& dataPackage);

		/// For setting the tts status of a response. \brief For setting the tts status of a response.
		/// \param enabledTTs A bool.
		/// \returns MessageResponseBase& A reference to this data structure.
		MessageResponseBase& setTTSStatus(bool enabledTTs);

	  protected:
		std::vector<ActionRowData> components{};
		AllowedMentionsData allowedMentions{};
		std::vector<EmbedData> embeds{};
		std::vector<File> files{};
		std::string customId{};
		std::string content{};
		std::string title{};
		int32_t flags{ 0 };
		bool tts{ false };
	};

	/// Interaction response data. \brief Interaction response data.
	struct DiscordCoreAPI_Dll InteractionResponseData {
		InteractionResponseData& operator=(const RespondToInputEventData& other);

		InteractionResponseData(const RespondToInputEventData& other);

		InteractionResponseData() = default;

		operator std::string();

		InteractionCallbackData data{};///< Interaction ApplicationCommand callback data.
		InteractionCallbackType type{};///< Interaction callback type.
	};

	/// Guild application command permissions data. \brief Guild application command permissions data.
	class DiscordCoreAPI_Dll GuildApplicationCommandPermissionData : public DiscordEntity {
	  public:
		std::vector<ApplicationCommandPermissionData> permissions{};
		Snowflake applicationId{};
		Snowflake guildId{};

		virtual ~GuildApplicationCommandPermissionData() = default;
	};

	/**@}*/

	/// Song types. \brief Song types.
	enum class SongType : int8_t {
		YouTube = 0,///< YouTube.
		SoundCloud = 1///< SoundCloud.
	};

	/// Represents a download Url. \brief Represents a download Url.
	struct DiscordCoreAPI_Dll DownloadUrl {
		int32_t contentSize{ 0 };
		std::string urlPath{};
	};

	/**
	 * \addtogroup voice_connection
	 * @{
	 */

	/// Represents a single frame of raw audio data. \brief Represents a single frame of raw audio data.
	struct DiscordCoreAPI_Dll RawFrameData {
		std::vector<uint8_t> data{};///< The audio data.
		int32_t sampleCount{ -1 };///< The number of samples per this frame.
	};

	/// Represents a single frame of encoded audio data. \brief Represents a single frame of encoded audio data.
	struct DiscordCoreAPI_Dll EncodedFrameData {
		std::vector<uint8_t> data{};///< The audio data.
		int32_t sampleCount{ -1 };///< The number of samples per this frame.
	};

	/// Audio frame types. \brief Audio frame types.
	enum class AudioFrameType : int8_t {
		Unset = 0,///< Unset.
		Encoded = 1,///< Encoded.
		RawPCM = 2,///< Raw PCM.
		Skip = 3///< Skip.
	};

	/// Represents a single frame of audio data. \brief Represents a single frame of audio data.
	struct DiscordCoreAPI_Dll AudioFrameData {
		AudioFrameType type{ AudioFrameType::Unset };///< The type of audio frame.
		EncodedFrameData encodedFrameData{};///< To be filled if it's already encoded.
		Snowflake guildMemberId{};///< The Id of the GuildMember from which it was sent.
		RawFrameData rawFrameData{};///< To be filled if it's raw audio data.

		void clearData();
	};

	/// A song from the various platforms. \brief A song from the various platforms.
	class DiscordCoreAPI_Dll Song : public DataParser<Song> {
	  public:
		friend class DiscordCoreInternal::SoundCloudRequestBuilder;
		friend class DiscordCoreInternal::YouTubeRequestBuilder;
		friend class DiscordCoreInternal::SoundCloudAPI;
		friend class DiscordCoreInternal::YouTubeAPI;
		friend class SongAPI;

		std::vector<DownloadUrl> finalDownloadUrls{};
		SongType type{ SongType::SoundCloud };///< The type of song.
		std::string secondDownloadUrl{};
		std::string firstDownloadUrl{};
		std::string html5PlayerFile{};
		std::string addedByUserName{};///< The User name of the individual who added this Song to the playlist.
		std::string thumbnailUrl{};///< The Url of the thumbnail image of this Song.
		int32_t contentLength{ 0 };
		std::string html5Player{};
		std::string description{};///< A description of the Song.
		Snowflake addedByUserId{};///< The User id of the individual who added this Song to the playlist.
		std::string songTitle{};///< The title of the Song.
		std::string duration{};///< The duration of the Song.
		std::string viewUrl{};///< The url for listening to this Song through a browser.
		std::string songId{};

		Song() = default;

		Song& operator=(const nlohmann::json& jsonObjectData);

		Song(const nlohmann::json& jsonObjectData);

		virtual ~Song() = default;

	  protected:
		std::string trackAuthorization{};
		bool doWeGetSaved{ false };
		YouTubeFormat format{};

		void parseObject(const nlohmann::json& jsonObjectData, Song* pDataStructure);
	};

	/// Song completion event data. \brief Song completion event data.
	struct DiscordCoreAPI_Dll SongCompletionEventData {
		GuildMemberData guildMember{};///< The sending GuildMember.
		bool wasItAFail{ false };///< Is this a replay? (Did a track recently fail to play?)
		Song previousSong{};///< The previously played Song.
		GuildData guild{};///< The sending Guild.
	};

	/// Playlist of songs and other variables. \brief Playlist of songs and other variables.
	struct DiscordCoreAPI_Dll Playlist {
		bool isLoopSongEnabled{ false };///< Is looping of Songs currently enabled?
		bool isLoopAllEnabled{ false };///< Is looping of the entire Playlist currently enabled?
		std::vector<Song> songQueue{};///< The list of Songs that are stored to be played.
		Song currentSong{};///< The current Song that is playing.
	};

	/**@}*/

	/**
	 * \addtogroup utilities
	 * @{
	 */

	/// Command data, for functions executed by the CommandController. \brief Command data, for functions executed by the CommandController.
	class DiscordCoreAPI_Dll CommandData : public DataParser<CommandData> {
	  public:
		std::vector<std::string> optionsArgs{};
		std::string subCommandGroupName{};
		std::string subCommandName{};
		InputEventData eventData{};
		std::string commandName{};

		CommandData() = default;

		CommandData(InputEventData inputEventData);

		CommandData& operator=(const nlohmann::json& jsonObjectData);

		CommandData(const nlohmann::json& jsonObjectData);

		virtual ~CommandData() = default;

	  protected:
		void parseObject(const nlohmann::json& jsonObjectData, CommandData* pDataStructure);
	};

	/// Base arguments for the command classes. \brief Base arguments for the command classes.
	struct DiscordCoreAPI_Dll BaseFunctionArguments : public CommandData {
		DiscordCoreClient* discordCoreClient{ nullptr };///< A pointer to the instance of DiscordCoreClient.

		BaseFunctionArguments() = default;

		BaseFunctionArguments(CommandData commandData, DiscordCoreClient* discordCoreClientNew);

		virtual ~BaseFunctionArguments() = default;
	};

	/// Base class for the command classes. \brief Base class for the command classes.
	struct DiscordCoreAPI_Dll BaseFunction {
		std::string helpDescription{};///< Description of the command for the Help command.
		std::string commandName{};///< Name of the command for calling purposes.
		EmbedData helpEmbed{};///< A Message embed for displaying the command via the Help command.

		BaseFunction() = default;

		/// The base function for the command's execute function. \brief The base function for the command's execute function.
		/// \param args A reference to an instance of BaseFunctionArguments.
		virtual void execute(BaseFunctionArguments& args) = 0;
		virtual std::unique_ptr<BaseFunction> create() = 0;
		virtual ~BaseFunction() = default;
	};

	struct DiscordCoreAPI_Dll MoveThroughMessagePagesData {
		InputEventData inputEventData{};
		uint32_t currentPageIndex{};
		std::string buttonId{};
	};

	DiscordCoreAPI_Dll MoveThroughMessagePagesData moveThroughMessagePages(const std::string& userID, InputEventData originalEvent, uint32_t currentPageIndex,
		const std::vector<EmbedData>& messageEmbeds, bool deleteAfter, uint32_t waitForMaxMs, bool returnResult = false);

	/**@}*/

};// namespace DiscordCoreAPI
