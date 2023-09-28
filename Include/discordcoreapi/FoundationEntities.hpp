/*
	MIT License

	DiscordCoreAPI, A bot library for Discord, written in C++, and featuring explicit multithreading through the usage of custom, asynchronous C++ CoRoutines.

	Copyright 2022, 2023 Chris M. (RealTimeChris)

	Permission is hereby granted, free of charge, to any person obtaining a copy
	of this software and associated documentation files (the "Software"), to deal
	in the Software without restriction, including without limitation the rights
	to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
	copies of the Software, and to permit persons to whom the Software is
	furnished to do so, subject to the following conditions:

	The above copyright notice and this permission notice shall be included in all
	copies or substantial portions of the Software.

	THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
	IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
	FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
	AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
	LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
	OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
	SOFTWARE.
*/
/// FoundationEntities.hpp - Header for all of the Discord/Support API data structures.
/// May 12, 2021 Chris M.
/// https://discordcoreapi.com
/// \file FoundationEntities.hpp
#pragma once

#include <discordcoreapi/Utilities.hpp>

namespace DiscordCoreAPI {

	namespace DiscordCoreInternal {

		struct VoiceConnectionData {
			jsonifier::string sessionId{};
			jsonifier::string endPoint{};
			jsonifier::string token{};
		};

		struct DiscordCoreAPI_Dll WebSocketResumeData {
			uint64_t lastNumberReceived{};
			jsonifier::string sessionId{};
			jsonifier::string botToken{};

			operator DiscordCoreInternal::EtfSerializer();
		};

		struct ConnectProperties {
			static constexpr jsonifier::string_view browser{ "DiscordCoreAPI" };
			static constexpr jsonifier::string_view device{ "DiscordCoreAPI" };
#if defined _WIN32
			static constexpr jsonifier::string_view os{ "Windows" };
#elif __linux__
			static constexpr jsonifier::string_view os{ "Linux" };
#else
			static constexpr jsonifier::string_view os{ "MacOS" };
#endif
		};

		struct DiscordCoreAPI_Dll WebSocketIdentifyData {
			UpdatePresenceData presence{ PresenceUpdateState::Online };
			std::array<uint64_t, 2> shard{};
			ConnectProperties properties{};
			uint64_t largeThreshold{ 250 };
			jsonifier::string botToken{};
			int64_t intents{};

			operator DiscordCoreInternal::EtfSerializer();
		};

		struct VoiceSocketProtocolPayloadDataData {
			jsonifier::string address{};
			jsonifier::string mode{};
			uint16_t port{};
		};

		struct VoiceSocketProtocolPayloadData {
			static constexpr jsonifier::string_view protocol{ "udp" };
			VoiceSocketProtocolPayloadDataData data{};
		};

		struct VoiceIdentifyData {
			jsonifier::string sessionId{};
			jsonifier::string serverId{};
			jsonifier::string token{};
			Snowflake userId{};
		};

		enum class SendSpeakingType : uint8_t {
			None			   = 0,
			Microphone		   = 1 << 0,
			Soundshare		   = 1 << 1,
			Priority		   = 1 << 2,
			Priority_And_Voice = Microphone | Priority,
		};

		struct SendSpeakingData {
			SendSpeakingType type{};
			uint64_t delay{};
			uint64_t ssrc{};
		};

		enum class HttpsWorkloadClass : uint8_t { Get = 0, Put = 1, Post = 2, Patch = 3, Delete = 4 };

		enum class PayloadType : uint8_t { Application_Json = 1, Multipart_Form = 2 };

		enum class HttpsWorkloadType : uint8_t {
			Unset											= 0,
			Get_Global_Application_Commands					= 1,
			Post_Global_Application_Command					= 2,
			Get_Global_Application_Command					= 3,
			Patch_Global_Application_Command				= 4,
			Delete_Global_Application_Command				= 5,
			Bulk_Put_Global_Application_Commands			= 6,
			Get_Guild_Application_Commands					= 7,
			Post_Guild_Application_Command					= 8,
			Get_Guild_Application_Command					= 9,
			Patch_Guild_Application_Command					= 10,
			Delete_Guild_Application_Command				= 11,
			Bulk_Put_Guild_Application_Commands				= 12,
			Get_Guild_Application_Commands_Permissions		= 13,
			Get_Guild_Application_Command_Permissions		= 14,
			Put_Guild_Application_Command_Permissions		= 15,
			Batch_Put_Guild_Application_Command_Permissions = 16,
			Post_Interaction_Response						= 17,
			Get_Interaction_Response						= 18,
			Patch_Interaction_Response						= 19,
			Delete_Interaction_Response						= 20,
			Post_Followup_Message							= 21,
			Get_Followup_Message							= 22,
			Patch_Followup_Message							= 23,
			Delete_Followup_Message							= 24,
			Get_Guild_Audit_Logs							= 25,
			Get_Channel										= 26,
			Patch_Channel									= 27,
			Delete_Channel									= 28,
			Get_Messages									= 29,
			Get_Message										= 30,
			Post_Message									= 31,
			Crosspost_Message								= 32,
			Put_Reaction									= 33,
			Delete_Own_Reaction								= 34,
			Delete_User_Reaction							= 35,
			Get_Reactions									= 36,
			Delete_All_Reactions							= 37,
			Delete_Reactions_By_Emoji						= 38,
			Patch_Message									= 39,
			Delete_Message									= 40,
			Delete_Message_Old								= 41,
			Bulk_Delete_Messages							= 42,
			Put_Channel_Permission_Overwrites				= 43,
			Get_Channel_Invites								= 44,
			Post_Channel_Invite								= 45,
			Delete_Channel_Permission_Overwrites			= 46,
			Post_Follow_News_Channel						= 47,
			Post_Trigger_Typing_Indicator					= 48,
			Get_Pinned_Messages								= 49,
			Put_Pin_Message									= 50,
			Delete_Pin_Message								= 51,
			Put_Recipient_To_Group_Dm						= 52,
			Delete_Recipient_From_Group_Dm					= 53,
			Post_Thread_With_Message						= 54,
			Post_Thread_Without_Message						= 55,
			Put_Self_In_Thread								= 56,
			Put_Thread_Member								= 57,
			Delete_Self_From_Thread							= 58,
			Delete_Thread_Member							= 59,
			Get_Thread_Member								= 60,
			Get_Thread_Members								= 61,
			Get_Active_Threads								= 62,
			Get_Public_Archived_Threads						= 63,
			Get_Private_Archived_Threads					= 64,
			Get_Joined_Private_Archived_Threads				= 65,
			Get_Emoji_List									= 66,
			Get_Guild_Emoji									= 67,
			Post_Guild_Emoji								= 68,
			Patch_Guild_Emoji								= 69,
			Delete_Guild_Emoji								= 70,
			Post_Guild										= 71,
			Get_Guild										= 72,
			Get_Guild_Preview								= 73,
			Patch_Guild										= 74,
			Delete_Guild									= 75,
			Get_Guild_Channels								= 76,
			Post_Guild_Channel								= 77,
			Patch_Guild_Channel_Positions					= 78,
			Get_Guild_Active_Threads						= 79,
			Get_Guild_Member								= 80,
			Get_Guild_Members								= 81,
			Get_Search_Guild_Members						= 82,
			Put_Guild_Member								= 83,
			Patch_Guild_Member								= 84,
			Patch_Current_Guild_Member						= 85,
			Put_Guild_Member_Role							= 86,
			Delete_Guild_Member_Role						= 87,
			Delete_Guild_Member								= 88,
			Get_Guild_Bans									= 89,
			Get_Guild_Ban									= 90,
			Put_Guild_Ban									= 91,
			Delete_Guild_Ban								= 92,
			Get_Guild_Roles									= 93,
			Post_Guild_Role									= 94,
			Patch_Guild_Role_Positions						= 95,
			Patch_Guild_Role								= 96,
			Delete_Guild_Role								= 97,
			Get_Guild_Prune_Count							= 98,
			Post_Guild_Prune								= 99,
			Get_Guild_Voice_Regions							= 100,
			Get_Guild_Invites								= 101,
			Get_Guild_Integrations							= 102,
			Delete_Guild_Integration						= 103,
			Get_Guild_Widget_Settings						= 104,
			Patch_Guild_Widget								= 105,
			Get_Guild_Widget								= 106,
			Get_Vanity_Invite								= 107,
			Get_Guild_Widget_Image							= 108,
			Get_Guild_Welcome_Screen						= 109,
			Patch_Guild_Welcome_Screen						= 110,
			Patch_Current_User_Voice_State					= 111,
			Patch_User_Voice_State							= 112,
			Get_Guild_Scheduled_Events						= 113,
			Post_Guild_Scheduled_Event						= 114,
			Get_Guild_Scheduled_Event						= 115,
			Patch_Guild_Scheduled_Event						= 116,
			Delete_Guild_Scheduled_Event					= 117,
			Get_Guild_Scheduled_Event_Users					= 118,
			Get_Guild_Template								= 119,
			Post_Guild_From_Guild_Template					= 120,
			Get_Guild_Templates								= 121,
			Post_Guild_Template								= 122,
			Put_Guild_Template								= 123,
			Patch_Guild_Template							= 124,
			Delete_Guild_Template							= 125,
			Get_Invite										= 126,
			Delete_Invite									= 127,
			Post_Stage_Instance								= 128,
			Get_Stage_Instance								= 129,
			Patch_Stage_Instance							= 130,
			Delete_Stage_Instance							= 131,
			Get_Sticker										= 132,
			Get_Nitro_Sticker_Packs							= 133,
			Get_Guild_Stickers								= 134,
			Post_Guild_Sticker								= 135,
			Patch_Guild_Sticker								= 136,
			Delete_Guild_Sticker							= 137,
			Get_Current_User								= 138,
			Get_User										= 139,
			Patch_Current_User								= 140,
			Get_Current_User_Guilds							= 141,
			Delete_Leave_Guild								= 142,
			Post_Create_User_Dm								= 143,
			Get_User_Connections							= 144,
			Get_Voice_Regions								= 145,
			Post_Webhook									= 146,
			Get_Channel_Webhooks							= 147,
			Get_Guild_Webhooks								= 148,
			Get_Webhook										= 149,
			Get_Webhook_With_Token							= 150,
			Patch_Webhook									= 151,
			Patch_Webhook_With_Token						= 152,
			Delete_Webhook									= 153,
			Delete_Webhook_With_Token						= 154,
			Post_Execute_Webhook							= 155,
			Get_Webhook_Message								= 156,
			Patch_Webhook_Message							= 157,
			Delete_Webhook_Message							= 158,
			Get_Application_Info							= 159,
			Get_Authorization_Info							= 160,
			Get_Gateway_Bot									= 161,
			Post_Thread_In_Forum_Channel					= 162,
			Get_Auto_Moderation_Rules						= 163,
			Get_Auto_Moderation_Rule						= 164,
			Post_Auto_Moderation_Rule						= 165,
			Patch_Auto_Moderation_Rule						= 166,
			Delete_Auto_Moderation_Rule						= 167,
			YouTubeGetSearchResults							= 168,
			SoundCloudGetSearchResults						= 169,
			SoundCloudGetClientId							= 170,
			SoundCloudGetDownloadLinks						= 171,
			LAST											= 172
		};

		class DiscordCoreAPI_Dll HttpsWorkloadData {
		  public:
			friend class HttpsClient;

			static UnorderedMap<HttpsWorkloadType, UniquePtr<std::atomic_int64_t>> workloadIdsExternal;
			static UnorderedMap<HttpsWorkloadType, UniquePtr<std::atomic_int64_t>> workloadIdsInternal;

			UnorderedMap<jsonifier::string, jsonifier::string> headersToInsert{};
			PayloadType payloadType{ PayloadType::Application_Json };
			std::atomic_int64_t thisWorkerId{};
			HttpsWorkloadClass workloadClass{};
			jsonifier::string relativePath{};
			jsonifier::string callStack{};
			jsonifier::string baseUrl{};
			jsonifier::string content{};

			HttpsWorkloadData() = default;

			HttpsWorkloadData& operator=(HttpsWorkloadData&& other) noexcept;
			HttpsWorkloadData(HttpsWorkloadData&& other) noexcept;

			HttpsWorkloadData& operator=(const HttpsWorkloadData& other) = delete;
			HttpsWorkloadData(const HttpsWorkloadData& other)			 = delete;

			HttpsWorkloadData& operator=(HttpsWorkloadType type);

			HttpsWorkloadData(HttpsWorkloadType type);

			HttpsWorkloadType getWorkloadType() const;

		  protected:
			static int64_t incrementAndGetWorkloadId(HttpsWorkloadType workloadType);

			HttpsWorkloadType workloadType{};
		};

		struct DiscordCoreAPI_Dll HelloData {
			HelloData() = default;
			jsonifier::vector<jsonifier::string> _trace{};
			uint64_t heartbeatInterval{};
		};

		struct WebSocketMessage {
			jsonifier::string t{};
			int64_t op{ -1 };
			int64_t s{};
		};

		template<typename ValueType> struct WebSocketMessageData {
			UnorderedSet<jsonifier::string> excludedKeys{};
			jsonifier::string t{};
			ValueType d{};
			int64_t op{};
			int64_t s{};

			inline operator DiscordCoreInternal::EtfSerializer() {
				EtfSerializer data{};
				data["op"] = op;
				data["s"]  = s;
				data["t"]  = t;
				data["d"]  = d.operator DiscordCoreInternal::EtfSerializer();
				return data;
			}
		};
	}

	template<typename ValueType, typename ValueType02> struct UpdatedEventData;
	template<typename ValueType> struct EventData;

	/**
	 * \addtogroup foundation_entities
	 * @{
	*/

	struct ConnectionPackage {
		uint64_t currentReconnectTries{};
		uint32_t currentShard{};
		bool areWeResuming{};
	};

	/// @brief Timeout durations for the timeout command.
	enum class TimeoutDurations : uint16_t {
		None		 = 0,///< None - erase timeout.
		Minute		 = 1,///< 1 Minute timeout.
		Five_Minutes = 5,///< 5 Minute timeout.
		Ten_Minutes	 = 10,///< 10 Minute timeout.
		Hour		 = 60,///< 1 Hour timeout.
		Day			 = 1440,///< 1 Day timeout.
		Week		 = 10080///< 1 Week timeout.
	};

	/// @brief RoleData tags data.
	struct RoleTagsData {
		jsonifier::string premiumSubscriber{};///< Are they a premium subscriber?
		jsonifier::string integrationId{};///< What is the integration id?
		jsonifier::string botId{};///< What is the bot id?
	};

	/// @brief UserData flags.
	enum class UserFlags : uint32_t {
		Staff					 = 1 << 0,///< Discord Employee.
		Partner					 = 1 << 1,///< Partnered Server Owner.
		Hypesquad				 = 1 << 2,///< HypeSquad Events Member.
		Bug_Hunter_Level_1		 = 1 << 3,///< Bug Hunter Level 1.
		Hypesquad_Online_House_1 = 1 << 6,///< House Bravery Member.
		Hypesquad_Online_House_2 = 1 << 7,///< House Brilliance Member.
		Hypesquad_Online_House_3 = 1 << 8,///< House Balance Member.
		Premium_Early_Suppoerter = 1 << 9,///< Early Nitro Supporter.
		Team_Pseudo_User		 = 1 << 10,///< UserData is a team.
		Bug_Hunter_Level_2		 = 1 << 14,///< Bug Hunter Level 2.
		Verified_Bot			 = 1 << 16,///< Verified Bot.
		Verified_Developer		 = 1 << 17,///< Early Verified Bot Developer.
		Certified_Moderator		 = 1 << 18,///< Discord Certified Moderator.
		Bot_Https_Interactions	 = 1 << 19,///< Bot uses only HTTPS interactions and is shown in the online member list.
		Bot						 = 1 << 20,///< Is it a bot?
		MFAEnabled				 = 1 << 21,///< Is MFA enabled?
		System					 = 1 << 22,///< Is it a system integration?
		Verified				 = 1 << 23///< Is it verified?
	};

	/// @brief Premium types denote the level of premium a user has.
	enum class PremiumType : uint8_t {
		None		  = 0,///< None.
		Nitro_Classic = 1,///< Nitro classic.
		Nitro		  = 2///< Nitro.
	};

	template<typename ValueType> class FlagEntity {
	  public:
		template<jsonifier_internal::enum_t ValueType02> inline auto setFlagValue(ValueType02 theFlagToSet, bool enabled) {
			auto newValue = static_cast<int64_t>(static_cast<ValueType*>(this)->flags);
			if (enabled) {
				newValue |= static_cast<int64_t>(theFlagToSet);
			} else {
				newValue &= ~static_cast<int64_t>(theFlagToSet);
			}
			static_cast<ValueType*>(this)->flags = static_cast<ValueType02>(newValue);
		}

		template<jsonifier_internal::enum_t ValueType02> inline bool getFlagValue(ValueType02 theFlagToCheckFor) const {
			return static_cast<int64_t>(static_cast<const ValueType*>(this)->flags) & static_cast<int64_t>(theFlagToCheckFor);
		}

	  protected:
		inline FlagEntity() = default;
	};

	enum class UserImageTypes {
		Banner			  = 0,
		Default_Avatar	  = 1,
		Avatar			  = 2,
		Avatar_Decoration = 3,
	};

	template<typename ValueType> class GetUserImageUrl {
	  public:
		inline jsonifier::string getUserImageUrl(UserImageTypes type) {
			jsonifier::string baseUrl{ "https://cdn.discordapp.com/" };
			switch (type) {
				case UserImageTypes::Banner: {
					baseUrl += "banners/" + static_cast<jsonifier::string>(static_cast<ValueType*>(this)->id) + "/" +
						static_cast<jsonifier::string>(static_cast<ValueType*>(this)->banner) + ".png";
					return baseUrl;
				}
				case UserImageTypes::Default_Avatar: {
					uint64_t index{};
					if (static_cast<jsonifier::string>(static_cast<ValueType*>(this)->discriminator) == "0") {
						index = (std::stoull(static_cast<jsonifier::string>(static_cast<ValueType*>(this)->id).data()) >> 22) % 6;
					} else {
						index = std::stoull(static_cast<jsonifier::string>(static_cast<ValueType*>(this)->discriminator).data()) % 5;
					}
					baseUrl += "embed/avatars/" + jsonifier::toString(index) + ".png";
					return baseUrl;
				}
				case UserImageTypes::Avatar: {
					baseUrl += "avatars/" + static_cast<jsonifier::string>(static_cast<ValueType*>(this)->id) + "/" +
						static_cast<jsonifier::string>(static_cast<ValueType*>(this)->avatar) + ".png";
					return baseUrl;
				}
				case UserImageTypes::Avatar_Decoration: {
					baseUrl += "avatar-decorations/" + static_cast<jsonifier::string>(static_cast<ValueType*>(this)->id) + "/" +
						static_cast<jsonifier::string>(static_cast<ValueType*>(this)->avatarDecoration) + ".png";
					return baseUrl;
				}
				default: {
					return {};
				}
			}
		}

	  protected:
		inline GetUserImageUrl() = default;
	};


	enum class GuildMemberImageTypes {
		Avatar = 0,
		Banner = 1,
	};

	template<typename ValueType> class GetGuildMemberImageUrl {
	  public:
		jsonifier::string getGuildMemberImageUrl(GuildMemberImageTypes type) {
			jsonifier::string baseUrl{ "https://cdn.discordapp.com/" };
			switch (type) {
				case GuildMemberImageTypes::Avatar: {
					baseUrl += "guilds/" + static_cast<jsonifier::string>(static_cast<ValueType*>(this)->guildId) + "/users/" +
						static_cast<jsonifier::string>(static_cast<ValueType*>(this)->user.id) + "/avatars/" +
						static_cast<jsonifier::string>(static_cast<ValueType*>(this)->avatar) + ".png";
					return baseUrl;
				}
				case GuildMemberImageTypes::Banner: {
					return baseUrl;
				}
				default: {
					return {};
				}
			}
		}

	  protected:
		inline GetGuildMemberImageUrl() = default;
	};

	enum class GuildImageTypes {
		Icon			 = 0,
		Splash			 = 1,
		Discovery_Splash = 2,
		Banner			 = 3,
	};

	template<typename ValueType> class GetGuildImageUrl {
	  public:
		jsonifier::string getGuildImageUrl(GuildImageTypes type) {
			jsonifier::string baseUrl{ "https://cdn.discordapp.com/" };
			switch (type) {
				case GuildImageTypes::Icon: {
					baseUrl += "icons/" + static_cast<jsonifier::string>(static_cast<ValueType*>(this)->id) + "/" +
						static_cast<jsonifier::string>(static_cast<ValueType*>(this)->icon) + ".png";
					return baseUrl;
				}
				case GuildImageTypes::Splash: {
					baseUrl += "splashes/" + static_cast<jsonifier::string>(static_cast<ValueType*>(this)->id) + "/" +
						static_cast<jsonifier::string>(static_cast<ValueType*>(this)->splash) + ".png";
					return baseUrl;
				}
				case GuildImageTypes::Discovery_Splash: {
					baseUrl += "icons/" + static_cast<jsonifier::string>(static_cast<ValueType*>(this)->id) + "/" +
						static_cast<jsonifier::string>(static_cast<ValueType*>(this)->discoverySplash) + ".png";
					return baseUrl;
				}
				case GuildImageTypes::Banner: {
					baseUrl += "banners/" + static_cast<jsonifier::string>(static_cast<ValueType*>(this)->id) + "/" +
						static_cast<jsonifier::string>(static_cast<ValueType*>(this)->banner) + ".png";
					return baseUrl;
				}
				default: {
					return {};
				}
			}
		}

	  protected:
		inline GetGuildImageUrl() = default;
	};

	/// @brief Attachment data.
	class AttachmentData {
	  public:
		jsonifier::string contentType{};///< Type of content for the attachment.
		jsonifier::string description{};///< A description of the attachment.
		jsonifier::string filename{};///< The file name of the attachment.
		jsonifier::string proxyUrl{};///< The proxy url for the attachment.
		jsonifier::string url{};///< The url for the attachment.
		uint64_t height{};///< The height of the attachment.
		bool ephemeral{};///< Whether it was an ephemeral response.
		uint64_t width{};///< The width of the attachment.
		uint64_t size{};///< The size of the attachment.
		Snowflake id{};///< This attachment data's id.
	};

	/// @brief StickerData format types.
	enum class StickerFormatType : uint8_t {
		Png	   = 1,///< Png.
		Apng   = 2,///< Apng.
		Lottie = 3///< Lottie
	};

	/// @brief Embed footer data.
	struct EmbedFooterData {
		jsonifier::string proxyIconUrl{};///< Proxy icon url.
		jsonifier::string iconUrl{};///< Icon url.
		jsonifier::string text{};///< Footer text.
	};

	/// @brief Embed image data.
	struct EmbedImageData {
		UnorderedSet<jsonifier::string_view> excludedKeys{};///< For excluding certain keys from parsing/serialization.
		jsonifier::string proxyUrl{};///< Proxy url.
		jsonifier::string url{};///< Image url.
		uint64_t height{};///< Image height.
		uint64_t width{};///< Image width.

		void generateExcludedKeys();
	};

	/// @brief Embed thumbnail data.
	struct EmbedThumbnailData {
		jsonifier::string proxyUrl{};///< Proxy url.
		jsonifier::string url{};///< Image url.
		uint64_t height{};///< Image height.
		uint64_t width{};///< Image width.
	};

	/// @brief Embed video data.
	struct EmbedVideoData {
		jsonifier::string proxyUrl{};///< Proxy url.
		jsonifier::string url{};///< Image url.
		uint64_t height{};///< Image height.
		uint64_t width{};///< Image width.
	};

	/// @brief Embed provider data.
	struct EmbedProviderData {
		jsonifier::string name{};///< Name.
		jsonifier::string url{};///< Url.
	};

	/// @brief Embed author data.
	struct EmbedAuthorData {
		UnorderedSet<jsonifier::string_view> excludedKeys{};///< For excluding certain keys from parsing/serialization.
		jsonifier::string proxyIconUrl{};///< Proxy icon url.
		jsonifier::string iconUrl{};///< Icon url.
		jsonifier::string name{};///< Name.
		jsonifier::string url{};///< Url.

		void generateExcludedKeys();
	};

	/// @brief Embed field data.
	struct EmbedFieldData {
		jsonifier::string value{};///< The text on the field.
		jsonifier::string name{};///< The title of the field.
		bool Inline{};///< Is the field inline with the rest of them?
	};

	/// @brief Embed types.
	enum class EmbedType : uint8_t {
		Rich	= 0,///< Rich.
		Image	= 1,///< Image.
		Video	= 2,///< Video.
		Gifv	= 3,///< Gifv.
		Article = 4,///< Article.
		link	= 5///< Link.
	};

	/// @brief Embed data.
	class DiscordCoreAPI_Dll EmbedData {
	  public:
		UnorderedSet<jsonifier::string_view> excludedKeys{};///< For excluding certain keys from parsing/serialization.
		jsonifier::vector<EmbedFieldData> fields{};///< Array of embed fields.
		jsonifier::string description{};///< Description of the embed.
		EmbedThumbnailData thumbnail{};///< Embed thumbnail data.
		EmbedProviderData provider{};///< Embed provider data.
		TimeStampParse timeStamp{};///< Timestamp to be placed on the embed.
		jsonifier::string title{};///< Title of the embed.
		uint64_t hexColorValue{};///< Hex color value of the embed.
		EmbedFooterData footer{};///< Embed footer data.
		EmbedAuthorData author{};///< Embed author data.
		jsonifier::string type{};///< Type of the embed.
		jsonifier::string url{};///< Url for the embed.
		EmbedImageData image{};///< Embed image data.
		EmbedVideoData video{};///< Embed video data.

		void generateExcludedKeys();

		/// @brief Sets the author's name and avatar for the embed.
		/// @param authorName The author's name.
		/// @param authorAvatarUrl The url to their avatar.
		EmbedData& setAuthor(jsonifier::string_view authorName, jsonifier::string_view authorAvatarUrl = "");

		/// @brief Sets the footer's values for the embed.
		/// @param footerText The footer's text.
		/// @param footerIconUrlText Url to the footer's icon.
		EmbedData& setFooter(jsonifier::string_view footerText, jsonifier::string_view footerIconUrlText = "");

		/// @brief Sets the timeStamp on the embed.
		/// @param timeStamp The timeStamp to be set.
		EmbedData& setTimeStamp(jsonifier::string_view timeStamp);

		/// @brief Adds a field to the embed.
		/// @param name The title of the embed field.
		/// @param value The contents of the embed field.
		/// @param Inline Is it inline with the rest of the fields on the embed?
		EmbedData& addField(jsonifier::string_view name, jsonifier::string_view value, bool Inline = true);

		/// @brief Sets the description (the main contents) of the embed.
		/// @param descriptionNew The contents of the description to set.
		EmbedData& setDescription(jsonifier::string_view descriptionNew);

		/// @brief Sets the color of the embed, by applying a hex-color value.
		/// @param hexColorValueNew A string containing a hex-number value (Between 0x00 0xFFFFFF).
		EmbedData& setColor(jsonifier::string_view hexColorValueNew);

		/// @brief Sets the thumbnail of the embed.
		/// @param thumbnailUrl The url to the thumbnail to be used.
		EmbedData& setThumbnail(jsonifier::string_view thumbnailUrl);

		/// @brief Sets the title of the embed.
		/// @param titleNew A string containing the desired title.
		EmbedData& setTitle(jsonifier::string_view titleNew);

		/// @brief Sets the image of the embed.
		/// @param imageUrl The url of the image to be set on the embed.
		EmbedData& setImage(jsonifier::string_view imageUrl);
	};

	/// @brief Message reference data.
	struct MessageReferenceData {
		bool failIfNotExists{};///< Fail if the Message doesn't exist?
		Snowflake messageId{};///< Snowflake of the Message to reference.
		Snowflake channelId{};///< Snowflake of the ChannelData that the referenced Message was sent in.
		Snowflake guildId{};///< Snowflake of the Guild that the referenced Message was sent in.
	};

	enum class MediaType : uint8_t { png = 0, gif = 1, jpeg = 2, mpeg = 3, mp3 = 4 };

	/// @brief Data representing a file to be sent via multipart-form data.
	struct File {
		jsonifier::string fileName{};///< The name of the file.
		jsonifier::string data{};///< The data of the file.
	};

	/// @brief The sorting order, for Guild Forum channels.
	enum class SortOrderTypes {
		Latest_Activity = 0,///< Sort forum posts by activity.
		Creation_Date	= 1///< Sort forum posts by creation time(from most recent to oldest).
	};

	/// @brief ChannelData types.
	enum class ChannelType : uint8_t {
		Guild_Text			= 0,///< A text channel within a server.
		DM					= 1,///< A direct message between users.
		Guild_Voice			= 2,///< A voice channel within a server.
		Group_DM			= 3,///< A direct message between multiple users.
		Guild_Category		= 4,///< An organizational category that contains up to 50 channels.
		Guild_Announcement	= 5,///< A channel that users can follow and crosspost into their own server (formerly news channels).
		Announcement_Thread = 10,///< A temporary sub-channel within a GUILD_ANNOUNCEMENT channel.
		Public_Thread		= 11,///< A temporary sub-channel within a GUILD_TEXT or GUILD_FORUM channel.
		Private_Thread		= 12,///< A temporary sub-channel within a GUILD_TEXT channel that is only viewable by those invited and those with the MANAGE_THREADS permission.
		Guild_Stage_Voice	= 13,///< A voice channel for hosting events with an audience.
		Guild_Directory		= 14,///< The channel in a hub containing the listed servers.
		Guild_Forum			= 15///< ChannelData that can only contain threads.
	};

	/// @brief Meta data for a ThreadData type of ChannelData.
	struct ThreadMetadataData {
		jsonifier::string archiveTimestamp{};///< (Where applicable) the time at which this ThreadData was archived.
		uint64_t autoArchiveDuration{};///< How int64_t before archiving this ThreadData.
		bool invitable{};///< The id of the individual who archived this ThreadData.
		bool archived{};///< Whether or not this ThreadData is currently archived.
		bool locked{};///< Whether or not this ThreadData is currently locked.
	};

	/// @brief Data for a single member of a ThreadData.
	class ThreadMemberData {
	  public:
		jsonifier::string joinTimestamp{};///< The time at which the member joined this ThreadData.
		Snowflake userId{};///< The UserData's id.
		uint64_t flags{};///< Flags.
		Snowflake id{};///< This threadmemberdata's id.

		inline ThreadMemberData() = default;
	};

	/// @brief ThreadData types.
	enum class ThreadType : uint8_t {
		Guild_News_Thread	 = 10,///< Guild news ThreadData.
		Guild_Public_Thread	 = 11,///< Guild public ThreadData.
		Guild_Private_Thread = 12///< Guild protected ThreadData.
	};

	/// @brief Voice state data.
	struct VoiceStateDataLight {
		Snowflake channelId{};///< The ChannelData id this UserData is connected to.
		Snowflake guildId{};///< The Guild id this voice state is for.
		Snowflake userId{};///< The UserData id this voice state is for.

		virtual inline ~VoiceStateDataLight() = default;
	};

	struct VoiceServerUpdateData {
		jsonifier::string endpoint{};
		Snowflake guildId{};
		jsonifier::string token{};
	};

	/// @brief Voice state data.
	struct VoiceStateData : public VoiceStateDataLight {
		jsonifier::string requestToSpeakTimestamp{};///< The time at which the UserData requested to speak.
		jsonifier::string sessionId{};///< The session id for this voice state.
		bool selfStream{};///< Whether this UserData is streaming using "Go Live".
		bool selfVideo{};///< Whether this UserData's camera is enabled.
		bool selfDeaf{};///< Whether this UserData is locally deafened.
		bool selfMute{};///< Whether this UserData is locally muted.
		bool suppress{};///< Whether this UserData is muted by the current UserData.
		bool deaf{};///< Whether this UserData is deafened by the server.
		bool mute{};///< Whether this UserData is muted by the server.
	};

	/// @brief Automatic ThreadData archiving durations.
	enum class ThreadAutoArchiveDuration : uint16_t {
		Shortest = 60,///< Shortest.
		Short	 = 1440,///< Short.
		Long	 = 4320,///< Long.
		Longest	 = 10080///< Longest.
	};

	enum class RoleFlags : uint8_t { Mentionable = 1 << 0, Managed = 1 << 1, Hoist = 1 << 2 };

	/// @brief A single UserData.
	class DiscordCoreAPI_Dll UserData : public GetUserImageUrl<UserData>, public FlagEntity<UserData> {
	  public:
		template<typename ValueType> friend struct jsonifier::core;
		friend class GetUserImageUrl<UserData>;

		jsonifier::string avatarDecoration{};///< The user's avatar decoration hash.
		jsonifier::string discriminator{};///< The user's Discord-tag.
		jsonifier::string globalName{};///< The user's display name, if it is set. For bots, this is the application name.
		jsonifier::string userName{};///< The user's username, not unique across the platform.
		jsonifier::string locale{};///< The user's chosen language option.
		jsonifier::string banner{};///< The user's banner hash.
		jsonifier::string avatar{};///< The user's avatar hash.
		jsonifier::string email{};///< The user's email.
		uint64_t accentColor{};///< The user's banner color encoded as an integer representation of hexadecimal color code.
		int32_t premiumType{};///< The type of Nitro subscription on a user's account.
		int32_t publicFlags{};///< The public flags on a user's account.
		bool mfaEnabled{};///< Whether the user has two-factor enabled on their account.
		UserFlags flags{};///< The flags on a user's account.
		bool verified{};///< Whether the email on this account has been verified.
		Snowflake id{};///< The user's id.
		bool system{};///< Whether the user is an Official Discord System user (part of the urgent message system).
		bool bot{};///< Whether the user belongs to an OAuth2 application.

		inline UserData() = default;

		UserData(Snowflake);
	};

	/// @brief A single UserData.
	class DiscordCoreAPI_Dll UserCacheData : public GetUserImageUrl<UserCacheData>, public FlagEntity<UserCacheData> {
	  public:
		template<typename ValueType> friend struct jsonifier::core;
		friend class GetUserImageUrl<UserCacheData>;

		jsonifier::string avatarDecoration{};///< The user's avatar decoration hash.
		jsonifier::string discriminator{};///< The user's 4-digit discord-tag identify.
		jsonifier::string globalName{};///< The user's global name.
		jsonifier::string userName{};///< The user's username.
		jsonifier::string avatar{};///< The user's avatar hash.
		jsonifier::string banner{};///< The user's banner hash.
		PremiumType premiumType{};///< The type of Nitro subscription on a user's account.
		uint64_t accentColor{};///< The user's banner color encoded as an integer representation of hexadecimal color code.
		UserFlags flags{};///< The public flags on a user's account.
		Snowflake id{};///< This user's Id.

		inline UserCacheData() = default;

		UserCacheData& operator=(UserData&& other) noexcept;
		UserCacheData(UserData&& other) noexcept;

		UserCacheData& operator=(const UserData& other);
		UserCacheData(const UserData& other);

		operator UserData();


		inline bool operator==(const Snowflake& other) const {
			return id == other;
		}
	};

	/// A single RoleData.
	class DiscordCoreAPI_Dll RoleData : public FlagEntity<RoleData> {
	  public:
		jsonifier::string unicodeEmoji{};///< Role unicode emoji. (optional)
		PermissionsParse permissions{};///< Permission bit set.
		jsonifier::string name{};///< Role name.
		jsonifier::string icon{};///< Role icon hash. (optional).
		RoleTagsData tags{};///< Role tags object. (optional)
		uint32_t position{};///< Position of this role.
		bool mentionable{};///< Whether this role is mentionable.
		RoleFlags flags{};///< Role flags combined as a bitfield.
		uint32_t color{};///< Integer representation of hexadecimal color code.
		Snowflake id{};///< Role id.
		bool managed{};///< Whether this role is managed by an integration.
		bool hoist{};///< Whether this role is pinned in the user listing.

		inline RoleData() = default;

		RoleData(Snowflake);
	};

	/// @brief Data structure representing a single RoleData.
	class DiscordCoreAPI_Dll RoleCacheData : public FlagEntity<RoleCacheData> {
	  public:
		friend class GuildData;

		jsonifier::string unicodeEmoji{};///< Emoji representing the RoleData.
		Permissions permissions{};///< The RoleData's base Guild Permissions.
		jsonifier::string name{};///< The RoleData's name.
		Snowflake guildId{};///< The id of the Guild that this RoleData is from.
		uint32_t position{};///< Its position amongst the rest of the Guild's roles.
		RoleFlags flags{};///< RoleData flags.
		uint32_t color{};///< The RoleData's color.
		Snowflake id{};///< This role's Id.

		inline RoleCacheData() = default;

		RoleCacheData& operator=(RoleData&& other) noexcept;
		RoleCacheData(RoleData&& other) noexcept;

		RoleCacheData& operator=(const RoleData&);
		RoleCacheData(const RoleData&);

		operator RoleData();

		inline bool operator==(const Snowflake& other) const {
			return id == other;
		}
	};

	/// @brief Data structure representing a single emoji.
	class DiscordCoreAPI_Dll PartialEmojiData {
	  public:
		friend class EmojiData;
		UnorderedSet<jsonifier::string> excludedKeys{};///< Excluding keys from parsing/serializing.
		jsonifier::string name{};///< What is its name?
		bool animated{};///< Is it animated?
		Snowflake id{};///< This emoji's id.

		inline PartialEmojiData() = default;

		void generateExcludedKeys();
	};

	/// @brief Data structure representing a single emoji.
	class DiscordCoreAPI_Dll EmojiData : public PartialEmojiData {
	  public:
		UnorderedSet<jsonifier::string> excludedKeys{};///< Excluding keys from parsing/serializing.
		jsonifier::vector<RoleData> roles{};///< Roles that are allowed to use this emoji.
		std::wstring unicodeName{ L"" };///< What is its unicode name?
		bool available{ true };///< Is it available to be used?
		bool requireColons{};///< Require colons to render it?
		UserData user{};///< UserData that created this emoji.
		bool managed{};///< Is it managed?

		void generateExcludedKeys();
	};

	enum class PresenceUpdateFlags : uint8_t {
		Desktop_Online = 0b00000001,
		Desktop_Idle   = 0b00000010,
		Desktop_Dnd	   = 0b000000011,
		Mobile_Online  = 0b00000010,
		Mobile_Idle	   = 0b00000100,
		Mobile_Dnd	   = 0b00000110,
		Web_Online	   = 0b00000100,
		Web_Idle	   = 0b00001000,
		Web_Dnd		   = 0b00001100,
		Status_Online  = 0b00001000,
		Status_Idle	   = 0b00010000,
		Status_Dnd	   = 0b00011000
	};

	struct ClientStatus {
		jsonifier::string desktop{};///< UserData's status set for an active desktop (Windows, Linux, Mac) application session.
		jsonifier::string mobile{};///< UserData's status set for an active mobile (iOS, Android) application session.
		jsonifier::string web{};///< UserData's status set for an active web (browser, bot account) application session.
	};

	/// @brief Presence update data.
	struct PresenceUpdateData {
		jsonifier::vector<ActivityData> activities{};///< Array of activity objects UserData's current activities.
		ClientStatus clientStatus{};///< 	UserData's platform-dependent status
		jsonifier::string status{};///< Either "idle", "dnd", "online", or "offline".
		Snowflake guildId{};///< Snowflake ID of the guild.
		UserData user{};///< UserData object UserData whose presence is being updated.
	};

	enum class GuildMemberFlags : uint8_t { Pending = 1 << 0, Deaf = 1 << 1, Mute = 1 << 2 };

	class UserIdBase {
	  public:
		Snowflake id{};

		inline UserIdBase() = default;
	};

	class GuildData;
	class GuildMemberCacheData;

	/// @brief Data structure representing a single GuildMemberData.
	class DiscordCoreAPI_Dll GuildMemberData : public FlagEntity<GuildMemberData>, public GetGuildMemberImageUrl<GuildMemberData> {
	  public:
		template<typename ValueType> friend class GetGuildMemberImageUrl;
		template<typename ValueType> friend struct jsonifier::core;
		template<typename ValueType> friend struct EventData;
		friend class GuildData;

		TimeStampParse communicationDisabledUntil{};///< ISO8601 timestamp when the user's timeout will expire.
		jsonifier::vector<Snowflake> roles{};///< Array of role object ids.
		jsonifier::string premiumSince{};///< ISO8601 timestamp when the user started boosting the guild.
		PermissionsParse permissions{};///< Total permissions of the member in the channel, including overwrites.
		jsonifier::string avatar{};///< The member's guild avatar hash.
		TimeStampParse joinedAt{};///< ISO8601 timestamp when the user joined the guild.
		jsonifier::string nick{};///< This user's guild nickname.
		GuildMemberFlags flags{};///< Guild member flags represented as a bit set, defaults to 0.
		Snowflake guildId{};///< The guild that this member belongs to.
		UserData user{};///< The user this guild member represents.
		bool pending{};///< Whether the user has not yet passed the guild's Membership Screening requirements.
		bool deaf{};///< Whether the user is deafened in voice channels.
		bool mute{};///< Whether the user is muted in voice channels.

		inline GuildMemberData() = default;

		VoiceStateDataLight getVoiceStateData();

		UserCacheData getUserData();
	};

	/// @brief Data structure representing a single GuildMemberData.
	class DiscordCoreAPI_Dll GuildMemberCacheData : public FlagEntity<GuildMemberCacheData>, public GetGuildMemberImageUrl<GuildMemberCacheData> {
	  public:
		template<typename ValueType> friend struct jsonifier::core;
		template<typename ValueType> friend struct std::hash;
		template<typename ValueType> friend struct EventData;
		friend class GuildData;

		jsonifier::vector<Snowflake> roles{};///< The Guild roGuildMemberDatales that they have.
		Permissions permissions{};///< Their base-level Permissions in the Guild.
		jsonifier::string nick{};///< Their nick/display name.
		GuildMemberFlags flags{};///< GuildMemberData flags.
		TimeStamp joinedAt{};///< When they joined the Guild.
		Snowflake guildId{};///< The current Guild's id.
		IconHash avatar{};///< This GuildMemberData's Guild Avatar.
		UserIdBase user{};///< The user id for this GuildMemberData.

		GuildMemberCacheData() = default;

		GuildMemberCacheData& operator=(GuildMemberData&& other) noexcept;
		GuildMemberCacheData(GuildMemberData&& other) noexcept;

		GuildMemberCacheData& operator=(const GuildMemberData&);
		GuildMemberCacheData(const GuildMemberData&);

		operator GuildMemberData();

		VoiceStateDataLight getVoiceStateData();

		UserCacheData getUserData();

		inline GuildMemberCacheData(uint64_t snowFlake) {
			user.id = snowFlake;
		}
	};

	/// @brief PermissionTypes overwrites types.
	enum class PermissionOverwritesType : uint8_t {
		RoleData = 0,///< RoleData.
		UserData = 1///< UserData.
	};

	/// @brief A PermissionTypes overwrite, for a given Channel.
	class OverWriteData {
	  public:
		PermissionOverwritesType type{};///< RoleData or UserData type.
		uint64_t allow{};///< Collection of Permissions to allow.
		uint64_t deny{};///< Collection of Permissions to deny.
		Snowflake id{};///< This overwrite's id.
	};

	enum class ChannelFlags : uint8_t { NSFW = 1 << 0, Managed = 1 << 1 };

	struct DefaultReactionData {
		jsonifier::string emojiName{};///< The unicode character of the emoji.
		Snowflake emojiId{};///< The id of a guild's custom emoji.
	};

	/// Forum tags.
	struct ForumTagData {
		jsonifier::string emojiName{};///< The unicode character of the emoji.
		jsonifier::string name{};///< The name of the tag(0 - 20 characters).
		Snowflake emojiId{};///< The id of a guild's custom emoji.
		bool moderated{};///< Whether this tag can only be added to or removed from threads by a member with the MANAGE_THREADS permission.
		Snowflake id{};///< Snowflake of the tag.
	};

	/// Forum layout types.
	enum class ForumLayoutTypes {
		Not_set		 = 0,///< No default has been set for forum channel.
		List_View	 = 1,///< Display posts as a list.
		Gallery_View = 2,///< Display posts as a collection of tiles.
	};

	/// @brief A ChannelData object.
	class DiscordCoreAPI_Dll ChannelData : public FlagEntity<ChannelData> {
	  public:
		jsonifier::vector<OverWriteData> permissionOverwrites{};///< Explicit permission overwrites for members and roles.
		jsonifier::vector<jsonifier::string> appliedTags{};///< The IDs of the set of tags that have been applied to a thread in a GUILD_FORUM channel.
		jsonifier::vector<ForumTagData> availableTags{};///< The set of tags that can be used in a GUILD_FORUM channel.
		DefaultReactionData defaultReactionEmoji{};///< The emoji to show in the add reaction button on a thread in a GUILD_FORUM channel.
		jsonifier::vector<UserData> recipients{};///< The recipients of the DM.
		uint32_t defaultThreadRateLimitPerUser{};/// The initial rate_limit_per_user to set on newly created threads in a channel.
		ForumLayoutTypes defaultForumLayout{};///< The default forum layout view used to display posts in GUILD_FORUM channels. Defaults to 0.
		uint32_t defaultAutoArchiveDuration{};///< Default duration, copied onto newly created threads, in minutes.
		jsonifier::string lastPinTimestamp{};///< When the last pinned message was pinned. This may be null in certain events.
		ThreadMetadataData threadMetadata{};///< Metadata in the case that this ChannelData is a ThreadData.
		SortOrderTypes defaultSortOrder{};///< The default sort order type used to order posts in GUILD_FORUM channels. Defaults to null.
		PermissionsParse permissions{};///< Computed permissions for the invoking user in the channel, including overwrites.
		jsonifier::string rtcRegion{};///< Voice region id for the voice channel, automatic when set to null.
		uint32_t totalMessageSent{};///< Number of messages ever sent in a thread, it's similar to messageCount on message creation.
		uint32_t rateLimitPerUser{};///< Amount of seconds a user has to wait before sending another message (0-21600); bots.
		uint32_t videoQualityMode{};///< The camera video quality mode of the voice channel, 1 when not present.
		jsonifier::string topic{};///< The channel topic (0-4096 characters for GUILD_FORUM channels, 0-1024 characters for all others).
		ThreadMemberData member{};///< ThreadData member object for the current user, if they have joined the thread.
		Snowflake lastMessageId{};///< The id of the last message sent in this channel (or thread for GUILD_FORUM channels).
		Snowflake applicationId{};///< Application id of the group DM creator if it is bot-created.
		jsonifier::string name{};///< The name of the channel (1-100 characters).
		jsonifier::string icon{};///< Icon hash of the group DM.
		uint32_t messageCount{};///< Number of messages (not including the initial message or deleted messages) in a thread.
		uint32_t memberCount{};///< An approximate count of users in a thread, stops counting at 50.
		ChannelFlags flags{};///< Channel flags combined as a bitfiel
		Snowflake parentId{};///< For guild channels: id of the parent category for a channel (each parent category can contain up to 50 channels).d.
		uint32_t userLimit{};///< The user limit of the voice channel.
		Snowflake ownerId{};///< Id of the creator of the group DM or thread.
		Snowflake guildId{};///< The id of the guild (may be missing for some channel objects received over gateway guild dispatches).
		uint32_t position{};///< Sorting position of the channel.
		ChannelType type{};///< The type of channel.
		uint32_t bitrate{};///< The bitrate (in bits) of the voice channel.
		Snowflake id{};///< The id of this channel.
		bool managed{};///< For group DM channels: whether the channel is managed by an application via the gdm.join OAuth2 scope.
		bool nsfw{};///< Whether the channel is nsfw.

		inline ChannelData() = default;

		ChannelData(Snowflake);

		jsonifier::string getIconUrl();
	};

	/// @brief Data structure representing a single ChannelData.
	class DiscordCoreAPI_Dll ChannelCacheData : public FlagEntity<ChannelCacheData> {
	  public:
		friend class GuildData;

		jsonifier::vector<OverWriteData> permissionOverwrites{};///< Permission overwrites.
		ChannelType type{ ChannelType::DM };///< The type of the ChannelData.
		jsonifier::string topic{};///< ChannelData topic.
		jsonifier::string name{};///< Name of the ChannelData.
		uint32_t memberCount{};///< Count of members active in the ChannelData.
		Snowflake parentId{};///< Snowflake of the ChannelData's parent ChannelData/category.
		ChannelFlags flags{};///< Flags combined as a bitmask.
		uint32_t position{};///< The position of the ChannelData, in the Guild's ChannelData list.
		Snowflake ownerId{};///< Snowflake of the ChannelData's owner.
		Snowflake guildId{};///< Snowflake of the ChannelData's Guild, if applicable.
		Snowflake id{};///< This channel's id.

		inline ChannelCacheData() = default;

		ChannelCacheData& operator=(ChannelData&& other) noexcept;
		ChannelCacheData(ChannelData&& other) noexcept;

		ChannelCacheData& operator=(const ChannelData& snowFlake);
		ChannelCacheData(const ChannelData& snowFlake);

		/// @brief For converting this into a ChannelData instance.
		operator ChannelData();

		inline bool operator==(const Snowflake& other) const {
			return id == other;
		}
	};

	/// @brief Data representing an active ThreadData.
	struct ActiveThreadsData {
		jsonifier::vector<ThreadMemberData> members{};
		jsonifier::vector<ChannelData> threads{};
		bool hasMore{};
	};

	/// @brief Data representing an archived ThreadData.
	struct ArchivedThreadsData {
		jsonifier::vector<ThreadMemberData> members{};
		jsonifier::vector<ChannelData> threads{};
		bool hasMore{};
	};

	/// @brief Application command-option types.
	enum class ApplicationCommandOptionType : uint8_t {
		Sub_Command		  = 1,///< Sub-command.
		Sub_Command_Group = 2,///< Sub-command group.
		String			  = 3,///< jsonifier::string.
		Integer			  = 4,///< Integer.
		Boolean			  = 5,///< Boolean.
		UserData		  = 6,///< UserData.
		ChannelData		  = 7,///< ChannelData.
		RoleData		  = 8,///< RoleData.
		Mentionable		  = 9,///< Mentionable.
		Number			  = 10,///< Number.
		Attachment		  = 11///< Attachment.
	};

	/// @brief Application command permission-types.
	enum class ApplicationCommandPermissionType : uint8_t {
		RoleData	= 1,///< RoleData.
		UserData	= 2,///< UserData.
		ChannelData = 3///< ChannelData.
	};

	/// @brief Event types for auto-moderation.
	enum class EventType : uint8_t {
		Message_Send = 1,///< When a member sends or edits a message in the guild.
	};

	/// @brief Trigger types for auto-moderation.
	enum class TriggerType : uint8_t {
		Keyword		   = 1,///< Check if content contains words from a user defined list of keywords.
		Spam		   = 3,///< Check if content represents generic spam.
		Keyword_Preset = 4,///< Check if content contains words from internal pre-defined wordsets.
		Mention_Spam   = 5///< Check if content contains more unique mentions than allowed.
	};

	/// @brief Keyword preset types for auto-moderation.
	enum class KeywordPresetType : uint8_t {
		Profanity	   = 1,///< Words that may be considered forms of swearing or cursing.
		Sexual_Content = 2,///< Words that refer to sexually explicit behavior or activity
		Slurs		   = 3///< Personal insults or words that may be considered hate speech.
	};

	/// @brief Action types for auto-moderation.
	enum class ActionType : uint8_t {
		Block_Message	   = 1,///< Blocks the content of a message according to the rule.
		Send_Alert_Message = 2,///< Logs user content to a specified channel.
		Timeout			   = 3///< Timeout user for a specified duration.
	};

	/// @brief Action metadata for auto-moderation-rules.
	struct ActionMetaData {
		int64_t durationSeconds{};///< Timeout duration in seconds.
		Snowflake channelId{};///< ChannelData to which user content should be logged.
	};

	/// @brief Trigger metadata for auto-moderation-rules.
	struct TriggerMetaData {
		jsonifier::vector<jsonifier::string> regexPatterns{};///< Array of strings** KEYWORD regular expression patterns which will be matched against content.
		jsonifier::vector<jsonifier::string> keywordFilter{};///< Substrings which will be searched for in content.keyword_filter	array of strings.
		jsonifier::vector<jsonifier::string> allowList{};///< Substrings which will be exempt from triggering the preset trigger type.
		jsonifier::vector<KeywordPresetType> presets{};///< The internally pre-defined wordsets which will be searched for in content.
		bool mentionRaidProtectionEnabled{};///< MENTION_SPAM whether to automatically detect mention raids.
		uint64_t mentionTotalLimit{};///< Total number of unique role and user mentions allowed per message (Maximum of 50).
	};

	/// @brief For representing a single auto-moderation-rule-action.
	struct ActionData {
		ActionMetaData metadata{};///< Additional metadata needed during execution for this specific action type.
		ActionType type{};///< The type of action.
	};

	/// @brief Represents an auto-moderation-rule.
	class AutoModerationRuleData {
	  public:
		jsonifier::vector<Snowflake> exemptChannels{};///< The channel ids that should not be affected by the rule(Maximum of 50).
		jsonifier::vector<Snowflake> exemptRoles{};///< The role ids that should not be affected by the rule(Maximum of 20).
		jsonifier::vector<ActionData> actions{};///< Actions which will execute when the rule is triggered.
		TriggerMetaData triggerMetaData{};///< The rule trigger metadata actions array of action objects the.
		TriggerType triggerType{};///< The rule trigger type.
		jsonifier::string name{};///< The rule name.
		EventType eventType{};///< The rule event type.
		Snowflake creatorId{};///< The user which first created this rule.
		Snowflake guildId{};///< The guild which this rule belongs to.
		bool enabled{};///< Whether the rule is enabled.
		Snowflake id{};

		inline AutoModerationRuleData() = default;
	};

	/// @brief Permissions data for an ApplicationCommandData.
	class ApplicationCommandPermissionData {
	  public:
		ApplicationCommandPermissionType type{ ApplicationCommandPermissionType::RoleData };///< The type of PermissionTypes.
		bool permission{};///< Whether the PermissionTypes is active or not.
		Snowflake id{};
	};

	/// @brief Represents the Permissions for accessing an ApplicationCommandData from within a Guild.
	class GuildApplicationCommandPermissionsData {
	  public:
		jsonifier::vector<ApplicationCommandPermissionData> permissions{};///< The Permissions.
		Snowflake applicationId{};///< The application's id.
		Snowflake guildId{};///< The Guild's id.
		Snowflake id{};

		inline GuildApplicationCommandPermissionsData() = default;
	};

	/// @brief For updating/modifying a given Channel's properties.
	struct UpdateChannelData {
		jsonifier::vector<OverWriteData> permissionOverwrites{};
		uint64_t defaultAutoArchiveDuration{ 10080 };
		uint64_t videoQualityMode{ 1 };
		jsonifier::string parentId{};
		jsonifier::string rtcRgion{};
		uint64_t rateLimitPerUser{};
		uint64_t bitrate{ 48000 };
		jsonifier::string topic{};
		jsonifier::string name{};
		uint64_t userLimit{};
		uint64_t position{};
		ChannelType type{};
		bool nsfw{};
	};

	/// @brief/// Data structure representing a single reaction.
	class ReactionData {
	  public:
		GuildMemberData member{};///< The GuildMemberData who placed the reaction.
		Snowflake channelId{};///< The id of the ChannelData where it was placed.
		Snowflake messageId{};///< The id of the Message upon which it was placed.
		Snowflake guildId{};///< The id of the Guild where it was placed.
		Snowflake userId{};///< The id of the UserData who placed the reaction.
		EmojiData emoji{};///< The emoji that was placed as a reaction.
		uint64_t count{};///< The number of times this particular emoji was placed as a reaction to the given MessageData.
		Snowflake id{};
		bool me{};///< Whether or not I (The bot) placed it.

		inline ReactionData() = default;
	};

	/// @brief Structure representing Voice Region Data.
	struct VoiceRegionData {
		jsonifier::string name{};///< Name of the region.
		bool deprecated{};///< Whether this is a deprecated voice region(avoid switching to these).
		Snowflake id{};///< Unique ID for the region.
		bool optimal{};///< True for a single server that is closest to the current UserData's client.
		bool custom{};///< Whether this is a custom voice region(used for events / etc).
	};

	/// @brief Message activity types.
	enum class MessageActivityType : uint8_t {
		Join		 = 1,///< Join.
		Spectate	 = 2,///< Spectate.
		Listen		 = 3,///< Listen.
		Join_Request = 5///< Join-request.
	};

	/// @brief Message activity data.
	struct MessageActivityData {
		MessageActivityType type{ MessageActivityType::Join };///< Message activity type.
		jsonifier::string partyId{};///< Party id.
	};

	/// @brief Ban data.
	struct BanData {
		bool failedDueToPerms{};///< Failed due to perms?
		jsonifier::string reason{};///< Reason for the ban.
		UserData user{};///< UserData that was banned.
	};

	/// @brief Team members object data.
	struct TeamMembersObjectData {
		jsonifier::vector<PermissionsParse> permissions{};///< Permissions for the team.
		uint64_t membershipState{};///< Current state.
		Snowflake teamId{};///< Snowflake of the current team.
		UserData user{};///< UserData data of the current UserData.
	};

	/// @brief For updating the current voice state.
	struct UpdateVoiceStateData {
		Snowflake channelId{};///< Snowflake of the desired voice ChannelData. Leave blank to disconnect.
		Snowflake guildId{};///< The id of the Guild fo which we would like to establish a voice connection.
		bool selfMute{};///< Whether or not we self-mute ourselves.
		bool selfDeaf{};///< Whether or not we self-deafen ourselves.

		operator DiscordCoreInternal::EtfSerializer();
	};

	/// @brief For updating the current voice state.
	struct UpdateVoiceStateDataDC {
		std::nullptr_t channelId{ nullptr };///< Snowflake of the desired voice ChannelData. Leave blank to disconnect.
		Snowflake guildId{};///< The id of the Guild fo which we would like to establish a voice connection.
		bool selfMute{};///< Whether or not we self-mute ourselves.
		bool selfDeaf{};///< Whether or not we self-deafen ourselves.

		operator DiscordCoreInternal::EtfSerializer();
	};

	/// @brief Team object data.
	class TeamObjectData {
	  public:
		jsonifier::vector<TeamMembersObjectData> members{};///< Array of team members object data.
		Snowflake ownerUserId{};///< UserData id of the team owner.
		jsonifier::string icon{};///< Icon for the team.
		Snowflake id{};

		inline TeamObjectData() = default;
	};

	/// Application flags, for the ApplicationData structure.
	enum class ApplicationFlags : uint32_t {
		Gateway_Presence				 = 1 << 12,///< Intent required for bots in 100 or more servers to receive presence_update events.
		Gateway_Presence_Limited		 = 1 << 13,///< Intent required for bots in under 100 servers to receive presence_update events, found in Bot Settings.
		Gateway_Guild_Members			 = 1 << 14,///< Intent required for bots in 100 or more servers to receive member-related events like guild_member_add.
		Gateway_Guild_Members_Limited	 = 1 << 15,///< Intent required for bots in under 100 servers to receive member-related events like guild_member_add, found in Bot Settings.
		Verificatino_Pending_Guild_Limit = 1 << 16,///< Indicates unusual growth of an app that prevents verification
		Embedded						 = 1 << 17,///< Indicates if an app is embedded within the Discord client (currently unavailable publicly)
		Gateway_Message_Content			 = 1 << 18,///< Intent required for bots in 100 or more servers to receive message content
		Gateway_Message_Content_Limited	 = 1 << 19///< Intent required for bots in under 100 servers to receive message content, found in Bot Settings};
	};

	/// @brief Install params data, for application data.
	struct InstallParamsData {
		jsonifier::vector<jsonifier::string> scopes{};///< The scopes to add the application to the server with.
		PermissionsParse permissions{};///< The permissions to request for the bot role.
	};

	/// @brief Application data.
	class ApplicationData {
	  public:
		jsonifier::vector<jsonifier::string> rpcOrigins{};///< Array of RPC origin strings.
		jsonifier::vector<jsonifier::string> tags{};///< Up to 5 tags describing the content and functionality of the application install_params.
		jsonifier::string termsOfServiceUrl{};///< Terms of service Url.
		jsonifier::string privacyPolicyUrl{};///< Privacy policy Url.
		jsonifier::string customInstallUrl{};
		jsonifier::string primarySkuId{};///< Primary SKU Snowflake.
		jsonifier::string description{};///< Description of the application.
		jsonifier::string coverImage{};///< The cover image.
		jsonifier::string verifyKey{};///< The verification key.
		jsonifier::string summary{};///< Summary of the application.
		InstallParamsData params{};///< Settings for the application's default in-app authorization link, if enabled.
		bool botRequireCodeGrant{};///< Does the bot require a code grant?
		jsonifier::string slug{};///< Sluhg.
		jsonifier::string name{};///< Application's name.
		jsonifier::string icon{};///< Application's icon.
		ApplicationFlags flags{};///< Application flags.
		TeamObjectData team{};///< Team object data.
		Snowflake guildId{};///< Guild id.
		bool botPublic{};///< Is the bot public?
		UserData owner{};///< Application's owner.
		Snowflake id{};

		inline ApplicationData() = default;
	};

	/// @brief Authorization info structure.
	struct AuthorizationInfoData {
		jsonifier::vector<jsonifier::string> scopes{};///< Array of strings - the scopes the UserData has authorized the application for.
		ApplicationData application{};///< Partial application object the current application.
		jsonifier::string expires{};///< When the access token expires.
		UserData user{};/// The UserData who has authorized, if the UserData has authorized with the identify scope.
	};

	/// @brief Account data.
	class AccountData {
	  public:
		jsonifier::string name{};///< Name of the account.
		Snowflake id{};

		inline AccountData() = default;
	};

	/// @brief Guild Widget Data.
	struct GuildWidgetData {
		Snowflake channelId{};///< The widget ChannelData id.
		bool enabled{};///< Whether the widget is enabled.
	};

	/// @brief Get Guild Widget Data.
	struct GetGuildWidgetObjectData {
		jsonifier::vector<ChannelData> channels{};///< Voice and stage channels which are accessible by everyone.
		jsonifier::vector<UserData> members{};///< Special widget user objects that includes users presence (Limit 100).
		jsonifier::string instantInvite{};///< Instant invite for the guilds specified widget invite channel.
		uint64_t presenceCount{};///< Number of online members in this guild.
		jsonifier::string name{};///< Guild name (2-100 characters).
		Snowflake id{};
	};

	/// @brief Widget style options.
	enum class WidgetStyleOptions : uint8_t {
		Shield	= 0,///< Shield
		Banner1 = 1,///< Banner1
		Banner2 = 2,///< Banner2
		Banner3 = 3,///< Banner3
		Banner4 = 4///< Banner4
	};

	/// @brief Guild widget image data.
	struct GuildWidgetImageData {
		jsonifier::string url{};
	};

	/// @brief Integration data.
	class IntegrationData {
	  public:
		ApplicationData application{};///< Application data.
		jsonifier::string syncedAt{};///< Time it was last synced at.
		bool enableEmoticons{ true };///< Emoticons enabled?
		uint64_t expireGracePeriod{};///< How int64_t before the integration expires.
		uint64_t subscriberCount{};///< Number of current subscribers.
		uint64_t expireBehavior{};///< What to do upon expiry.
		jsonifier::string name{};///< Name of the integration.
		jsonifier::string type{};///< Type of integration.
		AccountData account{};///< Account data.
		Snowflake roleId{};///< RoleData Snowflake.
		UserData user{};///< UserData data for the integration.
		bool revoked{};///< Has it been revoked?
		bool enabled{};///< Enabled?
		bool syncing{};///< Is it syncing?
		Snowflake id{};

		inline IntegrationData() = default;
	};

	/// @brief Audit log events.
	enum class AuditLogEvent : uint8_t {
		Guild_Update						  = 1,///< Guild update.
		Channel_Create						  = 10,///< ChannelData create.
		Channel_Update						  = 11,///< ChannelData update.
		Channel_Delete						  = 12,///< ChannelData delete.
		Channel_Overwrite_Create			  = 13,///< ChannelData overwrite create.
		Channel_Overwrite_Update			  = 14,///< ChannelData overwrite update.
		Channel_Overwrite_Delete			  = 15,///< ChannelData overwrite delete.
		Member_Kick							  = 20,///< Member kick.
		Member_Prune						  = 21,///< Member prune.
		Member_Ban_Add						  = 22,///< Member ban add.
		Member_Ban_Remove					  = 23,///< Member ban erase.
		Member_Update						  = 24,///< Member update.
		Member_Role_Update					  = 25,///< Member role update.
		Member_Move							  = 26,///< Member move.
		Member_Disconnect					  = 27,///< Member disconnect.
		Bot_Add								  = 28,///< Bot add.
		Role_Create							  = 30,///< RoleData create.
		Role_Update							  = 31,///< RoleData update.
		Role_Delete							  = 32,///< RoleData delete.
		Invite_Create						  = 40,///< Invite create.
		Invite_Update						  = 41,///< Invite update.
		Invite_Delete						  = 42,///< Invite delete.
		Webhook_Create						  = 50,///< Webhook create.
		Webhook_Update						  = 51,///< Webhook update.
		Webhook_Delete						  = 52,///< Webhook delete.
		Emoji_Create						  = 60,///< Emoji create.
		Emoji_Update						  = 61,///< Emoji update.
		Emoji_Delete						  = 62,///< Emoji delete.
		Message_Delete						  = 72,///< Message delete.
		Message_Bulk_Delete					  = 73,///< Message bulk delete.
		Message_Pin							  = 74,///< Message pin.
		Message_Unpin						  = 75,///< Message unpin.
		Integration_Create					  = 80,///< Integration create.
		Integration_Update					  = 81,///< Integration update.
		Integration_Delete					  = 82,///< Integration delete.
		Stage_Instance_Create				  = 83,///< Stage-Instance create.
		Stage_Instance_Update				  = 84,///< Stage-Instance update.
		Stage_Instance_Delete				  = 85,///< Stage-Instance delete.
		Sticker_Create						  = 90,///< StickerData create.
		Sticker_Update						  = 91,///< StickerData update.
		Sticker_Delete						  = 92,///< StickerData delete.
		Guild_Scheduled_Event_Create		  = 100,///< Guild-scheduled-event create.
		Guild_Scheduled_Event_Update		  = 101,///< Guild-scheduled-event update.
		Guild_Scheduled_Event_Delete		  = 102,///< Guild-scheduled-event delete.
		Thread_Create						  = 110,///< ThreadData create.
		Thread_Update						  = 111,///< ThreadData update.
		Thread_Delete						  = 112,///< ThreadData delete.
		Application_Command_Permission_Update = 121,///< Permissions were updated for a command.
		Auto_Moderation_Rule_Create			  = 140,///< Auto Moderation rule was created.
		Auto_Moderation_Rule_Update			  = 141,///< Auto Moderation rule was updated.
		Auto_Moderation_Rule_Delete			  = 142,///< Auto Moderation rule was deleted.
		Auto_Moderation_Block_Message		  = 143///< Message was blocked by AutoMod (according to a rule).
	};

	/// Audit log entry info data @brief Audit log entry info data.
	class OptionalAuditEntryInfoData {
	  public:
		jsonifier::string deleteMemberDays{};///< Number of days for which the member's Messages were deleted.
		jsonifier::string membersRemoved{};///< Number of members that were removed upon a prune.
		jsonifier::string roleName{};///< RoleData name.
		Snowflake applicationId{};///< ID of the app whose permissions were targeted APPLICATION_COMMAND_PERMISSION_UPDATE.
		jsonifier::string count{};///< Count.
		jsonifier::string type{};///< Type.
		Snowflake channelId{};///< ChannelData Snowflake.
		Snowflake messageId{};///< Message Snowflake.
		Snowflake id{};

		inline OptionalAuditEntryInfoData() = default;
	};

	/// @brief Audit log change data.
	struct AuditLogChangeData {
		jsonifier::string newValue{};///< New value.
		jsonifier::string oldValue{};///< Old value.
		jsonifier::string key{};///< The key of the audit log change.
	};

	/// @brief Guild prune count data.
	struct GuildPruneCountData {
		uint64_t count{};
	};

	/// @brief Audit log entry data.
	class AuditLogEntryData {
	  public:
		jsonifier::vector<AuditLogChangeData> changes{};///< Array of audit log change data.
		OptionalAuditEntryInfoData options{};///< Audit log entry info data.
		TimeStampParse createdTimeStamp{};///< Time at which this entry was created.
		AuditLogEvent actionType{};///< Audit log action type.
		jsonifier::string reason{};///< The reason that was entered for the given change.
		Snowflake targetId{};///< Snowflake of the target UserData.
		Snowflake userId{};///< Snowflake of the executing UserData.
		Snowflake id{};
	};

	/// @brief Premium tier levels.
	enum class PremiumTier : uint8_t {
		None   = 0,///< None.
		Tier_1 = 1,///< Tier 1.
		Tier_2 = 2,///< Tier 2.
		Tier_3 = 3///< Tier 3.
	};

	/// @brief Default Message notification
	/// levels.
	enum class DefaultMessageNotificationLevel : uint8_t {
		All_Messages  = 0,///< All messages.
		Only_Mentions = 1///< Only mentions.
	};

	/// @brief Explicit content filter levels.
	enum class ExplicitContentFilterLevel : uint8_t {
		Disabled			  = 0,///< Disabled.
		Members_Without_Roles = 1,///< Members without roles.
		All_Members			  = 2///< All members.
	};

	/// @brief MFA levels.
	enum class MFALevel : uint8_t {
		None	 = 0,///< None.
		Elevated = 1///< Elevated.
	};

	/// @brief/// Verification levels.
	enum class VerificationLevel : uint8_t {
		None	  = 0,///< None.
		Low		  = 1,///< Low.
		Medium	  = 2,///< Medium.
		High	  = 3,///< High.
		Very_High = 4///< Very high.
	};

	/// @brief Welcome screen ChannelData data.
	struct WelcomeScreenChannelData {
		jsonifier::string description{};///< Description of the welcome ChannelData.
		jsonifier::string emojiName{};///< Emoji name for the ChannelData.
		Snowflake channelId{};///< Snowflake of the welcome ChannelData.
		Snowflake emojiId{};///< Emoji id for the ChannelData.
	};

	/// @brief Welcome screen data.
	struct WelcomeScreenData {
		jsonifier::vector<WelcomeScreenChannelData> welcomeChannels{};///< Welcome screen ChannelData data.
		jsonifier::string description{};///< Description of the welcome screen.
	};

	/// @brief Stage instance privacy levels.
	enum class StageInstancePrivacyLevel : uint8_t {
		Public	   = 1,///< Public.
		Guild_Only = 2///< Guild only.
	};

	/// @brief Stage instance data.
	class StageInstanceData {
	  public:
		StageInstancePrivacyLevel privacyLevel{};///< Privacy level of the ChannelData.
		bool discoverableDisabled{};///< Is it discoverable?
		jsonifier::string topic{};///< The topic of the StageInstanceData.
		Snowflake channelId{};///< The ChannelData's id.
		Snowflake guildId{};///< The Guild id for which the ChannelData exists in.
		Snowflake id{};

		inline StageInstanceData() = default;
	};

	/// @brief StickerData types.
	enum class StickerType : uint8_t {
		Standard = 1,///< Standard.
		Guild	 = 2///< Guild.
	};

	enum class StickerFlags : uint8_t { Available = 1 << 0 };

	/// @brief Data representing a single StickerData.
	class StickerData {
	  public:
		jsonifier::string description{};///< Description of the StickerData.
		StickerFormatType formatType{};///< Format type.
		jsonifier::string packId{};///< Pack id of the StickerData.
		jsonifier::string asset{};///< Asset value for the StickerData
		jsonifier::string name{};///< The StickerData's name.
		jsonifier::string tags{};///< Tags for the StickerData to use.
		StickerFlags flags{};///< StickerData flags.
		uint64_t nsfwLevel{};///< NSFW warning level.
		uint64_t sortValue{};///< Where in the stack of stickers it resides.
		Snowflake guildId{};///< The Guild id for which the StickerData exists in.
		StickerType type{};///< The type of StickerData.
		UserData user{};///< The UserData that uploaded the Guild StickerData.
		Snowflake id{};

		inline StickerData() = default;
	};

	/// @brief Data representing a single Guild preview.
	struct GuildPreviewData {
		jsonifier::vector<jsonifier::string> features{};
		jsonifier::vector<StickerData> stickers{};
		jsonifier::vector<EmojiData> emojis{};
		jsonifier::string discoverySplash{};
		uint64_t approximatePresenceCount{};
		uint64_t approximateMemberCount{};
		jsonifier::string description{};
		jsonifier::string splash{};
		jsonifier::string name{};
		jsonifier::string icon{};
		Snowflake id{};
	};

	/// @brief Afk timeout durations.
	enum class AfkTimeOutDurations : uint16_t {
		Shortest = 60,///< Shortest.
		Short	 = 300,///< Short.
		Medium	 = 900,///< Medium.
		Long	 = 1800,///< Long.
		Longest	 = 3600///< Longest.
	};

	/// @brief Guild NSFW level.
	enum class GuildNSFWLevel : uint8_t {
		Default		   = 0,///< Default.
		Explicit	   = 1,///< Explicit.
		Safe		   = 2,///< Safe.
		Age_Restricted = 3///< Age restricted.
	};

	/// @brief System channel flags.
	enum class SystemChannelFlags : uint8_t {
		Suppress_Join_Notifications			  = 1 << 0,///< Suppress member join notifications.
		Suppress_Premium_Subscriptions		  = 1 << 1,///< Suppress server boost notifications.
		Suppress_Guild_Reminder_Notifications = 1 << 2,///< Suppress server setup tips.
		Suppress_Join_Notification_Replies	  = 1 << 3///< Hide member join sticker reply buttons.
	};

	class GuildCacheData;
	class Guild;

	/// @brief Guild scheduled event privacy levels.
	enum class GuildScheduledEventPrivacyLevel : uint8_t {
		Public	   = 1,///< Public.
		Guild_Only = 2///< Guild only.
	};

	/// @brief GuildScheduledEventStatus.
	enum class GuildScheduledEventStatus : uint8_t {
		Scheduled = 1,///< Scheduled.
		Active	  = 2,///< Active.
		Completed = 3,///< Completed.
		Canceled  = 4///< Cancelled.
	};

	/// @brief Guild scheduled event entity types.
	enum class GuildScheduledEventEntityType : uint8_t {
		None		   = 0,///< None.
		State_Instance = 1,///< Stage instance.
		Voice		   = 2,///< Voice.
		External	   = 3///< External.
	};

	/// @brief Guild scheduled event entity metadata.
	struct GuildScheduledEventMetadata {
		jsonifier::string location{};
	};

	/// @brief Data representing a Guild Scheduled Event.
	class GuildScheduledEventData {
	  public:
		GuildScheduledEventPrivacyLevel privacyLevel{};///< The privacy level of the scheduled event.
		GuildScheduledEventMetadata entityMetadata{};///< Additional metadata for the Guild scheduled event.
		GuildScheduledEventEntityType entityType{};///< The type of the scheduled event.
		jsonifier::string scheduledStartTime{};///< The time the scheduled event will start.
		jsonifier::string scheduledEndTime{};///< The time the scheduled event will end, required if entity_type is External.
		GuildScheduledEventStatus status{};///< The status of the scheduled event.
		jsonifier::string description{};///< The description of the scheduled event(1 - 1000 characters.
		jsonifier::string name{};///< The name of the scheduled event(1 - 100 characters).
		Snowflake creatorId{};///< The id of the UserData that created the scheduled event *.
		Snowflake channelId{};///< The ChannelData id in which the scheduled event will be hosted, or null if scheduled entity type is External.
		Snowflake entityId{};///< The id of an entity associated with a Guild scheduled event.
		uint32_t userCount{};///< The number of users subscribed to the scheduled event.
		Snowflake guildId{};///< The Guild id which the scheduled event belongs to.
		UserData creator{};///< The UserData that created the scheduled event.
		Snowflake id{};

		inline GuildScheduledEventData() = default;
	};

	/// @brief Data representing a single GuildScheduledEventUser.
	struct GuildScheduledEventUserData {
		Snowflake guildScheduledEventId{};///< The scheduled event id which the UserData subscribed to/
		GuildMemberData member{};///< Guild member data for this UserData for the Guild which this event belongs to, if any.
		UserData user{};///< UserData which subscribed to an event.
	};

	/// @brief Invite target types.
	enum class InviteTargetTypes : uint8_t {
		Stream				 = 1,///< Stream.
		Embedded_Application = 2///< Embedded application.
	};

	/// @brief WebHookData types.
	enum class WebHookType : uint8_t {
		Incoming		 = 1,///< Incoming.
		Channel_Follower = 2,///< ChannelData follower.
		Application		 = 3///< Application.
	};

	/// @brief For removing a reaction.
	struct ReactionRemoveData {
		Snowflake channelId{};
		Snowflake messageId{};
		Snowflake guildId{};
		Snowflake userId{};
		EmojiData emoji{};
	};

	/// @brief For storing Interaction-related values.
	struct InteractionPackageData {
		jsonifier::string interactionToken{};
		Snowflake applicationId{};
		Snowflake interactionId{};
	};

	/// @brief For storing MessageData-related values.
	struct MessagePackageData {
		Snowflake channelId{};
		Snowflake messageId{};
	};

	/// @brief Data structure representing an ApplicationCommandData's option choice.
	class ApplicationCommandOptionChoiceData {
	  public:
		UnorderedMap<jsonifier::string, jsonifier::string> nameLocalizations{};///< Dictionary with keys in available locales Localization dictionary for the name.
		UnorderedSet<jsonifier::string> excludedKeys{};
		jsonifier::raw_json_data value{};///< The value of the option.
		jsonifier::string name{};///< The name of the current choice.

		void generateExcludedKeys();

		bool operator==(const ApplicationCommandOptionChoiceData&) const;
	};

	/// @brief Data structure representing an ApplicationCommandData's option.
	struct ApplicationCommandOptionData {
		UnorderedMap<jsonifier::string, jsonifier::string> descriptionLocalizations{};///< Dictionary for the description localizations field.
		UnorderedMap<jsonifier::string, jsonifier::string> nameLocalizations{};///< Dictionary for the name localizations field.
		jsonifier::vector<ApplicationCommandOptionChoiceData> choices{};///< A vector of possible choices for the current ApplicationCommandData option.
		jsonifier::vector<ApplicationCommandOptionData> options{};///< A vector of possible options for the current ApplicationCommandData option.
		int64_t minValue{ std::numeric_limits<int64_t>::max() };///< If the option is an INTEGER or NUMBER type, the minimum value permitted.
		int64_t maxValue{ std::numeric_limits<int64_t>::min() };///< If the option is an INTEGER or NUMBER type, the maximum value permitted.
		UnorderedSet<jsonifier::string> excludedKeys{};
		jsonifier::vector<ChannelType> channelTypes{};///< Set when the ApplicationCommandData option type is set to ChannelData.
		ApplicationCommandOptionType type{};///< The type of command option.
		jsonifier::string description{};///< A description of the current ApplicationCommandData option.
		jsonifier::string name{};///< Name of the current ApplicationCommandData option.
		bool autocomplete{};///< If autocomplete interactions are enabled for this STRING, INTEGER, or NUMBER type option.
		bool required{};///< If the parameter is required or optional-- default false.

		void generateExcludedKeys();

		bool operator==(const ApplicationCommandOptionData&) const;
	};

	/// @brief Representing "TypingStart" data.
	struct TypingStartData {
		TimeStampParse timeStamp{};
		GuildMemberData member{};
		Snowflake channelId{};
		Snowflake guildId{};
		Snowflake userId{};
	};

	struct YouTubeFormat {
		jsonifier::string signatureCipher{};
		jsonifier::string audioSampleRate{};
		jsonifier::string audioQuality{};
		jsonifier::string downloadUrl{};
		jsonifier::string signature{};
		jsonifier::string mimeType{};
		jsonifier::string quality{};
		jsonifier::string codecs{};
		jsonifier::string aitags{};
		uint64_t averageBitrate{};
		int64_t contentLength{};
		uint64_t bitrate{};
		uint64_t height{};
		uint64_t width{};
		uint64_t itag{};
		uint64_t fps{};
	};

	/// @brief Application command types.
	enum class ApplicationCommandType : uint8_t {
		Chat_Input = 1,///< Chat input.
		UserData   = 2,///< UserData.
		Message	   = 3///< Message.
	};

	/// @brief Component types.
	enum class ComponentType : uint8_t {
		Action_Row		   = 1,///< Container for other components.
		Button			   = 2,///< Button object.
		String_Select	   = 3,///< Select menu for picking from defined text options.
		Text_Input		   = 4,///< Text input object.
		User_Select		   = 5,///< Select menu for users.
		Role_Select		   = 6,///< Select menu for roles.
		Mentionable_Select = 7,///< Select menu for mentionables (users and roles).
		Channel_Select	   = 8,///< Select menu for channels.
	};

	enum class SelectMenuType : uint8_t { String_Select = 3, User_Select = 5, Role_Select = 6, Mentionable_Select = 7, Channel_Select = 8 };

	enum class SelectDefaultValueType { User = 0, Role = 1, Channel = 2 };

	class SelectDefaultValueData {
	  public:
		inline SelectDefaultValueData() = default;

		inline SelectDefaultValueData(SelectDefaultValueType typeNew) {
			switch (typeNew) {
				case SelectDefaultValueType::User: {
					type = "user";
					break;
				}
				case SelectDefaultValueType::Role: {
					type = "role";
					break;
				}
				case SelectDefaultValueType::Channel: {
					type = "channel";
					break;
				}
			}
		}
		jsonifier::string type{};
		Snowflake id{};
	};

	/// @brief Allowable mentions for a Message.
	class AllowedMentionsData {
	  public:
		jsonifier::vector<jsonifier::string> parse{};///< A vector of allowed mention types to parse from the content.
		jsonifier::vector<jsonifier::string> roles{};///< Array of role_ids to mention (Max size of 100)
		jsonifier::vector<jsonifier::string> users{};///< Array of user_ids to mention (Max size of 100)
		bool repliedUser{};///< For replies, whether to mention the author of the Message being replied to (default false).
	};

	/// @brief Interaction types.
	enum class InteractionType : uint8_t {
		Ping							 = 1,///< Ping.
		Application_Command				 = 2,///< Application command.
		Message_Component				 = 3,///< Message component.
		Application_Command_Autocomplete = 4,///< Application command autocomplete.
		Modal_Submit					 = 5///< Modal submission.
	};

	/// @brief Represents a single selection from a select-menu.
	class SelectOptionData {
	  public:
		jsonifier::string description{};///< Description of the select-menu-option.
		jsonifier::string label{};///< A visible label for the select-menu-option.
		jsonifier::string value{};///< A value for identifying the option.
		PartialEmojiData emoji{};///< An optional emoji to put on it.
		bool _default{};///< Is it the default option?
	};

	/// @brief Button styles.
	enum class ButtonStyle : uint8_t {
		Primary	  = 1,///< Primary.
		Success	  = 3,///< Success.
		Secondary = 2,///< Secondary.
		Danger	  = 4,///< Danger.
		Link	  = 5///< Link.
	};

	struct DiscordCoreAPI_Dll ComponentData {
		jsonifier::vector<SelectDefaultValueData> defaultValues{};
		UnorderedSet<jsonifier::string> excludedKeys{};
		jsonifier::vector<SelectOptionData> options{};
		jsonifier::vector<ChannelType> channelTypes{};
		jsonifier::string placeholder{};
		jsonifier::string customId{};
		jsonifier::string label{};
		jsonifier::string value{};
		jsonifier::string title{};
		PartialEmojiData emoji{};
		jsonifier::string url{};
		ComponentType type{};
		uint64_t minValues{};
		uint64_t maxValues{};
		uint64_t minLength{};
		uint64_t maxLength{};
		uint64_t style{};
		bool disabled{};
		bool required{};

		void generateExcludedKeys();
	};

	/// @brief Action row data of Message components.
	class ActionRowData {
	  public:
		ComponentType type{ ComponentType::Action_Row };
		jsonifier::vector<ComponentData> components{};///< Array of components to make up the action-row.
	};

	/// @brief Interaction callback types.
	enum class InteractionCallbackType : uint8_t {
		Pong									= 1,///< ACK a Ping.
		Channel_Message_With_Source				= 4,///< Respond to an interaction with a message.
		Deferred_Channel_Message_With_Source	= 5,///< ACK an interaction and edit a response later, the user sees a loading state.
		Deferred_Update_Message					= 6,///< For components, ACK an interaction and edit the original message later; the user does not see a loading state.
		Update_Message							= 7,///< For components, edit the message the component was attached to.
		Application_Command_Autocomplete_Result = 8,///< Respond to an autocomplete interaction with suggested choices.
		Modal									= 9///< Respond to an interaction with a popup modal.
	};

	/// @brief Interaction ApplicationCommandData callback data.
	struct DiscordCoreAPI_Dll InteractionCallbackData {
		jsonifier::vector<ApplicationCommandOptionChoiceData> choices{};///< Autocomplete choices(max of 25 choices).
		jsonifier::vector<AttachmentData> attachments{};///< Array of partial attachment objects attachment objects with filename and description.
		UnorderedSet<jsonifier::string> excludedKeys{};///< Keys to exclude from parsing/serializing.
		jsonifier::vector<ActionRowData> components{};///< Message components.
		AllowedMentionsData allowedMentions{};///< Allowed mentions data.
		jsonifier::vector<EmbedData> embeds{};///< Message embeds.
		jsonifier::vector<File> files{};///< Files for uploading.
		jsonifier::string customId{};///< A developer-defined identifier for the component, max 100 characters.
		jsonifier::string content{};///< Message content.
		jsonifier::string title{};///< The title of the popup modal.
		uint64_t flags{};///< Flags.
		bool tts{};///< Is it TTS?

		void generateExcludedKeys();
	};

	struct CreateApplicationCommandData;

	/// @brief Data structure representing an ApplicationCommandData.
	class ApplicationCommandData {
	  public:
		UnorderedMap<jsonifier::string, jsonifier::string> descriptionLocalizations{};///< Dictionary with keys in available locales.
		UnorderedMap<jsonifier::string, jsonifier::string> nameLocalizations{};///< Dictionary with keys in available locales.
		jsonifier::vector<ApplicationCommandOptionData> options{};///< A vector of possible options for the current ApplicationCommandData.
		UnorderedSet<jsonifier::string> excludedKeys{};///< Keys to exclude at serialization time.
		Permission defaultMemberPermissions{};///< Set of permissions represented as a bit set all
		jsonifier::string description{};///< A description of the current ApplicationCommandData.
		ApplicationCommandType type{};///< The type of ApplicationCommandData.
		jsonifier::string version{};///< An autoincremented version.
		Snowflake applicationId{};///< The current application id.
		jsonifier::string name{};///< Name of the current ApplicationCommandData.
		bool dmPermission{};///< Indicates whether the command is available in DMs with the app, only for globally - scoped commands.
		Snowflake guildId{};///< (Where applicable) a Guild id for which guild to assign this ApplicationCommandData to.
		Snowflake id{};

		inline ApplicationCommandData() = default;

		void generateExcludedKeys();

		bool operator==(const ApplicationCommandData&) const;
	};

	/// @brief ChannelData mention data.
	class ChannelMentionData {
	  public:
		Snowflake guildId{};///< The id of the Guild where it took place.
		jsonifier::string name{};///< The name of the ChannelData that was mentioned.
		ChannelType type{};///< The type of ChannelData that was mentioned.
		Snowflake id{};
	};

	/// @brief Data for when some ChannelData pins are updated.
	struct ChannelPinsUpdateEventData {
		TimeStampParse lastPinTimeStamp{};///< The time of the last pinned Message.
		Snowflake channelId{};///< The id of the ChannelData within which the Message was pinned.
		Snowflake guildId{};///< The id of the Guild within which the Message was pinned.
	};

	/// @brief Data for when threads are synced.
	struct ThreadListSyncData {
		jsonifier::vector<jsonifier::string> channelIds{};///< The parent ChannelData ids whose threads are being synced.
		jsonifier::vector<ThreadMemberData> members{};///< Array of members that are a part of the ThreadData.
		jsonifier::vector<ChannelData> threads{};///< All active threads in the given channels that the current UserData can access.
		Snowflake guildId{};///< The id of the Guild for which the threads are being synced.
	};

	/// @brief Represents a ThreadData-members-update.
	class ThreadMembersUpdateData {
	  public:
		jsonifier::vector<jsonifier::string> removedMemberIds{};///< Members who have been removed.
		jsonifier::vector<ThreadMemberData> addedMembers{};///< New members added to the ThreadData.
		uint32_t memberCount{};///< Number of Guild-members in the ThreadData.
		Snowflake guildId{};///< Guild id of the ThreadData.
		Snowflake id{};

		inline ThreadMembersUpdateData() = default;
	};

	/// @brief Message Interaction data.
	class MessageInteractionData {
	  public:
		GuildMemberData member{};
		jsonifier::string name{};
		InteractionType type{};
		UserData user{};
		Snowflake id{};

		inline MessageInteractionData() = default;
	};

	/// @brief Message types.
	enum class MessageType : uint8_t {
		Default										 = 0,///< Default.
		Recipient_Add								 = 1,///< Recipient add.
		Recipient_Remove							 = 2,///< Recipient erase.
		Call										 = 3,///< Call.
		Channel_Name_Change							 = 4,///< ChannelData name change.
		Channel_Icon_Change							 = 5,///< ChannelData icon change.
		Channel_Pinned_Message						 = 6,///< ChannelData pinned Message.
		Guild_Member_Join							 = 7,///< Guild memeber join.
		User_Premium_Guild_Subscription				 = 8,///< UserData premium Guild subscription.
		User_Premium_Guild_Subscription_Tier_1		 = 9,///< UserData premium Guild subscription tier 1.
		User_Premium_Guild_Subscription_Tier_2		 = 10,///< UserData premium Guild subscription tier 2.
		User_Premium_Guild_Subscription_Tier_3		 = 11,///< UserData premium Guild subscription tier 3.
		Channel_Follow_Add							 = 12,///< ChannelData follow add.
		Guild_Discovery_Disqualified				 = 14,///< Guild discovery disqualified.
		Guild_Discovery_Requalified					 = 15,///< Guild discovery requalified.
		Guild_Discovery_Grace_Period_Initial_Warning = 16,///< Guild discovery grade period initial warning.
		Guild_Discovery_Grace_Period_Final_Warning	 = 17,///< Guild discovery grade period final warning.
		Thread_Created								 = 18,///< ThreadData created.
		Reply										 = 19,///< Reply.
		Chat_Input_Command							 = 20,///< Chat input command.
		Thread_Starter_Message						 = 21,///< ThreadData starter MessageData.
		Guild_Invite_Reminder						 = 22,///< Guild invite reminder.
		Context_Menu_Command						 = 23///< Context menu command.
	};

	/// @brief Message flags.
	enum class MessageFlags : uint8_t {
		Crossposted			   = 1 << 0,///< Crossposted.
		Is_Crosspost		   = 1 << 1,///< Is crosspost.
		Suppress_Embeds		   = 1 << 2,///< Supress embeds.
		Source_Message_Deleted = 1 << 3,///< Source Message deleted.
		Urgent				   = 1 << 4,///< Urgent.
		Has_Thread			   = 1 << 5,///< Has ThreadData.
		Ephemeral			   = 1 << 6,///< Ephemeral.
		Loading				   = 1 << 7///< Loading.
	};

	/// @brief StickerData item types.
	enum class StickerItemType : uint8_t {
		Png	   = 1,///< Png.
		Apng   = 2,///< Apng.
		Lottie = 3///< Lottie.
	};

	/// @brief Represents a forum thread message.
	struct ForumThreadMessageData {
		jsonifier::vector<jsonifier::string> stickerIds{};///< Array of snowflakes IDs of up to 3 stickers in the server to send in the message.
		jsonifier::vector<AttachmentData> attachments{};///< Array of partial attachment objects attachment objects with filename.
		jsonifier::vector<ActionRowData> components{};///< Array of message component objects the components to include with the message.
		AllowedMentionsData allowedMentions{};///< Allowed mention object allowed mentions for the message.
		jsonifier::vector<EmbedData> embeds{};///< Array of embed objects	embedded rich content (up to 6000 characters).
		jsonifier::vector<File> files{};///< File contents the contents of the file being sent one of content, file, embed(s), sticker_ids.
		jsonifier::string content{};///< The message contents (up to 2000 characters).
		uint64_t flags{};///< Flags to be set for the message.
	};

	/// @brief Message StickerData item data.
	class StickerItemData {
	  public:
		StickerItemType formatType{};///< Message StickerData item type.
		jsonifier::string name{};///< The name of the StickerData.
		Snowflake id{};
	};

	/// @brief The core of a Message's data structure.
	class MessageDataOld {
	  public:
		jsonifier::vector<ChannelMentionData> mentionChannels{};///< array of ChannelData mention data.
		jsonifier::vector<jsonifier::string> mentionRoles{};///< jsonifier::vector of "mention roles" ids.
		jsonifier::vector<StickerItemData> stickerItems{};///< Array of Message StickerData item data.
		jsonifier::vector<AttachmentData> attachments{};///< Array of attachment data.
		jsonifier::vector<ActionRowData> components{};///< Array of action row data.
		jsonifier::vector<ReactionData> reactions{};//< Array of reaction data.
		jsonifier::vector<StickerData> stickers{};///< Array of Message StickerData data.
		MessageReferenceData messageReference{};///< Message reference data.
		jsonifier::vector<UserData> mentions{};///< Array of UserData data, for individual's that were mentioned.
		jsonifier::vector<EmbedData> embeds{};///< Array of Message embeds.
		MessageInteractionData interaction{};///< Message Interaction data.
		jsonifier::string editedTimestamp{};///< The time at which it was edited.
		MessageActivityData activity{};///< Message activity data.
		ApplicationData application{};///< Application data.
		jsonifier::string content{};///< The Message's content.
		TimeStampParse timeStamp{};///< The timeStamp of when the Message was created.
		Snowflake applicationId{};///< Application id.
		jsonifier::string nonce{};///< Nonce.
		GuildMemberData member{};///< The author's Guild member data.
		bool mentionEveryone{};///< Does the Message mention everyone?
		Snowflake channelId{};///< The ChannelData it was sent in.
		Snowflake webHookId{};///< WebHook id of the MessageData, if applicable.
		ChannelData thread{};///< The ThreadData that the Message was sent in, if applicable.
		Snowflake guildId{};///< The id of the Guild the Message was sent in.
		MessageType type{};///< Message type.
		UserData author{};///< The author's UserData data.
		uint64_t flags{};///< Flags.
		Snowflake id{};
		bool pinned{};///< Is it pinned?
		bool tts{};///< Is it a text-to-speech MessageData?

		virtual inline ~MessageDataOld() = default;
	};

	/// @brief Data structure representing a single MessageData.
	class MessageData : public MessageDataOld {
	  public:
		MessageDataOld referencedMessage{};///< The referenced Message, to reply to.
	};

	/// @brief Resolved data.
	struct ResolvedData {
		UnorderedMap<jsonifier::string, AttachmentData> attachments{};///< UnorderedMap of Snowflakes to attachment objects the ids.
		UnorderedMap<jsonifier::string, GuildMemberData> members{};///< UnorderedMap full of GuildMemeberData.
		UnorderedMap<jsonifier::string, MessageData> messages{};///< UnorderedMap full of messageData->
		UnorderedMap<jsonifier::string, ChannelData> channels{};///< UnorderedMap full of ChannelData.
		UnorderedMap<jsonifier::string, UserData> users{};///< UnorderedMap full of UserData.
		UnorderedMap<jsonifier::string, RoleData> roles{};///< UnorderedMap full of RoleData.
	};

	/// @brief Represents a StickerData pack.
	struct StickerPackData {
		jsonifier::vector<StickerData> stickers{};///< Array of StickerData objects	the stickers in the pack.
		jsonifier::string coverStickerId{};///< Snowflake of a StickerData in the pack which is shown as the pack's icon.
		jsonifier::string bannerAssetId{};///< Snowflake of the StickerData pack's banner image.
		jsonifier::string description{};///< Description of the StickerData pack.
		jsonifier::string skuId{};///< Snowflake of the pack's SKU.
		jsonifier::string name{};///< Name of the StickerData pack.
		Snowflake snowflake{};///< Snowflake of the StickerData pack.
	};

	/// @brief Connection visibility types.
	enum class ConnectionVisibilityTypes : uint8_t {
		None	 = 0,///< None.
		Everyone = 1///< Everyone.
	};

	/// @brief Represents a single UserData Connection.
	struct ConnectionData {
		jsonifier::vector<IntegrationData> integrations{};///< An array of partial server integrations.
		ConnectionVisibilityTypes visibility{};///< Visibility of this connecti
		jsonifier::string name{};///< The username of the connection account.
		jsonifier::string type{};///< The service of the connection(twitch, youtube).
		bool showActivity{};///< Whether activities related to this connection will be shown in presence updates.on.
		bool friendSync{};///< Whether friend sync is enabled for this connection.
		bool verified{};///< Whether the connection is verified.
		bool revoked{};///< Whether the connection is revoked.
		Snowflake id{};///< Snowflake of the connection account.
	};

	struct ApplicationCommandInteractionDataOption;

	/// @brief ApplicationCommandData Interaction data option.
	struct ApplicationCommandInteractionDataOption {
		jsonifier::vector<ApplicationCommandInteractionDataOption> options{};///< ApplicationCommandData Interaction data options.
		ApplicationCommandOptionType type{};///< The type of ApplicationCommandData options.
		jsonifier::raw_json_data value{};///< The value.
		jsonifier::string name{};///< The name of the current option.
		bool focused{};///< True if this option is the currently focused option for autocomplete.
	};

	/// @brief Interaction data data.
	struct InteractionDataData {
		jsonifier::vector<ApplicationCommandInteractionDataOption> options{};///< ApplicationCommandData Interaction data options.
		jsonifier::vector<jsonifier::string> values{};///< The values of the components.
		jsonifier::string customIdSmall{};///< The custom id of a particular modal input.
		ApplicationCommandType type{};///< The type of ApplicationCommandData.
		ComponentType componentType{};///< The type of component.
		jsonifier::string customId{};///< The custom id of the Interaction entity.
		jsonifier::string value{};///< The input value of the modal.
		jsonifier::string name{};///< The name of the command.
		ResolvedData resolved{};///< Resolved data.
		Snowflake targetId{};///< The target MessageData's id.
		Snowflake guildId{};///< The guild that the command took place in.
	};

	/// @brief Interaction data.
	class InteractionData {
	  public:
		PermissionsParse appPermissions{};///< Bitwise set of permissions the app or bot has within the channel the interaction was sent from.
		jsonifier::string guildLocale{};///< The guild's preferred locale, if invoked in a guild.
		InteractionDataData data{};///< The Interaction's data.
		jsonifier::string locale{};///< The locale.
		jsonifier::string token{};///< The Interaction token.
		Snowflake applicationId{};///< The application's id.
		GuildMemberData member{};///< The data of the Guild member who sent the Interaction, if applicable.
		InteractionType type{};///< The type of Interaction.
		ChannelData channel{};///< The channel that the interaction was sent through.
		MessageData message{};///< The Message that the Interaction came through on, if applicable.
		Snowflake channelId{};///< The ChannelData the Interaction was sent in.
		Snowflake guildId{};///< The Guild id of the Guild it was sent in.
		uint64_t version{};///< The Interaction version.
		UserData user{};///< The UserData data of the sender of the Interaction.
		Snowflake id{};

		inline InteractionData() = default;
	};

	/// @brief Data from the SessionStart info.
	struct SessionStartData {
		uint32_t maxConcurrency{};///< The number of identify requests allowed per 5 seconds.
		uint32_t resetAfter{};///< The number of Milliseconds after which the limit resets.
		uint32_t remaining{};///< The remaining number of session starts the current UserData is allowed.
		uint32_t total{};///< The total number of session starts the current UserData is allowed.
	};

	/// @brief Data from the GetGatewayBot endpoint.
	struct GatewayBotData {
		SessionStartData sessionStartLimit{};///< Information on the current session start limit.
		jsonifier::string url{};///< The WSS Url that can be used for connecting to the gateway.
		uint32_t shards{};///< The recommended number of shards to use when connecting.
	};

	/// @brief Text input style for modals.
	enum class TextInputStyle : uint8_t {
		Short	  = 1,///< A single-line input.
		Paragraph = 2///< A multi-line input.
	};

	/// @brief Data representing a Guild Emoji Update event.
	struct GuildEmojisUpdateEventData {
		jsonifier::vector<EmojiData> emojis{};
		Snowflake guildId{};
	};

	/// @brief Data representing a Guild Stickers Update event.
	struct GuildStickersUpdateEventData {
		jsonifier::vector<StickerData> stickers{};
		Snowflake guildId{};
	};

	struct GuildMembersChunkEventData {
		jsonifier::vector<PresenceUpdateData> presences{};
		jsonifier::vector<jsonifier::string> notFound{};
		jsonifier::vector<GuildMemberData> members{};
		jsonifier::string nonce{};
		uint64_t chunkIndex{};
		uint64_t chunkCount{};
		Snowflake guildId{};
	};

	/// @brief A type of UserData, to represent the Bot and some of its associated endpoints.
	class DiscordCoreAPI_Dll BotUser : public UserData {
	  public:
		template<typename ValueType> friend class GetUserImageUrl;
		friend class DiscordCoreClient;

		BotUser(UserData dataPackage, DiscordCoreInternal::BaseSocketAgent* pBaseBaseSocketAgentNew);

		/// @brief Updates the bot's current voice-status. Joins/leaves a Channel, and/or self deafens/mutes.
		/// @param dataPackage The new VoiceStateData.
		void updateVoiceStatus(UpdateVoiceStateData& dataPackage);

		/// @brief Updates the bot's current activity status, to be viewed by others in the same server as the bot.
		/// @param dataPackage The new presence data.
		void updatePresence(UpdatePresenceData& dataPackage);

	  protected:
		BotUser() = default;

		DiscordCoreInternal::BaseSocketAgent* baseSocketAgent{};
	};

	/// @brief Guild flags.
	enum class GuildFlags : uint8_t {
		WidgetEnabled				 = 1 << 0,///< Widget enabled.
		Unavailable					 = 1 << 1,///< Unavailable.
		Owner						 = 1 << 2,///< Owner.
		Large						 = 1 << 3,///< Is it a large guild?
		Premium_Progress_Bar_Enabled = 1 << 4///< Premium progress bar enabled
	};

	template<GuildMemberT ValueType> TwoIdKey::TwoIdKey(const ValueType& other) {
		idOne = other.guildId;
		idTwo = other.user.id;
	}

	template<VoiceStateT ValueType> TwoIdKey::TwoIdKey(const ValueType& other) {
		idOne = other.guildId;
		idTwo = other.userId;
	}

	/// CRTP-based struct for exposing the connectToVoice functionality of the Guild-related classes.
	template<typename ValueType, typename DiscordCoreClientT, typename GuildMembersType> struct ConnectToVoice {
		/// @brief For connecting to an individual voice channel.
		/// @param guildMemberId An id of the guild member who's current voice channel to connect to.
		/// @param channelId An id of the voice channel to connect to.
		/// @param selfDeaf Whether or not to self-deafen the bot.
		/// @param selfMute Whether or not to self-mute the bot.
		/// @param streamInfoNew For usage with the Vc-to-Vc audio streaming option.
		/// @return VoiceConnection* A pointer to the currently held voice connection, or nullptr if it failed to connect.
		inline VoiceConnection& connectToVoice(const Snowflake guildMemberId, const Snowflake channelId = 0, bool selfDeaf = false, bool selfMute = false,
			StreamInfo streamInfoNew = StreamInfo{}) {
			if (static_cast<DiscordCoreClientT*>(static_cast<ValueType*>(this)->getDiscordCoreClient())->getVoiceConnection(static_cast<ValueType*>(this)->id).areWeConnected()) {
				return static_cast<DiscordCoreClientT*>(static_cast<ValueType*>(this)->getDiscordCoreClient())->getVoiceConnection(static_cast<ValueType*>(this)->id);
			} else if (guildMemberId != 0 || channelId != 0) {
				Snowflake channelIdNew{};
				if (guildMemberId != 0) {
					VoiceStateDataLight dataLight{};
					dataLight.guildId = static_cast<ValueType*>(this)->id;
					dataLight.userId  = guildMemberId;
					GuildMemberData getData{};
					getData.guildId		= static_cast<ValueType*>(this)->id;
					getData.user.id		= guildMemberId;
					auto voiceStateData = GuildMembersType::getVoiceStateData(getData);
					if (voiceStateData.channelId != 0) {
						channelIdNew = voiceStateData.channelId;
					}
				} else {
					channelIdNew = channelId;
				}
				int32_t theShardId{ static_cast<int32_t>((static_cast<ValueType*>(this)->id.operator const uint64_t&() >> 22) %
					static_cast<DiscordCoreClientT*>(static_cast<ValueType*>(this)->getDiscordCoreClient())->getConfigManager().getTotalShardCount()) };
				VoiceConnectInitData voiceConnectInitData{};
				voiceConnectInitData.currentShard = theShardId;
				voiceConnectInitData.streamInfo	  = streamInfoNew;
				voiceConnectInitData.channelId	  = channelIdNew;
				voiceConnectInitData.guildId	  = static_cast<ValueType*>(this)->id;
				voiceConnectInitData.userId		  = static_cast<DiscordCoreClientT*>(static_cast<ValueType*>(this)->getDiscordCoreClient())->getBotUser().id;
				voiceConnectInitData.selfDeaf	  = selfDeaf;
				voiceConnectInitData.selfMute	  = selfMute;
				auto& voiceConnectionNew =
					static_cast<DiscordCoreClientT*>(static_cast<ValueType*>(this)->getDiscordCoreClient())->getVoiceConnection(static_cast<ValueType*>(this)->id);
				StopWatch<Milliseconds> stopWatch{ Milliseconds{ 10000 } };
				stopWatch.reset();
				voiceConnectionNew.connect(voiceConnectInitData);
				while (!voiceConnectionNew.areWeConnected()) {
					if (stopWatch.hasTimeElapsed()) {
						break;
					}
					std::this_thread::sleep_for(1ms);
				}
				return voiceConnectionNew;
			}
			return static_cast<DiscordCoreClientT*>(static_cast<ValueType*>(this)->getDiscordCoreClient())->getVoiceConnection(static_cast<ValueType*>(this)->id);
		}
	};

	/// @brief A discord Guild. Used to connect to/disconnect from voice.
	class DiscordCoreAPI_Dll GuildData : public FlagEntity<GuildData>, public GetGuildImageUrl<GuildData>, public ConnectToVoice<GuildData, DiscordCoreClient, GuildMembers> {
	  public:
		friend class Guilds;

		jsonifier::vector<GuildScheduledEventData> guildScheduledEvents{};///< Scheduled events in the guild.
		DefaultMessageNotificationLevel defaultMessageNotifications{};///< Default message notifications level.
		jsonifier::vector<StageInstanceData> stageInstances{};///< Stage instances in the guild.
		jsonifier::vector<VoiceStateDataLight> voiceStates{};///< Voice states for the current GuildMembers.
		ExplicitContentFilterLevel explicitContentFilter{};///< Explicit content filter level.
		jsonifier::vector<PresenceUpdateData> presences{};///< Presences of the members in the guild.
		jsonifier::vector<jsonifier::string> features{};///< Enabled guild features.
		jsonifier::vector<GuildMemberData> members{};///< Custom guild emojis.
		jsonifier::vector<ChannelData> channels{};///< Custom guild emojis.
		jsonifier::vector<StickerData> stickers{};///< Custom guild stickers.
		jsonifier::vector<ChannelData> threads{};///< All active threads in the guild that the current user has permission to view.
		SystemChannelFlags systemChannelFlags{};///< System channel flags.
		VerificationLevel verificationLevel{};///< Verification level required for the guild.
		jsonifier::vector<EmojiData> emoji{};///< Custom guild emojis.
		uint32_t maxStageVideoChannelUsers{};///< The maximum amount of users in a stage video channel.
		jsonifier::vector<RoleData> roles{};///< Roles in the guild.
		uint32_t premiumSubscriptionCount{};///< The number of boosts this guild currently has.
		jsonifier::string preferredLocale{};///< The preferred locale of a Community guild; used in server discovery and notices from Discord.
		jsonifier::string discoverySplash{};///< Discovery splash hash; only present for guilds with the "DISCOVERABLE" feature.
		uint32_t approximatePresenceCount{};///< Approximate number of non-offline members in this guild, returned sometimes.
		Snowflake publicUpdatesChannelId{};///< The id of the channel where admins and moderators of Community guilds receive notices from Discord.
		VoiceConnection* voiceConnection{};///< Voice connection potentially held by the current Guild.
		WelcomeScreenData welcomeScreen{};///< The welcome screen of a Community guild, shown to new members, returned in an Invite's guild object.
		Snowflake safetyAlertsChannelId{};///< The id of the channel where admins and moderators of Community guilds receive safety alerts.
		uint32_t approximateMemberCount{};///< Approximate number of members in this guild, returned from the GET /guilds/id and /users/me/guilds.
		jsonifier::string vanityUrlCode{};///< The vanity url code for the guild.
		bool premiumProgressBarEnabled{};///< Whether the guild has the boost progress bar enabled.
		uint32_t maxVideoChannelUsers{};///< The maximum amount of users in a video channel.
		jsonifier::string description{};///< The description of a guild.
		PermissionsParse permissions{};///< Total permissions for the user in the guild (excludes overwrites).
		jsonifier::string discovery{};///< Url to the Guild's icon.
		Snowflake systemChannelId{};///< The id of the channel where guild notices such as welcome messages and boost events are posted.
		Snowflake widgetChannelId{};///< The channel id that the widget will generate an invite to, or null if set to no invite.
		Snowflake rulesChannelId{};///< The id of the channel where Community guilds can display rules and/or guidelines.
		GuildNSFWLevel nsfwLevel{};///< Guild NSFW level.
		jsonifier::string splash{};///< Splash hash.
		jsonifier::string banner{};///< Banner hash.
		TimeStampParse joinedAt{};///< When the bot joined this Guild.
		Snowflake applicationId{};///< Application id of the guild creator if it is bot-created.
		PremiumTier premiumTier{};///< Premium tier (Server Boost level).
		Snowflake afkChannelId{};///< Id of afk channel.
		jsonifier::string name{};///< Guild name (2-100 characters, excluding trailing and leading whitespace).
		jsonifier::string icon{};///< Icon hash.
		uint32_t maxPresences{};///< The maximum number of presences for the guild (null is always returned, apart from the largest of guilds).
		uint32_t memberCount{};///< The number of members in this guild.jsonifier::string region{};///< Voice region id for the guild (deprecated).
		uint32_t maxMembers{};///< The maximum number of members for the guild.
		uint32_t afkTimeout{};///< Afk timeout in seconds.
		bool widgetEnabled{};///< True if the server widget is enabled.
		MFALevel mfaLevel{};///< Required MFA level for the guild.
		Snowflake ownerId{};///< Id of owner.
		bool unavailable{};///< true if this guild is unavailable due to an outage.
		GuildFlags flags{};/// The flags for this Guild.
		Snowflake id{};///< Guild id.
		bool large{};///< true if this is considered a large guild.
		bool owner{};///< True if the user is the owner of the guild.

		inline GuildData() = default;

		GuildData(Snowflake snowflake);

		DiscordCoreClient* getDiscordCoreClient();

		bool areWeConnected();

		void disconnect();
	};

	/// @brief Represents a single ThreadData.
	class ThreadData : public ChannelData {
	  public:
	};

	/// @brief Data structure representing a single Guild, for the purposes of populating the cache.
	class DiscordCoreAPI_Dll GuildCacheData : public FlagEntity<GuildCacheData>,
											  public GetGuildImageUrl<GuildCacheData>,
											  public ConnectToVoice<GuildCacheData, DiscordCoreClient, GuildMembers> {
	  public:
		jsonifier::vector<Snowflake> channels{};///< Array of Guild channels.
		jsonifier::vector<Snowflake> members{};///< Array of GuildMembers.
		jsonifier::vector<Snowflake> emoji{};///< Array of Guild channels.
		jsonifier::vector<Snowflake> roles{};///< Array of Guild roles.
		VoiceConnection* voiceConnection{};///< A pointer to the VoiceConnection, if present.
		IconHash discoverySplash{};///< Url to the Guild's icon.
		jsonifier::string name{};///< The Guild's name.
		uint32_t memberCount{};///< Member count.
		TimeStamp joinedAt{};///< When the bot joined this Guild.
		IconHash discovery{};///< Url to the Guild's icon.
		Snowflake ownerId{};///< UserData id of the Guild's owner.
		GuildFlags flags{};///< Guild flags.
		IconHash banner{};///< Url to the Guild's icon.
		IconHash splash{};///< Url to the Guild's icon.
		IconHash icon{};///< Url to the Guild's icon.
		Snowflake id{};///< The id of this Guild.

		inline GuildCacheData() = default;

		GuildCacheData(Snowflake snowflakeNew);

		GuildCacheData& operator=(GuildData&& data) noexcept;
		GuildCacheData(GuildData&& data) noexcept;

		GuildCacheData& operator=(const GuildData&);

		GuildCacheData(const GuildData&);

		operator GuildData();

		DiscordCoreClient* getDiscordCoreClient();

		inline bool operator==(const Snowflake& other) const {
			return id == other;
		}

		bool areWeConnected();

		void disconnect();
	};

	class PartialGuildData {
	  public:
		bool unavailable{};
		Snowflake id{};
	};

	/// @brief Invite data.
	struct InviteData {
		GuildScheduledEventData guildScheduledEvent{};///< Scheduled Guild event.
		ApplicationData targetApplication{};///< Application data.
		uint64_t approximatePresenceCount{};///< Approximate presence count.
		StageInstanceData stageInstance{};///< Stage instance data.
		uint64_t approximateMemberCount{};///< Approximate member count.
		jsonifier::string expiresAt{};///< When the invite expires.
		jsonifier::string createdAt{};///< Time it was created at.
		PartialGuildData guild{};///< Guild data of the ChannelData that the invite is for.
		jsonifier::string code{};///< Unique invite code.
		UserData targetUser{};///< Target UserData of the invite.
		ChannelData channel{};///< ChannelData data of the ChannelData that the invite is for.
		uint64_t targetType{};///< Target type.
		Snowflake guildId{};///< The Guild this invite is for.
		UserData inviter{};///< The UserData who created the invite.
		uint64_t maxUses{};///< Max number of uses.
		uint64_t maxAge{};///< Maximum age of the invite.
		bool temporary{};///< Is it temporary?
		uint64_t uses{};///< The current number of uses.
	};

	/// @brief Represents a Guild Template.
	struct GuildTemplateData {
		GuildData serializedSourceGuild{};///< The Guild snapshot this template contains.
		jsonifier::string sourceGuildId{};///< The ID of the Guild this template is based on.
		jsonifier::string description{};///< The description for the template.
		jsonifier::string creatorId{};///< The ID of the UserData who created the template.
		jsonifier::string createdAt{};///< When this template was created.
		jsonifier::string updatedAt{};///< When this template was last synced to the source Guild.
		jsonifier::string code{};///< The template code(unique ID).
		jsonifier::string name{};///< Template name.
		uint32_t usageCount{};///< Number of times this template has been used.
		UserData creator{};///< The UserData who created the template.
		bool isDirty{};///< Whether the template has unsynced changes.
	};

	/// @brief WebHookData data.
	class WebHookData {
	  public:
		ChannelData sourceChannel{};///< ChannelData for which th WebHookData was issued.
		jsonifier::string avatar{};///< Avatar of the WebHookData.
		jsonifier::string token{};///< Token of the WebHookData.
		Snowflake applicationId{};///< Application id.
		jsonifier::string name{};///< Name of the WebHookData.
		jsonifier::string url{};///< Url of the WebHookData.
		GuildData sourceGuild{};///< Source Guild id.
		Snowflake channelId{};///< ChannelData id for which the WebHookData was issued.
		Snowflake guildId{};///< Guild id for which the WebHookData was issued.
		WebHookType type{};///< Type of WebHookData.
		UserData user{};///< UserData which create the WebHookData.
		Snowflake id{};

		inline WebHookData() = default;
	};

	/// @brief Audit log data.
	class DiscordCoreAPI_Dll AuditLogData {
	  public:
		jsonifier::vector<GuildScheduledEventData> guildScheduledEvents{};///< Array of guild scheduled event objects.
		jsonifier::vector<AutoModerationRuleData> autoModerationRules{};///< List of auto moderation rules referenced in the audit log.
		jsonifier::vector<AuditLogEntryData> auditLogEntries{};///< Array of audit log entry objects.
		jsonifier::vector<IntegrationData> integrations{};///< Array of partial integration objects.
		jsonifier::vector<WebHookData> webhooks{};///< Array of webhook objects.
		jsonifier::vector<ChannelData> threads{};///< Array of thread-specific channel objects.
		jsonifier::vector<UserData> users{};///< Array of user objects.

		auto getAuditLogData(const Snowflake userIdOfChanger, AuditLogEvent auditLogType);

		auto getAuditLogData(AuditLogEvent auditLogType, const Snowflake userIdOfTarget);
	};

	/// @brief Data representing an input-event, which is any Message or Interaction that is coming into the bot as an input.
	class DiscordCoreAPI_Dll InputEventData {
	  public:
		template<typename ValueType> friend struct jsonifier::core;
		template<typename ValueType> friend struct EventData;
		friend struct OnInteractionCreationData;
		friend struct BaseFunctionArguments;

		friend class DiscordCoreInternal::WebSocketClient;
		friend class DiscordCoreInternal::BaseSocketAgent;
		friend class RespondToInputEventData;
		friend class DiscordCoreClient;
		friend class CommandData;
		friend class InputEvents;

		InputEventResponseType responseType{};///< The type of response that this input value represents.

		InputEventData& operator=(InputEventData&& other) noexcept;

		InputEventData(InputEventData&& other) noexcept;

		InputEventData& operator=(const InputEventData& other);

		InputEventData(const InputEventData& other);

		InputEventData& operator=(const InteractionData& other);

		InputEventData(const InteractionData& interactionData);

		inline InputEventData() = default;

		/// @brief Returns the Interaction data, if appplicable, of this input-event.
		/// @return InteractionData A Snowflake containing the Interaction data.
		InteractionData getInteractionData() const;

		/// @brief Returns the GuildMemberData of this input-event.
		/// @return GuildMemberData A GuildMemberData containing the GuildMemberData data.
		GuildMemberData getGuildMemberData() const;

		/// @brief Returns the ChannelData of this input-event.
		/// @return ChannelData A ChannelData containing the ChannelData data.
		ChannelData getChannelData() const;

		/// @brief Returns the Message data, if applicable, of this input-event.
		/// @return Message A Message structure.
		MessageData getMessageData() const;

		/// @brief Returns the UserData of this input-event.
		/// @return UserData A UserData containing the UserData data.
		UserData getUserData() const;

	  protected:
		UniquePtr<InteractionData> interactionData{ makeUnique<InteractionData>() };
	};

	struct MoveThroughMessagePagesData;

	/// @brief Data for responding to an input-event.
	class DiscordCoreAPI_Dll RespondToInputEventData {
	  public:
		friend struct DeleteInteractionResponseData;
		friend struct DeleteFollowUpMessageData;
		friend struct InteractionResponseData;

		friend DiscordCoreAPI_Dll MoveThroughMessagePagesData moveThroughMessagePages(jsonifier::string_view userID, InputEventData originalEvent, uint32_t currentPageIndex,
			const jsonifier::vector<EmbedData>& messageEmbeds, bool deleteAfter, uint32_t waitForMaxMs, bool returnResult);

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

		operator InteractionCallbackData() const;

		RespondToInputEventData& operator=(const InteractionData& dataPackage);

		RespondToInputEventData(const InteractionData& dataPackage);

		RespondToInputEventData& operator=(const InputEventData& dataPackage);

		RespondToInputEventData(const InputEventData& dataPackage);

		/// @brief Adds a button to the response MessageData.
		/// @param disabled Whether the button is active or not.
		/// @param customIdNew A custom id to give for identifying the button.
		/// @param buttonLabel A visible label for the button.
		/// @param buttonStyle The style of the button.
		/// @param emojiName An emoji name, if desired.
		/// @param emojiId An emoji id, if desired.
		/// @param url A url, if applicable.
		/// @return RespondToInputEventData& A reference to this data structure.
		RespondToInputEventData& addButton(bool disabled, jsonifier::string_view customIdNew, jsonifier::string_view buttonLabel, ButtonStyle buttonStyle,
			jsonifier::string_view emojiName = "", Snowflake emojiId = Snowflake{}, jsonifier::string_view url = "");

		/// @brief Adds a select-menu to the response MessageData.
		/// @param disabled Whether the select-menu is active or not.
		/// @param customIdNew A custom id to give for identifying the select-menu.
		/// @param options A vector of select-menu-options to offer.
		/// @param placeholder Custom placeholder text if nothing is selected, max 100 characters.
		/// @param maxValues Maximum number of selections that are possible.
		/// @param minValues Minimum required number of selections that are required.
		/// @param type The type of select-menu that this is.
		/// @param channelTypes Types of channels that can be accepted if this is of the type ChannelType.
		/// @return RespondToInputEventData& A reference to this data structure.
		RespondToInputEventData& addSelectMenu(bool disabled, jsonifier::string_view customIdNew, const jsonifier::vector<SelectOptionData>& options,
			jsonifier::string_view placeholder, uint64_t maxValues, uint64_t minValues, SelectMenuType type,
			jsonifier::vector<ChannelType> channelTypes = jsonifier::vector<ChannelType>{});

		/// @brief Adds a modal to the response MessageData.
		/// @param topTitleNew A title for the modal.
		/// @param topCustomIdNew A custom id to give for the modal.
		/// @param titleNew A title for the modal's individual input.
		/// @param customIdNew A custom id to give for the modal's individual input.
		/// @param required Is it a required response?
		/// @param minLength Minimum length.
		/// @param maxLength Maximum length.
		/// @param inputStyle The input style.
		/// @param label A label for the modal.
		/// @param placeholder A placeholder for the modal.
		/// @return RespondToInputEventData& A reference to this data structure.
		RespondToInputEventData& addModal(jsonifier::string_view topTitleNew, jsonifier::string_view topCustomIdNew, jsonifier::string_view titleNew,
			jsonifier::string_view customIdNew, bool required, uint64_t minLength, uint64_t maxLength, TextInputStyle inputStyle, jsonifier::string_view label = "",
			jsonifier::string_view placeholder = "");

		/// @brief Adds a file to the current collection of files for this message response.
		/// @param theFile The file to be added.
		/// @return RespondToInputEventData& A reference to this data structure.
		RespondToInputEventData& addFile(const File& theFile);

		/// @brief For setting the allowable mentions in a response.
		/// @param dataPackage An AllowedMentionsData structure.
		/// @return RespondToInputEventData& A reference to this data structure.
		RespondToInputEventData& addAllowedMentions(const AllowedMentionsData& dataPackage);

		/// @brief For setting the type of response to make.
		/// @param typeNew An InputEventResponseType.
		/// @return RespondToInputEventData& A reference to this data structure.
		RespondToInputEventData& setResponseType(InputEventResponseType typeNew);

		/// @brief For setting the components in a response.
		/// @param dataPackage An ActionRowData structure.
		/// @return RespondToInputEventData& A reference to this data structure.
		RespondToInputEventData& addComponentRow(const ActionRowData& dataPackage);

		/// @brief For setting the embeds in a response.
		/// @param dataPackage An EmbedData structure.
		/// @return RespondToInputEventData& A reference to this data structure.
		RespondToInputEventData& addMessageEmbed(const EmbedData& dataPackage);

		/// @brief For setting the Message content in a response.
		/// @param dataPackage A string, containing the content.
		/// @return RespondToInputEventData& A reference to this data structure.
		RespondToInputEventData& addContent(jsonifier::string_view dataPackage);

		/// @brief For setting the tts status of a response.
		/// @param enabledTTs A bool.
		/// @return RespondToInputEventData& A reference to this data structure.
		RespondToInputEventData& setTTSStatus(bool enabledTTs);

		/// @brief For setting the choices of an autocomplete response.
		/// @param value An simdjson::ondemand::object value that is either a double, uint64_t or a string.
		/// @param theName A string for the name of the choice.
		/// @param theNameLocalizations A UnorderedMap<jsonifier::string, jsonifier::string> for the name localizations.
		/// @return RespondToInputEventData& A reference to this data structure.
		RespondToInputEventData& setAutoCompleteChoice(DiscordCoreInternal::EtfSerializer value, jsonifier::string_view theName,
			UnorderedMap<jsonifier::string, jsonifier::string> theNameLocalizations);

		/// @brief For setting the direct-MessageData User target of a response.
		/// @param targetUserIdNew A string, containing the target UserData's id.
		/// @return RespondToInputEventData& A reference to this data structure.
		RespondToInputEventData& setTargetUserID(const Snowflake targetUserIdNew);

	  protected:
		jsonifier::vector<ApplicationCommandOptionChoiceData> choices{};
		jsonifier::vector<ActionRowData> components{};
		AllowedMentionsData allowedMentions{};
		jsonifier::vector<EmbedData> embeds{};
		jsonifier::string interactionToken{};
		jsonifier::vector<File> files{};
		InputEventResponseType type{};///< The type of response to make.
		jsonifier::string customId{};
		jsonifier::string content{};
		InteractionType eventType{};
		jsonifier::string title{};
		Snowflake interactionId{};
		Snowflake applicationId{};
		Snowflake targetUserId{};
		Snowflake channelId{};
		Snowflake messageId{};
		uint64_t flags{};
		bool tts{};
	};

	/// @brief Message response base, for responding to messages.
	class DiscordCoreAPI_Dll MessageResponseBase {
	  public:
		template<typename ValueType> friend struct jsonifier::core;
		/// @brief Adds a button to the response MessageData.
		/// @param disabled Whether the button is active or not.
		/// @param customIdNew A custom id to give for identifying the button.
		/// @param buttonLabel A visible label for the button.
		/// @param buttonStyle The style of the button.
		/// @param emojiName An emoji name, if desired.
		/// @param emojiId An emoji id, if desired.
		/// @param url A url, if applicable.
		/// @return MessageResponseBase& A reference to this data structure.
		MessageResponseBase& addButton(bool disabled, jsonifier::string_view customIdNew, jsonifier::string_view buttonLabel, ButtonStyle buttonStyle,
			jsonifier::string_view emojiName = "", Snowflake emojiId = Snowflake{}, jsonifier::string_view url = "");

		/// @brief Adds a select-menu to the response MessageData.
		/// @param disabled Whether the select-menu is active or not.
		/// @param customIdNew A custom id to give for identifying the select-menu.
		/// @param options A vector of select-menu-options to offer.
		/// @param placeholder Custom placeholder text if nothing is selected, max 100 characters.
		/// @param maxValues Maximum number of selections that are possible.
		/// @param minValues Minimum required number of selections that are required.
		/// @param type The type of select-menu that this is.
		/// @param channelTypes Types of channels that can be accepted if this is of the type ChannelType.
		/// @return RespondToInputEventData& A reference to this data structure.
		MessageResponseBase& addSelectMenu(bool disabled, jsonifier::string_view customIdNew, jsonifier::vector<SelectOptionData> options, jsonifier::string_view placeholder,
			uint64_t maxValues, uint64_t minValues, SelectMenuType type, jsonifier::vector<ChannelType> channelTypes = jsonifier::vector<ChannelType>{});

		/// @brief Adds a modal to the response MessageData.
		/// @param topTitleNew A title for the modal.
		/// @param topCustomIdNew A custom id to give for the modal.
		/// @param titleNew A title for the modal's individual input.
		/// @param customIdNew A custom id to give for the modal's individual input.
		/// @param required Is it a required response?
		/// @param minLength Minimum length.
		/// @param maxLength Maximum length.
		/// @param inputStyle The input style.
		/// @param label A label for the modal.
		/// @param placeholder A placeholder for the modal.
		/// @return MessageResponseBase& A reference to this data structure.
		MessageResponseBase& addModal(jsonifier::string_view topTitleNew, jsonifier::string_view topCustomIdNew, jsonifier::string_view titleNew,
			jsonifier::string_view customIdNew, bool required, uint64_t minLength, uint64_t maxLength, TextInputStyle inputStyle, jsonifier::string_view label = "",
			jsonifier::string_view placeholder = "");

		/// @brief Adds a file to the current collection of files for this message response.
		/// @param theFile The file to be added.
		/// @return MessageResponseBase& A reference to this data structure.
		MessageResponseBase& addFile(File theFile);

		/// @brief For setting the allowable mentions in a response.
		/// @param dataPackage An AllowedMentionsData structure.
		/// @return MessageResponseBase& A reference to this data structure.
		MessageResponseBase& addAllowedMentions(const AllowedMentionsData& dataPackage);

		/// @brief For setting the components in a response.
		/// @param dataPackage An ActionRowData structure.
		/// @return MessageResponseBase& A reference to this data structure.
		MessageResponseBase& addComponentRow(const ActionRowData& dataPackage);

		/// @brief For setting the embeds in a response.
		/// @param dataPackage An EmbedData structure.
		/// @return MessageResponseBase& A reference to this data structure.
		MessageResponseBase& addMessageEmbed(const EmbedData& dataPackage);

		/// @brief For setting the Message content in a response.
		/// @param dataPackage A string, containing the content.
		/// @return MessageResponseBase& A reference to this data structure.
		MessageResponseBase& addContent(jsonifier::string_view dataPackage);

		/// @brief For setting the tts status of a response.
		/// @param enabledTTs A bool.
		/// @return MessageResponseBase& A reference to this data structure.
		MessageResponseBase& setTTSStatus(bool enabledTTs);

	  protected:
		jsonifier::vector<ActionRowData> components{};
		AllowedMentionsData allowedMentions{};
		jsonifier::vector<EmbedData> embeds{};
		jsonifier::vector<File> files{};
		jsonifier::string customId{};
		jsonifier::string content{};
		jsonifier::string title{};
		uint64_t flags{};
		bool tts{};
	};

	/// @brief Interaction response data.
	struct DiscordCoreAPI_Dll InteractionResponseData {
		InteractionCallbackData data{};///< Interaction ApplicationCommandData callback data.
		InteractionCallbackType type{};///< Interaction callback type.

		InteractionResponseData& operator=(const RespondToInputEventData& other);

		InteractionResponseData(const RespondToInputEventData& other);

		InteractionResponseData() = default;
	};

	/// @brief Guild application command permissions data.
	class GuildApplicationCommandPermissionData {
	  public:
		jsonifier::vector<ApplicationCommandPermissionData> permissions{};
		Snowflake applicationId{};
		Snowflake guildId{};
		Snowflake id{};
	};

	/**@}*/

	/// @brief Song types.
	enum class SongType : uint8_t {
		YouTube	   = 0,///< YouTube.
		SoundCloud = 1,///< SoundCloud.
		Neutral	   = 2///< For either type.
	};

	/// @brief Represents a download Url.
	struct DownloadUrl {
		uint64_t contentSize{};
		jsonifier::string urlPath{};
	};

	/**
	 * \addtogroup voice_connection
	 * @{
	 */

	class Song {
	  public:
		friend class DiscordCoreInternal::SoundCloudRequestBuilder;
		friend class DiscordCoreInternal::YouTubeRequestBuilder;
		friend class DiscordCoreInternal::SoundCloudAPI;
		friend class DiscordCoreInternal::YouTubeAPI;
		friend class SongAPI;

		jsonifier::vector<DownloadUrl> finalDownloadUrls{};
		SongType type{ SongType::SoundCloud };///< The type of song.
		jsonifier::string secondDownloadUrl{};
		jsonifier::string firstDownloadUrl{};
		jsonifier::string addedByUserName{};///< The UserData name of the individual who added this Song to the
		jsonifier::string thumbnailUrl{};///< The Url of the thumbnail image of this Song.
		jsonifier::string description{};///< A description of the Song.
		jsonifier::string songTitle{};///< The title of the Song.
		jsonifier::string duration{};///< The duration of the Song.
		jsonifier::string viewUrl{};///< The url for listening to this Song through a browser.
		jsonifier::string songId{};
		Snowflake addedByUserId{};///< The UserData id of the individual who added this Song to the.
		uint64_t contentLength{};
	};

	/// @brief Song completion event data.
	struct SongCompletionEventData {
		GuildMemberData guildMember{};///< The sending GuildMemberData.
		bool wasItAFail{};///< Is this a replay? (Did a track recently fail to play?)
		GuildData guild{};///< The sending Guild.
	};

	/// @brief Playlist of songs and other variables.
	struct Playlist {
		jsonifier::vector<Song> songQueue{};///< The list of Songs that are stored to be played.
		bool isLoopSongEnabled{};///< Is looping of Songs currently enabled?
		bool isLoopAllEnabled{};///< Is looping of the entire Playlist currently enabled?
		Song currentSong{};///< The current Song that is playing.

		inline bool areThereAnySongs() {
			if (isLoopAllEnabled) {
				return songQueue.size() > 0 || currentSong.songId != "";
			} else if (isLoopSongEnabled) {
				return songQueue.size() > 0 || currentSong.songId != "";
			} else {
				return songQueue.size() > 0;
			}
		}

		inline bool sendNextSong() {
			if (isLoopSongEnabled) {
				if (songQueue.size() > 0 && currentSong.songId == "") {
					currentSong = songQueue.at(0);
					songQueue.erase(songQueue.begin());
					return true;
				} else if (songQueue.size() == 0 && currentSong.songId == "") {
					return false;
				} else {
					return true;
				}
			} else if (isLoopAllEnabled) {
				if (songQueue.size() > 0 && currentSong.songId == "") {
					currentSong = songQueue.at(0);
					songQueue.erase(songQueue.begin());
					return true;
				} else if (songQueue.size() > 0 && currentSong.songId != "") {
					Song tempSong02 = currentSong;
					currentSong		= songQueue.at(0);
					songQueue.erase(songQueue.begin());
					songQueue.emplace_back(tempSong02);
					return true;
				} else if (songQueue.size() == 0 && currentSong.songId == "") {
					return false;
				}
			} else {
				if (songQueue.size() > 0) {
					currentSong = songQueue.at(0);
					songQueue.erase(songQueue.begin());
					return true;
				} else if (songQueue.size() == 0) {
					currentSong = Song{};
					return false;
				}
			}
			return false;
		}

		inline void modifyQueue(uint64_t firstSongPosition, uint64_t secondSongPosition) {
			Song tempSong					 = songQueue.at(firstSongPosition);
			songQueue.at(firstSongPosition)	 = songQueue.at(secondSongPosition);
			songQueue.at(secondSongPosition) = tempSong;
		}
	};

	/**@}*/

	/**
	* \addtogroup utilities
	* @{
	*/

	struct SerializerValue {
		UnorderedMap<jsonifier::string, JsonStringValue> values{};
	};

	/// @brief Command data, for functions executed by the CommandController.
	class DiscordCoreAPI_Dll CommandData {
	  public:
		template<typename ValueType> friend struct EventData;
		friend struct OnInteractionCreationData;

		CommandData() = default;

		CommandData& operator=(CommandData&&) = default;

		CommandData(CommandData&&) = default;

		CommandData& operator=(const CommandData&);

		CommandData(const CommandData&);

		CommandData(const InputEventData& inputEventData);

		/// @brief Returns the Interaction data, if appplicable, of this input-event.
		/// @return InteractionData A Snowflake containing the Interaction data.
		InteractionData getInteractionData() const;

		/// @brief Returns the GuildMemberData of this input-event.
		/// @return GuildMemberData A GuildMemberData containing the GuildMemberData data.
		GuildMemberData getGuildMemberData() const;

		/// @brief Returns the ChannelData of this input-event.
		/// @return ChannelData A ChannelData containing the ChannelData data.
		ChannelData getChannelData() const;

		/// @brief Returns the Message data, if applicable, of this input-event.
		/// @return Message A Message structure.
		MessageData getMessageData() const;

		/// @brief Returns the UserData of this input-event.
		/// @return UserData A UserData containing the UserData data.
		UserData getUserData() const;

		/// @brief Returns the name of this entered command.
		/// @return jsonifier::string A string containing the name of this command.
		jsonifier::string getCommandName() const;

		/// @brief Returns the subcommand-name of this entered command.
		/// @return jsonifier::string A string containing the name of this sub-command.
		jsonifier::string getSubCommandName() const;

		/// @brief Returns the subcommand-name of this entered command group.
		/// @return jsonifier::string A string containing the name of this sub-command's group.
		jsonifier::string getSubCommandGroupName() const;

		/// @brief Returns the collection of command inputs/options for this command.
		/// @return SerializerValue A SerializerValue containing the arguments entered with this command.
		SerializerValue getCommandArguments() const;

		/// @brief Returns the InputEventData for this command.
		/// @return InputEventData An InputEventData containing the data associated  with this command.
		InputEventData getInputEventData() const;

		virtual ~CommandData() = default;

	  protected:
		jsonifier::string subCommandGroupName{};
		jsonifier::string subCommandName{};
		SerializerValue optionsArgs{};
		InputEventData eventData{};
		jsonifier::string commandName{};
	};

	/// @brief Base arguments for the command classes.
	struct DiscordCoreAPI_Dll BaseFunctionArguments : public CommandData {
		BaseFunctionArguments() = default;

		BaseFunctionArguments(const CommandData& commandData);
	};

	/// @brief Base class for the command classes.
	struct DiscordCoreAPI_Dll BaseFunction {
		jsonifier::string helpDescription{};///< Description of the command for the Help command.
		jsonifier::string commandName{};///< Name of the command for calling purposes.
		EmbedData helpEmbed{};///< A Message embed for displaying the command via the Help command.

		/// @brief The base function for the command's execute function.
		/// @param args A reference to an instance of BaseFunctionArguments.
		virtual void execute(BaseFunctionArguments& args) = 0;
		virtual UniquePtr<BaseFunction> create()		  = 0;
		virtual ~BaseFunction()							  = default;
	};

	struct MoveThroughMessagePagesData {
		InputEventData inputEventData{};
		jsonifier::string buttonId{};
		uint32_t currentPageIndex{};
	};

	DiscordCoreAPI_Dll MoveThroughMessagePagesData moveThroughMessagePages(jsonifier::string_view userID, InputEventData originalEvent, uint32_t currentPageIndex,
		const jsonifier::vector<EmbedData>& messageEmbeds, bool deleteAfter, uint32_t waitForMaxMs, bool returnResult);

	/**@}*/

	struct ReadyData {
		UnorderedSet<jsonifier::string> excludedKeys{};
		jsonifier::vector<jsonifier::string> trace{};
		jsonifier::string resumeGatewayUrl{};
		ApplicationData application{};
		jsonifier::string sessionId{};
		jsonifier::string shard{};
		UserData user{};
		uint64_t v{};
	};

}
