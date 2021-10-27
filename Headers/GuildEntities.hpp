// GuildEntities.hpp - Header for the Guild classes and structs.
// May 12, 2021
// Chris M.
// https://github.com/RealTimeChris

#pragma once

#ifndef _GUILD_ENTITIES_
#define _GUILD_ENTITIES_

#include "IndexInitial.hpp"
#include "VoiceConnection.hpp"
#include "DataParsingFunctions.hpp"
#include "Http.hpp"
#include "ChannelEntities.hpp"
#include "MessageEntities.hpp"
#include "GuildMemberEntities.hpp"
#include "UserEntities.hpp"
#include "RoleEntities.hpp"
#include "CoRoutine.hpp"

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

	/// For geting a Guild's audit logs. \brief For geting a Guild's audit logs.
	struct DiscordCoreAPI_Dll GetAuditLogData {
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
		friend class DiscordCoreInternal::GuildManagerAgent;
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
		shared_ptr<VoiceConnection> connectToVoice(string channelId);
		
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

	class DiscordCoreAPI_Dll GuildManagerAgent : public ThreadContext, public agent {
	protected:

		friend class DiscordCoreAPI::DiscordCoreClient;
		friend class DiscordCoreAPI::EventHandler;
		friend class GuildManager;

		unbounded_buffer<vector<DiscordCoreAPI::InviteData>> outInvitesBuffer{ nullptr };
		unbounded_buffer<GetVanityInviteData> requestGetVanityInviteBuffer{ nullptr };
		unbounded_buffer<DiscordCoreAPI::AuditLogData> outAuditLogBuffer{ nullptr };
		unbounded_buffer<DiscordCoreAPI::InviteData> outInviteBuffer{ nullptr };
		unbounded_buffer<PutGuildBanData> requestPutGuildBanBuffer{ nullptr };
		unbounded_buffer<GetAuditLogData> requestGetAuditLogBuffer{ nullptr };
		unbounded_buffer<GetInvitesData> requestGetInvitesBuffer{ nullptr };
		unbounded_buffer<DiscordCoreAPI::BanData> outBanBuffer{ nullptr };
		unbounded_buffer<DiscordCoreAPI::Guild> outGuildBuffer{ nullptr };
		unbounded_buffer<GetInviteData> requestGetInviteBuffer{ nullptr };
		unbounded_buffer<GetGuildData> requestGetGuildBuffer{ nullptr };

		GuildManagerAgent();

		static void initialize();

		DiscordCoreAPI::AuditLogData getObjectData(GetAuditLogData dataPackage);
		
		DiscordCoreAPI::Guild getObjectData(GetGuildData dataPackage);

		DiscordCoreAPI::InviteData getObjectData(GetInviteData dataPackage);

		vector<DiscordCoreAPI::InviteData> getObjectData(GetInvitesData dataPackage);

		DiscordCoreAPI::InviteData getObjectData(GetVanityInviteData dataPackage);

		DiscordCoreAPI::BanData putObjectData(PutGuildBanData dataPackage);

		void run();

	};

	class DiscordCoreAPI_Dll GuildManager { 
	public:

		friend class DiscordCoreAPI::PermissionsConverter;
		friend class DiscordCoreAPI::DiscordCoreClient;
		friend class DiscordCoreAPI::YouTubeAPI;
		friend class DiscordCoreAPI::Guilds;

		GuildManager(GuildManager* pointer);

		~GuildManager();

	protected:

		shared_ptr<overwrite_buffer<map<string, DiscordCoreAPI::Guild>>> cache{};

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
	};
}
#endif