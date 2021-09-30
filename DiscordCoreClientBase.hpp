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

	namespace Statics{
		namespace {
			map<string, shared_ptr<unbounded_buffer<AudioFrameData>>>* audioBuffersMap{ new map<string, shared_ptr<unbounded_buffer<AudioFrameData>>>() };
			map<string, shared_ptr<VoiceConnection>>* voiceConnectionMap{ new map<string, shared_ptr<VoiceConnection>>() };
			map<string, shared_ptr<SoundCloudAPI>>* soundCloudAPIMap{ new map<string, shared_ptr<SoundCloudAPI>>() };
			map<string, shared_ptr<YouTubeAPI>>* youtubeAPIMap{ new map<string, shared_ptr<YouTubeAPI>>() };
			map<string, shared_ptr<SongAPI>>* songAPIMap{ new map<string, shared_ptr<SongAPI>>() };
			map<string, DiscordGuild*>* discordGuildMap{ new map<string, DiscordGuild*>() };
		}		
	}

	map<string, shared_ptr<unbounded_buffer<AudioFrameData>>>* getAudioBuffersMap() {
		return Statics::audioBuffersMap;
	}

	map<string, shared_ptr<VoiceConnection>>* getVoiceConnectionMap() {
		return Statics::voiceConnectionMap;
	}

	map<string, shared_ptr<SoundCloudAPI>>* getSoundCloudAPIMap() {
		return Statics::soundCloudAPIMap;
	}

	map<string, shared_ptr<YouTubeAPI>>* getYouTubeAPIMap() {
		return Statics::youtubeAPIMap;
	}

	map<string, shared_ptr<SongAPI>>* getSongAPIMap() {
		return Statics::songAPIMap;
	}

	map<string, DiscordGuild*>* getDiscordGuildMap() {
		return Statics::discordGuildMap;
	}

	class DiscordCoreClientBase {
	public:

		friend class DiscordCoreInternal::VoiceChannelWebSocketAgent;
		friend class DiscordCoreInternal::WebSocketConnectionAgent;
		friend class VoiceConnection;
		friend BotUser getBotData();
		friend class SoundCloudAPI;
		friend class InputEvents;
		friend class YouTubeAPI;
		friend class SongAPI;
		friend class Guild;

		static DiscordCoreClientBase* thisPointer;
		shared_ptr<DiscordCoreInternal::GuildMemberManager> guildMembers{ nullptr };
		shared_ptr<DiscordCoreInternal::ChannelManager> channels{ nullptr };
		shared_ptr<DiscordCoreInternal::RoleManager> roles{ nullptr };
		shared_ptr<DiscordCoreInternal::UserManager> users{ nullptr };

		void initialize() {
			DiscordCoreClientBase::thisPointer = this;
			DiscordCoreClientBase::currentUser = BotUser(this->users->fetchCurrentUserAsync().get());
			DiscordCoreClientBase::currentUser.Initialize(this->webSocketConnectionAgent);
		}

	protected:
		static BotUser currentUser;

		shared_ptr<DiscordCoreInternal::WebSocketConnectionAgent> webSocketConnectionAgent{ nullptr };
		
	};
	DiscordCoreClientBase* DiscordCoreClientBase::thisPointer{ nullptr };
	BotUser DiscordCoreClientBase::currentUser{ nullptr };
	
}
#endif
