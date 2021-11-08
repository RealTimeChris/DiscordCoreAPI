// GuildMemberEntities.hpp - Header for the GuildMember related classes and structs.
// May 13, 2021
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
	/// For modifying a GuildMember's values. \brief For modifying a GuildMember's values.
	struct DiscordCoreAPI_Dll ModifyGuildMemberData {
		string newVoiceChannelId{ "" }; ///< The new voice channel to move them into.
		string currentChannelId{ "" }; ///< The current voice channel, if applicaple.
		string guildMemberId{ "" };	///< The user id of the desired Guild memeber.
		vector<string> roleIds{}; ///<A collection of role id's to be applied to them.
		string guildId{ "" }; ///< The id of the Guild for which you would like to modify a member.
		bool mute{ false }; ///< Whether or not to mute them in voice.
		bool deaf{ false };	///< Whether or not to deafen them, in voice.
		string nick{ "" };	///< Their new display/nick name.
	};

	/// For getting a GuildMember, from the library's cache or Discord server. \brief For getting a GuildMember, from the library's cache or Discord server.
	struct DiscordCoreAPI_Dll GetGuildMemberData {
		string guildMemberId{ "" };///< The user id of the desired GuildMember.
		string guildId{ "" };///< The id of the Guild from which you would like to acquire a member.
	};

	/// A single GuildMember. \brief A single GuildMember.
	class DiscordCoreAPI_Dll GuildMember : public GuildMemberData {
	public:
		GuildMember(GuildMemberData dataNew, string guildIdNew);

	protected:

		friend struct Concurrency::details::_ResultHolder<GuildMember>;
		friend class DiscordCoreInternal::GuildMemberManager;
		friend class DiscordCoreInternal::HttpRequestAgent;
		template <typename returnVal>
		friend class DiscordCoreAPI::CoRoutine;
		friend struct GetGuildMemberRolesData;
		friend struct OnGuildMemberUpdateData;
		friend struct OnGuildMemberAddData;
		friend class DiscordCoreClient;
		friend class EventHandler;
		friend class Guild;

		GuildMember();
	};
	/**@}*/
};

namespace DiscordCoreInternal {

	class DiscordCoreAPI_Dll GuildMemberManager {
	public:

		friend class DiscordCoreAPI::DiscordCoreClient;
		friend class DiscordCoreAPI::GuildMembers;
		friend class DiscordCoreAPI::Guild;

	protected:

		shared_ptr<concurrent_unordered_map<string, DiscordCoreAPI::GuildMember>> cache{ new concurrent_unordered_map<string, DiscordCoreAPI::GuildMember>() };

		DiscordCoreAPI::CoRoutine<DiscordCoreAPI::GuildMember> getGuildMemberAsync(DiscordCoreAPI::GetGuildMemberData dataPackage);

		DiscordCoreAPI::CoRoutine<DiscordCoreAPI::GuildMember> getCachedGuildMemberAsync(DiscordCoreAPI::GetGuildMemberData dataPackage);

		DiscordCoreAPI::CoRoutine<DiscordCoreAPI::GuildMember> modifyGuildMemberAsync(DiscordCoreAPI::ModifyGuildMemberData dataPackage);

		void insertGuildMember(DiscordCoreAPI::GuildMember guildMember);

		void removeGuildMember(DiscordCoreAPI::GuildMember guildMember);
	};
};