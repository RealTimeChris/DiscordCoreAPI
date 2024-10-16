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
/// FoundationEntities.hpp - Header for all of the discord/support api data structures.
/// May 12, 2021 Chris M.
/// https://discordcoreapi.com
/// \file FoundationEntities.hpp
#pragma once

#include <discordcoreapi/Utilities.hpp>
#include <optional>

namespace discord_core_api {

	namespace discord_core_internal {

		struct voice_connection_data {
			jsonifier::string sessionId{};
			jsonifier::string endPoint{};
			jsonifier::string token{};
		};

		struct DiscordCoreAPI_Dll websocket_resume_data {
			uint64_t lastNumberReceived{};
			jsonifier::string sessionId{};
			jsonifier::string botToken{};

			operator discord_core_internal::etf_serializer();
		};

		struct connect_properties {
			jsonifier::string_view browser{ "DiscordCoreAPI" };
			jsonifier::string_view device{ "DiscordCoreAPI" };
#if defined _WIN32
			jsonifier::string_view os{ "Windows" };
#elif __linux__
			jsonifier::string_view os{ "Linux" };
#else
			jsonifier::string_view os{ "MacOs" };
#endif
		};

		struct DiscordCoreAPI_Dll websocket_identify_data {
			update_presence_data presence{ presence_update_state::online };
			std::array<uint64_t, 2> shard{};
			connect_properties properties{};
			uint64_t largeThreshold{ 250 };
			jsonifier::string botToken{};
			int64_t intents{};

			operator discord_core_internal::etf_serializer();
		};

		struct voice_socket_protocol_payload_data_data {
			jsonifier::string address{};
			jsonifier::string mode{};
			uint16_t port{};
		};

		struct voice_socket_protocol_payload_data {
			voice_socket_protocol_payload_data_data data{};
			jsonifier::string_view protocol{ "udp" };
		};

		struct voice_identify_data {
			jsonifier::string sessionId{};
			jsonifier::string serverId{};
			jsonifier::string token{};
			snowflake userId{};
		};

		enum class send_speaking_type : uint8_t {
			None			   = 0,
			Microphone		   = 1 << 0,
			Soundshare		   = 1 << 1,
			Priority		   = 1 << 2,
			Priority_And_Voice = Microphone | Priority,
		};

		struct send_speaking_data {
			send_speaking_type type{};
			uint64_t delay{};
			uint64_t ssrc{};
		};

		enum class https_workload_class : uint8_t { Get = 0, Put = 1, Post = 2, Patch = 3, Delete = 4 };

		enum class payload_type : uint8_t { Application_Json = 1, Multipart_Form = 2 };

		enum class https_workload_type : uint8_t {
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
			YouTube_Get_Search_Results						= 168,
			SoundCloud_Get_Search_Results					= 169,
			SoundCloud_Get_Client_Id						= 170,
			SoundCloud_Get_Download_Links					= 171,
			Last											= 172
		};

		class DiscordCoreAPI_Dll https_workload_data {
		  public:
			friend class https_client;

			unordered_map<jsonifier::string, jsonifier::string> headersToInsert{};
			payload_type payloadType{ payload_type::Application_Json };
			https_workload_class workloadClass{};
			jsonifier::string relativePath{};
			jsonifier::string callStack{};
			jsonifier::string baseUrl{};
			jsonifier::string content{};

			https_workload_data() = default;

			https_workload_data& operator=(https_workload_data&& other) noexcept;
			https_workload_data(https_workload_data&& other) noexcept;

			https_workload_data& operator=(const https_workload_data& other) = delete;
			https_workload_data(const https_workload_data& other)			 = delete;

			https_workload_data& operator=(https_workload_type type);

			https_workload_data(https_workload_type type);

			https_workload_type getWorkloadType() const;

		  protected:
			https_workload_type workloadType{};
		};

		struct DiscordCoreAPI_Dll hello_data {
			hello_data() = default;
			jsonifier::vector<jsonifier::string> _trace{};
			uint64_t heartbeatInterval{};
		};

		struct websocket_message {
			std::optional<jsonifier::string> t{};
			std::optional<int64_t> s{};
			int64_t op{ -1 };
		};

		template<typename value_type> struct websocket_message_data {
			unordered_set<jsonifier::string> jsonifierExcludedKeys{};
			std::optional<jsonifier::string> t{};
			std::optional<int64_t> s{};
			value_type d{};
			int64_t op{};

			DCA_INLINE operator discord_core_internal::etf_serializer() {
				etf_serializer data{};
				data["op"] = op;
				if (s.has_value()) {
					data["s"] = s.value();
				}
				if (t.has_value()) {
					data["t"] = t.value();
				}
				data["d"]  = d.operator discord_core_internal::etf_serializer();
				return data;
			}
		};
	}

	template<typename value_type, typename value_type02> struct updated_event_data;
	template<typename value_type> struct event_data;

	/**
	 * \addtogroup foundation_entities
	 * @{
	*/

	struct connection_package {
		uint64_t currentReconnectTries{};
		uint32_t currentShard{};
		bool areWeResuming{};
	};

	/// @brief Timeout durations for the timeout command.
	enum class timeout_durations : uint16_t {
		None		 = 0,///< None - erase timeout.
		Minute		 = 1,///< 1 minute timeout.
		Five_Minutes = 5,///< 5 minute timeout.
		Ten_Minutes	 = 10,///< 10 minute timeout.
		Hour		 = 60,///< 1 hour timeout.
		Day			 = 1440,///< 1 day timeout.
		Week		 = 10080///< 1 week timeout.
	};

	/// @brief Role_data tags data.
	struct role_tags_data {
		jsonifier::string premiumSubscriber{};///< Are they a premium subscriber?
		snowflake integrationId{};///< What is the integration id?
		snowflake botId{};///< What is the bot id?
	};

	/// @brief User_data flags.
	enum class user_flags : uint32_t {
		Staff					 = 1 << 0,///< Discord employee.
		Partner					 = 1 << 1,///< Partnered server owner.
		Hypesquad				 = 1 << 2,///< Hype_squad events member.
		Bug_Hunter_Level_1		 = 1 << 3,///< Bug hunter level 1.
		Hypesquad_Online_House_1 = 1 << 6,///< House bravery member.
		Hypesquad_Online_House_2 = 1 << 7,///< House brilliance member.
		Hypesquad_Online_House_3 = 1 << 8,///< House balance member.
		Premium_Early_Suppoerter = 1 << 9,///< Early nitro supporter.
		Team_Pseudo_User		 = 1 << 10,///< User_data is a team.
		Bug_Hunter_Level_2		 = 1 << 14,///< Bug hunter level 2.
		Verified_Bot			 = 1 << 16,///< Verified bot.
		Verified_Developer		 = 1 << 17,///< Early verified bot developer.
		Certified_Moderator		 = 1 << 18,///< Discord certified moderator.
		Bot_Https_Interactions	 = 1 << 19,///< Bot uses only https interactions and is shown in the online member list.
		Bot						 = 1 << 20,///< Is it a bot?
		Mfaenabled				 = 1 << 21,///< Is mfa enabled?
		System					 = 1 << 22,///< Is it a system integration?
		Verified				 = 1 << 23///< Is it verified?
	};

	/// @brief Premium types denote the level of premium a user has.
	enum class premium_type : uint8_t {
		None		  = 0,///< None.
		Nitro_Classic = 1,///< Nitro classic.
		Nitro		  = 2///< Nitro.
	};

	template<typename value_type> class flag_entity {
	  public:
		template<jsonifier::concepts::enum_t flag_type> DCA_INLINE void setFlagValue(flag_type theFlagToSet, bool enabled) {
			using underlying_type = std::underlying_type_t<flag_type>;
			auto newValue		  = static_cast<underlying_type>(static_cast<value_type*>(this)->flags);
			if (enabled) {
				newValue |= static_cast<underlying_type>(theFlagToSet);
			} else {
				newValue &= ~static_cast<underlying_type>(theFlagToSet);
			}
			static_cast<value_type*>(this)->flags = static_cast<flag_type>(newValue);
		}

		template<jsonifier::concepts::enum_t flag_type> DCA_INLINE bool getFlagValue(flag_type theFlagToCheckFor) const {
			using underlying_type = std::underlying_type_t<flag_type>;
			return static_cast<underlying_type>(static_cast<const value_type*>(this)->flags) & static_cast<underlying_type>(theFlagToCheckFor);
		}

	  protected:
		DCA_INLINE flag_entity()  = default;
		DCA_INLINE ~flag_entity() = default;
	};


	enum class user_image_types {
		Banner			  = 0,
		Default_Avatar	  = 1,
		Avatar			  = 2,
		Avatar_Decoration = 3,
	};

	template<typename value_type> class get_user_image_url {
	  public:
		template<user_image_types type> DCA_INLINE jsonifier::string getUserImageUrl() const {
			jsonifier::string baseUrl{ "https://cdn.discordapp.com/" };
			switch (type) {
				case user_image_types::Banner: {
					baseUrl += "banners/" + static_cast<jsonifier::string>(static_cast<const value_type*>(this)->id) + "/" +
						static_cast<jsonifier::string>(static_cast<const value_type*>(this)->banner) + ".png";
					return baseUrl;
				}
				case user_image_types::Default_Avatar: {
					uint64_t index{};
					if (static_cast<jsonifier::string>(static_cast<const value_type*>(this)->discriminator) == "0") {
						index = (jsonifier::strToUint64(static_cast<jsonifier::string>(static_cast<const value_type*>(this)->id).data()) >> 22) % 6;
					} else {
						index = jsonifier::strToUint64(static_cast<jsonifier::string>(static_cast<const value_type*>(this)->discriminator).data()) % 5;
					}
					baseUrl += "embed/avatars/" + jsonifier::toString(index) + ".png";
					return baseUrl;
				}
				case user_image_types::Avatar: {
					baseUrl += "avatars/" + static_cast<jsonifier::string>(static_cast<const value_type*>(this)->id) + "/" +
						static_cast<jsonifier::string>(static_cast<const value_type*>(this)->avatar) + ".png";
					return baseUrl;
				}
				case user_image_types::Avatar_Decoration: {
					baseUrl += "avatar-decorations/" + static_cast<jsonifier::string>(static_cast<const value_type*>(this)->id) + "/" +
						static_cast<jsonifier::string>(static_cast<const value_type*>(this)->avatarDecoration) + ".png";
					return baseUrl;
				}
				default: {
					return {};
				}
			}
		}

	  protected:
		DCA_INLINE get_user_image_url()	 = default;
		DCA_INLINE ~get_user_image_url() = default;
	};

	enum class guild_member_image_types {
		Avatar = 0,
		Banner = 1,
	};

	template<typename value_type> class get_guild_member_image_url {
	  public:
		template<guild_member_image_types type> jsonifier::string getGuildMemberImageUrl() const {
			jsonifier::string baseUrl{ "https://cdn.discordapp.com/" };
			switch (type) {
				case guild_member_image_types::Avatar: {
					baseUrl += "guilds/" + static_cast<jsonifier::string>(static_cast<const value_type*>(this)->guildId) + "/users/" +
						static_cast<jsonifier::string>(static_cast<const value_type*>(this)->user.id) + "/avatars/" +
						static_cast<jsonifier::string>(static_cast<const value_type*>(this)->avatar) + ".png";
					return baseUrl;
				}
				case guild_member_image_types::Banner: {
					return baseUrl;
				}
				default: {
					return {};
				}
			}
		}

	  protected:
		DCA_INLINE get_guild_member_image_url()	 = default;
		DCA_INLINE ~get_guild_member_image_url() = default;
	};

	enum class guild_image_types {
		Icon			 = 0,
		Splash			 = 1,
		Discovery_Splash = 2,
		Banner			 = 3,
	};

	template<typename value_type> class get_guild_image_url {
	  public:
		template<guild_image_types type> jsonifier::string getGuildImageUrl() const {
			jsonifier::string baseUrl{ "https://cdn.discordapp.com/" };
			switch (type) {
				case guild_image_types::Icon: {
					baseUrl += "icons/" + static_cast<jsonifier::string>(static_cast<const value_type*>(this)->id) + "/" +
						static_cast<jsonifier::string>(static_cast<const value_type*>(this)->icon) + ".png";
					return baseUrl;
				}
				case guild_image_types::Splash: {
					baseUrl += "splashes/" + static_cast<jsonifier::string>(static_cast<const value_type*>(this)->id) + "/" +
						static_cast<jsonifier::string>(static_cast<const value_type*>(this)->splash) + ".png";
					return baseUrl;
				}
				case guild_image_types::Discovery_Splash: {
					baseUrl += "icons/" + static_cast<jsonifier::string>(static_cast<const value_type*>(this)->id) + "/" +
						static_cast<jsonifier::string>(static_cast<const value_type*>(this)->discoverySplash) + ".png";
					return baseUrl;
				}
				case guild_image_types::Banner: {
					baseUrl += "banners/" + static_cast<jsonifier::string>(static_cast<const value_type*>(this)->id) + "/" +
						static_cast<jsonifier::string>(static_cast<const value_type*>(this)->banner) + ".png";
					return baseUrl;
				}
				default: {
					return {};
				}
			}
		}

	  protected:
		DCA_INLINE get_guild_image_url()  = default;
		DCA_INLINE ~get_guild_image_url() = default;
	};

	/// @brief Attachment data.
	class attachment_data {
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
		snowflake id{};///< This attachment data's id.
	};

	/// @brief Sticker_data format types.
	enum class sticker_format_type : uint8_t {
		png	   = 1,///< Png.
		apng   = 2,///< Apng.
		lottie = 3///< Lottie
	};

	/// @brief Embed footer data.
	struct embed_footer_data {
		jsonifier::string proxyIconUrl{};///< Proxy icon url.
		jsonifier::string iconUrl{};///< Icon url.
		jsonifier::string text{};///< Footer text.
	};

	/// @brief Embed image data.
	struct embed_image_data {
		unordered_set<jsonifier::string> jsonifierExcludedKeys{};///< For excluding certain keys from parsing/serialization.
		jsonifier::string proxyUrl{};///< Proxy url.
		jsonifier::string url{};///< Image url.
		uint64_t height{};///< Image height.
		uint64_t width{};///< Image width.

		void generateExcludedKeys();
	};

	/// @brief Embed thumbnail data.
	struct embed_thumbnail_data {
		jsonifier::string proxyUrl{};///< Proxy url.
		jsonifier::string url{};///< Image url.
		uint64_t height{};///< Image height.
		uint64_t width{};///< Image width.
	};

	/// @brief Embed video data.
	struct embed_video_data {
		jsonifier::string proxyUrl{};///< Proxy url.
		jsonifier::string url{};///< Image url.
		uint64_t height{};///< Image height.
		uint64_t width{};///< Image width.
	};

	/// @brief Embed provider data.
	struct embed_provider_data {
		jsonifier::string name{};///< Name.
		jsonifier::string url{};///< Url.
	};

	/// @brief Embed author data.
	struct embed_author_data {
		unordered_set<jsonifier::string> jsonifierExcludedKeys{};///< For excluding certain keys from parsing/serialization.
		jsonifier::string proxyIconUrl{};///< Proxy icon url.
		jsonifier::string iconUrl{};///< Icon url.
		jsonifier::string name{};///< Name.
		jsonifier::string url{};///< Url.

		void generateExcludedKeys();
	};

	/// @brief Embed field data.
	struct embed_field_data {
		jsonifier::string value{};///< The text on the field.
		jsonifier::string name{};///< The title of the field.
		bool Inline{};///< Is the field DCA_INLINE with the rest of them?
	};

	/// @brief Embed types.
	enum class embed_type : uint8_t {
		Rich	= 0,///< Rich.
		Image	= 1,///< Image.
		Video	= 2,///< Video.
		Gifv	= 3,///< Gifv.
		Article = 4,///< Article.
		Link	= 5///< Link.
	};

	/// @brief Embed data.
	class DiscordCoreAPI_Dll embed_data {
	  public:
		unordered_set<jsonifier::string> jsonifierExcludedKeys{};///< For excluding certain keys from parsing/serialization.
		jsonifier::vector<embed_field_data> fields{};///< Array of embed fields.
		embed_thumbnail_data thumbnail{};///< Embed thumbnail data.
		jsonifier::string description{};///< Description of the embed.
		embed_provider_data provider{};///< Embed provider data.
		embed_footer_data footer{};///< Embed footer data.
		embed_author_data author{};///< Embed author data.
		jsonifier::string title{};///< Title of the embed.
		uint64_t hexColorValue{};///< Hex color value of the embed.
		jsonifier::string type{};///< Type of the embed.
		embed_image_data image{};///< Embed image data.
		embed_video_data video{};///< Embed video data.
		jsonifier::string url{};///< Url for the embed.
		time_stamp timeStamp{};///< Timestamp to be placed on the embed.

		void generateExcludedKeys();

		/// @brief Sets the author's name and avatar for the embed.
		/// @param authorName the author's name.
		/// @param authorAvatarUrl the url to their avatar.
		/// @return embed_data& A reference to this embed_data instance.
		embed_data& setAuthor(const jsonifier::string& authorName, const jsonifier::string& authorAvatarUrl = "");

		/// @brief Sets the footer's values for the embed.
		/// @param footerText the footer's text.
		/// @param footerIconUrlText url to the footer's icon.
		/// @return embed_data& A reference to this embed_data instance.
		embed_data& setFooter(const jsonifier::string& footerText, const jsonifier::string& footerIconUrlText = "");

		/// @brief Sets the timeStamp on the embed.
		/// @param timeStamp the timeStamp to be set.
		/// @return embed_data& A reference to this embed_data instance.
		embed_data& setTimeStamp(const jsonifier::string& timeStamp);

		/// @brief Adds a field to the embed.
		/// @param name the title of the embed field.
		/// @param value the contents of the embed field.
		/// @param Inline is it DCA_INLINE with the rest of the fields on the embed?
		/// @return embed_data& A reference to this embed_data instance.
		embed_data& addField(const jsonifier::string& name, const jsonifier::string& value, bool Inline = true);

		/// @brief Sets the description (the main contents) of the embed.
		/// @param descriptionNew the contents of the description to set.
		/// @return embed_data& A reference to this embed_data instance.
		embed_data& setDescription(const jsonifier::string& descriptionNew);

		/// @brief Sets the color of the embed, by applying a hex-color value.
		/// @param hexColorValueNew a string containing a hex-number value (between 0x00 0xFFFFFF).
		/// @return embed_data& A reference to this embed_data instance.
		embed_data& setColor(const jsonifier::string& hexColorValueNew);

		/// @brief Sets the thumbnail of the embed.
		/// @param thumbnailUrl the url to the thumbnail to be used.
		/// @return embed_data& A reference to this embed_data instance.
		embed_data& setThumbnail(const jsonifier::string& thumbnailUrl);

		/// @brief Sets the title of the embed.
		/// @param titleNew a string containing the desired title.
		/// @return embed_data& A reference to this embed_data instance.
		embed_data& setTitle(const jsonifier::string& titleNew);

		/// @brief Sets the image of the embed.
		/// @param imageUrl the url of the image to be set on the embed.
		/// @return embed_data& A reference to this embed_data instance.
		embed_data& setImage(const jsonifier::string& imageUrl);
	};

	/// @brief Message reference data.
	struct message_reference_data {
		bool failIfNotExists{};///< Fail if the message doesn't exist?
		snowflake messageId{};///< snowflake of the message to reference.
		snowflake channelId{};///< snowflake of the channel_data that the referenced message was sent in.
		snowflake guildId{};///< snowflake of the guild that the referenced message was sent in.
	};

	enum class media_type : uint8_t { png = 0, gif = 1, jpeg = 2, mpeg = 3, mp3 = 4 };

	/// @brief Data representing a file to be sent via multipart-form data.
	struct file {
		jsonifier::string fileName{};///< The name of the file.
		jsonifier::string data{};///< The data of the file.
	};

	/// @brief The sorting order, for guild forum channels.
	enum class sort_order_types {
		Latest_Activity = 0,///< Sort forum posts by activity.
		Creation_Date	= 1///< Sort forum posts by creation time(from most recent to oldest).
	};

	/// @brief Channel_data types.
	enum class channel_type : uint8_t {
		Guild_Text			= 0,///< A text channel within a server.
		Dm					= 1,///< A direct message between users.
		Guild_Voice			= 2,///< A voice channel within a server.
		Group_DM			= 3,///< A direct message between multiple users.
		Guild_Category		= 4,///< An organizational category that contains up to 50 channels.
		Guild_Announcement	= 5,///< A channel that users can follow and crosspost into their own server (formerly news channels).
		Announcement_Thread = 10,///< A temporary sub-channel within a GUILD_ANNOUNCEMENT channel.
		Public_Thread		= 11,///< A temporary sub-channel within a GUILD_TEXT or GUILD_FORUM channel.
		Private_Thread		= 12,///< A temporary sub-channel within a GUILD_TEXT channel that is only viewable by those invited and those with the MANAGE_THREADS permission.
		Guild_Stage_Voice	= 13,///< A voice channel for hosting events with an audience.
		Guild_Directory		= 14,///< The channel in a hub containing the listed servers.
		Guild_Forum			= 15///< channel_data that can only contain threads.
	};

	/// @brief Meta data for a thread_data type of channel_data.
	struct thread_metadata_data {
		jsonifier::string archiveTimestamp{};///< (where applicable) the time at which this thread_data was archived.
		uint64_t autoArchiveDuration{};///< How int64_t before archiving this thread_data.
		bool invitable{};///< The id of the individual who archived this thread_data.
		bool archived{};///< Whether or not this thread_data is currently archived.
		bool locked{};///< Whether or not this thread_data is currently locked.
	};

	/// @brief Data for a single member of a thread_data.
	class thread_member_data {
	  public:
		jsonifier::string joinTimestamp{};///< The time at which the member joined this thread_data.
		snowflake userId{};///< The user_data's id.
		uint64_t flags{};///< Flags.
		snowflake id{};///< This threadmemberdata's id.

		DCA_INLINE thread_member_data() = default;
	};

	/// @brief Thread_data types.
	enum class thread_type : uint8_t {
		Guild_News_Thread	 = 10,///< Guild news thread_data.
		Guild_Public_Thread	 = 11,///< Guild public thread_data.
		Guild_Private_Thread = 12///< Guild protected thread_data.
	};

	/// @brief Voice state data.
	struct voice_state_data_light {
		snowflake channelId{};///< The channel_data id this user_data is connected to.
		snowflake guildId{};///< The guild id this voice state is for.
		snowflake userId{};///< The user_data id this voice state is for.

		virtual DCA_INLINE ~voice_state_data_light() = default;
	};

	struct voice_server_update_data {
		jsonifier::string endpoint{};
		snowflake guildId{};
		jsonifier::string token{};
	};

	/// @brief Voice state data.
	struct voice_state_data : public voice_state_data_light {
		jsonifier::string requestToSpeakTimestamp{};///< The time at which the user_data requested to speak.
		jsonifier::string sessionId{};///< The session id for this voice state.
		bool selfStream{};///< Whether this user_data is streaming using "go live".
		bool selfVideo{};///< Whether this user_data's camera is enabled.
		bool selfDeaf{};///< Whether this user_data is locally deafened.
		bool selfMute{};///< Whether this user_data is locally muted.
		bool suppress{};///< Whether this user_data is muted by the current user_data.
		bool deaf{};///< Whether this user_data is deafened by the server.
		bool mute{};///< Whether this user_data is muted by the server.
	};

	/// @brief Automatic thread_data archiving durations.
	enum class thread_auto_archive_duration : uint16_t {
		Shortest = 60,///< Shortest.
		Short	 = 1440,///< Short.
		Long	 = 4320,///< Long.
		Longest	 = 10080///< Longest.
	};

	enum class role_flags : uint8_t { mentionable = 1 << 0, managed = 1 << 1, hoist = 1 << 2 };

	/// @brief A single user_data.
	class DiscordCoreAPI_Dll user_data : public get_user_image_url<user_data>, public flag_entity<user_data> {
	  public:
		template<typename value_type> friend struct jsonifier::core;
		friend class get_user_image_url<user_data>;

		jsonifier::string avatarDecoration{};///< The user's avatar decoration hash.
		jsonifier::string discriminator{};///< The user's discord-tag.
		jsonifier::string globalName{};///< The user's display name, if it is set. for bots, this is the application name.
		jsonifier::string userName{};///< The user's username, not unique across the platform.
		jsonifier::string locale{};///< The user's chosen language option.
		jsonifier::string banner{};///< The user's banner hash.
		jsonifier::string avatar{};///< The user's avatar hash.
		jsonifier::string email{};///< The user's email.
		uint64_t accentColor{};///< The user's banner color encoded as an integer representation of hexadecimal color code.
		int32_t premiumType{};///< The type of nitro subscription on a user's account.
		int32_t publicFlags{};///< The public flags on a user's account.
		bool mfaEnabled{};///< Whether the user has two-factor enabled on their account.
		user_flags flags{};///< The flags on a user's account.
		bool verified{};///< Whether the email on this account has been verified.
		snowflake id{};///< The user's id.
		bool system{};///< Whether the user is an official discord system user (part of the urgent message system).
		bool bot{};///< Whether the user belongs to an oauth2 application.

		DCA_INLINE user_data() = default;

		user_data(snowflake);
	};

	/// @brief A single user_data.
	class DiscordCoreAPI_Dll user_cache_data : public get_user_image_url<user_cache_data>, public flag_entity<user_cache_data> {
	  public:
		template<typename value_type> friend struct jsonifier::core;
		friend class get_user_image_url<user_cache_data>;

		jsonifier::string avatarDecoration{};///< The user's avatar decoration hash.
		jsonifier::string discriminator{};///< The user's 4-digit discord-tag identify.
		jsonifier::string globalName{};///< The user's global name.
		jsonifier::string userName{};///< The user's username.
		jsonifier::string avatar{};///< The user's avatar hash.
		jsonifier::string banner{};///< The user's banner hash.
		premium_type premiumType{};///< The type of nitro subscription on a user's account.
		uint64_t accentColor{};///< The user's banner color encoded as an integer representation of hexadecimal color code.
		user_flags flags{};///< The public flags on a user's account.
		snowflake id{};///< This user's id.

		DCA_INLINE user_cache_data() = default;

		user_cache_data& operator=(user_data&& other) noexcept;
		user_cache_data(user_data&& other) noexcept;

		user_cache_data& operator=(const user_data& other);
		user_cache_data(const user_data& other);

		operator user_data();


		DCA_INLINE bool operator==(const snowflake& other) const {
			return id == other;
		}
	};

	/// a single role_data.
	class DiscordCoreAPI_Dll role_data : public flag_entity<role_data> {
	  public:
		jsonifier::string unicodeEmoji{};///< Role unicode emoji. (optional)
		permissions_parse permissions{};///< Permission bit set.
		jsonifier::string name{};///< Role name.
		jsonifier::string icon{};///< Role icon hash. (optional).
		role_tags_data tags{};///< Role tags object. (optional)
		uint32_t position{};///< Position of this role.
		bool mentionable{};///< Whether this role is mentionable.
		role_flags flags{};///< Role flags combined as a bitfield.
		uint32_t color{};///< Integer representation of hexadecimal color code.
		snowflake id{};///< Role id.
		bool managed{};///< Whether this role is managed by an integration.
		bool hoist{};///< Whether this role is pinned in the user listing.

		DCA_INLINE role_data() = default;

		role_data(snowflake);
	};

	/// @brief Data structure representing a single role_data.
	class DiscordCoreAPI_Dll role_cache_data : public flag_entity<role_cache_data> {
	  public:
		friend class guild_data;

		jsonifier::string unicodeEmoji{};///< Emoji representing the role_data.
		permissions permissionsVal{};///< The role_data's base guild permissions.
		jsonifier::string name{};///< The role_data's name.
		snowflake guildId{};///< The id of the guild that this role_data is from.
		uint32_t position{};///< Its position amongst the rest of the guild's roles.
		role_flags flags{};///< Role_data flags.
		uint32_t color{};///< The role_data's color.
		snowflake id{};///< This role's id.

		DCA_INLINE role_cache_data() = default;

		role_cache_data& operator=(role_data&& other) noexcept;
		role_cache_data(role_data&& other) noexcept;

		role_cache_data& operator=(const role_data&);
		role_cache_data(const role_data&);

		operator role_data();

		DCA_INLINE bool operator==(const snowflake& other) const {
			return id == other;
		}
	};

	/// @brief Data structure representing a single emoji.
	class DiscordCoreAPI_Dll partial_emoji_data {
	  public:
		friend class emoji_data;
		unordered_set<jsonifier::string> jsonifierExcludedKeys{};///< Excluding keys from parsing/serializing.
		jsonifier::string name{};///< What is its name?
		bool animated{};///< Is it animated?
		snowflake id{};///< This emoji's id.

		DCA_INLINE partial_emoji_data() = default;

		void generateExcludedKeys();
	};

	/// @brief Data structure representing a single emoji.
	class DiscordCoreAPI_Dll emoji_data : public partial_emoji_data {
	  public:
		unordered_set<jsonifier::string> jsonifierExcludedKeys{};///< Excluding keys from parsing/serializing.
		jsonifier::vector<role_data> roles{};///< Roles that are allowed to use this emoji.
		std::wstring unicodeName{ L"" };///< What is its unicode name?
		bool available{ true };///< Is it available to be used?
		bool requireColons{};///< Require colons to render it?
		user_data user{};///< User_data that created this emoji.
		bool managed{};///< Is it managed?

		void generateExcludedKeys();
	};

	enum class presence_update_flags : uint8_t {
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

	struct client_status {
		jsonifier::string desktop{};///< User_data's status set for an active desktop (windows, linux, mac) application session.
		jsonifier::string mobile{};///< User_data's status set for an active mobile (iOS, android) application session.
		jsonifier::string web{};///< User_data's status set for an active web (browser, bot account) application session.
	};

	/// @brief Presence update data.
	struct presence_update_data {
		jsonifier::vector<activity_data> activities{};///< Array of activity objects user_data's current activities.
		client_status clientStatus{};///< 	user_data's platform-dependent status
		jsonifier::string status{};///< Either "idle", "dnd", "online", or "offline".
		snowflake guildId{};///< snowflake id of the guild.
		user_data user{};///< User_data object user_data whose presence is being updated.
	};

	enum class guild_member_flags : uint8_t { Pending = 1 << 0, Deaf = 1 << 1, Mute = 1 << 2 };

	class user_id_base {
	  public:
		snowflake id{};

		DCA_INLINE user_id_base() = default;
	};

	class guild_data;
	class guild_member_cache_data;

	/// @brief Data structure representing a single guild_member_data.
	class DiscordCoreAPI_Dll guild_member_data : public flag_entity<guild_member_data>, public get_guild_member_image_url<guild_member_data> {
	  public:
		template<typename value_type> friend class get_guild_member_image_url;
		template<typename value_type> friend struct jsonifier::core;
		template<typename value_type> friend struct event_data;
		friend class guild_data;

		time_stamp communicationDisabledUntil{};///< Iso8601 timestamp when the user's timeout will expire.
		jsonifier::vector<snowflake> roles{};///< Array of role object ids.
		jsonifier::string premiumSince{};///< Iso8601 timestamp when the user started boosting the guild.
		permissions_parse permissions{};///< Total permissions of the member in the channel, including overwrites.
		jsonifier::string avatar{};///< The member's guild avatar hash.
		guild_member_flags flags{};///< Guild member flags represented as a bit set, defaults to 0.
		jsonifier::string nick{};///< This user's guild nickname.
		time_stamp joinedAt{};///< Iso8601 timestamp when the user joined the guild.
		snowflake guildId{};///< The guild that this member belongs to.
		user_data user{};///< The user this guild member represents.
		bool pending{};///< Whether the user has not yet passed the guild's membership screening requirements.
		bool deaf{};///< Whether the user is deafened in voice channels.
		bool mute{};///< Whether the user is muted in voice channels.

		DCA_INLINE guild_member_data() = default;

		voice_state_data_light getVoiceStateData();

		user_cache_data getUserData();
	};

	/// @brief Data structure representing a single guild_member_data.
	class DiscordCoreAPI_Dll guild_member_cache_data : public flag_entity<guild_member_cache_data>, public get_guild_member_image_url<guild_member_cache_data> {
	  public:
		template<typename value_type> friend struct jsonifier::core;
		template<typename value_type> friend struct std::hash;
		template<typename value_type> friend struct event_data;
		friend class guild_data;

		jsonifier::vector<snowflake> roles{};///< The guild roGuildMemberDatales that they have.
		permissions permissionsVal{};///< Their base-level permissions in the guild.
		guild_member_flags flags{};///< Guild_member_data flags.
		jsonifier::string nick{};///< Their nick/display name.
		time_stamp joinedAt{};///< When they joined the guild.
		user_id_base user{};///< The user id for this guild_member_data.
		snowflake guildId{};///< The current guild's id.
		icon_hash avatar{};///< This guild_member_data's guild avatar.

		guild_member_cache_data() = default;

		guild_member_cache_data& operator=(guild_member_data&& other) noexcept;
		guild_member_cache_data(guild_member_data&& other) noexcept;

		guild_member_cache_data& operator=(const guild_member_data&);
		guild_member_cache_data(const guild_member_data&);

		operator guild_member_data();

		voice_state_data_light getVoiceStateData();

		user_cache_data getUserData();

		DCA_INLINE guild_member_cache_data(uint64_t snowFlake) {
			user.id = snowFlake;
		}
	};

	/// @brief Permission_types overwrites types.
	enum class permission_overwrites_type : uint8_t {
		role_data = 0,///< Role_data.
		user_data = 1///< User_data.
	};

	/// @brief A permission_types overwrite, for a given channel.
	class over_write_data {
	  public:
		permission_overwrites_type type{};///< Role_data or user_data type.
		permissions_parse allow{};///< collection of permissions to allow.
		permissions_parse deny{};///< collection of permissions to deny.
		snowflake id{};///< This overwrite's id.
	};

	enum class channel_flags : uint8_t { nsfw = 1 << 0, managed = 1 << 1 };

	struct default_reaction_data {
		jsonifier::string emojiName{};///< The unicode character of the emoji.
		snowflake emojiId{};///< The id of a guild's custom emoji.
	};

	/// forum tags.
	struct forum_tag_data {
		jsonifier::string emojiName{};///< The unicode character of the emoji.
		jsonifier::string name{};///< The name of the tag(0 - 20 characters).
		snowflake emojiId{};///< The id of a guild's custom emoji.
		bool moderated{};///< Whether this tag can only be added to or removed from threads by a member with the MANAGE_THREADS permission.
		snowflake id{};///< snowflake of the tag.
	};

	/// forum layout types.
	enum class forum_layout_types {
		Not_set		 = 0,///< No default has been set for forum channel.
		List_View	 = 1,///< Display posts as a list.
		Gallery_View = 2,///< Display posts as a collection of tiles.
	};

	/// @brief A channel_data object.
	class DiscordCoreAPI_Dll channel_data : public flag_entity<channel_data> {
	  public:
		jsonifier::vector<over_write_data> permissionOverwrites{};///< Explicit permission overwrites for members and roles.
		jsonifier::vector<forum_tag_data> availableTags{};///< The set of tags that can be used in a GUILD_FORUM channel.
		default_reaction_data defaultReactionEmoji{};///< The emoji to show in the add reaction button on a thread in a G
		jsonifier::vector<snowflake> appliedTags{};///< The ids of the set of tags that have been applied to a thread in a GUILD_FORUM channel.UILD_FORUM channel.
		jsonifier::vector<user_data> recipients{};///< The recipients of the dm.
		uint32_t defaultThreadRateLimitPerUser{};/// the initial rate_limit_per_user to set on newly created threads in a channel.
		forum_layout_types defaultForumLayout{};///< The default forum layout view used to display posts in GUILD_FORUM channels. defaults to 0.
		uint32_t defaultAutoArchiveDuration{};///< Default duration, copied onto newly created threads, in minutes.
		thread_metadata_data threadMetadata{};///< Metadata in the case that this channel_data is a thread_data.
		jsonifier::string lastPinTimestamp{};///< When the last pinned message was pinned. this may be null in certain events.
		sort_order_types defaultSortOrder{};///< The default sort order type used to order posts in GUILD_FORUM channels. defaults to null.
		permissions_parse permissions{};///< computed permissions for the invoking user in the channel, including overwrites.
		jsonifier::string rtcRegion{};///< Voice region id for the voice channel, automatic when set to null.
		uint32_t totalMessageSent{};///< Number of messages ever sent in a thread, it's similar to messageCount on message creation.
		uint32_t rateLimitPerUser{};///< Amount of seconds a user has to wait before sending another message (0-21600); bots.
		uint32_t videoQualityMode{};///< The camera video quality mode of the voice channel, 1 when not present.
		thread_member_data member{};///< Thread_data member object for the current user, if they have joined the
		jsonifier::string topic{};///< The channel topic (0-4096 characters for GUILD_FORUM channels, 0-1024 characters for all others). thread.
		snowflake lastMessageId{};///< The id of the last message sent in this channel (or thread for GUILD_FORUM channels).
		snowflake applicationId{};///< Application id of the group dm creator if it is bot-created.
		jsonifier::string name{};///< The name of the channel (1-100 characters).
		jsonifier::string icon{};///< Icon hash of the group dm.
		uint32_t messageCount{};///< Number of messages (not including the initial message or deleted messages) in a thread.
		uint32_t memberCount{};///< An approximate count of users in a thread, stops counting at 50.
		channel_flags flags{};///< channel flags combined as a bitfiel
		snowflake parentId{};///< For guild channels: id of the parent category for a channel (each parent category can contain up to 50 channels).d.
		uint32_t userLimit{};///< The user limit of the voice channel.
		snowflake ownerId{};///< Id of the creator of the group dm or thread.
		snowflake guildId{};///< The id of the guild (may be missing for some channel objects received over gateway guild dispatches).
		uint32_t position{};///< Sorting position of the channel.
		channel_type type{};///< The type of channel.
		uint32_t bitrate{};///< The bitrate (in bits) of the voice channel.
		snowflake id{};///< The id of this channel.
		bool managed{};///< For group dm channels: whether the channel is managed by an application via the gdm.join oauth2 scope.
		bool nsfw{};///< Whether the channel is nsfw.

		DCA_INLINE channel_data() = default;

		channel_data(snowflake);

		jsonifier::string getIconUrl();
	};

	/// @brief Data structure representing a single channel_data.
	class DiscordCoreAPI_Dll channel_cache_data : public flag_entity<channel_cache_data> {
	  public:
		friend class guild_data;

		jsonifier::vector<over_write_data> permissionOverwrites{};///< Permission overwrites.
		channel_type type{ channel_type::Dm };///< The type of the channel_data.
		jsonifier::string topic{};///< channel_data topic.
		jsonifier::string name{};///< Name of the channel_data.
		uint32_t memberCount{};///< count of members active in the channel_data.
		snowflake parentId{};///< snowflake of the channel_data's parent channel_data/category.
		channel_flags flags{};///< Flags combined as a bitmask.
		uint32_t position{};///< The position of the channel_data, in the guild's channel_data list.
		snowflake ownerId{};///< snowflake of the channel_data's owner.
		snowflake guildId{};///< snowflake of the channel_data's guild, if applicable.
		snowflake id{};///< This channel's id.

		DCA_INLINE channel_cache_data() = default;

		channel_cache_data& operator=(channel_data&& other) noexcept;
		channel_cache_data(channel_data&& other) noexcept;

		channel_cache_data& operator=(const channel_data& snowFlake);
		channel_cache_data(const channel_data& snowFlake);

		/// @brief For converting this into a channel_data instance.
		operator channel_data();

		DCA_INLINE bool operator==(const snowflake& other) const {
			return id == other;
		}
	};

	/// @brief Data representing an active thread_data.
	struct active_threads_data {
		jsonifier::vector<thread_member_data> members{};
		jsonifier::vector<channel_data> threads{};
		bool hasMore{};
	};

	/// @brief Data representing an archived thread_data.
	struct archived_threads_data {
		jsonifier::vector<thread_member_data> members{};
		jsonifier::vector<channel_data> threads{};
		bool hasMore{};
	};

	/// @brief Application command-option types.
	enum class application_command_option_type : uint8_t {
		Sub_Command		  = 1,///< Sub-command.
		Sub_Command_Group = 2,///< Sub-command group.
		String			  = 3,///< Jsonifier::string.
		Integer			  = 4,///< Integer.
		Boolean			  = 5,///< Boolean.
		User_Data		  = 6,///< User_data.
		Channel_Data	  = 7,///< channel_data.
		Role_Data		  = 8,///< Role_data.
		Mentionable		  = 9,///< Mentionable.
		Number			  = 10,///< Number.
		Attachment		  = 11///< Attachment.
	};

	/// @brief Application command permission-types.
	enum class application_command_permission_type : uint8_t {
		Role_Data	 = 1,///< Role_data.
		User_Data	 = 2,///< User_data.
		Channel_Data = 3///< channel_data.
	};

	/// @brief Event types for auto-moderation.
	enum class event_type : uint8_t {
		Message_Send = 1,///< When a member sends or edits a message in the guild.
	};

	/// @brief Trigger types for auto-moderation.
	enum class trigger_type : uint8_t {
		Keyword		   = 1,///< check if content contains words from a user defined list of keywords.
		Spam		   = 3,///< check if content represents generic spam.
		Keyword_Preset = 4,///< check if content contains words from internal pre-defined wordsets.
		Mention_Spam   = 5///< check if content contains more unique mentions than allowed.
	};

	/// @brief Keyword preset types for auto-moderation.
	enum class keyword_preset_type : uint8_t {
		Profanity	   = 1,///< Words that may be considered forms of swearing or cursing.
		Sexual_Content = 2,///< Words that refer to sexually explicit behavior or activity
		Slurs		   = 3///< Personal insults or words that may be considered hate speech.
	};

	/// @brief Action types for auto-moderation.
	enum class action_type : uint8_t {
		Block_Message	   = 1,///< Blocks the content of a message according to the rule.
		Send_Alert_Message = 2,///< Logs user content to a specified channel.
		Timeout			   = 3///< Timeout user for a specified duration.
	};

	/// @brief Action metadata for auto-moderation-rules.
	struct action_meta_data {
		int64_t durationSeconds{};///< Timeout duration in seconds.
		snowflake channelId{};///< channel_data to which user content should be logged.
	};

	/// @brief Trigger metadata for auto-moderation-rules.
	struct trigger_meta_data {
		jsonifier::vector<jsonifier::string> regexPatterns{};///< Array of strings** keyword regular expression patterns which will be matched against content.
		jsonifier::vector<jsonifier::string> keywordFilter{};///< Substrings which will be searched for in content.keyword_filter	array of strings.
		jsonifier::vector<jsonifier::string> allowList{};///< Substrings which will be exempt from triggering the preset trigger type.
		jsonifier::vector<keyword_preset_type> presets{};///< The internally pre-defined wordsets which will be searched for in content.
		bool mentionRaidProtectionEnabled{};///< MENTION_SPAM whether to automatically detect mention raids.
		uint64_t mentionTotalLimit{};///< Total number of unique role and user mentions allowed per message (maximum of 50).
	};

	/// @brief For representing a single auto-moderation-rule-action.
	struct action_data {
		action_meta_data metadata{};///< Additional metadata needed during execution for this specific action type.
		action_type type{};///< The type of action.
	};

	/// @brief Represents an auto-moderation-rule.
	class auto_moderation_rule_data {
	  public:
		jsonifier::vector<snowflake> exemptChannels{};///< The channel ids that should not be affected by the rule(maximum of 50).
		jsonifier::vector<snowflake> exemptRoles{};///< The role ids that should not be affected by the rule(maximum of 20).
		jsonifier::vector<action_data> actions{};///< Actions which will execute when the rule is triggered.
		trigger_meta_data triggerMetaData{};///< The rule trigger metadata actions array of action objects the.
		trigger_type triggerType{};///< The rule trigger type.
		jsonifier::string name{};///< The rule name.
		event_type eventType{};///< The rule event type.
		snowflake creatorId{};///< The user which first created this rule.
		snowflake guildId{};///< The guild which this rule belongs to.
		bool enabled{};///< Whether the rule is enabled.
		snowflake id{};

		DCA_INLINE auto_moderation_rule_data() = default;
	};

	/// @brief Permissions data for an application_command_data.
	class application_command_permission_data {
	  public:
		application_command_permission_type type{ application_command_permission_type::Role_Data };///< The type of permission_types.
		bool permission{};///< Whether the permission_types is active or not.
		snowflake id{};
	};

	/// @brief Represents the permissions for accessing an application_command_data from within a guild.
	class guild_application_command_permissions_data {
	  public:
		jsonifier::vector<application_command_permission_data> permissions{};///< The permissions.
		snowflake applicationId{};///< The application's id.
		snowflake guildId{};///< The guild's id.
		snowflake id{};

		DCA_INLINE guild_application_command_permissions_data() = default;
	};

	/// @brief For updating/modifying a given channel's properties.
	struct update_channel_data {
		jsonifier::vector<over_write_data> permissionOverwrites{};
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
		channel_type type{};
		bool nsfw{};
	};

	/// @brief/// data structure representing a single reaction.
	class reaction_data {
	  public:
		guild_member_data member{};///< The guild_member_data who placed the reaction.
		snowflake channelId{};///< The id of the channel_data where it was placed.
		snowflake messageId{};///< The id of the message upon which it was placed.
		snowflake guildId{};///< The id of the guild where it was placed.
		snowflake userId{};///< The id of the user_data who placed the reaction.
		emoji_data emoji{};///< The emoji that was placed as a reaction.
		uint64_t count{};///< The number of times this particular emoji was placed as a reaction to the given message_data.
		snowflake id{};
		bool me{};///< Whether or not i (the bot) placed it.

		DCA_INLINE reaction_data() = default;
	};

	/// @brief Structure representing voice region data.
	struct voice_region_data {
		jsonifier::string name{};///< Name of the region.
		bool deprecated{};///< Whether this is a deprecated voice region(avoid switching to these).
		snowflake id{};///< Unique id for the region.
		bool optimal{};///< True for a single server that is closest to the current user_data's client.
		bool custom{};///< Whether this is a custom voice region(used for events / etc).
	};

	/// @brief Message activity types.
	enum class message_activity_type : uint8_t {
		Join		 = 1,///< Join.
		Spectate	 = 2,///< Spectate.
		Listen		 = 3,///< Listen.
		Join_Request = 5///< Join-request.
	};

	/// @brief Message activity data.
	struct message_activity_data {
		message_activity_type type{ message_activity_type::Join };///< Message activity type.
		jsonifier::string partyId{};///< Party id.
	};

	/// @brief Ban data.
	struct ban_data {
		bool failedDueToPerms{};///< Failed due to perms?
		jsonifier::string reason{};///< Reason for the ban.
		user_data user{};///< User_data that was banned.
	};

	/// @brief Team members object data.
	struct team_members_object_data {
		jsonifier::vector<permissions_parse> permissions{};///< Permissions for the team.
		uint64_t membershipState{};///< current state.
		snowflake teamId{};///< snowflake of the current team.
		user_data user{};///< User_data data of the current user_data.
	};

	/// @brief For updating the current voice state.
	struct update_voice_state_data {
		snowflake channelId{};///< snowflake of the desired voice channel_data. leave blank to disconnect.
		snowflake guildId{};///< The id of the guild fo which we would like to establish a voice connection.
		bool selfMute{};///< Whether or not we self-mute ourselves.
		bool selfDeaf{};///< Whether or not we self-deafen ourselves.

		operator discord_core_internal::etf_serializer();
	};

	/// @brief For updating the current voice state.
	struct update_voice_state_data_dc {
		std::nullptr_t channelId{ nullptr };///< snowflake of the desired voice channel_data. leave blank to disconnect.
		snowflake guildId{};///< The id of the guild fo which we would like to establish a voice connection.
		bool selfDeaf{};///< Whether or not we self-deafen ourselves.
		bool selfMute{};///< Whether or not we self-mute ourselves.

		operator discord_core_internal::etf_serializer();
	};

	/// @brief Team object data.
	class team_object_data {
	  public:
		jsonifier::vector<team_members_object_data> members{};///< Array of team members object data.
		snowflake ownerUserId{};///< User_data id of the team owner.
		jsonifier::string icon{};///< Icon for the team.
		snowflake id{};

		DCA_INLINE team_object_data() = default;
	};

	/// application flags, for the application_data structure.
	enum class application_flags : uint32_t {
		Gateway_Presence				 = 1 << 12,///< Intent required for bots in 100 or more servers to receive presence_update events.
		Gateway_Presence_Limited		 = 1 << 13,///< Intent required for bots in under 100 servers to receive presence_update events, found in bot settings.
		Gateway_Guild_Members			 = 1 << 14,///< Intent required for bots in 100 or more servers to receive member-related events like guild_member_add.
		Gateway_Guild_Members_Limited	 = 1 << 15,///< Intent required for bots in under 100 servers to receive member-related events like guild_member_add, found in bot settings.
		Verificatino_Pending_Guild_Limit = 1 << 16,///< Indicates unusual growth of an app that prevents verification
		embedded						 = 1 << 17,///< Indicates if an app is embedded within the discord client (currently unavailable publicly)
		Gateway_Message_Content			 = 1 << 18,///< Intent required for bots in 100 or more servers to receive message content
		Gateway_Message_Content_Limited	 = 1 << 19///< Intent required for bots in under 100 servers to receive message content, found in bot settings};
	};

	/// @brief Install params data, for application data.
	struct install_params_data {
		jsonifier::vector<jsonifier::string> scopes{};///< The scopes to add the application to the server with.
		permissions_parse permissions{};///< The permissions to request for the bot role.
	};

	/// @brief Application data.
	class application_data {
	  public:
		jsonifier::vector<jsonifier::string> rpcOrigins{};///< Array of rpc origin strings.
		jsonifier::vector<jsonifier::string> tags{};///< Up to 5 tags describing the content and functionality of the application install_params.
		jsonifier::string termsOfServiceUrl{};///< Terms of service url.
		jsonifier::string privacyPolicyUrl{};///< Privacy policy url.
		jsonifier::string customInstallUrl{};
		jsonifier::string primarySkuId{};///< Primary sku snowflake.
		jsonifier::string description{};///< Description of the application.
		jsonifier::string coverImage{};///< The cover image.
		jsonifier::string verifyKey{};///< The verification key.
		install_params_data params{};///< Settings for the application's default in-app authorization link, if enabled.
		jsonifier::string summary{};///< Summary of the application.
		bool botRequireCodeGrant{};///< Does the bot require a code grant?
		application_flags flags{};///< Application flags.
		jsonifier::string slug{};///< Sluhg.
		jsonifier::string name{};///< Application's name.
		jsonifier::string icon{};///< Application's icon.
		team_object_data team{};///< Team object data.
		snowflake guildId{};///< Guild id.
		user_data owner{};///< Application's owner.
		bool botPublic{};///< Is the bot public?
		snowflake id{};

		DCA_INLINE application_data() = default;
	};

	/// @brief Authorization info structure.
	struct authorization_info_data {
		jsonifier::vector<jsonifier::string> scopes{};///< Array of strings - the scopes the user_data has authorized the application for.
		application_data application{};///< Partial application object the current application.
		jsonifier::string expires{};///< When the access token expires.
		user_data user{};/// the user_data who has authorized, if the user_data has authorized with the identify scope.
	};

	/// @brief Account data.
	class account_data {
	  public:
		jsonifier::string name{};///< Name of the account.
		snowflake id{};

		DCA_INLINE account_data() = default;
	};

	/// @brief Guild widget data.
	struct guild_widget_data {
		snowflake channelId{};///< The widget channel_data id.
		bool enabled{};///< Whether the widget is enabled.
	};

	/// @brief Get guild widget data.
	struct get_guild_widget_object_data {
		jsonifier::vector<channel_data> channels{};///< Voice and stage channels which are accessible by everyone.
		jsonifier::vector<user_data> members{};///< Special widget user objects that includes users presence (limit 100).
		jsonifier::string instantInvite{};///< Instant invite for the guilds specified widget invite channel.
		uint64_t presenceCount{};///< Number of online members in this guild.
		jsonifier::string name{};///< Guild name (2-100 characters).
		snowflake id{};
	};

	/// @brief Widget style options.
	enum class widget_style_options : uint8_t {
		Shield	= 0,///< Shield
		Banner1 = 1,///< Banner1
		Banner2 = 2,///< Banner2
		Banner3 = 3,///< Banner3
		Banner4 = 4///< Banner4
	};

	/// @brief Guild widget image data.
	struct guild_widget_image_data {
		jsonifier::string url{};
	};

	/// @brief Integration data.
	class integration_data {
	  public:
		application_data application{};///< Application data.
		jsonifier::string syncedAt{};///< Time it was last synced at.
		bool enableEmoticons{ true };///< Emoticons enabled?
		uint64_t expireGracePeriod{};///< How int64_t before the integration expires.
		uint64_t subscriberCount{};///< Number of current subscribers.
		uint64_t expireBehavior{};///< What to do upon expiry.
		jsonifier::string name{};///< Name of the integration.
		jsonifier::string type{};///< Type of integration.
		account_data account{};///< Account data.
		snowflake roleId{};///< Role_data snowflake.
		user_data user{};///< User_data data for the integration.
		bool revoked{};///< Has it been revoked?
		bool enabled{};///< Enabled?
		bool syncing{};///< Is it syncing?
		snowflake id{};

		DCA_INLINE integration_data() = default;
	};

	/// @brief Audit log events.
	enum class audit_log_event : uint8_t {
		Guild_Update						  = 1,///< Guild update.
		Channel_Create						  = 10,///< channel_data create.
		Channel_Update						  = 11,///< channel_data update.
		Channel_Delete						  = 12,///< channel_data delete.
		Channel_Overwrite_Create			  = 13,///< channel_data overwrite create.
		Channel_Overwrite_Update			  = 14,///< channel_data overwrite update.
		Channel_Overwrite_Delete			  = 15,///< channel_data overwrite delete.
		Member_Kick							  = 20,///< Member kick.
		Member_Prune						  = 21,///< Member prune.
		Member_Ban_Add						  = 22,///< Member ban add.
		Member_Ban_Remove					  = 23,///< Member ban erase.
		Member_Update						  = 24,///< Member update.
		Member_Role_Update					  = 25,///< Member role update.
		Member_Move							  = 26,///< Member move.
		Member_Disconnect					  = 27,///< Member disconnect.
		Bot_Add								  = 28,///< Bot add.
		Role_Create							  = 30,///< Role_data create.
		Role_Update							  = 31,///< Role_data update.
		Role_Delete							  = 32,///< Role_data delete.
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
		Stage_Instance_Create				  = 83,///< Stage-instance create.
		Stage_Instance_Update				  = 84,///< Stage-instance update.
		Stage_Instance_Delete				  = 85,///< Stage-instance delete.
		Sticker_Create						  = 90,///< Sticker_data create.
		Sticker_Update						  = 91,///< Sticker_data update.
		Sticker_Delete						  = 92,///< Sticker_data delete.
		Guild_Scheduled_Event_Create		  = 100,///< Guild-scheduled-event create.
		Guild_Scheduled_Event_Update		  = 101,///< Guild-scheduled-event update.
		Guild_Scheduled_Event_Delete		  = 102,///< Guild-scheduled-event delete.
		Thread_Create						  = 110,///< Thread_data create.
		Thread_Update						  = 111,///< Thread_data update.
		Thread_Delete						  = 112,///< Thread_data delete.
		Application_Command_Permission_Update = 121,///< Permissions were updated for a command.
		Auto_Moderation_Rule_Create			  = 140,///< Auto moderation rule was created.
		Auto_Moderation_Rule_Update			  = 141,///< Auto moderation rule was updated.
		Auto_Moderation_Rule_Delete			  = 142,///< Auto moderation rule was deleted.
		Auto_Moderation_Block_Message		  = 143///< Message was blocked by auto_mod (according to a rule).
	};

	/// audit log entry info data @brief Audit log entry info data.
	class optional_audit_entry_info_data {
	  public:
		jsonifier::string deleteMemberDays{};///< Number of days for which the member's messages were deleted.
		jsonifier::string membersRemoved{};///< Number of members that were removed upon a prune.
		jsonifier::string roleName{};///< Role_data name.
		snowflake applicationId{};///< Id of the app whose permissions were targeted APPLICATION_COMMAND_PERMISSION_UPDATE.
		jsonifier::string count{};///< count.
		jsonifier::string type{};///< Type.
		snowflake channelId{};///< channel_data snowflake.
		snowflake messageId{};///< Message snowflake.
		snowflake id{};

		DCA_INLINE optional_audit_entry_info_data() = default;
	};

	/// @brief Audit log change data.
	struct audit_log_change_data {
		jsonifier::string newValue{};///< New value.
		jsonifier::string oldValue{};///< Old value.
		jsonifier::string key{};///< The key of the audit log change.
	};

	/// @brief Guild prune count data.
	struct guild_prune_count_data {
		uint64_t count{};
	};

	/// @brief Audit log entry data.
	class audit_log_entry_data {
	  public:
		jsonifier::vector<audit_log_change_data> changes{};///< Array of audit log change data.
		optional_audit_entry_info_data options{};///< Audit log entry info data.
		time_stamp createdTimeStamp{};///< Time at which this entry was created.
		audit_log_event actionType{};///< Audit log action type.
		jsonifier::string reason{};///< The reason that was entered for the given change.
		snowflake targetId{};///< snowflake of the target user_data.
		snowflake userId{};///< snowflake of the executing user_data.
		snowflake id{};
	};

	/// @brief Premium tier levels.
	enum class premium_tier : uint8_t {
		None   = 0,///< None.
		Tier_1 = 1,///< Tier 1.
		Tier_2 = 2,///< Tier 2.
		Tier_3 = 3///< Tier 3.
	};

	/// @brief Default message notification
	/// levels.
	enum class default_message_notification_level : uint8_t {
		All_Messages  = 0,///< All messages.
		Only_Mentions = 1///< Only mentions.
	};

	/// @brief Explicit content filter levels.
	enum class explicit_content_filter_level : uint8_t {
		Disabled			  = 0,///< Disabled.
		Members_Without_Roles = 1,///< Members without roles.
		All_Members			  = 2///< All members.
	};

	/// @brief Mfa levels.
	enum class mfalevel : uint8_t {
		None	 = 0,///< None.
		Elevated = 1///< Elevated.
	};

	/// @brief/// verification levels.
	enum class verification_level : uint8_t {
		None	  = 0,///< None.
		Low		  = 1,///< Low.
		Medium	  = 2,///< Medium.
		High	  = 3,///< High.
		Very_High = 4///< Very high.
	};

	/// @brief Welcome screen channel_data data.
	struct welcome_screen_channel_data {
		jsonifier::string description{};///< Description of the welcome channel_data.
		jsonifier::string emojiName{};///< Emoji name for the channel_data.
		snowflake channelId{};///< snowflake of the welcome channel_data.
		snowflake emojiId{};///< Emoji id for the channel_data.
	};

	/// @brief Welcome screen data.
	struct welcome_screen_data {
		jsonifier::vector<welcome_screen_channel_data> welcomeChannels{};///< Welcome screen channel_data data.
		jsonifier::string description{};///< Description of the welcome screen.
	};

	/// @brief Stage instance privacy levels.
	enum class stage_instance_privacy_level : uint8_t {
		Public	   = 1,///< Public.
		Guild_Only = 2///< Guild only.
	};

	/// @brief Stage instance data.
	class stage_instance_data {
	  public:
		stage_instance_privacy_level privacyLevel{};///< Privacy level of the channel_data.
		bool discoverableDisabled{};///< Is it discoverable?
		jsonifier::string topic{};///< The topic of the stage_instance_data.
		snowflake channelId{};///< The channel_data's id.
		snowflake guildId{};///< The guild id for which the channel_data exists in.
		snowflake id{};

		DCA_INLINE stage_instance_data() = default;
	};

	/// @brief Sticker_data types.
	enum class sticker_type : uint8_t {
		Standard = 1,///< Standard.
		Guild	 = 2///< Guild.
	};

	enum class sticker_flags : uint8_t { Available = 1 << 0 };

	/// @brief Data representing a single sticker_data.
	class sticker_data {
	  public:
		jsonifier::string description{};///< Description of the sticker_data.
		sticker_format_type formatType{};///< Format type.
		jsonifier::string packId{};///< Pack id of the sticker_data.
		jsonifier::string asset{};///< Asset value for the sticker_data
		jsonifier::string name{};///< The sticker_data's name.
		jsonifier::string tags{};///< Tags for the sticker_data to use.
		sticker_flags flags{};///< Sticker_data flags.
		uint64_t nsfwLevel{};///< Nsfw warning level.
		uint64_t sortValue{};///< Where in the stack of stickers it resides.
		snowflake guildId{};///< The guild id for which the sticker_data exists in.
		sticker_type type{};///< The type of sticker_data.
		user_data user{};///< The user_data that uploaded the guild sticker_data.
		snowflake id{};

		DCA_INLINE sticker_data() = default;
	};

	/// @brief Data representing a single guild preview.
	struct guild_preview_data {
		jsonifier::vector<jsonifier::string> features{};
		jsonifier::vector<sticker_data> stickers{};
		jsonifier::vector<emoji_data> emojis{};
		jsonifier::string discoverySplash{};
		uint64_t approximatePresenceCount{};
		uint64_t approximateMemberCount{};
		jsonifier::string description{};
		jsonifier::string splash{};
		jsonifier::string name{};
		jsonifier::string icon{};
		snowflake id{};
	};

	/// @brief Afk timeout durations.
	enum class afk_time_out_durations : uint16_t {
		Shortest = 60,///< Shortest.
		Short	 = 300,///< Short.
		Medium	 = 900,///< Medium.
		Long	 = 1800,///< Long.
		Longest	 = 3600///< Longest.
	};

	/// @brief Guild nsfw level.
	enum class guild_nsfwlevel : uint8_t {
		Default		   = 0,///< Default.
		Explicit	   = 1,///< Explicit.
		Safe		   = 2,///< Safe.
		Age_Restricted = 3///< Age restricted.
	};

	/// @brief System channel flags.
	enum class system_channel_flags : uint8_t {
		Suppress_Join_Notifications			  = 1 << 0,///< Suppress member join notifications.
		Suppress_Premium_Subscriptions		  = 1 << 1,///< Suppress server boost notifications.
		Suppress_Guild_Reminder_Notifications = 1 << 2,///< Suppress server setup tips.
		Suppress_Join_Notification_Replies	  = 1 << 3///< Hide member join sticker reply buttons.
	};

	class guild_cache_data;
	class guild;

	/// @brief Guild scheduled event privacy levels.
	enum class guild_scheduled_event_privacy_level : uint8_t {
		Public	   = 1,///< Public.
		Guild_Only = 2///< Guild only.
	};

	/// @brief Guild_scheduled_event_status.
	enum class guild_scheduled_event_status : uint8_t {
		Scheduled = 1,///< Scheduled.
		Active	  = 2,///< Active.
		Completed = 3,///< completed.
		Canceled  = 4///< cancelled.
	};

	/// @brief Guild scheduled event entity types.
	enum class guild_scheduled_event_entity_type : uint8_t {
		None		   = 0,///< None.
		State_Instance = 1,///< Stage instance.
		Voice		   = 2,///< Voice.
		External	   = 3///< External.
	};

	/// @brief Guild scheduled event entity metadata.
	struct guild_scheduled_event_metadata {
		jsonifier::string location{};
	};

	/// @brief Data representing a guild scheduled event.
	class guild_scheduled_event_data {
	  public:
		guild_scheduled_event_privacy_level privacyLevel{};///< The privacy level of the scheduled event.
		guild_scheduled_event_metadata entityMetadata{};///< Additional metadata for the guild scheduled event.
		guild_scheduled_event_entity_type entityType{};///< The type of the scheduled event.
		jsonifier::string scheduledStartTime{};///< The time the scheduled event will start.
		jsonifier::string scheduledEndTime{};///< The time the scheduled event will end, required if entity_type is external.
		guild_scheduled_event_status status{};///< The status of the scheduled event.
		jsonifier::string description{};///< The description of the scheduled event(1 - 1000 characters.
		jsonifier::string name{};///< The name of the scheduled event(1 - 100 characters).
		snowflake creatorId{};///< The id of the user_data that created the scheduled event *.
		snowflake channelId{};///< The channel_data id in which the scheduled event will be hosted, or null if scheduled entity type is external.
		snowflake entityId{};///< The id of an entity associated with a guild scheduled event.
		uint32_t userCount{};///< The number of users subscribed to the scheduled event.
		snowflake guildId{};///< The guild id which the scheduled event belongs to.
		user_data creator{};///< The user_data that created the scheduled event.
		snowflake id{};

		DCA_INLINE guild_scheduled_event_data() = default;
	};

	/// @brief Data representing a single guild_scheduled_event_user.
	struct guild_scheduled_event_user_data {
		snowflake guildScheduledEventId{};///< The scheduled event id which the user_data subscribed to/
		guild_member_data member{};///< Guild member data for this user_data for the guild which this event belongs to, if any.
		user_data user{};///< User_data which subscribed to an event.
	};

	/// @brief Invite target types.
	enum class invite_target_types : uint8_t {
		Stream				 = 1,///< Stream.
		Embedded_Application = 2///< Embedded application.
	};

	/// @brief Web_hook_data types.
	enum class web_hook_type : uint8_t {
		Incoming		 = 1,///< Incoming.
		Channel_Follower = 2,///< channel_data follower.
		Application		 = 3///< Application.
	};

	/// @brief For removing a reaction.
	struct reaction_remove_data {
		snowflake channelId{};
		snowflake messageId{};
		snowflake guildId{};
		snowflake userId{};
		emoji_data emoji{};
	};

	/// @brief For storing interaction-related values.
	struct interaction_package_data {
		jsonifier::string interactionToken{};
		snowflake applicationId{};
		snowflake interactionId{};
	};

	/// @brief For storing message_data-related values.
	struct message_package_data {
		snowflake channelId{};
		snowflake messageId{};
	};

	/// @brief Data structure representing an application_command_data's option choice.
	class application_command_option_choice_data {
	  public:
		unordered_map<jsonifier::string, jsonifier::string> nameLocalizations{};///< Dictionary with keys in available locales localization dictionary for the name.
		unordered_set<jsonifier::string> jsonifierExcludedKeys{};
		jsonifier::raw_json_data value{};///< The value of the option.
		jsonifier::string name{};///< The name of the current choice.

		void generateExcludedKeys();

		bool operator==(const application_command_option_choice_data&) const;
	};

	/// @brief Data structure representing an application_command_data's option.
	struct application_command_option_data {
		unordered_map<jsonifier::string, jsonifier::string> descriptionLocalizations{};///< Dictionary for the description localizations field.
		unordered_map<jsonifier::string, jsonifier::string> nameLocalizations{};///< Dictionary for the name localizations field.
		jsonifier::vector<application_command_option_choice_data> choices{};///< A vector of possible choices for the current application_command_data option.
		jsonifier::vector<application_command_option_data> options{};///< A vector of possible options for the current application_command_data option.
		int64_t minValue{ std::numeric_limits<int64_t>::max() };///< If the option is an integer or number type, the minimum value permitted.
		int64_t maxValue{ std::numeric_limits<int64_t>::min() };///< If the option is an integer or number type, the maximum value permitted.
		unordered_set<jsonifier::string> jsonifierExcludedKeys{};
		jsonifier::vector<channel_type> channelTypes{};///< Set when the application_command_data option type is set to channel_data.
		application_command_option_type type{};///< The type of command option.
		jsonifier::string description{};///< A description of the current application_command_data option.
		jsonifier::string name{};///< Name of the current application_command_data option.
		bool autocomplete{};///< If autocomplete interactions are enabled for this string, integer, or number type option.
		bool required{};///< If the parameter is required or optional-- default false.

		void generateExcludedKeys();

		bool operator==(const application_command_option_data&) const;
	};

	/// @brief Representing "typing_start" data.
	struct typing_start_data {
		guild_member_data member{};
		time_stamp timeStamp{};
		snowflake channelId{};
		snowflake guildId{};
		snowflake userId{};
	};

	struct you_tube_format {
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
	enum class application_command_type : uint8_t {
		Chat_Input = 1,///< chat input.
		User_Data  = 2,///< User_data.
		Message	   = 3///< Message.
	};

	/// @brief Component types.
	enum class component_type : uint8_t {
		Action_Row		   = 1,///< container for other components.
		Button			   = 2,///< Button object.
		String_Select	   = 3,///< Select menu for picking from defined text options.
		Text_Input		   = 4,///< Text input object.
		User_Select		   = 5,///< Select menu for users.
		Role_Select		   = 6,///< Select menu for roles.
		Mentionable_Select = 7,///< Select menu for mentionables (users and roles).
		Channel_Select	   = 8,///< Select menu for channels.
	};

	enum class select_menu_type : uint8_t { String_Select = 3, User_Select = 5, Role_Select = 6, Mentionable_Select = 7, Channel_Select = 8 };

	enum class select_default_value_type { user = 0, role = 1, channel = 2 };

	class select_default_value_data {
	  public:
		DCA_INLINE select_default_value_data() = default;

		DCA_INLINE select_default_value_data(select_default_value_type typeNew) {
			switch (typeNew) {
				case select_default_value_type::user: {
					type = "user";
					break;
				}
				case select_default_value_type::role: {
					type = "role";
					break;
				}
				case select_default_value_type::channel: {
					type = "channel";
					break;
				}
			}
		}
		jsonifier::string type{};
		snowflake id{};
	};

	/// @brief Allowable mentions for a message.
	class allowed_mentions_data {
	  public:
		jsonifier::vector<jsonifier::string> parse{};///< A vector of allowed mention types to parse from the content.
		jsonifier::vector<jsonifier::string> roles{};///< Array of role_ids to mention (max size of 100)
		jsonifier::vector<jsonifier::string> users{};///< Array of user_ids to mention (max size of 100)
		bool repliedUser{};///< For replies, whether to mention the author of the message being replied to (default false).
	};

	/// @brief Interaction types.
	enum class interaction_type : uint8_t {
		Ping							 = 1,///< Ping.
		Application_Command				 = 2,///< Application command.
		Message_Component				 = 3,///< Message component.
		Application_Command_Autocomplete = 4,///< Application command autocomplete.
		Modal_Submit					 = 5///< Modal submission.
	};

	/// @brief Represents a single selection from a select-menu.
	class select_option_data {
	  public:
		jsonifier::string description{};///< Description of the select-menu-option.
		partial_emoji_data emoji{};///< An optional emoji to put on it.
		jsonifier::string label{};///< A visible label for the select-menu-option.
		jsonifier::string value{};///< A value for identifying the option.
		bool _default{};///< Is it the default option?
	};

	/// @brief Button styles.
	enum class button_style : uint8_t {
		Primary	  = 1,///< Primary.
		Success	  = 3,///< Success.
		Secondary = 2,///< Secondary.
		Danger	  = 4,///< Danger.
		Link	  = 5///< Link.
	};

	struct DiscordCoreAPI_Dll component_data {
		jsonifier::vector<select_default_value_data> defaultValues{};
		unordered_set<jsonifier::string> jsonifierExcludedKeys{};
		jsonifier::vector<select_option_data> options{};
		jsonifier::vector<channel_type> channelTypes{};
		jsonifier::string placeholder{};
		jsonifier::string customId{};
		partial_emoji_data emoji{};
		jsonifier::string label{};
		jsonifier::string value{};
		jsonifier::string title{};
		jsonifier::string url{};
		component_type type{};
		uint64_t minValues{};
		uint64_t maxValues{};
		uint64_t minLength{};
		uint64_t maxLength{};
		uint64_t style{};
		bool disabled{};
		bool required{};

		void generateExcludedKeys();
	};

	/// @brief Action row data of message components.
	class action_row_data {
	  public:
		component_type type{ component_type::Action_Row };
		jsonifier::vector<component_data> components{};///< Array of components to make up the action-row.
		unordered_set<jsonifier::string> jsonifierExcludedKeys{};

		void generateExcludedKeys();
	};

	/// @brief Interaction callback types.
	enum class interaction_callback_type : uint8_t {
		Pong									= 1,///< Ack a ping.
		Channel_Message_With_Source				= 4,///< Respond to an interaction with a message.
		Deferred_Channel_Message_With_Source	= 5,///< Ack an interaction and edit a response later, the user sees a loading state.
		Deferred_Update_Message					= 6,///< For components, ack an interaction and edit the original message later; the user does not see a loading state.
		Update_Message							= 7,///< For components, edit the message the component was attached to.
		Application_Command_Autocomplete_Result = 8,///< Respond to an autocomplete interaction with suggested choices.
		Modal									= 9///< Respond to an interaction with a popup modal.
	};

	/// @brief Interaction application_command_data callback data.
	struct DiscordCoreAPI_Dll interaction_callback_data {
		jsonifier::vector<application_command_option_choice_data> choices{};///< Autocomplete choices(max of 25 choices).
		jsonifier::vector<attachment_data> attachments{};///< Array of partial attachment objects attachment objects with filename and description.
		unordered_set<jsonifier::string> jsonifierExcludedKeys{};///< Keys to exclude from parsing/serializing.
		jsonifier::vector<action_row_data> components{};///< Message components.
		allowed_mentions_data allowedMentions{};///< Allowed mentions data.
		jsonifier::vector<embed_data> embeds{};///< Message embeds.
		jsonifier::vector<file> files{};///< Files for uploading.
		jsonifier::string customId{};///< A developer-defined identifier for the component, max 100 characters.
		jsonifier::string content{};///< Message content.
		jsonifier::string title{};///< The title of the popup modal.
		uint64_t flags{};///< Flags.
		bool tts{};///< Is it tts?

		void generateExcludedKeys();
	};

	struct create_application_command_data;

	/// @brief Data structure representing an application_command_data.
	class application_command_data {
	  public:
		unordered_map<jsonifier::string, jsonifier::string> descriptionLocalizations{};///< Dictionary with keys in available locales.
		unordered_map<jsonifier::string, jsonifier::string> nameLocalizations{};///< Dictionary with keys in available locales.
		jsonifier::vector<application_command_option_data> options{};///< A vector of possible options for the current application_command_data.
		unordered_set<jsonifier::string> jsonifierExcludedKeys{};///< Keys to exclude at serialization time.
		permission defaultMemberPermissions{};///< Set of permissions represented as a bit set all
		jsonifier::string description{};///< A description of the current application_command_data.
		application_command_type type{};///< The type of application_command_data.
		jsonifier::string version{};///< An autoincremented version.
		snowflake applicationId{};///< The current application id.
		jsonifier::string name{};///< Name of the current application_command_data.
		bool dmPermission{};///< Indicates whether the command is available in dms with the app, only for globally - scoped commands.
		snowflake guildId{};///< (where applicable) a guild id for which guild to assign this application_command_data to.
		snowflake id{};

		DCA_INLINE application_command_data() = default;

		void generateExcludedKeys();

		bool operator==(const application_command_data&) const;
	};

	/// @brief Channel_data mention data.
	class channel_mention_data {
	  public:
		snowflake guildId{};///< The id of the guild where it took place.
		jsonifier::string name{};///< The name of the channel_data that was mentioned.
		channel_type type{};///< The type of channel_data that was mentioned.
		snowflake id{};
	};

	/// @brief Data for when some channel_data pins are updated.
	struct channel_pins_update_event_data {
		time_stamp lastPinTimeStamp{};///< The time of the last pinned message.
		snowflake channelId{};///< The id of the channel_data within which the message was pinned.
		snowflake guildId{};///< The id of the guild within which the message was pinned.
	};

	/// @brief Data for when threads are synced.
	struct thread_list_sync_data {
		jsonifier::vector<jsonifier::string> channelIds{};///< The parent channel_data ids whose threads are being synced.
		jsonifier::vector<thread_member_data> members{};///< Array of members that are a part of the thread_data.
		jsonifier::vector<channel_data> threads{};///< All active threads in the given channels that the current user_data can access.
		snowflake guildId{};///< The id of the guild for which the threads are being synced.
	};

	/// @brief Represents a thread_data-members-update.
	class thread_members_update_data {
	  public:
		jsonifier::vector<jsonifier::string> removedMemberIds{};///< Members who have been removed.
		jsonifier::vector<thread_member_data> addedMembers{};///< New members added to the thread_data.
		uint32_t memberCount{};///< Number of guild-members in the thread_data.
		snowflake guildId{};///< Guild id of the thread_data.
		snowflake id{};

		DCA_INLINE thread_members_update_data() = default;
	};

	/// @brief Message interaction data.
	class message_interaction_data {
	  public:
		guild_member_data member{};
		jsonifier::string name{};
		interaction_type type{};
		user_data user{};
		snowflake id{};

		DCA_INLINE message_interaction_data() = default;
	};

	/// @brief Message types.
	enum class message_type : uint8_t {
		Default										 = 0,///< Default.
		Recipient_Add								 = 1,///< Recipient add.
		Recipient_Remove							 = 2,///< Recipient erase.
		Call										 = 3,///< call.
		Channel_Name_Change							 = 4,///< channel_data name change.
		Channel_Icon_Change							 = 5,///< channel_data icon change.
		Channel_Pinned_Message						 = 6,///< channel_data pinned message.
		Guild_Member_Join							 = 7,///< Guild memeber join.
		User_Premium_Guild_Subscription				 = 8,///< User_data premium guild subscription.
		User_Premium_Guild_Subscription_Tier_1		 = 9,///< User_data premium guild subscription tier 1.
		User_Premium_Guild_Subscription_Tier_2		 = 10,///< User_data premium guild subscription tier 2.
		User_Premium_Guild_Subscription_Tier_3		 = 11,///< User_data premium guild subscription tier 3.
		Channel_Follow_Add							 = 12,///< channel_data follow add.
		Guild_Discovery_Disqualified				 = 14,///< Guild discovery disqualified.
		Guild_Discovery_Requalified					 = 15,///< Guild discovery requalified.
		Guild_Discovery_Grace_Period_Initial_Warning = 16,///< Guild discovery grade period initial warning.
		Guild_Discovery_Grace_Period_Final_Warning	 = 17,///< Guild discovery grade period final warning.
		Thread_Created								 = 18,///< Thread_data created.
		Reply										 = 19,///< Reply.
		Chat_Input_Command							 = 20,///< chat input command.
		Thread_Starter_Message						 = 21,///< Thread_data starter message_data.
		Guild_Invite_Reminder						 = 22,///< Guild invite reminder.
		Context_Menu_Command						 = 23///< context menu command.
	};

	/// @brief Message flags.
	enum class message_flags : uint8_t {
		Crossposted			   = 1 << 0,///< crossposted.
		Is_Crosspost		   = 1 << 1,///< Is crosspost.
		Suppress_Embeds		   = 1 << 2,///< Supress embeds.
		Source_Message_Deleted = 1 << 3,///< Source message deleted.
		Urgent				   = 1 << 4,///< Urgent.
		Has_Thread			   = 1 << 5,///< Has thread_data.
		Ephemeral			   = 1 << 6,///< Ephemeral.
		Loading				   = 1 << 7///< Loading.
	};

	/// @brief Sticker_data item types.
	enum class sticker_item_type : uint8_t {
		png	   = 1,///< Png.
		apng   = 2,///< Apng.
		lottie = 3///< Lottie.
	};

	/// @brief Represents a forum thread message.
	struct forum_thread_message_data {
		jsonifier::vector<jsonifier::string> stickerIds{};///< Array of snowflakes ids of up to 3 stickers in the server to send in the message.
		jsonifier::vector<attachment_data> attachments{};///< Array of partial attachment objects attachment objects with filename.
		jsonifier::vector<action_row_data> components{};///< Array of message component objects the components to include with the message.
		allowed_mentions_data allowedMentions{};///< Allowed mention object allowed mentions for the message.
		jsonifier::vector<embed_data> embeds{};///< Array of embed objects	embedded rich content (up to 6000 characters).
		jsonifier::vector<file> files{};///< File contents the contents of the file being sent one of content, file, embed(s), sticker_ids.
		jsonifier::string content{};///< The message contents (up to 2000 characters).
		uint64_t flags{};///< Flags to be set for the message.
	};

	/// @brief Message sticker_data item data.
	class sticker_item_data {
	  public:
		sticker_item_type formatType{};///< Message sticker_data item type.
		jsonifier::string name{};///< The name of the sticker_data.
		snowflake id{};
	};

	/// @brief The core of a message's data structure.
	class message_data {
	  public:
		jsonifier::vector<channel_mention_data> mentionChannels{};///< Array of channel_data mention data.
		jsonifier::vector<jsonifier::string> mentionRoles{};///< Jsonifier::vector of "mention roles" ids.
		jsonifier::vector<sticker_item_data> stickerItems{};///< Array of message sticker_data item data.
		jsonifier::vector<attachment_data> attachments{};///< Array of attachment data.
		jsonifier::vector<action_row_data> components{};///< Array of action row data.
		jsonifier::vector<reaction_data> reactions{};//< array of reaction data.
		jsonifier::vector<sticker_data> stickers{};///< Array of message sticker_data data.
		message_reference_data messageReference{};///< Message reference data.
		jsonifier::vector<user_data> mentions{};///< Array of user_data data, for individual's that were mentioned.
		jsonifier::vector<embed_data> embeds{};///< Array of message embeds.
		message_interaction_data interaction{};///< Message interaction data.
		jsonifier::string editedTimestamp{};///< The time at which it was edited.
		message_activity_data activity{};///< Message activity data.
		application_data application{};///< A
		jsonifier::string content{};///< The message's content.
		guild_member_data member{};///< The author's guild member data.pplication data.
		snowflake applicationId{};///< Application id.
		jsonifier::string nonce{};///< Nonce.
		time_stamp timeStamp{};///< The timeStamp of when the message was created.
		bool mentionEveryone{};///< Does the message mention everyone?
		snowflake channelId{};///< The channel_data it was sent in.
		snowflake webHookId{};///< Web_hook id of the message_data, if applicable.
		channel_data thread{};///< The thread_data that the message was sent in, if applicable.
		snowflake guildId{};///< The id of the guild the message was sent in.
		message_type type{};///< Message type.
		user_data author{};///< The author's user_data data.
		uint64_t flags{};///< Flags.
		snowflake id{};
		bool pinned{};///< Is it pinned?
		bool tts{};///< Is it a text-to-speech message_data?

		virtual DCA_INLINE ~message_data() = default;
	};

	/// @brief Resolved data.
	struct resolved_data {
		unordered_map<snowflake, attachment_data> attachments{};///< Unordered_map of snowflakes to attachment objects the ids.
		unordered_map<snowflake, guild_member_data> members{};///< Unordered_map full of guild_memeber_data.
		unordered_map<snowflake, channel_data> channels{};///< Unordered_map full of channel_data.
		unordered_map<snowflake, message_data> messages{};///< Unordered_map full of messageData.
		unordered_map<snowflake, user_data> users{};///< Unordered_map full of user_data.
		unordered_map<snowflake, role_data> roles{};///< Unordered_map full of role_data.
	};

	/// @brief Represents a sticker_data pack.
	struct sticker_pack_data {
		jsonifier::vector<sticker_data> stickers{};///< Array of sticker_data objects	the stickers in the pack.
		jsonifier::string coverStickerId{};///< snowflake of a sticker_data in the pack which is shown as the pack's icon.
		jsonifier::string bannerAssetId{};///< snowflake of the sticker_data pack's banner image.
		jsonifier::string description{};///< Description of the sticker_data pack.
		jsonifier::string skuId{};///< snowflake of the pack's sku.
		jsonifier::string name{};///< Name of the sticker_data pack.
		snowflake id{};///< snowflake of the sticker_data pack.
	};

	/// @brief Connection visibility types.
	enum class connection_visibility_types : uint8_t {
		None	 = 0,///< None.
		Everyone = 1///< Everyone.
	};

	/// @brief Represents a single user_data connection.
	struct connection_data {
		jsonifier::vector<integration_data> integrations{};///< An array of partial server integrations.
		connection_visibility_types visibility{};///< Visibility of this connecti
		jsonifier::string name{};///< The username of the connection account.
		jsonifier::string type{};///< The service of the connection(twitch, youtube).
		bool showActivity{};///< Whether activities related to this connection will be shown in presence updates.on.
		bool friendSync{};///< Whether friend sync is enabled for this connection.
		bool verified{};///< Whether the connection is verified.
		bool revoked{};///< Whether the connection is revoked.
		snowflake id{};///< snowflake of the connection account.
	};

	struct application_command_interaction_data_option;

	/// @brief Application_command_data interaction data option.
	struct application_command_interaction_data_option {
		jsonifier::vector<application_command_interaction_data_option> options{};///< Application_command_data interaction data options.
		application_command_option_type type{};///< The type of application_command_data options.
		jsonifier::raw_json_data value{};///< The value.
		jsonifier::string name{};///< The name of the current option.
		bool focused{};///< True if this option is the currently focused option for autocomplete.
	};

	/// @brief Interaction data data.
	struct interaction_data_data {
		jsonifier::vector<application_command_interaction_data_option> options{};///< Application_command_data interaction data options.
		jsonifier::vector<action_row_data> components{};///< Array of message componentsthe values submitted by the user.
		jsonifier::vector<jsonifier::string> values{};///< Array of select option values the user selected in a select menu component.
		application_command_type type{};///< The type of application_command.
		component_type componentType{};///< The type of the component.
		jsonifier::string customId{};///< The custom_id of the component.
		jsonifier::string name{};///< The name of the invoked command.
		resolved_data resolved{};///< Resolved data.
		snowflake targetId{};///< The target message_data's id.
		snowflake guildId{};///< The guild that the command took place in.
		snowflake id{};///< The ID of the invoked command.
	};

	/// @brief Data from the session_start info.
	struct session_start_data {
		uint32_t maxConcurrency{};///< The number of identify requests allowed per 5 seconds.
		uint32_t resetAfter{};///< The number of milliseconds after which the limit resets.
		uint32_t remaining{};///< The remaining number of session starts the current user_data is allowed.
		uint32_t total{};///< The total number of session starts the current user_data is allowed.
	};

	/// @brief Data from the get_gateway_bot endpoint.
	struct gateway_bot_data {
		session_start_data sessionStartLimit{};///< Information on the current session start limit.
		jsonifier::string url{};///< The wss url that can be used for connecting to the gateway.
		uint32_t shards{};///< The recommended number of shards to use when connecting.
	};

	/// @brief Text input style for modals.
	enum class text_input_style : uint8_t {
		Short	  = 1,///< A single-line input.
		Paragraph = 2///< A multi-line input.
	};

	/// @brief Data representing a guild emoji update event.
	struct guild_emojis_update_event_data {
		jsonifier::vector<emoji_data> emojis{};
		snowflake guildId{};
	};

	/// @brief Data representing a guild stickers update event.
	struct guild_stickers_update_event_data {
		jsonifier::vector<sticker_data> stickers{};
		snowflake guildId{};
	};

	struct guild_members_chunk_event_data {
		jsonifier::vector<presence_update_data> presences{};
		jsonifier::vector<jsonifier::string> notFound{};
		jsonifier::vector<guild_member_data> members{};
		jsonifier::string nonce{};
		uint64_t chunkIndex{};
		uint64_t chunkCount{};
		snowflake guildId{};
	};

	/// @brief A type of user_data, to represent the bot and some of its associated endpoints.
	class DiscordCoreAPI_Dll bot_user : public user_data {
	  public:
		template<typename value_type> friend class get_user_image_url;
		friend class discord_core_client;

		bot_user(user_data& dataPackage, discord_core_internal::base_socket_agent* pBaseBaseSocketAgentNew);

		/// @brief Updates the bot's current voice-status. joins/leaves a channel, and/or self deafens/mutes.
		/// @param dataPackage the new voice_state_data.
		void updateVoiceStatus(update_voice_state_data dataPackage);

		/// @brief Updates the bot's current activity status, to be viewed by others in the same server as the bot.
		/// @param dataPackage the new presence data.
		void updatePresence(update_presence_data dataPackage);

	  protected:
		bot_user() = default;

		discord_core_internal::base_socket_agent* baseSocketAgent{};
	};

	/// @brief Guild flags.
	enum class guild_flags : uint8_t {
		Widget_enabled				 = 1 << 0,///< Widget enabled.
		Unavailable					 = 1 << 1,///< Unavailable.
		Owner						 = 1 << 2,///< Owner.
		Large						 = 1 << 3,///< Is it a large guild?
		Premium_Progress_Bar_Enabled = 1 << 4///< Premium progress bar enabled
	};

	template<guild_member_t value_type> two_id_key::two_id_key(const value_type& other) {
		idOne = other.guildId;
		idTwo = other.user.id;
	}

	template<voice_state_t value_type> two_id_key::two_id_key(const value_type& other) {
		idOne = other.guildId;
		idTwo = other.userId;
	}

	/// crtp-based struct for exposing the connectToVoice functionality of the guild-related classes.
	template<typename value_type, typename discord_core_client_t, typename guild_members_type> struct connect_to_voice {
		/// @brief For connecting to an individual voice channel.
		/// @param guildMemberId an id of the guild member who's current voice channel to connect to.
		/// @param channelId an id of the voice channel to connect to.
		/// @param selfDeaf whether or not to self-deafen the bot.
		/// @param selfMute whether or not to self-mute the bot.
		/// @param streamInfoNew for usage with the vc-to-vc audio streaming option.
		/// @return voice_connection* a pointer to the currently held voice connection, or nullptr if it failed to connect.
		DCA_INLINE voice_connection& connectToVoice(const snowflake guildMemberId, const snowflake channelId = 0, bool selfDeaf = false, bool selfMute = false,
			stream_info streamInfoNew = stream_info{}) {
			if (static_cast<discord_core_client_t*>(static_cast<value_type*>(this)->getDiscordCoreClient())
					->getVoiceConnection(static_cast<value_type*>(this)->id)
					.areWeConnected()) {
				return static_cast<discord_core_client_t*>(static_cast<value_type*>(this)->getDiscordCoreClient())->getVoiceConnection(static_cast<value_type*>(this)->id);
			} else if (guildMemberId != 0 || channelId != 0) {
				snowflake channelIdNew{};
				if (guildMemberId != 0) {
					voice_state_data_light dataLight{};
					dataLight.guildId = static_cast<value_type*>(this)->id;
					dataLight.userId  = guildMemberId;
					guild_member_data getData{};
					getData.guildId		= static_cast<value_type*>(this)->id;
					getData.user.id		= guildMemberId;
					auto voiceStateData = guild_members_type::getVoiceStateData(getData);
					if (voiceStateData.channelId != 0) {
						channelIdNew = voiceStateData.channelId;
					}
				} else {
					channelIdNew = channelId;
				}
				int32_t theShardId{ static_cast<int32_t>((static_cast<value_type*>(this)->id.operator const uint64_t&() >> 22) %
					static_cast<discord_core_client_t*>(static_cast<value_type*>(this)->getDiscordCoreClient())->getConfigManager().getTotalShardCount()) };
				voice_connect_init_data voiceConnectInitData{};
				voiceConnectInitData.currentShard = theShardId;
				voiceConnectInitData.streamInfo	  = streamInfoNew;
				voiceConnectInitData.channelId	  = channelIdNew;
				voiceConnectInitData.guildId	  = static_cast<value_type*>(this)->id;
				voiceConnectInitData.userId		  = static_cast<discord_core_client_t*>(static_cast<value_type*>(this)->getDiscordCoreClient())->getBotUser().id;
				voiceConnectInitData.selfDeaf	  = selfDeaf;
				voiceConnectInitData.selfMute	  = selfMute;
				auto& voiceConnectionNew =
					static_cast<discord_core_client_t*>(static_cast<value_type*>(this)->getDiscordCoreClient())->getVoiceConnection(static_cast<value_type*>(this)->id);
				stop_watch<milliseconds> stopWatch{ milliseconds{ 10000 } };
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
			return static_cast<discord_core_client_t*>(static_cast<value_type*>(this)->getDiscordCoreClient())->getVoiceConnection(static_cast<value_type*>(this)->id);
		}
	};

	/// @brief A discord guild. used to connect to/disconnect from voice.
	class DiscordCoreAPI_Dll guild_data : public flag_entity<guild_data>,
										  public get_guild_image_url<guild_data>,
										  public connect_to_voice<guild_data, discord_core_client, guild_members> {
	  public:
		friend class guilds;

		jsonifier::vector<guild_scheduled_event_data> guildScheduledEvents{};///< Scheduled events in the guild.
		default_message_notification_level defaultMessageNotifications{};///< Default message notifications level.
		jsonifier::vector<stage_instance_data> stageInstances{};///< Stage instances in the guild.
		jsonifier::vector<voice_state_data_light> voiceStates{};///< Voice states for the current guild_members.
		explicit_content_filter_level explicitContentFilter{};///< Explicit content filter level.
		jsonifier::vector<presence_update_data> presences{};///< Presences of the members in the guild.
		jsonifier::vector<jsonifier::string> features{};///< Enabled guild features.
		jsonifier::vector<guild_member_data> members{};///< custom guild emojis.
		jsonifier::vector<channel_data> channels{};///< custom guild emojis.
		jsonifier::vector<sticker_data> stickers{};///< custom guild stickers.
		jsonifier::vector<channel_data> threads{};///< All active threads in the guild that the current user has permission to view.
		system_channel_flags systemChannelFlags{};///< System channel flags.
		verification_level verificationLevel{};///< Verification level required for the guild.
		jsonifier::vector<emoji_data> emoji{};///< custom guild emojis.
		uint32_t maxStageVideoChannelUsers{};///< The maximum amount of users in a stage video channel.
		jsonifier::vector<role_data> roles{};///< Roles in the guild.
		uint32_t premiumSubscriptionCount{};///< The number of boosts this guild currently has.
		jsonifier::string preferredLocale{};///< The preferred locale of a community guild; used in server discovery and notices from discord.
		jsonifier::string discoverySplash{};///< Discovery splash hash; only present for guilds with the "discoverable" feature.
		uint32_t approximatePresenceCount{};///< Approximate number of non-offline members in this guild, returned sometimes.
		voice_connection* voiceConnection{};///< Voice connection potentially held by the current guild.
		welcome_screen_data welcomeScreen{};///< The welcome screen of a community guild, shown to new members, returned in an invite's guild object.
		snowflake publicUpdatesChannelId{};///< The id of the channel where admins and moderators of community guilds receive notices from discord.
		snowflake safetyAlertsChannelId{};///< The id of the channel where admins and moderators of community guilds receive safety alerts.
		uint32_t approximateMemberCount{};///< Approximate number of members in this guild, returned from the get /guilds/id and /users/me/guilds.
		jsonifier::string vanityUrlCode{};///< The vanity url code for the guild.
		bool premiumProgressBarEnabled{};///< Whether the guild has the boost progress bar enabled.
		uint32_t maxVideoChannelUsers{};///< The maximum amount of users in a video channel.
		jsonifier::string description{};///< The description of a guild.
		permissions_parse permissions{};///< Total permissions for the user in the guild (excludes overwrites).
		jsonifier::string discovery{};///< Url to the guild's icon.
		snowflake systemChannelId{};///< The id of the channel where guild notices such as welcome messages and boost events are posted.
		snowflake widgetChannelId{};///< The channel id that the widget will generate an invite to, or null if set to no invite.
		guild_nsfwlevel nsfwLevel{};///< Guild nsfw level.
		jsonifier::string splash{};///< Splash hash.
		jsonifier::string banner{};///< Banner hash.
		premium_tier premiumTier{};///< Premium tier (server boost level).
		snowflake rulesChannelId{};///< The id of the channel where community guilds can display rules and/or guidelines.
		snowflake applicationId{};///< Application id of the guild creator if it is bot-created.
		snowflake afkChannelId{};///< Id of afk channel.
		jsonifier::string name{};///< Guild name (2-100 characters, excluding trailing and leading whitespace).
		jsonifier::string icon{};///< Icon hash.
		uint32_t maxPresences{};///< The maximum number of presences for the guild (null is always returned, apart from the largest of guilds).
		uint32_t memberCount{};///< The number of members in this guild.jsonifier::string region{};///< Voice region id for the guild (deprecated).
		uint32_t maxMembers{};///< The maximum number of members for the guild.
		uint32_t afkTimeout{};///< Afk timeout in seconds.
		time_stamp joinedAt{};///< When the bot joined this guild.
		bool widgetEnabled{};///< True if the server widget is enabled.
		mfalevel mfaLevel{};///< Required mfa level for the guild.
		snowflake ownerId{};///< Id of owner.
		guild_flags flags{};/// the flags for this guild.
		bool unavailable{};///< True if this guild is unavailable due to an outage.
		snowflake id{};///< Guild id.
		bool large{};///< True if this is considered a large guild.
		bool owner{};///< True if the user is the owner of the guild.

		DCA_INLINE guild_data() = default;

		guild_data(snowflake snowflake);

		discord_core_client* getDiscordCoreClient();

		bool areWeConnected();

		/// \brief For disconnecting from the current guild's voice channel.
		void disconnect();
	};

	/// @brief Represents a single thread_data.
	class thread_data : public channel_data {
	  public:
	};

	/// @brief Data structure representing a single guild, for the purposes of populating the cache.
	class DiscordCoreAPI_Dll guild_cache_data : public flag_entity<guild_cache_data>,
												public get_guild_image_url<guild_cache_data>,
												public connect_to_voice<guild_cache_data, discord_core_client, guild_members> {
	  public:
		jsonifier::vector<snowflake> channels{};///< Array of guild channels.
		jsonifier::vector<snowflake> members{};///< Array of guild_members.
		jsonifier::vector<snowflake> emoji{};///< Array of guild channels.
		jsonifier::vector<snowflake> roles{};///< Array of guild roles.
		voice_connection* voiceConnection{};///< A pointer to the voice_connection, if present.
		icon_hash discoverySplash{};///< Url to the guild's icon.
		jsonifier::string name{};///< The guild's name.
		uint32_t memberCount{};///< Member count.
		time_stamp joinedAt{};///< When the bot joined this guild.
		icon_hash discovery{};///< Url to the guild's icon.
		snowflake ownerId{};///< User_data id of the guild's owner.
		guild_flags flags{};///< Guild flags.
		icon_hash banner{};///< Url to the guild's icon.
		icon_hash splash{};///< Url to the guild's icon.
		icon_hash icon{};///< Url to the guild's icon.
		snowflake id{};///< The id of this guild.

		DCA_INLINE guild_cache_data() = default;

		guild_cache_data(snowflake snowflakeNew);

		guild_cache_data& operator=(guild_data&& data) noexcept;
		guild_cache_data(guild_data&& data) noexcept;

		guild_cache_data& operator=(const guild_data&);

		guild_cache_data(const guild_data&);

		operator guild_data();

		discord_core_client* getDiscordCoreClient();

		DCA_INLINE bool operator==(const snowflake& other) const {
			return id == other;
		}

		bool areWeConnected();

		/// \brief For disconnecting from the current guild's voice channel.
		void disconnect();
	};

	/// @brief Interaction data.
	class interaction_data {
	  public:
		jsonifier::string appPermissions{};
		jsonifier::string guildLocale{};
		interaction_data_data data{};
		jsonifier::string locale{};
		guild_member_data member{};
		snowflake applicationId{};
		jsonifier::string token{};
		interaction_type type{};
		message_data message{};
		channel_data channel{};
		snowflake channelId{};
		snowflake guildId{};
		guild_data guild{};
		int64_t version{};
		user_data user{};
		snowflake id{};

		DCA_INLINE interaction_data() = default;
	};

	class partial_guild_data {
	  public:
		bool unavailable{};
		snowflake id{};
	};

	/// @brief Invite data.
	struct invite_data {
		guild_scheduled_event_data guildScheduledEvent{};///< Scheduled guild event.
		application_data targetApplication{};///< Application data.
		uint64_t approximatePresenceCount{};///< Approximate presence count.
		stage_instance_data stageInstance{};///< Stage instance data.
		uint64_t approximateMemberCount{};///< Approximate member count.
		jsonifier::string expiresAt{};///< When the invite expires.
		jsonifier::string createdAt{};///< Time it was created at.
		partial_guild_data guild{};///< Guild data of the channel_data that the invite is for.
		jsonifier::string code{};///< Unique invite code.
		user_data targetUser{};///< Target user_data of the invite.
		channel_data channel{};///< channel_data data of the channel_data that the invite is for.
		uint64_t targetType{};///< Target type.
		snowflake guildId{};///< The guild this invite is for.
		user_data inviter{};///< The user_data who created the invite.
		uint64_t maxUses{};///< Max number of uses.
		uint64_t maxAge{};///< Maximum age of the invite.
		bool temporary{};///< Is it temporary?
		uint64_t uses{};///< The current number of uses.
	};

	/// @brief Represents a guild template.
	struct guild_template_data {
		guild_data serializedSourceGuild{};///< The guild snapshot this template contains.
		jsonifier::string sourceGuildId{};///< The id of the guild this template is based on.
		jsonifier::string description{};///< The description for the template.
		jsonifier::string creatorId{};///< The id of the user_data who created the template.
		jsonifier::string createdAt{};///< When this template was created.
		jsonifier::string updatedAt{};///< When this template was last synced to the source guild.
		jsonifier::string code{};///< The template code(unique id).
		jsonifier::string name{};///< Template name.
		uint32_t usageCount{};///< Number of times this template has been used.
		user_data creator{};///< The user_data who created the template.
		bool isDirty{};///< Whether the template has unsynced changes.
	};

	/// @brief Web_hook_data data.
	class web_hook_data {
	  public:
		channel_data sourceChannel{};///< channel_data for which th web_hook_data was issued.
		jsonifier::string avatar{};///< Avatar of the web_hook_data.
		jsonifier::string token{};///< Token of the web_hook_data.
		snowflake applicationId{};///< Application id.
		jsonifier::string name{};///< Name of the web_hook_data.
		guild_data sourceGuild{};///< Source guild id.
		jsonifier::string url{};///< Url of the web_hook_data.
		snowflake channelId{};///< channel_data id for which the web_hook_data was issued.
		web_hook_type type{};///< Type of web_hook_data.
		snowflake guildId{};///< Guild id for which the web_hook_data was issued.
		user_data user{};///< User_data which create the web_hook_data.
		snowflake id{};

		DCA_INLINE web_hook_data() = default;
	};

	/// @brief Audit log data.
	class DiscordCoreAPI_Dll audit_log_data {
	  public:
		jsonifier::vector<guild_scheduled_event_data> guildScheduledEvents{};///< Array of guild scheduled event objects.
		jsonifier::vector<auto_moderation_rule_data> autoModerationRules{};///< List of auto moderation rules referenced in the audit log.
		jsonifier::vector<audit_log_entry_data> auditLogEntries{};///< Array of audit log entry objects.
		jsonifier::vector<integration_data> integrations{};///< Array of partial integration objects.
		jsonifier::vector<web_hook_data> webhooks{};///< Array of webhook objects.
		jsonifier::vector<channel_data> threads{};///< Array of thread-specific channel objects.
		jsonifier::vector<user_data> users{};///< Array of user objects.

		auto getAuditLogData(const snowflake userIdOfChanger, audit_log_event auditLogType);

		auto getAuditLogData(audit_log_event auditLogType, const snowflake userIdOfTarget);
	};

	/// @brief Data representing an input-event, which is any message or interaction that is coming into the bot as an input.
	class DiscordCoreAPI_Dll input_event_data {
	  public:
		template<typename value_type> friend struct jsonifier::core;
		template<typename value_type> friend struct event_data;
		friend struct on_interaction_creation_data;
		friend struct base_function_arguments;

		friend class discord_core_internal::websocket_client;
		friend class discord_core_internal::base_socket_agent;
		friend class respond_to_input_event_data;
		friend class discord_core_client;
		friend class command_data;
		friend class input_events;

		input_event_response_type responseType{};///< The type of response that this input value represents.

		input_event_data& operator=(input_event_data&& other) noexcept;

		input_event_data(input_event_data&& other) noexcept;

		input_event_data& operator=(const input_event_data& other);

		input_event_data(const input_event_data& other);

		input_event_data& operator=(const interaction_data& other);

		input_event_data(const interaction_data& interactionData);

		DCA_INLINE input_event_data() = default;

		/// @brief Returns the interaction data, if appplicable, of this input-event.
		/// @return interaction_data a snowflake containing the interaction data.
		const interaction_data& getInteractionData() const;

		/// @brief Returns the guild_member_data of this input-event.
		/// @return guild_member_data a guild_member_data containing the guild_member_data data.
		const guild_member_data& getGuildMemberData() const;

		/// @brief Returns the channel_data of this input-event.
		/// @return channel_data a channel_data containing the channel_data data.
		const channel_data& getChannelData() const;

		/// @brief Returns the message data, if applicable, of this input-event.
		/// @return message a message structure.
		const message_data& getMessageData() const;

		/// @brief Returns the user_data of this input-event.
		/// @return user_data a user_data containing the user_data data.
		const user_data& getUserData() const;

	  protected:
		unique_ptr<interaction_data> interactionData{ makeUnique<interaction_data>() };
	};

	struct DiscordCoreAPI_Dll move_through_message_pages_data;

	/// @brief Data for responding to an input-event.
	class DiscordCoreAPI_Dll respond_to_input_event_data {
	  public:
		friend struct delete_interaction_response_data;
		friend struct delete_follow_up_message_data;
		friend struct interaction_response_data;

		friend DiscordCoreAPI_Dll move_through_message_pages_data moveThroughMessagePages(const jsonifier::string& userID, input_event_data originalEvent,
			uint32_t currentPageIndex, const jsonifier::vector<embed_data>& messageEmbeds, bool deleteAfter, uint32_t waitForMaxMs, bool returnResult);

		friend class create_ephemeral_interaction_response_data;
		friend class create_deferred_interaction_response_data;
		friend class create_ephemeral_follow_up_message_data;
		friend class create_interaction_response_data;
		friend class edit_interaction_response_data;
		friend class create_follow_up_message_data;
		friend class edit_follow_up_message_data;
		friend class create_message_data;
		friend class edit_message_data;
		friend class input_events;
		friend class send_dmdata;

		operator interaction_callback_data() const;

		DCA_INLINE respond_to_input_event_data() noexcept = default;

		respond_to_input_event_data& operator=(const interaction_data dataPackage);

		respond_to_input_event_data(const interaction_data dataPackage);

		respond_to_input_event_data& operator=(const input_event_data dataPackage);

		respond_to_input_event_data(const input_event_data dataPackage);

		/// @brief Adds a button to the response message_data.
		/// @param disabled whether the button is active or not.
		/// @param customIdNew a custom id to give for identifying the button.
		/// @param buttonLabel a visible label for the button.
		/// @param buttonStyle the style of the button.
		/// @param emojiName an emoji name, if desired.
		/// @param emojiId an emoji id, if desired.
		/// @param url a url, if applicable.
		/// @return respond_to_input_event_data& a reference to this data structure.
		respond_to_input_event_data& addButton(bool disabled, const jsonifier::string& customIdNew, const jsonifier::string& buttonLabel, button_style buttonStyle,
			const jsonifier::string& emojiName = "", snowflake emojiId = snowflake{}, const jsonifier::string& url = "");

		/// @brief Adds a select-menu to the response message_data.
		/// @param disabled whether the select-menu is active or not.
		/// @param customIdNew a custom id to give for identifying the select-menu.
		/// @param options a vector of select-menu-options to offer.
		/// @param placeholder custom placeholder text if nothing is selected, max 100 characters.
		/// @param maxValues maximum number of selections that are possible.
		/// @param minValues minimum required number of selections that are required.
		/// @param type the type of select-menu that this is.
		/// @param channelTypes types of channels that can be accepted if this is of the type channel_type.
		/// @return respond_to_input_event_data& a reference to this data structure.
		respond_to_input_event_data& addSelectMenu(bool disabled, const jsonifier::string& customIdNew, const jsonifier::vector<select_option_data>& options,
			const jsonifier::string& placeholder, uint64_t maxValues, uint64_t minValues, select_menu_type type,
			jsonifier::vector<channel_type> channelTypes = jsonifier::vector<channel_type>{});

		/// @brief Adds a modal to the response message_data.
		/// @param topTitleNew a title for the modal.
		/// @param topCustomIdNew a custom id to give for the modal.
		/// @param titleNew a title for the modal's individual input.
		/// @param customIdNew a custom id to give for the modal's individual input.
		/// @param required is it a required response?
		/// @param minLength minimum length.
		/// @param maxLength maximum length.
		/// @param inputStyle the input style.
		/// @param label a label for the modal.
		/// @param placeholder a placeholder for the modal.
		/// @return respond_to_input_event_data& a reference to this data structure.
		respond_to_input_event_data& addModal(const jsonifier::string& topTitleNew, const jsonifier::string& topCustomIdNew, const jsonifier::string& titleNew,
			const jsonifier::string& customIdNew, bool required, uint64_t minLength, uint64_t maxLength, text_input_style inputStyle, const jsonifier::string& label = "",
			const jsonifier::string& placeholder = "");

		/// @brief Adds a file to the current collection of files for this message response.
		/// @param theFile the file to be added.
		/// @return respond_to_input_event_data& a reference to this data structure.
		respond_to_input_event_data& addFile(const file& theFile);

		/// @brief For setting the allowable mentions in a response.
		/// @param dataPackage an allowed_mentions_data structure.
		/// @return respond_to_input_event_data& a reference to this data structure.
		respond_to_input_event_data& addAllowedMentions(const allowed_mentions_data& dataPackage);

		/// @brief For setting the type of response to make.
		/// @param typeNew an input_event_response_type.
		/// @return respond_to_input_event_data& a reference to this data structure.
		respond_to_input_event_data& setResponseType(input_event_response_type typeNew);

		/// @brief For setting the components in a response.
		/// @param dataPackage an action_row_data structure.
		/// @return respond_to_input_event_data& a reference to this data structure.
		respond_to_input_event_data& addComponentRow(const action_row_data& dataPackage);

		/// @brief For setting the embeds in a response.
		/// @param dataPackage an embed_data structure.
		/// @return respond_to_input_event_data& a reference to this data structure.
		respond_to_input_event_data& addMessageEmbed(const embed_data dataPackage);

		/// @brief For setting the message content in a response.
		/// @param dataPackage a string, containing the content.
		/// @return respond_to_input_event_data& a reference to this data structure.
		respond_to_input_event_data& addContent(const jsonifier::string& dataPackage);

		/// @brief For setting the tts status of a response.
		/// @param enabledTTs a bool.
		/// @return respond_to_input_event_data& a reference to this data structure.
		respond_to_input_event_data& setTTSStatus(bool enabledTTs);

		/// @brief For setting the choices of an autocomplete response.
		/// @param value an simdjson::ondemand::object value that is either a double, uint64_t or a string.
		/// @param theName a string for the name of the choice.
		/// @param theNameLocalizations a unordered_map<jsonifier::string, jsonifier::string> for the name localizations.
		/// @return respond_to_input_event_data& a reference to this data structure.
		respond_to_input_event_data& setAutoCompleteChoice(discord_core_internal::etf_serializer value, const jsonifier::string& theName,
			const unordered_map<jsonifier::string, jsonifier::string>& theNameLocalizations);

		/// @brief For setting the direct-message_data user target of a response.
		/// @param targetUserIdNew a string, containing the target user_data's id.
		/// @return respond_to_input_event_data& a reference to this data structure.
		respond_to_input_event_data& setTargetUserID(const snowflake targetUserIdNew);

	  protected:
		jsonifier::vector<application_command_option_choice_data> choices{};
		jsonifier::vector<action_row_data> components{};
		allowed_mentions_data allowedMentions{};
		jsonifier::vector<embed_data> embeds{};
		jsonifier::string interactionToken{};
		input_event_response_type type{};///< The type of response to make.
		jsonifier::vector<file> files{};
		interaction_type eventType{};
		jsonifier::string customId{};
		jsonifier::string content{};
		jsonifier::string title{};
		snowflake interactionId{};
		snowflake applicationId{};
		snowflake targetUserId{};
		snowflake channelId{};
		snowflake messageId{};
		uint64_t flags{};
		bool tts{};
	};

	/// @brief Message response base, for responding to messages.
	class DiscordCoreAPI_Dll message_response_base {
	  public:
		template<typename value_type> friend struct jsonifier::core;
		/// @brief Adds a button to the response message_data.
		/// @param disabled whether the button is active or not.
		/// @param customIdNew a custom id to give for identifying the button.
		/// @param buttonLabel a visible label for the button.
		/// @param buttonStyle the style of the button.
		/// @param emojiName an emoji name, if desired.
		/// @param emojiId an emoji id, if desired.
		/// @param url a url, if applicable.
		/// @return message_response_base& a reference to this data structure.
		message_response_base& addButton(bool disabled, const jsonifier::string& customIdNew, const jsonifier::string& buttonLabel, button_style buttonStyle,
			const jsonifier::string& emojiName = "", snowflake emojiId = snowflake{}, const jsonifier::string& url = "");

		/// @brief Adds a select-menu to the response message_data.
		/// @param disabled whether the select-menu is active or not.
		/// @param customIdNew a custom id to give for identifying the select-menu.
		/// @param options a vector of select-menu-options to offer.
		/// @param placeholder custom placeholder text if nothing is selected, max 100 characters.
		/// @param maxValues maximum number of selections that are possible.
		/// @param minValues minimum required number of selections that are required.
		/// @param type the type of select-menu that this is.
		/// @param channelTypes types of channels that can be accepted if this is of the type channel_type.
		/// @return respond_to_input_event_data& a reference to this data structure.
		message_response_base& addSelectMenu(bool disabled, const jsonifier::string& customIdNew, const jsonifier::vector<select_option_data>& options,
			const jsonifier::string& placeholder, uint64_t maxValues, uint64_t minValues, select_menu_type type,
			jsonifier::vector<channel_type> channelTypes = jsonifier::vector<channel_type>{});

		/// @brief Adds a modal to the response message_data.
		/// @param topTitleNew a title for the modal.
		/// @param topCustomIdNew a custom id to give for the modal.
		/// @param titleNew a title for the modal's individual input.
		/// @param customIdNew a custom id to give for the modal's individual input.
		/// @param required is it a required response?
		/// @param minLength minimum length.
		/// @param maxLength maximum length.
		/// @param inputStyle the input style.
		/// @param label a label for the modal.
		/// @param placeholder a placeholder for the modal.
		/// @return message_response_base& a reference to this data structure.
		message_response_base& addModal(const jsonifier::string& topTitleNew, const jsonifier::string& topCustomIdNew, const jsonifier::string& titleNew,
			const jsonifier::string& customIdNew, bool required, uint64_t minLength, uint64_t maxLength, text_input_style inputStyle, const jsonifier::string& label = "",
			const jsonifier::string& placeholder = "");

		/// @brief Adds a file to the current collection of files for this message response.
		/// @param theFile the file to be added.
		/// @return message_response_base& a reference to this data structure.
		message_response_base& addFile(const file& theFile);

		/// @brief For setting the allowable mentions in a response.
		/// @param dataPackage an allowed_mentions_data structure.
		/// @return message_response_base& a reference to this data structure.
		message_response_base& addAllowedMentions(const allowed_mentions_data& dataPackage);

		/// @brief For setting the components in a response.
		/// @param dataPackage an action_row_data structure.
		/// @return message_response_base& a reference to this data structure.
		message_response_base& addComponentRow(const action_row_data& dataPackage);

		/// @brief For setting the embeds in a response.
		/// @param dataPackage an embed_data structure.
		/// @return message_response_base& a reference to this data structure.
		message_response_base& addMessageEmbed(const embed_data& dataPackage);

		/// @brief For setting the message content in a response.
		/// @param dataPackage a string, containing the content.
		/// @return message_response_base& a reference to this data structure.
		message_response_base& addContent(const jsonifier::string& dataPackage);

		/// @brief For setting the tts status of a response.
		/// @param enabledTTs a bool.
		/// @return message_response_base& a reference to this data structure.
		message_response_base& setTTSStatus(bool enabledTTs);

	  protected:
		jsonifier::vector<action_row_data> components{};
		allowed_mentions_data allowedMentions{};
		jsonifier::vector<embed_data> embeds{};
		jsonifier::vector<file> files{};
		jsonifier::string customId{};
		jsonifier::string content{};
		jsonifier::string title{};
		uint64_t flags{};
		bool tts{};
	};

	/// @brief Interaction response data.
	struct DiscordCoreAPI_Dll interaction_response_data {
		interaction_callback_data data{};///< Interaction application_command_data callback data.
		interaction_callback_type type{};///< Interaction callback type.

		interaction_response_data& operator=(const respond_to_input_event_data& other);

		interaction_response_data(const respond_to_input_event_data& other);

		interaction_response_data() = default;
	};

	/// @brief Guild application command permissions data.
	class guild_application_command_permission_data {
	  public:
		jsonifier::vector<application_command_permission_data> permissions{};
		snowflake applicationId{};
		snowflake guildId{};
		snowflake id{};
	};

	/**@}*/

	/// @brief Song types.
	enum class song_type : uint8_t {
		Neutral	   = 0,///< For either type.
		YouTube	   = 1,///< You_tube.
		SoundCloud = 2///< Sound_cloud.
	};

	/// @brief Represents a download url.
	struct download_url {
		uint64_t contentSize{};
		jsonifier::string urlPath{};
	};

	/**
	 * \addtogroup voice_connection
	 * @{
	 */

	class song {
	  public:
		friend class discord_core_internal::sound_cloud_request_builder;
		friend class discord_core_internal::you_tube_request_builder;
		friend class discord_core_internal::sound_cloud_api;
		friend class discord_core_internal::you_tube_api;
		friend class song_api;

		jsonifier::vector<download_url> finalDownloadUrls{};
		song_type type{ song_type::Neutral };///< The type of song.
		jsonifier::string secondDownloadUrl{};
		jsonifier::string firstDownloadUrl{};
		jsonifier::string addedByUserName{};///< The user_data name of the individual who added this song to the
		jsonifier::string thumbnailUrl{};///< The url of the thumbnail image of this song.
		jsonifier::string description{};///< A description of the song.
		jsonifier::string songTitle{};///< The title of the song.
		jsonifier::string duration{};///< The duration of the song.
		jsonifier::string viewUrl{};///< The url for listening to this song through a browser.
		jsonifier::string songId{};
		snowflake addedByUserId{};///< The user_data id of the individual who added this song to the.
		uint64_t contentLength{};
	};

	/// @brief Song completion event data.
	struct song_completion_event_data {
		snowflake guildMemberId{};///< The sending guild_member_data.
		snowflake guildId{};///< The sending guild.
		bool wasItAFail{};///< Is this a replay? (did a track recently fail to play?)
	};

	/// @brief Playlist of songs and other variables.
	struct playlist {
		jsonifier::vector<song> songQueue{};///< The list of songs that are stored to be played.
		bool isLoopSongEnabled{};///< Is looping of songs currently enabled?
		bool isLoopAllEnabled{};///< Is looping of the entire playlist currently enabled?
		song currentSong{};///< The current song that is playing.

		DCA_INLINE bool areThereAnySongs() {
			if (isLoopAllEnabled) {
				return songQueue.size() > 0 || currentSong.songId != "";
			} else if (isLoopSongEnabled) {
				return songQueue.size() > 0 || currentSong.songId != "";
			} else {
				return songQueue.size() > 0;
			}
		}

		DCA_INLINE bool sendNextSong() {
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
					song tempSong02 = currentSong;
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
					currentSong = song{};
					return false;
				}
			}
			return false;
		}

		DCA_INLINE void modifyQueue(uint64_t firstSongPosition, uint64_t secondSongPosition) {
			song tempSong					 = songQueue.at(firstSongPosition);
			songQueue.at(firstSongPosition)	 = songQueue.at(secondSongPosition);
			songQueue.at(secondSongPosition) = tempSong;
		}
	};

	/**@}*/

	/**
	* \addtogroup utilities
	* @{
	*/

	struct serializer_value {
		unordered_map<jsonifier::string, jsonifier::raw_json_data> values{};
	};

	/// @brief Command data, for functions executed by the command_controller.
	class DiscordCoreAPI_Dll command_data {
	  public:
		template<typename value_type> friend struct event_data;
		friend struct on_interaction_creation_data;

		command_data() = default;

		command_data& operator=(command_data&&) = default;

		command_data(command_data&&) = default;

		command_data& operator=(const command_data&);

		command_data(const command_data&);

		command_data(const input_event_data& inputEventData);

		/// @brief Returns the interaction data, if appplicable, of this input-event.
		/// @return interaction_data a snowflake containing the interaction data.
		const interaction_data& getInteractionData() const;

		/// @brief Returns the guild_member_data of this input-event.
		/// @return guild_member_data a guild_member_data containing the guild_member_data data.
		const guild_member_data& getGuildMemberData() const;

		/// @brief Returns the channel_data of this input-event.
		/// @return channel_data a channel_data containing the channel_data data.
		const channel_data& getChannelData() const;

		/// @brief Returns the message data, if applicable, of this input-event.
		/// @return message a message structure.
		const message_data& getMessageData() const;

		/// @brief Returns the user_data of this input-event.
		/// @return user_data a user_data containing the user_data data.
		const user_data& getUserData() const;

		/// @brief Returns the name of this entered command.
		/// @return jsonifier::string a string containing the name of this command.
		jsonifier::string getCommandName() const;

		/// @brief Returns the subcommand-name of this entered command.
		/// @return jsonifier::string a string containing the name of this sub-command.
		jsonifier::string getSubCommandName() const;

		/// @brief Returns the subcommand-name of this entered command group.
		/// @return jsonifier::string a string containing the name of this sub-command's group.
		jsonifier::string getSubCommandGroupName() const;

		/// @brief Returns the collection of command inputs/options for this command.
		/// @return serializer_value a serializer_value containing the arguments entered with this command.
		serializer_value getCommandArguments() const;

		/// @brief Returns the input_event_data for this command.
		/// @return input_event_data an input_event_data containing the data associated  with this command.
		const input_event_data& getInputEventData() const;

		virtual ~command_data() = default;

	  protected:
		unique_ptr<input_event_data> eventData{};
		jsonifier::string subCommandGroupName{};
		jsonifier::string subCommandName{};
		jsonifier::string commandName{};
		serializer_value optionsArgs{};
	};

	/// @brief Base arguments for the command classes.
	struct DiscordCoreAPI_Dll base_function_arguments : public command_data {
		base_function_arguments() = default;

		base_function_arguments(const command_data& commandData);
	};

	/// @brief Base class for the command classes.
	struct DiscordCoreAPI_Dll base_function {
		jsonifier::string helpDescription{};///< Description of the command for the help command.
		jsonifier::string commandName{};///< Name of the command for calling purposes.
		embed_data helpEmbed{};///< A message embed for displaying the command via the help command.

		/// @brief The base function for the command's execute function.
		/// @param args a reference to an instance of base_function_arguments.
		virtual void execute(const base_function_arguments& args) = 0;
		virtual unique_ptr<base_function> create()				  = 0;
		virtual ~base_function()								  = default;
	};

	struct DiscordCoreAPI_Dll move_through_message_pages_data {
		input_event_data inputEventData{};
		jsonifier::string buttonId{};
		uint32_t currentPageIndex{};
	};

	DiscordCoreAPI_Dll move_through_message_pages_data moveThroughMessagePages(snowflake userID, input_event_data originalEvent, uint32_t currentPageIndex,
		const jsonifier::vector<embed_data>& messageEmbeds, bool deleteAfter, uint32_t waitForMaxMs, bool returnResult);

	/**@}*/

	struct ready_data {
		unordered_set<jsonifier::string> jsonifierExcludedKeys{};
		jsonifier::vector<jsonifier::string> trace{};
		jsonifier::string resumeGatewayUrl{};
		application_data application{};
		jsonifier::string sessionId{};
		jsonifier::string shard{};
		user_data user{};
		uint64_t v{};
	};

}
