// DiscordCoreClientBase.hpp - Header for the DiscordCoreClientBase class.
// Aug 7, 2021
// Chris M.
// https://github.com/RealTimeChris

#pragma once

#ifndef _DISCORD_CORE_CLIENT_BASE_
#define _DISCORD_CORE_CLIENT_BASE_

#include "../pch.h"
#include "UserStuff.hpp"
#include "RoleStuff.hpp"
#include "ChannelStuff.hpp"
#include "MessageStuff.hpp"
#include "GuildMemberStuff.hpp"

namespace DiscordCoreAPI {

	class DiscordCoreClientBase {
	public:
		shared_ptr<DiscordCoreInternal::WebSocketConnectionAgent> pWebSocketConnectionAgent{ nullptr };
		static shared_ptr<DiscordCoreClientBase> thisPointerBase;
		shared_ptr<BotUser> currentUser{ nullptr };

		DiscordCoreClientBase() = default;

		void initialize(DiscordCoreInternal::HttpAgentResources agentResourcesNew, shared_ptr<DiscordCoreClient> discordCoreClientNew) {
			this->guildMembers = make_shared<GuildMemberManager>(agentResourcesNew, DiscordCoreInternal::ThreadManager::getThreadContext().get(), discordCoreClientNew);
			this->channels = make_shared<ChannelManager>(agentResourcesNew, DiscordCoreInternal::ThreadManager::getThreadContext().get(), discordCoreClientNew);
			this->roles = make_shared<RoleManager>(agentResourcesNew, DiscordCoreInternal::ThreadManager::getThreadContext().get(), discordCoreClientNew);
			this->users = make_shared<UserManager>(agentResourcesNew, DiscordCoreInternal::ThreadManager::getThreadContext().get(), discordCoreClientNew);
		}

		explicit DiscordCoreClientBase(DiscordCoreClientBase* dataPackage) {
			*this = *dataPackage;
		}

	protected:
		friend class Guild;
		friend class WebSocketConnectionAgent;
		friend class HttpRequestAgent;
		friend class ChannelStuff;
		friend class RoleStuff;
		friend class UserStuff;
		friend class InputEventManager;
		friend class DiscordCoreClient;
		friend class GuildMemberStuff;
		map<string, shared_ptr<unbounded_buffer<vector<RawFrame>>>> audioBuffersMap;
		shared_ptr<GuildMemberManager> guildMembers{ nullptr };
		shared_ptr<ChannelManager> channels{ nullptr };
		shared_ptr<RoleManager> roles{ nullptr };
		shared_ptr<UserManager> users{ nullptr };
		hstring botToken;
	};
	shared_ptr<DiscordCoreClientBase> DiscordCoreClientBase::thisPointerBase{ nullptr };
}
#endif
