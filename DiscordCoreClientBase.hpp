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
#include "WebSocketStuff.hpp"

namespace DiscordCoreAPI {

	class DiscordCoreClientBase {
	public:
		static shared_ptr<DiscordCoreClientBase> thisPointerBase;
		shared_ptr<BotUser> currentUser{ nullptr };

		void initialize(DiscordCoreInternal::HttpAgentResources agentResourcesNew, shared_ptr<DiscordCoreClient> discordCoreClientNew, shared_ptr<DiscordCoreInternal::WebSocketConnectionAgent> pWebSocketConnectionAgentNew) {
			this->guildMembers = make_shared<GuildMemberManager>(agentResourcesNew, DiscordCoreInternal::ThreadManager::getThreadContext().get(), discordCoreClientNew);
			this->channels = make_shared<ChannelManager>(agentResourcesNew, DiscordCoreInternal::ThreadManager::getThreadContext().get(), discordCoreClientNew);
			this->roles = make_shared<RoleManager>(agentResourcesNew, DiscordCoreInternal::ThreadManager::getThreadContext().get(), discordCoreClientNew);
			this->users = make_shared<UserManager>(agentResourcesNew, DiscordCoreInternal::ThreadManager::getThreadContext().get(), discordCoreClientNew);
			this->currentUser = make_shared<BotUser>(this->users->fetchCurrentUserAsync().get().data, discordCoreClientNew, pWebSocketConnectionAgentNew);
			DiscordCoreClientBase::pWebSocketConnectionAgent = pWebSocketConnectionAgentNew;
		}

		DiscordCoreClientBase(DiscordCoreClientBase* dataPackage) {
			*this = *dataPackage;
		}

	protected:
		friend class Guild;
		friend class WebSocketConnectionAgent;
		friend class YouTubeAPI;
		friend class VoiceConnection;
		friend class HttpRequestAgent;
		friend class ChannelStuff;
		friend class RoleStuff;
		friend class UserStuff;
		friend class InputEvents;
		friend class DiscordCoreClient;
		friend class GuildMembers;
		static shared_ptr<DiscordCoreInternal::WebSocketConnectionAgent> pWebSocketConnectionAgent;
		static map<string, shared_ptr<unbounded_buffer<AudioFrameData*>>> audioBuffersMap;
		shared_ptr<GuildMemberManager> guildMembers{ nullptr };
		shared_ptr<ChannelManager> channels{ nullptr };
		shared_ptr<RoleManager> roles{ nullptr };
		shared_ptr<UserManager> users{ nullptr };
		hstring botToken;
	};
	map<string, shared_ptr<unbounded_buffer<AudioFrameData*>>> DiscordCoreClientBase::audioBuffersMap;
	shared_ptr<DiscordCoreClientBase> DiscordCoreClientBase::thisPointerBase{ nullptr };
	shared_ptr<DiscordCoreInternal::WebSocketConnectionAgent> DiscordCoreClientBase::pWebSocketConnectionAgent{ nullptr };
}
#endif
