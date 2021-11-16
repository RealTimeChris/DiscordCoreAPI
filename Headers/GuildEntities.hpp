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
	struct DiscordCoreAPI_Dll GetGuildPreviewData {
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

	///	For deleting a Guild. \brief For deleting a Guild.
	struct DiscordCoreAPI_Dll DeleteGuildData {
		string guildId{ "" };///< The Guild you would like to delete.
	};

	/// For geting a Guild's audit logs. \brief For geting a Guild's audit logs.
	struct DiscordCoreAPI_Dll GetGuildAuditLogsData {
		AuditLogEvent actionType{};///< The action type to acquire audit-logs for.
		string guildId{ "" };///< The guiild id for the Guild which you wish to query the log of.
		string userId{ "" };///< The User for whom to look for the actions of.
		__int32 limit{ 0 };///< The maximum number of actions to acquire from the log.
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
		friend class CoRoutine;
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

	/// For modifying the properties of a chosen Guild. \brief For modifying the properties of a chosen Guild.
	struct DiscordCoreAPI_Dll ModifyGuildData {
		ModifyGuildData(Guild dataPackage) {
			this->defaultMessageNotifications = dataPackage.defaultMessageNotifications;
			this->publicUpdatesChannelId = dataPackage.publicUpdatesChannelId;
			this->explicitContentFilter = dataPackage.explicitContentFilter;
			this->systemChannelFlags = dataPackage.systemChannelFlags;
			this->verificationLevel = dataPackage.verificationLevel;
			this->preferredLocale = dataPackage.preferredLocale;
			this->systemChannelId = dataPackage.systemChannelId;
			this->rulesChannelId = dataPackage.rulesChannelId;
			this->afkChannelId = dataPackage.afkChannelId;
			this->description = dataPackage.description;
			this->afkTimeout = dataPackage.afkTimeOut;
			this->features = dataPackage.features;
			this->ownerId = dataPackage.ownerId;
			this->guildId = dataPackage.id;
			this->name = dataPackage.name;
		}
		DefaultMessageNotificationLevel defaultMessageNotifications{};///< Default message notification level.
		ExplicitContentFilterLevel explicitContentFilter{};///< Explicit content filter level.
		vector<unsigned __int8> discoverySplash{};/// Base64 16 : 9 png / jpeg image for the guild discovery splash(when the server has the DISCOVERABLE feature).
		VerificationLevel verificationLevel{};///< Verification level.
		string publicUpdatesChannelId{ "" };///< The id of the channel where admins and moderators of Community guilds receive notices from Discord.
		vector<unsigned __int8> splash{};///< Base64 16 : 9 png / jpeg image for the guild splash(when the server has the INVITE_SPLASH feature).
		AfkTimeOutDurations afkTimeout{};///< Afk timeout in seconds.
		vector<unsigned __int8> banner{};///< Base64 16 : 9 png / jpeg image for the guild banner(when the server has the BANNER feature).
		__int32 systemChannelFlags{ 0 };///< System channel flags.
		vector<unsigned __int8> icon{};///< Base64 1024x1024 png / jpeg / gif image for the guild icon(can be animated gif when the server has the ANIMATED_ICON feature).
		string preferredLocale{ "" };///< The preferred locale of a Community guild used in server discovery and notices from Discord; defaults to "en-US".
		string systemChannelId{ "" };///< The id of the channel where guild notices such as welcome messages and boost events are posted.
		string rulesChannelId{ "" };///< The id of the channel where Community guilds display rules and /or guidelines.
		string afkChannelId{ "" };///< Id for afk channels.
		vector<string> features{};///< Array of guild feature strings enabled guild features.
		string description{ "" };///< The description for the guild, if the guild is discoverable.
		string ownerId{ "" };///< User id to transfer guild ownership to(must be owner).
		string guildId{ "" };///< Id of the chosen Guild to modify.
		string reason{ "" };///< Reason for modifying the Guild.
		string name{ "" };///< Desired name of the Guild.		
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

		/// Getes an audit log from the Discord servers. \brief Getes an audit log from the Discord servers.
		/// \param dataPackage A GetGuildAuditLogsData structure.
		/// \returns A CoRoutine containing AuditLogData.
		static CoRoutine<AuditLogData> getGuildAuditLogsAsync(GetGuildAuditLogsData dataPackage);

		/// Creates a new Guild. \brief Creates a new Guild.
		/// \param dataPackage A CreateGuildData structure.
		/// \returns A CoRoutine containing a Guild.
		static CoRoutine<Guild> createGuildAsync(CreateGuildData dataPackage);

		/// Acquires the preview Data of a chosen Guild. \brief Acquires the preview Data of a chosen Guild.
		/// \param dataPackage A GetGuildPreviewData structure.
		/// \returns A CoRoutine containing a GuildPreviewData.
		static CoRoutine<GuildPreviewData> getGuildPreviewAsync(GetGuildPreviewData dataPackage);

		/// Collects a Guild from the library's cache. \brief Collects a Guild from the library's cache.
		/// \param dataPackage A GetGuildData structure.
		/// \returns A CoRoutine containing a Guild.
		static CoRoutine<Guild> getCachedGuildAsync(GetGuildData dataPackage);

		/// Collects a Guild from the Discord servers. \brief Collects a Guild from the Discord servers.
		/// \param dataPackage A GetGuildData structure.
		/// \returns A CoRoutine containing a Guild.
		static CoRoutine<Guild> getGuildAsync(GetGuildData dataPackage);

		/// Modifies a chosen Guild's properties. \brief Modifies a chosen Guild's properties.
		/// \param dataPackage A ModifyGuildData structure.
		/// \returns A CoRoutine containing a Guild.
		static CoRoutine<Guild> modifyGuildAsync(ModifyGuildData dataPackage);

		/// Deletes a chosen Guild. \brief Deletes a chosen Guild.
		/// \param dataPackage A DeleteGuildData structure.
		/// \returns A CoRoutine containing void.
		static CoRoutine<void> deleteGuildAsync(DeleteGuildData dataPackage);

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
		static shared_ptr<concurrent_unordered_map<string, Guild>> cache;

		static void insertGuild(Guild Guild);

		static void removeGuild(string GuildId);

	};
	/**@}*/

}