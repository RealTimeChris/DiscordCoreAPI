// GuildEntities.hpp - Header for the Guild classes and structs.
// May 12, 2021
// Chris M.
// https://github.com/RealTimeChris

#pragma once

#include "IndexInitial.hpp"
#include "FoundationEntities.hpp"

namespace DiscordCoreAPI {

	/**
	* \addtogroup foundation_entities
	* @{
	*/

	enum class AfkTimeOutDurations {
		SHORTEST = 60,
		SHORT = 300,
		MEDIUM = 900,
		LONG = 1800,
		LONGEST = 3600
	};

	/// For creating a Guild. \brief For creating a Guild.
	struct DiscordCoreAPI_Dll CreateGuildData {
		DefaultMessageNotificationLevel defaultMessageNotifications{};///< Default message notification level.
		AfkTimeOutDurations afkTimeout{ AfkTimeOutDurations::SHORTEST };///< Afk timeout in seconds.
		ExplicitContentFilterLevel explicitContentFilter{};///< Explicit content filter level.
		__int32 systemChannelFlags{ 0 };///< System channel flags.
		vector<ChannelData> channels{};///< Array of partial channel objects.
		__int32 verificationLevel{ 0 };///< Verification level.
		vector<unsigned __int8> icon{};///< base64 128x128 image for the guild icon.
		string systemChannelId{ "" };///< The id of the channel where guild notices such as welcome messages and boost events are posted.
		string afkChannelId{ "" };///< Id for afk channel.
		vector<RoleData> roles{};///< Array of role objects.
		string name{ "" };///< The name of the new Guild.
	};

	/// For getting a Guild from the library's cache or a Discord server. \brief For getting a Guild from the library's cache or a Discord server.
	struct DiscordCoreAPI_Dll GetGuildData {
		string guildId{ "" };///< The id of the Guild to acquire.
	};

	/// For acquiring a Guild preview of a chosen Guild. \brief For acquiring a Guild preview of a chosen Guild.
	struct GetGuildPreviewData {
		string guildId{ "" };///< The id of the Guild's preview to acquire.
	};

	/// For banning a current GuildMember. \brief For banning a current GuildMember.
	struct DiscordCoreAPI_Dll CreateGuildBanData {
		__int32 deleteMessageDays{ 0 };///< The number of days of their Messages to delete.
		string guildMemberId{ "" };///< The id of the member to be banned.
		string guildId{ "" };///< The id of the Guild from which to ban the member.
		string reason{ "" };///< The reason for the ban.
	};

	/// For geting all of the current invites from a Guild. \brief For geting all of the current invites from a Guild.
	struct DiscordCoreAPI_Dll GetInvitesData {
		string guildId{ "" };///< The id of the Guild you wish to acquire.
	};

	/// For geting a single invite's data from a Guild. \brief For geting a single invite's data from a Guild.
	struct DiscordCoreAPI_Dll GetInviteData {
		string inviteId{ "" };///< The id of the invite you wish to acquire.
	};

	/// For geting the vanity invite data of a Guild. \brief For geting the vanity invite data of a Guild.
	struct DiscordCoreAPI_Dll GetVanityInviteData {
		string guildId{ "" };///< The id of the Guild to acquire the vanity invite from.
	};
	/**@}*/

	/**
	* \addtogroup voice_connection
	* @{
	*/
	/// A discord Guild. Used to connect to/disconnect from voice. \brief A discord Guild. Used to connect to/disconnect from voice.
	class DiscordCoreAPI_Dll Guild : public GuildData {
	public:

		friend struct Concurrency::details::_ResultHolder<Guild>;
		friend class DiscordCoreInternal::HttpRequestAgent;
		template<typename returnValueType>
		friend class DiscordCoreAPI::CoRoutine;
		friend struct OnGuildCreationData;
		friend struct OnGuildDeletionData;
		friend struct OnGuildUpdateData;
		friend class DiscordCoreClient;
		friend class Guilds;

		/// Connects to a given voice Channel. \brief Connects to a given voice Channel.
		/// \param channelId The voice Channel's id to connect to.
		/// \param connectionData A ConnectionWebSocketDatam structure.
		/// \returns A shared_ptr containing the voice connection.
		VoiceConnection* connectToVoice(string channelId, DiscordCoreInternal::ConnectionWebSocketData* connectionData = nullptr);

		/// Disconnects from a voice Channel. \brief Disconnects from a voice Channel.
		/// \returns void
		void disconnect();

		/// Checks if we are currently connected to a voice Channel. \brief Checks if we are currently connected to a voice Channel.
		/// \returns A bool telling us if we are connected.
		bool areWeConnected();

	protected:

		bool areWeConnectedBool{ false };

		Guild();

		Guild(GuildData dataNew);

		void initialize();
	};
	/**@}*/

	/**
	* \addtogroup discord_core_client
	* @{
	*/
	/// An interface class for the Guild related Discord endpoints. \brief An interface class for the Guild related Discord endpoints.
	class DiscordCoreAPI_Dll Guilds {
	public:

		friend class DiscordCoreClient;
		friend class EventHandler;

		/// Creates a new Guild. \brief Creates a new Guild.
		/// \param dataPackage A CreateGuildData structure.
		/// \returns A CoRoutine containing a Guild.
		static CoRoutine<Guild> createGuildAsync(CreateGuildData dataPackage);

		/// Acquires the preview Data of a chosen Guild. \brief Acquires the preview Data of a chosen Guild.
		/// \param dataPackage A GetGuildPreviewData structure.
		/// \returns A CoRoutine containing a GuildPreviewData.
		static CoRoutine<GuildPreviewData> getGuildPreviewAsync(GetGuildPreviewData dataPackage);

		/// Getes an audit log from the Discord servers. \brief Getes an audit log from the Discord servers.
		/// \param dataPackage A GetAuditLogData structure.
		/// \returns A CoRoutine containing AuditLogData.
		static CoRoutine<AuditLogData> getAuditLogDataAsync(GetAuditLogData dataPackage);

		/// Collects a Guild from the library's cache. \brief Collects a Guild from the library's cache.
		/// \param dataPackage A GetGuildData structure.
		/// \returns A CoRoutine containing a Guild.
		static CoRoutine<Guild> getCachedGuildAsync(GetGuildData dataPackage);

		/// Collects a Guild from the Discord servers. \brief Collects a Guild from the Discord servers.
		/// \param dataPackage A GetGuildData structure.
		/// \returns A CoRoutine containing a Guild.
		static CoRoutine<Guild> getGuildAsync(GetGuildData dataPackage);

		/// Getes an invite from the Discord servers. \brief Getes an invite from the Discord servers.
		/// \param dataPackage A GetInviteData structure.
		/// \returns A CoRoutine containing InviteData.
		static CoRoutine<InviteData> getInviteAsync(GetInviteData dataPackage);

		/// Getes multiple invites from the Discord servers. \brief Getes multiple invites from the Discord servers.
		/// \param dataPackage A GetInvitesData structure.
		/// \returns A CoRoutine containing a vector of InviteData.
		static CoRoutine<vector<InviteData>> getInvitesAsync(GetInvitesData dataPackage);

		/// Getes the vanity invite data from a particular server. \brief Getes the vanity invite data from a particular server.
		/// \param dataPackage A GetVanityInviteData structure.
		/// \returns A CoRoutine containing InviteData.
		static CoRoutine<InviteData> getVanityInviteAsync(GetVanityInviteData dataPackage);

		/// Bans a GuildMember. \brief Bans a GuildMember.
		/// \param dataPackage A CreateGuildBanData structure.
		/// \returns A CoRoutine containing BanData.
		static CoRoutine<BanData> createGuildBanAsync(CreateGuildBanData dataPackage);

		/// Returns all of the Guilds that the current bot is in. \brief Returns all of the Guilds that the current bot is in.
		/// \returns A CoRoutine containing a vector of Guild.
		static CoRoutine<vector<Guild>> getAllGuildsAsync();

	protected:
		static shared_ptr<concurrent_unordered_map<string, DiscordCoreAPI::Guild>> cache;

		static void insertGuild(Guild Guild);

		static void removeGuild(string GuildId);

	};
	/**@}*/

}