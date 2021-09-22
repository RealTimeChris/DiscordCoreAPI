// DiscordCoreClientBase.hpp - Header for the DiscordCoreClientBase class.
// Aug 7, 2021
// Chris M.
// https://github.com/RealTimeChris

#pragma once

#ifndef _DISCORD_CORE_CLIENT_BASE_
#define _DISCORD_CORE_CLIENT_BASE_

#include "../pch.h"
#include "VoiceConnectionStuff.hpp"
#include "YouTubeStuff.hpp"
#include "SoundCloudStuff.hpp"
#include "UserStuff.hpp"
#include "RoleStuff.hpp"
#include "ChannelStuff.hpp"
#include "MessageStuff.hpp"
#include "GuildMemberStuff.hpp"
#include "WebSocketStuff.hpp"

namespace DiscordCoreAPI {

	class DiscordCoreClientBase {
	public:
		
		friend class DiscordCoreInternal::VoiceChannelWebSocketAgent;
		friend class DiscordCoreInternal::WebSocketConnectionAgent;
		friend class VoiceConnection;
		friend class InputEvents;
		friend class SongAPI;
		friend class Guild;

		static DiscordCoreClientBase* thisPointer;
		static BotUser currentUser;

		void initialize() {
			DiscordCoreClientBase::currentUser = BotUser(this->users->fetchCurrentUserAsync().get());
			DiscordCoreClientBase::currentUser.Initialize(DiscordCoreClientBase::webSocketConnectionAgent);
			DiscordCoreClientBase::thisPointer = this;
		}

	protected:
		static shared_ptr<DiscordCoreInternal::WebSocketConnectionAgent> webSocketConnectionAgent;
		static map<string, shared_ptr<unbounded_buffer<AudioFrameData>>>* audioBuffersMap;
		static map<string, shared_ptr<VoiceConnection>>* voiceConnectionMap;
		static map<string, shared_ptr<YouTubeAPI>>* youtubeAPIMap;
		static map<string, shared_ptr<SongAPI>>* songAPIMap;
		static map<string, shared_ptr<SoundCloudAPI>>* soundCloudAPIMap;
		shared_ptr<DiscordCoreInternal::GuildMemberManager> guildMembers{ nullptr };
		shared_ptr<DiscordCoreInternal::ChannelManager> channels{ nullptr };
		shared_ptr<DiscordCoreInternal::RoleManager> roles{ nullptr };
		shared_ptr<DiscordCoreInternal::UserManager> users{ nullptr };

	};
	map<string, shared_ptr<unbounded_buffer<AudioFrameData>>>* DiscordCoreClientBase::audioBuffersMap{ new map<string, shared_ptr<unbounded_buffer<AudioFrameData>>>() };
	map<string, shared_ptr<VoiceConnection>>* DiscordCoreClientBase::voiceConnectionMap{ new map<string, shared_ptr<VoiceConnection>>() };
	map<string, shared_ptr<SoundCloudAPI>>* DiscordCoreClientBase::soundCloudAPIMap{ new map<string, shared_ptr<SoundCloudAPI>>() };
	map<string, shared_ptr<YouTubeAPI>>* DiscordCoreClientBase::youtubeAPIMap{ new map<string, shared_ptr<YouTubeAPI>>() };
	shared_ptr<DiscordCoreInternal::WebSocketConnectionAgent> DiscordCoreClientBase::webSocketConnectionAgent{ nullptr };
	map<string, shared_ptr<SongAPI>>* DiscordCoreClientBase::songAPIMap{ new map<string, shared_ptr<SongAPI>>() };
	DiscordCoreClientBase* DiscordCoreClientBase::thisPointer{ nullptr };
	BotUser DiscordCoreClientBase::currentUser{ nullptr };
}
#endif
