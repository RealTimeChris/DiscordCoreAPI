// DiscordCoreClientBase.hpp - Header for the DiscordCoreClientBase class.
// Aug 7, 2021
// Chris M.
// https://github.com/RealTimeChris

#pragma once

#ifndef _DISCORD_CORE_CLIENT_BASE_
#define _DISCORD_CORE_CLIENT_BASE_

#include "../pch.h"
#include "VoiceConnection.hpp"
#include "YouTubeAPI.hpp"
#include "SoundCloudAPI.hpp"
#include "UserEntities.hpp"
#include "RoleEntities.hpp"
#include "ChannelEntities.hpp"
#include "MessageEntities.hpp"
#include "GuildMemberEntities.hpp"
#include "WebSocketEntities.hpp"
#include "DatabaseEntities.hpp"

namespace DiscordCoreAPI {

	class DiscordCoreClientBase {
	public:		

		friend class DiscordCoreInternal::VoiceChannelWebSocketAgent;
		friend class DiscordCoreInternal::WebSocketConnectionAgent;
		friend class VoiceConnection;
		friend class SoundCloudAPI;
		friend class InputEvents;
		friend class YouTubeAPI;
		friend class SongAPI;
		friend class Guild;

		static DiscordCoreClientBase* thisPointer;
		static BotUser currentUser;

		shared_ptr<DiscordCoreInternal::GuildMemberManager> guildMembers{ nullptr };
		shared_ptr<DiscordCoreInternal::ChannelManager> channels{ nullptr };
		shared_ptr<DiscordCoreInternal::RoleManager> roles{ nullptr };
		shared_ptr<DiscordCoreInternal::UserManager> users{ nullptr };

		void initialize(DiscordCoreClientBase* thisPtrNew) {
			DiscordCoreClientBase::thisPointer = thisPtrNew;
			DiscordCoreClientBase::currentUser = BotUser(this->users->fetchCurrentUserAsync().get());
			DiscordCoreClientBase::currentUser.Initialize(this->webSocketConnectionAgent);
		}

	protected:
		map<string, shared_ptr<unbounded_buffer<AudioFrameData>>>* audioBuffersMap{ new map<string, shared_ptr<unbounded_buffer<AudioFrameData>>>() };
		map<string, shared_ptr<VoiceConnection>>* voiceConnectionMap{ new map<string, shared_ptr<VoiceConnection>>() };
		map<string, shared_ptr<SoundCloudAPI>>* soundCloudAPIMap{ new map<string, shared_ptr<SoundCloudAPI>>() };
		map<string, shared_ptr<YouTubeAPI>>* youtubeAPIMap{ new map<string, shared_ptr<YouTubeAPI>>() };
		shared_ptr<DiscordCoreInternal::WebSocketConnectionAgent> webSocketConnectionAgent{ nullptr };
		map<string, shared_ptr<SongAPI>>* songAPIMap{ new map<string, shared_ptr<SongAPI>>() };
		map<string, DiscordGuild*>* discordGuildMap{ new map<string, DiscordGuild*>() };
	};
	DiscordCoreClientBase* DiscordCoreClientBase::thisPointer{ nullptr };
	BotUser DiscordCoreClientBase::currentUser{ nullptr };
}
#endif
