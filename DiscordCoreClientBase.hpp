// DiscordCoreClientBase.hpp - Header for the DiscordCoreClientBase class.
// Aug 7, 2021
// Chris M.
// https://github.com/RealTimeChris

#pragma once

#ifndef _DISCORD_CORE_CLIENT_BASE_
#define _DISCORD_CORE_CLIENT_BASE_

#include "../pch.h"
#include "VoiceConnectionStuff.hpp"
#include "UserStuff.hpp"
#include "RoleStuff.hpp"
#include "ChannelStuff.hpp"
#include "MessageStuff.hpp"
#include "YouTubeStuff.hpp"
#include "GuildMemberStuff.hpp"
#include "WebSocketStuff.hpp"

namespace DiscordCoreAPI {

	class DiscordCoreClientBase {
	public:

		friend class DiscordCoreClient;
		friend class VoiceConnection;
		friend class YouTubeAPICore;
		friend class InputEvents;
		friend class YouTubeAPI;
		friend class Guild;

		static DiscordCoreClientBase* thisPointer;
		static BotUser currentUser;

		void initialize(DiscordCoreInternal::HttpAgentResources agentResourcesNew, shared_ptr<DiscordCoreClient> discordCoreClient) {
			this->guildMembers = make_shared<DiscordCoreInternal::GuildMemberManager>(nullptr);
			this->guildMembers->initialize(agentResourcesNew, DiscordCoreInternal::ThreadManager::getThreadContext().get(), discordCoreClient);
			this->channels = make_shared<DiscordCoreInternal::ChannelManager>(nullptr);
			this->channels->initialize(agentResourcesNew, DiscordCoreInternal::ThreadManager::getThreadContext().get(), discordCoreClient);
			this->roles = make_shared<DiscordCoreInternal::RoleManager>(nullptr);
			this->roles->initialize(agentResourcesNew, DiscordCoreInternal::ThreadManager::getThreadContext().get(), discordCoreClient);
			this->users = make_shared<DiscordCoreInternal::UserManager>(nullptr);
			this->users->initialize(agentResourcesNew, DiscordCoreInternal::ThreadManager::getThreadContext().get(), discordCoreClient);
			DiscordCoreClientBase::currentUser = BotUser(this->users->fetchCurrentUserAsync().get());
			DiscordCoreClientBase::currentUser.Initialize(DiscordCoreClientBase::webSocketConnectionAgent);
			DiscordCoreClientBase::thisPointer = this;
		}

	protected:
		static shared_ptr<DiscordCoreInternal::WebSocketConnectionAgent> webSocketConnectionAgent;
		static map<string, shared_ptr<unbounded_buffer<AudioFrameData>>*>* audioBuffersMap;
		static map<string, shared_ptr<VoiceConnection>>* voiceConnectionMap;
		static map<string, shared_ptr<YouTubeAPICore>>* youtubeAPIMap;
		static map<string, Playlist>* guildYouTubeQueueMap;
		shared_ptr<DiscordCoreInternal::GuildMemberManager> guildMembers{ nullptr };
		shared_ptr<DiscordCoreInternal::ChannelManager> channels{ nullptr };
		shared_ptr<DiscordCoreInternal::RoleManager> roles{ nullptr };
		shared_ptr<DiscordCoreInternal::UserManager> users{ nullptr };

	};
	map<string, shared_ptr<unbounded_buffer<AudioFrameData>>*>* DiscordCoreClientBase::audioBuffersMap{ new map<string, shared_ptr<unbounded_buffer<AudioFrameData>>*>() };
	map<string, shared_ptr<VoiceConnection>>* DiscordCoreClientBase::voiceConnectionMap{ new map<string, shared_ptr<VoiceConnection>>() };
	map<string, shared_ptr<YouTubeAPICore>>* DiscordCoreClientBase::youtubeAPIMap{ new map<string, shared_ptr<YouTubeAPICore>>() };
	shared_ptr<DiscordCoreInternal::WebSocketConnectionAgent> DiscordCoreClientBase::webSocketConnectionAgent{ nullptr };
	map<string, Playlist>* DiscordCoreClientBase::guildYouTubeQueueMap{ new map<string, Playlist>() };
	DiscordCoreClientBase* DiscordCoreClientBase::thisPointer{ nullptr };
	BotUser DiscordCoreClientBase::currentUser{ nullptr };
}
#endif
