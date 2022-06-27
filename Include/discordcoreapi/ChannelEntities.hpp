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
/// ChannelEntities.hpp - Header for the classes related classes and structs.
/// May 13, 2021
/// https://discordcoreapi.com
/// \file ChannelEntities.hpp

#pragma once

#include <discordcoreapi/FoundationEntities.hpp>
#include <discordcoreapi/Https.hpp>

namespace DiscordCoreAPI {

	/**
	 * \addtogroup foundation_entities
	 * @{
	 */

	/// For collecting a Channel from Discord's servers. \brief For collecting a Channel from Discord's servers.
	struct DiscordCoreAPI_Dll GetChannelData {
		uint64_t channelId{};///< The id of the Channel to acquire.
	};

	/// For deleting or closing a Channel. \brief For deleting or closing a Channel.
	struct DiscordCoreAPI_Dll DeleteOrCloseChannelData {
		uint64_t channelId{};///< The id of the Channel to close/delete.
		std::string reason{};///< The reason for deleting/closing the Channel.
	};

	/// For editing the Permission overwrites of a given Channel, for a given Role or User. \brief For editing the Permission overwrites of a given Channel, for a given Role or User.
	struct DiscordCoreAPI_Dll EditChannelPermissionOverwritesData {
		PermissionOverwritesType type{};///< The type of overwrite.
		uint64_t roleOrUserId{};///< The Role or User Id to modify the permissions for.
		uint64_t channelId{};///< The Channel id of the desired Channel to modify the permissions for.
		std::string reason{};///< Reason for editing the permission overwrites.
		std::string allow{};///< The permissions to list as "allowed".
		std::string deny{};///< The permissions to list as "deny".
	};

	/// For collecting the invites to a given Channel. \brief For collecting the invites to a given Channel.
	struct DiscordCoreAPI_Dll GetChannelInvitesData {
		uint64_t channelId{};
	};

	/// For creating an invite to a given Channel. \brief For creating an invite to a given Channel.
	struct DiscordCoreAPI_Dll CreateChannelInviteData {
		uint64_t targetApplicationId{};///< The id of the embedded application to open for this invite, required if target_type is 2.
		InviteTargetTypes targetType{};///<	The type of target for this voice Channel invite.
		uint64_t targetUserId{};///< Id of the user whose stream to display for this invite, required if target_type is 1.
		bool temporary{ false };///< Whether this invite only grants temporary membership.
		uint64_t channelId{};///< The id of the Channel to create the invite for.
		std::string reason{};///< Reason for creating the invite.
		bool unique{ false };///< If true, don't try to reuse a similar invite (useful for creating many unique one time use invites).
		int32_t maxUses{ 0 };///< Max number of uses or 0 for unlimited.between 0 and 100.
		int32_t maxAge{ 0 };///< Duration of invite in seconds before expiry, or 0 for never.between 0 and 604800 (7 days)	86400 (24 hours).
	};

	/// For deleting the Permission overwrites of a given Channel for a given Role or User. \brief For deleting the Permission overwrites of a given Channel for a given Role or User.
	struct DiscordCoreAPI_Dll DeleteChannelPermissionOverwritesData {
		uint64_t roleOrUserId{};///< The Role or User id for whom to delete the Permission overwrites.
		uint64_t channelId{};///< The id of the Channel to modify permissions for.
		std::string reason{};///< Reason for deleting these permission overwrites.
	};

	/// For following a news Channel. \brief For following a news Channel.
	struct DiscordCoreAPI_Dll FollowNewsChannelData {
		uint64_t targetChannelId{};
		uint64_t channelId{};
	};

	/// For triggering the typing indicator in a given Channel. \brief For triggering the typing indicator in a given Channel.
	struct DiscordCoreAPI_Dll TriggerTypingIndicatorData {
		uint64_t channelId{};
	};

	/// For acquiring a list of Channels from a chosen Guild. \brief For acquiring a list of Channels from a chosen Guild.
	struct DiscordCoreAPI_Dll GetGuildChannelsData {
		uint64_t guildId{};///< The Guild from which to collect the Channels from.
	};

	/// For creating a new Channel within a chosen Guild. \brief For creating a new Channel within a chosen Guild.
	struct DiscordCoreAPI_Dll CreateGuildChannelData {
		std::vector<OverWriteData> permissionOverwrites{};///< Array of overwrite objects	the Channel's permission overwrites.
		int32_t defaultAutoArchiveDuration{ 0 };///< Defualt duration of time before archiving a thread.
		int32_t rateLimitPerUser{ 0 };///< Amount of seconds a user has to wait before sending another message(0 - 21600).
		int32_t userLimit{ 0 };///< The user limit of the voice Channel(voice only).
		int32_t position{ 0 };///< Sorting position of the Channel.
		int32_t bitrate{ 0 };///< The bitrate(in bits) of the voice Channel(voice only).
		std::string reason{};///< Reason for creating the Channel.
		uint64_t parentId{};///< Id of the parent category for a Channel.
		std::string topic{};///< Channel topic(0 - 1024 characters).
		std::string name{};///< The name of the Channel.
		ChannelType type{};///< The type of Channel.
		bool nsfw{ false };///<  Whether the Channel is nsfw.
		uint64_t guildId{};///< The Guild within which to create the Channel.
	};

	/// For modifying the Channel position responseData of a single Channel. \brief For modifying the Channel position responseData of a single Channel.
	struct DiscordCoreAPI_Dll ModifyGuildChannelPositionData {
		bool lockPermissions{ false };///< Syncs the permission overwrites with the new parent, if moving to a new category.
		int32_t position{ 0 };///< Sorting position of the Channel.
		uint64_t parentId{};///< The new parent ID for the Channel that is moved.
		uint64_t id{};///< Channel id.
	};

	/// For modifying the current positions of one or more Channels in the Guild. \brief For modifying the current positions of one or more Channels in the Guild.
	struct DiscordCoreAPI_Dll ModifyGuildChannelPositionsData {
		std::vector<ModifyGuildChannelPositionData> modifyChannelData{};///< Array of new Channel position's responseData.
		std::string reason{};///< Reason for re-ordering the Channel positions.
		uint64_t guildId{};///< Guild within which to re-order the Channel positions.
	};

	/// For collecting a direct-messaging Channel. \brief For collecting a direct-messaging Channel.
	struct DiscordCoreAPI_Dll CreateDMChannelData {
		uint64_t userId{};///< The User for whom to collect the direct-messaging Channel to.
	};

	/// A Channel object. \brief A Channel object.
	class DiscordCoreAPI_Dll Channel : public ChannelData {
	  public:
		std::unordered_map<uint64_t, UserData> recipients{};///< Recipients, in the case of a group Dm or Dm.
		int32_t defaultAutoArchiveDuration{ 0 };///< Default time it takes to archive a thread.
		ThreadMetadataData threadMetadata{};///< Metadata in the case that this Channel is a Thread.
		TimeStamp lastPinTimestamp{ "" };///< Timestamp of the last pinned Message.
		int32_t videoQualityMode{ 0 };///< Video quality mode.
		int32_t rateLimitPerUser{ 0 };///< Amount of seconds a User has to wait before sending another Message.
		std::string lastMessageId{};///< Id of the last Message.
		ThreadMemberData member{};///< Thread member object for the current User, if they have joined the Thread.
		int32_t messageCount{ 0 };///< An approximate count of Messages in a Thread stops counting at 50.
		std::string permissions{};///< Computed permissions for the invoking user in the channel, including overwrites.
		uint64_t applicationId{};///< Application id of the current application.
		std::string rtcRegion{};///< Real-time clock region.
		int32_t userLimit{ 0 };///< User limit, in the case of voice channels.
		int32_t bitrate{ 0 };///< Bitrate of the Channel, if it is a voice Channel.
		std::string topic{};///< The Channel's topic.
		std::string icon{};///< Icon for the Channel, if applicable.

		Channel& operator=(const nlohmann::json& jsonObjectData) {
			this->parseObject(jsonObjectData, this);
			return *this;
		}

		Channel(const nlohmann::json& jsonObjectData) {
			*this = jsonObjectData;
		}

		Channel& operator=(ChannelData&&);

		Channel(ChannelData&&);

		Channel& operator=(ChannelData&);

		Channel(ChannelData&);

		Channel() = default;

		~Channel() = default;

	  	void parseObjectReal(const nlohmann::json& jsonObjectData, Channel* pDataStructure) {
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
				pDataStructure->type = jsonObjectData["type"].get<DiscordCoreAPI::ChannelType>();
			}

			if (jsonObjectData.contains("guild_id") && !jsonObjectData["guild_id"].is_null()) {
				pDataStructure->guildId = stoull(jsonObjectData["guild_id"].get<std::string>());
			}

			if (jsonObjectData.contains("default_auto_archive_duration") && !jsonObjectData["default_auto_archive_duration"].is_null()) {
				pDataStructure->defaultAutoArchiveDuration = jsonObjectData["default_auto_archive_duration"].get<int32_t>();
			}

			if (jsonObjectData.contains("position") && !jsonObjectData["position"].is_null()) {
				pDataStructure->position = jsonObjectData["position"].get<int32_t>();
			}

			if (jsonObjectData.contains("permission_overwrites") && !jsonObjectData["permission_overwrites"].is_null()) {
				pDataStructure->permissionOverwrites.clear();
				for (auto& value: jsonObjectData["permission_overwrites"]) {
					DiscordCoreAPI::OverWriteData newData{ value };
					uint64_t overWriteId = newData.id;
					pDataStructure->permissionOverwrites[overWriteId] = newData;
				}
			}

			if (jsonObjectData.contains("name") && !jsonObjectData["name"].is_null()) {
				pDataStructure->name = jsonObjectData["name"].get<std::string>();
			}

			if (jsonObjectData.contains("topic") && !jsonObjectData["topic"].is_null()) {
				pDataStructure->topic = jsonObjectData["topic"].get<std::string>();
			}

			if (jsonObjectData.contains("permissions") && !jsonObjectData["permissions"].is_null()) {
				pDataStructure->permissions = jsonObjectData["permissions"].get<std::string>();
			}

			if (jsonObjectData.contains("nsfw") && !jsonObjectData["nsfw"].is_null()) {
				pDataStructure->flags =
					DiscordCoreAPI::setBool<int8_t, DiscordCoreAPI::ChannelFlags>(pDataStructure->flags, DiscordCoreAPI::ChannelFlags::NSFW, jsonObjectData["nsfw"].get<bool>());
			}

			if (jsonObjectData.contains("last_message_id") && !jsonObjectData["last_message_id"].is_null()) {
				pDataStructure->lastMessageId = jsonObjectData["last_message_id"].get<std::string>();
			}

			if (jsonObjectData.contains("bitrate") && !jsonObjectData["bitrate"].is_null()) {
				pDataStructure->bitrate = jsonObjectData["bitrate"].get<int32_t>();
			}

			if (jsonObjectData.contains("user_limit") && !jsonObjectData["user_limit"].is_null()) {
				pDataStructure->userLimit = jsonObjectData["user_limit"].get<int32_t>();
			}

			if (jsonObjectData.contains("rate_limit_per_user") && !jsonObjectData["rate_limit_per_user"].is_null()) {
				pDataStructure->rateLimitPerUser = jsonObjectData["rate_limit_per_user"].get<int32_t>();
			}

			if (jsonObjectData.contains("recipients") && !jsonObjectData["recipients"].is_null()) {
				pDataStructure->recipients.clear();
				for (auto& value: jsonObjectData["recipients"]) {
					DiscordCoreAPI::UserData newData{ value };
					uint64_t userId = newData.id;
					pDataStructure->recipients[userId] = newData;
				}
			}

			if (jsonObjectData.contains("icon") && !jsonObjectData["icon"].is_null()) {
				pDataStructure->icon = jsonObjectData["icon"].get<std::string>();
			}

			if (jsonObjectData.contains("owner_id") && !jsonObjectData["owner_id"].is_null()) {
				pDataStructure->ownerId = stoull(jsonObjectData["owner_id"].get<std::string>());
			}

			if (jsonObjectData.contains("application_id") && !jsonObjectData["application_id"].is_null()) {
				pDataStructure->applicationId = stoull(jsonObjectData["application_id"].get<std::string>());
			}

			if (jsonObjectData.contains("parent_id") && !jsonObjectData["parent_id"].is_null()) {
				if (jsonObjectData["parent_id"].is_string()) {
					pDataStructure->parentId = stoull(jsonObjectData["parent_id"].get<std::string>());
				} else {
					pDataStructure->parentId = jsonObjectData["parent_id"].get<int64_t>();
				}
			}

			if (jsonObjectData.contains("last_pin_timestamp") && !jsonObjectData["last_pin_timestamp"].is_null()) {
				pDataStructure->lastPinTimestamp = jsonObjectData["last_pin_timestamp"].get<std::string>();
			}

			if (jsonObjectData.contains("rtc_region") && !jsonObjectData["rtc_region"].is_null()) {
				pDataStructure->rtcRegion = jsonObjectData["rtc_region"].get<std::string>();
			}

			if (jsonObjectData.contains("video_quality_mode") && !jsonObjectData["video_quality_mode"].is_null()) {
				pDataStructure->videoQualityMode = jsonObjectData["video_quality_mode"].get<int32_t>();
			}

			if (jsonObjectData.contains("message_count") && !jsonObjectData["message_count"].is_null()) {
				pDataStructure->messageCount = jsonObjectData["message_count"].get<int32_t>();
			}

			if (jsonObjectData.contains("member_count") && !jsonObjectData["member_count"].is_null()) {
				pDataStructure->memberCount = jsonObjectData["member_count"].get<int32_t>();
			}

			if (jsonObjectData.contains("thread_metadata") && !jsonObjectData["thread_metadata"].is_null()) {
				pDataStructure->threadMetadata = jsonObjectData["thread_metadata"];
			}

			if (jsonObjectData.contains("member") && !jsonObjectData["member"].is_null()) {
				pDataStructure->member = jsonObjectData["member"];
			}
		}
	};

	/// For modifying a Channel's properties. \brief For modifying a Channel's properties.
	struct DiscordCoreAPI_Dll ModifyChannelData {
		ModifyChannelData(Channel newData) {
			this->channelData.nsfw = getBool<int8_t, ChannelFlags>(newData.flags, ChannelFlags::NSFW);
			this->channelData.permissionOverwrites = newData.permissionOverwrites;
			this->channelData.rateLimitPerUser = newData.rateLimitPerUser;
			this->channelData.userLimit = newData.userLimit;
			this->channelData.rtcRgion = newData.rtcRegion;
			this->channelData.parentId = newData.parentId;
			this->channelData.position = newData.position;
			this->channelData.topic = newData.topic;
			this->channelData.name = newData.name;
			this->channelData.type = newData.type;
		};
		UpdateChannelData channelData{};///< The responseData of the Channel to be updated.
		uint64_t channelId{};///< The id of the Channel to modify.
		std::string reason{};///< A reason for modifying the Channel.
	};

	/**@}*/

	/**
	 * \addtogroup main_endpoints
	 * @{
	 */
	/// An interface class for the Channel related endpoints. \brief An interface class for the Channel-related endpoints.
	class DiscordCoreAPI_Dll Channels {
	  public:
		friend class DiscordCoreInternal::DataParser;
		friend DiscordCoreClient;
		friend EventHandler;
		friend Guild;

		static void initialize(DiscordCoreInternal::HttpsClient*, ConfigManager* configManager);

		/// Collects a Channel from the Discord servers. \brief Collects a Channel from the Discord servers.
		/// \param dataPackage A GetChannelData structure.
		/// \returns A CoRoutine containing a Channel.
		static CoRoutine<Channel> getChannelAsync(GetChannelData dataPackage);

		/// Collects a Channel from the library's cache. \brief Collects a Channel from the library's cache.
		/// \param dataPackage A GetChannelData structure.
		/// \returns A CoRoutine containing a Channel.
		static CoRoutine<ChannelData> getCachedChannelAsync(GetChannelData dataPackage);

		/// Modifies a Channel's properties. \brief Modifies a Channel's properties.
		/// \param dataPackage A ModifyChannelData structure.
		/// \returns A CoRoutine containing a Channel.
		static CoRoutine<Channel> modifyChannelAsync(ModifyChannelData dataPackage);

		/// Delete a Channel, or close a private message. \brief Delete a Channel, or close a private message.
		/// \param dataPackage A DeleteOrCloseChannelData structure.
		/// \returns A CoRoutine containing a Channel.
		static CoRoutine<void> deleteOrCloseChannelAsync(DeleteOrCloseChannelData dataPackage);

		/// Edit the given Permissions overwrites for a given User or Role. \brief Edit the given Permissions overwrites for a given User or Role.
		/// \param dataPackage An EditChannelPermissionOverwritesData structure.
		/// \returns A CoRoutine containing void.
		static CoRoutine<void> editChannelPermissionOverwritesAsync(EditChannelPermissionOverwritesData dataPackage);

		/// Collects a std::vector of the invites to a given Channel. \brief Collects a std::vector of the invites to a given Channel.
		/// \param dataPackage A GetChannelInvitesData structure.
		/// \returns A CoRoutine containing a std::vector<InviteData>.
		static CoRoutine<std::vector<InviteData>> getChannelInvitesAsync(GetChannelInvitesData dataPackage);

		/// Creates an invite to a selected Channel. \brief Creates an invite to a selected Channel.
		/// \param dataPackage A CreateChannelInviteData structure.
		/// \returns A CoRoutine containing an InviteData.
		static CoRoutine<InviteData> createChannelInviteAsync(CreateChannelInviteData dataPackage);

		/// Delete the given Permissions overwrites for a given User or Role. \brief Delete the given Permissions overwrites for a given User or Role.
		/// \param dataPackage A DeleteChannelPermissionOverwritesData structure
		/// \returns A CoRoutine containing void.
		static CoRoutine<void> deleteChannelPermissionOverwritesAsync(DeleteChannelPermissionOverwritesData dataPackage);

		/// Follows a given new Channel with another Channel. \brief Follows a given new Channel with another Channel.
		/// \param dataPackage A FollowNewsChannelData structure.
		/// \returns A CoRoutine containing a Channel.
		static CoRoutine<Channel> followNewsChannelAsync(FollowNewsChannelData dataPackage);

		/// Triggers the typing indicator for the bot in the given Channel. \brief Triggers the typing indicator for the bot in the given Channel.
		/// \param dataPackage A TriggerTypingIndicatorData structure.
		/// \returns A CoRoutine containing void.
		static CoRoutine<void> triggerTypingIndicatorAsync(TriggerTypingIndicatorData dataPackage);

		/// Collects a list of Channels from a chosen Guild. \brief Collects a list of Channels from a chosen Guild.
		/// \param dataPackage A GetGuildChannelsData structure.
		/// \returns A CoRoutine containing a std::vector<Channel>.
		static CoRoutine<std::vector<Channel>> getGuildChannelsAsync(GetGuildChannelsData dataPackage);

		/// Creates a new Channel within a chosen Guild. \brief Creates a new Channel within a chosen Guild.
		/// \param dataPackage A CreateGuildChannelData structure.
		/// \returns A CoRoutine containing a std::vector<Channel>.
		static CoRoutine<Channel> createGuildChannelAsync(CreateGuildChannelData dataPackage);

		/// Re-orders the Channel positions, within a chosen Guild. \brief Re-orders the Channel positions, within a chosen Guild.
		/// \param dataPackage A ModifyGuildChannelPositionsData  structure.
		/// \returns A CoRoutine containing void.
		static CoRoutine<void> modifyGuildChannelPositionsAsync(ModifyGuildChannelPositionsData dataPackage);

		/// Collect a direct-Message Channel between the bot and the User. \brief Collect a direct-Message Channel between the bot and the User.
		/// \param dataPackage A CreateDMChannelData structure.
		/// \returns A CoRoutine containing a Channel.
		static CoRoutine<Channel> createDMChannelAsync(CreateDMChannelData dataPackage);

		/// Collect a list of voice regions that are usable for the RTC-Region option of a given Channel.
		/// \brief Collect a list of voice regions that are usable for the RTC-Region option of a given Channel.
		/// \returns A CoRoutine containing a std::vector<VoiceRegionData>.
		static CoRoutine<std::vector<VoiceRegionData>> getVoiceRegionsAsync();

		static void insertChannel(ChannelData dataPackage);

		static void removeChannel(const uint64_t& channelId);

	  protected:
		static std::unique_ptr<std::unordered_map<uint64_t, std::unique_ptr<ChannelData>>> cache;
		static DiscordCoreInternal::HttpsClient* httpsClient;
		static ConfigManager* configManager;
		static std::shared_mutex theMutex;
	};
	/**@}*/
}// namespace DiscordCoreAPI
