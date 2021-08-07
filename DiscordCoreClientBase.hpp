// DiscordCoreClientBase.hpp - Header for the DiscordCoreClientBase class.
// Aug 7, 2021
// Chris M.
// https://github.com/RealTimeChris

#pragma once

#ifndef _DISCORDCORECLIENTBASE_
#define _DISCORDCORECLIENTBASE_

#include "../pch.h"
#include "ChannelStuff.hpp"
#include "MessageStuff.hpp"
#include "GuildMemberStuff.hpp"
#include "UserStuff.hpp"
#include "RoleStuff.hpp"

namespace DiscordCoreAPI {

	class DiscordCoreClientBase {
	public:
		shared_ptr<ChannelManager> channels{ nullptr };
		shared_ptr<GuildMemberManager> guildMembers{ nullptr };
		shared_ptr<RoleManager> roles{ nullptr };
		shared_ptr<UserManager> users{ nullptr };
		shared_ptr<DiscordCoreClientBase> thisPointer{ this };
		shared_ptr<BotUser> currentUser{ nullptr };
	protected:
		friend class Guild;
		friend class WebSocketConnectionAgent;
		friend class HttpRequestAgent;
		hstring botToken;
	};

}

#endif