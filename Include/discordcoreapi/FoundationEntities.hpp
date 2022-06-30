/*
*
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

#ifdef max
	#undef max
#endif

#ifdef min
	#undef min
#endif

/**
 * \defgroup main_endpoints Main Endpoints
 * \brief For all of the Discord API's endpoints.
 */

/**
 * \defgroup voice_connection Voice Connection
 * \brief For all of the voice connection related stuff.
 */

/**
 * \defgroup discord_events Discord Events
 * \brief For all of the events that could be sent by the Discord API.
 */

/**
 * \defgroup utilities Utilities
 * \brief For utility classes/functions.
 */

/**
 * \defgroup foundation_entities Foundation Entities
 * \brief For all of the building blocks of the main endpoints.
 */

/**
 * \addtogroup foundation_entities
 * @{
 */

namespace DiscordCoreInternal {

	using Snowflake = uint64_t;

	struct DiscordCoreAPI_Dll VoiceConnectInitData {
		Snowflake channelId{};
		Snowflake guildId{};
		Snowflake userId{};
		int32_t currentShard{};
		bool selfDeaf{ false };
		bool selfMute{ false };
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
		std::string botToken{};
		std::string sessionId{};
		int32_t lastNumberReceived{};

		operator nlohmann::json() {
			nlohmann::json theData{};
			theData["d"]["seq"] = this->lastNumberReceived;
			theData["d"]["session_id"] = this->sessionId;
			theData["d"]["token"] = this->botToken;
			theData["op"] = 6;
			return theData;
		}
	};

	struct DiscordCoreAPI_Dll WebSocketIdentifyData {
		std::string botToken{};
		int64_t intents{};
		int32_t currentShard{};
		int32_t numberOfShards{};
		operator nlohmann::json() {
			nlohmann::json data{};
			data["d"]["properties"]["browser"] = "DiscordCoreAPI";
			data["d"]["properties"]["device"] = "DiscordCoreAPI";
			data["d"]["shard"] = { this->currentShard, this->numberOfShards };
			data["d"]["large_threshold"] = 250;
			data["d"]["intents"] = this->intents;
			data["d"]["compress"] = false;
			data["d"]["token"] = this->botToken;
			data["op"] = 2;
#ifdef _WIN32
			data["d"]["properties"]["os"] = "Windows";
#else
			data["d"]["properties"]["os"] = "Linux";
#endif
			return data;
		}
	};

	struct DiscordCoreAPI_Dll VoiceSocketProtocolPayloadData {
		std::string voicePort{};
		std::string externalIp{};
		std::string voiceEncryptionMode{};

		operator std::vector<uint8_t>() {
			nlohmann::json data{};
			data["d"]["data"]["port"] = stol(this->voicePort);
			data["d"]["data"]["mode"] = this->voiceEncryptionMode;
			data["d"]["data"]["address"] = this->externalIp;
			data["d"]["protocol"] = "udp";
			data["op"] = 1;
			std::vector<uint8_t> newVector{};
			std::string newString = data.dump();
			newString.shrink_to_fit();
			newVector.insert(newVector.begin(), newString.begin(), newString.end());
			return newVector;
		}
	};

	struct DiscordCoreAPI_Dll VoiceIdentifyData {
		VoiceConnectInitData connectInitData{};
		VoiceConnectionData connectionData{};
		operator std::vector<uint8_t>() {
			nlohmann::json data{};
			data["d"]["session_id"] = this->connectionData.sessionId;
			data["d"]["server_id"] = std::to_string(this->connectInitData.guildId);
			data["d"]["user_id"] = std::to_string(this->connectInitData.userId);
			data["d"]["token"] = this->connectionData.token;
			data["op"] = 0;
			std::vector<uint8_t> newVector{};
			std::string newString = data.dump();
			newString.shrink_to_fit();
			newVector.insert(newVector.begin(), newString.begin(), newString.end());
			return newVector;
		}
	};

	struct DiscordCoreAPI_Dll SendSpeakingData {
		std::string ssrc{};
		int32_t delay{};

		operator std::vector<uint8_t>() {
			nlohmann::json data{};
			data["d"]["speaking"] = 1 << 0;
			data["d"]["delay"] = delay;
			data["d"]["ssrc"] = ssrc;
			data["op"] = 5;
			std::vector<uint8_t> newVector{};
			std::string newString = data.dump();
			newString.shrink_to_fit();
			newVector.insert(newVector.begin(), newString.begin(), newString.end());
			return newVector;
		}
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

	/// Base class DiscordCoreAPI_Dll for all Discord entities. \brief Base class DiscordCoreAPI_Dll for all Discord entities.
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

		RoleTagsData& operator=(const nlohmann ::json& jsonObjectData) {
			this->parseObject(jsonObjectData, this);
			return *this;
		}

		RoleTagsData(const nlohmann ::json& jsonObjectData) {
			*this = jsonObjectData;
		}

		virtual ~RoleTagsData() = default;

	  protected:
		void parseObject(const nlohmann::json& jsonObjectData, RoleTagsData* pDataStructure) {
			if (jsonObjectData.contains("bot_id") && !jsonObjectData["bot_id"].is_null()) {
				pDataStructure->botId = jsonObjectData["bot_id"].get<std::string>();
			}

			if (jsonObjectData.contains("integration_id") && !jsonObjectData["integration_id"].is_null()) {
				pDataStructure->integrationId = jsonObjectData["integration_id"].get<std::string>();
			}
		}
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

		UserData& operator=(const nlohmann::json& jsonObjectData) {
			this->parseObject(jsonObjectData, this);
			return *this;
		}

		UserData(const nlohmann::json& jsonObjectData) {
			*this = jsonObjectData;
		}

		virtual ~UserData() = default;

	  protected:
		void parseObject(const nlohmann::json& jsonObjectData, UserData* pDataStructure) {
			if (jsonObjectData.contains("username") && !jsonObjectData["username"].is_null()) {
				pDataStructure->userName = jsonObjectData["username"].get<std::string>();
			}

			if (jsonObjectData.contains("id") && !jsonObjectData["id"].is_null()) {
				if (jsonObjectData["id"].is_string()) {
					pDataStructure->id = stoull(jsonObjectData["id"].get<std::string>());
				} else {
					pDataStructure->id = jsonObjectData["id"].get<int64_t>();
				}
			}

			if (jsonObjectData.contains("discriminator") && !jsonObjectData["discriminator"].is_null()) {
				pDataStructure->discriminator = jsonObjectData["discriminator"].get<std::string>();
			}

			if (jsonObjectData.contains("avatar") && !jsonObjectData["avatar"].is_null()) {
				std::string avatarString = "https://cdn.discordapp.com/avatars/" + std::to_string(pDataStructure->id) + "/" + jsonObjectData["avatar"].get<std::string>();
				pDataStructure->avatar = avatarString;
			}

			if (jsonObjectData.contains("bot") && !jsonObjectData["bot"].is_null()) {
				pDataStructure->flags = setBool<int32_t, UserFlags>(pDataStructure->flags, UserFlags::Bot, jsonObjectData["bot"].get<bool>());
			}

			if (jsonObjectData.contains("system") && !jsonObjectData["system"].is_null()) {
				pDataStructure->flags = setBool<int32_t, UserFlags>(pDataStructure->flags, UserFlags::System, jsonObjectData["system"].get<bool>());
			}

			if (jsonObjectData.contains("mfa_enabled") && !jsonObjectData["mfa_enabled"].is_null()) {
				pDataStructure->flags = setBool<int32_t, UserFlags>(pDataStructure->flags, UserFlags::MFAEnabled, jsonObjectData["mfa_enabled"].get<bool>());
			}

			if (jsonObjectData.contains("verified") && !jsonObjectData["verified"].is_null()) {
				pDataStructure->flags = setBool<int32_t, UserFlags>(pDataStructure->flags, UserFlags::Verified, jsonObjectData["verified"].get<bool>());
			}

			if (jsonObjectData.contains("public_flags") && !jsonObjectData["public_flags"].is_null()) {
				pDataStructure->flags = jsonObjectData["public_flags"].get<int32_t>();
			}
		}
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

		operator nlohmann::json() {
			nlohmann::json newValue{};
			newValue["content_type"] = this->contentType;
			newValue["description"] = this->description;
			newValue["ephemeral"] = this->ephemeral;
			newValue["file_name"] = this->filename;
			newValue["proxy_url"] = this->proxyUrl;
			newValue["height"] = this->height;
			newValue["width"] = this->width;
			newValue["size"] = this->size;
			newValue["url"] = this->url;
			return newValue;
		}

		AttachmentData& operator=(const nlohmann::json& jsonObjectData) {
			this->parseObject(jsonObjectData, this);
			return *this;
		}

		AttachmentData(const nlohmann::json& jsonObjectData) {
			*this = jsonObjectData;
		}

		virtual ~AttachmentData() = default;

	  protected:
		void parseObject(const nlohmann::json& jsonObjectData, AttachmentData* pDataStructure) {
			if (jsonObjectData.contains("id") && !jsonObjectData["id"].is_null()) {
				pDataStructure->id = stoull(jsonObjectData["id"].get<std::string>());
			}

			if (jsonObjectData.contains("filename") && !jsonObjectData["filename"].is_null()) {
				pDataStructure->filename = jsonObjectData["filename"].get<std::string>();
			}

			if (jsonObjectData.contains("content_type") && !jsonObjectData["content_type"].is_null()) {
				pDataStructure->contentType = jsonObjectData["content_type"].get<std::string>();
			}

			if (jsonObjectData.contains("ephemeral") && !jsonObjectData["ephemeral"].is_null()) {
				pDataStructure->ephemeral = jsonObjectData["ephemeral"].get<bool>();
			}

			if (jsonObjectData.contains("description") && !jsonObjectData["description"].is_null()) {
				pDataStructure->description = jsonObjectData["description"].get<std::string>();
			}

			if (jsonObjectData.contains("size") && !jsonObjectData["size"].is_null()) {
				pDataStructure->size = jsonObjectData["size"].get<int32_t>();
			}

			if (jsonObjectData.contains("url") && !jsonObjectData["url"].is_null()) {
				pDataStructure->url = jsonObjectData["url"].get<std::string>();
			}

			if (jsonObjectData.contains("proxy_url") && !jsonObjectData["proxy_url"].is_null()) {
				pDataStructure->proxyUrl = jsonObjectData["proxy_url"].get<std::string>();
			}

			if (jsonObjectData.contains("width") && !jsonObjectData["width"].is_null()) {
				pDataStructure->width = jsonObjectData["width"].get<int32_t>();
			}

			if (jsonObjectData.contains("height") && !jsonObjectData["height"].is_null()) {
				pDataStructure->height = jsonObjectData["height"].get<int32_t>();
			}
		}
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

		EmbedFooterData& operator=(const nlohmann::json& jsonObjectData) {
			this->parseObject(jsonObjectData, this);
			return *this;
		}

		EmbedFooterData(const nlohmann::json& jsonObjectData) {
			*this = jsonObjectData;
		}

		virtual ~EmbedFooterData() = default;

	  protected:
		void parseObject(const nlohmann::json& jsonObjectData, EmbedFooterData* pDataStructure) {
			if (jsonObjectData.contains("text") && !jsonObjectData["text"].is_null()) {
				pDataStructure->text = jsonObjectData["text"].get<std::string>();
			}

			if (jsonObjectData.contains("icon_url") && !jsonObjectData["icon_url"].is_null()) {
				pDataStructure->iconUrl = jsonObjectData["icon_url"].get<std::string>();
			}

			if (jsonObjectData.contains("proxy_icon_url") && !jsonObjectData["proxy_icon_url"].is_null()) {
				pDataStructure->proxyIconUrl = jsonObjectData["proxy_icon_url"].get<std::string>();
			}
		}
	};

	/// Embed image data. \brief Embed image data.
	struct DiscordCoreAPI_Dll EmbedImageData : public DataParser<EmbedImageData> {
		std::string proxyUrl{};///< Proxy url.
		int32_t height{ 0 };///< Image height.
		int32_t width{ 0 };///< Image width.
		std::string url{};///< Image url.

		EmbedImageData() = default;

		EmbedImageData& operator=(const nlohmann::json& jsonObjectData) {
			this->parseObject(jsonObjectData, this);
			return *this;
		}

		EmbedImageData(const nlohmann::json& jsonObjectData) {
			*this = jsonObjectData;
		}

		virtual ~EmbedImageData() = default;

	  protected:
		void parseObject(const nlohmann::json& jsonObjectData, EmbedImageData* pDataStructure) {
			if (jsonObjectData.contains("url") && !jsonObjectData["url"].is_null()) {
				pDataStructure->url = jsonObjectData["url"].get<std::string>();
			}

			if (jsonObjectData.contains("proxy_url") && !jsonObjectData["proxy_url"].is_null()) {
				pDataStructure->proxyUrl = jsonObjectData["proxy_url"].get<std::string>();
			}

			if (jsonObjectData.contains("width") && !jsonObjectData["width"].is_null()) {
				pDataStructure->width = jsonObjectData["width"].get<int32_t>();
			}

			if (jsonObjectData.contains("height") && !jsonObjectData["height"].is_null()) {
				pDataStructure->height = jsonObjectData["height"].get<int32_t>();
			}
		}
	};

	/// Embed thumbnail data. \brief Embed thumbnail data.
	struct DiscordCoreAPI_Dll EmbedThumbnailData : public DataParser<EmbedThumbnailData> {
		std::string proxyUrl{};///< Proxy url.
		int32_t height{ 0 };///< Image height.
		int32_t width{ 0 };///< Image width.
		std::string url{};///< Image url.

		EmbedThumbnailData() = default;

		EmbedThumbnailData& operator=(const nlohmann::json& jsonObjectData) {
			this->parseObject(jsonObjectData, this);
			return *this;
		}

		EmbedThumbnailData(const nlohmann::json& jsonObjectData) {
			*this = jsonObjectData;
		}

		virtual ~EmbedThumbnailData() = default;

	  protected:
		void parseObject(const nlohmann::json& jsonObjectData, EmbedThumbnailData* pDataStructure) {
			if (jsonObjectData.contains("url") && !jsonObjectData["url"].is_null()) {
				pDataStructure->url = jsonObjectData["url"].get<std::string>();
			}

			if (jsonObjectData.contains("proxy_url") && !jsonObjectData["proxy_url"].is_null()) {
				pDataStructure->proxyUrl = jsonObjectData["proxy_url"].get<std::string>();
			}

			if (jsonObjectData.contains("width") && !jsonObjectData["width"].is_null()) {
				pDataStructure->width = jsonObjectData["width"].get<int32_t>();
			}

			if (jsonObjectData.contains("height") && !jsonObjectData["height"].is_null()) {
				pDataStructure->height = jsonObjectData["height"].get<int32_t>();
			}
		}
	};

	/// Embed video data. \brief Embed video data.
	struct DiscordCoreAPI_Dll EmbedVideoData : public DataParser<EmbedVideoData> {
		std::string proxyUrl{};///< Proxy url.
		int32_t height{ 0 };///< Image height.
		int32_t width{ 0 };///< Image width.
		std::string url{};///< Image url.

		EmbedVideoData() = default;

		EmbedVideoData& operator=(const nlohmann::json& jsonObjectData) {
			this->parseObject(jsonObjectData, this);
			return *this;
		}

		EmbedVideoData(const nlohmann::json& jsonObjectData) {
			*this = jsonObjectData;
		}

		virtual ~EmbedVideoData() = default;

	  protected:
		void parseObject(const nlohmann::json& jsonObjectData, EmbedVideoData* pDataStructure) {
			if (jsonObjectData.contains("url") && !jsonObjectData["url"].is_null()) {
				pDataStructure->url = jsonObjectData["url"].get<std::string>();
			}

			if (jsonObjectData.contains("proxy_url") && !jsonObjectData["proxy_url"].is_null()) {
				pDataStructure->proxyUrl = jsonObjectData["proxy_url"].get<std::string>();
			}

			if (jsonObjectData.contains("width") && !jsonObjectData["width"].is_null()) {
				pDataStructure->width = jsonObjectData["width"].get<int32_t>();
			}

			if (jsonObjectData.contains("height") && !jsonObjectData["height"].is_null()) {
				pDataStructure->height = jsonObjectData["height"].get<int32_t>();
			}
		}
	};

	/// Embed provider data. \brief Embed provider data.
	struct DiscordCoreAPI_Dll EmbedProviderData : public DataParser<EmbedProviderData> {
		std::string name{};///< Name.
		std::string url{};///< Url.

		EmbedProviderData() = default;

		EmbedProviderData& operator=(const nlohmann::json& jsonObjectData) {
			this->parseObject(jsonObjectData, this);
			return *this;
		}

		EmbedProviderData(const nlohmann::json& jsonObjectData) {
			*this = jsonObjectData;
		}

		virtual ~EmbedProviderData() = default;

	  protected:
		void parseObject(const nlohmann::json& jsonObjectData, EmbedProviderData* pDataStructure) {
			if (jsonObjectData.contains("url") && !jsonObjectData["url"].is_null()) {
				pDataStructure->url = jsonObjectData["url"].get<std::string>();
			}

			if (jsonObjectData.contains("name") && !jsonObjectData["name"].is_null()) {
				pDataStructure->name = jsonObjectData["name"].get<std::string>();
			}
		}
	};

	/// Embed author data. \brief Embed author data.
	struct DiscordCoreAPI_Dll EmbedAuthorData : public DataParser<EmbedAuthorData> {
		std::string proxyIconUrl{};///< Proxy icon url.
		std::string iconUrl{};///< Icon url.
		std::string name{};///< Name.
		std::string url{};///< Url.

		EmbedAuthorData() = default;

		EmbedAuthorData& operator=(const nlohmann::json& jsonObjectData) {
			this->parseObject(jsonObjectData, this);
			return *this;
		}

		EmbedAuthorData(const nlohmann::json& jsonObjectData) {
			*this = jsonObjectData;
		}

		virtual ~EmbedAuthorData() = default;

	  protected:
		void parseObject(const nlohmann::json& jsonObjectData, EmbedAuthorData* pDataStructure) {
			if (jsonObjectData.contains("url") && !jsonObjectData["url"].is_null()) {
				pDataStructure->url = jsonObjectData["url"].get<std::string>();
			}

			if (jsonObjectData.contains("proxy_icon_url") && !jsonObjectData["proxy_icon_url"].is_null()) {
				pDataStructure->proxyIconUrl = jsonObjectData["proxy_icon_url"].get<std::string>();
			}

			if (jsonObjectData.contains("name") && !jsonObjectData["name"].is_null()) {
				pDataStructure->name = jsonObjectData["name"].get<std::string>();
			}

			if (jsonObjectData.contains("icon_url") && !jsonObjectData["icon_url"].is_null()) {
				pDataStructure->iconUrl = jsonObjectData["icon_url"].get<std::string>();
			}
		}
	};

	/// Embed field data. \brief Embed field data.
	struct DiscordCoreAPI_Dll EmbedFieldData : public DataParser<EmbedFieldData> {
		bool Inline{ false };///< Is the field inline with the rest of them?
		std::string value{};///< The text on the field.
		std::string name{};///< The title of the field.

		EmbedFieldData() = default;

		operator nlohmann::json() {
			nlohmann::json newValue{};
			newValue["inline"] = this->Inline;
			newValue["value"] = this->value;
			newValue["name"] = this->name;
			return newValue;
		}

		EmbedFieldData& operator=(const nlohmann::json& jsonObjectData) {
			this->parseObject(jsonObjectData, this);
			return *this;
		}

		EmbedFieldData(const nlohmann::json& jsonObjectData) {
			*this = jsonObjectData;
		}

		virtual ~EmbedFieldData() = default;

	  protected:
		void parseObject(const nlohmann::json& jsonObjectData, EmbedFieldData* pDataStructure) {
			if (jsonObjectData.contains("inline") && !jsonObjectData["inline"].is_null()) {
				pDataStructure->Inline = jsonObjectData["inline"].get<bool>();
			}

			if (jsonObjectData.contains("name") && !jsonObjectData["name"].is_null()) {
				pDataStructure->name = jsonObjectData["name"].get<std::string>();
			}

			if (jsonObjectData.contains("value") && !jsonObjectData["value"].is_null()) {
				pDataStructure->value = jsonObjectData["value"].get<std::string>();
			}
		}
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

		operator nlohmann::json() {
			nlohmann::json fields{};
			for (auto& value2: this->fields) {
				fields.push_back(value2);
			}
			int32_t colorValInt = stol(this->hexColorValue, 0, 16);
			std::stringstream stream;
			stream << std::setbase(10) << colorValInt;
			std::string realColorVal = stream.str();
			nlohmann::json embed{};
			embed["footer"]["proxy_icon_url"] = this->footer.proxyIconUrl;
			embed["author"]["proxy_icon_url"] = this->author.proxyIconUrl;
			embed["thumbnail"]["proxy_url"] = this->thumbnail.proxyUrl;
			embed["thumbnail"]["height"] = this->thumbnail.height;
			embed["thumbnail"]["width"] = this->thumbnail.width;
			embed["image"]["proxy_url"] = this->image.proxyUrl;
			embed["author"]["icon_url"] = this->author.iconUrl;
			embed["footer"]["icon_url"] = this->footer.iconUrl;
			embed["video"]["proxy_url"] = this->video.proxyUrl;
			embed["provider"]["name"] = this->provider.name;
			embed["thumbnail"]["url"] = this->thumbnail.url;
			embed["provider"]["url"] = this->provider.url;
			embed["video"]["height"] = this->video.height;
			embed["image"]["height"] = this->image.height;
			embed["author"]["name"] = this->author.name;
			embed["image"]["width"] = this->image.width;
			embed["footer"]["text"] = this->footer.text;
			embed["video"]["width"] = this->video.width;
			embed["author"]["url"] = this->author.url;
			embed["description"] = this->description;
			embed["image"]["url"] = this->image.url;
			embed["video"]["url"] = this->video.url;
			embed["timestamp"] = this->timestamp;
			embed["title"] = this->title;
			embed["color"] = realColorVal;
			embed["type"] = this->type;
			embed["url"] = this->url;
			embed["fields"] = fields;
			return embed;
		}

		EmbedData& operator=(const nlohmann::json& jsonObjectData) {
			this->parseObject(jsonObjectData, this);
			return *this;
		}

		EmbedData(const nlohmann::json& jsonObjectData) {
			*this = jsonObjectData;
		}

		/// Sets the author's name and avatar for the embed. \brief Sets the author's name and avatar for the embed.
		/// \param authorName The author's name.
		/// \param authorAvatarUrl The url to their avatar.
		/// \returns EmbedData& A reference to this embed.
		EmbedData& setAuthor(const std::string& authorName, const std::string& authorAvatarUrl = "") {
			this->author.name = authorName;
			this->author.iconUrl = authorAvatarUrl;
			return *this;
		}

		/// Sets the footer's values for the embed. \brief Sets the footer's values for the embed.
		/// \param footerText The footer's text.
		/// \param footerIconUrlText Url to the footer's icon.
		/// \returns EmbedData& A reference to this embed.
		EmbedData& setFooter(const std::string& footerText, const std::string& footerIconUrlText = "") {
			this->footer.text = footerText;
			this->footer.iconUrl = footerIconUrlText;
			return *this;
		}

		/// Sets the timestamp on the embed. \brief Sets the timestamp on the embed.
		/// \param timeStamp The timestamp to be set.
		/// \returns EmbedData& A reference to this embed.
		EmbedData& setTimeStamp(const std::string& timeStamp) {
			this->timestamp = timeStamp;
			return *this;
		}

		/// Adds a field to the embed. \brief Adds a field to the embed.
		/// \param name The title of the embed field.
		/// \param value The contents of the embed field.
		/// \param Inline Is it inline with the rest of the fields on the embed?
		/// \returns EmbedData& A reference to this embed.
		EmbedData& addField(const std::string& name, const std::string& value, bool Inline = true) {
			EmbedFieldData field{};
			field.Inline = Inline;
			field.value = value;
			field.name = name;
			this->fields.push_back(field);
			return *this;
		}

		/// Sets the description (the main contents) of the embed. \brief Sets the description (the main contents) of the embed.
		/// \param descriptionNew The contents of the description to set.
		/// \returns EmbedData& A reference to this embed.
		EmbedData& setDescription(const std::string& descriptionNew) {
			this->description = descriptionNew;
			return *this;
		}

		/// Sets the color of the embed, by applying a hex-color value. \brief Sets the color of the embed, by applying a hex-color value.
		/// \param hexColorValueNew A std::string containing a hex-number value (Between 0x00 0xFFFFFF).
		/// \returns EmbedData& A reference to this embed.
		EmbedData& setColor(const std::string& hexColorValueNew) {
			this->hexColorValue = hexColorValueNew;
			return *this;
		}

		/// Sets the thumbnail of the embed. \brief Sets the thumbnail of the embed.
		/// \param thumbnailUrl The url to the thumbnail to be used.
		/// \returns EmbedData& A reference to this embed.
		EmbedData& setThumbnail(const std::string& thumbnailUrl) {
			this->thumbnail.url = thumbnailUrl;
			return *this;
		}

		/// Sets the title of the embed. \brief Sets the title of the embed.
		/// \param titleNew A std::string containing the desired title.
		/// \returns EmbedData& A reference to this embed.
		EmbedData& setTitle(const std::string& titleNew) {
			this->title = titleNew;
			return *this;
		}

		/// Sets the image of the embed. \brief Sets the image of the embed.
		/// \param imageUrl The url of the image to be set on the embed.
		/// \returns EmbedData& A reference to this embed.
		EmbedData& setImage(const std::string& imageUrl) {
			this->image.url = imageUrl;
			return *this;
		}

		virtual ~EmbedData() = default;

	  protected:
		void parseObject(const nlohmann::json& jsonObjectData, EmbedData* pDataStructure) {
			if (jsonObjectData.contains("title") && !jsonObjectData["title"].is_null()) {
				pDataStructure->title = jsonObjectData["title"].get<std::string>();
			}

			if (jsonObjectData.contains("type") && !jsonObjectData["type"].is_null()) {
				pDataStructure->type = jsonObjectData["type"].get<std::string>();
			}

			if (jsonObjectData.contains("description") && !jsonObjectData["description"].is_null()) {
				pDataStructure->description = jsonObjectData["description"].get<std::string>();
			}

			if (jsonObjectData.contains("url") && !jsonObjectData["url"].is_null()) {
				pDataStructure->url = jsonObjectData["url"].get<std::string>();
			}

			if (jsonObjectData.contains("timestamp") && !jsonObjectData["timestamp"].is_null()) {
				pDataStructure->timestamp = jsonObjectData["timestamp"].get<std::string>();
			}

			if (jsonObjectData.contains("color") && !jsonObjectData["color"].is_null()) {
				int32_t colorValInt = jsonObjectData["color"].get<int32_t>();
				std::stringstream stream{};
				stream << std::setbase(16) << colorValInt;
				pDataStructure->hexColorValue = stream.str();
			}

			if (jsonObjectData.contains("footer") && !jsonObjectData["footer"].is_null()) {
				pDataStructure->footer = jsonObjectData["footer"];
			}

			if (jsonObjectData.contains("image") && !jsonObjectData["image"].is_null()) {
				pDataStructure->image = jsonObjectData["image"];
			}

			if (jsonObjectData.contains("provider") && !jsonObjectData["provider"].is_null()) {
				pDataStructure->provider = jsonObjectData["provider"];
			}

			if (jsonObjectData.contains("thumbnail") && !jsonObjectData["thumbnail"].is_null()) {
				pDataStructure->thumbnail = jsonObjectData["thumbnail"];
			}

			if (jsonObjectData.contains("video") && !jsonObjectData["video"].is_null()) {
				pDataStructure->video = jsonObjectData["video"];
			}

			if (jsonObjectData.contains("author") && !jsonObjectData["author"].is_null()) {
				pDataStructure->author = jsonObjectData["author"];
			}

			if (jsonObjectData.contains("fields") && !jsonObjectData["fields"].is_null()) {
				pDataStructure->fields.clear();
				pDataStructure->fields.reserve(jsonObjectData["fields"].size());
				for (auto& value: jsonObjectData["fields"]) {
					EmbedFieldData newData{ value };
					pDataStructure->fields.push_back(newData);
				}
				pDataStructure->fields.shrink_to_fit();
			}
		}
	};

	/// Message reference data.\brief Message reference data.
	struct DiscordCoreAPI_Dll MessageReferenceData : public DataParser<MessageReferenceData> {
		bool failIfNotExists{ false };///< Fail if the Message doesn't exist?
		Snowflake messageId{};///< Id of the Message to reference.
		Snowflake channelId{};///< Id of the Channel that the referenced Message was sent in.
		Snowflake guildId{};///< Id of the Guild that the referenced Message was sent in.

		MessageReferenceData() = default;

		operator nlohmann::json() {
			nlohmann::json newValue{};
			newValue["fail_if_not_exists"] = this->failIfNotExists;
			newValue["message_id"] = this->messageId;
			newValue["channel_id"] = this->channelId;
			newValue["guild_id"] = this->guildId;
			return newValue;
		}

		MessageReferenceData& operator=(const nlohmann::json& jsonObjectData) {
			this->parseObject(jsonObjectData, this);
			return *this;
		}

		MessageReferenceData(const nlohmann::json& jsonObjectData) {
			*this = jsonObjectData;
		}

		virtual ~MessageReferenceData() = default;

	  protected:
		void parseObject(const nlohmann::json& jsonObjectData, MessageReferenceData* pDataStructure) {
			if (jsonObjectData.contains("message_id") && !jsonObjectData["message_id"].is_null()) {
				pDataStructure->messageId = stoull(jsonObjectData["message_id"].get<std::string>());
			}

			if (jsonObjectData.contains("channel_id") && !jsonObjectData["channel_id"].is_null()) {
				pDataStructure->channelId = stoull(jsonObjectData["channel_id"].get<std::string>());
			}

			if (jsonObjectData.contains("guild_id") && !jsonObjectData["guild_id"].is_null()) {
				pDataStructure->guildId = stoull(jsonObjectData["guild_id"].get<std::string>());
			}

			if (jsonObjectData.contains("fail_if_not_exists") && !jsonObjectData["fail_if_not_exists"].is_null()) {
				pDataStructure->failIfNotExists = jsonObjectData["fail_if_not_exists"].get<bool>();
			}
		}
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
		int32_t autoArchiveDuration{ 0 };///< How int64_t before archiving this Thread.
		TimeStamp archiveTimestamp{ "" };///< (Where applicable) the time at which this Thread was archived.
		bool invitable{ false };///< The id of the individual who archived this Thread.
		bool archived{ false };///< Whether or not this Thread is currently archived.
		bool locked{ false };///< Whether or not this Thread is currently locked.

		ThreadMetadataData() = default;

		ThreadMetadataData& operator=(const nlohmann::json& jsonObjectData) {
			this->parseObject(jsonObjectData, this);
			return *this;
		}

		ThreadMetadataData(const nlohmann::json& other) {
			*this = other;
		}

		virtual ~ThreadMetadataData() = default;

	  protected:
		void parseObject(const nlohmann::json& jsonObjectData, ThreadMetadataData* pDataStructure) {
			if (jsonObjectData.contains("archived") && !jsonObjectData["archived"].is_null()) {
				pDataStructure->archived = jsonObjectData["archived"].get<bool>();
			}

			if (jsonObjectData.contains("invitable") && !jsonObjectData["invitable"].is_null()) {
				pDataStructure->invitable = jsonObjectData["invitable"].get<bool>();
			}

			if (jsonObjectData.contains("auto_archive_duration") && !jsonObjectData["auto_archive_duration"].is_null()) {
				pDataStructure->autoArchiveDuration = jsonObjectData["auto_archive_duration"].get<int32_t>();
			}

			if (jsonObjectData.contains("archive_timestamp") && !jsonObjectData["archive_timestamp"].is_null()) {
				pDataStructure->archiveTimestamp = jsonObjectData["archive_timestamp"].get<std::string>();
			}

			if (jsonObjectData.contains("locked") && !jsonObjectData["locked"].is_null()) {
				pDataStructure->locked = jsonObjectData["locked"].get<bool>();
			}
		}
	};

	/// Data for a single member of a Thread. \brief Data for a single member of a Thread.
	class DiscordCoreAPI_Dll ThreadMemberData : public DiscordEntity, public DataParser<ThreadMemberData> {		
	  public:
		TimeStamp joinTimestamp{ "" };///< The time at which the member joined this Thread.
		int32_t flags{ 0 };///< Flags.
		Snowflake userId{};///< The User's id.

		ThreadMemberData() = default;

		ThreadMemberData& operator=(const nlohmann::json& jsonObjectData) {
			this->parseObject(jsonObjectData, this);
			return *this;
		}

		ThreadMemberData(const nlohmann::json& other) {
			*this = other;
		}

		virtual ~ThreadMemberData() = default;

	  protected:
		void parseObject(const nlohmann::json& jsonObjectData, ThreadMemberData* pDataStructure) {
			if (jsonObjectData.contains("id") && !jsonObjectData["id"].is_null()) {
				pDataStructure->id = stoull(jsonObjectData["id"].get<std::string>());
			}

			if (jsonObjectData.contains("user_id") && !jsonObjectData["user_id"].is_null()) {
				pDataStructure->userId = stoull(jsonObjectData["user_id"].get<std::string>());
			}

			if (jsonObjectData.contains("join_timestamp") && !jsonObjectData["join_timestamp"].is_null()) {
				pDataStructure->joinTimestamp = jsonObjectData["join_timestamp"].get<std::string>();
			}

			if (jsonObjectData.contains("flags") && !jsonObjectData["flags"].is_null()) {
				pDataStructure->flags = jsonObjectData["flags"].get<int32_t>();
			}
		}
	};

	class DiscordCoreAPI_Dll ThreadMemberDataVector : public DataParser<ThreadMemberDataVector> {
	  public:
		ThreadMemberDataVector() = default;

		operator std::vector<ThreadMemberData>() {
			return this->theThreadMemberDatas;
		}

		ThreadMemberDataVector& operator=(const nlohmann::json& jsonObjectData) {
			this->parseObject(jsonObjectData, this);
			return *this;
		}

		ThreadMemberDataVector(const nlohmann::json& jsonObjectData) {
			*this = jsonObjectData;
		}

		virtual ~ThreadMemberDataVector() = default;

	  protected:
		std::vector<ThreadMemberData> theThreadMemberDatas{};

		void parseObject(const nlohmann::json& jsonObjectData, ThreadMemberDataVector* pDataStructure) {
			pDataStructure->theThreadMemberDatas.reserve(jsonObjectData.size());
			for (auto& value: jsonObjectData) {
				DiscordCoreAPI::ThreadMemberData newData{ value };
				pDataStructure->theThreadMemberDatas.push_back(newData);
			}
			pDataStructure->theThreadMemberDatas.shrink_to_fit();
		}
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

	inline bool operator==(const GuildMemberId lhs, const GuildMemberId rhs) {
		if (lhs.guildMemberId == rhs.guildMemberId && lhs.guildId == rhs.guildId) {
			return true;
		} else {
			return false;
		}
	}

	inline bool operator<(const GuildMemberId& lhs, const GuildMemberId& rhs) {
		if ((lhs.guildId + lhs.guildMemberId) < (rhs.guildId + rhs.guildMemberId)) {
			return true;
		} else {
			return false;
		}
	}

	/// Data structure representing a single GuildMember. \brief Data structure representing a single GuildMember.
	/// Data structure representing a single Guild. \brief Data structure representing a single Guild.
	class DiscordCoreAPI_Dll GuildMemberData : public DiscordEntity, public DataParser<GuildMemberData> {
	  public:
		std::vector<Snowflake> roles{};///< The Guild roles that they have.
		Permissions permissions{};///< Their base-level Permissions in the Guild.
		TimeStamp joinedAt{ "" };///< When they joined the Guild.
		StringWrapper userAvatar{};///< This GuildMember's User Avatar.
		StringWrapper userName{};///< This GuildMember's UserName.
		Snowflake guildId{};///< The current Guild's id.
		StringWrapper nick{};///< Their nick/display name.
		int8_t flags{ 0 };///< GuildMember flags.

		GuildMemberData() = default;

		GuildMemberData& operator=(const nlohmann::json& jsonObjectData) {
			this->parseObject(jsonObjectData, this);
			return *this;
		}

		GuildMemberData(const nlohmann::json& jsonObjectData) {
			*this = jsonObjectData;
		}

		void insertUser(UserData theUser);

		virtual ~GuildMemberData() = default;

	  protected:
		void parseObject(const nlohmann::json& jsonObjectData, GuildMemberData* pDataStructure) {
			if (jsonObjectData.contains("roles") && !jsonObjectData["roles"].is_null()) {
				for (auto& value: jsonObjectData["roles"].get<std::vector<std::string>>()) {
					pDataStructure->roles.push_back(stoull(value));
				}
			}

			if (jsonObjectData.contains("permissions") && !jsonObjectData["permissions"].is_null()) {
				pDataStructure->permissions = jsonObjectData["permissions"].get<std::string>();
			}

			if (jsonObjectData.contains("joined_at") && !jsonObjectData["joined_at"].is_null()) {
				pDataStructure->joinedAt = jsonObjectData["joined_at"].get<std::string>();
			}

			if (jsonObjectData.contains("guild_id") && !jsonObjectData["guild_id"].is_null()) {
				pDataStructure->guildId = stoull(jsonObjectData["guild_id"].get<std::string>());
			}

			if (jsonObjectData.contains("nick") && !jsonObjectData["nick"].is_null()) {
				pDataStructure->nick = jsonObjectData["nick"].get<std::string>();
			}

			if (jsonObjectData.contains("user") && !jsonObjectData["user"].is_null()) {
				UserData theUser{ jsonObjectData["user"] };
				this->insertUser(theUser);
				pDataStructure->id = theUser.id;
				pDataStructure->userAvatar = theUser.avatar;
				pDataStructure->userName = theUser.userName;
			}

			if (jsonObjectData.contains("flags") && !jsonObjectData["flags"].is_null()) {
				pDataStructure->flags = jsonObjectData["flags"].get<int8_t>();
			}

			if (jsonObjectData.contains("pending") && !jsonObjectData["pending"].is_null()) {
				pDataStructure->flags = setBool<int8_t, GuildMemberFlags>(pDataStructure->flags, GuildMemberFlags::Pending, jsonObjectData["pending"].get<bool>());
			}

			if (jsonObjectData.contains("mute") && !jsonObjectData["mute"].is_null()) {
				pDataStructure->flags = setBool<int8_t, GuildMemberFlags>(pDataStructure->flags, GuildMemberFlags::Mute, jsonObjectData["mute"].get<bool>());
			}

			if (jsonObjectData.contains("deaf") && !jsonObjectData["deaf"].is_null()) {
				pDataStructure->flags = setBool<int8_t, GuildMemberFlags>(pDataStructure->flags, GuildMemberFlags::Deaf, jsonObjectData["deaf"].get<bool>());
			}
		}
	};

	/// Voice state data. \brief Voice state data.
	struct DiscordCoreAPI_Dll VoiceStateData : public DataParser<VoiceStateData> {
		TimeStamp requestToSpeakTimestamp{ "" };///< The time at which the User requested to speak.
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

		VoiceStateData& operator=(const nlohmann::json& jsonObjectData) {
			this->parseObject(jsonObjectData, this);
			return *this;
		}

		VoiceStateData(const nlohmann::json& jsonObjectData) {
			*this = jsonObjectData;
		}
	
		virtual ~VoiceStateData() = default;

	  protected:
		void parseObject(const nlohmann::json& jsonObjectData, VoiceStateData* pDataStructure) {
			if (jsonObjectData.contains("guild_id") && !jsonObjectData["guild_id"].is_null()) {
				pDataStructure->guildId = stoull(jsonObjectData["guild_id"].get<std::string>());
			}

			if (jsonObjectData.contains("channel_id") && !jsonObjectData["channel_id"].is_null()) {
				pDataStructure->channelId = stoull(jsonObjectData["channel_id"].get<std::string>());
			}

			if (jsonObjectData.contains("user_id") && !jsonObjectData["user_id"].is_null()) {
				pDataStructure->userId = stoull(jsonObjectData["user_id"].get<std::string>());
			}

			if (jsonObjectData.contains("member") && !jsonObjectData["member"].is_null()) {
				pDataStructure->member = jsonObjectData["member"];
			}

			if (jsonObjectData.contains("session_id") && !jsonObjectData["session_id"].is_null()) {
				pDataStructure->sessionId = jsonObjectData["session_id"].get<std::string>();
			}

			if (jsonObjectData.contains("deaf") && !jsonObjectData["deaf"].is_null()) {
				pDataStructure->deaf = jsonObjectData["deaf"].get<bool>();
			}

			if (jsonObjectData.contains("mute") && !jsonObjectData["mute"].is_null()) {
				pDataStructure->mute = jsonObjectData["mute"].get<bool>();
			}

			if (jsonObjectData.contains("self_deaf") && !jsonObjectData["self_deaf"].is_null()) {
				pDataStructure->selfDeaf = jsonObjectData["self_deaf"].get<bool>();
			}

			if (jsonObjectData.contains("self_mute") && !jsonObjectData["self_mute"].is_null()) {
				pDataStructure->selfMute = jsonObjectData["self_mute"].get<bool>();
			}

			if (jsonObjectData.contains("self_stream") && !jsonObjectData["self_stream"].is_null()) {
				pDataStructure->selfStream = jsonObjectData["self_stream"].get<bool>();
			}

			if (jsonObjectData.contains("self_video") && !jsonObjectData["self_video"].is_null()) {
				pDataStructure->selfVideo = jsonObjectData["self_video"].get<bool>();
			}

			if (jsonObjectData.contains("suppress") && !jsonObjectData["suppress"].is_null()) {
				pDataStructure->suppress = jsonObjectData["suppress"].get<bool>();
			}

			if (jsonObjectData.contains("request_to_speak_timestamp") && !jsonObjectData["request_to_speak_timestamp"].is_null()) {
				pDataStructure->requestToSpeakTimestamp = jsonObjectData["request_to_speak_timestamp"].get<std::string>();
			}
		}
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

		OverWriteData& operator=(const nlohmann::json& jsonObjectData) {
			this->parseObject(jsonObjectData, this);
			return *this;
		}

		OverWriteData(const nlohmann::json& jsonObjectData) {
			*this = jsonObjectData;
		}

		virtual ~OverWriteData() = default;

	  protected:
		void parseObject(const nlohmann::json& jsonObjectData, OverWriteData* pDataStructure) {
			if (jsonObjectData.contains("type") && !jsonObjectData["type"].is_null()) {
				pDataStructure->type = jsonObjectData["type"].get<PermissionOverwritesType>();
			}

			if (jsonObjectData.contains("allow") && !jsonObjectData["allow"].is_null()) {
				pDataStructure->allow = jsonObjectData["allow"].get<std::string>();
			}

			if (jsonObjectData.contains("deny") && !jsonObjectData["deny"].is_null()) {
				pDataStructure->deny = jsonObjectData["deny"].get<std::string>();
			}

			if (jsonObjectData.contains("id") && !jsonObjectData["id"].is_null()) {
				if (jsonObjectData["id"].is_string()) {
					pDataStructure->id = stoull(jsonObjectData["id"].get<std::string>());
				} else {
					pDataStructure->id = jsonObjectData["id"].get<int64_t>();
				}
			}
		}
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

		ChannelData& operator=(const nlohmann::json& jsonObjectData) {
			this->parseObject(jsonObjectData, this);
			return *this;
		}

		ChannelData(const nlohmann::json& other) {
			*this = other;
		}

		virtual ~ChannelData() = default;

	  protected:
		void parseObject(const nlohmann::json& jsonObjectData, ChannelData* pDataStructure) {
			if (jsonObjectData.contains("id") && !jsonObjectData["id"].is_null()) {
				if (jsonObjectData["id"].is_string()) {
					pDataStructure->id = stoull(jsonObjectData["id"].get<std::string>());
				} else {
					pDataStructure->id = jsonObjectData["id"].get<int64_t>();
				}
			}

			if (jsonObjectData.contains("flags") && !jsonObjectData["flags"].is_null()) {
				pDataStructure->flags = jsonObjectData["flags"].get<int8_t>();
			}

			if (jsonObjectData.contains("type") && !jsonObjectData["type"].is_null()) {
				pDataStructure->type = jsonObjectData["type"].get<ChannelType>();
			}

			if (jsonObjectData.contains("parent_id") && !jsonObjectData["parent_id"].is_null()) {
				pDataStructure->parentId = stoull(jsonObjectData["parent_id"].get<std::string>());
			}

			if (jsonObjectData.contains("guild_id") && !jsonObjectData["guild_id"].is_null()) {
				pDataStructure->guildId = stoull(jsonObjectData["guild_id"].get<std::string>());
			}

			if (jsonObjectData.contains("position") && !jsonObjectData["position"].is_null()) {
				pDataStructure->position = jsonObjectData["position"].get<int32_t>();
			}

			if (jsonObjectData.contains("permission_overwrites") && !jsonObjectData["permission_overwrites"].is_null()) {
				pDataStructure->permissionOverwrites.clear();
				for (auto& value: jsonObjectData["permission_overwrites"]) {
					OverWriteData newData{ value };
					Snowflake overWriteId = newData.id;
					pDataStructure->permissionOverwrites[overWriteId] = newData;
				}
			}

			if (jsonObjectData.contains("name") && !jsonObjectData["name"].is_null()) {
				pDataStructure->name = jsonObjectData["name"].get<std::string>();
			}

			if (jsonObjectData.contains("nsfw") && !jsonObjectData["nsfw"].is_null()) {
				pDataStructure->flags = setBool<int8_t, ChannelFlags>(pDataStructure->flags, ChannelFlags::NSFW, jsonObjectData["nsfw"].get<bool>());
			}

			if (jsonObjectData.contains("owner_id") && !jsonObjectData["owner_id"].is_null()) {
				pDataStructure->ownerId = stoull(jsonObjectData["owner_id"].get<std::string>());
			}

			if (jsonObjectData.contains("member_count") && !jsonObjectData["member_count"].is_null()) {
				pDataStructure->memberCount = jsonObjectData["member_count"].get<int32_t>();
			}
		}
	};


	/// Data representing an active Thread. \brief Data representing an active Thread.
	struct DiscordCoreAPI_Dll ActiveThreadsData : public DataParser<ActiveThreadsData> {
		std::vector<ThreadMemberData> members{};
		std::vector<ChannelData> threads{};
		bool hasMore{ false };

		ActiveThreadsData() = default;

		ActiveThreadsData& operator=(const nlohmann::json& jsonObjectData) {
			this->parseObject(jsonObjectData, this);
			return *this;
		}

		ActiveThreadsData(const nlohmann::json& jsonObjectData) {
			*this = jsonObjectData;
		}

		virtual ~ActiveThreadsData() = default;

	  protected:
		void parseObject(const nlohmann::json& jsonObjectData, ActiveThreadsData* pDataStructure) {
			if (jsonObjectData.contains("threads") && !jsonObjectData["threads"].is_null()) {
				pDataStructure->threads.clear();
				pDataStructure->threads.reserve(jsonObjectData["threads"].size());
				for (auto& value: jsonObjectData["threads"]) {
					ChannelData newData{ value };
					pDataStructure->threads.push_back(newData);
				}
				pDataStructure->threads.shrink_to_fit();
			}

			if (jsonObjectData.contains("members") && !jsonObjectData["members"].is_null()) {
				pDataStructure->members.clear();
				pDataStructure->members.reserve(jsonObjectData["members"].size());
				for (auto& value: jsonObjectData["members"]) {
					ThreadMemberData newData{};
					pDataStructure->members.push_back(newData);
				}
				pDataStructure->members.shrink_to_fit();
			}

			if (jsonObjectData.contains("has_more") && !jsonObjectData["has_more"].is_null()) {
				pDataStructure->hasMore = jsonObjectData["has_more"].get<bool>();
			}
		}
	};

	/// Data representing an archived Thread. \brief Data representing an archived Thread.
	struct DiscordCoreAPI_Dll ArchivedThreadsData : public DataParser<ArchivedThreadsData> {
		std::vector<ThreadMemberData> members{};
		std::vector<ChannelData> threads{};
		bool hasMore{ false };

		ArchivedThreadsData() = default;

		ArchivedThreadsData& operator=(const nlohmann::json& jsonObjectData) {
			this->parseObject(jsonObjectData, this);
			return *this;
		}

		ArchivedThreadsData(const nlohmann::json& jsonObjectData) {
			*this = jsonObjectData;
		}

		virtual ~ArchivedThreadsData() = default;

	  protected:
		void parseObject(const nlohmann::json& jsonObjectData, ArchivedThreadsData* pDataStructure) {
			if (jsonObjectData.contains("threads") && !jsonObjectData["threads"].is_null()) {
				pDataStructure->threads.clear();
				pDataStructure->threads.reserve(jsonObjectData["threads"].size());
				for (auto& value: jsonObjectData["threads"]) {
					ChannelData newData{ value };
					pDataStructure->threads.push_back(newData);
				}
				pDataStructure->threads.shrink_to_fit();
			}

			if (jsonObjectData.contains("members") && !jsonObjectData["members"].is_null()) {
				pDataStructure->members.clear();
				pDataStructure->members.reserve(jsonObjectData["members"].size());
				for (auto& value: jsonObjectData["members"]) {
					ThreadMemberData newData{ value };
					pDataStructure->members.push_back(newData);
				}
				pDataStructure->members.shrink_to_fit();
			}

			if (jsonObjectData.contains("has_more") && !jsonObjectData["has_more"].is_null()) {
				pDataStructure->hasMore = jsonObjectData["has_more"].get<bool>();
			}
		}
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

		RoleData& operator=(const nlohmann ::json& jsonObjectData) {
			this->parseObject(jsonObjectData, this);
			return *this;
		}

		RoleData(const nlohmann ::json& jsonObjectData) {
			*this = jsonObjectData;
		}

		virtual ~RoleData() = default;

	  protected:
		void parseObject(const nlohmann::json& jsonObjectData, RoleData* pDataStructure) {
			if (jsonObjectData.contains("id") && !jsonObjectData["id"].is_null()) {
				if (jsonObjectData["id"].is_string()) {
					pDataStructure->id = stoull(jsonObjectData["id"].get<std::string>());
				} else {
					pDataStructure->id = jsonObjectData["id"].get<int64_t>();
				}
			}

			if (jsonObjectData.contains("name") && !jsonObjectData["name"].is_null()) {
				pDataStructure->name = jsonObjectData["name"].get<std::string>();
			}

			if (jsonObjectData.contains("unicode_emoji") && !jsonObjectData["unicode_emoji"].is_null()) {
				std::stringstream theStream{};
				theStream << jsonObjectData["unicode_emoji"] << std::endl;
				for (auto& value: theStream.str()) {
					pDataStructure->unicodeEmoji.push_back(value);
				}
				pDataStructure->unicodeEmoji = static_cast<std::string>(pDataStructure->unicodeEmoji).substr(1, pDataStructure->unicodeEmoji.size() - 3);
			}

			if (jsonObjectData.contains("color") && !jsonObjectData["color"].is_null()) {
				pDataStructure->color = jsonObjectData["color"].get<int32_t>();
			}

			if (jsonObjectData.contains("hoist") && !jsonObjectData["hoist"].is_null()) {
				pDataStructure->flags = setBool<int8_t, RoleFlags>(pDataStructure->flags, RoleFlags::Hoist, jsonObjectData["hoist"].get<bool>());
			}

			if (jsonObjectData.contains("managed") && !jsonObjectData["managed"].is_null()) {
				pDataStructure->flags = setBool<int8_t, RoleFlags>(pDataStructure->flags, RoleFlags::Managed, jsonObjectData["managed"].get<bool>());
			}

			if (jsonObjectData.contains("mentionable") && !jsonObjectData["mentionable"].is_null()) {
				pDataStructure->flags = setBool<int8_t, RoleFlags>(pDataStructure->flags, RoleFlags::Mentionable, jsonObjectData["mentionable"].get<bool>());
			}

			if (jsonObjectData.contains("position") && !jsonObjectData["position"].is_null()) {
				pDataStructure->position = jsonObjectData["position"].get<int32_t>();
			}

			if (jsonObjectData.contains("permissions") && !jsonObjectData["permissions"].is_null()) {
				pDataStructure->permissions = jsonObjectData["permissions"].get<std::string>();
			}
		}
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

		ActionMetaData& operator=(const nlohmann::json& jsonObjectData) {
			this->parseObject(jsonObjectData, this);
			return *this;
		}

		ActionMetaData(const nlohmann::json& jsonObjectData) {
			*this = jsonObjectData;
		}

		virtual ~ActionMetaData() = default;

	  protected:
		void parseObject(const nlohmann::json& jsonObjectData, ActionMetaData* pDataStructure) {
			if (jsonObjectData.contains("channel_id") && !jsonObjectData["channel_id"].is_null()) {
				pDataStructure->channelId = stoull(jsonObjectData["channel_id"].get<std::string>());
			}

			if (jsonObjectData.contains("duration_seconds") && !jsonObjectData["duration_seconds"].is_null()) {
				pDataStructure->durationSeconds = jsonObjectData["duration_seconds"].get<int64_t>();
			}
		}
	};

	/// Trigger metadata for auto-moderation-rules. \brief Trigger metadata for auto-moderation-rules.
	struct DiscordCoreAPI_Dll TriggerMetaData : public DataParser<TriggerMetaData> {
		std::vector<std::string> keywordFilter{};///< Substrings which will be searched for in content.
		std::vector<KeywordPresetType> presets{};///< The internally pre-defined wordsets which will be searched for in content.

		TriggerMetaData() = default;

		TriggerMetaData& operator=(const nlohmann::json& jsonObjectData) {
			this->parseObject(jsonObjectData, this);
			return *this;
		}

		TriggerMetaData(const nlohmann::json& jsonObjectData) {
			*this = jsonObjectData;
		}

		virtual ~TriggerMetaData() = default;

	  protected:
		void parseObject(const nlohmann::json& jsonObjectData, TriggerMetaData* pDataStructure) {
			if (jsonObjectData.contains("keyword_filter") && !jsonObjectData["keyword_filter"].is_null()) {
				for (auto& value: jsonObjectData["keyword_filter"]) {
					pDataStructure->keywordFilter.push_back(value.get<std::string>());
				}
			}

			if (jsonObjectData.contains("presets") && !jsonObjectData["presets"].is_null()) {
				for (auto& value: jsonObjectData["presets"]) {
					pDataStructure->presets.push_back(static_cast<KeywordPresetType>(value.get<uint64_t>()));
				}
			}
		}
	};

	/// For representing a single auto-moderation-rule-action. \brief For representing a single auto-moderation-rule-action.
	struct DiscordCoreAPI_Dll ActionData : public DataParser<ActionData> {
		ActionType type{};///< The type of action.
		ActionMetaData metadata{};///< Additional metadata needed during execution for this specific action type.

		ActionData() = default;

		ActionData& operator=(const nlohmann::json& jsonObjectData) {
			this->parseObject(jsonObjectData, this);
			return *this;
		}

		ActionData(const nlohmann::json& jsonObjectData) {
			*this = jsonObjectData;
		}

		virtual ~ActionData() = default;

	  protected:
		void parseObject(const nlohmann::json& jsonObjectData, ActionData* pDataStructure) {
			if (jsonObjectData.contains("metadata") && !jsonObjectData["metadata"].is_null()) {
				pDataStructure->metadata = jsonObjectData["metadata"];
			}

			if (jsonObjectData.contains("type") && !jsonObjectData["type"].is_null()) {
				pDataStructure->type = static_cast<ActionType>(jsonObjectData["type"].get<int64_t>());
			}
		}
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

		AutoModerationRuleData& operator=(const nlohmann::json& jsonObjectData) {
			this->parseObject(jsonObjectData, this);
			return *this;
		}

		AutoModerationRuleData(const nlohmann::json& jsonObjectData) {
			*this = jsonObjectData;
		}

		virtual ~AutoModerationRuleData() = default;

	  protected:
		void parseObject(const nlohmann::json& jsonObjectData, AutoModerationRuleData* pDataStructure) {
			if (jsonObjectData.contains("name") && !jsonObjectData["name"].is_null()) {
				pDataStructure->name = jsonObjectData["name"].get<std::string>();
			}

			if (jsonObjectData.contains("id") && !jsonObjectData["id"].is_null()) {
				pDataStructure->id = stoull(jsonObjectData["id"].get<std::string>());
			}

			if (jsonObjectData.contains("enabled") && !jsonObjectData["enabled"].is_null()) {
				pDataStructure->enabled = jsonObjectData["enabled"].get<bool>();
			}

			if (jsonObjectData.contains("trigger_type") && !jsonObjectData["trigger_type"].is_null()) {
				pDataStructure->triggerType = static_cast<TriggerType>(jsonObjectData["trigger_type"].get<uint64_t>());
			}

			if (jsonObjectData.contains("event_type") && !jsonObjectData["event_type"].is_null()) {
				pDataStructure->eventType = static_cast<EventType>(jsonObjectData["event_type"].get<uint64_t>());
			}

			if (jsonObjectData.contains("creator_id") && !jsonObjectData["creator_id"].is_null()) {
				pDataStructure->creatorId = jsonObjectData["creator_id"].get<Snowflake>();
			}

			if (jsonObjectData.contains("actions") && !jsonObjectData["actions"].is_null()) {
				for (auto& value: jsonObjectData["actions"]) {
					ActionData newData{ value };
					pDataStructure->actions.push_back(newData);
				}
			}

			if (jsonObjectData.contains("exempt_roles") && !jsonObjectData["exempt_roles"].is_null()) {
				for (auto& value: jsonObjectData["exempt_roles"]) {
					pDataStructure->exemptRoles.push_back(value.get<Snowflake>());
				}
			}

			if (jsonObjectData.contains("trigger_metadata") && !jsonObjectData["trigger_metadata"].is_null()) {
				pDataStructure->triggerMetaData = jsonObjectData["trigger_metadata"];
			}

			if (jsonObjectData.contains("exempt_channels") && !jsonObjectData["exempt_channels"].is_null()) {
				for (auto& value: jsonObjectData["exempt_channels"]) {
					pDataStructure->exemptChannels.push_back(value.get<Snowflake>());
				}
			}

			if (jsonObjectData.contains("guild_id") && !jsonObjectData["guild_id"].is_null()) {
				pDataStructure->guildId = jsonObjectData["guild_id"].get<Snowflake>();
			}
		}
	};

	/// Permissions data for an ApplicationCommand. \brief Permissions data for an ApplicationCommand.
	class DiscordCoreAPI_Dll ApplicationCommandPermissionData : public DiscordEntity, public DataParser<ApplicationCommandPermissionData> {
	  public:
		ApplicationCommandPermissionType type{ ApplicationCommandPermissionType::Role };///< The type of Permission.
		bool permission{ false };///< Whether the Permission is active or not.

		ApplicationCommandPermissionData() = default;

		ApplicationCommandPermissionData& operator=(const nlohmann::json& jsonObjectData) {
			this->parseObject(jsonObjectData, this);
			return *this;
		}

		ApplicationCommandPermissionData(const nlohmann::json& jsonObjectData) {
			*this = jsonObjectData;
		}
		virtual ~ApplicationCommandPermissionData() = default;

	  protected:
		void parseObject(const nlohmann::json& jsonObjectData, ApplicationCommandPermissionData* pDataStructure) {
			if (jsonObjectData.contains("id") && !jsonObjectData["id"].is_null()) {
				pDataStructure->id = stoull(jsonObjectData["id"].get<std::string>());
			}

			if (jsonObjectData.contains("permission") && !jsonObjectData["permission"].is_null()) {
				pDataStructure->permission = jsonObjectData["permission"].get<bool>();
			}

			if (jsonObjectData.contains("type") && !jsonObjectData["type"].is_null()) {
				pDataStructure->type = jsonObjectData["type"].get<ApplicationCommandPermissionType>();
			}
		}
	};

	/// Represents the Permissions for accessing an ApplicationCommand from within a Guild. \brief Represents the Permissions for accessing an ApplicationCommand from within a Guild.
	class DiscordCoreAPI_Dll GuildApplicationCommandPermissionsData : public DiscordEntity, public DataParser<GuildApplicationCommandPermissionsData> {
	  public:
		std::vector<ApplicationCommandPermissionData> permissions{};///< The Permissions.
		Snowflake applicationId{};///< The application's id.
		Snowflake guildId{};///< The Guild's id.

		GuildApplicationCommandPermissionsData() = default;

		GuildApplicationCommandPermissionsData& operator=(const nlohmann::json& jsonObjectData) {
			this->parseObject(jsonObjectData, this);
			return *this;
		}

		GuildApplicationCommandPermissionsData(const nlohmann::json& jsonObjectData) {
			*this = jsonObjectData;
		}

		virtual ~GuildApplicationCommandPermissionsData() = default;

	  protected:
		void parseObject(const nlohmann::json& jsonObjectData, GuildApplicationCommandPermissionsData* pDataStructure) {
			if (jsonObjectData.contains("guild_id") && !jsonObjectData["guild_id"].is_null()) {
				pDataStructure->guildId = stoull(jsonObjectData["guild_id"].get<std::string>());
			}

			if (jsonObjectData.contains("application_id") && !jsonObjectData["application_id"].is_null()) {
				pDataStructure->applicationId = stoull(jsonObjectData["application_id"].get<std::string>());
			}

			if (jsonObjectData.contains("id") && !jsonObjectData["id"].is_null()) {
				pDataStructure->id = stoull(jsonObjectData["id"].get<std::string>());
			}

			if (jsonObjectData.contains("permissions") && !jsonObjectData["permissions"].is_null()) {
				pDataStructure->permissions.clear();
				pDataStructure->permissions.reserve(jsonObjectData["permissions"].size());
				for (auto& value: jsonObjectData["permissions"]) {
					ApplicationCommandPermissionData newData{ value };
					pDataStructure->permissions.push_back(newData);
				}
				pDataStructure->permissions.shrink_to_fit();
			}
		}
	};

	class DiscordCoreAPI_Dll GuildApplicationCommandPermissionsDataVector : public DataParser<GuildApplicationCommandPermissionsDataVector> {
	  public:
		std::vector<GuildApplicationCommandPermissionsData> theGuildApplicationCommandPermissionsDatas{};

		GuildApplicationCommandPermissionsDataVector() = default;

		GuildApplicationCommandPermissionsDataVector& operator=(const nlohmann::json& jsonObjectData) {
			this->parseObject(jsonObjectData, this);
			return *this;
		}

		GuildApplicationCommandPermissionsDataVector(const nlohmann::json& jsonObjectData) {
			*this = jsonObjectData;
		}

		virtual ~GuildApplicationCommandPermissionsDataVector() = default;

	  protected:
		void parseObject(const nlohmann::json& jsonObjectData, GuildApplicationCommandPermissionsDataVector* pDataStructure) {
			pDataStructure->theGuildApplicationCommandPermissionsDatas.reserve(jsonObjectData.size());
			for (auto& value: jsonObjectData) {
				DiscordCoreAPI::GuildApplicationCommandPermissionsData newData{ value };
				pDataStructure->theGuildApplicationCommandPermissionsDatas.push_back(newData);
			}
			pDataStructure->theGuildApplicationCommandPermissionsDatas.shrink_to_fit();
		}
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

		EmojiData& operator=(const nlohmann::json& jsonObjectData) {
			this->parseObject(jsonObjectData, this);
			return *this;
		}

		EmojiData(const nlohmann::json& jsonObjectData) {
			*this = jsonObjectData;
		}

		virtual ~EmojiData() = default;

	  protected:
		void parseObject(const nlohmann::json& jsonObjectData, EmojiData* pDataStructure) {
			if (jsonObjectData.contains("id") && !jsonObjectData["id"].is_null()) {
				pDataStructure->id = stoull(jsonObjectData["id"].get<std::string>());
			}

			if (jsonObjectData.contains("name") && !jsonObjectData["name"].is_null()) {
				pDataStructure->name = jsonObjectData["name"].get<std::string>();
			}

			if (jsonObjectData.contains("roles") && !jsonObjectData["roles"].is_null()) {
				pDataStructure->roles.clear();
				pDataStructure->roles.reserve(jsonObjectData["roles"].size());
				for (auto& value: jsonObjectData["roles"]) {
					RoleData newData{ value };
					pDataStructure->roles.push_back(newData);
				}
				pDataStructure->roles.shrink_to_fit();
			}

			if (jsonObjectData.contains("user") && !jsonObjectData["user"].is_null()) {
				pDataStructure->user = jsonObjectData["user"];
			}

			if (jsonObjectData.contains("require_colons") && !jsonObjectData["require_colons"].is_null()) {
				pDataStructure->requireColons = jsonObjectData["require_colons"].get<bool>();
			}

			if (jsonObjectData.contains("managed") && !jsonObjectData["managed"].is_null()) {
				pDataStructure->managed = jsonObjectData["managed"].get<bool>();
			}

			if (jsonObjectData.contains("animated") && !jsonObjectData["animated"].is_null()) {
				pDataStructure->animated = jsonObjectData["animated"].get<bool>();
			}

			if (jsonObjectData.contains("available") && !jsonObjectData["available"].is_null()) {
				pDataStructure->available = jsonObjectData["available"].get<bool>();
			}
		}
	};

	class DiscordCoreAPI_Dll EmojiDataVector : public DataParser<EmojiDataVector> {
	  public:
		std::vector<EmojiData> theEmojiDatas{};

		EmojiDataVector() = default;

		EmojiDataVector& operator=(const nlohmann::json& jsonObjectData) {
			this->parseObject(jsonObjectData, this);
			return *this;
		}

		EmojiDataVector(const nlohmann::json& jsonObjectData) {
			*this = jsonObjectData;
		}

		virtual ~EmojiDataVector() = default;

	  protected:
		void parseObject(const nlohmann::json& jsonObjectData, EmojiDataVector* pDataStructure) {
			pDataStructure->theEmojiDatas.reserve(jsonObjectData.size());
			for (auto& value: jsonObjectData) {
				DiscordCoreAPI::EmojiData newData{ value };
				pDataStructure->theEmojiDatas.push_back(newData);
			}
			pDataStructure->theEmojiDatas.shrink_to_fit();
		}
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

		ReactionData& operator=(const nlohmann::json& jsonObjectData) {
			this->parseObject(jsonObjectData, this);
			return *this;
		}

		ReactionData(const nlohmann::json& jsonObjectData) {
			*this = jsonObjectData;
		}

		virtual ~ReactionData() = default;

	  protected:
		void parseObject(const nlohmann::json& jsonObjectData, ReactionData* pDataStructure) {
			if (jsonObjectData.contains("count") && !jsonObjectData["count"].is_null()) {
				pDataStructure->count = jsonObjectData["count"].get<int32_t>();
			}

			if (jsonObjectData.contains("me") && !jsonObjectData["me"].is_null()) {
				pDataStructure->count = jsonObjectData["me"].get<bool>();
			}

			if (jsonObjectData.contains("emoji") && !jsonObjectData["emoji"].is_null()) {
				pDataStructure->emoji = jsonObjectData["emoji"];
			}

			if (jsonObjectData.contains("guild_id") && !jsonObjectData["guild_id"].is_null()) {
				pDataStructure->guildId = stoull(jsonObjectData["guild_id"].get<std::string>());
			}

			if (jsonObjectData.contains("channel_id") && !jsonObjectData["channel_id"].is_null()) {
				pDataStructure->channelId = stoull(jsonObjectData["channel_id"].get<std::string>());
			}

			if (jsonObjectData.contains("user_id") && !jsonObjectData["user_id"].is_null()) {
				pDataStructure->userId = stoull(jsonObjectData["user_id"].get<std::string>());
			}

			if (jsonObjectData.contains("message_id") && !jsonObjectData["message_id"].is_null()) {
				pDataStructure->messageId = stoull(jsonObjectData["message_id"].get<std::string>());
			}

			if (jsonObjectData.contains("member") && !jsonObjectData["member"].is_null()) {
				pDataStructure->member = jsonObjectData["member"];
			}
		}
	};

	/// Structure representing Voice Region Data. \brief Structure representing Voice Region Data.
	struct DiscordCoreAPI_Dll VoiceRegionData : public DataParser<VoiceRegionData> {
		bool deprecated{ false };///< Whether this is a deprecated voice region(avoid switching to these).
		bool optimal{ false };///< True for a single server that is closest to the current User's client.
		bool custom{ false };///< Whether this is a custom voice region(used for events / etc).
		std::string name{};///< Name of the region.
		Snowflake id{};///< Unique ID for the region.

		VoiceRegionData() = default;

		VoiceRegionData& operator=(const nlohmann::json& jsonObjectData) {
			this->parseObject(jsonObjectData, this);
			return *this;
		}

		VoiceRegionData(const nlohmann::json& jsonObjectData) {
			*this = jsonObjectData;
		}

		virtual ~VoiceRegionData() = default;

	  protected:
		void parseObject(const nlohmann::json& jsonObjectData, VoiceRegionData* pDataStructure) {
			if (jsonObjectData.contains("id") && !jsonObjectData["id"].is_null()) {
				pDataStructure->id = stoull(jsonObjectData["id"].get<std::string>());
			}

			if (jsonObjectData.contains("custom") && !jsonObjectData["custom"].is_null()) {
				pDataStructure->custom = jsonObjectData["custom"].get<bool>();
			}

			if (jsonObjectData.contains("deprecated") && !jsonObjectData["deprecated"].is_null()) {
				pDataStructure->deprecated = jsonObjectData["deprecated"].get<bool>();
			}

			if (jsonObjectData.contains("optimal") && !jsonObjectData["optimal"].is_null()) {
				pDataStructure->optimal = jsonObjectData["optimal"].get<bool>();
			}

			if (jsonObjectData.contains("name") && !jsonObjectData["name"].is_null()) {
				pDataStructure->name = jsonObjectData["name"].get<std::string>();
			}
		}
	};

	class DiscordCoreAPI_Dll VoiceRegionDataVector : public DataParser<VoiceRegionDataVector> {
	  public:
		VoiceRegionDataVector() = default;

		operator std::vector<VoiceRegionData>() {
			return this->theVoiceRegionDatas;
		}

		VoiceRegionDataVector& operator=(const nlohmann::json& jsonObjectData) {
			this->parseObject(jsonObjectData, this);
			return *this;
		}

		VoiceRegionDataVector(const nlohmann::json& jsonObjectData) {
			*this = jsonObjectData;
		}

		virtual ~VoiceRegionDataVector() = default;

	  protected:
		std::vector<VoiceRegionData> theVoiceRegionDatas{};

		void parseObject(const nlohmann::json& jsonObjectData, VoiceRegionDataVector* pDataStructure) {
			pDataStructure->theVoiceRegionDatas.reserve(jsonObjectData.size());
			for (auto& value: jsonObjectData) {
				DiscordCoreAPI::VoiceRegionData newData{ value };
				pDataStructure->theVoiceRegionDatas.push_back(newData);
			}
			pDataStructure->theVoiceRegionDatas.shrink_to_fit();
		}
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

		MessageActivityData& operator=(const nlohmann::json& jsonObjectData) {
			this->parseObject(jsonObjectData, this);
			return *this;
		}

		MessageActivityData(const nlohmann::json& jsonObjectData) {
			*this = jsonObjectData;
		}

		virtual ~MessageActivityData() = default;

		void parseObject(const nlohmann::json& jsonObjectData, MessageActivityData* pDataStructure) {
			if (jsonObjectData.contains("type") && !jsonObjectData["type"].is_null()) {
				pDataStructure->type = jsonObjectData["type"].get<MessageActivityType>();
			}

			if (jsonObjectData.contains("party_id") && !jsonObjectData["party_id"].is_null()) {
				pDataStructure->partyId = jsonObjectData["party_id"].get<std::string>();
			}
		}
	};

	/// Ban data. \brief Ban data.
	struct DiscordCoreAPI_Dll BanData : public DataParser<BanData> {
		bool failedDueToPerms{ false };///< Failed due to perms?
		std::string reason{};///< Reason for the ban.
		UserData user{};///< User that was banned.

		BanData() = default;

		BanData& operator=(const nlohmann::json& jsonObjectData) {
			this->parseObject(jsonObjectData, this);
			return *this;
		}

		BanData(const nlohmann::json& jsonObjectData) {
			*this = jsonObjectData;
		}

		virtual ~BanData() = default;

	  protected:
		void parseObject(const nlohmann::json& jsonObjectData, BanData* pDataStructure) {
			if (jsonObjectData.contains("user") && !jsonObjectData["user"].is_null()) {
				pDataStructure->user = jsonObjectData["user"];
			}

			if (jsonObjectData.contains("reason") && !jsonObjectData["reason"].is_null()) {
				pDataStructure->reason = jsonObjectData["reason"].get<std::string>();
			}
		}
	};

	class DiscordCoreAPI_Dll BanDataVector : public DataParser<BanDataVector> {
	  public:
		BanDataVector() = default;

		operator std::vector<BanData>() {
			return this->theBanDatas;
		}

		BanDataVector& operator=(const nlohmann::json& jsonObjectData) {
			this->parseObject(jsonObjectData, this);
			return *this;
		}

		BanDataVector(const nlohmann::json& jsonObjectData) {
			*this = jsonObjectData;
		}

		virtual ~BanDataVector() = default;

	  protected:
		std::vector<BanData> theBanDatas{};

		void parseObject(const nlohmann::json& jsonObjectData, BanDataVector* pDataStructure) {
			pDataStructure->theBanDatas.reserve(jsonObjectData.size());
			for (auto& value: jsonObjectData) {
				DiscordCoreAPI::BanData newData{ value };
				pDataStructure->theBanDatas.push_back(newData);
			}
			pDataStructure->theBanDatas.shrink_to_fit();
		}
	};

	/// Team members object data. \brief Team members object data.
	struct DiscordCoreAPI_Dll TeamMembersObjectData : public DataParser<TeamMembersObjectData> {
		std::vector<Permissions> permissions{};///< Permissions for the team.
		int32_t membershipState{ 0 };///< Current state.
		std::string teamId{};///< Id of the current team.
		UserData user{};///< User data of the current User.

		TeamMembersObjectData() = default;

		TeamMembersObjectData& operator=(const nlohmann::json& jsonObjectData) {
			this->parseObject(jsonObjectData, this);
			return *this;
		}

		TeamMembersObjectData(const nlohmann::json& jsonObjectData) {
			*this = jsonObjectData;
		}

		virtual ~TeamMembersObjectData() = default;

	  protected:
		void parseObject(const nlohmann::json& jsonObjectData, TeamMembersObjectData* pDataStructure) {
			if (jsonObjectData.contains("membership_state") && !jsonObjectData["membership_state"].is_null()) {
				pDataStructure->membershipState = jsonObjectData["membership_state"].get<int32_t>();
			}

			if (jsonObjectData.contains("team_id") && !jsonObjectData["team_id"].is_null()) {
				pDataStructure->teamId = jsonObjectData["team_id"].get<std::string>();
			}

			if (jsonObjectData.contains("permissions") && !jsonObjectData["permissions"].is_null()) {
				pDataStructure->permissions.clear();
				pDataStructure->permissions.reserve(jsonObjectData["permissions"].size());
				for (auto& value: jsonObjectData["permissions"]) {
					pDataStructure->permissions.push_back(value.get<std::string>());
				}
				pDataStructure->permissions.shrink_to_fit();
			}

			if (jsonObjectData.contains("user") && !jsonObjectData["user"].is_null()) {
				pDataStructure->user = jsonObjectData["user"];
			}
		}
	};

	/// For updating the current voice state. \brief For updating the current voice state.
	struct DiscordCoreAPI_Dll UpdateVoiceStateData {
		bool selfMute{ false };///< Whether or not we self-mute ourselves.
		bool selfDeaf{ false };///< Whether or not we self-deafen ourselves.
		Snowflake channelId{};///< Id of the desired voice Channel. Leave blank to disconnect.
		Snowflake guildId{};///< The id of the Guild fo which we would like to establish a voice connection.

		operator nlohmann::json() {
			nlohmann::json data{};
			if (this->channelId == 0) {
				data["d"]["channel_id"] = nullptr;
			} else {
				data["d"]["channel_id"] = std::to_string(this->channelId);
			}
			data["d"]["self_deaf"] = this->selfDeaf;
			data["d"]["self_mute"] = this->selfMute;
			data["d"]["guild_id"] = std::to_string(this->guildId);
			data["op"] = 4;
			return data;
		}
	};

	/// Team object data. \brief Team object data.
	class DiscordCoreAPI_Dll TeamObjectData : public DiscordEntity, public DataParser<TeamObjectData> {
	  public:
		std::vector<TeamMembersObjectData> members{};///< Array of team members object data.
		std::string ownerUserId{};///< User id of the team owner.
		std::string icon{};///< Icon for the team.

		TeamObjectData() = default;

		TeamObjectData& operator=(const nlohmann::json& jsonObjectData) {
			this->parseObject(jsonObjectData, this);
			return *this;
		}

		TeamObjectData(const nlohmann::json& jsonObjectData) {
			*this = jsonObjectData;
		}

		virtual ~TeamObjectData() = default;

	  protected:
		void parseObject(const nlohmann::json& jsonObjectData, TeamObjectData* pDataStructure) {
			if (jsonObjectData.contains("icon") && !jsonObjectData["icon"].is_null()) {
				pDataStructure->icon = jsonObjectData["icon"].get<std::string>();
			}

			if (jsonObjectData.contains("id") && !jsonObjectData["id"].is_null()) {
				pDataStructure->id = stoull(jsonObjectData["id"].get<std::string>());
			}

			if (jsonObjectData.contains("members") && !jsonObjectData["members"].is_null()) {
				pDataStructure->members.clear();
				pDataStructure->members.reserve(jsonObjectData["members"].size());
				for (auto& value: jsonObjectData["members"]) {
					TeamMembersObjectData newData{ value };
					pDataStructure->members.push_back(newData);
				}
				pDataStructure->members.shrink_to_fit();
			}

			if (jsonObjectData.contains("owner_user_id") && !jsonObjectData["owner_user_id"].is_null()) {
				pDataStructure->ownerUserId = jsonObjectData["owner_user_id"].get<std::string>();
			}
		}
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

		InstallParamsData& operator=(const nlohmann::json& jsonObjectData) {
			this->parseObject(jsonObjectData, this);
			return *this;
		}

		InstallParamsData(const nlohmann::json& jsonObjectData) {
			*this = jsonObjectData;
		}

		virtual ~InstallParamsData() = default;

	  protected:
		void parseObject(const nlohmann::json& jsonObjectData, InstallParamsData* pDataStructure) {
			if (jsonObjectData.contains("scopes") && !jsonObjectData["scopes"].is_null()) {
				pDataStructure->scopes = jsonObjectData["scopes"].get<std::vector<std::string>>();
			}

			if (jsonObjectData.contains("permissions") && !jsonObjectData["permissions"].is_null()) {
				pDataStructure->permissions = jsonObjectData["name"].get<std::string>();
			}
		}
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

		ApplicationData& operator=(const nlohmann::json& jsonObjectData) {
			this->parseObject(jsonObjectData, this);
			return *this;
		}

		ApplicationData(const nlohmann::json& jsonObjectData) {
			*this = jsonObjectData;
		}

		virtual ~ApplicationData() = default;

	  protected:
		void parseObject(const nlohmann::json& jsonObjectData, ApplicationData* pDataStructure) {
			if (jsonObjectData.contains("params") && !jsonObjectData["params"].is_null()) {
				pDataStructure->params = jsonObjectData["params"];
			}

			if (jsonObjectData.contains("tags") && !jsonObjectData["tags"].is_null()) {
				pDataStructure->tags = jsonObjectData["tags"].get<std::vector<std::string>>();
			}

			if (jsonObjectData.contains("id") && !jsonObjectData["id"].is_null()) {
				pDataStructure->id = stoull(jsonObjectData["id"].get<std::string>());
			}

			if (jsonObjectData.contains("name") && !jsonObjectData["name"].is_null()) {
				pDataStructure->name = jsonObjectData["name"].get<std::string>();
			}

			if (jsonObjectData.contains("icon") && !jsonObjectData["icon"].is_null()) {
				pDataStructure->icon = jsonObjectData["icon"].get<std::string>();
			}

			if (jsonObjectData.contains("description") && !jsonObjectData["description"].is_null()) {
				pDataStructure->description = jsonObjectData["description"].get<std::string>();
			}

			if (jsonObjectData.contains("rpc_origins") && !jsonObjectData["rpc_origins"].is_null()) {
				pDataStructure->rpcOrigins.clear();
				pDataStructure->rpcOrigins.reserve(jsonObjectData["rpc_origins"].size());
				for (auto& value: jsonObjectData["rpc_origins"]) {
					pDataStructure->rpcOrigins.push_back(value.get<std::string>());
				}
				pDataStructure->rpcOrigins.shrink_to_fit();
			}

			if (jsonObjectData.contains("bot_public") && !jsonObjectData["bot_public"].is_null()) {
				pDataStructure->botPublic = jsonObjectData["bot_public"].get<bool>();
			}

			if (jsonObjectData.contains("bot_require_code_grant") && !jsonObjectData["bot_require_code_grant"].is_null()) {
				pDataStructure->botRequireCodeGrant = jsonObjectData["bot_require_code_grant"].get<bool>();
			}

			if (jsonObjectData.contains("terms_of_service_url") && !jsonObjectData["terms_of_service_url"].is_null()) {
				pDataStructure->termsOfServiceUrl = jsonObjectData["terms_of_service_url"].get<std::string>();
			}

			if (jsonObjectData.contains("privacy_policy_url") && !jsonObjectData["privacy_policy_url"].is_null()) {
				pDataStructure->privacyPolicyUrl = jsonObjectData["privacy_policy_url"].get<std::string>();
			}

			if (jsonObjectData.contains("owner") && !jsonObjectData["owner"].is_null()) {
				pDataStructure->owner = jsonObjectData["owner"];
			}

			if (jsonObjectData.contains("summary") && !jsonObjectData["summary"].is_null()) {
				pDataStructure->summary = jsonObjectData["summary"].get<std::string>();
			}

			if (jsonObjectData.contains("verify_key") && !jsonObjectData["verify_key"].is_null()) {
				pDataStructure->verifyKey = jsonObjectData["verify_key"].get<std::string>();
			}

			if (jsonObjectData.contains("team") && !jsonObjectData["team"].is_null()) {
				pDataStructure->team = jsonObjectData["team"];
			}

			if (jsonObjectData.contains("guild_id") && !jsonObjectData["guild_id"].is_null()) {
				pDataStructure->guildId = stoull(jsonObjectData["guild_id"].get<std::string>());
			}

			if (jsonObjectData.contains("primary_sku_id") && !jsonObjectData["primary_sku_id"].is_null()) {
				pDataStructure->primarySkuId = jsonObjectData["primary_sku_id"].get<std::string>();
			}

			if (jsonObjectData.contains("slug") && !jsonObjectData["slug"].is_null()) {
				pDataStructure->slug = jsonObjectData["slug"].get<std::string>();
			}

			if (jsonObjectData.contains("cover_image") && !jsonObjectData["cover_image"].is_null()) {
				pDataStructure->coverImage = jsonObjectData["cover_image"].get<std::string>();
			}

			if (jsonObjectData.contains("flags") && !jsonObjectData["flags"].is_null()) {
				pDataStructure->flags = jsonObjectData["flags"].get<ApplicationFlags>();
			}
		}
	};

	/// Authorization info structure. \brief Authorization info structure.
	struct DiscordCoreAPI_Dll AuthorizationInfoData : public DataParser<AuthorizationInfoData> {
		std::vector<std::string> scopes{};///< Array of strings - the scopes the User has authorized the application for.
		ApplicationData application{};///< Partial application object the current application.
		std::string expires{};///< When the access token expires.
		UserData user{};/// The User who has authorized, if the User has authorized with the identify scope.

		AuthorizationInfoData() = default;

		AuthorizationInfoData& operator=(const nlohmann::json& jsonObjectData) {
			this->parseObject(jsonObjectData, this);
			return *this;
		}

		AuthorizationInfoData(const nlohmann::json& jsonObjectData) {
			*this = jsonObjectData;
		}

		virtual ~AuthorizationInfoData() = default;

	  protected:
		void parseObject(const nlohmann::json& jsonObjectData, AuthorizationInfoData* pDataStructure) {
			if (jsonObjectData.contains("application") && !jsonObjectData["application"].is_null()) {
				pDataStructure->application = jsonObjectData["application"];
			}

			if (jsonObjectData.contains("scopes") && !jsonObjectData["scopes"].is_null()) {
				pDataStructure->scopes = jsonObjectData["scopes"].get<std::vector<std::string>>();
			}

			if (jsonObjectData.contains("expires") && !jsonObjectData["expires"].is_null()) {
				pDataStructure->expires = jsonObjectData["expires"];
			}

			if (jsonObjectData.contains("user") && !jsonObjectData["user"].is_null()) {
				pDataStructure->user = jsonObjectData["user"];
			}
		};
	};

	/// Account data. \brief Account data.
	class DiscordCoreAPI_Dll AccountData : public DiscordEntity, public DataParser<AccountData> {
	  public:
		std::string name{};///< Name of the account.

		AccountData() = default;

		AccountData& operator=(const nlohmann::json& jsonObjectData) {
			this->parseObject(jsonObjectData, this);
			return *this;
		}

		AccountData(const nlohmann::json& jsonObjectData) {
			*this = jsonObjectData;
		}

		virtual ~AccountData() = default;

	  protected:
		void parseObject(const nlohmann::json& jsonObjectData, AccountData* pDataStructure) {
			if (jsonObjectData.contains("id") && !jsonObjectData["id"].is_null()) {
				pDataStructure->id = stoull(jsonObjectData["id"].get<std::string>());
			}

			if (jsonObjectData.contains("name") && !jsonObjectData["name"].is_null()) {
				pDataStructure->name = jsonObjectData["name"].get<std::string>();
			}
		}
	};

	/// Guild Widget Data. \brief Guild Widget Data.
	struct DiscordCoreAPI_Dll GuildWidgetData : public DataParser<GuildWidgetData> {
		bool enabled{ false };///< Whether the widget is enabled.
		Snowflake channelId{};///< The widget Channel id.

		GuildWidgetData() = default;

		GuildWidgetData& operator=(const nlohmann::json& jsonObjectData) {
			this->parseObject(jsonObjectData, this);
			return *this;
		}

		GuildWidgetData(const nlohmann::json& jsonObjectData) {
			*this = jsonObjectData;
		}

		virtual ~GuildWidgetData() = default;

	  protected:
		void parseObject(const nlohmann::json& jsonObjectData, GuildWidgetData* pDataStructure) {
			if (jsonObjectData.contains("enabled") && !jsonObjectData["enabled"].is_null()) {
				pDataStructure->enabled = jsonObjectData["enabled"].get<bool>();
			}

			if (jsonObjectData.contains("channel_id") && !jsonObjectData["channel_id"].is_null()) {
				pDataStructure->channelId = stoull(jsonObjectData["channel_id"].get<std::string>());
			}
		};
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

		GuildWidgetImageData& operator=(const nlohmann::json& jsonObjectData) {
			this->parseObject(jsonObjectData, this);
			return *this;
		}

		GuildWidgetImageData(const nlohmann::json& jsonObjectData) {
			*this = jsonObjectData;
		}

		virtual ~GuildWidgetImageData() = default;

	  protected:
		void parseObject(const nlohmann::json& jsonObjectData, GuildWidgetImageData* pDataStructure) {
			if (jsonObjectData.contains("widget_image") && !jsonObjectData["widget_image"].is_null()) {
				pDataStructure->url = jsonObjectData["widget_image"].get<bool>();
			}
		}
	};

	/// Integration data. \brief Integration data.
	class DiscordCoreAPI_Dll IntegrationData : public DiscordEntity, public DataParser<IntegrationData> {
	  public:
		int32_t expireGracePeriod{ 0 };///< How int64_t before the integration expires.
		ApplicationData application{};///< Application data.
		int32_t subscriberCount{ 0 };///< Number of current subscribers.
		bool enableEmoticons{ true };///< Emoticons enabled?
		int32_t expireBehavior{ 0 };///< What to do upon expiry.
		TimeStamp syncedAt{ "" };///< Time it was last synced at.
		bool enabled{ false };///< Enabled?
		bool syncing{ false };///< Is it syncing?
		AccountData account{};///< Account data.
		bool revoked{ false };///< Has it been revoked?
		std::string name{};///< Name of the integration.
		std::string type{};///< Type of integration.
		Snowflake roleId{};///< Role Id.
		UserData user{};///< User data for the integration.

		IntegrationData() = default;

		IntegrationData& operator=(const nlohmann::json& jsonObjectData) {
			this->parseObject(jsonObjectData, this);
			return *this;
		}

		IntegrationData(const nlohmann::json& jsonObjectData) {
			*this = jsonObjectData;
		}

		virtual ~IntegrationData() = default;

	  protected:
		void parseObject(const nlohmann::json& jsonObjectData, IntegrationData* pDataStructure) {
			if (jsonObjectData.contains("id") && !jsonObjectData["id"].is_null()) {
				pDataStructure->id = stoull(jsonObjectData["id"].get<std::string>());
			}

			if (jsonObjectData.contains("name") && !jsonObjectData["name"].is_null()) {
				pDataStructure->name = jsonObjectData["name"].get<std::string>();
			}

			if (jsonObjectData.contains("type") && !jsonObjectData["type"].is_null()) {
				pDataStructure->type = jsonObjectData["type"].get<std::string>();
			}

			if (jsonObjectData.contains("enabled") && !jsonObjectData["enabled"].is_null()) {
				pDataStructure->enabled = jsonObjectData["enabled"].get<bool>();
			}

			if (jsonObjectData.contains("syncing") && !jsonObjectData["syncing"].is_null()) {
				pDataStructure->syncing = jsonObjectData["syncing"].get<bool>();
			}

			if (jsonObjectData.contains("role_id") && !jsonObjectData["role_id"].is_null()) {
				pDataStructure->roleId = stoull(jsonObjectData["role_id"].get<std::string>());
			}

			if (jsonObjectData.contains("enable_emoticons") && !jsonObjectData["enable_emoticons"].is_null()) {
				pDataStructure->enableEmoticons = jsonObjectData["enable_emoticons"].get<bool>();
			}

			if (jsonObjectData.contains("expire_behavior") && !jsonObjectData["expire_behavior"].is_null()) {
				pDataStructure->expireBehavior = jsonObjectData["expire_behavior"].get<int32_t>();
			}

			if (jsonObjectData.contains("expire_grace_period") && !jsonObjectData["expire_grace_period"].is_null()) {
				pDataStructure->expireGracePeriod = jsonObjectData["expire_grace_period"].get<int32_t>();
			}

			if (jsonObjectData.contains("user") && !jsonObjectData["user"].is_null()) {
				pDataStructure->user = jsonObjectData["user"];
			}

			if (jsonObjectData.contains("account") && !jsonObjectData["account"].is_null()) {
				pDataStructure->account = jsonObjectData["account"];
			}

			if (jsonObjectData.contains("synced_at") && !jsonObjectData["synced_at"].is_null()) {
				pDataStructure->syncedAt = jsonObjectData["synced_at"].get<std::string>();
			}

			if (jsonObjectData.contains("subscriber_count") && !jsonObjectData["subscriber_count"].is_null()) {
				pDataStructure->subscriberCount = jsonObjectData["subscriber_count"].get<int32_t>();
			}

			if (jsonObjectData.contains("revoked") && !jsonObjectData["revoked"].is_null()) {
				pDataStructure->revoked = jsonObjectData["revoked"].get<bool>();
			}

			if (jsonObjectData.contains("application") && !jsonObjectData["application"].is_null()) {
				pDataStructure->application = jsonObjectData["application"];
			}
		}
	};

	class DiscordCoreAPI_Dll IntegrationDataVector : public DataParser<IntegrationDataVector> {
	  public:
		IntegrationDataVector() = default;

		operator std::vector<IntegrationData>() {
			return this->theIntegrationDatas;
		}

		IntegrationDataVector& operator=(const nlohmann::json& jsonObjectData) {
			this->parseObject(jsonObjectData, this);
			return *this;
		}

		IntegrationDataVector(const nlohmann::json& jsonObjectData) {
			*this = jsonObjectData;
		}

		virtual ~IntegrationDataVector() = default;

	  protected:
		std::vector<IntegrationData> theIntegrationDatas{};

		void parseObject(const nlohmann::json& jsonObjectData, IntegrationDataVector* pDataStructure) {
			pDataStructure->theIntegrationDatas.reserve(jsonObjectData.size());
			for (auto& value: jsonObjectData) {
				DiscordCoreAPI::IntegrationData newData{ value };
				pDataStructure->theIntegrationDatas.push_back(newData);
			}
			pDataStructure->theIntegrationDatas.shrink_to_fit();
		}
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

		OptionalAuditEntryInfoData& operator=(const nlohmann::json& jsonObjectData) {
			this->parseObject(jsonObjectData, this);
			return *this;
		}

		OptionalAuditEntryInfoData(const nlohmann::json& jsonObjectData) {
			*this = jsonObjectData;
		}

		virtual ~OptionalAuditEntryInfoData() = default;

	  protected:
		void parseObject(const nlohmann::json& jsonObjectData, OptionalAuditEntryInfoData* pDataStructure) {
			if (jsonObjectData.contains("delete_member_days") && !jsonObjectData["delete_member_days"].is_null()) {
				pDataStructure->deleteMemberDays = jsonObjectData["delete_member_days"].get<std::string>();
			}

			if (jsonObjectData.contains("members_removed") && !jsonObjectData["members_removed"].is_null()) {
				pDataStructure->membersRemoved = jsonObjectData["members_removed"].get<std::string>();
			}

			if (jsonObjectData.contains("channel_id") && !jsonObjectData["channel_id"].is_null()) {
				pDataStructure->channelId = stoull(jsonObjectData["channel_id"].get<std::string>());
			}

			if (jsonObjectData.contains("message_id") && !jsonObjectData["message_id"].is_null()) {
				pDataStructure->messageId = stoull(jsonObjectData["message_id"].get<std::string>());
			}

			if (jsonObjectData.contains("count") && !jsonObjectData["count"].is_null()) {
				pDataStructure->count = jsonObjectData["count"].get<std::string>();
			}

			if (jsonObjectData.contains("id") && !jsonObjectData["id"].is_null()) {
				pDataStructure->id = stoull(jsonObjectData["id"].get<std::string>());
			}

			if (jsonObjectData.contains("type") && !jsonObjectData["type"].is_null()) {
				pDataStructure->type = jsonObjectData["type"].get<std::string>();
			}

			if (jsonObjectData.contains("role_name") && !jsonObjectData["role_name"].is_null()) {
				pDataStructure->roleName = jsonObjectData["role_name"].get<std::string>();
			}
		}
	};

	/// Audit log change data. \brief Audit log change data.
	struct DiscordCoreAPI_Dll AuditLogChangeData : public DataParser<AuditLogChangeData> {
		nlohmann::json newValue{};///< New value.
		nlohmann::json oldValue{};///< Old value.
		std::string key{};///< The key of the audit log change.

		AuditLogChangeData() = default;

		AuditLogChangeData& operator=(const nlohmann::json& jsonObjectData) {
			this->parseObject(jsonObjectData, this);
			return *this;
		}

		AuditLogChangeData(const nlohmann::json& jsonObjectData) {
			*this = jsonObjectData;
		}

		virtual ~AuditLogChangeData() = default;

	  protected:
		void parseObject(const nlohmann::json& jsonObjectData, AuditLogChangeData* pDataStructure) {
			if (jsonObjectData.contains("new_value") && !jsonObjectData["new_value"].is_null()) {
				pDataStructure->newValue = jsonObjectData["new_value"];
			}

			if (jsonObjectData.contains("old_value") && !jsonObjectData["old_value"].is_null()) {
				pDataStructure->oldValue = jsonObjectData["old_value"];
			}

			if (jsonObjectData.contains("key") && !jsonObjectData["key"].is_null()) {
				pDataStructure->key = jsonObjectData["key"].get<std::string>();
			}
		}
	};

	/// Guild prune count data. \brief Guild prune count data.
	struct DiscordCoreAPI_Dll GuildPruneCountData : public DataParser<GuildPruneCountData> {
		int32_t count{ 0 };

		GuildPruneCountData() = default;

		GuildPruneCountData& operator=(const nlohmann::json& jsonObjectData) {
			this->parseObject(jsonObjectData, this);
			return *this;
		}

		GuildPruneCountData(const nlohmann::json& jsonObjectData) {
			*this = jsonObjectData;
		}

		virtual ~GuildPruneCountData() = default;

	  protected:
		void parseObject(const nlohmann::json& jsonObjectData, GuildPruneCountData* pDataStructure) {
			if (jsonObjectData.contains("pruned") && !jsonObjectData["pruned"].is_null()) {
				pDataStructure->count = jsonObjectData["pruned"].get<int32_t>();
			}
		}
	};

	/// Audit log entry data. \brief Audit log entry data.
	class DiscordCoreAPI_Dll AuditLogEntryData : public DiscordEntity, public DataParser<AuditLogEntryData> {
	  public:
		std::vector<AuditLogChangeData> changes{};///< Array of audit log change data.
		OptionalAuditEntryInfoData options{};///< Audit log entry info data.
		TimeStamp createdTimeStamp{ "" };///< Time at which this entry was created.
		AuditLogEvent actionType{};///< Audit log action type.
		std::string reason{};///< The reason that was entered for the given change.
		Snowflake targetId{};///< Id of the target User.
		Snowflake userId{};///< Id of the executing User.

		AuditLogEntryData() = default;

		AuditLogEntryData& operator=(const nlohmann::json& jsonObjectData) {
			this->parseObject(jsonObjectData, this);
			return *this;
		}

		AuditLogEntryData(const nlohmann::json& jsonObjectData) {
			*this = jsonObjectData;
		}

		virtual ~AuditLogEntryData() = default;

	  protected:
		void parseObject(const nlohmann::json& jsonObjectData, AuditLogEntryData* pDataStructure) {
			if (jsonObjectData.contains("target_id") && !jsonObjectData["target_id"].is_null()) {
				pDataStructure->targetId = stoull(jsonObjectData["target_id"].get<std::string>());
			}

			if (jsonObjectData.contains("changes") && !jsonObjectData["changes"].is_null()) {
				pDataStructure->changes.clear();
				pDataStructure->changes.reserve(jsonObjectData["changes"].size());
				for (auto& value: jsonObjectData["changes"]) {
					AuditLogChangeData newData{ value };
					pDataStructure->changes.push_back(newData);
				}
				pDataStructure->changes.shrink_to_fit();
			}

			if (jsonObjectData.contains("user_id") && !jsonObjectData["user_id"].is_null()) {
				pDataStructure->userId = stoull(jsonObjectData["user_id"].get<std::string>());
			}

			if (jsonObjectData.contains("id") && !jsonObjectData["id"].is_null()) {
				pDataStructure->id = stoull(jsonObjectData["id"].get<std::string>());
				pDataStructure->createdTimeStamp = pDataStructure->getCreatedAtTimestamp(TimeFormat::LongDateTime);
			}

			if (jsonObjectData.contains("action_type") && !jsonObjectData["action_type"].is_null()) {
				pDataStructure->actionType = jsonObjectData["action_type"].get<AuditLogEvent>();
			}

			if (jsonObjectData.contains("options") && !jsonObjectData["options"].is_null()) {
				pDataStructure->options = jsonObjectData["options"];
			}

			if (jsonObjectData.contains("reason") && !jsonObjectData["reason"].is_null()) {
				pDataStructure->reason = jsonObjectData["reason"].get<std::string>();
			}
		}
	};

	/// Party data. \brief Party data.
	class DiscordCoreAPI_Dll PartyData : public DiscordEntity, public DataParser<PartyData> {
	  public:
		std::vector<int32_t> size{ 0, 0 };///< The size of the party.

		PartyData() = default;

		PartyData& operator=(const nlohmann::json& jsonObjectData) {
			this->parseObject(jsonObjectData, this);
			return *this;
		}

		PartyData(const nlohmann::json& jsonObjectData) {
			*this = jsonObjectData;
		}

		virtual ~PartyData() = default;

	  protected:
		void parseObject(const nlohmann::json& jsonObjectData, PartyData* pDataStructure) {
			if (jsonObjectData.contains("id") && !jsonObjectData["id"].is_null()) {
				pDataStructure->id = stoull(jsonObjectData["id"].get<std::string>());
			}

			if (jsonObjectData.contains("Size") && !jsonObjectData["Size"].is_null()) {
				pDataStructure->size[0] = jsonObjectData["Size"]["CurrentSize"].get<int32_t>();
				pDataStructure->size[1] = jsonObjectData["Size"]["MaxSize"].get<int32_t>();
			}
		}
	};

	/// Assets data. \brief Party data.
	struct DiscordCoreAPI_Dll AssetsData : public DataParser<AssetsData> {
		StringWrapper largeImage{};///< Keyname of an asset to display.
		StringWrapper smallImage{};///< Keyname of an asset to display.
		StringWrapper largeText{};///< Hover text for the large image.
		StringWrapper smallText{};///< Hover text for the small image.

		AssetsData() = default;

		AssetsData& operator=(const nlohmann::json& jsonObjectData) {
			this->parseObject(jsonObjectData, this);
			return *this;
		}

		AssetsData(const nlohmann::json& jsonObjectData) {
			*this = jsonObjectData;
		}

		virtual ~AssetsData() = default;

	  protected:
		void parseObject(const nlohmann::json& jsonObjectData, AssetsData* pDataStructure) {
			if (jsonObjectData.contains("LargeImage") && !jsonObjectData["LargeImage"].is_null()) {
				pDataStructure->largeImage = jsonObjectData["LargeImage"].get<std::string>();
			}

			if (jsonObjectData.contains("LargeText") && !jsonObjectData["LargeText"].is_null()) {
				pDataStructure->largeText = jsonObjectData["LargeText"].get<std::string>();
			}

			if (jsonObjectData.contains("SmallImage") && !jsonObjectData["SmallImage"].is_null()) {
				pDataStructure->smallImage = jsonObjectData["SmallImage"].get<std::string>();
			}

			if (jsonObjectData.contains("SmallText") && !jsonObjectData["SmallText"].is_null()) {
				pDataStructure->smallText = jsonObjectData["SmallText"].get<std::string>();
			}
		}
	};

	/// Secrets data. \brief Secrets data.
	struct DiscordCoreAPI_Dll SecretsData : public DataParser<SecretsData> {
		StringWrapper spectate{};///< Unique hash for the given match context.
		StringWrapper match{};///< Unique hash for Spectate button.
		StringWrapper join{};///< Unique hash for chat invitesand Ask to Join.

		SecretsData() = default;

		SecretsData& operator=(const nlohmann::json& jsonObjectData) {
			this->parseObject(jsonObjectData, this);
			return *this;
		}

		SecretsData(const nlohmann::json& jsonObjectData) {
			*this = jsonObjectData;
		}

		virtual ~SecretsData() = default;

	  protected:
		void parseObject(const nlohmann::json& jsonObjectData, SecretsData* pDataStructure) {
			if (jsonObjectData.contains("Join") && !jsonObjectData["Join"].is_null()) {
				pDataStructure->join = jsonObjectData["Join"].get<std::string>();
			}

			if (jsonObjectData.contains("Spectate") && !jsonObjectData["Spectate"].is_null()) {
				pDataStructure->spectate = jsonObjectData["Spectate"].get<std::string>();
			}

			if (jsonObjectData.contains("Match") && !jsonObjectData["Match"].is_null()) {
				pDataStructure->match = jsonObjectData["Match"].get<std::string>();
			}
		}
	};

	/// Timestamp data. \brief Timestamp data.
	struct DiscordCoreAPI_Dll TimestampData : public DataParser<TimestampData> {
		int64_t start{ 0 };///< Unix timestamp - Send this to have an "elapsed" timer.
		int64_t end{ 0 };///< Unix timestamp - send this to have a "remaining" timer.

		TimestampData() = default;

		TimestampData& operator=(const nlohmann::json& jsonObjectData) {
			this->parseObject(jsonObjectData, this);
			return *this;
		}

		TimestampData(const nlohmann::json& jsonObjectData) {
			*this = jsonObjectData;
		}

		virtual ~TimestampData() = default;

	  protected:
		void parseObject(const nlohmann::json& jsonObjectData, TimestampData* pDataStructure) {
			if (jsonObjectData.contains("Start") && !jsonObjectData["Start"].is_null()) {
				pDataStructure->start = jsonObjectData["Start"].get<int64_t>();
			}

			if (jsonObjectData.contains("End") && !jsonObjectData["End"].is_null()) {
				pDataStructure->end = jsonObjectData["End"].get<int64_t>();
			}
		}
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
		Snowflake applicationId{};///< Application id for the current application.
		TimestampData timestamps{};///< Timestamp data.
		int32_t createdAt{ 0 };///< Timestamp of when the activity began.
		bool instance{ false };///< Whether this activity is an instanced context, like a match.
		SecretsData secrets{};///< Secrets data.
		StringWrapper details{};///< Details about the activity.
		ButtonData buttons{};///< Button Data.
		ActivityType type{};///< Activity data.
		AssetsData assets{};///< Assets data.
		StringWrapper state{};///< The player's current party status.
		StringWrapper name{};///< Name of the activity.
		int32_t flags{ 0 };///< Flags.
		StringWrapper url{};///< Url associated with the activity.
		EmojiData emoji{};///< Emoji associated with the activity.
		PartyData party{};///< Party data.

		ActivityData() = default;

		ActivityData& operator=(const nlohmann::json& jsonObjectData) {
			this->parseObject(jsonObjectData, this);
			return *this;
		}

		ActivityData(const nlohmann::json& jsonObjectData) {
			*this = jsonObjectData;
		}

		virtual ~ActivityData() = default;

	  protected:
		void parseObject(const nlohmann::json& jsonObjectData, ActivityData* pDataStructure) {
			if (jsonObjectData.contains("Name") && !jsonObjectData["Name"].is_null()) {
				pDataStructure->name = jsonObjectData["Name"].get<std::string>();
			}

			if (jsonObjectData.contains("Timestamps") && !jsonObjectData["Timestamps"].is_null()) {
				pDataStructure->timestamps = jsonObjectData["Timestamps"];
			}

			if (jsonObjectData.contains("ApplicationId") && !jsonObjectData["ApplicationId"].is_null()) {
				pDataStructure->applicationId = stoull(jsonObjectData["ApplicationId"].get<std::string>());
			}

			if (jsonObjectData.contains("Details") && !jsonObjectData["Details"].is_null()) {
				pDataStructure->details = jsonObjectData["Details"].get<std::string>();
			}

			if (jsonObjectData.contains("State") && !jsonObjectData["State"].is_null()) {
				pDataStructure->state = jsonObjectData["State"].get<std::string>();
			}

			if (jsonObjectData.contains("ActivityParty") && !jsonObjectData["ActivityParty"].is_null()) {
				pDataStructure->party = jsonObjectData["ActivityParty"];
			}

			if (jsonObjectData.contains("ActivityAssets") && !jsonObjectData["ActivityAssets"].is_null()) {
				pDataStructure->assets = jsonObjectData["ActivityAssets"];
			}

			if (jsonObjectData.contains("ActivitySecrets") && !jsonObjectData["ActivitySecrets"].is_null()) {
				pDataStructure->secrets = jsonObjectData["ActivitySecrets"];
			}

			if (jsonObjectData.contains("Instance") && !jsonObjectData["Instance"].is_null()) {
				pDataStructure->instance = jsonObjectData["Instance"].get<bool>();
			}
		}
	};

	/// Client status data. \brief Client status data.
	struct DiscordCoreAPI_Dll ClientStatusData : public DataParser<ClientStatusData> {
		StringWrapper desktop{};///< Desktop name.
		StringWrapper mobile{};///< Mobile name.
		StringWrapper web{};///< Web link.

		ClientStatusData() = default;

		ClientStatusData& operator=(const nlohmann::json& jsonObjectData) {
			this->parseObject(jsonObjectData, this);
			return *this;
		}

		ClientStatusData(const nlohmann::json& jsonObjectData) {
			*this = jsonObjectData;
		}

		virtual ~ClientStatusData() = default;

	  protected:
		void parseObject(const nlohmann::json& jsonObjectData, ClientStatusData* pDataStructure) {
			if (jsonObjectData.contains("desktop") && !jsonObjectData["desktop"].is_null()) {
				pDataStructure->desktop = jsonObjectData["desktop"].get<std::string>();
			}

			if (jsonObjectData.contains("mobile") && !jsonObjectData["mobile"].is_null()) {
				pDataStructure->mobile = jsonObjectData["mobile"].get<std::string>();
			}

			if (jsonObjectData.contains("web") && !jsonObjectData["web"].is_null()) {
				pDataStructure->web = jsonObjectData["web"].get<std::string>();
			}
		}
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

		WelcomeScreenChannelData& operator=(const nlohmann::json& jsonObjectData) {
			this->parseObject(jsonObjectData, this);
			return *this;
		}

		WelcomeScreenChannelData(const nlohmann::json& jsonObjectData) {
			*this = jsonObjectData;
		}

		virtual ~WelcomeScreenChannelData() = default;

	  protected:
		void parseObject(const nlohmann::json& jsonObjectData, WelcomeScreenChannelData* pDataStructure) {
			if (jsonObjectData.contains("channel_id") && !jsonObjectData["channel_id"].is_null()) {
				pDataStructure->channelId = stoull(jsonObjectData["channel_id"].get<std::string>());
			}

			if (jsonObjectData.contains("description") && !jsonObjectData["description"].is_null()) {
				pDataStructure->description = jsonObjectData["description"].get<std::string>();
			}

			if (jsonObjectData.contains("emoji_id") && !jsonObjectData["emoji_id"].is_null()) {
				pDataStructure->emojiId = stoull(jsonObjectData["emoji_id"].get<std::string>());
			}

			if (jsonObjectData.contains("emoji_name") && !jsonObjectData["emoji_name"].is_null()) {
				pDataStructure->emojiName = jsonObjectData["emoji_name"].get<std::string>();
			}
		}
	};

	/// Welcome screen data. \brief Welcome screen data.
	struct DiscordCoreAPI_Dll WelcomeScreenData : public DataParser<WelcomeScreenData> {
		std::vector<WelcomeScreenChannelData> welcomeChannels{};///< Welcome screen Channel data.
		std::string description{};///< Description of the welcome screen.

		WelcomeScreenData() = default;

		WelcomeScreenData& operator=(const nlohmann::json& jsonObjectData) {
			this->parseObject(jsonObjectData, this);
			return *this;
		}

		WelcomeScreenData(const nlohmann::json& jsonObjectData) {
			*this = jsonObjectData;
		}

		virtual ~WelcomeScreenData() = default;

	  protected:
		void parseObject(const nlohmann::json& jsonObjectData, WelcomeScreenData* pDataStructure) {
			if (jsonObjectData.contains("description") && !jsonObjectData["description"].is_null()) {
				pDataStructure->description = jsonObjectData["description"].get<std::string>();
			}

			if (jsonObjectData.contains("welcome_channels") && !jsonObjectData["welcome_channels"].is_null()) {
				pDataStructure->welcomeChannels.clear();
				pDataStructure->welcomeChannels.reserve(jsonObjectData["welcome_channels"].size());
				for (auto& value: jsonObjectData["welcome_channels"]) {
					WelcomeScreenChannelData newData{ value };
					pDataStructure->welcomeChannels.push_back(newData);
				}
				pDataStructure->welcomeChannels.shrink_to_fit();
			}
		}
	};

	/// Presence update data. \brief Presence update data.
	struct DiscordCoreAPI_Dll PresenceUpdateData : public DataParser<PresenceUpdateData> {
		std::vector<ActivityData> activities{};///< Array of activities.
		ClientStatusData clientStatus{};///< Current client status.
		StringWrapper status{};///< Status of the current presence.
		Snowflake guildId{};///< Guild id for the current presence.
		UserData user{};///< User data for the current presence.

		PresenceUpdateData() = default;

		PresenceUpdateData& operator=(const nlohmann::json& jsonObjectData) {
			this->parseObject(jsonObjectData, this);
			return *this;
		}

		PresenceUpdateData(const nlohmann::json& jsonObjectData) {
			*this = jsonObjectData;
		}

		virtual ~PresenceUpdateData() = default;

	  protected:
		void parseObject(const nlohmann::json& jsonObjectData, PresenceUpdateData* pDataStructure) {
			if (jsonObjectData.contains("user") && !jsonObjectData["user"].is_null()) {
				pDataStructure->user = jsonObjectData["user"];
			}

			if (jsonObjectData.contains("guild_id") && !jsonObjectData["guild_id"].is_null()) {
				pDataStructure->guildId = stoull(jsonObjectData["guild_id"].get<std::string>());
			}

			if (jsonObjectData.contains("status") && !jsonObjectData["status"].is_null()) {
				pDataStructure->status = jsonObjectData["status"].get<std::string>();
			}

			if (jsonObjectData.contains("activities") && !jsonObjectData["activities"].is_null()) {
				pDataStructure->activities.clear();
				pDataStructure->activities.reserve(jsonObjectData["activities"].size());
				for (auto& value: jsonObjectData["activities"]) {
					ActivityData newData{ value };
					pDataStructure->activities.push_back(newData);
				}
				pDataStructure->activities.shrink_to_fit();
			}

			if (jsonObjectData.contains("client_status") && !jsonObjectData["client_status"].is_null()) {
				pDataStructure->clientStatus = jsonObjectData["client_status"];
			}
		}
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

		StageInstanceData& operator=(const nlohmann::json& jsonObjectData) {
			this->parseObject(jsonObjectData, this);
			return *this;
		}

		StageInstanceData(const nlohmann::json& jsonObjectData) {
			*this = jsonObjectData;
		}

		virtual ~StageInstanceData() = default;

	  protected:
		void parseObject(const nlohmann::json& jsonObjectData, StageInstanceData* pDataStructure) {
			if (jsonObjectData.contains("id") && !jsonObjectData["id"].is_null()) {
				pDataStructure->id = stoull(jsonObjectData["id"].get<std::string>());
			}

			if (jsonObjectData.contains("guild_id") && !jsonObjectData["guild_id"].is_null()) {
				pDataStructure->guildId = stoull(jsonObjectData["guild_id"].get<std::string>());
			}

			if (jsonObjectData.contains("channel_id") && !jsonObjectData["channel_id"].is_null()) {
				pDataStructure->channelId = stoull(jsonObjectData["channel_id"].get<std::string>());
			}

			if (jsonObjectData.contains("topic") && !jsonObjectData["topic"].is_null()) {
				pDataStructure->topic = jsonObjectData["topic"].get<std::string>();
			}

			if (jsonObjectData.contains("privacy_level") && !jsonObjectData["privacy_level"].is_null()) {
				pDataStructure->privacyLevel = jsonObjectData["privacy_level"].get<StageInstancePrivacyLevel>();
			}

			if (jsonObjectData.contains("discoverable_disabled") && !jsonObjectData["discoverable_disabled"].is_null()) {
				pDataStructure->discoverableDisabled = jsonObjectData["discoverable_disabled"].get<bool>();
			}
		}
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

		StickerData& operator=(const nlohmann::json& jsonObjectData) {
			this->parseObject(jsonObjectData, this);
			return *this;
		}

		StickerData(const nlohmann::json& jsonObjectData) {
			*this = jsonObjectData;
		}

		virtual ~StickerData() = default;

	  protected:
		void parseObject(const nlohmann::json& jsonObjectData, StickerData* pDataStructure) {
			if (jsonObjectData.contains("asset") && !jsonObjectData["asset"].is_null()) {
				pDataStructure->asset = jsonObjectData["asset"].get<std::string>();
			}

			if (jsonObjectData.contains("description") && !jsonObjectData["description"].is_null()) {
				pDataStructure->description = jsonObjectData["description"].get<std::string>();
			}

			if (jsonObjectData.contains("format_type") && !jsonObjectData["format_type"].is_null()) {
				pDataStructure->formatType = jsonObjectData["format_type"].get<StickerFormatType>();
			}

			if (jsonObjectData.contains("available") && !jsonObjectData["available"].is_null()) {
				pDataStructure->stickerFlags = setBool<int8_t, StickerFlags>(pDataStructure->stickerFlags, StickerFlags::Available, jsonObjectData["available"].get<bool>());
			}

			if (jsonObjectData.contains("guild_id") && !jsonObjectData["guild_id"].is_null()) {
				pDataStructure->guildId = stoull(jsonObjectData["guild_id"].get<std::string>());
			}

			if (jsonObjectData.contains("pack_id") && !jsonObjectData["pack_id"].is_null()) {
				pDataStructure->packId = jsonObjectData["pack_id"].get<std::string>();
			}

			if (jsonObjectData.contains("type") && !jsonObjectData["type"].is_null()) {
				pDataStructure->type = jsonObjectData["type"].get<StickerType>();
			}

			if (jsonObjectData.contains("sort_value") && !jsonObjectData["sort_value"].is_null()) {
				pDataStructure->sortValue = jsonObjectData["sort_value"].get<int32_t>();
			}

			if (jsonObjectData.contains("name") && !jsonObjectData["name"].is_null()) {
				pDataStructure->name = jsonObjectData["name"].get<std::string>();
			}

			if (jsonObjectData.contains("id") && !jsonObjectData["id"].is_null()) {
				pDataStructure->id = stoull(jsonObjectData["id"].get<std::string>());
			}

			if (jsonObjectData.contains("user") && !jsonObjectData["user"].is_null()) {
				pDataStructure->user = jsonObjectData["user"];
			}
		}
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

		GuildPreviewData& operator=(const nlohmann::json& jsonObjectData) {
			this->parseObject(jsonObjectData, this);
			return *this;
		}

		GuildPreviewData(const nlohmann::json& jsonObjectData) {
			*this = jsonObjectData;
		}

		virtual ~GuildPreviewData() = default;

	  protected:
		void parseObject(const nlohmann::json& jsonObjectData, GuildPreviewData* pDataStructure) {
			if (jsonObjectData.contains("approximate_presence_count") && !jsonObjectData["approximate_presence_count"].is_null()) {
				pDataStructure->approximatePresenceCount = jsonObjectData["approximate_presence_count"].get<int32_t>();
			}

			if (jsonObjectData.contains("approximate_member_count") && !jsonObjectData["approximate_member_count"].is_null()) {
				pDataStructure->approximateMemberCount = jsonObjectData["approximate_member_count"].get<int32_t>();
			}

			if (jsonObjectData.contains("discovery_splash") && !jsonObjectData["discovery_splash"].is_null()) {
				pDataStructure->discoverySplash = jsonObjectData["discovery_splash"].get<std::string>();
			}

			if (jsonObjectData.contains("emojis") && !jsonObjectData["emojis"].is_null()) {
				pDataStructure->emojis.clear();
				pDataStructure->emojis.reserve(jsonObjectData["emojis"].size());
				for (auto& value: jsonObjectData["emojis"]) {
					EmojiData newData{ value };
					pDataStructure->emojis.push_back(newData);
				}
				pDataStructure->emojis.shrink_to_fit();
			}

			if (jsonObjectData.contains("stickers") && !jsonObjectData["stickers"].is_null()) {
				pDataStructure->emojis.clear();
				pDataStructure->emojis.reserve(jsonObjectData["stickers"].size());
				for (auto& value: jsonObjectData["stickers"]) {
					StickerData newData{ value };
					pDataStructure->stickers.push_back(newData);
				}
				pDataStructure->stickers.shrink_to_fit();
			}

			if (jsonObjectData.contains("features") && !jsonObjectData["features"].is_null()) {
				pDataStructure->features = jsonObjectData["features"].get<std::vector<std::string>>();
			}

			if (jsonObjectData.contains("description") && !jsonObjectData["description"].is_null()) {
				pDataStructure->description = jsonObjectData["description"].get<std::string>();
			}

			if (jsonObjectData.contains("splash") && !jsonObjectData["splash"].is_null()) {
				pDataStructure->splash = jsonObjectData["splash"].get<std::string>();
			}

			if (jsonObjectData.contains("icon") && !jsonObjectData["icon"].is_null()) {
				pDataStructure->icon = jsonObjectData["icon"].get<std::string>();
			}

			if (jsonObjectData.contains("name") && !jsonObjectData["name"].is_null()) {
				pDataStructure->name = jsonObjectData["name"].get<std::string>();
			}

			if (jsonObjectData.contains("id") && !jsonObjectData["id"].is_null()) {
				pDataStructure->id = stoull(jsonObjectData["id"].get<std::string>());
			}
		}
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
		DiscordCoreClient* discordCoreClient{ nullptr };///< A pointer to the DiscordCoreClient.
		VoiceConnection* voiceConnectionPtr{ nullptr };///< A pointer to the VoiceConnection, if present.
		std::vector<StringWrapper> features{};///< List of Guild features.
		std::vector<Snowflake> channels{};///< Array of Guild channels.
		std::vector<Snowflake> members{};///< Array of GuildMembers.
		std::vector<Snowflake> roles{};///< Array of Guild roles.
		TimeStamp joinedAt{ "" };///< When the bot joined this Guild.
		int32_t memberCount{ 0 };///< Member count.
		StringWrapper icon{};///< Url to the Guild's icon.
		StringWrapper name{};///< The Guild's name.
		Snowflake ownerId{};///< User id of the Guild's owner.
		int8_t flags{ 0 };///< Guild flags.

		GuildData() = default;

		GuildData& operator=(const nlohmann::json& jsonObjectData) {
			this->parseObject(jsonObjectData, this);
			return *this;
		}

		GuildData(const nlohmann::json& jsonObjectData) {
			*this = jsonObjectData;
		}

		VoiceConnection* connectToVoice(const Snowflake guildMemberId, const Snowflake channelId = 0, bool selfDeaf = false, bool selfMute = false);

		void insertGuildMember(GuildMemberData theData);

		void insertChannel(ChannelData theData);

		void insertRole(RoleData theData);

		bool areWeConnected();

		void initialize();

		void disconnect();

		virtual ~GuildData() = default;

	  protected:
		void parseObject(const nlohmann::json& jsonObjectData, GuildData* pDataStructure) {
			if (jsonObjectData.contains("id") && !jsonObjectData["id"].is_null()) {
				pDataStructure->id = stoull(jsonObjectData["id"].get<std::string>());
			}

			if (jsonObjectData.contains("icon") && !jsonObjectData["icon"].is_null()) {
				std::string iconUrlString = "https://cdn.discordapp.com/";
				iconUrlString += "icons/" + std::to_string(pDataStructure->id) + "/" + jsonObjectData["icon"].get<std::string>() + ".png";
				pDataStructure->icon = iconUrlString;
			}

			if (jsonObjectData.contains("name") && !jsonObjectData["name"].is_null()) {
				pDataStructure->name = jsonObjectData["name"].get<std::string>();
			}

			if (jsonObjectData.contains("joined_at") && !jsonObjectData["joined_at"].is_null()) {
				pDataStructure->joinedAt = jsonObjectData["joined_at"].get<std::string>();
			}

			if (jsonObjectData.contains("owner") && !jsonObjectData["owner"].is_null()) {
				pDataStructure->flags = setBool<int8_t, GuildFlags>(pDataStructure->flags, GuildFlags::Owner, jsonObjectData["owner"].get<bool>());
			}

			if (jsonObjectData.contains("owner_id") && !jsonObjectData["owner_id"].is_null()) {
				pDataStructure->ownerId = stoull(jsonObjectData["owner_id"].get<std::string>());
			}

			if (jsonObjectData.contains("features") && !jsonObjectData["features"].is_null()) {
				for (auto& value: jsonObjectData["features"].get<std::vector<std::string>>()) {
					pDataStructure->features.push_back(value);
				}
			}

			if (jsonObjectData.contains("roles") && !jsonObjectData["roles"].is_null()) {
				for (auto& value: jsonObjectData["roles"]) {
					RoleData newData{ value };
					pDataStructure->roles.push_back(newData.id);
					this->insertRole(newData);
				}
			}

			if (jsonObjectData.contains("widget_enabled") && !jsonObjectData["widget_enabled"].is_null()) {
				pDataStructure->flags = setBool<int8_t, GuildFlags>(pDataStructure->flags, GuildFlags::WidgetEnabled, jsonObjectData["widget_enabled"].get<bool>());
			}

			if (jsonObjectData.contains("large") && !jsonObjectData["large"].is_null()) {
				pDataStructure->flags = setBool<int8_t, GuildFlags>(pDataStructure->flags, GuildFlags::Large, jsonObjectData["large"].get<bool>());
			}

			if (jsonObjectData.contains("unavailable") && !jsonObjectData["unavailable"].is_null()) {
				pDataStructure->flags = setBool<int8_t, GuildFlags>(pDataStructure->flags, GuildFlags::Unavailable, jsonObjectData["unavailable"].get<bool>());
			}

			if (jsonObjectData.contains("member_count") && !jsonObjectData["member_count"].is_null()) {
				pDataStructure->memberCount = jsonObjectData["member_count"].get<int32_t>();
			}

			if (jsonObjectData.contains("voice_states") && !jsonObjectData["voice_states"].is_null()) {
				pDataStructure->voiceStates.clear();
				for (auto& value: jsonObjectData["voice_states"]) {
					VoiceStateData newData{ value };
					Snowflake userId = newData.userId;
					pDataStructure->voiceStates[userId] = newData;
				}
			}

			if (jsonObjectData.contains("members") && !jsonObjectData["members"].is_null()) {
				for (auto& value: jsonObjectData["members"]) {
					GuildMemberData newData{ value };
					newData.guildId = pDataStructure->id;
					pDataStructure->members.push_back(newData.id);
					this->insertGuildMember(newData);
				}
			}

			if (jsonObjectData.contains("channels") && !jsonObjectData["channels"].is_null()) {
				for (auto& value: jsonObjectData["channels"]) {
					ChannelData newData{ value };
					newData.guildId = pDataStructure->id;
					pDataStructure->channels.push_back(newData.id);
					this->insertChannel(newData);
				}
			}

			if (jsonObjectData.contains("presences") && !jsonObjectData["presences"].is_null()) {
				pDataStructure->presences.clear();
				for (auto& value: jsonObjectData["presences"]) {
					PresenceUpdateData newData{ value };
					Snowflake presenceId = newData.user.id;
					pDataStructure->presences[presenceId] = newData;
				}
			}
		}
	};

	class DiscordCoreAPI_Dll GuildDataVector : public DataParser<GuildDataVector> {
	  public:
		friend class Guilds;

		GuildDataVector() = default;

		operator std::vector<GuildData>() {
			return this->theGuildDatas;
		}

		GuildDataVector& operator=(const nlohmann::json& jsonObjectData) {
			this->parseObject(jsonObjectData, this);
			return *this;
		}

		GuildDataVector(const nlohmann::json& jsonObjectData) {
			*this = jsonObjectData;
		}

		virtual ~GuildDataVector() = default;

	  protected:
		std::vector<GuildData> theGuildDatas{};

		void parseObject(const nlohmann::json& jsonObjectData, GuildDataVector* pDataStructure) {
			pDataStructure->theGuildDatas.reserve(jsonObjectData.size());
			for (auto& value: jsonObjectData) {
				DiscordCoreAPI::GuildData newData{ value };
				pDataStructure->theGuildDatas.push_back(newData);
			}
			pDataStructure->theGuildDatas.shrink_to_fit();
		}
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

		GuildScheduledEventMetadata& operator=(const nlohmann::json& jsonObjectData) {
			this->parseObject(jsonObjectData, this);
			return *this;
		}

		GuildScheduledEventMetadata(const nlohmann::json& jsonObjectData) {
			*this = jsonObjectData;
		}

		virtual ~GuildScheduledEventMetadata() = default;

	  protected:
		void parseObject(const nlohmann::json& jsonObjectData, GuildScheduledEventMetadata* pDataStructure) {
			if (jsonObjectData.contains("location") && !jsonObjectData["location"].is_null()) {
				pDataStructure->location = jsonObjectData["location"].get<std::string>();
			}
		}
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

		GuildScheduledEventData& operator=(const nlohmann::json& jsonObjectData) {
			this->parseObject(jsonObjectData, this);
			return *this;
		}

		GuildScheduledEventData(const nlohmann::json& jsonObjectData) {
			*this = jsonObjectData;
		}

		virtual ~GuildScheduledEventData() = default;

	  protected:
		void parseObject(const nlohmann::json& jsonObjectData, GuildScheduledEventData* pDataStructure) {
			if (jsonObjectData.contains("privacy_level") && !jsonObjectData["privacy_level"].is_null()) {
				pDataStructure->privacyLevel = jsonObjectData["privacy_level"].get<GuildScheduledEventPrivacyLevel>();
			}

			if (jsonObjectData.contains("entity_type") && !jsonObjectData["entity_type"].is_null()) {
				pDataStructure->entityType = jsonObjectData["entity_type"].get<GuildScheduledEventEntityType>();
			}

			if (jsonObjectData.contains("status") && !jsonObjectData["status"].is_null()) {
				pDataStructure->status = jsonObjectData["status"].get<GuildScheduledEventStatus>();
			}

			if (jsonObjectData.contains("entity_metadata") && !jsonObjectData["entity_metadata"].is_null()) {
				pDataStructure->entityMetadata = jsonObjectData["entity_metadata"];
			}

			if (jsonObjectData.contains("scheduled_start_time") && !jsonObjectData["scheduled_start_time"].is_null()) {
				pDataStructure->scheduledStartTime = jsonObjectData["scheduled_start_time"].get<std::string>();
			}

			if (jsonObjectData.contains("scheduled_end_time") && !jsonObjectData["scheduled_end_time"].is_null()) {
				pDataStructure->scheduledEndTime = jsonObjectData["scheduled_end_time"].get<std::string>();
			}

			if (jsonObjectData.contains("user_count") && !jsonObjectData["user_count"].is_null()) {
				pDataStructure->userCount = jsonObjectData["user_count"].get<uint32_t>();
			}

			if (jsonObjectData.contains("description") && !jsonObjectData["description"].is_null()) {
				pDataStructure->description = jsonObjectData["description"].get<std::string>();
			}

			if (jsonObjectData.contains("channel_id") && !jsonObjectData["channel_id"].is_null()) {
				pDataStructure->channelId = stoull(jsonObjectData["channel_id"].get<std::string>());
			}

			if (jsonObjectData.contains("creator_id") && !jsonObjectData["creator_id"].is_null()) {
				pDataStructure->creatorId = jsonObjectData["creator_id"].get<std::string>();
			}

			if (jsonObjectData.contains("entity_id") && !jsonObjectData["entity_id"].is_null()) {
				pDataStructure->entityId = jsonObjectData["entity_id"].get<std::string>();
			}

			if (jsonObjectData.contains("guild_id") && !jsonObjectData["guild_id"].is_null()) {
				pDataStructure->guildId = stoull(jsonObjectData["guild_id"].get<std::string>());
			}

			if (jsonObjectData.contains("creator") && !jsonObjectData["creator"].is_null()) {
				pDataStructure->creator = jsonObjectData["creator"];
			}

			if (jsonObjectData.contains("name") && !jsonObjectData["name"].is_null()) {
				pDataStructure->name = jsonObjectData["name"].get<std::string>();
			}

			if (jsonObjectData.contains("id") && !jsonObjectData["id"].is_null()) {
				pDataStructure->id = stoull(jsonObjectData["id"].get<std::string>());
			}
		}
	};

	/// Data representing a single GuildScheduledEventUser. \brief Data representing a single GuildScheduledEventUser.
	struct DiscordCoreAPI_Dll GuildScheduledEventUserData : public DataParser<GuildScheduledEventUserData> {
		std::string guildScheduledEventId{};///< The scheduled event id which the User subscribed to/
		GuildMemberData member{};///< Guild member data for this User for the Guild which this event belongs to, if any.
		UserData user{};///< User which subscribed to an event.

		GuildScheduledEventUserData() = default;

		GuildScheduledEventUserData& operator=(const nlohmann::json& jsonObjectData) {
			this->parseObject(jsonObjectData, this);
			return *this;
		}

		GuildScheduledEventUserData(const nlohmann::json& jsonObjectData) {
			*this = jsonObjectData;
		}

		virtual ~GuildScheduledEventUserData() = default;

	  protected:
		void parseObject(const nlohmann::json& jsonObjectData, GuildScheduledEventUserData* pDataStructure) {
			if (jsonObjectData.contains("guild_scheduled_event_id") && !jsonObjectData["guild_scheduled_event_id"].is_null()) {
				pDataStructure->guildScheduledEventId = jsonObjectData["guild_scheduled_event_id"].get<std::string>();
			}

			if (jsonObjectData.contains("user") && !jsonObjectData["user"].is_null()) {
				pDataStructure->user = jsonObjectData["user"];
			}

			if (jsonObjectData.contains("member") && !jsonObjectData["member"].is_null()) {
				pDataStructure->member = jsonObjectData["member"];
			}
		}
	};

	class DiscordCoreAPI_Dll GuildScheduledEventUserDataVector : public DataParser<GuildScheduledEventUserDataVector> {
	  public:
		GuildScheduledEventUserDataVector() = default;

		operator std::vector<GuildScheduledEventUserData>() {
			return this->theGuildScheduledEventUserDatas;
		}

		GuildScheduledEventUserDataVector& operator=(const nlohmann::json& jsonObjectData) {
			this->parseObject(jsonObjectData, this);
			return *this;
		}

		GuildScheduledEventUserDataVector(const nlohmann::json& jsonObjectData) {
			*this = jsonObjectData;
		}

		virtual ~GuildScheduledEventUserDataVector() = default;

	  protected:
		std::vector<GuildScheduledEventUserData> theGuildScheduledEventUserDatas{};

		void parseObject(const nlohmann::json& jsonObjectData, GuildScheduledEventUserDataVector* pDataStructure) {
			pDataStructure->theGuildScheduledEventUserDatas.reserve(jsonObjectData.size());
			for (auto& value: jsonObjectData) {
				DiscordCoreAPI::GuildScheduledEventUserData newData{ value };
				pDataStructure->theGuildScheduledEventUserDatas.push_back(newData);
			}
			pDataStructure->theGuildScheduledEventUserDatas.shrink_to_fit();
		}
	};

	/// Invite data. \brief Invite data.
	struct DiscordCoreAPI_Dll InviteData : public DataParser<InviteData> {
		GuildScheduledEventData guildScheduledEvent{};///< Scheduled Guild event.
		int32_t approximatePresenceCount{ 0 };///< Approximate presence count.
		ApplicationData targetApplication{};///< Application data.
		int32_t approximateMemberCount{ 0 };///< Approximate member count.
		StageInstanceData stageInstance{};///< Stage instance data.
		TimeStamp createdAt{ "" };///< Time it was created at.
		TimeStamp expiresAt{ "" };///< When the invite expires.
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

		InviteData& operator=(const nlohmann::json& jsonObjectData) {
			this->parseObject(jsonObjectData, this);
			return *this;
		}

		InviteData(const nlohmann::json& jsonObjectData) {
			*this = jsonObjectData;
		}

		virtual ~InviteData() = default;

	  protected:
		void parseObject(const nlohmann::json& jsonObjectData, InviteData* pDataStructure) {
			if (jsonObjectData.contains("code") && !jsonObjectData["code"].is_null() && jsonObjectData["code"].is_string()) {
				pDataStructure->code = jsonObjectData["code"].get<std::string>();
			} else if (jsonObjectData.contains("code") && !jsonObjectData["code"].is_null() && jsonObjectData["code"].is_number_integer()) {
				pDataStructure->code = std::to_string(jsonObjectData["code"].get<int32_t>());
			}

			if (jsonObjectData.contains("guild") && !jsonObjectData["guild"].is_null()) {
				pDataStructure->guild = jsonObjectData["guild"];
			}

			if (jsonObjectData.contains("channel") && !jsonObjectData["channel"].is_null()) {
				pDataStructure->channel = jsonObjectData["channel"];
			}

			if (jsonObjectData.contains("inviter") && !jsonObjectData["inviter"].is_null()) {
				pDataStructure->inviter = jsonObjectData["inviter"];
			}

			if (jsonObjectData.contains("target_type") && !jsonObjectData["target_type"].is_null()) {
				pDataStructure->targetType = jsonObjectData["target_type"].get<int32_t>();
			}

			if (jsonObjectData.contains("target_user") && !jsonObjectData["target_user"].is_null()) {
				pDataStructure->targetUser = jsonObjectData["target_user"];
			}

			if (jsonObjectData.contains("target_application") && !jsonObjectData["target_application"].is_null()) {
				pDataStructure->targetApplication = jsonObjectData["target_application"];
			}

			if (jsonObjectData.contains("approximate_presence_count") && !jsonObjectData["approximate_presence_count"].is_null()) {
				pDataStructure->approximatePresenceCount = jsonObjectData["approximate_presence_count"].get<int32_t>();
			}

			if (jsonObjectData.contains("approximate_member_count") && !jsonObjectData["approximate_member_count"].is_null()) {
				pDataStructure->approximateMemberCount = jsonObjectData["approximate_member_count"].get<int32_t>();
			}

			if (jsonObjectData.contains("guild_id") && !jsonObjectData["guild_id"].is_null()) {
				pDataStructure->guildId = stoull(jsonObjectData["guild_id"].get<std::string>());
			}

			if (jsonObjectData.contains("expires_at") && !jsonObjectData["expires_at"].is_null()) {
				pDataStructure->expiresAt = jsonObjectData["expires_at"].get<std::string>();
			}

			if (jsonObjectData.contains("stage_instance") && !jsonObjectData["stage_instance"].is_null()) {
				pDataStructure->stageInstance = jsonObjectData["stage_instance"];
			}

			if (jsonObjectData.contains("guild_scheduled_event") && !jsonObjectData["guild_scheduled_event"].is_null()) {
				pDataStructure->guildScheduledEvent = jsonObjectData["guild_scheduled_event"];
			}

			if (jsonObjectData.contains("uses") && !jsonObjectData["uses"].is_null()) {
				pDataStructure->uses = jsonObjectData["uses"].get<int32_t>();
			}

			if (jsonObjectData.contains("max_uses") && !jsonObjectData["max_uses"].is_null()) {
				pDataStructure->maxUses = jsonObjectData["max_uses"].get<int32_t>();
			}

			if (jsonObjectData.contains("max_age") && !jsonObjectData["max_age"].is_null()) {
				pDataStructure->maxAge = jsonObjectData["max_age"].get<int32_t>();
			}

			if (jsonObjectData.contains("temporary") && !jsonObjectData["temporary"].is_null()) {
				pDataStructure->temporary = jsonObjectData["temporary"].get<bool>();
			}

			if (jsonObjectData.contains("created_at") && !jsonObjectData["created_at"].is_null()) {
				pDataStructure->createdAt = jsonObjectData["created_at"].get<std::string>();
			}
		}
	};

	class DiscordCoreAPI_Dll InviteDataVector : public DataParser<InviteDataVector> {
	  public:
		InviteDataVector() = default;

		operator std::vector<InviteData>() {
			return this->theInviteDatas;
		}

		InviteDataVector& operator=(const nlohmann::json& jsonObjectData) {
			this->parseObject(jsonObjectData, this);
			return *this;
		}

		InviteDataVector(const nlohmann::json& jsonObjectData) {
			*this = jsonObjectData;
		}

		virtual ~InviteDataVector() = default;

	  protected:
		std::vector<InviteData> theInviteDatas{};

		void parseObject(const nlohmann::json& jsonObjectData, InviteDataVector* pDataStructure) {
			pDataStructure->theInviteDatas.reserve(jsonObjectData.size());
			for (auto& value: jsonObjectData) {
				DiscordCoreAPI::InviteData newData{ value };
				pDataStructure->theInviteDatas.push_back(newData);
			}
			pDataStructure->theInviteDatas.shrink_to_fit();
		}
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

		GuildTemplateData& operator=(const nlohmann::json& jsonObjectData) {
			this->parseObject(jsonObjectData, this);
			return *this;
		}

		GuildTemplateData(const nlohmann::json& jsonObjectData) {
			*this = jsonObjectData;
		}

		virtual ~GuildTemplateData() = default;

	  protected:
		void parseObject(const nlohmann::json& jsonObjectData, GuildTemplateData* pDataStructure) {
			if (jsonObjectData.contains("serialized_source_guild") && !jsonObjectData["serialized_source_guild"].is_null()) {
				pDataStructure->serializedSourceGuild = jsonObjectData["serialized_source_guild"];
			}

			if (jsonObjectData.contains("creator") && !jsonObjectData["creator"].is_null()) {
				pDataStructure->creator = jsonObjectData["creator"];
			}

			if (jsonObjectData.contains("source_guild_id") && !jsonObjectData["source_guild_id"].is_null()) {
				pDataStructure->sourceGuildId = jsonObjectData["source_guild_id"].get<std::string>();
			}

			if (jsonObjectData.contains("description") && !jsonObjectData["description"].is_null()) {
				pDataStructure->description = jsonObjectData["description"].get<std::string>();
			}

			if (jsonObjectData.contains("usage_count") && !jsonObjectData["usage_count"].is_null()) {
				pDataStructure->usageCount = jsonObjectData["usage_count"].get<uint32_t>();
			}

			if (jsonObjectData.contains("creator_id") && !jsonObjectData["creator_id"].is_null()) {
				pDataStructure->creatorId = jsonObjectData["creator_id"].get<std::string>();
			}

			if (jsonObjectData.contains("created_at") && !jsonObjectData["created_at"].is_null()) {
				pDataStructure->createdAt = jsonObjectData["created_at"].get<std::string>();
			}

			if (jsonObjectData.contains("updated_at") && !jsonObjectData["updated_at"].is_null()) {
				pDataStructure->updatedAt = jsonObjectData["updated_at"].get<std::string>();
			}

			if (jsonObjectData.contains("is_dirty") && !jsonObjectData["is_dirty"].is_null()) {
				pDataStructure->isDirty = jsonObjectData["is_dirty"].get<bool>();
			}

			if (jsonObjectData.contains("code") && !jsonObjectData["code"].is_null()) {
				pDataStructure->code = jsonObjectData["code"].get<std::string>();
			}

			if (jsonObjectData.contains("name") && !jsonObjectData["name"].is_null()) {
				pDataStructure->name = jsonObjectData["name"].get<std::string>();
			}
		}
	};

	class DiscordCoreAPI_Dll GuildTemplateDataVector : public DataParser<GuildTemplateDataVector> {		
	  public:
		GuildTemplateDataVector() = default;

		operator std::vector<GuildTemplateData>() {
			return this->theGuildTemplateDatas;
		}

		GuildTemplateDataVector& operator=(const nlohmann::json& jsonObjectData) {
			this->parseObject(jsonObjectData, this);
			return *this;
		}

		GuildTemplateDataVector(const nlohmann::json& jsonObjectData) {
			*this = jsonObjectData;
		}

		virtual ~GuildTemplateDataVector() = default;

	  protected:
		std::vector<GuildTemplateData> theGuildTemplateDatas{};

		void parseObject(const nlohmann::json& jsonObjectData, GuildTemplateDataVector* pDataStructure) {
			pDataStructure->theGuildTemplateDatas.reserve(jsonObjectData.size());
			for (auto& value: jsonObjectData) {
				DiscordCoreAPI::GuildTemplateData newData{ value };
				pDataStructure->theGuildTemplateDatas.push_back(newData);
			}
			pDataStructure->theGuildTemplateDatas.shrink_to_fit();
		}
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

		WebHookData& operator=(const nlohmann::json& jsonObjectData) {
			this->parseObject(jsonObjectData, this);
			return *this;
		}

		WebHookData(const nlohmann::json& jsonObjectData) {
			*this = jsonObjectData;
		}

		virtual ~WebHookData() = default;

	  protected:
		void parseObject(const nlohmann::json& jsonObjectData, WebHookData* pDataStructure) {
			if (jsonObjectData.contains("id") && !jsonObjectData["id"].is_null()) {
				pDataStructure->id = stoull(jsonObjectData["id"].get<std::string>());
			}

			if (jsonObjectData.contains("type") && !jsonObjectData["type"].is_null()) {
				pDataStructure->type = jsonObjectData["type"].get<WebHookType>();
			}

			if (jsonObjectData.contains("guild_id") && !jsonObjectData["guild_id"].is_null()) {
				pDataStructure->guildId = stoull(jsonObjectData["guild_id"].get<std::string>());
			}

			if (jsonObjectData.contains("channel_id") && !jsonObjectData["channel_id"].is_null()) {
				pDataStructure->channelId = stoull(jsonObjectData["channel_id"].get<std::string>());
			}

			if (jsonObjectData.contains("user") && !jsonObjectData["user"].is_null()) {
				pDataStructure->user = jsonObjectData["user"];
			}

			if (jsonObjectData.contains("name") && !jsonObjectData["name"].is_null()) {
				pDataStructure->name = jsonObjectData["name"].get<std::string>();
			}

			if (jsonObjectData.contains("avatar") && !jsonObjectData["avatar"].is_null()) {
				pDataStructure->avatar = jsonObjectData["avatar"].get<std::string>();
			}

			if (jsonObjectData.contains("token") && !jsonObjectData["token"].is_null()) {
				pDataStructure->token = jsonObjectData["token"].get<std::string>();
			}

			if (jsonObjectData.contains("application_id") && !jsonObjectData["application_id"].is_null()) {
				pDataStructure->applicationId = stoull(jsonObjectData["application_id"].get<std::string>());
			}

			if (jsonObjectData.contains("source_guild") && !jsonObjectData["source_guild"].is_null()) {
				pDataStructure->sourceGuild = jsonObjectData["source_guild"];
			}

			if (jsonObjectData.contains("source_channel") && !jsonObjectData["source_channel"].is_null()) {
				pDataStructure->sourceChannel = jsonObjectData["source_channel"];
			}

			if (jsonObjectData.contains("url") && !jsonObjectData["url"].is_null()) {
				pDataStructure->url = jsonObjectData["url"].get<std::string>();
			}
		}
	};

	class DiscordCoreAPI_Dll WebHookDataVector : public DataParser<WebHookDataVector> {
	  public:
		std::vector<WebHookData> theWebHookDatas{};

		WebHookDataVector() = default;

		WebHookDataVector& operator=(const nlohmann::json& jsonObjectData) {
			this->parseObject(jsonObjectData, this);
			return *this;
		}

		WebHookDataVector(const nlohmann::json& jsonObjectData) {
			*this = jsonObjectData;
		}

		virtual ~WebHookDataVector() = default;

	  protected:
		void parseObject(const nlohmann::json& jsonObjectData, WebHookDataVector* pDataStructure) {
			pDataStructure->theWebHookDatas.reserve(jsonObjectData.size());
			for (auto& value: jsonObjectData) {
				DiscordCoreAPI::WebHookData newData{ value };
				pDataStructure->theWebHookDatas.push_back(newData);
			}
			pDataStructure->theWebHookDatas.shrink_to_fit();
		}
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
		auto getAuditLogData(const Snowflake& userIdOfChanger, AuditLogEvent auditLogType) {
			for (auto& value: this->auditLogEntries) {
				if (value.id == userIdOfChanger && value.actionType == auditLogType) {
					return value;
				}
			}
			return AuditLogEntryData();
		}
		auto getAuditLogData(AuditLogEvent auditLogType, const Snowflake& userIdOfTarget) {
			for (auto& value: this->auditLogEntries) {
				if (value.targetId == userIdOfTarget && value.actionType == auditLogType) {
					return value;
				}
			}
			return AuditLogEntryData();
		}

		AuditLogData() = default;

		AuditLogData& operator=(const nlohmann::json& jsonObjectData) {
			this->parseObject(jsonObjectData, this);
			return *this;
		}

		AuditLogData(const nlohmann::json& jsonObjectData) {
			*this = jsonObjectData;
		}

		virtual ~AuditLogData() = default;

	  protected:
		void parseObject(const nlohmann::json& jsonObjectData, AuditLogData* pDataStructure) {
			if (jsonObjectData.contains("webhooks") && !jsonObjectData["webhooks"].is_null()) {
				pDataStructure->webhooks.clear();
				pDataStructure->webhooks.reserve(jsonObjectData["webhooks"].size());
				for (auto& value: jsonObjectData["webhooks"]) {
					WebHookData newData{ value };
					pDataStructure->webhooks.push_back(newData);
				}
				pDataStructure->webhooks.shrink_to_fit();
			}

			if (jsonObjectData.contains("guild_scheduled_events") && !jsonObjectData["guild_scheduled_events"].is_null()) {
				pDataStructure->webhooks.clear();
				pDataStructure->webhooks.reserve(jsonObjectData["guild_scheduled_events"].size());
				for (auto& value: jsonObjectData["guild_scheduled_events"]) {
					GuildScheduledEventData newData{ value };
					pDataStructure->guildScheduledEvents.push_back(newData);
				}
				pDataStructure->webhooks.shrink_to_fit();
			}

			if (jsonObjectData.contains("auto_moderation_rules") && !jsonObjectData["auto_moderation_rules"].is_null()) {
				pDataStructure->users.clear();
				pDataStructure->users.reserve(jsonObjectData["auto_moderation_rules"].size());
				for (auto& value: jsonObjectData["auto_moderation_rules"]) {
					AutoModerationRuleData newData{ value };
					pDataStructure->autoModerationRules.push_back(newData);
				}
				pDataStructure->autoModerationRules.shrink_to_fit();
			}

			if (jsonObjectData.contains("users") && !jsonObjectData["users"].is_null()) {
				pDataStructure->users.clear();
				pDataStructure->users.reserve(jsonObjectData["users"].size());
				for (auto& value: jsonObjectData["users"]) {
					UserData newData{ value };
					pDataStructure->users.push_back(newData);
				}
				pDataStructure->users.shrink_to_fit();
			}

			if (jsonObjectData.contains("audit_log_entries") && !jsonObjectData["audit_log_entries"].is_null()) {
				pDataStructure->auditLogEntries.clear();
				pDataStructure->auditLogEntries.reserve(jsonObjectData["audit_log_entries"].size());
				for (auto& value: jsonObjectData["audit_log_entries"]) {
					AuditLogEntryData newData{ value };
					pDataStructure->auditLogEntries.push_back(newData);
				}
				pDataStructure->auditLogEntries.shrink_to_fit();
			}

			if (jsonObjectData.contains("integrations") && !jsonObjectData["integrations"].is_null()) {
				pDataStructure->integrations.clear();
				pDataStructure->integrations.reserve(jsonObjectData["integrations"].size());
				for (auto& value: jsonObjectData["integrations"]) {
					IntegrationData newData{ value };
					pDataStructure->integrations.push_back(newData);
				}
				pDataStructure->integrations.shrink_to_fit();
			}

			if (jsonObjectData.contains("threads") && !jsonObjectData["threads"].is_null()) {
				pDataStructure->threads.clear();
				pDataStructure->threads.reserve(jsonObjectData["threads"].size());
				for (auto& value: jsonObjectData["threads"]) {
					ChannelData newData{ value };
					pDataStructure->threads.push_back(newData);
				}
				pDataStructure->threads.shrink_to_fit();
			}
		}
	};

	/// For removing a reaction. \brief For removing a reaction.
	struct DiscordCoreAPI_Dll ReactionRemoveData : public DataParser<ReactionRemoveData> {
		Snowflake channelId{};
		Snowflake messageId{};
		Snowflake guildId{};
		Snowflake userId{};
		EmojiData emoji{};

		ReactionRemoveData() = default;

		ReactionRemoveData& operator=(const nlohmann::json& jsonObjectData) {
			this->parseObject(jsonObjectData, this);
			return *this;
		}

		ReactionRemoveData(const nlohmann::json& jsonObjectData) {
			*this = jsonObjectData;
		}

		virtual ~ReactionRemoveData() = default;

	  protected:
		void parseObject(const nlohmann::json& jsonObjectData, ReactionRemoveData* pDataStructure) {
			if (jsonObjectData.contains("user_id") && !jsonObjectData["user_id"].is_null()) {
				pDataStructure->userId = stoull(jsonObjectData["user_id"].get<std::string>());
			}

			if (jsonObjectData.contains("channel_id") && !jsonObjectData["channel_id"].is_null()) {
				pDataStructure->channelId = stoull(jsonObjectData["channel_id"].get<std::string>());
			}

			if (jsonObjectData.contains("message_id") && !jsonObjectData["message_id"].is_null()) {
				pDataStructure->messageId = stoull(jsonObjectData["message_id"].get<std::string>());
			}

			if (jsonObjectData.contains("guild_id") && !jsonObjectData["guild_id"].is_null()) {
				pDataStructure->guildId = stoull(jsonObjectData["guild_id"].get<std::string>());
			}

			if (jsonObjectData.contains("emoji") && !jsonObjectData["emoji"].is_null()) {
				pDataStructure->emoji = jsonObjectData["emoji"];
			}
		}
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

		ApplicationCommandOptionChoiceData& operator=(const nlohmann::json& jsonObjectData) {
			this->parseObject(jsonObjectData, this);
			return *this;
		}

		ApplicationCommandOptionChoiceData(const nlohmann::json& jsonObjectData) {
			*this = jsonObjectData;
		}

		virtual ~ApplicationCommandOptionChoiceData() = default;

	  protected:
		void parseObject(const nlohmann::json& jsonObjectData, ApplicationCommandOptionChoiceData* pDataStructure) {
			if (jsonObjectData.contains("value") && !jsonObjectData["value"].is_null() && jsonObjectData["value"].is_string()) {
				if (jsonObjectData["value"].is_string()) {
					pDataStructure->value = jsonObjectData["value"].get<std::string>();
				} else if (jsonObjectData["value"].is_number_float()) {
					pDataStructure->value = jsonObjectData["value"].get<float>();
				} else if (jsonObjectData["value"].is_number()) {
					pDataStructure->value = jsonObjectData["value"].get<int32_t>();
				}
			}

			if (jsonObjectData.contains("name") && !jsonObjectData["name"].is_null()) {
				pDataStructure->name = jsonObjectData["name"].get<std::string>();
			}

			if (jsonObjectData.contains("name_localizations") && !jsonObjectData["name_localizations"].is_null()) {
				pDataStructure->nameLocalizations.clear();
				auto newMap = jsonObjectData["name_localizations"].get<std::unordered_map<std::string, std::string>>();
				for (auto& [key, newValue]: newMap) {
					pDataStructure->nameLocalizations[key] = newValue;
				}
			}
		}
	};

	/// Data structure representing an ApplicationCommand's option. \brief Data structure representing an ApplicationCommand's option.
	struct DiscordCoreAPI_Dll ApplicationCommandOptionData : public DataParser<ApplicationCommandOptionData> {
		std::unordered_map<std::string, std::string> descriptionLocalizations{};///< Dictionary for the description localizations field.
		std::unordered_map<std::string, std::string> nameLocalizations{};///< Dictionary for the name localizations field.
		std::vector<ApplicationCommandOptionChoiceData> choices{};///< A std::vector of possible choices for the current ApplicationCommand option.
		std::vector<ApplicationCommandOptionData> options{};///< A std::vector of possible options for the current ApplicationCommand option.
		std::vector<ChannelType> channelTypes{};///< Set when the ApplicationCommand option type is set to Channel.
		ApplicationCommandOptionType type{};///< The type of command option.
		bool autocomplete{ false };///< If autocomplete interactions are enabled for this STRING, INTEGER, or NUMBER type option.
		std::string description{};///< A description of the current ApplicationCommand option.
		bool required{ false };///< If the parameter is required or optional-- default false.
		int32_t minValue{ 0 };///< If the option is an INTEGER or NUMBER type, the minimum value permitted.
		int32_t maxValue{ 0 };///< If the option is an INTEGER or NUMBER type, the maximum value permitted.
		std::string name{};///< Name of the current ApplicationCommand option.

		ApplicationCommandOptionData() = default;

		operator nlohmann::json() {
			nlohmann::json newOption{};
			if (this->type == DiscordCoreAPI::ApplicationCommandOptionType::Channel) {
				newOption["channel_types"] = this->channelTypes;
			}
			if (this->type != DiscordCoreAPI::ApplicationCommandOptionType::Sub_Command && this->type != DiscordCoreAPI::ApplicationCommandOptionType::Sub_Command_Group) {
				newOption["required"] = this->required;
			}
			newOption["description_localizations"] = this->descriptionLocalizations;
			newOption["name_localizations"] = this->nameLocalizations;
			newOption["description"] = this->description;
			newOption["min_value"] = this->minValue;
			newOption["max_value"] = this->maxValue;
			newOption["required"] = this->required;
			newOption["name"] = this->name;
			newOption["type"] = this->type;
			newOption["options"] = nlohmann::json{};
			newOption["choices"] = nlohmann::json{};
			if (this->choices.size() > 0) {
				for (int32_t x = 0; x < this->choices.size(); x++) {
					nlohmann::json jsonValue{};
					jsonValue["name_localizations"] = this->choices[x].nameLocalizations;
					jsonValue["value"] = this->choices[x].value;
					jsonValue["name"] = this->choices[x].name;
					newOption["choices"].push_back(jsonValue);
				}
			}
			if (newOption["choices"].size() == 0) {
				newOption["autocomplete"] = this->autocomplete;
			}
			if (this->options.size() > 0) {
				for (auto& value: options) {
					nlohmann::json theData = value;
				}
			}
			return newOption;
		}

		ApplicationCommandOptionData& operator=(const nlohmann::json& jsonObjectData) {
			this->parseObject(jsonObjectData, this);
			return *this;
		}

		ApplicationCommandOptionData(const nlohmann::json& jsonObjectData) {
			*this = jsonObjectData;
		}

		virtual ~ApplicationCommandOptionData() = default;

	  protected:
		void parseObject(const nlohmann::json& jsonObjectData, ApplicationCommandOptionData* pDataStructure) {
			if (jsonObjectData.contains("name") && !jsonObjectData["name"].is_null()) {
				pDataStructure->name = jsonObjectData["name"].get<std::string>();
			}

			if (jsonObjectData.contains("name_localizations") && !jsonObjectData["name_localizations"].is_null()) {
				pDataStructure->nameLocalizations.clear();
				auto newMap = jsonObjectData["name_localizations"].get<std::unordered_map<std::string, std::string>>();
				for (auto& [key, newValue]: newMap) {
					pDataStructure->nameLocalizations[key] = newValue;
				}
			}

			if (jsonObjectData.contains("description_localizations") && !jsonObjectData["description_localizations"].is_null()) {
				pDataStructure->nameLocalizations.clear();
				auto newMap = jsonObjectData["description_localizations"].get<std::unordered_map<std::string, std::string>>();
				for (auto& [key, newValue]: newMap) {
					pDataStructure->descriptionLocalizations[key] = newValue;
				}
			}

			if (jsonObjectData.contains("description") && !jsonObjectData["description"].is_null()) {
				pDataStructure->description = jsonObjectData["description"].get<std::string>();
			}

			if (jsonObjectData.contains("channel_types") && !jsonObjectData["channel_types"].is_null()) {
				pDataStructure->channelTypes.clear();
				pDataStructure->channelTypes.reserve(jsonObjectData["channel_types"].size());
				for (auto& value: jsonObjectData["channel_types"]) {
					pDataStructure->channelTypes.push_back(value);
				}
				pDataStructure->channelTypes.shrink_to_fit();
			}

			if (jsonObjectData.contains("min_value") && !jsonObjectData["min_value"].is_null()) {
				pDataStructure->minValue = jsonObjectData["min_value"].get<int32_t>();
			}

			if (jsonObjectData.contains("max_value") && !jsonObjectData["max_value"].is_null()) {
				pDataStructure->maxValue = jsonObjectData["max_value"].get<int32_t>();
			}

			if (jsonObjectData.contains("required") && !jsonObjectData["required"].is_null()) {
				pDataStructure->required = jsonObjectData["required"].get<bool>();
			}

			if (jsonObjectData.contains("autocomplete") && !jsonObjectData["autocomplete"].is_null()) {
				pDataStructure->autocomplete = jsonObjectData["autocomplete"].get<bool>();
			}

			if (jsonObjectData.contains("type") && !jsonObjectData["type"].is_null()) {
				pDataStructure->type = jsonObjectData["type"].get<ApplicationCommandOptionType>();
			}

			if (pDataStructure->type == ApplicationCommandOptionType::Sub_Command_Group || pDataStructure->type == ApplicationCommandOptionType::Sub_Command) {
				if (jsonObjectData.contains("options") && !jsonObjectData["options"].is_null()) {
					pDataStructure->options.reserve(jsonObjectData["options"].size());
					for (auto& value: jsonObjectData["options"]) {
						ApplicationCommandOptionData newData{ value };
						pDataStructure->options.push_back(newData);
					}
					pDataStructure->channelTypes.shrink_to_fit();
				}
			} else {
				if (jsonObjectData.contains("choices") && !jsonObjectData["choices"].is_null()) {
					pDataStructure->choices.clear();
					pDataStructure->choices.reserve(jsonObjectData["choices"].size());
					for (auto& value: jsonObjectData["choices"]) {
						ApplicationCommandOptionChoiceData newData{ value };
						pDataStructure->choices.push_back(newData);
					}
					pDataStructure->choices.shrink_to_fit();
				}
			}
		}
	};

	/// Representing "TypingStart" data. \brief Representing "TypingStart" data.
	struct DiscordCoreAPI_Dll TypingStartData : public DataParser<TypingStartData> {
		GuildMemberData member{};
		int32_t timestamp{ 0 };
		Snowflake channelId{};
		Snowflake guildId{};
		Snowflake userId{};

		TypingStartData() = default;

		TypingStartData& operator=(const nlohmann::json& jsonObjectData) {
			this->parseObject(jsonObjectData, this);
			return *this;
		}

		TypingStartData(const nlohmann::json& jsonObjectData) {
			*this = jsonObjectData;
		}

		virtual ~TypingStartData() = default;

	  protected:
		void parseObject(const nlohmann::json& jsonObjectData, TypingStartData* pDataStructure) {
			if (jsonObjectData.contains("channel_id") && !jsonObjectData["channel_id"].is_null()) {
				pDataStructure->channelId = stoull(jsonObjectData["channel_id"].get<std::string>());
			}

			if (jsonObjectData.contains("guild_id") && !jsonObjectData["guild_id"].is_null()) {
				pDataStructure->guildId = stoull(jsonObjectData["guild_id"].get<std::string>());
			}

			if (jsonObjectData.contains("member") && !jsonObjectData["member"].is_null()) {
				pDataStructure->member = jsonObjectData["member"];
			}

			if (jsonObjectData.contains("user_id") && !jsonObjectData["user_id"].is_null()) {
				pDataStructure->userId = stoull(jsonObjectData["user_id"].get<std::string>());
			}

			if (jsonObjectData.contains("timestamp") && !jsonObjectData["timestamp"].is_null()) {
				pDataStructure->timestamp = jsonObjectData["timestamp"].get<int32_t>();
			}
		}
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
		std::vector<YouTubeFormat> theFormats{};

		YouTubeFormatVector() = default;

		YouTubeFormatVector& operator=(const nlohmann::json& jsonObjectData) {
			this->parseObject(jsonObjectData, this);
			return *this;
		}

		YouTubeFormatVector(const nlohmann::json& jsonObjectData) {
			*this = jsonObjectData;
		}

		virtual ~YouTubeFormatVector() = default;

	  protected:
		void parseObject(const nlohmann::json& jsonObjectData, YouTubeFormatVector* pDataStructure) {
			if (!jsonObjectData.is_null()) {
				if (jsonObjectData.contains("streamingData") && !jsonObjectData["streamingData"].is_null() && jsonObjectData["streamingData"].contains("formats") &&
					!jsonObjectData["streamingData"]["formats"].is_null()) {
					for (auto& value: jsonObjectData["streamingData"]["formats"]) {
						YouTubeFormat newData{};

						if (value.contains("audioQuality") && !value["audioQuality"].is_null()) {
							newData.audioQuality = value["audioQuality"].get<std::string>();
						}

						if (value.contains("averageBitrate") && !value["averageBitrate"].is_null()) {
							newData.averageBitrate = value["averageBitrate"].get<int32_t>();
						}

						if (value.contains("audioSampleRate") && !value["audioSampleRate"].is_null()) {
							newData.audioSampleRate = value["audioSampleRate"].get<std::string>();
						}

						if (value.contains("bitrate") && !value["bitrate"].is_null()) {
							newData.bitrate = value["bitrate"].get<int32_t>();
						}

						if (value.contains("contentLength") && !value["contentLength"].is_null()) {
							newData.contentLength = stoll(value["contentLength"].get<std::string>());
						}

						if (value.contains("fps") && !value["fps"].is_null()) {
							newData.fps = value["fps"].get<int32_t>();
						}

						if (value.contains("height") && !value["height"].is_null()) {
							newData.height = value["height"].get<int32_t>();
						}

						if (value.contains("width") && !value["width"].is_null()) {
							newData.height = value["width"].get<int32_t>();
						}

						if (value.contains("aitags") && !value["aitags"].is_null()) {
							newData.aitags = value["aitags"].get<std::string>();
						}

						if (value.contains("itag") && !value["itag"].is_null()) {
							newData.itag = value["itag"].get<int32_t>();
						}

						if (value.contains("url") && !value["url"].is_null()) {
							newData.downloadUrl = value["url"].get<std::string>();
						}

						if (value.contains("mimeType") && !value["mimeType"].is_null()) {
							newData.mimeType = value["mimeType"].get<std::string>();
						}

						if (value.contains("quality") && !value["quality"].is_null()) {
							newData.quality = value["quality"].get<std::string>();
						}

						if (value.contains("signatureCipher") && !value["signatureCipher"].is_null()) {
							newData.signatureCipher = value["signatureCipher"].get<std::string>();
						} else if (value.contains("cipher") && !value["cipher"].is_null()) {
							newData.signatureCipher = value["cipher"].get<std::string>();
						}

						if (newData.signatureCipher.find("&sp") != std::string::npos) {
							newData.signature = newData.signatureCipher.substr(0, newData.signatureCipher.find("&sp"));
						}

						if (newData.signatureCipher.find("url") != std::string::npos) {
							newData.downloadUrl = newData.signatureCipher.substr(newData.signatureCipher.find("url") + 4);
						} else if (value.contains("url") && !value["url"].is_null()) {
							newData.downloadUrl = value["url"];
						}

						pDataStructure->theFormats.push_back(newData);
					}
				}

				if (jsonObjectData.contains("streamingData") && !jsonObjectData["streamingData"].is_null() && jsonObjectData["streamingData"].contains("adaptiveFormats") &&
					!jsonObjectData["streamingData"]["adaptiveFormats"].is_null()) {
					for (auto& value: jsonObjectData["streamingData"]["adaptiveFormats"]) {
						YouTubeFormat newData{};

						if (value.contains("audioQuality") && !value["audioQuality"].is_null()) {
							newData.audioQuality = value["audioQuality"].get<std::string>();
						}

						if (value.contains("averageBitrate") && !value["averageBitrate"].is_null()) {
							newData.averageBitrate = value["averageBitrate"].get<int32_t>();
						}

						if (value.contains("audioSampleRate") && !value["audioSampleRate"].is_null()) {
							newData.audioSampleRate = value["audioSampleRate"].get<std::string>();
						}

						if (value.contains("bitrate") && !value["bitrate"].is_null()) {
							newData.bitrate = value["bitrate"].get<int32_t>();
						}

						if (value.contains("contentLength") && !value["contentLength"].is_null()) {
							newData.contentLength = stoll(value["contentLength"].get<std::string>());
						}

						if (value.contains("fps") && !value["fps"].is_null()) {
							newData.fps = value["fps"].get<int32_t>();
						}

						if (value.contains("height") && !value["height"].is_null()) {
							newData.height = value["height"].get<int32_t>();
						}

						if (value.contains("width") && !value["width"].is_null()) {
							newData.height = value["width"].get<int32_t>();
						}

						if (value.contains("aitags") && !value["aitags"].is_null()) {
							newData.aitags = value["aitags"].get<std::string>();
						}

						if (value.contains("itag") && !value["itag"].is_null()) {
							newData.itag = value["itag"].get<int32_t>();
						}

						if (value.contains("mimeType") && !value["mimeType"].is_null()) {
							newData.mimeType = value["mimeType"].get<std::string>();
						}

						if (value.contains("quality") && !value["quality"].is_null()) {
							newData.quality = value["quality"].get<std::string>();
						}

						if (value.contains("signatureCipher") && !value["signatureCipher"].is_null()) {
							newData.signatureCipher = value["signatureCipher"].get<std::string>();
						} else if (value.contains("cipher") && !value["cipher"].is_null()) {
							newData.signatureCipher = value["cipher"].get<std::string>();
						}

						if (newData.signatureCipher.find("&sp") != std::string::npos) {
							newData.signature = newData.signatureCipher.substr(0, newData.signatureCipher.find("&sp"));
						}

						if (newData.signatureCipher.find("url") != std::string::npos) {
							newData.downloadUrl = newData.signatureCipher.substr(newData.signatureCipher.find("url") + 4);
						} else if (value.contains("url") && !value["url"].is_null()) {
							newData.downloadUrl = value["url"];
						}
						pDataStructure->theFormats.push_back(newData);
					}
				}
			}
		}
	};

	/// Application command types. \brief Application command types.
	enum class ApplicationCommandType : int8_t {
		Chat_Input = 1,///< Chat input.
		User = 2,///< User.
		Message = 3///< Message.
	};

	/// User command Interaction data. \brief User command Interaction data.
	struct DiscordCoreAPI_Dll UserCommandInteractionData : public DataParser<UserCommandInteractionData> {
		std::string targetId{};///< The target User's id.

		UserCommandInteractionData() = default;

		UserCommandInteractionData& operator=(const nlohmann::json& jsonObjectData) {
			this->parseObject(jsonObjectData, this);
			return *this;
		}

		UserCommandInteractionData(const nlohmann::json& jsonObjectData) {
			*this = jsonObjectData;
		}

		virtual ~UserCommandInteractionData() = default;

	  protected:
		void parseObject(const nlohmann::json& jsonObjectData, UserCommandInteractionData* pDataStructure) {
			if (jsonObjectData.contains("target_id") && !jsonObjectData["target_id"].is_null()) {
				pDataStructure->targetId = jsonObjectData["target_id"].get<std::string>();
			}
		}
	};

	/// Message command interacction data. \brief Message command interacction data.
	struct DiscordCoreAPI_Dll MessageCommandInteractionData : public DataParser<MessageCommandInteractionData> {
		std::string targetId{};///< The target Message's id.

		MessageCommandInteractionData() = default;

		MessageCommandInteractionData& operator=(const nlohmann::json& jsonObjectData) {
			this->parseObject(jsonObjectData, this);
			return *this;
		}

		MessageCommandInteractionData(const nlohmann::json& jsonObjectData) {
			*this = jsonObjectData;
		}

		virtual ~MessageCommandInteractionData() = default;

	  protected:
		void parseObject(const nlohmann::json& jsonObjectData, MessageCommandInteractionData* pDataStructure) {
			if (jsonObjectData.contains("target_id") && !jsonObjectData["target_id"].is_null()) {
				pDataStructure->targetId = jsonObjectData["target_id"].get<std::string>();
			}
		}
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

		ComponentInteractionData& operator=(const nlohmann::json& jsonObjectData) {
			this->parseObject(jsonObjectData, this);
			return *this;
		}

		ComponentInteractionData(const nlohmann::json& jsonObjectData) {
			*this = jsonObjectData;
		}

		virtual ~ComponentInteractionData() = default;

	  protected:
		void parseObject(const nlohmann::json& jsonObjectData, ComponentInteractionData* pDataStructure) {
			if (jsonObjectData.contains("values") && !jsonObjectData["values"].is_null()) {
				pDataStructure->values.clear();
				pDataStructure->values.reserve(jsonObjectData["values"].size());
				for (auto& value: jsonObjectData["values"]) {
					pDataStructure->values.push_back(value);
				}
				pDataStructure->values.shrink_to_fit();
			}

			if (jsonObjectData.contains("custom_id") && !jsonObjectData["custom_id"].is_null()) {
				pDataStructure->customId = jsonObjectData["custom_id"].get<std::string>();
			}

			if (jsonObjectData.contains("component_type") && !jsonObjectData["component_type"].is_null()) {
				pDataStructure->componentType = jsonObjectData["component_type"].get<ComponentType>();
			}
		}
	};

	/// Modal interaction data, for inputs from text modals. \brief Modal interaction data, for inputs from text modals.
	struct DiscordCoreAPI_Dll ModalInteractionData : public DataParser<ModalInteractionData> {
		std::string customIdSmall{};///< The custom id of a particular modal input.
		std::string customId{};///< The custom id of the Interaction entity.
		std::string value{};///< The input value of the modal.

		ModalInteractionData() = default;

		ModalInteractionData& operator=(const nlohmann::json& jsonObjectData) {
			this->parseObject(jsonObjectData, this);
			return *this;
		}

		ModalInteractionData(const nlohmann::json& jsonObjectData) {
			*this = jsonObjectData;
		}

		virtual ~ModalInteractionData() = default;

	  protected:
		void parseObject(const nlohmann::json& jsonObjectData, ModalInteractionData* pDataStructure) {
			if (jsonObjectData.contains("components") && !jsonObjectData["components"].is_null()) {
				pDataStructure->value = jsonObjectData["components"][0]["components"][0]["value"].get<std::string>();
			}

			if (jsonObjectData.contains("custom_id") && !jsonObjectData["custom_id"].is_null()) {
				pDataStructure->customId = jsonObjectData["custom_id"].get<std::string>();
			}

			if (jsonObjectData.contains("components") && !jsonObjectData["components"].is_null()) {
				pDataStructure->customIdSmall = jsonObjectData["components"][0]["components"][0]["custom_id"].get<std::string>();
			}
		}
	};

	/// Allowable mentions for a Message. \brief Allowable mentions for a Message.
	class DiscordCoreAPI_Dll AllowedMentionsData : public DataParser<AllowedMentionsData> {
	  public:
		std::vector<std::string> parse{};///< A std::vector of allowed mention types to parse from the content.
		std::vector<std::string> roles{};///< Array of role_ids to mention (Max size of 100)
		std::vector<std::string> users{};///< Array of user_ids to mention (Max size of 100)
		bool repliedUser{ false };///< For replies, whether to mention the author of the Message being replied to (default false).

		AllowedMentionsData() = default;

		operator nlohmann::json() {
			nlohmann::json newValue{};
			newValue["replied_user"] = this->repliedUser;
			newValue["parse"] = this->parse;
			newValue["roles"] = this->roles;
			newValue["users"] = this->users;
			return newValue;
		}

		AllowedMentionsData& operator=(const nlohmann::json& jsonObjectData) {
			this->parseObject(jsonObjectData, this);
			return *this;
		}

		AllowedMentionsData(const nlohmann::json& jsonObjectData) {
			*this = jsonObjectData;
		}

		virtual ~AllowedMentionsData() = default;

	  protected:
		void parseObject(const nlohmann::json& jsonObjectData, AllowedMentionsData* pDataStructure) {
			if (jsonObjectData.contains("parse") && !jsonObjectData["parse"].is_null()) {
				pDataStructure->parse.clear();
				pDataStructure->parse.reserve(jsonObjectData["parse"].size());
				for (auto& value: jsonObjectData["parse"]) {
					pDataStructure->parse.push_back(value.get<std::string>());
				}
				pDataStructure->parse.shrink_to_fit();
			}

			if (jsonObjectData.contains("roles") && !jsonObjectData["roles"].is_null()) {
				pDataStructure->roles.clear();
				pDataStructure->roles.reserve(jsonObjectData["roles"].size());
				for (auto& value: jsonObjectData["roles"]) {
					pDataStructure->roles.push_back(value.get<std::string>());
				}
				pDataStructure->roles.shrink_to_fit();
			}

			if (jsonObjectData.contains("users") && !jsonObjectData["users"].is_null()) {
				pDataStructure->users.clear();
				pDataStructure->users.reserve(jsonObjectData["users"].size());
				for (auto& value: jsonObjectData["users"]) {
					pDataStructure->users.push_back(value.get<std::string>());
				}
				pDataStructure->users.shrink_to_fit();
			}

			if (jsonObjectData.contains("replied_user") && !jsonObjectData["replied_user"].is_null()) {
				pDataStructure->repliedUser = jsonObjectData["replied_user"].get<bool>();
			}
		}
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

		SelectOptionData& operator=(const nlohmann::json& jsonObjectData) {
			this->parseObject(jsonObjectData, this);
			return *this;
		}

		SelectOptionData(const nlohmann::json& jsonObjectData) {
			*this = jsonObjectData;
		}

		virtual ~SelectOptionData() = default;

	  protected:
		void parseObject(const nlohmann::json& jsonObjectData, SelectOptionData* pDataStructure) {
			if (jsonObjectData.contains("label") && !jsonObjectData["label"].is_null()) {
				pDataStructure->label = jsonObjectData["label"].get<std::string>();
			}

			if (jsonObjectData.contains("value") && !jsonObjectData["value"].is_null()) {
				pDataStructure->value = jsonObjectData["value"].get<std::string>();
			}

			if (jsonObjectData.contains("description") && !jsonObjectData["description"].is_null()) {
				pDataStructure->description = jsonObjectData["description"].get<std::string>();
			}

			if (jsonObjectData.contains("emoji") && !jsonObjectData["emoji"].is_null()) {
				pDataStructure->emoji = jsonObjectData["emoji"];
			}

			if (jsonObjectData.contains("default") && !jsonObjectData["default"].is_null()) {
				pDataStructure->_default = jsonObjectData["default"].get<bool>();
			}
		}
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
		std::string url{};///< Url, for url types.
		EmojiData emoji{};///< Emoji name, id, and animated.
		int32_t style{};///< One of button styles.
	};

	/// Action row data of Message components. \brief Action row data of Message components.
	class DiscordCoreAPI_Dll ActionRowData : public DataParser<ActionRowData> {
	  public:
		std::vector<ComponentData> components{};///< Array of components to make up the action-row.

		ActionRowData() = default;

		ActionRowData& operator=(const nlohmann::json& jsonObjectData) {
			this->parseObject(jsonObjectData, this);
			return *this;
		}

		operator nlohmann::json() {
			nlohmann::json components{};
			for (auto& valueNew: this->components) {
				if (valueNew.type == ComponentType::Button) {
					nlohmann::json component{};
					component["emoji"]["animated"] = valueNew.emoji.animated;
					StringWrapper theString = valueNew.emoji.name;
					component["emoji"]["name"] = static_cast<std::string>(theString);
					if (valueNew.emoji.id != 0) {
						component["emoji"]["id"] = valueNew.emoji.id;
					}
					component["custom_id"] = valueNew.customId;
					component["disabled"] = valueNew.disabled;
					component["label"] = valueNew.label;
					component["style"] = valueNew.style;
					component["type"] = valueNew.type;
					component["url"] = valueNew.url;
					components.push_back(component);
				} else if (valueNew.type == ComponentType::SelectMenu) {
					nlohmann::json optionsArray{};
					for (auto& value01: valueNew.options) {
						nlohmann::json option{};
						if (value01.emoji.name != "") {
							option["emoji"]["name"] = static_cast<std::string>(value01.emoji.name);
							option["emoji"]["animated"] = value01.emoji.animated;
						}
						if (value01.emoji.id != 0) {
							option["emoji"]["id"] = value01.emoji.id;
						}
						option["description"] = value01.description;
						option["default"] = value01._default;
						option["label"] = value01.label;
						option["value"] = value01.value;
						optionsArray.push_back(option);
					};
					nlohmann::json component{};
					component["placeholder"] = valueNew.placeholder;
					component["max_values"] = valueNew.maxValues;
					component["min_values"] = valueNew.minValues;
					component["custom_id"] = valueNew.customId;
					component["disabled"] = valueNew.disabled;
					component["options"] = optionsArray;
					component["type"] = valueNew.type;
					components.push_back(component);

				} else if (valueNew.type == ComponentType::TextInput) {
					nlohmann::json component{};
					component["placeholder"] = valueNew.placeholder;
					component["min_length"] = valueNew.minLength;
					component["max_length"] = valueNew.maxLength;
					component["custom_id"] = valueNew.customId;
					component["required"] = valueNew.required;
					component["style"] = valueNew.style;
					component["label"] = valueNew.label;
					component["value"] = valueNew.value;
					component["type"] = valueNew.type;
					components.push_back(component);
				}
			}
			nlohmann::json theData{};
			theData["type"] = 1;
			theData["components"] = components;
			return theData;
		}

		ActionRowData(const nlohmann::json& jsonObjectData) {
			*this = jsonObjectData;
		}

		virtual ~ActionRowData() = default;

	  protected:
		void parseObject(const nlohmann::json& jsonObjectData, ActionRowData* pDataStructure) {
			if (jsonObjectData.contains("components") && !jsonObjectData["components"].is_null()) {
				pDataStructure->components.clear();
				pDataStructure->components.reserve(jsonObjectData["components"].size());
				for (auto& value: jsonObjectData["components"]) {
					ComponentData newData{};
					if (value.contains("type") && !value["type"].is_null()) {
						newData.type = value["type"].get<ComponentType>();
					}

					if (value.contains("custom_id") && !value["custom_id"].is_null()) {
						newData.customId = value["custom_id"].get<std::string>();
					}

					if (value.contains("placeholder") && !value["placeholder"].is_null()) {
						newData.placeholder = value["placeholder"].get<std::string>();
					}

					if (value.contains("disabled") && !value["disabled"].is_null()) {
						newData.disabled = value["disabled"].get<bool>();
					}

					if (value.contains("style") && !value["style"].is_null()) {
						newData.style = value["style"].get<int32_t>();
					}

					if (value.contains("label") && !value["label"].is_null()) {
						newData.label = value["label"].get<std::string>();
					}

					if (value.contains("min_length") && !value["min_length"].is_null()) {
						newData.minLength = value["min_length"].get<int32_t>();
					}

					if (value.contains("max_length") && !value["max_length"].is_null()) {
						newData.maxLength = value["max_length"].get<int32_t>();
					}

					if (value.contains("value") && !value["value"].is_null()) {
						newData.value = value["value"].get<std::string>();
					}

					if (value.contains("required") && !value["required"].is_null()) {
						newData.required = value["required"].get<bool>();
					}

					if (value.contains("emoji") && !value["emoji"].is_null()) {
						newData.emoji = value["emoji"];
					}

					if (value.contains("url") && !value["url"].is_null()) {
						newData.url = value["url"].get<std::string>();
					}

					if (value.contains("options") && !value["options"].is_null()) {
						newData.options.reserve(value["options"].size());
						for (auto& value02: value["options"]) {
							SelectOptionData newerData{ value };
							newData.options.push_back(newerData);
						}
						newData.options.shrink_to_fit();
					}

					if (value.contains("max_values") && !value["max_values"].is_null()) {
						newData.maxValues = value["max_values"].get<int32_t>();
					}

					if (value.contains("min_values") && !value["min_values"].is_null()) {
						newData.minValues = value["min_values"].get<int32_t>();
					}

					pDataStructure->components.push_back(newData);
				}
			}
			pDataStructure->components.shrink_to_fit();
		}
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
		std::vector<ApplicationCommandOptionData> options{};///< A std::vector of possible options for the current ApplicationCommand.
		std::string defaultMemberPermissions{};///< Set of permissions represented as a bit set all
		ApplicationCommandType type{};///< The type of ApplicationCommand.
		bool dmPermission{ false };///< Indicates whether the command is available in DMs with the app, only for globally - scoped commands.
		std::string description{};///< A description of the current ApplicationCommand.
		Snowflake applicationId{};///< The current application id.
		std::string version{};///< An autoincremented version.
		std::string name{};///< Name of the current ApplicationCommand.
		Snowflake guildId{};///< (Where applicable) a Guild id for which guild to assign this ApplicationCommand to.

		ApplicationCommandData() = default;

		ApplicationCommandData& operator=(const nlohmann::json& jsonObjectData) {
			this->parseObject(jsonObjectData, this);
			return *this;
		}

		ApplicationCommandData(const nlohmann::json& jsonObjectData) {
			*this = jsonObjectData;
		}

		virtual ~ApplicationCommandData() = default;

	  protected:
		void parseObject(const nlohmann::json& jsonObjectData, ApplicationCommandData* pDataStructure) {
			if (jsonObjectData.contains("id") && !jsonObjectData["id"].is_null()) {
				pDataStructure->id = stoull(jsonObjectData["id"].get<std::string>());
			}

			if (jsonObjectData.contains("default_member_permissions") && !jsonObjectData["default_member_permissions"].is_null()) {
				pDataStructure->defaultMemberPermissions = jsonObjectData["default_member_permissions"].get<std::string>();
			}

			if (jsonObjectData.contains("dm_permission") && !jsonObjectData["dm_permission"].is_null()) {
				pDataStructure->dmPermission = jsonObjectData["dm_permission"].get<bool>();
			}

			if (jsonObjectData.contains("version") && !jsonObjectData["version"].is_null()) {
				pDataStructure->version = jsonObjectData["version"].get<std::string>();
			}

			if (jsonObjectData.contains("type") && !jsonObjectData["type"].is_null()) {
				pDataStructure->type = jsonObjectData["type"].get<ApplicationCommandType>();
			}

			if (jsonObjectData.contains("name_localizations") && !jsonObjectData["name_localizations"].is_null()) {
				pDataStructure->nameLocalizations.clear();
				auto newMap = jsonObjectData["name_localizations"].get<std::unordered_map<std::string, std::string>>();
				for (auto& [key, newValue]: newMap) {
					pDataStructure->nameLocalizations[key] = newValue;
				}
			}

			if (jsonObjectData.contains("description_localizations") && !jsonObjectData["description_localizations"].is_null()) {
				pDataStructure->nameLocalizations.clear();
				auto newMap = jsonObjectData["description_localizations"].get<std::unordered_map<std::string, std::string>>();
				for (auto& [key, newValue]: newMap) {
					pDataStructure->descriptionLocalizations[key] = newValue;
				}
			}

			if (jsonObjectData.contains("application_id") && !jsonObjectData["application_id"].is_null()) {
				pDataStructure->applicationId = stoull(jsonObjectData["application_id"].get<std::string>());
			}

			if (jsonObjectData.contains("name") && !jsonObjectData["name"].is_null()) {
				pDataStructure->name = jsonObjectData["name"].get<std::string>();
			}

			if (jsonObjectData.contains("description") && !jsonObjectData["description"].is_null()) {
				pDataStructure->description = jsonObjectData["description"].get<std::string>();
			}

			if (jsonObjectData.contains("version") && !jsonObjectData["version"].is_null()) {
				pDataStructure->version = jsonObjectData["version"].get<std::string>();
			}

			if (jsonObjectData.contains("options") && !jsonObjectData["options"].is_null()) {
				pDataStructure->options.reserve(jsonObjectData["options"].size());
				for (auto& value: jsonObjectData["options"]) {
					ApplicationCommandOptionData newData{ value };
					pDataStructure->options.push_back(newData);
				}
				pDataStructure->options.shrink_to_fit();
			}
		}
	};

	/// Channel mention data. \brief Channel mention data.
	class DiscordCoreAPI_Dll ChannelMentionData : public DiscordEntity, public DataParser<ChannelMentionData> {
	  public:
		Snowflake guildId{};///< The id of the Guild where it took place.
		std::string name{};///< The name of the Channel that was mentioned.
		ChannelType type{};///< The type of Channel that was mentioned.

		ChannelMentionData() = default;

		ChannelMentionData& operator=(const nlohmann::json& jsonObjectData) {
			this->parseObject(jsonObjectData, this);
			return *this;
		}

		ChannelMentionData(const nlohmann::json& jsonObjectData) {
			*this = jsonObjectData;
		}

		virtual ~ChannelMentionData() = default;

	  protected:
		void parseObject(const nlohmann::json& jsonObjectData, ChannelMentionData* pDataStructure) {
			if (jsonObjectData.contains("id") && !jsonObjectData["id"].is_null()) {
				pDataStructure->id = stoull(jsonObjectData["id"].get<std::string>());
			}

			if (jsonObjectData.contains("guild_id") && !jsonObjectData["guild_id"].is_null()) {
				pDataStructure->guildId = stoull(jsonObjectData["guild_id"].get<std::string>());
			}

			if (jsonObjectData.contains("type") && !jsonObjectData["type"].is_null()) {
				pDataStructure->type = jsonObjectData["type"].get<ChannelType>();
			}

			if (jsonObjectData.contains("name") && !jsonObjectData["name"].is_null()) {
				pDataStructure->name = jsonObjectData["name"].get<std::string>();
			}
		}
	};

	/// Data for when some Channel pins are updated. \brief Data for when some Channel pins are updated.
	struct DiscordCoreAPI_Dll ChannelPinsUpdateEventData : public DataParser<ChannelPinsUpdateEventData> {
		TimeStamp lastPinTimestamp{ "" };///< The time of the last pinned Message.
		Snowflake channelId{};///< The id of the Channel within which the Message was pinned.
		Snowflake guildId{};///< The id of the Guild within which the Message was pinned.

		ChannelPinsUpdateEventData() = default;

		ChannelPinsUpdateEventData& operator=(const nlohmann::json& jsonObjectData) {
			this->parseObject(jsonObjectData, this);
			return *this;
		}

		ChannelPinsUpdateEventData(const nlohmann::json& jsonObjectData) {
			*this = jsonObjectData;
		}

		virtual ~ChannelPinsUpdateEventData() = default;

	  protected:
		void parseObject(const nlohmann::json& jsonObjectData, ChannelPinsUpdateEventData* pDataStructre) {
			if (jsonObjectData.contains("guild_id") && !jsonObjectData["guild_id"].is_null()) {
				pDataStructre->guildId = stoull(jsonObjectData["guild_id"].get<std::string>());
			}

			if (jsonObjectData.contains("channel_id") && !jsonObjectData["channel_id"].is_null()) {
				pDataStructre->channelId = stoull(jsonObjectData["channel_id"].get<std::string>());
			}

			if (jsonObjectData.contains("last_pin_timestamp") && !jsonObjectData["last_pin_timestamp"].is_null()) {
				pDataStructre->lastPinTimestamp = jsonObjectData["last_pin_timestamp"].get<std::string>();
			}
		}
	};

	/// Data for when threads are synced. \brief Data for when threads are synced.
	struct DiscordCoreAPI_Dll ThreadListSyncData : public DataParser<ThreadListSyncData> {
		std::vector<ThreadMemberData> members{};///< Array of members that are a part of the Thread.
		std::vector<std::string> channelIds{};///< The parent Channel ids whose threads are being synced. If omitted, then threads were synced for entire Guild.
		std::vector<ChannelData> threads{};///< All active threads in the given channels that the current User can access.
		Snowflake guildId{};///< The id of the Guild for which the threads are being synced.

		ThreadListSyncData() = default;

		ThreadListSyncData& operator=(const nlohmann::json& jsonObjectData) {
			this->parseObject(jsonObjectData, this);
			return *this;
		}

		ThreadListSyncData(const nlohmann::json& jsonObjectData) {
			*this = jsonObjectData;
		}

		virtual ~ThreadListSyncData() = default;

	  protected:
		void parseObject(const nlohmann::json& jsonObjectData, ThreadListSyncData* pDataStructure) {
			if (jsonObjectData.contains("guild_id") && !jsonObjectData["guild_id"].is_null()) {
				pDataStructure->guildId = stoull(jsonObjectData["guild_id"].get<std::string>());
			}

			if (jsonObjectData.contains("channel_ids") && !jsonObjectData["channel_ids"].is_null()) {
				pDataStructure->channelIds.clear();
				pDataStructure->channelIds.reserve(jsonObjectData["channel_ids"].size());
				for (auto& value: jsonObjectData["channel_ids"]) {
					pDataStructure->channelIds.push_back(value.get<std::string>());
				}
				pDataStructure->channelIds.shrink_to_fit();
			}

			if (jsonObjectData.contains("members") && !jsonObjectData["members"].is_null()) {
				pDataStructure->members.clear();
				pDataStructure->members.reserve(jsonObjectData["members"].size());
				for (auto& value: jsonObjectData["members"]) {
					ThreadMemberData newData{ value };
					pDataStructure->members.push_back(newData);
				}
				pDataStructure->members.shrink_to_fit();
			}

			if (jsonObjectData.contains("threads") && !jsonObjectData["threads"].is_null()) {
				pDataStructure->threads.clear();
				pDataStructure->threads.reserve(jsonObjectData["threads"].size());
				for (auto& value: jsonObjectData["threads"]) {
					ChannelData newData{ value };
					pDataStructure->threads.push_back(newData);
				}
				pDataStructure->threads.shrink_to_fit();
			}
		}
	};

	/// Represents a Thread-members-update. \brief Represents a Thread-members-update.
	class DiscordCoreAPI_Dll ThreadMembersUpdateData : public DiscordEntity, public DataParser<ThreadMembersUpdateData> {
	  public:
		std::vector<ThreadMemberData> addedMembers{};///< New members added to the Thread.
		std::vector<std::string> removedMemberIds{};///< Members who have been removed.
		int32_t memberCount{ 0 };///< Number of Guild-members in the Thread.
		Snowflake guildId{};///< Guild id of the Thread.

		ThreadMembersUpdateData() = default;

		ThreadMembersUpdateData& operator=(const nlohmann::json& jsonObjectData) {
			this->parseObject(jsonObjectData, this);
			return *this;
		}

		ThreadMembersUpdateData(const nlohmann::json& jsonObjectData) {
			*this = jsonObjectData;
		}

		virtual ~ThreadMembersUpdateData() = default;

	  protected:
		void parseObject(const nlohmann::json& jsonObjectData, ThreadMembersUpdateData* pDataStructure) {
			if (jsonObjectData.contains("guild_id") && !jsonObjectData["guild_id"].is_null()) {
				pDataStructure->guildId = stoull(jsonObjectData["guild_id"].get<std::string>());
			}

			if (jsonObjectData.contains("id") && !jsonObjectData["id"].is_null()) {
				pDataStructure->id = stoull(jsonObjectData["id"].get<std::string>());
			}

			if (jsonObjectData.contains("member_count") && !jsonObjectData["member_count"].is_null()) {
				pDataStructure->memberCount = jsonObjectData["member_count"].get<int32_t>();
			}

			if (jsonObjectData.contains("added_members") && !jsonObjectData["added_members"].is_null()) {
				pDataStructure->addedMembers.clear();
				pDataStructure->addedMembers.reserve(jsonObjectData["added_members"].size());
				for (auto& value: jsonObjectData["added_members"]) {
					ThreadMemberData newData{ value };
					pDataStructure->addedMembers.push_back(newData);
				}
				pDataStructure->addedMembers.shrink_to_fit();
			}

			if (jsonObjectData.contains("removed_member_ids") && !jsonObjectData["removed_member_ids"].is_null()) {
				pDataStructure->removedMemberIds.clear();
				pDataStructure->removedMemberIds.reserve(jsonObjectData["removed_member_ids"].size());
				for (auto& value: jsonObjectData["removed_member_ids"]) {
					pDataStructure->removedMemberIds.push_back(value);
				}
				pDataStructure->removedMemberIds.shrink_to_fit();
			}
		}
	};

	/// Message Interaction data. \brief Message Interaction data.
	class DiscordCoreAPI_Dll MessageInteractionData : public DiscordEntity, public DataParser<MessageInteractionData> {
	  public:
		GuildMemberData member{};
		InteractionType type{};
		std::string name{};
		UserData user{};

		MessageInteractionData() = default;

		MessageInteractionData& operator=(const nlohmann::json& jsonObjectData) {
			this->parseObject(jsonObjectData, this);
			return *this;
		}

		MessageInteractionData(const nlohmann::json& jsonObjectData) {
			*this = jsonObjectData;
		}

		virtual ~MessageInteractionData() = default;

	  protected:
		void parseObject(const nlohmann::json& jsonObjectData, MessageInteractionData* pDataStructure) {
			if (jsonObjectData.contains("id") && !jsonObjectData["id"].is_null()) {
				pDataStructure->id = stoull(jsonObjectData["id"].get<std::string>());
			}

			if (jsonObjectData.contains("type") && !jsonObjectData["type"].is_null()) {
				pDataStructure->type = jsonObjectData["type"].get<InteractionType>();
			}

			if (jsonObjectData.contains("name") && !jsonObjectData["name"].is_null()) {
				pDataStructure->name = jsonObjectData["name"].get<std::string>();
			}

			if (jsonObjectData.contains("user") && !jsonObjectData["user"].is_null()) {
				pDataStructure->user = jsonObjectData["user"];
			}

			if (jsonObjectData.contains("member") && !jsonObjectData["member"].is_null()) {
				pDataStructure->member = jsonObjectData["member"];
			}
		}
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

		StickerItemData& operator=(const nlohmann::json& jsonObjectData) {
			this->parseObject(jsonObjectData, this);
			return *this;
		}

		StickerItemData(const nlohmann::json& jsonObjectData) {
			*this = jsonObjectData;
		}

		virtual ~StickerItemData() = default;

	  protected:
		void parseObject(const nlohmann::json& jsonObjectData, StickerItemData* pDataStructure) {
			if (jsonObjectData.contains("id") && !jsonObjectData["id"].is_null()) {
				pDataStructure->id = stoull(jsonObjectData["id"].get<std::string>());
			}

			if (jsonObjectData.contains("name") && !jsonObjectData["name"].is_null()) {
				pDataStructure->name = jsonObjectData["name"].get<std::string>();
			}

			if (jsonObjectData.contains("format_type") && !jsonObjectData["format_type"].is_null()) {
				pDataStructure->formatType = jsonObjectData["format_type"].get<StickerItemType>();
			}
		}
	};

	/// The core of a Message's data structure. \brief The core of a Message's data structure.
	class DiscordCoreAPI_Dll MessageDataOld : public DiscordEntity, public DataParser<MessageDataOld> {
	  public:
		std::vector<ChannelMentionData> mentionChannels{};///< array of Channel mention data.
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
		TimeStamp editedTimestamp{ "" };///< The time at which it was edited.
		MessageActivityData activity{};///< Message activity data.
		bool mentionEveryone{ false };///< Does the Message mention everyone?
		ApplicationData application{};///< Application data.
		TimeStamp timestamp{ "" };///< The timestamp of when the Message was created.
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

		MessageDataOld& operator=(const nlohmann::json& jsonObjectData) {
			this->parseObject(jsonObjectData, this);
			return *this;
		}

		MessageDataOld(const nlohmann::json& jsonObjectData) {
			*this = jsonObjectData;
		}

		virtual ~MessageDataOld() = default;

	  protected:
		void parseObject(const nlohmann::json& jsonObjectData, MessageDataOld* pDataStructure) {
			if (jsonObjectData.contains("content") && !jsonObjectData["content"].is_null()) {
				pDataStructure->content = jsonObjectData["content"].get<std::string>();
			}

			if (jsonObjectData.contains("id") && !jsonObjectData["id"].is_null()) {
				pDataStructure->id = stoull(jsonObjectData["id"].get<std::string>());
			}

			if (jsonObjectData.contains("channel_id") && !jsonObjectData["channel_id"].is_null()) {
				pDataStructure->channelId = stoull(jsonObjectData["channel_id"].get<std::string>());
			}

			if (jsonObjectData.contains("guild_id") && !jsonObjectData["guild_id"].is_null()) {
				pDataStructure->guildId = stoull(jsonObjectData["guild_id"].get<std::string>());
			}

			if (jsonObjectData.contains("author") && !jsonObjectData["author"].is_null()) {
				pDataStructure->author = jsonObjectData["author"];
			}

			if (jsonObjectData.contains("member") && !jsonObjectData["member"].is_null()) {
				pDataStructure->member = jsonObjectData["member"];
			}

			if (jsonObjectData.contains("timestamp") && !jsonObjectData["timestamp"].is_null()) {
				pDataStructure->timestamp = jsonObjectData["timestamp"].get<std::string>();
			}

			if (jsonObjectData.contains("edited_timestamp") && !jsonObjectData["edited_timestamp"].is_null()) {
				pDataStructure->editedTimestamp = jsonObjectData["edited_timestamp"].get<std::string>();


				if (jsonObjectData.contains("tts") && !jsonObjectData["tts"].is_null()) {
					pDataStructure->tts = jsonObjectData["tts"].get<bool>();
				}

				if (jsonObjectData.contains("mention_everyone") && !jsonObjectData["mention_everyone"].is_null()) {
					pDataStructure->mentionEveryone = jsonObjectData["mention_everyone"].get<bool>();
				}

				if (jsonObjectData.contains("mentions") && !jsonObjectData["mentions"].is_null()) {
					pDataStructure->mentions.clear();
					pDataStructure->mentions.reserve(jsonObjectData["mentions"].size());
					for (auto& value: jsonObjectData["mentions"]) {
						UserData newData{ value };
						pDataStructure->mentions.push_back(newData);
					}
					pDataStructure->mentions.shrink_to_fit();
				}

				if (jsonObjectData.contains("mention_roles") && !jsonObjectData["mention_roles"].is_null()) {
					pDataStructure->mentionRoles.clear();
					pDataStructure->mentionRoles.reserve(jsonObjectData["mention_roles"].size());
					for (auto& value: jsonObjectData["mention_roles"]) {
						pDataStructure->mentionRoles.push_back(value.get<std::string>());
					}
					pDataStructure->mentionRoles.shrink_to_fit();
				}

				if (jsonObjectData.contains("mention_channels") && !jsonObjectData["mention_channels"].is_null()) {
					pDataStructure->mentionChannels.clear();
					pDataStructure->mentionChannels.reserve(jsonObjectData["mention_channels"].size());
					for (auto& value: jsonObjectData["mention_channels"]) {
						ChannelMentionData newData{ value };
						pDataStructure->mentionChannels.push_back(newData);
					}
					pDataStructure->mentionChannels.shrink_to_fit();
				}

				if (jsonObjectData.contains("attachments") && !jsonObjectData["attachments"].is_null()) {
					pDataStructure->attachments.clear();
					pDataStructure->attachments.reserve(jsonObjectData["attachments"].size());
					for (auto& value: jsonObjectData["attachments"]) {
						AttachmentData newData{ value };
						pDataStructure->attachments.push_back(newData);
					}
					pDataStructure->attachments.shrink_to_fit();
				}

				if (jsonObjectData.contains("embeds") && !jsonObjectData["embeds"].is_null()) {
					pDataStructure->embeds.clear();
					pDataStructure->embeds.reserve(jsonObjectData["embeds"].size());
					for (auto& value: jsonObjectData["embeds"]) {
						EmbedData newData{ value };
						pDataStructure->embeds.push_back(newData);
					}
					pDataStructure->embeds.shrink_to_fit();
				}

				if (jsonObjectData.contains("reactions") && !jsonObjectData["reactions"].is_null()) {
					pDataStructure->reactions.clear();
					pDataStructure->reactions.reserve(jsonObjectData["reactions"].size());
					for (auto& value: jsonObjectData["reactions"]) {
						ReactionData newData{ value };
						pDataStructure->reactions.push_back(newData);
					}
					pDataStructure->reactions.shrink_to_fit();
				}

				if (jsonObjectData.contains("nonce") && !jsonObjectData["nonce"].is_null()) {
					pDataStructure->nonce = jsonObjectData["nonce"].get<std::string>();
				}

				if (jsonObjectData.contains("pinned") && !jsonObjectData["pinned"].is_null()) {
					pDataStructure->pinned = jsonObjectData["pinned"].get<bool>();
				}

				if (jsonObjectData.contains("webhook_id") && !jsonObjectData["webhook_id"].is_null()) {
					pDataStructure->webHookId = stoull(jsonObjectData["webhook_id"].get<std::string>());
				}

				if (jsonObjectData.contains("type") && !jsonObjectData["type"].is_null()) {
					pDataStructure->type = jsonObjectData["type"].get<MessageType>();
				}

				if (jsonObjectData.contains("activity") && !jsonObjectData["activity"].is_null()) {
					pDataStructure->activity = jsonObjectData["activity"];
				}

				if (jsonObjectData.contains("application") && !jsonObjectData["application"].is_null()) {
					pDataStructure->application = jsonObjectData["application"];
				}

				if (jsonObjectData.contains("application_id") && !jsonObjectData["application_id"].is_null()) {
					pDataStructure->applicationId = stoull(jsonObjectData["application_id"].get<std::string>());
				}

				if (jsonObjectData.contains("message_reference") && !jsonObjectData["message_reference"].is_null()) {
					pDataStructure->messageReference = jsonObjectData["message_reference"];
				}

				if (jsonObjectData.contains("flags") && !jsonObjectData["flags"].is_null()) {
					pDataStructure->flags = jsonObjectData["flags"].get<int32_t>();
				}

				if (jsonObjectData.contains("sticker_items") && !jsonObjectData["sticker_items"].is_null()) {
					pDataStructure->stickerItems.clear();
					pDataStructure->stickerItems.reserve(jsonObjectData["sticker_items"].size());
					for (auto& value: jsonObjectData["sticker_items"]) {
						StickerItemData newData{ value };
						pDataStructure->stickerItems.push_back(newData);
					}
					pDataStructure->stickerItems.shrink_to_fit();
				}

				if (jsonObjectData.contains("stickers") && !jsonObjectData["stickers"].is_null()) {
					pDataStructure->stickers.clear();
					pDataStructure->stickers.reserve(jsonObjectData["stickers"].size());
					for (auto& value: jsonObjectData["stickers"]) {
						StickerData newData{ value };
						pDataStructure->stickers.push_back(newData);
					}
					pDataStructure->stickers.shrink_to_fit();
				}

				if (jsonObjectData.contains("interaction") && !jsonObjectData["interaction"].is_null()) {
					pDataStructure->interaction = jsonObjectData["interaction"];
				}

				if (jsonObjectData.contains("components") && !jsonObjectData["components"].is_null()) {
					pDataStructure->components.clear();
					pDataStructure->components.reserve(jsonObjectData["components"].size());
					for (auto& value: jsonObjectData["components"]) {
						ActionRowData newData{ value };
						pDataStructure->components.push_back(newData);
					}
					pDataStructure->components.shrink_to_fit();
				}

				if (jsonObjectData.contains("thread") && !jsonObjectData["thread"].is_null()) {
					pDataStructure->thread = jsonObjectData["thread"];
				}
			}
		}
	};

	/// Data structure representing a single Message. \brief Data structure representing a single Message.
	class DiscordCoreAPI_Dll MessageData : public MessageDataOld, public DataParser<MessageData> {
	  public:
		std::unique_ptr<MessageDataOld> referencedMessage{ std::make_unique<MessageDataOld>() };///< The referenced Message, to reply to.

		MessageData& operator=(const MessageData& other) {
			if (this != &other) {
				*this->referencedMessage = *other.referencedMessage;
				this->messageReference = other.messageReference;
				this->mentionEveryone = other.mentionEveryone;
				this->mentionChannels = other.mentionChannels;
				this->editedTimestamp = other.editedTimestamp;
				this->applicationId = other.applicationId;
				this->stickerItems = other.stickerItems;
				this->mentionRoles = other.mentionRoles;
				this->application = other.application;
				this->interaction = other.interaction;
				this->attachments = other.attachments;
				this->components = other.components;
				this->timestamp = other.timestamp;
				this->channelId = other.channelId;
				this->webHookId = other.webHookId;
				this->reactions = other.reactions;
				this->activity = other.activity;
				this->mentions = other.mentions;
				this->stickers = other.stickers;
				this->content = other.content;
				this->guildId = other.guildId;
				this->member = other.member;
				this->thread = other.thread;
				this->pinned = other.pinned;
				this->author = other.author;
				this->embeds = other.embeds;
				this->nonce = other.nonce;
				this->flags = other.flags;
				this->type = other.type;
				this->tts = other.tts;
				this->id = other.id;
			}
			return *this;
		}

		MessageData(const MessageData& other) {
			*this = other;
		}

		MessageData() = default;

		MessageData& operator=(const nlohmann::json& jsonObjectData) {
			this->parseObject(jsonObjectData, this);
			return *this;
		}

		MessageData(const nlohmann::json& jsonObjectData) {
			*this = jsonObjectData;
		}

		virtual ~MessageData() = default;

	  protected:
		void parseObject(const nlohmann::json& jsonObjectData, MessageData* pDataStructure) {
			if (jsonObjectData.contains("content") && !jsonObjectData["content"].is_null()) {
				pDataStructure->content = jsonObjectData["content"].get<std::string>();
			}

			if (jsonObjectData.contains("id") && !jsonObjectData["id"].is_null()) {
				pDataStructure->id = stoull(jsonObjectData["id"].get<std::string>());
			}

			if (jsonObjectData.contains("channel_id") && !jsonObjectData["channel_id"].is_null()) {
				pDataStructure->channelId = stoull(jsonObjectData["channel_id"].get<std::string>());
			}

			if (jsonObjectData.contains("guild_id") && !jsonObjectData["guild_id"].is_null()) {
				pDataStructure->guildId = stoull(jsonObjectData["guild_id"].get<std::string>());
			}

			if (jsonObjectData.contains("author") && !jsonObjectData["author"].is_null()) {
				pDataStructure->author = jsonObjectData["author"];
			}

			if (jsonObjectData.contains("member") && !jsonObjectData["member"].is_null()) {
				pDataStructure->member = jsonObjectData["member"];
			}

			if (jsonObjectData.contains("timestamp") && !jsonObjectData["timestamp"].is_null()) {
				pDataStructure->timestamp = jsonObjectData["timestamp"].get<std::string>();
			}

			if (jsonObjectData.contains("edited_timestamp") && !jsonObjectData["edited_timestamp"].is_null()) {
				pDataStructure->editedTimestamp = jsonObjectData["edited_timestamp"].get<std::string>();
			}

			if (jsonObjectData.contains("tts") && !jsonObjectData["tts"].is_null()) {
				pDataStructure->tts = jsonObjectData["tts"].get<bool>();
			}

			if (jsonObjectData.contains("mention_everyone") && !jsonObjectData["mention_everyone"].is_null()) {
				pDataStructure->mentionEveryone = jsonObjectData["mention_everyone"].get<bool>();
			}

			if (jsonObjectData.contains("mentions") && !jsonObjectData["mentions"].is_null()) {
				pDataStructure->mentions.clear();
				pDataStructure->mentions.reserve(jsonObjectData["mentions"].size());
				for (auto& value: jsonObjectData["mentions"]) {
					UserData newData{ value };
					pDataStructure->mentions.push_back(newData);
				}
				pDataStructure->mentions.shrink_to_fit();
			}

			if (jsonObjectData.contains("mention_roles") && !jsonObjectData["mention_roles"].is_null()) {
				pDataStructure->mentionRoles.clear();
				pDataStructure->mentionRoles.reserve(jsonObjectData["mention_roles"].size());
				for (auto& value: jsonObjectData["mention_roles"]) {
					pDataStructure->mentionRoles.push_back(value.get<std::string>());
				}
				pDataStructure->mentionRoles.shrink_to_fit();
			}

			if (jsonObjectData.contains("mention_channels") && !jsonObjectData["mention_channels"].is_null()) {
				pDataStructure->mentionChannels.clear();
				pDataStructure->mentionChannels.reserve(jsonObjectData["mention_channels"].size());
				for (auto& value: jsonObjectData["mention_channels"]) {
					ChannelMentionData newData{ value };
					pDataStructure->mentionChannels.push_back(newData);
				}
				pDataStructure->mentionChannels.shrink_to_fit();
			}

			if (jsonObjectData.contains("attachments") && !jsonObjectData["attachments"].is_null()) {
				pDataStructure->attachments.clear();
				pDataStructure->attachments.reserve(jsonObjectData["attachments"].size());
				for (auto& value: jsonObjectData["attachments"]) {
					AttachmentData newData{ value };
					pDataStructure->attachments.push_back(newData);
				}
				pDataStructure->attachments.shrink_to_fit();
			}

			if (jsonObjectData.contains("embeds") && !jsonObjectData["embeds"].is_null()) {
				pDataStructure->embeds.clear();
				pDataStructure->embeds.reserve(jsonObjectData["embeds"].size());
				for (auto& value: jsonObjectData["embeds"]) {
					EmbedData newData{ value };
					pDataStructure->embeds.push_back(newData);
				}
				pDataStructure->embeds.shrink_to_fit();
			}

			if (jsonObjectData.contains("reactions") && !jsonObjectData["reactions"].is_null()) {
				pDataStructure->reactions.clear();
				pDataStructure->reactions.reserve(jsonObjectData["reactions"].size());
				for (auto& value: jsonObjectData["reactions"]) {
					ReactionData newData{ value };
					pDataStructure->reactions.push_back(newData);
				}
				pDataStructure->reactions.shrink_to_fit();
			}

			if (jsonObjectData.contains("nonce") && !jsonObjectData["nonce"].is_null()) {
				pDataStructure->nonce = jsonObjectData["nonce"].get<std::string>();
			}

			if (jsonObjectData.contains("pinned") && !jsonObjectData["pinned"].is_null()) {
				pDataStructure->pinned = jsonObjectData["pinned"].get<bool>();
			}

			if (jsonObjectData.contains("webhook_id") && !jsonObjectData["webhook_id"].is_null()) {
				pDataStructure->webHookId = stoull(jsonObjectData["webhook_id"].get<std::string>());
			}

			if (jsonObjectData.contains("type") && !jsonObjectData["type"].is_null()) {
				pDataStructure->type = jsonObjectData["type"].get<MessageType>();
			}

			if (jsonObjectData.contains("activity") && !jsonObjectData["activity"].is_null()) {
				pDataStructure->activity = jsonObjectData["activity"];
			}

			if (jsonObjectData.contains("application") && !jsonObjectData["application"].is_null()) {
				pDataStructure->application = jsonObjectData["application"];
			}

			if (jsonObjectData.contains("application_id") && !jsonObjectData["application_id"].is_null()) {
				pDataStructure->applicationId = stoull(jsonObjectData["application_id"].get<std::string>());
			}

			if (jsonObjectData.contains("message_reference") && !jsonObjectData["message_reference"].is_null()) {
				pDataStructure->messageReference = jsonObjectData["message_reference"];
			}

			if (jsonObjectData.contains("flags") && !jsonObjectData["flags"].is_null()) {
				pDataStructure->flags = jsonObjectData["flags"].get<int32_t>();
			}

			if (jsonObjectData.contains("sticker_items") && !jsonObjectData["sticker_items"].is_null()) {
				pDataStructure->stickerItems.clear();
				pDataStructure->stickerItems.reserve(jsonObjectData["sticker_items"].size());
				for (auto& value: jsonObjectData["sticker_items"]) {
					StickerItemData newData{ value };
					pDataStructure->stickerItems.push_back(newData);
				}
				pDataStructure->stickerItems.shrink_to_fit();
			}

			if (jsonObjectData.contains("stickers") && !jsonObjectData["stickers"].is_null()) {
				pDataStructure->stickers.clear();
				pDataStructure->stickers.reserve(jsonObjectData["stickers"].size());
				for (auto& value: jsonObjectData["stickers"]) {
					StickerData newData{ value };
					pDataStructure->stickers.push_back(newData);
				}
				pDataStructure->stickers.shrink_to_fit();
			}

			if (jsonObjectData.contains("interaction") && !jsonObjectData["interaction"].is_null()) {
				pDataStructure->interaction = jsonObjectData["interaction"];
			}

			if (jsonObjectData.contains("components") && !jsonObjectData["components"].is_null()) {
				pDataStructure->components.clear();
				pDataStructure->components.reserve(jsonObjectData["components"].size());
				for (auto& value: jsonObjectData["components"]) {
					ActionRowData newData{ value };
					pDataStructure->components.push_back(newData);
				}
				pDataStructure->components.shrink_to_fit();
			}

			if (jsonObjectData.contains("thread") && !jsonObjectData["thread"].is_null()) {
				pDataStructure->thread = jsonObjectData["thread"];
			}
		}
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

		StickerPackData& operator=(const nlohmann::json& jsonObjectData) {
			this->parseObject(jsonObjectData, this);
			return *this;
		}

		StickerPackData(const nlohmann::json& jsonObjectData) {
			*this = jsonObjectData;
		}

		virtual ~StickerPackData() = default;

	  protected:
		void parseObject(const nlohmann::json& jsonObjectData, StickerPackData* pDataStructure) {
			if (jsonObjectData.contains("stickers") && !jsonObjectData["stickers"].is_null()) {
				pDataStructure->stickers.clear();
				pDataStructure->stickers.reserve(jsonObjectData["stickers"].size());
				for (auto& value: jsonObjectData["stickers"]) {
					StickerData newData{ value };
					pDataStructure->stickers.push_back(newData);
				}
				pDataStructure->stickers.shrink_to_fit();
			}

			if (jsonObjectData.contains("cover_sticker_id") && !jsonObjectData["cover_sticker_id"].is_null()) {
				pDataStructure->coverStickerId = jsonObjectData["cover_sticker_id"].get<std::string>();
			}

			if (jsonObjectData.contains("banner_asset_id") && !jsonObjectData["banner_asset_id"].is_null()) {
				pDataStructure->bannerAssetId = jsonObjectData["banner_asset_id"].get<std::string>();
			}

			if (jsonObjectData.contains("description") && !jsonObjectData["description"].is_null()) {
				pDataStructure->description = jsonObjectData["description"].get<std::string>();
			}

			if (jsonObjectData.contains("name") && !jsonObjectData["name"].is_null()) {
				pDataStructure->name = jsonObjectData["name"].get<std::string>();
			}

			if (jsonObjectData.contains("id") && !jsonObjectData["id"].is_null()) {
				pDataStructure->Id = stoull(jsonObjectData["id"].get<std::string>());
			}

			if (jsonObjectData.contains("sku_id") && !jsonObjectData["sku_id"].is_null()) {
				pDataStructure->skuId = jsonObjectData["sku_id"].get<std::string>();
			}
		}
	};

	class DiscordCoreAPI_Dll StickerPackDataVector : public DataParser<StickerPackDataVector> {
	  public:
		StickerPackDataVector() = default;

		operator std::vector<StickerPackData>() {
			return this->theStickerPackDatas;
		}

		StickerPackDataVector& operator=(const nlohmann::json& jsonObjectData) {
			this->parseObject(jsonObjectData, this);
			return *this;
		}

		StickerPackDataVector(const nlohmann::json& jsonObjectData) {
			*this = jsonObjectData;
		}

		virtual ~StickerPackDataVector() = default;

	  protected:
		std::vector<StickerPackData> theStickerPackDatas{};

		void parseObject(const nlohmann::json& jsonObjectData, StickerPackDataVector* pDataStructure) {
			pDataStructure->theStickerPackDatas.reserve(jsonObjectData.size());
			for (auto& value: jsonObjectData) {
				DiscordCoreAPI::StickerPackData newData{ value };
				pDataStructure->theStickerPackDatas.push_back(newData);
			}
			pDataStructure->theStickerPackDatas.shrink_to_fit();
		}
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

		ConnectionData& operator=(const nlohmann::json& jsonObjectData) {
			this->parseObject(jsonObjectData, this);
			return *this;
		}

		ConnectionData(const nlohmann::json& jsonObjectData) {
			*this = jsonObjectData;
		}

		virtual ~ConnectionData() = default;

	  protected:
		void parseObject(const nlohmann::json& jsonObjectData, ConnectionData* pDataStructure) {
			if (jsonObjectData.contains("name") && !jsonObjectData["name"].is_null()) {
				pDataStructure->name = jsonObjectData["name"].get<std::string>();
			}

			if (jsonObjectData.contains("id") && !jsonObjectData["id"].is_null()) {
				pDataStructure->id = stoull(jsonObjectData["id"].get<std::string>());
			}

			if (jsonObjectData.contains("type") && !jsonObjectData["type"].is_null()) {
				pDataStructure->type = jsonObjectData["type"].get<std::string>();
			}

			if (jsonObjectData.contains("show_activity") && !jsonObjectData["show_activity"].is_null()) {
				pDataStructure->showActivity = jsonObjectData["show_activity"].get<bool>();
			}

			if (jsonObjectData.contains("friend_sync") && !jsonObjectData["friend_sync"].is_null()) {
				pDataStructure->friendSync = jsonObjectData["friend_sync"].get<bool>();
			}

			if (jsonObjectData.contains("revoked") && !jsonObjectData["revoked"].is_null()) {
				pDataStructure->revoked = jsonObjectData["revoked"].get<bool>();
			}

			if (jsonObjectData.contains("verified") && !jsonObjectData["verified"].is_null()) {
				pDataStructure->verified = jsonObjectData["verified"].get<bool>();
			}

			if (jsonObjectData.contains("visibility") && !jsonObjectData["visibility"].is_null()) {
				pDataStructure->visibility = jsonObjectData["visibility"].get<ConnectionVisibilityTypes>();
			}

			if (jsonObjectData.contains("integrations") && !jsonObjectData["integrations"].is_null()) {
				pDataStructure->integrations.clear();
				pDataStructure->integrations.reserve(jsonObjectData["integrations"].size());
				for (auto& value: jsonObjectData["integrations"]) {
					IntegrationData newData{ value };
					pDataStructure->integrations.push_back(newData);
				}
				pDataStructure->integrations.shrink_to_fit();
			}
		}
	};

	class DiscordCoreAPI_Dll ConnectionDataVector : public DataParser<ConnectionDataVector> {
	  public:
		ConnectionDataVector() = default;

		operator std::vector<ConnectionData>() {
			return this->theConnectionDatas;
		}

		ConnectionDataVector& operator=(const nlohmann::json& jsonObjectData) {
			this->parseObject(jsonObjectData, this);
			return *this;
		}

		ConnectionDataVector(const nlohmann::json& jsonObjectData) {
			*this = jsonObjectData;
		}

		virtual ~ConnectionDataVector() = default;

	  protected:
		std::vector<ConnectionData> theConnectionDatas{};

		void parseObject(const nlohmann::json& jsonObjectData, ConnectionDataVector* pDataStructure) {
			pDataStructure->theConnectionDatas.reserve(jsonObjectData.size());
			for (auto& value: jsonObjectData) {
				DiscordCoreAPI::ConnectionData newData{ value };
				pDataStructure->theConnectionDatas.push_back(newData);
			}
			pDataStructure->theConnectionDatas.shrink_to_fit();
		}
	};

	/// ApplicationCommand Interaction data option. \brief ApplicationCommand Interaction data option.
	struct DiscordCoreAPI_Dll ApplicationCommandInteractionDataOption : public DataParser<ApplicationCommandInteractionDataOption> {
		std::vector<ApplicationCommandInteractionDataOption> options{};///< ApplicationCommand Interaction data options.
		ApplicationCommandOptionType type{};///< The type of ApplicationCommand options.
		std::string valueString{};///< The value if it's a std::string.
		bool valueBool{ false };///< the value if it's a bool.
		int32_t valueInt{ 0 };///< The value if it's an int32_t.
		bool focused{ false };///< 	True if this option is the currently focused option for autocomplete.
		std::string name{};///< The name of the current option.

		ApplicationCommandInteractionDataOption() = default;

		ApplicationCommandInteractionDataOption& operator=(const nlohmann::json& jsonObjectData) {
			this->parseObject(jsonObjectData, this);
			return *this;
		}

		ApplicationCommandInteractionDataOption(const nlohmann::json& jsonObjectData) {
			*this = jsonObjectData;
		}

		virtual ~ApplicationCommandInteractionDataOption() = default;

	  protected:
		void parseObject(const nlohmann::json& jsonObjectData, ApplicationCommandInteractionDataOption* pDataStructure) {
			if (jsonObjectData.contains("name") && !jsonObjectData["name"].is_null()) {
				pDataStructure->name = jsonObjectData["name"].get<std::string>();
			}

			if (jsonObjectData.contains("type") && !jsonObjectData["type"].is_null()) {
				pDataStructure->type = jsonObjectData["type"].get<ApplicationCommandOptionType>();
			}

			if (jsonObjectData.contains("focused") && !jsonObjectData["focused"].is_null()) {
				pDataStructure->focused = jsonObjectData["focused"].get<bool>();
			}

			if (jsonObjectData.contains("value") && !jsonObjectData["value"].is_null()) {
				if (jsonObjectData["value"].is_boolean()) {
					pDataStructure->valueBool = jsonObjectData["value"].get<bool>();
				}
				if (jsonObjectData["value"].is_string()) {
					pDataStructure->valueString = jsonObjectData["value"].get<std::string>();
				}
				if (jsonObjectData["value"].is_number()) {
					pDataStructure->valueInt = jsonObjectData["value"].get<int32_t>();
				}
			}

			if (jsonObjectData.contains("options") && !jsonObjectData["options"].is_null()) {
				pDataStructure->options.reserve(jsonObjectData["options"].size());
				for (auto& value: jsonObjectData["options"]) {
					ApplicationCommandInteractionDataOption newData{ value };
					pDataStructure->options.push_back(newData);
				}
				pDataStructure->options.shrink_to_fit();
			}
		}
	};

	/// ApplicationCommand Interaction data. \brief ApplicationCommand Interaction data.
	class DiscordCoreAPI_Dll ApplicationCommandInteractionData : public DiscordEntity,  public DataParser<ApplicationCommandInteractionData> {
	  public:
		std::vector<ApplicationCommandInteractionDataOption> options{};///< ApplicationCommand Interaction data options.
		ApplicationCommandType type{};///< The type of ApplicationCommand.
		ResolvedData resolved{};///< Resolved data.
		Snowflake guildId{};///< The guild that the command took place in.
		std::string name{};///< The name of the command.

		ApplicationCommandInteractionData() = default;

		ApplicationCommandInteractionData& operator=(const nlohmann::json& jsonObjectData) {
			this->parseObject(jsonObjectData, this);
			return *this;
		}

		ApplicationCommandInteractionData(const nlohmann::json& jsonObjectData) {
			*this = jsonObjectData;
		}

		virtual ~ApplicationCommandInteractionData() = default;

	  protected:
		void parseObject(const nlohmann::json& jsonObjectData, ApplicationCommandInteractionData* pDataStructure) {
			if (jsonObjectData.contains("type") && !jsonObjectData["type"].is_null()) {
				pDataStructure->type = jsonObjectData["type"].get<ApplicationCommandType>();
			}

			if (jsonObjectData.contains("name") && !jsonObjectData["name"].is_null()) {
				pDataStructure->name = jsonObjectData["name"].get<std::string>();
			}

			if (jsonObjectData.contains("guild_id") && !jsonObjectData["guild_id"].is_null()) {
				pDataStructure->guildId = stoull(jsonObjectData["guild_id"].get<std::string>());
			}

			if (jsonObjectData.contains("id") && !jsonObjectData["id"].is_null()) {
				pDataStructure->id = stoull(jsonObjectData["id"].get<std::string>());
			}

			if (jsonObjectData.contains("options") && !jsonObjectData["options"].is_null()) {
				pDataStructure->options.reserve(jsonObjectData["options"].size());
				for (auto& value: jsonObjectData["options"]) {
					ApplicationCommandInteractionDataOption newData{ value };
					pDataStructure->options.push_back(newData);
				}
				pDataStructure->options.shrink_to_fit();
			}

			if (jsonObjectData.contains("resolved") && !jsonObjectData["resolved"].is_null()) {
				auto& value = jsonObjectData["resolved"];

				if (value.contains("attachments") && !value["attachments"].is_null()) {
					pDataStructure->resolved.attachments.clear();
					auto newMap = value["attachments"].get<std::unordered_map<std::string, nlohmann::json>>();
					for (auto& [key, newValue]: newMap) {
						AttachmentData newData{ newValue };
						pDataStructure->resolved.attachments[stoull(key)] = newData;
					}
				}

				if (value.contains("users") && !value["users"].is_null()) {
					pDataStructure->resolved.users.clear();
					auto newMap = value["users"].get<std::unordered_map<std::string, nlohmann::json>>();
					for (auto& [key, newValue]: newMap) {
						UserData newData{ value };
						pDataStructure->resolved.users[stoull(key)] = newData;
					}
				}
				if (value.contains("channels") && !value["channels"].is_null()) {
					pDataStructure->resolved.channels.clear();
					auto newMap = value["channels"].get<std::unordered_map<std::string, nlohmann::json>>();
					for (auto& [key, newValue]: newMap) {
						ChannelData newData{ newValue };
						pDataStructure->resolved.channels[stoull(key)] = newData;
					}
				}
				if (value.contains("roles") && !value["roles"].is_null()) {
					pDataStructure->resolved.roles.clear();
					auto newMap = value["roles"].get<std::unordered_map<std::string, nlohmann::json>>();
					for (auto& [key, newValue]: newMap) {
						RoleData newData{ newValue };
						pDataStructure->resolved.roles[stoull(key)] = newData;
					}
				}
				if (value.contains("members") && !value["members"].is_null()) {
					pDataStructure->resolved.members.clear();
					auto newMap = value["members"].get<std::unordered_map<std::string, nlohmann::json>>();
					for (auto& [key, newValue]: newMap) {
						GuildMemberData newData{ newValue };
						pDataStructure->resolved.members[stoull(key)] = newData;
					}
				}
				if (value.contains("messages") && !value["messages"].is_null()) {
					pDataStructure->resolved.messages.clear();
					auto newMap = value["messages"].get<std::unordered_map<std::string, nlohmann::json>>();
					for (auto& [key, newValue]: newMap) {
						MessageData newData{ newValue };
						pDataStructure->resolved.messages[stoull(key)] = newData;
					}
				}
			}
		}
	};

	/// Interaction data data. \brief Interaction data data.
	struct DiscordCoreAPI_Dll InteractionDataData : public DataParser<InteractionDataData> {
		ApplicationCommandInteractionData applicationCommandData{};///< ApplicationCommand Interaction data.
		MessageCommandInteractionData messageInteractionData{};///< Message command Interaction data.
		UserCommandInteractionData userInteractionData{};///< User command Interaction data.
		ComponentInteractionData componentData{};///< Component Interaction data.
		ModalInteractionData modalData{};///< Modal Interaction data.

		InteractionDataData() = default;

		InteractionDataData& operator=(const nlohmann::json& jsonObjectData) {
			this->parseObject(jsonObjectData, this);
			return *this;
		}

		InteractionDataData(const nlohmann::json& jsonObjectData) {
			*this = jsonObjectData;
		}

		virtual ~InteractionDataData() = default;

	  protected:
		void parseObject(const nlohmann::json& jsonObjectData, InteractionDataData* pDataStructure) {
			if (jsonObjectData.contains("id") && !jsonObjectData["id"].is_null()) {
				pDataStructure->applicationCommandData = jsonObjectData;
			}

			if (jsonObjectData.contains("target_id") && !jsonObjectData["target_id"].is_null()) {
				pDataStructure->messageInteractionData = jsonObjectData;
				pDataStructure->userInteractionData = jsonObjectData;
			}

			if (jsonObjectData.contains("component_type") && !jsonObjectData["component_type"].is_null()) {
				pDataStructure->componentData = jsonObjectData;
			}

			if (jsonObjectData.contains("components") && !jsonObjectData["components"].is_null()) {
				pDataStructure->modalData = jsonObjectData;
			}
		}
	};

	/// Interaction data. \brief Interaction data.
	class DiscordCoreAPI_Dll InteractionData : public DiscordEntity, public DataParser<InteractionData> {
	  public:
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

		InteractionData& operator=(const nlohmann::json& jsonObjectData) {
			this->parseObject(jsonObjectData, this);
			return *this;
		}

		InteractionData(const nlohmann::json& jsonObjectData) {
			*this = jsonObjectData;
		}

		virtual ~InteractionData() = default;

	  protected:
		void parseObject(const nlohmann::json& jsonObjectData, InteractionData* pDataStructure) {
			if (jsonObjectData.contains("data") && !jsonObjectData["data"].is_null()) {
				pDataStructure->data = jsonObjectData["data"];
				pDataStructure->rawData = jsonObjectData["data"];
			}

			if (jsonObjectData.contains("type") && !jsonObjectData["type"].is_null()) {
				pDataStructure->type = jsonObjectData["type"].get<InteractionType>();
			}

			if (jsonObjectData.contains("token") && !jsonObjectData["token"].is_null()) {
				pDataStructure->token = jsonObjectData["token"].get<std::string>();
			}

			if (jsonObjectData.contains("member") && !jsonObjectData["member"].is_null()) {
				pDataStructure->member = jsonObjectData["member"];
				pDataStructure->user.avatar = pDataStructure->member.userAvatar;
				pDataStructure->user.id = pDataStructure->member.id;
				pDataStructure->user.userName = pDataStructure->member.userName;
			} else if (jsonObjectData.contains("user") && !jsonObjectData["user"].is_null()) {
				pDataStructure->user = jsonObjectData["user"];
			}

			if (jsonObjectData.contains("channel_id") && !jsonObjectData["channel_id"].is_null()) {
				pDataStructure->channelId = stoull(jsonObjectData["channel_id"].get<std::string>());
			}

			if (jsonObjectData.contains("guild_id") && !jsonObjectData["guild_id"].is_null()) {
				pDataStructure->guildId = stoull(jsonObjectData["guild_id"].get<std::string>());
			}

			if (jsonObjectData.contains("locale") && !jsonObjectData["locale"].is_null()) {
				pDataStructure->locale = jsonObjectData["locale"].get<std::string>();
			}

			if (jsonObjectData.contains("guild_locale") && !jsonObjectData["guild_locale"].is_null()) {
				pDataStructure->guildLocale = jsonObjectData["guild_locale"].get<std::string>();
			}

			if (jsonObjectData.contains("message") && !jsonObjectData["message"].is_null()) {
				pDataStructure->message = jsonObjectData["message"];
			}

			if (jsonObjectData.contains("version") && !jsonObjectData["version"].is_null()) {
				pDataStructure->version = jsonObjectData["version"].get<int32_t>();
			}

			if (jsonObjectData.contains("id") && !jsonObjectData["id"].is_null()) {
				pDataStructure->id = stoull(jsonObjectData["id"].get<std::string>());
			}

			if (jsonObjectData.contains("application_id") && !jsonObjectData["application_id"].is_null()) {
				pDataStructure->applicationId = stoull(jsonObjectData["application_id"].get<std::string>());
			}
		}
	};

	/// Data from the SessionStart info. \brief Data from the SessionStart info.
	struct DiscordCoreAPI_Dll SessionStartData : public DataParser<SessionStartData> {
		uint32_t maxConcurrency{ 0 };///< The number of identify requests allowed per 5 seconds.
		uint32_t resetAfter{ 0 };///< The number of std::chrono::milliseconds after which the limit resets.
		uint32_t remaining{ 0 };///< The remaining number of session starts the current User is allowed.
		uint32_t total{ 0 };///< The total number of session starts the current User is allowed.

		SessionStartData() = default;

		SessionStartData& operator=(const nlohmann::json& jsonObjectData) {
			this->parseObject(jsonObjectData, this);
			return *this;
		}

		SessionStartData(const nlohmann::json& jsonObjectData) {
			*this = jsonObjectData;
		}

		virtual ~SessionStartData() = default;

	  protected:
		void parseObject(const nlohmann::json& jsonObjectData, SessionStartData* pDataStructure) {
			if (jsonObjectData.contains("max_concurrency") && !jsonObjectData["max_concurrency"].is_null()) {
				pDataStructure->maxConcurrency = jsonObjectData["max_concurrency"].get<uint32_t>();
			}

			if (jsonObjectData.contains("remaining") && !jsonObjectData["remaining"].is_null()) {
				pDataStructure->remaining = jsonObjectData["remaining"].get<uint32_t>();
			}

			if (jsonObjectData.contains("reset_after") && !jsonObjectData["reset_after"].is_null()) {
				pDataStructure->resetAfter = jsonObjectData["reset_after"].get<uint32_t>();
			}

			if (jsonObjectData.contains("total") && !jsonObjectData["total"].is_null()) {
				pDataStructure->total = jsonObjectData["total"].get<uint32_t>();
			}
		}
	};

	/// Data from the GetGatewatBot endpoint. \brief Data from the GetGatewatBot endpoint.
	struct DiscordCoreAPI_Dll GatewayBotData : public DataParser<GatewayBotData> {
		SessionStartData sessionStartLimit{};///< Information on the current session start limit.
		uint32_t shards{ 0 };///< The recommended number of shards to use when connecting.
		std::string url{};///< The WSS Url that can be used for connecting to the gateway.

		GatewayBotData() = default;

		GatewayBotData& operator=(const nlohmann::json& jsonObjectData) {
			this->parseObject(jsonObjectData, this);
			return *this;
		}

		GatewayBotData(const nlohmann::json& jsonObjectData) {
			*this = jsonObjectData;
		}

		virtual ~GatewayBotData() = default;

	  protected:
		void parseObject(const nlohmann::json& jsonObjectData, GatewayBotData* pDataStructure) {
			if (jsonObjectData.contains("session_start_limit") && !jsonObjectData["session_start_limit"].is_null()) {
				pDataStructure->sessionStartLimit = jsonObjectData["session_start_limit"];
			}

			if (jsonObjectData.contains("shards") && !jsonObjectData["shards"].is_null()) {
				pDataStructure->shards = jsonObjectData["shards"].get<uint32_t>();
			}

			if (jsonObjectData.contains("url") && !jsonObjectData["url"].is_null()) {
				pDataStructure->url = jsonObjectData["url"].get<std::string>();
			}
		}
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

		GuildEmojisUpdateEventData& operator=(const nlohmann::json& jsonObjectData) {
			this->parseObject(jsonObjectData, this);
			return *this;
		}

		GuildEmojisUpdateEventData(const nlohmann::json& jsonObjectData) {
			*this = jsonObjectData;
		}

		virtual ~GuildEmojisUpdateEventData() = default;

	  protected:
		void parseObject(const nlohmann::json& jsonObjectData, GuildEmojisUpdateEventData* pDataStructure) {
			if (jsonObjectData.contains("guild_id") && !jsonObjectData["guild_id"].is_null()) {
				pDataStructure->guildId = stoull(jsonObjectData["guild_id"].get<std::string>());
			}

			if (jsonObjectData.contains("emojis") && !jsonObjectData["emojis"].is_null()) {
				pDataStructure->emojis.clear();
				pDataStructure->emojis.reserve(jsonObjectData["emojis"].size());
				for (auto& value: jsonObjectData["emojis"]) {
					EmojiData newData{ value };
					pDataStructure->emojis.push_back(newData);
				}
				pDataStructure->emojis.shrink_to_fit();
			}
		}
	};

	/// Data representing a Guild Sticker Update event. \brief Data representing a Guild Stickers Update event.
	struct DiscordCoreAPI_Dll GuildStickersUpdateEventData : public DataParser<GuildStickersUpdateEventData> {
		std::vector<StickerData> stickers{};
		Snowflake guildId{};

		GuildStickersUpdateEventData() = default;

		GuildStickersUpdateEventData& operator=(const nlohmann::json& jsonObjectData) {
			this->parseObject(jsonObjectData, this);
			return *this;
		}

		GuildStickersUpdateEventData(const nlohmann::json& jsonObjectData) {
			*this = jsonObjectData;
		}

		virtual ~GuildStickersUpdateEventData() = default;

	  protected:
		void parseObject(const nlohmann::json& jsonObjectData, GuildStickersUpdateEventData* pDataStructure) {
			if (jsonObjectData.contains("guild_id") && !jsonObjectData["guild_id"].is_null()) {
				pDataStructure->guildId = stoull(jsonObjectData["guild_id"].get<std::string>());
			}

			if (jsonObjectData.contains("stickers") && !jsonObjectData["stickers"].is_null()) {
				pDataStructure->stickers.clear();
				pDataStructure->stickers.reserve(jsonObjectData["stickers"].size());
				for (auto& value: jsonObjectData["stickers"]) {
					StickerData newData{ value };
					pDataStructure->stickers.push_back(newData);
				}
				pDataStructure->stickers.shrink_to_fit();
			}
		}
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

		GuildMembersChunkEventData& operator=(const nlohmann::json& jsonObjectData) {
			this->parseObject(jsonObjectData, this);
			return *this;
		}

		GuildMembersChunkEventData(const nlohmann::json& jsonObjectData) {
			*this = jsonObjectData;
		}

		virtual ~GuildMembersChunkEventData() = default;

	  protected:
		void parseObject(const nlohmann::json& jsonObjectData, GuildMembersChunkEventData* pDataStructure) {
			if (jsonObjectData.contains("guild_id") && !jsonObjectData["guild_id"].is_null()) {
				pDataStructure->guildId = stoull(jsonObjectData["guild_id"].get<std::string>());
			}

			if (jsonObjectData.contains("nonce") && !jsonObjectData["nonce"].is_null()) {
				pDataStructure->nonce = jsonObjectData["nonce"].get<std::string>();
			}

			if (jsonObjectData.contains("chunk_index") && !jsonObjectData["chunk_index"].is_null()) {
				pDataStructure->chunkIndex = jsonObjectData["chunk_index"].get<int32_t>();
			}

			if (jsonObjectData.contains("chunk_count") && !jsonObjectData["chunk_count"].is_null()) {
				pDataStructure->chunkCount = jsonObjectData["chunk_count"].get<int32_t>();
			}

			if (jsonObjectData.contains("presences") && !jsonObjectData["presences"].is_null()) {
				pDataStructure->presences.clear();
				pDataStructure->presences.reserve(jsonObjectData["presences"].size());
				for (auto& value: jsonObjectData["presences"]) {
					PresenceUpdateData newData{ value };
					pDataStructure->presences.push_back(newData);
				}
				pDataStructure->presences.shrink_to_fit();
			}

			if (jsonObjectData.contains("not_found") && !jsonObjectData["not_found"].is_null()) {
				pDataStructure->notFound.clear();
				pDataStructure->notFound.reserve(jsonObjectData["not_found"].size());
				for (auto& value: jsonObjectData["not_found"]) {
					pDataStructure->notFound.push_back(value);
				}
				pDataStructure->notFound.shrink_to_fit();
			}

			if (jsonObjectData.contains("members") && !jsonObjectData["members"].is_null()) {
				pDataStructure->members.clear();
				pDataStructure->members.reserve(jsonObjectData["members"].size());
				for (auto& value: jsonObjectData["members"]) {
					GuildMemberData newData{ value };
					pDataStructure->members.push_back(newData);
				}
				pDataStructure->members.shrink_to_fit();
			}
		}
	};

	/// Data representing an input-event, which is any Message or Interaction that is coming into the bot as an input. \brief Data representing an input-event, which is any Message or Interaction that is coming into the bot as an input.
	class DiscordCoreAPI_Dll InputEventData {
	  public:
		friend class DiscordCoreInternal::BaseSocketAgent;
		friend OnInteractionCreationData;
		friend RespondToInputEventData;
		friend BaseFunctionArguments;
		friend DiscordCoreClient;
		friend EventHandler;
		friend CommandData;
		friend InputEvents;

		InputEventResponseType responseType{};///< The type of response that this input value represents.

		InputEventData& operator=(const InputEventData& other);

		InputEventData(const InputEventData& other);

		InputEventData& operator=(InputEventData& other);

		InputEventData(InputEventData& other);

		InputEventData& operator=(InteractionData& other);

		InputEventData(InteractionData& interactionData);

		InputEventData() = default;

		/// Returns the userName of the last User to trigger this input-event. \brief Returns the userName of the last User to trigger this input-event.
		/// \returns std::string A std::string containing the User name.
		std::string getUserName();

		/// Gets the avatar Url of the last User to trigger this input-event. \brief Gets the avatar Url of the last User to trigger this input-event.
		/// \returns std::string A std::string containing the avatar Url.
		std::string getAvatarUrl();

		/// Returns the Message embeds that are on the Message, if applicable. \brief Returns the Message embeds that are on the Message, if applicable.
		/// \returns std::vector A std::vector containing the EmbedData.
		std::vector<EmbedData> getEmbeds();

		/// Returns the Message components that are on the Message, if applicable. \brief Returns the Message components that are on the Message, if applicable.
		/// \returns std::vector A std::vector containing ActionRowData.
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
		/// \returns std::string A std::string containing the Interaction token.
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
		friend CreateEphemeralInteractionResponseData;
		friend CreateDeferredInteractionResponseData;
		friend CreateEphemeralFollowUpMessageData;
		friend struct InteractionResponseData;
		friend DeleteInteractionResponseData;
		friend CreateInteractionResponseData;
		friend EditInteractionResponseData;
		friend DeleteFollowUpMessageData;
		friend CreateFollowUpMessageData;
		friend EditFollowUpMessageData;
		friend CreateMessageData;
		friend EditMessageData;
		friend InputEvents;
		friend SendDMData;

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
		/// \param options A std::vector of select-menu-options to offer.
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
		/// \param dataPackage A std::string, containing the content.
		/// \returns RespondToInputEventData& A reference to this data structure.
		RespondToInputEventData& addContent(const std::string& dataPackage);

		/// For setting the tts status of a response. \brief For setting the tts status of a response.
		/// \param enabledTTs A bool.
		/// \returns RespondToInputEventData& A reference to this data structure.
		RespondToInputEventData& setTTSStatus(bool enabledTTs);

		/// For setting the choices of an autocomplete response. \brief For setting the choices of an autocomplete response.
		/// \param theValue An nlohmann::json value that is either a float, int32_t or a std::string.
		/// \param theName A std::string for the name of the choice.
		/// \param theNameLocalizations A std::unordered_map<std::string, std::string> for the name localizations.
		/// \returns RespondToInputEventData& A reference to this data structure.
		RespondToInputEventData& setAutoCompleteChoice(nlohmann::json theValue, const std::string& theName,
			std::unordered_map<std::string, std::string> theNameLocalizations = std::unordered_map<std::string, std::string>{});

		/// For setting the direct-Message User target of a response. \brief For setting the direct-Message User target of a response.
		/// \param targetUserIdNew A std::string, containing the target User's id.
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
		/// \param options A std::vector of select-menu-options to offer.
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
		/// \param dataPackage A std::string, containing the content.
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
		InteractionResponseData& operator=(const RespondToInputEventData& other) {
			this->data.allowedMentions = other.allowedMentions;
			this->data.components = other.components;
			this->data.customId = other.customId;
			this->data.choices = other.choices;
			this->data.content = other.content;
			this->data.embeds = other.embeds;
			this->data.title = other.title;
			this->data.files = other.files;
			this->data.flags = other.flags;
			this->data.tts = other.tts;
			return *this;
		}
		InteractionResponseData(const RespondToInputEventData& other) {
			*this = other;
		}
		InteractionResponseData() = default;

		operator std::string() {
			nlohmann::json data{};
			for (auto& value: this->data.attachments) {
				data["data"]["attachments"].push_back(DiscordCoreAPI::AttachmentData{ value });
			}
			if (this->data.components.size() == 0) {
				data["data"]["components"] = nlohmann::json::array();
			} else {
				for (auto& value: this->data.components) {
					data["data"]["components"].push_back(value);
				}
			}
			data["data"]["allowed_mentions"] = DiscordCoreAPI::AllowedMentionsData{ this->data.allowedMentions };
			if (this->data.choices.size() > 0) {
				nlohmann::json::array_t theArray{};
				for (auto& value: this->data.choices) {
					nlohmann::json theValue{};
					theValue["name"] = value.name;
					theValue["name_localizations"] = value.nameLocalizations;
					theValue["value"] = value.value;
					theArray.push_back(theValue);
				}
				data["data"]["choices"] = theArray;
			}
			if (this->data.embeds.size() == 0) {
				data["data"]["embeds"] = nlohmann::json::array();
			} else {
				for (auto& value: this->data.embeds) {
					data["data"]["embeds"].push_back(DiscordCoreAPI::EmbedData{ value });
				}
			}
			if (this->data.customId != "") {
				data["data"]["custom_id"] = this->data.customId;
			}
			if (this->data.content != "") {
				data["data"]["content"] = this->data.content;
			}
			if (this->data.title != "") {
				data["data"]["title"] = this->data.title;
			}
			data["data"]["flags"] = this->data.flags;
			data["data"]["tts"] = this->data.tts;
			data["type"] = this->type;
			return data.dump();
		}

		InteractionCallbackData data{};///< Interaction ApplicationCommand callback data.
		InteractionCallbackType type{};///< Interaction callback type.
	};

	/// Command data, for functions executed by the CommandController. \brief Command data, for functions executed by the CommandController.
	struct DiscordCoreAPI_Dll CommandData : public DataParser<CommandData> {
		friend struct DiscordCoreAPI_Dll BaseFunctionArguments;

		std::vector<std::string> optionsArgs{};
		std::string subCommandGroupName{};
		std::string subCommandName{};
		std::string commandName{};

		CommandData() = default;

		CommandData(InputEventData inputEventData);

		CommandData& operator=(const nlohmann::json& jsonObjectData) {
			this->parseObject(jsonObjectData, this);
			return *this;
		}

		CommandData(const nlohmann::json& jsonObjectData) {
			*this = jsonObjectData;
		}

		virtual ~CommandData() = default;

	  protected:
		InputEventData eventData{};

		void parseObject(const nlohmann::json& jsonObjectData, CommandData* pDataStructure) {
			if (jsonObjectData.contains("options") && !jsonObjectData["options"].is_null()) {
				pDataStructure->optionsArgs.reserve(jsonObjectData["options"].size());
				for (auto& value: jsonObjectData["options"]) {
					if (value.contains("type") && value["type"] == 1) {
						if (value.contains("name")) {
							pDataStructure->subCommandName = value["name"];
						}
					} else if (value.contains("type") && value["type"] == 2) {
						if (value.contains("name")) {
							pDataStructure->subCommandGroupName = value["name"];
						}
					}
					if (value.contains("options")) {
						parseObject(value, pDataStructure);
					}
					if (value.contains("value") && !value["value"].is_null()) {
						auto& newValueNew = value["value"];
						if (newValueNew.is_string()) {
							pDataStructure->optionsArgs.push_back(newValueNew);
						} else if (newValueNew.is_number()) {
							pDataStructure->optionsArgs.push_back(std::to_string(newValueNew.get<int64_t>()));
						} else if (newValueNew.is_boolean()) {
							pDataStructure->optionsArgs.push_back(std::to_string(newValueNew.get<bool>()));
						}
					}
				}
				pDataStructure->optionsArgs.shrink_to_fit();
			}

			if (jsonObjectData.contains("name") && !jsonObjectData["name"].is_null()) {
				pDataStructure->commandName = jsonObjectData["name"].get<std::string>();
			}
		}
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
	struct DiscordCoreAPI_Dll Song : public DataParser<Song> {
		friend class DiscordCoreInternal::SoundCloudRequestBuilder;
		friend class DiscordCoreInternal::YouTubeRequestBuilder;
		friend class DiscordCoreInternal::SoundCloudAPI;
		friend class DiscordCoreInternal::YouTubeAPI;
		friend SongAPI;

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

		Song& operator=(const nlohmann::json& jsonObjectData) {
			this->parseObject(jsonObjectData, this);
			return *this;
		}

		Song(const nlohmann::json& jsonObjectData) {
			*this = jsonObjectData;
		}

		virtual ~Song() = default;

	  protected:
		std::string trackAuthorization{};
		bool doWeGetSaved{ false };
		YouTubeFormat format{};

		void parseObject(const nlohmann::json& jsonObjectData, Song* pDataStructure) {
			if (jsonObjectData.contains("lengthText") && !jsonObjectData["lengthText"].is_null()) {
				pDataStructure->duration = jsonObjectData["lengthText"]["accessibility"]["accessibilityData"]["label"].get<std::string>();
			}

			if (jsonObjectData.contains("detailedMetadataSnippets") && !jsonObjectData["detailedMetadataSnippets"].is_null()) {
				for (auto& value: jsonObjectData["detailedMetadataSnippets"][0]["snippetText"]["runs"]) {
					std::string newString = value["text"].get<std::string>();
					if (newString.size() > 256) {
						newString = newString.substr(0, 256);
					}
					pDataStructure->description = utf8MakeValid(newString);
				}
			}

			if (jsonObjectData.contains("thumbnail") && !jsonObjectData["thumbnail"].is_null()) {
				pDataStructure->thumbnailUrl = jsonObjectData["thumbnail"]["thumbnails"][0]["url"].get<std::string>();
			}

			if (jsonObjectData.contains("title") && !jsonObjectData["title"].is_null()) {
				if (jsonObjectData["title"].contains("runs")) {
					std::string newString = jsonObjectData["title"]["runs"][0]["text"].get<std::string>();
					if (newString.size() > 256) {
						newString = newString.substr(0, 256);
					}
					pDataStructure->songTitle = utf8MakeValid(newString);
					;
				} else if (jsonObjectData["title"].contains("simpleText")) {
					std::string newString = jsonObjectData["title"]["simpleText"].get<std::string>();
					if (newString.size() > 256) {
						newString = newString.substr(0, 256);
					}
					pDataStructure->songTitle = utf8MakeValid(newString);
				}
			}
			if (jsonObjectData.contains("videoId") && !jsonObjectData["videoId"].is_null()) {
				pDataStructure->songId = jsonObjectData["videoId"].get<std::string>();
			}

			if (jsonObjectData.contains("track_authorization") && !jsonObjectData["track_authorization"].is_null()) {
				pDataStructure->trackAuthorization = jsonObjectData["track_authorization"].get<std::string>();
			}

			if (jsonObjectData.contains("media") && !jsonObjectData["media"].is_null()) {
				bool isItFound{ false };
				for (auto& value: jsonObjectData["media"]["transcodings"]) {
					if (value["preset"] == "opus_0_0") {
						isItFound = true;
						pDataStructure->firstDownloadUrl = value["url"].get<std::string>();
						pDataStructure->songId = value["url"].get<std::string>();
						pDataStructure->doWeGetSaved = true;
					}
				}
				bool isItFound2{ false };
				if (!isItFound) {
					for (auto& value: jsonObjectData["media"]["transcodings"]) {
						if (value["preset"] == "mp3_0_0") {
							pDataStructure->firstDownloadUrl = value["url"].get<std::string>();
							pDataStructure->songId = value["url"].get<std::string>();
							isItFound2 = true;
						}
					}
				}
				if (!isItFound2 && !isItFound) {
					for (auto& value: jsonObjectData["media"]["transcodings"]) {
						pDataStructure->firstDownloadUrl = value["url"].get<std::string>();
						pDataStructure->songId = value["url"].get<std::string>();
					}
				}
			}

			if (jsonObjectData.contains("title") && !jsonObjectData["title"].is_null() && !jsonObjectData["title"].is_object()) {
				std::string newString = jsonObjectData["title"].get<std::string>();
				if (newString.size() > 256) {
					newString = newString.substr(0, 256);
				}
				pDataStructure->songTitle = utf8MakeValid(newString);
			}

			if (jsonObjectData.contains("description") && !jsonObjectData["description"].is_null()) {
				std::string newString = jsonObjectData["description"].get<std::string>();
				if (newString.size() > 256) {
					newString = newString.substr(0, 256);
				}
				pDataStructure->description = utf8MakeValid(newString);
				pDataStructure->description += "...";
			}

			if (jsonObjectData.contains("artwork_url") && !jsonObjectData["artwork_url"].is_null()) {
				pDataStructure->thumbnailUrl = jsonObjectData["artwork_url"].get<std::string>();
			} else if (jsonObjectData.contains("user") && !jsonObjectData["user"].is_null()) {
				if (jsonObjectData["user"].contains("avatar_url") && !jsonObjectData["user"]["avatar_url"].is_null()) {
					pDataStructure->thumbnailUrl = jsonObjectData["user"]["avatar_url"].get<std::string>();
				}
			}

			if (jsonObjectData.contains("duration") && !jsonObjectData["duration"].is_null()) {
				pDataStructure->duration = TimeStamp::convertMsToDurationString(jsonObjectData["duration"].get<int32_t>());
			}

			if (jsonObjectData.contains("permalink_url") && !jsonObjectData["permalink_url"].is_null()) {
				pDataStructure->viewUrl = jsonObjectData["permalink_url"].get<std::string>();
			}
		}
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

	/// Base arguments for the command classes. \brief Base arguments for the command classes.
	struct DiscordCoreAPI_Dll BaseFunctionArguments {
		DiscordCoreClient* discordCoreClient{ nullptr };///< A pointer to the instance of DiscordCoreClient.
		InputEventData eventData{};///< InputEventData representing the input event that triggered the command.
		CommandData commandData{};///< The input command's data.

		BaseFunctionArguments() = default;

		BaseFunctionArguments(CommandData commandData, DiscordCoreClient* discordCoreClientNew) {
			this->discordCoreClient = discordCoreClientNew;
			this->eventData = commandData.eventData;
			this->commandData = commandData;
		}

		virtual ~BaseFunctionArguments() = default;
	};

	/// Base class DiscordCoreAPI_Dll for the command classes. \brief Base class DiscordCoreAPI_Dll for the command classes.
	struct DiscordCoreAPI_Dll BaseFunction {
		std::string helpDescription{};///< Description of the command for the Help command.
		BaseFunctionArguments args{};///< The function arguments.
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

/**@}*/

namespace DiscordCoreInternal {

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

	struct DiscordCoreAPI_Dll HttpsWorkloadData {
		static std::unordered_map<HttpsWorkloadType, std::atomic_int64_t> workloadIdsExternal;
		static std::unordered_map<HttpsWorkloadType, std::atomic_int64_t> workloadIdsInternal;
		static std::mutex accessMutex;

		std::unordered_map<std::string, std::string> headersToInsert{};
		PayloadType payloadType{ PayloadType::Application_Json };
		std::atomic_int64_t thisWorkerId{ 0 };
		HttpsWorkloadClass workloadClass{};
		HttpsWorkloadType workloadType{};
		std::string relativePath{};
		std::string callStack{};
		std::string content{};
		std::string baseUrl{};

		HttpsWorkloadData& operator=(const HttpsWorkloadData& other) {
			if (this != &other) {
				this->thisWorkerId.store(this->thisWorkerId.load());
				this->headersToInsert = other.headersToInsert;
				this->workloadClass = other.workloadClass;
				this->workloadType = other.workloadType;
				this->relativePath = other.relativePath;
				this->payloadType = other.payloadType;
				this->callStack = other.callStack;
				this->baseUrl = other.baseUrl;
				this->content = other.content;
			}
			return *this;
		}

		HttpsWorkloadData(const HttpsWorkloadData& other) {
			*this = other;
		}

		HttpsWorkloadData& operator=(HttpsWorkloadData& other) {
			if (this != &other) {
				this->thisWorkerId.store(this->thisWorkerId.load());
				this->headersToInsert = other.headersToInsert;
				this->workloadClass = other.workloadClass;
				this->workloadType = other.workloadType;
				this->relativePath = other.relativePath;
				this->payloadType = other.payloadType;
				this->callStack = other.callStack;
				this->baseUrl = other.baseUrl;
				this->content = other.content;
			}
			return *this;
		}

		HttpsWorkloadData(HttpsWorkloadData& other) {
			*this = other;
		}

		HttpsWorkloadData() = default;

		static int64_t getAndIncrementWorkloadId(HttpsWorkloadType workloadType) {
			std::lock_guard<std::mutex> theLock{ HttpsWorkloadData::accessMutex };
			int64_t theValue = HttpsWorkloadData::workloadIdsExternal[workloadType].load();
			HttpsWorkloadData::workloadIdsExternal[workloadType].store(theValue + 1);
			return theValue;
		}
	};

};// namespace DiscordCoreInternal
