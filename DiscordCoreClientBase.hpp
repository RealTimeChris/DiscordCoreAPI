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
		shared_ptr<GuildMemberManager> guildMembers{ nullptr };
		shared_ptr<DiscordCoreClientBase> thisPointer{ this };
		shared_ptr<ChannelManager> channels{ nullptr };
		shared_ptr<BotUser> currentUser{ nullptr };
		shared_ptr<RoleManager> roles{ nullptr };
		shared_ptr<UserManager> users{ nullptr };

	protected:
		friend class Guild;
		friend class WebSocketConnectionAgent;
		friend class HttpRequestAgent;
		map<string, shared_ptr<unbounded_buffer<vector<RawFrame>>>> audioBuffersMap;
		hstring botToken;
	};

}
#endif
