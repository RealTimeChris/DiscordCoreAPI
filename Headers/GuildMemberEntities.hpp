// GuildMemberEntities.hpp - Header for the GuildMember related classes and structs.
// May 13, 2021
// Chris M.
// https://github.com/RealTimeChris

#pragma once

#ifndef _GUILD_MEMBER_ENTITIES
#define _GUILD_MEMBER_ENTITIES

#include "IndexInitial.hpp"
#include "FoundationEntities.hpp"
#include "CoRoutine.hpp"

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
	protected:

		friend struct Concurrency::details::_ResultHolder<GuildMember>;
		friend class DiscordCoreInternal::GuildMemberManagerAgent;
		friend class DiscordCoreInternal::GuildMemberManager;
		template <typename returnVal>
		friend struct DiscordCoreAPI::CoRoutine;
		friend struct GetGuildMemberRolesData;
		friend struct OnGuildMemberUpdateData;
		friend struct OnGuildMemberAddData;
		friend class DiscordCoreClient;
		friend class Guild;

		GuildMember();

		GuildMember(GuildMemberData dataNew, string guildIdNew);
	};
	/**@}*/
};

namespace DiscordCoreInternal {

	class DiscordCoreAPI_Dll GuildMemberManagerAgent :  agent {
	protected:

		friend class DiscordCoreAPI::DiscordCoreClient;
		friend class DiscordCoreAPI::EventHandler;
		friend class GuildMemberManager;

		static  shared_ptr<ThreadContext> threadContext;

		unbounded_buffer<PatchGuildMemberData> requestPatchGuildMemberBuffer{ nullptr };
		unbounded_buffer<DiscordCoreAPI::GuildMember> outGuildMemberBuffer{ nullptr };
		unbounded_buffer<GetGuildMemberData> requestGetGuildMemberBuffer{ nullptr };
		unbounded_buffer<GetGuildMemberRolesData> requestGetRolesBuffer{ nullptr };

		GuildMemberManagerAgent();

		static void initialize();

		DiscordCoreAPI::GuildMember getObjectData(GetGuildMemberData dataPackage);

		DiscordCoreAPI::GuildMember patchObjectData(PatchGuildMemberData dataPackage);

		void run();
	};

	class DiscordCoreAPI_Dll GuildMemberManager {
	public:

		friend class DiscordCoreAPI::DiscordCoreClient;
		friend class DiscordCoreAPI::GuildMembers;
		friend class DiscordCoreAPI::Guild;

		GuildMemberManager(GuildMemberManager* pointer);

	protected:

		shared_ptr<overwrite_buffer<map<string, DiscordCoreAPI::GuildMember>>> cache{};

		DiscordCoreAPI::CoRoutine<DiscordCoreAPI::GuildMember> getGuildMemberAsync(DiscordCoreAPI::GetGuildMemberData dataPackage);

		DiscordCoreAPI::CoRoutine<DiscordCoreAPI::GuildMember> getCachedGuildMemberAsync(DiscordCoreAPI::GetGuildMemberData dataPackage);

		DiscordCoreAPI::CoRoutine<DiscordCoreAPI::GuildMember> modifyGuildMemberAsync(DiscordCoreAPI::ModifyGuildMemberData dataPackage);

		void insertGuildMember(DiscordCoreAPI::GuildMember guildMember, string guildId);

		void removeGuildMember(string guildId, string guildMemberId);
	};
};
#endif