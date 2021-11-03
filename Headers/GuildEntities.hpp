// GuildEntities.hpp - Header for the Guild classes and structs.
// May 12, 2021
// Chris M.
// https://github.com/RealTimeChris

#pragma once

#include "IndexInitial.hpp"
#include "VoiceConnection.hpp"
#include "DataParsingFunctions.hpp"
#include "Http.hpp"
#include "ChannelEntities.hpp"
#include "GuildMemberEntities.hpp"
#include "UserEntities.hpp"
#include "RoleEntities.hpp"

namespace DiscordCoreAPI {

	/**
	* \addtogroup foundation_entities
	* @{
	*/
	/// For getting a Guild from the library's cache or a Discord server. \brief For getting a Guild from the library's cache or a Discord server.
	struct DiscordCoreAPI_Dll GetGuildData {
		string guildId{ "" };///< The id of the Guild to acquire.
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
		friend class DiscordCoreInternal::GuildManager;
		template<typename returnValueType>
		friend class DiscordCoreAPI::CoRoutine;
		friend struct OnGuildCreationData;
		friend struct OnGuildDeletionData;
		friend struct OnGuildUpdateData;
		friend class DiscordCoreClient;

		/// Connects to a given voice Channel. \brief Connects to a given voice Channel.
		/// \param channelId The voice Channel's id to connect to.
		/// \returns A shared_ptr containing the voice connection.
		VoiceConnection* connectToVoice(string channelId);

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
};

namespace DiscordCoreInternal {

	class DiscordCoreAPI_Dll GuildManager {
	public:

		friend class DiscordCoreAPI::DiscordCoreClient;
		friend class DiscordCoreAPI::Guilds;

	protected:

		overwrite_buffer<map<string, DiscordCoreAPI::Guild>> cache{};

		DiscordCoreAPI::CoRoutine<DiscordCoreAPI::AuditLogData> getAuditLogDataAsync(DiscordCoreAPI::GetAuditLogData dataPackage);

		DiscordCoreAPI::CoRoutine<vector<DiscordCoreAPI::Guild>> getAllGuildsAsync();

		DiscordCoreAPI::CoRoutine<DiscordCoreAPI::Guild> getGuildAsync(DiscordCoreAPI::GetGuildData dataPackage);

		DiscordCoreAPI::CoRoutine<DiscordCoreAPI::Guild> getCachedGuildAsync(DiscordCoreAPI::GetGuildData dataPackage);

		DiscordCoreAPI::CoRoutine<DiscordCoreAPI::BanData> createGuildBanAsync(DiscordCoreAPI::CreateGuildBanData dataPackage);

		DiscordCoreAPI::CoRoutine<vector<DiscordCoreAPI::InviteData>> getInvitesAsync(DiscordCoreAPI::GetInvitesData dataPackage);

		DiscordCoreAPI::CoRoutine<DiscordCoreAPI::InviteData> getInviteAsync(DiscordCoreAPI::GetInviteData dataPackage);

		DiscordCoreAPI::CoRoutine<DiscordCoreAPI::InviteData> getVanityInviteAsync(DiscordCoreAPI::GetVanityInviteData dataPackage);

		void insertGuild(DiscordCoreAPI::Guild Guild);

		void removeGuild(string guildId);

		void clearGuilds();
	};
}