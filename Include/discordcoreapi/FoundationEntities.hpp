/*
	DiscordCoreAPI, A bot library for Discord, written in C++, and featuring explicit multithreading through the usage of custom, asynchronous C++ CoRoutines.

	Copyright 2021, 2022 Chris M. (RealTimeChris)

	This library is free software; you can redistribute it and/or
	modify it under the terms of the GNU Lesser General Public
	License as published by the Free Software Foundation; either
	version 2.1 of the License, or (at your option) any later version.

	This library is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
	Lesser General Public License for more details.

	You should have received a copy of the GNU Lesser General Public
	License along with this library; if not, write to the Free Software
	Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301
	USA
*/
/// FoundationEntities.hpp - Header for all of the Discord/Support API data
/// structures. May 12, 2021 Chris M.
/// https://discordcoreapi.com
/// \file FoundationEntities.hpp

#pragma once

#ifndef FOUNDATION_ENTITIES
	#define FOUNDATION_ENTITIES

	#include <discordcoreapi/Utilities.hpp>
	#include <discordcoreapi/DataParsingFunctions.hpp>

namespace DiscordCoreInternal {

	struct DiscordCoreAPI_Dll VoiceConnectInitData {
		DiscordCoreAPI::StreamType streamType{};
		DiscordCoreAPI::StreamInfo streamInfo{};
		Bool selfDeaf{ false };
		Bool selfMute{ false };
		Snowflake channelId{};
		Int32 currentShard{};
		Snowflake guildId{};
		Snowflake userId{};
	};

	struct DiscordCoreAPI_Dll VoiceConnectionData {
		String sessionId{};
		String endPoint{};
		String token{};
	};

	struct DiscordCoreAPI_Dll WebSocketResumeData {
		Int32 lastNumberReceived{};
		String sessionId{};
		String botToken{};

		operator DiscordCoreAPI::JsonObject();
	};

	struct DiscordCoreAPI_Dll WebSocketIdentifyData {
		UpdatePresenceData presence{};
		Int32 largeThreshold{ 250 };
		Int32 numberOfShards{};
		Int32 currentShard{};
		String botToken{};
		Int64 intents{};

		operator DiscordCoreAPI::JsonObject();
	};

	struct DiscordCoreAPI_Dll VoiceSocketProtocolPayloadData {
		String voiceEncryptionMode{};
		String externalIp{};
		Uint64 voicePort{};

		operator DiscordCoreAPI::JsonObject();
	};

	struct DiscordCoreAPI_Dll VoiceIdentifyData {
		VoiceConnectInitData connectInitData{};
		VoiceConnectionData connectionData{};

		operator DiscordCoreAPI::JsonObject();
	};

	enum class SendSpeakingType : Uint8 {
		Microphone = 1 << 0,
		Soundshare = 1 << 1,
		Priority = 1 << 2,
		Priority_And_Voice = Microphone | Priority,
	};

	struct DiscordCoreAPI_Dll SendSpeakingData {
		SendSpeakingType type{};
		Int32 delay{};
		Int32 ssrc{};

		operator DiscordCoreAPI::JsonObject();
	};

	enum class HttpsWorkloadClass : Uint8 { Get = 0, Put = 1, Post = 2, Patch = 3, Delete = 4 };

	enum class PayloadType : Uint8 { Application_Json = 1, Multipart_Form = 2 };

	enum class HttpsWorkloadType : Uint8 {
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

		static UMap<HttpsWorkloadType, std::unique_ptr<AtomicInt64>> workloadIdsExternal;
		static UMap<HttpsWorkloadType, std::unique_ptr<AtomicInt64>> workloadIdsInternal;

		mutable UMap<String, String> headersToInsert{};
		PayloadType payloadType{ PayloadType::Application_Json };
		mutable AtomicInt64 thisWorkerId{ 0 };
		HttpsWorkloadClass workloadClass{};
		mutable String baseUrl{};
		String relativePath{};
		String callStack{};
		String content{};

		HttpsWorkloadData& operator=(const HttpsWorkloadData& other) noexcept;

		HttpsWorkloadData(const HttpsWorkloadData& other) noexcept;

		HttpsWorkloadData(DiscordCoreInternal::HttpsWorkloadType theType) noexcept;

	  protected:
		static Int64 incrementAndGetWorkloadId(HttpsWorkloadType workloadType) noexcept;

		HttpsWorkloadType workloadType{};
	};

	struct DiscordCoreAPI_Dll HelloData {
		HelloData(simdjson::ondemand::value);
		Int32 heartbeatInterval{};
	};

	struct DiscordCoreAPI_Dll WebSocketMessage {
		WebSocketMessage() noexcept = default;
		WebSocketMessage(simdjson::ondemand::value);
		Uint64 op{ static_cast<Uint64>(-1) };
		template<typename ReturnType> ReturnType processJsonMessage() {
			return ReturnType{ this->d };
		}
		simdjson::ondemand::value d{};
		String t{};
		Uint64 s{};
	};

	struct DiscordCoreAPI_Dll InvalidSessionData {
		InvalidSessionData(simdjson::ondemand::value);
		Bool d{};
	};
}

/// The main namespace for this library. \brief The main namespace for this
/// library.
namespace DiscordCoreAPI {

	/**
	 * \addtogroup foundation_entities
	 * @{
	*/

	class DiscordCoreAPI_Dll NewBase {};

	struct DiscordCoreAPI_Dll ConnectionPackage {
		Int32 currentReconnectTries{ 0 };
		Bool areWeResuming{ false };
		Uint32 currentShard{ 0 };
	};

	/// Timeout durations for the timeout command. \brief Timeout durations for the timeout command.
	enum class TimeoutDurations : Int16 {
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
		/// snowflake-id into a time and date stamp. \returns String A
		/// String containing the timestamp.
		String getCreatedAtTimestamp(TimeFormat timeFormat);

		virtual ~DiscordEntity() noexcept = default;
	};

	/// Role tags data. \brief Role tags data.
	struct DiscordCoreAPI_Dll RoleTagsData {
		String premiumSubscriber{};///< Are they a premium subscriber?
		String integrationId{};///< What is the integration id?
		String botId{};///< What is the bot id?

		RoleTagsData() noexcept = default;

		RoleTagsData(simdjson::ondemand::value);

		virtual ~RoleTagsData() noexcept = default;
	};

	/// User flags. \brief User flags.
	enum class UserFlags : Int32 {
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
	enum class PremiumType : Uint8 {
		None = 0,///< None.
		Nitro_Classic = 1,///< Nitro classic.
		Nitro = 2///< Nitro.
	};

	/// Data structure representing a single User. \brief Data structure representing a single User.
	class DiscordCoreAPI_Dll UserData : public DiscordEntity {
	  public:
		friend class GuildData;

		StringWrapper discriminator{};///< The user's 4-digit discord-tag	identify.
		StringWrapper userName{};///< The user's userName, not unique across the platform identify.
		IconHash avatar{};///< The user's avatar hash.
		Int32 flags{};///< The public flags on a user' s account.

		UserData() noexcept = default;

		UserData& operator=(UserData&&) noexcept = default;

		UserData(UserData&&) noexcept = default;

		UserData& operator=(const UserData&) noexcept = default;

		UserData(const UserData&) noexcept = default;

		UserData(simdjson::ondemand::value jsonObjectData);

		String getAvatarUrl();

		virtual ~UserData() noexcept = default;
	};

	/// Attachment data. \brief Attachment data.
	class DiscordCoreAPI_Dll AttachmentData : public DiscordEntity {
	  public:
		String contentType{};///< Type of content for the attachment.
		String description{};///< A description of the attachment.
		Bool ephemeral{ false };///< Whether it was an ephemeral response.
		String filename{};///< The file name of the attachment.
		String proxyUrl{};///< The proxy url for the attachment.
		Int32 height{ 0 };///< The height of the attachment.
		Int32 width{ 0 };///< The width of the attachment.
		Int32 size{ 0 };///< The size of the attachment.
		String url{};///< The url for the attachment.

		operator JsonObject();

		AttachmentData() noexcept = default;

		AttachmentData(simdjson::ondemand::value);

		virtual ~AttachmentData() noexcept = default;
	};

	/// Sticker format types. \brief Sticker format types.
	enum class StickerFormatType : Uint8 {
		Png = 1,///< Png.
		Apng = 2,///< Apng.
		Lottie = 3///< Lottie
	};

	/// Embed footer data. \brief Embed footer data.
	struct DiscordCoreAPI_Dll EmbedFooterData {
		String proxyIconUrl{};///< Proxy icon url.
		String iconUrl{};///< Icon url.
		String text{};///< Footer text.

		EmbedFooterData() noexcept = default;

		EmbedFooterData(simdjson::ondemand::value jsonObjectData);

		virtual ~EmbedFooterData() noexcept = default;
	};

	/// Embed image data. \brief Embed image data.
	struct DiscordCoreAPI_Dll EmbedImageData {
		String proxyUrl{};///< Proxy url.
		Int32 height{ 0 };///< Image height.
		Int32 width{ 0 };///< Image width.
		String url{};///< Image url.

		EmbedImageData() noexcept = default;

		EmbedImageData(simdjson::ondemand::value jsonObjectData);

		virtual ~EmbedImageData() noexcept = default;
	};

	/// Embed thumbnail data. \brief Embed thumbnail data.
	struct DiscordCoreAPI_Dll EmbedThumbnailData {
		String proxyUrl{};///< Proxy url.
		Int32 height{ 0 };///< Image height.
		Int32 width{ 0 };///< Image width.
		String url{};///< Image url.

		EmbedThumbnailData() noexcept = default;

		EmbedThumbnailData(simdjson::ondemand::value jsonObjectData);

		virtual ~EmbedThumbnailData() noexcept = default;
	};

	/// Embed video data. \brief Embed video data.
	struct DiscordCoreAPI_Dll EmbedVideoData {
		String proxyUrl{};///< Proxy url.
		Int32 height{ 0 };///< Image height.
		Int32 width{ 0 };///< Image width.
		String url{};///< Image url.

		EmbedVideoData() noexcept = default;

		EmbedVideoData(simdjson::ondemand::value jsonObjectData);

		virtual ~EmbedVideoData() noexcept = default;
	};

	/// Embed provider data. \brief Embed provider data.
	struct DiscordCoreAPI_Dll EmbedProviderData {
		String name{};///< Name.
		String url{};///< Url.

		EmbedProviderData() noexcept = default;

		EmbedProviderData(simdjson::ondemand::value jsonObjectData);

		virtual ~EmbedProviderData() noexcept = default;
	};

	/// Embed author data. \brief Embed author data.
	struct DiscordCoreAPI_Dll EmbedAuthorData {
		String proxyIconUrl{};///< Proxy icon url.
		String iconUrl{};///< Icon url.
		String name{};///< Name.
		String url{};///< Url.

		EmbedAuthorData() noexcept = default;

		EmbedAuthorData(simdjson::ondemand::value jsonObjectData);

		virtual ~EmbedAuthorData() noexcept = default;
	};

	/// Embed field data. \brief Embed field data.
	struct DiscordCoreAPI_Dll EmbedFieldData {
		Bool Inline{ false };///< Is the field inline with the rest of them?
		String value{};///< The text on the field.
		String name{};///< The title of the field.

		EmbedFieldData() noexcept = default;

		EmbedFieldData(simdjson::ondemand::value jsonObjectData);

		operator JsonObject();

		virtual ~EmbedFieldData() noexcept = default;
	};

	/// Embed types. \brief Embed types.
	enum class EmbedType : Uint8 {
		Rich = 0,///< Rich.
		Image = 1,///< Image.
		Video = 2,///< Video.
		Gifv = 3,///< Gifv.
		Article = 4,///< Article.
		link = 5///< Link.
	};

	/// Embed data. \brief Embed data.
	class DiscordCoreAPI_Dll EmbedData {
	  public:
		Vector<EmbedFieldData> fields{};///< Array of embed fields.
		EmbedThumbnailData thumbnail{};///< Embed thumbnail data.
		ColorValue hexColorValue{ 0 };///< Hex color value of the embed.
		EmbedProviderData provider{};///< Embed provider data.
		String description{};///< Description of the embed.
		EmbedFooterData footer{};///< Embed footer data.
		EmbedAuthorData author{};///< Embed author data.
		String timestamp{};///< Timestamp to be placed on the embed.
		EmbedImageData image{};///< Embed image data.
		EmbedVideoData video{};///< Embed video data.
		String title{};///< Title of the embed.
		String type{};///< Type of the embed.
		String url{};///< Url for the embed.

		EmbedData() noexcept = default;

		operator JsonObject();

		/// Sets the author's name and avatar for the embed. \brief Sets the author's name and avatar for the embed.
		/// \param authorName The author's name.
		/// \param authorAvatarUrl The url to their avatar.
		/// \returns EmbedData& A reference to this embed.
		EmbedData& setAuthor(const String& authorName, const String& authorAvatarUrl = "");

		/// Sets the footer's values for the embed. \brief Sets the footer's values for the embed.
		/// \param footerText The footer's text.
		/// \param footerIconUrlText Url to the footer's icon.
		/// \returns EmbedData& A reference to this embed.
		EmbedData& setFooter(const String& footerText, const String& footerIconUrlText = "");

		/// Sets the timestamp on the embed. \brief Sets the timestamp on the embed.
		/// \param timeStamp The timestamp to be set.
		/// \returns EmbedData& A reference to this embed.
		EmbedData& setTimeStamp(const String& timeStamp);

		/// Adds a field to the embed. \brief Adds a field to the embed.
		/// \param name The title of the embed field.
		/// \param value The contents of the embed field.
		/// \param Inline Is it inline with the rest of the fields on the embed?
		/// \returns EmbedData& A reference to this embed.
		EmbedData& addField(const String& name, const String& value, Bool Inline = true);

		/// Sets the description (the main contents) of the embed. \brief Sets the description (the main contents) of the embed.
		/// \param descriptionNew The contents of the description to set.
		/// \returns EmbedData& A reference to this embed.
		EmbedData& setDescription(const String& descriptionNew);

		/// Sets the color of the embed, by applying a hex-color value. \brief Sets the color of the embed, by applying a hex-color value.
		/// \param hexColorValueNew A string containing a hex-number value (Between 0x00 0xFFFFFF).
		/// \returns EmbedData& A reference to this embed.
		EmbedData& setColor(const String& hexColorValueNew);

		/// Sets the thumbnail of the embed. \brief Sets the thumbnail of the embed.
		/// \param thumbnailUrl The url to the thumbnail to be used.
		/// \returns EmbedData& A reference to this embed.
		EmbedData& setThumbnail(const String& thumbnailUrl);

		/// Sets the title of the embed. \brief Sets the title of the embed.
		/// \param titleNew A string containing the desired title.
		/// \returns EmbedData& A reference to this embed.
		EmbedData& setTitle(const String& titleNew);

		/// Sets the image of the embed. \brief Sets the image of the embed.
		/// \param imageUrl The url of the image to be set on the embed.
		/// \returns EmbedData& A reference to this embed.
		EmbedData& setImage(const String& imageUrl);

		EmbedData(simdjson::ondemand::value jsonObjectData);

		virtual ~EmbedData() noexcept = default;
	};

	/// Message reference data.\brief Message reference data.
	struct DiscordCoreAPI_Dll MessageReferenceData {
		Bool failIfNotExists{ false };///< Fail if the Message doesn't exist?
		Snowflake messageId{};///< Id of the Message to reference.
		Snowflake channelId{};///< Id of the Channel that the referenced Message was sent in.
		Snowflake guildId{};///< Id of the Guild that the referenced Message was sent in.

		MessageReferenceData() noexcept = default;

		MessageReferenceData(simdjson::ondemand::value jsonObjectData);

		operator JsonObject();

		virtual ~MessageReferenceData() noexcept = default;
	};

	enum class MediaType : Uint8 { png = 0, gif = 1, jpeg = 2, mpeg = 3, mp3 = 4 };

	/// Data representing a file to be sent via multipart-form data. \brief Data representing a file to be sent via multipart-form data.
	struct DiscordCoreAPI_Dll File {
		String fileName{};///< The name of the file.
		String data{};///< The data of the file.
	};

	/// Channel types. \brief Channel types.
	enum class ChannelType : Uint8 {
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
	struct DiscordCoreAPI_Dll ThreadMetadataData {
		TimeStamp<std::chrono::milliseconds> archiveTimestamp{};///< (Where applicable) the time at which this Thread was archived.
		Int32 autoArchiveDuration{ 0 };///< How Int64 before archiving this Thread.
		Bool invitable{ false };///< The id of the individual who archived this Thread.
		Bool archived{ false };///< Whether or not this Thread is currently archived.
		Bool locked{ false };///< Whether or not this Thread is currently locked.

		ThreadMetadataData() noexcept = default;

		ThreadMetadataData(simdjson::ondemand::value);

		virtual ~ThreadMetadataData() noexcept = default;
	};

	/// Data for a single member of a Thread. \brief Data for a single member of a Thread.
	class DiscordCoreAPI_Dll ThreadMemberData : public DiscordEntity {
	  public:
		TimeStamp<std::chrono::milliseconds> joinTimestamp{};///< The time at which the member joined this Thread.
		Int32 flags{ 0 };///< Flags.
		Snowflake userId{};///< The User's id.

		ThreadMemberData() noexcept = default;

		ThreadMemberData(simdjson::ondemand::value);

		virtual ~ThreadMemberData() noexcept = default;
	};

	class DiscordCoreAPI_Dll ThreadMemberDataVector {
	  public:
		ThreadMemberDataVector() noexcept = default;

		operator Vector<ThreadMemberData>();

		ThreadMemberDataVector(simdjson::ondemand::value);

		virtual ~ThreadMemberDataVector() noexcept = default;

	  protected:
		Vector<ThreadMemberData> theThreadMemberDatas{};
	};

	/// Thread types. \brief Thread types.
	enum class ThreadType : Uint8 {
		Guild_News_Thread = 10,///< Guild news Thread.
		Guild_Public_Thread = 11,///< Guild public Thread.
		Guild_Private_Thread = 12///< Guild private Thread.
	};

	/// Voice state data. \brief Voice state data.
	struct DiscordCoreAPI_Dll VoiceStateData {
		TimeStamp<std::chrono::milliseconds> requestToSpeakTimestamp{ "" };///< The time at which the User requested to speak.
		String sessionId{};///< The session id for this voice state.
		Bool selfStream{ false };///< Whether this User is streaming using "Go Live".
		Bool selfVideo{ false };///< Whether this User's camera is enabled.
		Bool selfDeaf{ false };///< Whether this User is locally deafened.
		Bool selfMute{ false };///< Whether this User is locally muted.
		Bool suppress{ false };///< Whether this User is muted by the current User.
		Snowflake channelId{};///< The Channel id this User is connected to.
		Bool deaf{ false };///< Whether this User is deafened by the server.
		Bool mute{ false };///< Whether this User is muted by the server.
		Snowflake guildId{};///< The Guild id this voice state is for.
		Snowflake userId{};///< The User id this voice state is for.

		VoiceStateData() noexcept = default;

		VoiceStateData(simdjson::ondemand::value jsonObjectData);

		virtual ~VoiceStateData() noexcept = default;
	};

	/// Automatic Thread archiving durations. \brief Automatic Thread archiving durations.
	enum class ThreadAutoArchiveDuration : Int16 {
		Shortest = 60,///< Shortest.
		Short = 1440,///< Short.
		Long = 4320,///< Long.
		Longest = 10080///< Longest.
	};

	/// Party data. \brief Party data.
	class DiscordCoreAPI_Dll PartyData : public DiscordEntity {
	  public:
		Vector<Int32> size{ 0, 0 };///< The size of the party.

		PartyData() noexcept = default;

		PartyData(simdjson::ondemand::value jsonObjectData);

		virtual ~PartyData() noexcept = default;
	};

	enum class RoleFlags : Uint8 { Mentionable = 1 << 0, Managed = 1 << 1, Hoist = 1 << 2 };

	/// Data structure representing a single Role. \brief Data structure representing a single Role.
	class DiscordCoreAPI_Dll RoleData : public DiscordEntity {
	  public:
		friend class GuildData;

		StringWrapper unicodeEmoji{};///< Emoji representing the Role.
		Permissions permissions{};///< The Role's base Guild Permissions.
		Int16 position{ 0 };///< Its position amongst the rest of the Guild's roles.
		ColorValue color{ 0 };///< The Role's color.
		StringWrapper name{};///< The Role's name.
		Snowflake guildId{};///< The id of the Guild that this Role is from.
		Int8 flags{ 0 };///< Role flags.

		RoleData() noexcept = default;

		RoleData& operator=(RoleData&&) noexcept = default;

		RoleData(RoleData&&) noexcept = default;

		RoleData& operator=(const RoleData&) noexcept = default;

		RoleData(const RoleData&) noexcept = default;

		RoleData(simdjson::ondemand::value jsonObjectData);

		virtual ~RoleData() noexcept = default;
	};

	/// Data structure representing a single emoji. \brief Data structure representing a single emoji.
	class DiscordCoreAPI_Dll EmojiData : public DiscordEntity {
	  public:
		std::wstring unicodeName{ L"" };///< What is its unicode name?
		Vector<RoleData> roles{};///< Roles that are allowed to use this emoji.
		Bool requireColons{ false };///< Require colons to render it?
		Bool available{ true };///< Is it available to be used?
		Bool animated{ false };///< Is it animated?
		Bool managed{ false };///< Is it managed?
		StringWrapper name{};///< What is its name?
		UserData user{};///< User that created this emoji.

		EmojiData() noexcept = default;

		EmojiData(simdjson::ondemand::value jsonObjectData);

		virtual ~EmojiData() noexcept = default;
	};

	/// Assets data. \brief Party data.
	struct DiscordCoreAPI_Dll AssetsData {
		StringWrapper largeImage{};///< Keyname of an asset to display.
		StringWrapper smallImage{};///< Keyname of an asset to display.
		StringWrapper largeText{};///< Hover text for the large image.
		StringWrapper smallText{};///< Hover text for the small image.

		AssetsData() noexcept = default;

		virtual ~AssetsData() noexcept = default;
	};

	/// Secrets data. \brief Secrets data.
	struct DiscordCoreAPI_Dll SecretsData {
		StringWrapper spectate{};///< Unique hash for the given match context.
		StringWrapper match{};///< Unique hash for Spectate button.
		StringWrapper join{};///< Unique hash for chat invitesand Ask to Join.

		SecretsData() noexcept = default;

		virtual ~SecretsData() noexcept = default;
	};

	/// Timestamp data. \brief Timestamp data.
	struct DiscordCoreAPI_Dll TimestampData {
		Int64 start{ 0 };///< Unix timestamp - Send this to have an "elapsed" timer.
		Int64 end{ 0 };///< Unix timestamp - send this to have a "remaining" timer.

		TimestampData() noexcept = default;

		virtual ~TimestampData() noexcept = default;
	};

	/// Button data. \brief Button data.
	struct DiscordCoreAPI_Dll ButtonData {
		StringWrapper label{};///< Visible label of the button.
		StringWrapper url{};///< Url to display on the button.
	};

	/// Activity types. \brief Activity types.
	enum class ActivityType : Uint8 {
		Game = 0,///< Game.
		Streaming = 1,///< Streaming.
		Listening = 2,///< Listening.
		Watching = 3,///< Watching.
		Custom = 4,///< Custom.
		Competing = 5///< Competing.
	};

	/// Activity data. \brief Activity data.
	struct DiscordCoreAPI_Dll ActivityData {
		TimestampData timestamps{};///< Timestamp data.
		Snowflake applicationId{};///< Application id for the current application.
		StringWrapper details{};///< Details about the activity.
		Int32 createdAt{ 0 };///< Timestamp of when the activity began.
		Bool instance{ false };///< Whether this activity is an instanced context, like a match.
		StringWrapper state{};///< The player's current party status.
		SecretsData secrets{};///< Secrets data.
		ButtonData buttons{};///< Button Data.
		StringWrapper name{};///< Name of the activity.
		ActivityType type{};///< Activity data.
		AssetsData assets{};///< Assets data.
		StringWrapper url{};///< Url associated with the activity.
		Int32 flags{ 0 };///< Flags.
		EmojiData emoji{};///< Emoji associated with the activity.
		PartyData party{};///< Party data.

		ActivityData() noexcept = default;

		virtual ~ActivityData() noexcept = default;
	};

	enum class PresenceUpdateFlags : Uint8 {
		Desktop_Online = 0b00000001,
		Desktop_Idle = 0b00000010,
		Desktop_Dnd = 0b000000011,
		Mobile_Online = 0b00000010,
		Mobile_Idle = 0b00000100,
		Mobile_Dnd = 0b00000110,
		Web_Online = 0b00000100,
		Web_Idle = 0b00001000,
		Web_Dnd = 0b00001100,
		Status_Online = 0b00001000,
		Status_Idle = 0b00010000,
		Status_Dnd = 0b00011000
	};

	Void parseObject(simdjson::ondemand::value jsonObjectData, PresenceUpdateFlags& theData);

	/// Presence update data. \brief Presence update data.
	struct DiscordCoreAPI_Dll PresenceUpdateData {
		Uint8 theStatus{};///< Current client status.
		Snowflake guildId{};///< Guild id for the current presence.
		Snowflake userId{};///< User id for the current presence.

		PresenceUpdateData() noexcept = default;

		PresenceUpdateData(simdjson::ondemand::value jsonObjectData);

		virtual ~PresenceUpdateData() noexcept = default;
	};

	enum class GuildMemberFlags : Uint8 { Pending = 1 << 0, Deaf = 1 << 1, Mute = 1 << 2 };

	/// Data structure representing a single GuildMember. \brief Data structure representing a single GuildMember.
	/// Data structure representing a single Guild. \brief Data structure representing a single Guild.
	class DiscordCoreAPI_Dll GuildMemberData : public DiscordEntity {
	  public:
		friend class GuildData;
		TimeStamp<std::chrono::milliseconds> joinedAt{};///< When they joined the Guild.
		Vector<Snowflake> roles{};///< The Guild roles that they have.
		Snowflake voiceChannelId{};///< Currently held voice channel, if applicable.
		Permissions permissions{};///< Their base-level Permissions in the Guild
		Snowflake guildId{};///< The current Guild's id.
		StringWrapper nick{};///< Their nick/display name.
		IconHash avatar{};///< This GuildMember's Guild Avatar.
		Int8 flags{ 0 };///< GuildMember flags.

		GuildMemberData() noexcept = default;

		GuildMemberData& operator=(GuildMemberData&&) noexcept;

		GuildMemberData(GuildMemberData&&) noexcept;

		GuildMemberData& operator=(const GuildMemberData&) noexcept = default;

		GuildMemberData(const GuildMemberData&) noexcept = default;

		GuildMemberData(simdjson::ondemand::value);

		String getAvatarUrl();

		UserData getUserData();

		virtual ~GuildMemberData() noexcept = default;
	};

	/// PermissionTypes overwrites types. \brief PermissionTypes overwrites types.
	enum class PermissionOverwritesType : Uint8 {
		Role = 0,///< Role.
		User = 1///< User.
	};

	/// A PermissionTypes overwrite, for a given Channel. \brief A PermissionTypes overwrite, for a given Channel.
	class DiscordCoreAPI_Dll OverWriteData : public DiscordEntity {
	  public:
		PermissionOverwritesType type{};///< Role or User type.
		Uint64 allow{};///< Collection of Permissions to allow.
		Uint64 deny{};///< Collection of Permissions to deny.

		OverWriteData() noexcept = default;

		OverWriteData(simdjson::ondemand::value jsonObjectData);

		virtual ~OverWriteData() noexcept = default;
	};

	enum class ChannelFlags : Uint8 { NSFW = 1 << 0 };

	/// Data structure representing a single Channel. \brief Data structure representing a single Channel.
	class DiscordCoreAPI_Dll ChannelData : public DiscordEntity {
	  public:
		friend class GuildData;

		Vector<OverWriteData> permissionOverwrites{};
		ChannelType type{ ChannelType::Dm };///< The type of the Channel.
		Int32 memberCount{ 0 };///< Count of members active in the Channel.
		Uint16 position{ 0 };///< The position of the Channel, in the Guild's Channel list.
		Snowflake parentId{};///< Id of the Channel's parent Channel/category.
		StringWrapper topic{};///< Channel topic.
		Snowflake ownerId{};///< Id of the Channel's owner.
		Snowflake guildId{};///< Id of the Channel's Guild, if applicable.
		StringWrapper name{};///< Name of the Channel.
		Uint8 flags{};///< Flags combined as a bitmask.

		ChannelData() noexcept = default;

		ChannelData& operator=(ChannelData&&) noexcept = default;

		ChannelData(ChannelData&&) noexcept = default;

		ChannelData& operator=(const ChannelData&) noexcept = default;

		ChannelData(const ChannelData&) noexcept = default;

		ChannelData(simdjson::ondemand::value);

		virtual ~ChannelData() noexcept = default;
	};

	/// Data representing an active Thread. \brief Data representing an active Thread.
	struct DiscordCoreAPI_Dll ActiveThreadsData {
		Vector<ThreadMemberData> members{};
		Vector<ChannelData> threads{};
		Bool hasMore{ false };

		ActiveThreadsData() noexcept = default;

		ActiveThreadsData(simdjson::ondemand::value jsonObjectData);

		virtual ~ActiveThreadsData() noexcept = default;
	};

	/// Data representing an archived Thread. \brief Data representing an archived Thread.
	struct DiscordCoreAPI_Dll ArchivedThreadsData {
		Vector<ThreadMemberData> members{};
		Vector<ChannelData> threads{};
		Bool hasMore{ false };

		ArchivedThreadsData() noexcept = default;

		ArchivedThreadsData(simdjson::ondemand::value jsonObjectData);

		virtual ~ArchivedThreadsData() noexcept = default;
	};

	/// Application command-option types. \brief Application command-option types.
	enum class ApplicationCommandOptionType : Uint8 {
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
	enum class ApplicationCommandPermissionType : Uint8 {
		Role = 1,///< Role.
		User = 2,///< User.
		Channel = 3///< Channel.
	};

	/// Event types for auto-moderation. \brief Event types for auto-moderation.
	enum class EventType : Uint8 {
		Message_Send = 1,///< When a member sends or edits a message in the guild.
	};

	/// Trigger types for auto-moderation. \brief Trigger types for auto-moderation.
	enum class TriggerType : Uint8 {
		Keyword = 1,///< Check if content contains words from a user defined list of keywords.
		Harmful_Link = 2,///< Check if content contains any harmful links.
		Spam = 3,///< Check if content represents generic spam.
		Keyword_Preset = 4///< Check if content contains words from internal pre-defined wordsets.
	};

	/// Keyword preset types for auto-moderation. \brief Keyword preset types for auto-moderation.
	enum class KeywordPresetType : Uint8 {
		Profanity = 1,///< Words that may be considered forms of swearing or cursing.
		Sexual_Content = 2,///< Words that refer to sexually explicit behavior or activity
		Slurs = 3///< Personal insults or words that may be considered hate speech.
	};

	/// Action types for auto-moderation. \brief Action types for auto-moderation.
	enum class ActionType : Uint8 {
		Block_Message = 1,///< Blocks the content of a message according to the rule.
		Send_Alert_Message = 2,///< Logs user content to a specified channel.
		Timeout = 3///< Timeout user for a specified duration.
	};

	/// Action metadata for auto-moderation-rules. \brief Action metadata for auto-moderation-rules.
	struct DiscordCoreAPI_Dll ActionMetaData {
		Snowflake channelId{};///< Channel to which user content should be logged.
		Int64 durationSeconds{};///< Timeout duration in seconds.

		ActionMetaData() noexcept = default;

		ActionMetaData(simdjson::ondemand::value jsonObjectData);

		virtual ~ActionMetaData() noexcept = default;
	};

	/// Trigger metadata for auto-moderation-rules. \brief Trigger metadata for auto-moderation-rules.
	struct DiscordCoreAPI_Dll TriggerMetaData {
		Vector<String> keywordFilter{};///< Substrings which will be searched for in content.
		Vector<KeywordPresetType> presets{};///< The internally pre-defined wordsets which will be searched for in content.

		TriggerMetaData() noexcept = default;

		TriggerMetaData(simdjson::ondemand::value jsonObjectData);

		virtual ~TriggerMetaData() noexcept = default;
	};

	/// For representing a single auto-moderation-rule-action. \brief For representing a single auto-moderation-rule-action.
	struct DiscordCoreAPI_Dll ActionData {
		ActionType type{};///< The type of action.
		ActionMetaData metadata{};///< Additional metadata needed during execution for this specific action type.

		ActionData() noexcept = default;

		ActionData(simdjson::ondemand::value jsonObjectData);

		virtual ~ActionData() noexcept = default;
	};

	/// Represents an auto-moderation-rule. \brief Represents an auto-moderation-rule.
	class DiscordCoreAPI_Dll AutoModerationRuleData : public DiscordEntity {
	  public:
		Vector<Snowflake> exemptChannels{};///< The channel ids that should not be affected by the rule(Maximum of 50).
		Vector<Snowflake> exemptRoles{};///< The role ids that should not be affected by the rule(Maximum of 20).
		Vector<ActionData> actions{};///< Actions which will execute when the rule is triggered.
		TriggerMetaData triggerMetaData{};///< The rule trigger metadata actions array of action objects the.
		TriggerType triggerType{};///< The rule trigger type.
		EventType eventType{};///< The rule event type.
		Snowflake creatorId{};///< The user which first created this rule.
		Snowflake guildId{};///< The guild which this rule belongs to.
		String name{};///< The rule name.
		Bool enabled{};///< Whether the rule is enabled.

		AutoModerationRuleData() noexcept = default;

		AutoModerationRuleData(simdjson::ondemand::value jsonObjectData);

		virtual ~AutoModerationRuleData() noexcept = default;
	};

	/// Permissions data for an ApplicationCommand. \brief Permissions data for an ApplicationCommand.
	class DiscordCoreAPI_Dll ApplicationCommandPermissionData : public DiscordEntity {
	  public:
		ApplicationCommandPermissionType type{ ApplicationCommandPermissionType::Role };///< The type of PermissionTypes.
		Bool permission{ false };///< Whether the PermissionTypes is active or not.

		ApplicationCommandPermissionData() noexcept = default;

		ApplicationCommandPermissionData(simdjson::ondemand::value jsonObjectData);

		virtual ~ApplicationCommandPermissionData() noexcept = default;
	};

	/// Represents the Permissions for accessing an ApplicationCommand from within a Guild. \brief Represents the Permissions for accessing an ApplicationCommand from within a Guild.
	class DiscordCoreAPI_Dll GuildApplicationCommandPermissionsData : public DiscordEntity {
	  public:
		Vector<ApplicationCommandPermissionData> permissions{};///< The Permissions.
		Snowflake applicationId{};///< The application's id.
		Snowflake guildId{};///< The Guild's id.

		GuildApplicationCommandPermissionsData() noexcept = default;

		GuildApplicationCommandPermissionsData(simdjson::ondemand::value jsonObjectData);

		virtual ~GuildApplicationCommandPermissionsData() noexcept = default;
	};

	class DiscordCoreAPI_Dll GuildApplicationCommandPermissionsDataVector {
	  public:
		GuildApplicationCommandPermissionsDataVector() noexcept = default;

		operator Vector<GuildApplicationCommandPermissionsData>();

		GuildApplicationCommandPermissionsDataVector(simdjson::ondemand::value jsonObjectData);

		virtual ~GuildApplicationCommandPermissionsDataVector() noexcept = default;

	  protected:
		Vector<GuildApplicationCommandPermissionsData> theGuildApplicationCommandPermissionsDatas{};
	};

	class DiscordCoreAPI_Dll EmojiDataVector {
	  public:
		EmojiDataVector() noexcept = default;

		EmojiDataVector(simdjson::ondemand::value jsonObjectData);

		virtual ~EmojiDataVector() noexcept = default;

	  protected:
		Vector<EmojiData> theEmojiDatas{};
	};

	/// For updating/modifying a given Channel's properties. \brief For updating/modifying a given Channel's properties.
	struct DiscordCoreAPI_Dll UpdateChannelData {
		Vector<OverWriteData> permissionOverwrites{};
		Int32 defaultAutoArchiveDuration{ 10080 };
		Int32 videoQualityMode{ 1 };
		Int32 rateLimitPerUser{ 0 };
		Int32 bitrate{ 48000 };
		String parentId{};
		String rtcRgion{};
		Int32 userLimit{ 0 };
		Int32 position{ 0 };
		String topic{};
		String name{};
		ChannelType type{};
		Bool nsfw{ false };
	};

	/// Data structure representing a single reaction. \brief/// Data structure representing a single reaction.
	class DiscordCoreAPI_Dll ReactionData : public DiscordEntity {
	  public:
		GuildMemberData member{};///< The GuildMember who placed the reaction.
		Snowflake channelId{};///< The id of the Channel where it was placed.
		Snowflake messageId{};///< The id of the Message upon which it was placed.
		Snowflake guildId{};///< The id of the Guild where it was placed.
		Int32 count{ 0 };///< The number of times this particular emoji was placed as a reaction to the given Message.
		EmojiData emoji{};///< The emoji that was placed as a reaction.
		Snowflake userId{};///< The id of the User who placed the reaction.
		Bool me{ false };///< Whether or not I (The bot) placed it.

		ReactionData() noexcept = default;

		ReactionData(simdjson::ondemand::value jsonObjectData);

		virtual ~ReactionData() noexcept = default;
	};

	/// Structure representing Voice Region Data. \brief Structure representing Voice Region Data.
	struct DiscordCoreAPI_Dll VoiceRegionData {
		Bool deprecated{ false };///< Whether this is a deprecated voice region(avoid switching to these).
		Bool optimal{ false };///< True for a single server that is closest to the current User's client.
		Bool custom{ false };///< Whether this is a custom voice region(used for events / etc).
		String name{};///< Name of the region.
		Snowflake id{};///< Unique ID for the region.

		VoiceRegionData() noexcept = default;

		VoiceRegionData(simdjson::ondemand::value jsonObjectData);

		virtual ~VoiceRegionData() noexcept = default;
	};

	class DiscordCoreAPI_Dll VoiceRegionDataVector {
	  public:
		VoiceRegionDataVector() noexcept = default;

		operator Vector<VoiceRegionData>();

		VoiceRegionDataVector(simdjson::ondemand::value jsonObjectData);

		virtual ~VoiceRegionDataVector() noexcept = default;

	  protected:
		Vector<VoiceRegionData> theVoiceRegionDatas{};
	};

	/// Message activity types. \brief Message activity types.
	enum class MessageActivityType : Uint8 {
		Join = 1,///< Join.
		Spectate = 2,///< Spectate.
		Listen = 3,///< Listen.
		Join_Request = 5///< Join-request.
	};

	/// Message activity data. \brief Message activity data.
	struct DiscordCoreAPI_Dll MessageActivityData {
		MessageActivityType type{ MessageActivityType::Join };///< Message activity type.
		String partyId{};///< Party id.

		MessageActivityData() noexcept = default;

		MessageActivityData(simdjson::ondemand::value jsonObjectData);

		virtual ~MessageActivityData() noexcept = default;
	};

	/// Ban data. \brief Ban data.
	struct DiscordCoreAPI_Dll BanData {
		Bool failedDueToPerms{ false };///< Failed due to perms?
		String reason{};///< Reason for the ban.
		UserData user{};///< User that was banned.

		BanData() noexcept = default;

		BanData(simdjson::ondemand::value jsonObjectData);

		virtual ~BanData() noexcept = default;
	};

	class DiscordCoreAPI_Dll BanDataVector {
	  public:
		BanDataVector() noexcept = default;

		operator Vector<BanData>();

		BanDataVector(simdjson::ondemand::value jsonObjectData);

		virtual ~BanDataVector() noexcept = default;

	  protected:
		Vector<BanData> theBanDatas{};
	};

	/// Team members object data. \brief Team members object data.
	struct DiscordCoreAPI_Dll TeamMembersObjectData {
		Vector<Permissions> permissions{};///< Permissions for the team.
		Int32 membershipState{ 0 };///< Current state.
		String teamId{};///< Id of the current team.
		UserData user{};///< User data of the current User.

		TeamMembersObjectData() noexcept = default;

		TeamMembersObjectData(simdjson::ondemand::value jsonObjectData);

		virtual ~TeamMembersObjectData() noexcept = default;
	};

	/// For updating the current voice state. \brief For updating the current voice state.
	struct DiscordCoreAPI_Dll UpdateVoiceStateData {
		Bool selfMute{ false };///< Whether or not we self-mute ourselves.
		Bool selfDeaf{ false };///< Whether or not we self-deafen ourselves.
		Snowflake channelId{};///< Id of the desired voice Channel. Leave blank to disconnect.
		Snowflake guildId{};///< The id of the Guild fo which we would like to establish a voice connection.

		/// For converting this data structure into a string. \brief For converting this data structure into a string.
		operator JsonObject();
	};

	/// Team object data. \brief Team object data.
	class DiscordCoreAPI_Dll TeamObjectData : public DiscordEntity {
	  public:
		Vector<TeamMembersObjectData> members{};///< Array of team members object data.
		Snowflake ownerUserId{};///< User id of the team owner.
		String icon{};///< Icon for the team.

		TeamObjectData() noexcept = default;

		TeamObjectData(simdjson::ondemand::value jsonObjectData);

		virtual ~TeamObjectData() noexcept = default;
	};

	/// Application flags, for the ApplicationData structure.
	enum class ApplicationFlags : Int32 {
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
	struct DiscordCoreAPI_Dll InstallParamsData {
		Vector<String> scopes{};///< The scopes to add the application to the server with.
		Permissions permissions{};///< The permissions to request for the bot role.

		InstallParamsData() noexcept = default;

		InstallParamsData(simdjson::ondemand::value jsonObjectData);

		virtual ~InstallParamsData() noexcept = default;
	};

	/// Application data. \brief Application data.
	class DiscordCoreAPI_Dll ApplicationData : public DiscordEntity {
	  public:
		Vector<String> rpcOrigins{};///< Array of RPC origin strings.
		Bool botRequireCodeGrant{ false };///< Does the bot require a code grant?
		Vector<String> tags{};///< Up to 5 tags describing the content and functionality of the application install_params.
		String termsOfServiceUrl{};///< Terms of service Url.
		String privacyPolicyUrl{};///< Privacy policy Url.
		ApplicationFlags flags{ 0 };///< Application flags.
		InstallParamsData params{};///< Settings for the application's default in-app authorization link, if enabled String customInstallUrl{};
		String primarySkuId{};///< Primary SKU Id.
		String description{};///< Description of the application.
		String coverImage{};///< The cover image.
		Bool botPublic{ false };///< Is the bot public?
		String verifyKey{};///< The verification key.
		String summary{};///< Summary of the application.
		TeamObjectData team{};///< Team object data.
		Snowflake guildId{};///< Guild id.
		String slug{};///< Sluhg.
		String name{};///< Application's name.
		String icon{};///< Application's icon.
		UserData owner{};///< Application's owner.

		ApplicationData() noexcept = default;

		ApplicationData(simdjson::ondemand::value jsonObjectData);

		virtual ~ApplicationData() noexcept = default;
	};

	/// Authorization info structure. \brief Authorization info structure.
	struct DiscordCoreAPI_Dll AuthorizationInfoData {
		Vector<String> scopes{};///< Array of strings - the scopes the User has authorized the application for.
		ApplicationData application{};///< Partial application object the current application.
		String expires{};///< When the access token expires.
		UserData user{};/// The User who has authorized, if the User has authorized with the identify scope.

		AuthorizationInfoData() noexcept = default;

		AuthorizationInfoData(simdjson::ondemand::value jsonObjectData);

		virtual ~AuthorizationInfoData() noexcept = default;
	};

	/// Account data. \brief Account data.
	class DiscordCoreAPI_Dll AccountData : public DiscordEntity {
	  public:
		String name{};///< Name of the account.

		AccountData() noexcept = default;

		AccountData(simdjson::ondemand::value jsonObjectData);

		virtual ~AccountData() noexcept = default;
	};

	/// Guild Widget Data. \brief Guild Widget Data.
	struct DiscordCoreAPI_Dll GuildWidgetData {
		Bool enabled{ false };///< Whether the widget is enabled.
		Snowflake channelId{};///< The widget Channel id.

		GuildWidgetData() noexcept = default;

		GuildWidgetData(simdjson::ondemand::value jsonObjectData);

		virtual ~GuildWidgetData() noexcept = default;
	};

	/// Get Guild Widget Data. \brief Get Guild Widget Data.
	struct DiscordCoreAPI_Dll GetGuildWidgetObjectData : public DiscordEntity {
		Vector<ChannelData> channels{};///< Voice and stage channels which are accessible by everyone.
		Vector<UserData> members{};///< Special widget user objects that includes users presence (Limit 100).
		String instantInvite{};///< Instant invite for the guilds specified widget invite channel.
		Int32 presence_count{ 0 };///< Number of online members in this guild.
		String name{};///< Guild name (2-100 characters).
	};

	/// Widget style options. \brief Widget style options.
	enum class WidgetStyleOptions : Uint8 {
		Shield = 0,///< Shield
		Banner1 = 1,///< Banner1
		Banner2 = 2,///< Banner2
		Banner3 = 3,///< Banner3
		Banner4 = 4///< Banner4
	};

	/// Guild widget image data. \brief Guild widget image data.
	struct DiscordCoreAPI_Dll GuildWidgetImageData {
		String url{};

		GuildWidgetImageData() noexcept = default;

		GuildWidgetImageData(simdjson::ondemand::value jsonObjectData);

		virtual ~GuildWidgetImageData() noexcept = default;
	};

	/// Integration data. \brief Integration data.
	class DiscordCoreAPI_Dll IntegrationData : public DiscordEntity {
	  public:
		TimeStamp<std::chrono::milliseconds> syncedAt{};///< Time it was last synced at.
		Int32 expireGracePeriod{ 0 };///< How Int64 before the integration expires.
		ApplicationData application{};///< Application data.
		Int32 subscriberCount{ 0 };///< Number of current subscribers.
		Bool enableEmoticons{ true };///< Emoticons enabled?
		Int32 expireBehavior{ 0 };///< What to do upon expiry.
		Bool enabled{ false };///< Enabled?
		Bool syncing{ false };///< Is it syncing?
		AccountData account{};///< Account data.
		Bool revoked{ false };///< Has it been revoked?
		String name{};///< Name of the integration.
		String type{};///< Type of integration.
		Snowflake roleId{};///< Role Id.
		UserData user{};///< User data for the integration.

		IntegrationData() noexcept = default;

		IntegrationData(simdjson::ondemand::value jsonObjectData);

		virtual ~IntegrationData() noexcept = default;
	};

	class DiscordCoreAPI_Dll IntegrationDataVector {
	  public:
		IntegrationDataVector() noexcept = default;

		operator Vector<IntegrationData>();

		IntegrationDataVector(simdjson::ondemand::value jsonObjectData);

		virtual ~IntegrationDataVector() noexcept = default;

	  protected:
		Vector<IntegrationData> theIntegrationDatas{};
	};

	/// Audit log events. \brief Audit log events.
	enum class AuditLogEvent : Uint8 {
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
	class DiscordCoreAPI_Dll OptionalAuditEntryInfoData : public DiscordEntity {
	  public:
		String deleteMemberDays{};///< Number of days for which the member's Messages were deleted.
		String membersRemoved{};///< Number of members that were removed upon a prune.
		Snowflake applicationId{};///< ID of the app whose permissions were targeted APPLICATION_COMMAND_PERMISSION_UPDATE.
		String roleName{};///< Role name.
		Snowflake channelId{};///< Channel Id.
		Snowflake messageId{};///< Message Id.
		String count{};///< Count.
		String type{};///< Type.

		OptionalAuditEntryInfoData() noexcept = default;

		OptionalAuditEntryInfoData(simdjson::ondemand::value jsonObjectData);

		virtual ~OptionalAuditEntryInfoData() noexcept = default;
	};

	/// Audit log change data. \brief Audit log change data.
	struct DiscordCoreAPI_Dll AuditLogChangeData {
		JsonStringValue newValue{};///< New value.
		JsonStringValue oldValue{};///< Old value.
		String key{};///< The key of the audit log change.

		AuditLogChangeData() noexcept = default;

		AuditLogChangeData(simdjson::ondemand::value jsonObjectData);

		virtual ~AuditLogChangeData() noexcept = default;
	};

	/// Guild prune count data. \brief Guild prune count data.
	struct DiscordCoreAPI_Dll GuildPruneCountData {
		Int32 count{ 0 };

		GuildPruneCountData() noexcept = default;

		GuildPruneCountData(simdjson::ondemand::value jsonObjectData);

		virtual ~GuildPruneCountData() noexcept = default;
	};

	/// Audit log entry data. \brief Audit log entry data.
	class DiscordCoreAPI_Dll AuditLogEntryData : public DiscordEntity {
	  public:
		TimeStamp<std::chrono::milliseconds> createdTimeStamp{ "" };///< Time at which this entry was created.
		Vector<AuditLogChangeData> changes{};///< Array of audit log change data.
		OptionalAuditEntryInfoData options{};///< Audit log entry info data.
		AuditLogEvent actionType{};///< Audit log action type.
		String reason{};///< The reason that was entered for the given change.
		Snowflake targetId{};///< Id of the target User.
		Snowflake userId{};///< Id of the executing User.

		AuditLogEntryData() noexcept = default;

		AuditLogEntryData(simdjson::ondemand::value jsonObjectData);

		virtual ~AuditLogEntryData() noexcept = default;
	};

	/// Premium tier levels. \brief Premium tier levels.
	enum class PremiumTier : Uint8 {
		None = 0,///< None.
		Tier_1 = 1,///< Tier 1.
		Tier_2 = 2,///< Tier 2.
		Tier_3 = 3///< Tier 3.
	};

	/// Default Message notification levels. \brief Default Message notification
	/// levels.
	enum class DefaultMessageNotificationLevel : Uint8 {
		All_Messages = 0,///< All messages.
		Only_Mentions = 1///< Only mentions.
	};

	/// Explicit content filter levels. \brief Explicit content filter levels.
	enum class ExplicitContentFilterLevel : Uint8 {
		Disabled = 0,///< Disabled.
		Members_Without_Roles = 1,///< Members without roles.
		All_Members = 2///< All members.
	};

	/// MFA levels. \brief MFA levels.
	enum class MFALevel : Uint8 {
		None = 0,///< None.
		Elevated = 1///< Elevated.
	};

	/// Verification levels. \brief/// Verification levels.
	enum class VerificationLevel : Uint8 {
		None = 0,///< None.
		Low = 1,///< Low.
		Medium = 2,///< Medium.
		High = 3,///< High.
		Very_High = 4///< Very high.
	};

	/// Welcome screen Channel data. \brief Welcome screen Channel data.
	struct DiscordCoreAPI_Dll WelcomeScreenChannelData {
		String description{};///< Description of the welcome Channel.
		String emojiName{};///< Emoji name for the Channel.
		Snowflake channelId{};///< Id of the welcome Channel.
		Snowflake emojiId{};///< Emoji id for the Channel.

		WelcomeScreenChannelData() noexcept = default;

		WelcomeScreenChannelData(simdjson::ondemand::value jsonObjectData);

		virtual ~WelcomeScreenChannelData() noexcept = default;
	};

	/// Welcome screen data. \brief Welcome screen data.
	struct DiscordCoreAPI_Dll WelcomeScreenData {
		Vector<WelcomeScreenChannelData> welcomeChannels{};///< Welcome screen Channel data.
		String description{};///< Description of the welcome screen.

		WelcomeScreenData() noexcept = default;

		WelcomeScreenData(simdjson::ondemand::value jsonObjectData);

		virtual ~WelcomeScreenData() noexcept = default;
	};

	/// Stage instance privacy levels. \brief Stage instance privacy levels.
	enum class StageInstancePrivacyLevel : Uint8 {
		Public = 1,///< Public.
		Guild_Only = 2///< Guild only.
	};

	/// Stage instance data. \brief Stage instance data.
	class DiscordCoreAPI_Dll StageInstanceData : public DiscordEntity {
	  public:
		StageInstancePrivacyLevel privacyLevel{ 0 };///< Privacy level of the Channel.
		Bool discoverableDisabled{ false };///< Is it discoverable?
		Snowflake channelId{};///< The Channel's id.
		String topic{};///< The topic of the StageInstance.
		Snowflake guildId{};///< The Guild id for which the Channel exists in.

		StageInstanceData() noexcept = default;

		StageInstanceData(simdjson::ondemand::value jsonObjectData);

		virtual ~StageInstanceData() noexcept = default;
	};

	/// Sticker types. \brief Sticker types.
	enum class StickerType : Uint8 {
		Standard = 1,///< Standard.
		Guild = 2///< Guild.
	};

	enum class StickerFlags : Uint8 { Available = 1 << 0 };

	/// Data representing a single Sticker. \brief Data representing a single Sticker.
	class DiscordCoreAPI_Dll StickerData : public DiscordEntity {
	  public:
		StickerFormatType formatType{};///< Format type.
		String description{};///< Description of the Sticker.
		Int8 stickerFlags{ 0 };///< Sticker flags.
		Int32 nsfwLevel{ 0 };///< NSFW warning level.
		Int32 sortValue{ 0 };///< Where in the stack of stickers it resides.
		String packId{};///< Pack id of the Sticker.
		String asset{};///< Asset value for the Sticker.
		String name{};///< The Sticker's name.
		String tags{};///< Tags for the Sticker to use.
		StickerType type{};///< The type of Sticker.
		Snowflake guildId{};///< The Guild id for which the Sticker exists in.
		UserData user{};///< The User that uploaded the Guild Sticker.

		StickerData() noexcept = default;

		StickerData(simdjson::ondemand::value jsonObjectData);

		virtual ~StickerData() noexcept = default;
	};

	/// Data representing a single Guild preview. \brief Data representing a single Guild preview.
	struct DiscordCoreAPI_Dll GuildPreviewData {
		Int32 approximatePresenceCount{ 0 };
		Vector<String> features{};
		Vector<StickerData> stickers{};
		Int32 approximateMemberCount{ 0 };
		Vector<EmojiData> emojis{};
		String discoverySplash{};
		String description{};
		String splash{};
		String name{};
		String icon{};
		Snowflake id{};

		GuildPreviewData() noexcept = default;

		GuildPreviewData(simdjson::ondemand::value jsonObjectData);

		virtual ~GuildPreviewData() noexcept = default;
	};

	/// Afk timeout durations. \brief Afk timeout durations.
	enum class AfkTimeOutDurations : Int16 {
		Shortest = 60,///< Shortest.
		Short = 300,///< Short.
		Medium = 900,///< Medium.
		Long = 1800,///< Long.
		Longest = 3600///< Longest.
	};

	/// Guild NSFW level. \brief Guild NSFW level.
	enum class GuildNSFWLevel : Uint8 {
		Default = 0,///< Default.
		Explicit = 1,///< Explicit.
		Safe = 2,///< Safe.
		Age_Restricted = 3///< Age restricted.
	};

	/// System channel flags. \brief System channel flags.
	enum class SystemChannelFlags : Uint8 {
		Suppress_Join_Notifications = 1 << 0,///< Suppress member join notifications.
		Suppress_Premium_Subscriptions = 1 << 1,///< Suppress server boost notifications.
		Suppress_Guild_Reminder_Notifications = 1 << 2,///< Suppress server setup tips.
		Suppress_Join_Notification_Replies = 1 << 3///< Hide member join sticker reply buttons.
	};

	/// Guild flags. \brief Guild flags.
	enum class GuildFlags : Uint8 {
		WidgetEnabled = 1 << 0,///< Widget enabled.
		Unavailable = 1 << 1,///< Unavailable.
		Owner = 1 << 2,///< Owner.
		Large = 1 << 3,///< Large.
		Premium_Progress_Bar_Enabled = 1 << 4///< Premium progress bar enabled
	};

	/// Data structure representing a single Guild. \brief Data structure representing a single Guild.
	class DiscordCoreAPI_Dll GuildData : public DiscordEntity {
	  public:
		DiscordCoreClient* discordCoreClient{ nullptr };///< A pointer to the DiscordCoreClient.
		TimeStamp<std::chrono::milliseconds> joinedAt{};///< When the bot joined this Guild.
		VoiceConnection* voiceConnectionPtr{ nullptr };///< A pointer to the VoiceConnection, if present.
		Vector<Snowflake> guildScheduledEvents{};///< Array of Guild channels.
		Vector<PresenceUpdateData> presences{};///< Presence states for each of the GuildMembers..
		Vector<Snowflake> stageInstances{};///< Array of Guild channels.
		Vector<Snowflake> stickers{};///< Array of Guild channels.
		Vector<Snowflake> channels{};///< Array of Guild channels.
		Vector<Snowflake> threads{};///< Array of Guild channels.
		Vector<Snowflake> members{};///< Array of GuildMembers.
		Vector<Snowflake> roles{};///< Array of Guild roles.
		Vector<Snowflake> emoji{};///< Array of Guild channels.
		Int32 memberCount{ 0 };///< Member count.
		StringWrapper name{};///< The Guild's name.
		Snowflake ownerId{};///< User id of the Guild's owner.
		Int8 flags{ 0 };///< Guild flags.
		IconHash icon{};///< Url to the Guild's icon.

		GuildData() noexcept = default;

		GuildData& operator=(GuildData&&) noexcept = default;
		;

		GuildData(GuildData&&) noexcept = default;

		GuildData& operator=(const GuildData&) noexcept = default;

		GuildData(const GuildData&) noexcept = default;

		GuildData(simdjson::ondemand::value jsonObjectData);

		/// For connecting to an individual voice channel. \brief For connecting to an individual voice channel.
		/// \param guildMemberId An id of the guild member who's current voice channel to connect to.
		/// \param channelId An id of the voice channel to connect to.
		/// \param selfDeaf Whether or not to self-deafen the bot.
		/// \param selfMute Whether or not to self-mute the bot.
		/// \param streamType For usage with the Vc-to-Vc audio streaming option.
		/// \param streamInfo For usage with the Vc-to-Vc audio streaming option.
		/// \returns VoiceConnection* A pointer to the currently held voice connection, or nullptr if it failed to connect.
		VoiceConnection* connectToVoice(const Snowflake guildMemberId, const Snowflake channelId = Snowflake{ 0 }, Bool selfDeaf = false, Bool selfMute = false,
			StreamType streamType = StreamType::None, StreamInfo streamInfo = StreamInfo{});

		String getBannerUrl() noexcept;

		String getIconUrl() noexcept;

		Bool areWeConnected();

		Void disconnect();

		virtual ~GuildData() noexcept = default;
	};

	class DiscordCoreAPI_Dll GuildDataVector {
	  public:
		friend class Guilds;

		GuildDataVector() noexcept = default;

		operator Vector<GuildData>();

		GuildDataVector(simdjson::ondemand::value jsonObjectData);

		virtual ~GuildDataVector() noexcept = default;

	  protected:
		Vector<GuildData> theGuildDatas{};
	};

	/// Guild scheduled event privacy levels. \brief Guild scheduled event privacy levels.
	enum class GuildScheduledEventPrivacyLevel : Uint8 {
		Public = 1,///< Public.
		Guild_Only = 2///< Guild only.
	};

	/// GuildScheduledEventStatus. \brief GuildScheduledEventStatus.
	enum class GuildScheduledEventStatus : Uint8 {
		Scheduled = 1,///< Scheduled.
		Active = 2,///< Active.
		Completed = 3,///< Completed.
		Canceled = 4///< Cancelled.
	};

	/// Guild scheduled event entity types. \brief Guild scheduled event entity types.
	enum class GuildScheduledEventEntityType : Uint8 {
		None = 0,///< None.
		State_Instance = 1,///< Stage instance.
		Voice = 2,///< Voice.
		External = 3///< External.
	};

	/// Guild scheduled event entity metadata. \brief Guild scheduled event entity metadata.
	struct DiscordCoreAPI_Dll GuildScheduledEventMetadata {
		String location{};

		GuildScheduledEventMetadata() noexcept = default;

		GuildScheduledEventMetadata(simdjson::ondemand::value jsonObjectData);

		virtual ~GuildScheduledEventMetadata() noexcept = default;
	};

	/// Data representing a Guild Scheduled Event. \brief Data representing a Guild Scheduled Event.
	class DiscordCoreAPI_Dll GuildScheduledEventData : public DiscordEntity {
	  public:
		GuildScheduledEventPrivacyLevel privacyLevel{};///< The privacy level of the scheduled event.
		GuildScheduledEventMetadata entityMetadata{};///< Additional metadata for the Guild scheduled event.
		GuildScheduledEventEntityType entityType{};///< The type of the scheduled event.
		GuildScheduledEventStatus status{};///< The status of the scheduled event.
		String scheduledStartTime{};///< The time the scheduled event will start.
		String scheduledEndTime{};///< The time the scheduled event will end, required if entity_type is External.
		String description{};///< The description of the scheduled event(1 - 1000 characters.
		Uint32 userCount{ 0 };///< The number of users subscribed to the scheduled event.
		String creatorId{};///< The id of the User that created the scheduled event *.
		String entityId{};///< The id of an entity associated with a Guild scheduled event.
		Snowflake channelId{};///< The Channel id in which the scheduled event will be hosted, or null if scheduled entity type is External.
		Snowflake guildId{};///< The Guild id which the scheduled event belongs to.
		String name{};///< The name of the scheduled event(1 - 100 characters).
		UserData creator{};///< The User that created the scheduled event.

		GuildScheduledEventData() noexcept = default;

		GuildScheduledEventData(simdjson::ondemand::value jsonObjectData);

		virtual ~GuildScheduledEventData() noexcept = default;
	};

	/// Data representing a single GuildScheduledEventUser. \brief Data representing a single GuildScheduledEventUser.
	struct DiscordCoreAPI_Dll GuildScheduledEventUserData {
		Snowflake guildScheduledEventId{};///< The scheduled event id which the User subscribed to/
		GuildMemberData member{};///< Guild member data for this User for the Guild which this event belongs to, if any.
		UserData user{};///< User which subscribed to an event.

		GuildScheduledEventUserData() noexcept = default;

		GuildScheduledEventUserData(simdjson::ondemand::value jsonObjectData);

		virtual ~GuildScheduledEventUserData() noexcept = default;
	};

	class DiscordCoreAPI_Dll GuildScheduledEventUserDataVector {
	  public:
		GuildScheduledEventUserDataVector() noexcept = default;

		operator Vector<GuildScheduledEventUserData>();

		GuildScheduledEventUserDataVector(simdjson::ondemand::value jsonObjectData);

		virtual ~GuildScheduledEventUserDataVector() noexcept = default;

	  protected:
		Vector<GuildScheduledEventUserData> theGuildScheduledEventUserDatas{};
	};

	class DiscordCoreAPI_Dll GuildScheduledEventDataVector {
	  public:
		GuildScheduledEventDataVector() noexcept = default;

		operator Vector<GuildScheduledEventData>();

		GuildScheduledEventDataVector(simdjson::ondemand::value jsonObjectData);

		virtual ~GuildScheduledEventDataVector() noexcept = default;

	  protected:
		Vector<GuildScheduledEventData> theGuildScheduledEventDatas{};
	};

	/// Invite data. \brief Invite data.
	struct DiscordCoreAPI_Dll InviteData {
		TimeStamp<std::chrono::milliseconds> expiresAt{};///< When the invite expires.
		TimeStamp<std::chrono::milliseconds> createdAt{};///< Time it was created at.
		GuildScheduledEventData guildScheduledEvent{};///< Scheduled Guild event.
		Int32 approximatePresenceCount{ 0 };///< Approximate presence count.
		ApplicationData targetApplication{};///< Application data.
		Int32 approximateMemberCount{ 0 };///< Approximate member count.
		StageInstanceData stageInstance{};///< Stage instance data.
		Int32 targetType{ 0 };///< Target type.
		Bool temporary{ false };///< Is it temporary?
		UserData targetUser{};///< Target User of the invite.
		ChannelData channel{};///< Channel data of the Channel that the invite is for.
		Int32 maxUses{ 0 };///< Max number of uses.
		Int32 maxAge{ 0 };///< Maximum age of the invite.
		Snowflake guildId{};///< The Guild this invite is for.
		String code{};///< Unique invite code.
		UserData inviter{};///< The User who created the invite.
		GuildData guild{};///< Guild data of the Channel that the invite is for.
		Int32 uses{ 0 };///< The current number of uses.

		InviteData() noexcept = default;

		InviteData(simdjson::ondemand::value jsonObjectData);

		virtual ~InviteData() noexcept = default;
	};

	class DiscordCoreAPI_Dll InviteDataVector {
	  public:
		InviteDataVector() noexcept = default;

		operator Vector<InviteData>();

		InviteDataVector(simdjson::ondemand::value jsonObjectData);

		virtual ~InviteDataVector() noexcept = default;

	  protected:
		Vector<InviteData> theInviteDatas{};
	};

	/// Represents a Guild Template. \brief Represents a Guild Template.
	struct DiscordCoreAPI_Dll GuildTemplateData {
		GuildData serializedSourceGuild{};///< The Guild snapshot this template contains.
		String sourceGuildId{};///< The ID of the Guild this template is based on.
		String description{};///< The description for the template.
		Uint32 usageCount{ 0 };///< Number of times this template has been used.
		String creatorId{};///< The ID of the User who created the template.
		String createdAt{};///< When this template was created.
		String updatedAt{};///< When this template was last synced to the source Guild.
		Bool isDirty{ false };///< Whether the template has unsynced changes.
		String code{};///< The template code(unique ID).
		String name{};///< Template name.
		UserData creator{};///< The User who created the template.

		GuildTemplateData() noexcept = default;

		GuildTemplateData(simdjson::ondemand::value jsonObjectData);

		virtual ~GuildTemplateData() noexcept = default;
	};

	class DiscordCoreAPI_Dll GuildTemplateDataVector {
	  public:
		GuildTemplateDataVector() noexcept = default;

		operator Vector<GuildTemplateData>();

		GuildTemplateDataVector(simdjson::ondemand::value jsonObjectData);

		virtual ~GuildTemplateDataVector() noexcept = default;

	  protected:
		Vector<GuildTemplateData> theGuildTemplateDatas{};
	};

	/// Invite target types. \brief Invite target types.
	enum class InviteTargetTypes : Uint8 {
		Stream = 1,///< Stream.
		Embedded_Application = 2///< Embedded application.
	};

	/// WebHook types. \brief WebHook types.
	enum class WebHookType : Uint8 {
		Incoming = 1,///< Incoming.
		Channel_Follower = 2,///< Channel follower.
		Application = 3///< Application.
	};

	/// WebHook data. \brief WebHook data.
	class DiscordCoreAPI_Dll WebHookData : public DiscordEntity {
	  public:
		ChannelData sourceChannel{};///< Channel for which th WebHook was issued.
		Snowflake applicationId{};///< Application id.
		GuildData sourceGuild{};///< Source Guild id.
		WebHookType type{ 0 };///< Type of WebHook.
		String avatar{};///< Avatar of the WebHook.
		Snowflake channelId{};///< Channel id for which the WebHook was issued.
		String token{};///< Token of the WebHook.
		Snowflake guildId{};///< Guild id for which the WebHook was issued.
		String name{};///< Name of the WebHook.
		String url{};///< Url of the WebHook.
		UserData user{};///< User which create the WebHook.

		WebHookData() noexcept = default;

		WebHookData(simdjson::ondemand::value jsonObjectData);

		virtual ~WebHookData() noexcept = default;
	};

	class DiscordCoreAPI_Dll WebHookDataVector {
	  public:
		WebHookDataVector() noexcept = default;

		operator Vector<WebHookData>();

		WebHookDataVector(simdjson::ondemand::value jsonObjectData);

		virtual ~WebHookDataVector() noexcept = default;

	  protected:
		Vector<WebHookData> theWebHookDatas{};
	};

	/// Audit log data. \brief Audit log data.
	class DiscordCoreAPI_Dll AuditLogData {
	  public:
		Vector<GuildScheduledEventData> guildScheduledEvents{};///< Array of guild scheduled event objects.
		Vector<AutoModerationRuleData> autoModerationRules{};///< List of auto moderation rules referenced in the audit log.
		Vector<AuditLogEntryData> auditLogEntries{};///< Array of audit log entry objects.
		Vector<IntegrationData> integrations{};///< Array of partial integration objects.
		Vector<WebHookData> webhooks{};///< Array of webhook objects.
		Vector<ChannelData> threads{};///< Array of thread-specific channel objects.
		Vector<UserData> users{};///< Array of user objects.

		auto getAuditLogData(const Snowflake userIdOfChanger, AuditLogEvent auditLogType);

		auto getAuditLogData(AuditLogEvent auditLogType, const Snowflake userIdOfTarget);

		AuditLogData() noexcept = default;

		AuditLogData(simdjson::ondemand::value jsonObjectData);

		virtual ~AuditLogData() noexcept = default;
	};

	/// For removing a reaction. \brief For removing a reaction.
	struct DiscordCoreAPI_Dll ReactionRemoveData {
		Snowflake channelId{};
		Snowflake messageId{};
		Snowflake guildId{};
		Snowflake userId{};
		EmojiData emoji{};

		ReactionRemoveData() noexcept = default;

		ReactionRemoveData(simdjson::ondemand::value jsonObjectData);

		virtual ~ReactionRemoveData() noexcept = default;
	};

	/// For storing Interaction-related values. \brief For storing Interaction-related values.
	struct DiscordCoreAPI_Dll InteractionPackageData {
		String interactionToken{};
		Snowflake applicationId{};
		Snowflake interactionId{};
	};

	/// For storing Message-related values. \brief For storing Message-related values.
	struct DiscordCoreAPI_Dll MessagePackageData {
		Snowflake channelId{};
		Snowflake messageId{};
	};

	enum class JsonType { Unset = 0, Integer = 1, Float = 2, String = 3, Boolean = 4 };

	/// Data structure representing an ApplicationCommand's option choice. \brief Data structure representing an ApplicationCommand's option choice.
	class DiscordCoreAPI_Dll ApplicationCommandOptionChoiceData {
	  public:
		UMap<String, String> nameLocalizations{};///< Dictionary with keys in available locales Localization dictionary for the name field.
		JsonType type{};///< The value of the option.
		String name{};///< The name of the current choice.
		Double valueFloat{};
		Uint64 valueInt{};
		StringView valueString{};
		String valueStringReal{};
		Bool valueBool{};

		ApplicationCommandOptionChoiceData() noexcept = default;

		operator JsonObject();

		ApplicationCommandOptionChoiceData(simdjson::ondemand::value jsonObjectData);

		virtual ~ApplicationCommandOptionChoiceData() noexcept = default;
	};

	/// Data structure representing an ApplicationCommand's option. \brief Data structure representing an ApplicationCommand's option.
	struct DiscordCoreAPI_Dll ApplicationCommandOptionData {
		UMap<String, String> descriptionLocalizations{};///< Dictionary for the description localizations field.
		UMap<String, String> nameLocalizations{};///< Dictionary for the name localizations field.
		Vector<ApplicationCommandOptionChoiceData> choices{};///< A vector of possible choices for the current ApplicationCommand option.
		Vector<ApplicationCommandOptionData> options{};///< A vector of possible options for the current ApplicationCommand option.
		Vector<ChannelType> channelTypes{};///< Set when the ApplicationCommand option type is set to Channel.
		ApplicationCommandOptionType type{};///< The type of command option.
		Bool autocomplete{ false };///< If autocomplete interactions are enabled for this STRING, INTEGER, or NUMBER type option.
		String description{};///< A description of the current ApplicationCommand option.
		Bool required{ false };///< If the parameter is required or optional-- default false.
		Int32 minValue{ 0 };///< If the option is an INTEGER or NUMBER type, the minimum value permitted.
		Int32 maxValue{ 0 };///< If the option is an INTEGER or NUMBER type, the maximum value permitted.
		String name{};///< Name of the current ApplicationCommand option.

		ApplicationCommandOptionData() noexcept = default;

		operator JsonObject();

		ApplicationCommandOptionData(simdjson::ondemand::value jsonObjectData);

		virtual ~ApplicationCommandOptionData() noexcept = default;
	};

	/// Representing "TypingStart" data. \brief Representing "TypingStart" data.
	struct DiscordCoreAPI_Dll TypingStartData {
		GuildMemberData member{};
		Int32 timestamp{ 0 };
		Snowflake channelId{};
		Snowflake guildId{};
		Snowflake userId{};

		TypingStartData() noexcept = default;

		TypingStartData(simdjson::ondemand::value jsonObjectData);

		virtual ~TypingStartData() noexcept = default;
	};

	struct DiscordCoreAPI_Dll YouTubeFormat {
		String signatureCipher{};
		String audioSampleRate{};
		Int32 averageBitrate{ 0 };
		Int64 contentLength{ 0 };
		String audioQuality{};
		String downloadUrl{};
		String signature{};
		String mimeType{};
		String quality{};
		String codecs{};
		String aitags{};
		Int32 bitrate{ 0 };
		Int32 height{ 0 };
		Int32 width{ 0 };
		Int32 itag{ 0 };
		Int32 fps{ 0 };
		YouTubeFormat() noexcept = default;
		YouTubeFormat(simdjson::ondemand::value);
	};

	/// YouTube format data. \brief YouTube format data.
	class DiscordCoreAPI_Dll YouTubeFormatVector {
	  public:
		YouTubeFormatVector() noexcept = default;

		operator Vector<YouTubeFormat>();

		YouTubeFormatVector(simdjson::ondemand::value jsonObjectData);

		virtual ~YouTubeFormatVector() noexcept = default;

	  protected:
		Vector<YouTubeFormat> theFormats{};
	};

	/// Application command types. \brief Application command types.
	enum class ApplicationCommandType : Uint8 {
		Chat_Input = 1,///< Chat input.
		User = 2,///< User.
		Message = 3///< Message.
	};

	/// User command Interaction data. \brief User command Interaction data.
	struct DiscordCoreAPI_Dll UserCommandInteractionData {
		Snowflake targetId{};///< The target User's id.

		UserCommandInteractionData() noexcept = default;

		UserCommandInteractionData(simdjson::ondemand::value jsonObjectData);

		virtual ~UserCommandInteractionData() noexcept = default;
	};

	/// Message command interacction data. \brief Message command interacction data.
	struct DiscordCoreAPI_Dll MessageCommandInteractionData {
		Snowflake targetId{};///< The target Message's id.

		MessageCommandInteractionData() noexcept = default;

		MessageCommandInteractionData(simdjson::ondemand::value jsonObjectData);

		virtual ~MessageCommandInteractionData() noexcept = default;
	};

	/// Component types. \brief Component types.
	enum class ComponentType : Uint8 {
		ActionRow = 1,///< A container for other components.
		Button = 2,///< A button object.
		SelectMenu = 3,///< A select menu for picking from choices.
		TextInput = 4///< A text input object
	};

	/// Component Interaction data. \brief Component Interaction data.
	struct DiscordCoreAPI_Dll ComponentInteractionData {
		Vector<String> values{};///< The values of the components.
		ComponentType componentType{};///< The type of component.
		String customId{};///< The custom id of the Interaction entity.

		ComponentInteractionData() noexcept = default;

		ComponentInteractionData(simdjson::ondemand::value jsonObjectData);

		virtual ~ComponentInteractionData() noexcept = default;
	};

	/// Modal interaction data, for inputs from text modals. \brief Modal interaction data, for inputs from text modals.
	struct DiscordCoreAPI_Dll ModalInteractionData {
		String customIdSmall{};///< The custom id of a particular modal input.
		String customId{};///< The custom id of the Interaction entity.
		String value{};///< The input value of the modal.

		ModalInteractionData() noexcept = default;

		ModalInteractionData(simdjson::ondemand::value jsonObjectData);

		virtual ~ModalInteractionData() noexcept = default;
	};

	/// Allowable mentions for a Message. \brief Allowable mentions for a Message.
	class DiscordCoreAPI_Dll AllowedMentionsData {
	  public:
		Vector<String> parse{};///< A vector of allowed mention types to parse from the content.
		Vector<String> roles{};///< Array of role_ids to mention (Max size of 100)
		Vector<String> users{};///< Array of user_ids to mention (Max size of 100)
		Bool repliedUser{ false };///< For replies, whether to mention the author of the Message being replied to (default false).

		AllowedMentionsData() noexcept = default;

		AllowedMentionsData(simdjson::ondemand::value jsonObjectData);

		operator JsonObject();

		virtual ~AllowedMentionsData() noexcept = default;
	};

	/// Interaction types. \brief Interaction types.
	enum class InteractionType : Uint8 {
		Ping = 1,///< Ping.
		Application_Command = 2,///< Application command.
		Message_Component = 3,///< Message component.
		Application_Command_Autocomplete = 4,///< Application command autocomplete.
		Modal_Submit = 5///< Modal submission.
	};

	/// Represents a single selection from a select-menu. \brief Represents a single selection from a select-menu.
	class DiscordCoreAPI_Dll SelectOptionData {
	  public:
		String description{};///< Description of the select-menu-option.
		Bool _default{ false };///< Is it the default option?
		String label{};///< A visible label for the select-menu-option.
		String value{};///< A value for identifying the option.
		EmojiData emoji{};///< An optional emoji to put on it.

		SelectOptionData() noexcept = default;

		SelectOptionData(simdjson::ondemand::value jsonObjectData);

		virtual ~SelectOptionData() noexcept = default;
	};

	/// Button styles. \brief Button styles.
	enum class ButtonStyle : Uint8 {
		Primary = 1,///< Primary.
		Success = 3,///< Success.
		Secondary = 2,///< Secondary.
		Danger = 4,///< Danger.
		Link = 5///< Link.
	};

	/// Represents a single Message-component. \brief Represents a single Message-component.
	struct DiscordCoreAPI_Dll ComponentData {
		Vector<SelectOptionData> options{};///< Aray of select options the choices in the select, max 25.
		String placeholder{};///< Custom placeholder text if nothing is selected, max 100 characters.
		String customId{};///< A developer-defined identifier for the component, max 100 characters.
		Int32 minValues{ 0 };///< The minimum number of items that must be chosen; default 1, min 0, max 25.
		Int32 maxValues{ 0 };///< The maximum number of items that can be chosen; default 1, max 25.
		Bool disabled{ false };///< Whether the component is disabled, default false.
		Int32 minLength{ 0 };///< The minimum input length for a text input.
		Int32 maxLength{ 0 };///< The maximum input length for a text input.
		Bool required{ false };///< Whether this component is required to be filled.
		ComponentType type{};///< Integer component type.
		String label{};///< The label for this component.
		String value{};///< A pre-filled value for this component.
		String title{};///< Url, for url types.
		Int32 style{};///< One of ButtonStyle, or TextInputStyle.
		String url{};///< Url, for url types.
		EmojiData emoji{};///< Emoji name, id, and animated.
		ComponentData() noexcept = default;
		ComponentData(simdjson::ondemand::value);
	};

	/// Action row data of Message components. \brief Action row data of Message components.
	class DiscordCoreAPI_Dll ActionRowData {
	  public:
		Vector<ComponentData> components{};///< Array of components to make up the action-row.

		ActionRowData() noexcept = default;

		operator JsonObject();

		ActionRowData(simdjson::ondemand::value jsonObjectData);

		virtual ~ActionRowData() noexcept = default;
	};

	/// Interaction callback types. \brief Interaction callback types.
	enum class InteractionCallbackType : Uint8 {
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
		Vector<ApplicationCommandOptionChoiceData> choices{};///< Autocomplete choices(max of 25 choices).
		Vector<AttachmentData> attachments{};///< Array of partial attachment objects attachment objects with filename and description.
		Vector<ActionRowData> components{};///< Message components.
		AllowedMentionsData allowedMentions{};///< Allowed mentions data.
		Vector<EmbedData> embeds{};///< Message embeds.
		Vector<File> files{};///< Files for uploading.
		String customId{};///< A developer-defined identifier for the component, max 100 characters.
		String content{};///< Message content.
		String title{};///< The title of the popup modal.
		Int32 flags{ 0 };///< Flags.
		Bool tts{ false };///< Is it TTS?
	};

	/// Data structure representing an ApplicationCommand. \brief Data structure representing an ApplicationCommand.
	class DiscordCoreAPI_Dll ApplicationCommandData : public DiscordEntity {
	  public:
		UMap<String, String> descriptionLocalizations{};///< Dictionary with keys in available locales Localization dictionary for name field.
		UMap<String, String> nameLocalizations{};///< Dictionary with keys in available locales Localization dictionary for name field.
		Vector<ApplicationCommandOptionData> options{};///< A vector of possible options for the current ApplicationCommand.
		Permissions defaultMemberPermissions{};///< Set of permissions represented as a bit set all
		ApplicationCommandType type{};///< The type of ApplicationCommand.
		Bool dmPermission{ false };///< Indicates whether the command is available in DMs with the app, only for globally - scoped commands.
		String description{};///< A description of the current ApplicationCommand.
		Snowflake applicationId{};///< The current application id.
		String version{};///< An autoincremented version.
		String name{};///< Name of the current ApplicationCommand.
		Snowflake guildId{};///< (Where applicable) a Guild id for which guild to assign this ApplicationCommand to.

		ApplicationCommandData() noexcept = default;

		ApplicationCommandData(simdjson::ondemand::value jsonObjectData);

		virtual ~ApplicationCommandData() noexcept = default;
	};

	/// Channel mention data. \brief Channel mention data.
	class DiscordCoreAPI_Dll ChannelMentionData : public DiscordEntity {
	  public:
		Snowflake guildId{};///< The id of the Guild where it took place.
		String name{};///< The name of the Channel that was mentioned.
		ChannelType type{};///< The type of Channel that was mentioned.

		ChannelMentionData() noexcept = default;

		ChannelMentionData(simdjson::ondemand::value jsonObjectData);

		virtual ~ChannelMentionData() noexcept = default;
	};

	/// Data for when some Channel pins are updated. \brief Data for when some Channel pins are updated.
	struct DiscordCoreAPI_Dll ChannelPinsUpdateEventData {
		TimeStamp<std::chrono::milliseconds> lastPinTimeStamp{};///< The time of the last pinned Message.
		Snowflake channelId{};///< The id of the Channel within which the Message was pinned.
		Snowflake guildId{};///< The id of the Guild within which the Message was pinned.

		ChannelPinsUpdateEventData() noexcept = default;

		ChannelPinsUpdateEventData(simdjson::ondemand::value jsonObjectData);

		virtual ~ChannelPinsUpdateEventData() noexcept = default;
	};

	/// Data for when threads are synced. \brief Data for when threads are synced.
	struct DiscordCoreAPI_Dll ThreadListSyncData {
		Vector<ThreadMemberData> members{};///< Array of members that are a part of the Thread.
		Vector<String> channelIds{};///< The parent Channel ids whose threads are being synced. If omitted, then threads were synced for entire Guild.
		Vector<ChannelData> threads{};///< All active threads in the given channels that the current User can access.
		Snowflake guildId{};///< The id of the Guild for which the threads are being synced.

		ThreadListSyncData() noexcept = default;

		ThreadListSyncData(simdjson::ondemand::value jsonObjectData);

		virtual ~ThreadListSyncData() noexcept = default;
	};

	/// Represents a Thread-members-update. \brief Represents a Thread-members-update.
	class DiscordCoreAPI_Dll ThreadMembersUpdateData : public DiscordEntity {
	  public:
		Vector<ThreadMemberData> addedMembers{};///< New members added to the Thread.
		Vector<String> removedMemberIds{};///< Members who have been removed.
		Int32 memberCount{ 0 };///< Number of Guild-members in the Thread.
		Snowflake guildId{};///< Guild id of the Thread.

		ThreadMembersUpdateData() noexcept = default;

		ThreadMembersUpdateData(simdjson::ondemand::value jsonObjectData);

		virtual ~ThreadMembersUpdateData() noexcept = default;
	};

	/// Message Interaction data. \brief Message Interaction data.
	class DiscordCoreAPI_Dll MessageInteractionData : public DiscordEntity {
	  public:
		GuildMemberData member{};
		InteractionType type{};
		String name{};
		UserData user{};

		MessageInteractionData() noexcept = default;

		MessageInteractionData(simdjson::ondemand::value jsonObjectData);

		virtual ~MessageInteractionData() noexcept = default;
	};

	/// Message types. \brief Message types.
	enum class MessageType : Uint8 {
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
	enum class MessageFlags : Uint8 {
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
	enum class StickerItemType : Uint8 {
		Png = 1,///< Png.
		Apng = 2,///< Apng.
		Lottie = 3///< Lottie.
	};

	/// Represents a forum thread message. \brief Represents a forum thread message.
	struct DiscordCoreAPI_Dll ForumThreadMessageData {
		Vector<AttachmentData> attachments{};///< Array of partial attachment objects attachment objects with filename.
		Vector<ActionRowData> components{};///< Array of message component objects the components to include with the message.
		AllowedMentionsData allowedMentions{};///< Allowed mention object allowed mentions for the message.
		Vector<String> stickerIds{};///< Array of snowflakes IDs of up to 3 stickers in the server to send in the message.
		Vector<EmbedData> embeds{};///< Array of embed objects	embedded rich content (up to 6000 characters).
		Vector<File> files{};///< File contents the contents of the file being sent one of content, file, embed(s), sticker_ids.
		String content{};///< The message contents (up to 2000 characters).
		Int32 flags{ 0 };///< Flags to be set for the message.
	};

	/// Message Sticker item data. \brief Message Sticker item data.
	class DiscordCoreAPI_Dll StickerItemData : public DiscordEntity {
	  public:
		StickerItemType formatType{};///< Message Sticker item type.
		String name{};///< The name of the Sticker.

		StickerItemData() noexcept = default;

		StickerItemData(simdjson::ondemand::value jsonObjectData);

		virtual ~StickerItemData() noexcept = default;
	};

	/// The core of a Message's data structure. \brief The core of a Message's data structure.
	class DiscordCoreAPI_Dll MessageDataOld : public DiscordEntity {
	  public:
		TimeStamp<std::chrono::milliseconds> editedTimestamp{};///< The time at which it was edited.
		Vector<ChannelMentionData> mentionChannels{};///< array of Channel mention data.
		TimeStamp<std::chrono::milliseconds> timestamp{};///< The timestamp of when the Message was created.
		Vector<StickerItemData> stickerItems{};///< Array of Message Sticker item data.
		Vector<AttachmentData> attachments{};///< Array of attachment data.
		MessageReferenceData messageReference{};///< Message reference data.
		Vector<String> mentionRoles{};///< Vector of "mention roles" ids.
		Vector<ActionRowData> components{};///< Array of action row data.
		Vector<ReactionData> reactions{};//< Array of reaction data.
		MessageInteractionData interaction{};///< Message Interaction data.
		Vector<StickerData> stickers{};///< Array of Message Sticker data.
		Vector<UserData> mentions{};///< Array of User data, for individual's that were mentioned.
		Vector<EmbedData> embeds{};///< Array of Message embeds.
		MessageActivityData activity{};///< Message activity data.
		Bool mentionEveryone{ false };///< Does the Message mention everyone?
		ApplicationData application{};///< Application data.
		Snowflake applicationId{};///< Application id.
		GuildMemberData member{};///< The author's Guild member data.
		String content{};///< The Message's content.
		Snowflake channelId{};///< The Channel it was sent in.
		Snowflake webHookId{};///< WebHook id of the Message, if applicable.
		Bool pinned{ false };///< Is it pinned?
		ChannelData thread{};///< The Thread that the Message was sent in, if applicable.
		String nonce{};///< Nonce.
		Snowflake guildId{};///< The id of the Guild the Message was sent in.
		Int32 flags{ 0 };///< Flags.
		MessageType type{};///< Message type.
		UserData author{};///< The author's User data.
		Bool tts{ false };///< Is it a text-to-speech Message?

		MessageDataOld() noexcept = default;

		MessageDataOld(simdjson::ondemand::value jsonObjectData);

		virtual ~MessageDataOld() noexcept = default;
	};

	/// Data structure representing a single Message. \brief Data structure representing a single Message.
	class DiscordCoreAPI_Dll MessageData : public MessageDataOld {
	  public:
		std::unique_ptr<MessageDataOld> referencedMessage{ std::make_unique<MessageDataOld>() };///< The referenced Message, to reply to.

		MessageData& operator=(const MessageData& other);

		MessageData(const MessageData& other);

		MessageData() noexcept = default;

		MessageData(simdjson::ondemand::value jsonObjectData);

		virtual ~MessageData() noexcept = default;
	};

	/// Resolved data. \brief Resolved data.
	struct DiscordCoreAPI_Dll ResolvedData {
		UMap<Uint64, AttachmentData> attachments{};///< Map of Snowflakes to attachment objects the ids and attachment objects.
		UMap<Uint64, GuildMemberData> members{};///< Map full of GuildMemeberData.
		UMap<Uint64, MessageData> messages{};///< Map full of messageData->
		UMap<Uint64, ChannelData> channels{};///< Map full of ChannelData.
		UMap<Uint64, UserData> users{};///< Map full of UserData.
		UMap<Uint64, RoleData> roles{};///< Map full of RoleData.

		ResolvedData() noexcept = default;

		ResolvedData(simdjson::ondemand::value jsonObjectData);
	};

	/// Represents a Sticker pack. \brief Represents a Sticker pack.
	struct DiscordCoreAPI_Dll StickerPackData {
		Vector<StickerData> stickers{};///< Array of Sticker objects	the stickers in the pack.
		String coverStickerId{};///< Id of a Sticker in the pack which is shown as the pack's icon.
		String bannerAssetId{};///< Id of the Sticker pack's banner image.
		String description{};///< Description of the Sticker pack.
		String skuId{};///< Id of the pack's SKU.
		String name{};///< Name of the Sticker pack.
		Snowflake Id{};///< Id of the Sticker pack.

		StickerPackData() noexcept = default;

		StickerPackData(simdjson::ondemand::value jsonObjectData);

		virtual ~StickerPackData() noexcept = default;
	};

	class DiscordCoreAPI_Dll StickerPackDataVector {
	  public:
		StickerPackDataVector() noexcept = default;

		operator Vector<StickerPackData>();

		StickerPackDataVector(simdjson::ondemand::value jsonObjectData);

		virtual ~StickerPackDataVector() noexcept = default;

	  protected:
		Vector<StickerPackData> theStickerPackDatas{};
	};

	/// Connection visibility types. \brief Connection visibility types.
	enum class ConnectionVisibilityTypes : Uint8 {
		None = 0,///< None.
		Everyone = 1///< Everyone.
	};

	/// Represents a single User Connection. \brief Represents a single User Connection.
	struct DiscordCoreAPI_Dll ConnectionData {
		Vector<IntegrationData> integrations{};///< An array of partial server integrations.
		ConnectionVisibilityTypes visibility{};///< Visibility of this connection.
		Bool showActivity{ false };///< Whether activities related to this connection will be shown in presence updates.
		Bool friendSync{ false };///< Whether friend sync is enabled for this connection.
		Bool verified{ false };///< Whether the connection is verified.
		Bool revoked{ false };///< Whether the connection is revoked.
		String name{};///< The userName of the connection account.
		String type{};///< The service of the connection(twitch, youtube).
		Snowflake id{};///< Id of the connection account.

		ConnectionData() noexcept = default;

		ConnectionData(simdjson::ondemand::value jsonObjectData);

		virtual ~ConnectionData() noexcept = default;
	};

	class DiscordCoreAPI_Dll ConnectionDataVector {
	  public:
		ConnectionDataVector() noexcept = default;

		ConnectionDataVector(simdjson::ondemand::value jsonObjectData) noexcept;

		operator Vector<ConnectionData>();

		virtual ~ConnectionDataVector() noexcept = default;

	  protected:
		Vector<ConnectionData> theConnectionDatas{};
	};

	/// ApplicationCommand Interaction data option. \brief ApplicationCommand Interaction data option.
	struct DiscordCoreAPI_Dll ApplicationCommandInteractionDataOption {
		Vector<ApplicationCommandInteractionDataOption> options{};///< ApplicationCommand Interaction data options.
		ApplicationCommandOptionType type{};///< The type of ApplicationCommand options.
		JsonStringValue value{};///< The value if it's an Int32.
		Bool focused{ false };///< 	True if this option is the currently focused option for autocomplete.
		String name{};///< The name of the current option.

		ApplicationCommandInteractionDataOption() noexcept = default;

		ApplicationCommandInteractionDataOption(simdjson::ondemand::value jsonObjectData);

		virtual ~ApplicationCommandInteractionDataOption() noexcept = default;
	};

	/// ApplicationCommand Interaction data. \brief ApplicationCommand Interaction data.
	class DiscordCoreAPI_Dll ApplicationCommandInteractionData : public DiscordEntity {
	  public:
		Vector<ApplicationCommandInteractionDataOption> options{};///< ApplicationCommand Interaction data options.
		ApplicationCommandType type{};///< The type of ApplicationCommand.
		ResolvedData resolved{};///< Resolved data.
		Snowflake guildId{};///< The guild that the command took place in.
		String name{};///< The name of the command.

		ApplicationCommandInteractionData() noexcept = default;

		ApplicationCommandInteractionData(simdjson::ondemand::value jsonObjectData);

		virtual ~ApplicationCommandInteractionData() noexcept = default;
	};

	/// Interaction data data. \brief Interaction data data.
	struct DiscordCoreAPI_Dll InteractionDataData {
		ApplicationCommandInteractionData applicationCommandData{};///< ApplicationCommand Interaction data.
		MessageCommandInteractionData messageInteractionData{};///< Message command Interaction data.
		UserCommandInteractionData userInteractionData{};///< User command Interaction data.
		ComponentInteractionData componentData{};///< Component Interaction data.
		ModalInteractionData modalData{};///< Modal Interaction data.

		InteractionDataData() noexcept = default;

		InteractionDataData(simdjson::ondemand::value jsonObjectData);

		virtual ~InteractionDataData() noexcept = default;
	};

	/// Interaction data. \brief Interaction data.
	class DiscordCoreAPI_Dll InteractionData : public DiscordEntity {
	  public:
		Permissions appPermissions{};///< Bitwise set of permissions the app or bot has within the channel the interaction was sent from.
		InteractionDataData data{};///< The Interaction's data.
		String guildLocale{};///< The guild's preferred locale, if invoked in a guild.
		Snowflake applicationId{};///< The application's id.
		GuildMemberData member{};///< The data of the Guild member who sent the Interaction, if applicable.
		InteractionType type{};///< The type of Interaction.
		MessageData message{};///< The Message that the Interaction came through on, if applicable.
		Snowflake channelId{};///< The Channel the Interaction was sent in.
		Int32 version{ 0 };///< The Interaction version.
		String locale{};///< The selected language of the invoking user.
		String token{};///< The Interaction token.
		Snowflake guildId{};///< The Guild id of the Guild it was sent in.
		UserData user{};///< The User data of the sender of the Interaction.

		InteractionData() noexcept = default;

		InteractionData(simdjson::ondemand::value jsonObjectData);

		virtual ~InteractionData() noexcept = default;
	};

	/// Data from the SessionStart info. \brief Data from the SessionStart info.
	struct DiscordCoreAPI_Dll SessionStartData {
		Uint32 maxConcurrency{ 0 };///< The number of identify requests allowed per 5 seconds.
		Uint32 resetAfter{ 0 };///< The number of std::chrono::milliseconds after which the limit resets.
		Uint32 remaining{ 0 };///< The remaining number of session starts the current User is allowed.
		Uint32 total{ 0 };///< The total number of session starts the current User is allowed.

		SessionStartData() noexcept = default;

		SessionStartData(simdjson::ondemand::value jsonObjectData);

		virtual ~SessionStartData() noexcept = default;
	};

	/// Data from the GetGatewayBot endpoint. \brief Data from the GetGatewayBot endpoint.
	struct DiscordCoreAPI_Dll GatewayBotData {
		SessionStartData sessionStartLimit{};///< Information on the current session start limit.
		Uint32 shards{ 0 };///< The recommended number of shards to use when connecting.
		String url{};///< The WSS Url that can be used for connecting to the gateway.

		GatewayBotData() noexcept = default;

		GatewayBotData(simdjson::ondemand::value jsonObjectData);

		virtual ~GatewayBotData() noexcept = default;
	};

	/// Text input style for modals. \brief Text input style for modals.
	enum class TextInputStyle : Uint8 {
		Short = 1,///< A single-line input.
		Paragraph = 2///< A multi-line input.
	};

	/// Data representing a Guild Emoji Update event. \brief Data representing a Guild Emoji Update event.
	struct DiscordCoreAPI_Dll GuildEmojisUpdateEventData {
		Vector<EmojiData> emojis{};
		Snowflake guildId{};

		GuildEmojisUpdateEventData() noexcept = default;

		GuildEmojisUpdateEventData(simdjson::ondemand::value jsonObjectData);

		virtual ~GuildEmojisUpdateEventData() noexcept = default;
	};

	/// Data representing a Guild Sticker Update event. \brief Data representing a Guild Stickers Update event.
	struct DiscordCoreAPI_Dll GuildStickersUpdateEventData {
		Vector<StickerData> stickers{};
		Snowflake guildId{};

		GuildStickersUpdateEventData() noexcept = default;

		GuildStickersUpdateEventData(simdjson::ondemand::value jsonObjectData);

		virtual ~GuildStickersUpdateEventData() noexcept = default;
	};

	struct DiscordCoreAPI_Dll GuildMembersChunkEventData {
		Vector<PresenceUpdateData> presences{};
		Vector<GuildMemberData> members{};
		Vector<String> notFound{};
		Int32 chunkIndex{ 0 };
		Int32 chunkCount{ 0 };
		String nonce{};
		Snowflake guildId{};

		GuildMembersChunkEventData() noexcept = default;

		GuildMembersChunkEventData(simdjson::ondemand::value jsonObjectData);

		virtual ~GuildMembersChunkEventData() noexcept = default;
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
		friend class CommandData;
		friend class InputEvents;

		InputEventResponseType responseType{};///< The type of response that this input value represents.

		InputEventData& operator=(InputEventData&& other) noexcept;

		InputEventData(InputEventData&& other) noexcept;

		InputEventData& operator=(const InputEventData& other) noexcept;

		InputEventData(const InputEventData& other) noexcept;

		InputEventData& operator=(const InteractionData& other) noexcept;

		InputEventData(const InteractionData& interactionData) noexcept;

		InputEventData() noexcept = default;

		/// Returns the userName of the last User to trigger this input-event. \brief Returns the userName of the last User to trigger this input-event.
		/// \returns String A string containing the User name.
		String getUserName() const;

		/// Gets the avatar Url of the last User to trigger this input-event. \brief Gets the avatar Url of the last User to trigger this input-event.
		/// \returns String A string containing the avatar Url.
		String getAvatarUrl() const;

		/// Returns the Message embeds that are on the Message, if applicable. \brief Returns the Message embeds that are on the Message, if applicable.
		/// \returns Vector A vector containing the EmbedData.
		Vector<EmbedData> getEmbeds() const;

		/// Returns the Message components that are on the Message, if applicable. \brief Returns the Message components that are on the Message, if applicable.
		/// \returns Vector A vector containing ActionRowData.
		Vector<ActionRowData> getComponents() const;

		/// Returns the User id of the last requester of this input-event. \brief Returns the User id of the last requester of this input-event.
		/// \returns Snowflake A Snowflake containing the author's id.
		Snowflake getAuthorId() const;

		/// Returns the Interaction id, if appplicable, of this input-event. \brief Returns the Interaction id, if appplicable, of this input-event.
		/// \returns Snowflake A Snowflake containing the Interaction id.
		Snowflake getInteractionId() const;

		/// Returns the application id. \brief Returns the application id.
		/// \returns Snowflake A Snowflake containing the application id.
		Snowflake getApplicationId() const;

		/// Returns the Channel id of this input-event. \brief Returns the Channel id of this input-event.
		/// \returns Snowflake A Snowflake containing the Channel id.
		Snowflake getChannelId() const;

		/// Returns the Interaction token, if applicable, of this input-event. \brief Returns the Interaction token, if applicable, of this input-event.
		/// \returns String A string containing the Interaction token.
		String getInteractionToken() const;

		/// Returns the Guild id, of this input-event. \brief Returns the Guild id, of this input-event.
		/// \returns Snowflake A Snowflake containing the Guild id.
		Snowflake getGuildId() const;

		/// Returns the Message id, if applicable, of this input-event. \brief Returns the Message id, if applicable, of this input-event.
		/// \returns Snowflake A Snowflake containing the Message id.
		Snowflake getMessageId() const;

		/// Returns the Interaction data, if applicable, of this input-event. \brief Returns the InteractionData, if applicable, of this input-event.
		/// \returns InteractionData An InteractionData structure.
		InteractionData getInteractionData() const;

		/// Returns the Message data, if applicable, of this input-event. \brief Returns the Message data, if applicable, of this input-event.
		/// \returns MessageData A MessageData structure.
		MessageData getMessageData() const;

		virtual ~InputEventData() noexcept = default;

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

		RespondToInputEventData& operator=(const InteractionData& dataPackage);

		RespondToInputEventData(const InteractionData& dataPackage);

		RespondToInputEventData& operator=(const InputEventData& dataPackage);

		RespondToInputEventData(const InputEventData& dataPackage);

		/// Adds a button to the response Message. \brief Adds a button to the response Message.
		/// \param disabled Whether the button is active or not.
		/// \param customIdNew A custom id to give for identifying the button.
		/// \param buttonLabel A visible label for the button.
		/// \param buttonStyle The style of the button.
		/// \param emojiName An emoji name, if desired.
		/// \param emojiId An emoji id, if desired.
		/// \param url A url, if applicable.
		/// \returns RespondToInputEventData& A reference to this data structure.
		RespondToInputEventData& addButton(Bool disabled, const String& customIdNew, const String& buttonLabel, ButtonStyle buttonStyle, const String& emojiName = "",
			Snowflake emojiId = Snowflake{ 0 }, const String& url = "");

		/// Adds a select-menu to the response Message. \brief Adds a select-menu to the response Message.
		/// \param disabled Whether the select-menu is active or not.
		/// \param customIdNew A custom id to give for identifying the select-menu.
		/// \param options A vector of select-menu-options to offer.
		/// \param placeholder Custom placeholder text if nothing is selected, max 100 characters.
		/// \param maxValues Maximum number of selections that are possible.
		/// \param minValues Minimum required number of selections that are required.
		/// \returns RespondToInputEventData& A reference to this data structure.
		RespondToInputEventData& addSelectMenu(Bool disabled, const String& customIdNew, Vector<SelectOptionData> options, const String& placeholder, Int32 maxValues,
			Int32 minValues);

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
		RespondToInputEventData& addModal(const String& topTitleNew, const String& topCustomIdNew, const String& titleNew, const String& customIdNew, Bool required,
			Int32 minLength, Int32 maxLength, TextInputStyle inputStyle, const String& label = "", const String& placeholder = "");

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
		RespondToInputEventData& addContent(const String& dataPackage);

		/// For setting the tts status of a response. \brief For setting the tts status of a response.
		/// \param enabledTTs A Bool.
		/// \returns RespondToInputEventData& A reference to this data structure.
		RespondToInputEventData& setTTSStatus(Bool enabledTTs);

		/// For setting the choices of an autocomplete response. \brief For setting the choices of an autocomplete response.
		/// \param theValue An simdjson::ondemand::object value that is either a Float, Int32 or a string.
		/// \param theName A string for the name of the choice.
		/// \param theNameLocalizations A UMap<String, String> for the name localizations.
		/// \returns RespondToInputEventData& A reference to this data structure.
		RespondToInputEventData& setAutoCompleteChoice(simdjson::ondemand::object theValue, const String& theName, UMap<String, String> theNameLocalizations);

		/// For setting the direct-Message User target of a response. \brief For setting the direct-Message User target of a response.
		/// \param targetUserIdNew A string, containing the target User's id.
		/// \returns RespondToInputEventData& A reference to this data structure.
		RespondToInputEventData& setTargetUserID(const Snowflake targetUserIdNew);

	  protected:
		Vector<ApplicationCommandOptionChoiceData> choices{};
		Vector<ActionRowData> components{};
		AllowedMentionsData allowedMentions{};
		Vector<EmbedData> embeds{};
		String interactionToken{};
		InputEventResponseType type{};///< The type of response to make.
		InteractionType eventType{};
		Vector<File> files{};
		Snowflake interactionId{};
		Snowflake applicationId{};
		Snowflake targetUserId{};
		String customId{};
		String content{};
		Snowflake channelId{};
		Snowflake messageId{};
		String title{};
		Int32 flags{ 0 };
		Bool tts{ false };
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
		MessageResponseBase& addButton(Bool disabled, const String& customIdNew, const String& buttonLabel, ButtonStyle buttonStyle, const String& emojiName = "",
			Snowflake emojiId = Snowflake{ 0 }, const String& url = "");

		/// Adds a select-menu to the response Message. \brief Adds a select-menu to the response Message.
		/// \param disabled Whether the select-menu is active or not.
		/// \param customIdNew A custom id to give for identifying the select-menu.
		/// \param options A vector of select-menu-options to offer.
		/// \param placeholder Custom placeholder text if nothing is selected, max 100 characters.
		/// \param maxValues Maximum number of selections that are possible.
		/// \param minValues Minimum required number of selections that are required.
		/// \returns MessageResponseBase& A reference to this data structure.
		MessageResponseBase& addSelectMenu(Bool disabled, const String& customIdNew, Vector<SelectOptionData> options, const String& placeholder, Int32 maxValues, Int32 minValues);

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
		MessageResponseBase& addModal(const String& topTitleNew, const String& topCustomIdNew, const String& titleNew, const String& customIdNew, Bool required, Int32 minLength,
			Int32 maxLength, TextInputStyle inputStyle, const String& label = "", const String& placeholder = "");

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
		MessageResponseBase& addContent(const String& dataPackage);

		/// For setting the tts status of a response. \brief For setting the tts status of a response.
		/// \param enabledTTs A Bool.
		/// \returns MessageResponseBase& A reference to this data structure.
		MessageResponseBase& setTTSStatus(Bool enabledTTs);

	  protected:
		Vector<ActionRowData> components{};
		AllowedMentionsData allowedMentions{};
		Vector<EmbedData> embeds{};
		Vector<File> files{};
		String customId{};
		String content{};
		String title{};
		Int32 flags{ 0 };
		Bool tts{ false };
	};

	/// Interaction response data. \brief Interaction response data.
	struct DiscordCoreAPI_Dll InteractionResponseData {
		InteractionResponseData& operator=(const RespondToInputEventData& other);

		InteractionResponseData(const RespondToInputEventData& other);

		InteractionResponseData() noexcept = default;

		operator JsonObject();

		InteractionCallbackData data{};///< Interaction ApplicationCommand callback data.
		InteractionCallbackType type{};///< Interaction callback type.
	};

	/// Guild application command permissions data. \brief Guild application command permissions data.
	class DiscordCoreAPI_Dll GuildApplicationCommandPermissionData : public DiscordEntity {
	  public:
		Vector<ApplicationCommandPermissionData> permissions{};
		Snowflake applicationId{};
		Snowflake guildId{};

		virtual ~GuildApplicationCommandPermissionData() noexcept = default;
	};

	/**@}*/

	/// Song types. \brief Song types.
	enum class SongType : Uint8 {
		YouTube = 0,///< YouTube.
		SoundCloud = 1///< SoundCloud.
	};

	/// Represents a download Url. \brief Represents a download Url.
	struct DiscordCoreAPI_Dll DownloadUrl {
		Int32 contentSize{ 0 };
		String urlPath{};
	};

	/**
	 * \addtogroup voice_connection
	 * @{
	 */

	struct DiscordCoreAPI_Dll MediaTranscoding {
		MediaTranscoding() noexcept = default;
		MediaTranscoding(simdjson::ondemand::value);
		String thePreset{};
		String theUrl{};
	};

	/// A song from the various platforms. \brief A song from the various platforms.
	class DiscordCoreAPI_Dll Song {
	  public:
		friend class DiscordCoreInternal::SoundCloudRequestBuilder;
		friend class DiscordCoreInternal::YouTubeRequestBuilder;
		friend class DiscordCoreInternal::SoundCloudAPI;
		friend class DiscordCoreInternal::YouTubeAPI;
		friend class SongAPI;

		Vector<DownloadUrl> finalDownloadUrls{};
		SongType type{ SongType::SoundCloud };///< The type of song.
		String secondDownloadUrl{};
		String firstDownloadUrl{};
		String html5PlayerFile{};
		String addedByUserName{};///< The User name of the individual who added this Song to the playlist.
		String thumbnailUrl{};///< The Url of the thumbnail image of this Song.
		Uint64 contentLength{ 0 };
		String html5Player{};
		String description{};///< A description of the Song.
		Snowflake addedByUserId{};///< The User id of the individual who added this Song to the playlist.
		String songTitle{};///< The title of the Song.
		String duration{};///< The duration of the Song.
		String viewUrl{};///< The url for listening to this Song through a browser.
		String songId{};

		Song() noexcept = default;

		Song(simdjson::ondemand::value jsonObjectData);

		virtual ~Song() noexcept = default;

	  protected:
		String trackAuthorization{};
		Bool doWeGetSaved{ false };
		YouTubeFormat format{};
	};

	/// Song completion event data. \brief Song completion event data.
	struct DiscordCoreAPI_Dll SongCompletionEventData {
		GuildMemberData guildMember{};///< The sending GuildMember.
		Bool wasItAFail{ false };///< Is this a replay? (Did a track recently fail to play?)
		Song previousSong{};///< The previously played Song.
		GuildData guild{};///< The sending Guild.
	};

	/// Playlist of songs and other variables. \brief Playlist of songs and other variables.
	struct DiscordCoreAPI_Dll Playlist {
		Bool isLoopSongEnabled{ false };///< Is looping of Songs currently enabled?
		Bool isLoopAllEnabled{ false };///< Is looping of the entire Playlist currently enabled?
		Vector<Song> songQueue{};///< The list of Songs that are stored to be played.
		Song currentSong{};///< The current Song that is playing.
	};

	/**@}*/

	/**
	 * \addtogroup utilities
	 * @{
	 */

	struct DiscordCoreAPI_Dll JsonObjectValue {
		UMap<String, JsonStringValue> theValues{};
	};

	template<typename ReturnType> auto getArgument(JsonObjectValue& optionsArgs, StringView theArgName);

	template<> inline auto getArgument<Int64>(JsonObjectValue& optionsArgs, StringView theArgName) {
		auto theValue = optionsArgs.theValues[theArgName.data()];
		switch (theValue.theType) {
			case ValueType::Int64: {
				return stoll(theValue.theValue);
			}
		}
		return 0ll;
	}

	template<> inline auto getArgument<Int32>(JsonObjectValue& optionsArgs, StringView theArgName) {
		auto theValue = optionsArgs.theValues[theArgName.data()];
		switch (theValue.theType) {
			case ValueType::Int64: {
				return static_cast<Int32>(stoll(theValue.theValue));
			}
		}
		return 0;
	}

	template<> inline auto getArgument<Int16>(JsonObjectValue& optionsArgs, StringView theArgName) {
		auto theValue = optionsArgs.theValues[theArgName.data()];
		switch (theValue.theType) {
			case ValueType::Int64: {
				return static_cast<Int16>(stoll(theValue.theValue));
			}
		}
		return static_cast<Int16>(0);
	}

	template<> inline auto getArgument<Int8>(JsonObjectValue& optionsArgs, StringView theArgName) {
		auto theValue = optionsArgs.theValues[theArgName.data()];
		switch (theValue.theType) {
			case ValueType::Int64: {
				return static_cast<Int8>(stoll(theValue.theValue));
			}
		}
		return static_cast<Int8>(0);
	}

	template<> inline auto getArgument<Uint64>(JsonObjectValue& optionsArgs, StringView theArgName) {
		auto theValue = optionsArgs.theValues[theArgName.data()];
		switch (theValue.theType) {
			case ValueType::Uint64: {
				return stoull(theValue.theValue);
			}
		}
		return 0ull;
	}

	template<> inline auto getArgument<Uint32>(JsonObjectValue& optionsArgs, StringView theArgName) {
		auto theValue = optionsArgs.theValues[theArgName.data()];
		switch (theValue.theType) {
			case ValueType::Uint64: {
				return static_cast<Uint32>(stoull(theValue.theValue));
			}
		}
		return static_cast<Uint32>(0);
	}

	template<> inline auto getArgument<Uint16>(JsonObjectValue& optionsArgs, StringView theArgName) {
		auto theValue = optionsArgs.theValues[theArgName.data()];
		switch (theValue.theType) {
			case ValueType::Uint64: {
				return static_cast<Uint16>(stoull(theValue.theValue));
			}
		}
		return static_cast<Uint16>(0);
	}

	template<> inline auto getArgument<Uint8>(JsonObjectValue& optionsArgs, StringView theArgName) {
		auto theValue = optionsArgs.theValues[theArgName.data()];
		switch (theValue.theType) {
			case ValueType::Uint64: {
				return static_cast<Uint8>(stoull(theValue.theValue));
			}
		}
		return static_cast<Uint8>(0);
	}

	template<> inline auto getArgument<Float>(JsonObjectValue& optionsArgs, StringView theArgName) {
		auto theValue = optionsArgs.theValues[theArgName.data()];
		switch (theValue.theType) {
			case ValueType::Float: {
				return stold(theValue.theValue);
			}
		}
		return 0.0l;
	}

	template<> inline auto getArgument<Double>(JsonObjectValue& optionsArgs, StringView theArgName) {
		auto theValue = optionsArgs.theValues[theArgName.data()];
		switch (theValue.theType) {
			case ValueType::Float: {
				return stold(theValue.theValue);
			}
		}
		return 0.0L;
	}

	template<> inline auto getArgument<String>(JsonObjectValue& optionsArgs, StringView theArgName) {
		auto theValue = optionsArgs.theValues[theArgName.data()];
		switch (theValue.theType) {
			case ValueType::String: {
				return theValue.theValue;
			}
		}
		return String{};
	}

	template<> inline auto getArgument<Bool>(JsonObjectValue& optionsArgs, StringView theArgName) {
		auto theValue = optionsArgs.theValues[theArgName.data()];
		switch (theValue.theType) {
			case ValueType::Bool: {
				return static_cast<Bool>(stoull(theValue.theValue));
			}
		}
		return false;
	}

	/// Command data, for functions executed by the CommandController. \brief Command data, for functions executed by the CommandController.
	class DiscordCoreAPI_Dll CommandData {
	  public:
		String subCommandGroupName{};
		String subCommandName{};
		InputEventData eventData{};
		String commandName{};
		JsonObjectValue optionsArgs{};

		CommandData() noexcept = default;

		CommandData(InputEventData inputEventData);

		CommandData(simdjson::ondemand::value jsonObjectData);

		virtual ~CommandData() noexcept = default;
	};

	/// Base arguments for the command classes. \brief Base arguments for the command classes.
	struct DiscordCoreAPI_Dll BaseFunctionArguments : public CommandData {
		DiscordCoreClient* discordCoreClient{ nullptr };///< A pointer to the instance of DiscordCoreClient.

		BaseFunctionArguments() noexcept = default;

		BaseFunctionArguments(CommandData commandData, DiscordCoreClient* discordCoreClientNew);

		virtual ~BaseFunctionArguments() noexcept = default;
	};

	/// Base class for the command classes. \brief Base class for the command classes.
	struct DiscordCoreAPI_Dll BaseFunction {
		String helpDescription{};///< Description of the command for the Help command.
		String commandName{};///< Name of the command for calling purposes.
		EmbedData helpEmbed{};///< A Message embed for displaying the command via the Help command.

		BaseFunction() noexcept = default;

		/// The base function for the command's execute function. \brief The base function for the command's execute function.
		/// \param args A reference to an instance of BaseFunctionArguments.
		virtual Void execute(BaseFunctionArguments& args) = 0;
		virtual std::unique_ptr<BaseFunction> create() = 0;
		virtual ~BaseFunction() noexcept = default;
	};

	struct DiscordCoreAPI_Dll MoveThroughMessagePagesData {
		InputEventData inputEventData{};
		Uint32 currentPageIndex{};
		String buttonId{};
	};

	DiscordCoreAPI_Dll MoveThroughMessagePagesData moveThroughMessagePages(const String& userID, InputEventData originalEvent, Uint32 currentPageIndex,
		const Vector<EmbedData>& messageEmbeds, Bool deleteAfter, Uint32 waitForMaxMs, Bool returnResult = false);

	/**@}*/

};// namespace DiscordCoreAPI

namespace DiscordCoreInternal {

	struct DiscordCoreAPI_Dll ReadyData {
		ReadyData(simdjson::ondemand::value);
		ReadyData() noexcept = default;
		DiscordCoreAPI::ApplicationData application{};
		DiscordCoreAPI::UserData user{};
		String resumeGatewayUrl{};
		String sessionId{};
		Uint32 shard[2]{};
		Int32 v{};
	};
}

template<> struct DiscordCoreAPI_Dll std::hash<DiscordCoreAPI::DiscordEntity> {
	DiscordCoreAPI::Uint64 operator()(DiscordCoreAPI::DiscordEntity const& object) const noexcept {
		return static_cast<DiscordCoreAPI::Snowflake>(object.id);
	}
};

template<> struct DiscordCoreAPI_Dll std::hash<DiscordCoreAPI::ChannelData> {
	DiscordCoreAPI::Uint64 operator()(DiscordCoreAPI::ChannelData const& object) const noexcept {
		return static_cast<DiscordCoreAPI::Snowflake>(object.id);
	}
};

template<> struct DiscordCoreAPI_Dll std::hash<DiscordCoreAPI::GuildData> {
	DiscordCoreAPI::Uint64 operator()(DiscordCoreAPI::GuildData const& object) const noexcept {
		return static_cast<DiscordCoreAPI::Snowflake>(object.id);
	}
};

template<> struct DiscordCoreAPI_Dll std::hash<DiscordCoreAPI::RoleData> {
	DiscordCoreAPI::Uint64 operator()(DiscordCoreAPI::RoleData const& object) const noexcept {
		return static_cast<DiscordCoreAPI::Snowflake>(object.id);
	}
};

template<> struct DiscordCoreAPI_Dll std::hash<DiscordCoreAPI::UserData> {
	DiscordCoreAPI::Uint64 operator()(DiscordCoreAPI::UserData const& object) const noexcept {
		return static_cast<DiscordCoreAPI::Snowflake>(object.id);
	}
};

template<> struct DiscordCoreAPI_Dll std::hash<DiscordCoreAPI::GuildMemberData> {
	DiscordCoreAPI::Uint64 operator()(DiscordCoreAPI::GuildMemberData const& object) const noexcept {
		return static_cast<DiscordCoreAPI::Snowflake>(object.guildId) ^ (static_cast<DiscordCoreAPI::Snowflake>(object.id) << 1);
	}
};

#endif