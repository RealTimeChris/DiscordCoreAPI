// GuildScheduledEventEntities.hpp - Header for the Guild Scheduled Events stuff.
// Nov 27, 2021
// Chris M.
// https://github.com/RealTimeChris

#pragma once

#include "IndexInitial.hpp"
#include "FoundationEntities.hpp"

namespace DiscordCoreAPI {

	class DiscordCoreAPI_Dll GuildScheduledEvent : public GuildScheduledEventData {
	public:

	protected:
		GuildScheduledEvent(GuildScheduledEventData dataPackage);

		GuildScheduledEvent();

		virtual ~GuildScheduledEvent() {};

	};

	class DiscordCoreAPI_Dll GuildScheduledEvents {

	};
}
