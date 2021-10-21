// StickerEntities.hpp - Header for the sticker related classes and structs.
// May 13, 2021
// Chris M.
// https://github.com/RealTimeChris

#pragma once

#ifndef _STICKER_ENTITIES_
#define _STICKER_ENTITIES_

#include "IndexInitial.hpp"
#include "FoundationEntities.hpp"

namespace DiscordCoreAPI {

	class DiscordCoreAPI_Dll Sticker : public StickerData {
	protected:

		friend struct Concurrency::details::_ResultHolder<Sticker>;
		friend class DiscordCoreInternal::StickerManagerAgent;
		friend class DiscordCoreInternal::StickerManager;
		friend class Guild;

		Sticker();

		Sticker(StickerData dataNew);
	};
};

namespace DiscordCoreInternal {

	class DiscordCoreAPI_Dll StickerManagerAgent : ThreadContext, agent {
	protected:

		friend class DiscordCoreAPI::DiscordCoreClient;
		friend class DiscordCoreAPI::EventHandler;
		friend class StickerManager;

		static overwrite_buffer<map<string, DiscordCoreAPI::Sticker>> cache;

		StickerManagerAgent();

		void run();
	};

	class DiscordCoreAPI_Dll StickerManager : ThreadContext {
	public:

		friend class DiscordCoreAPI::DiscordCoreClient;
		friend class DiscordCoreAPI::GuildMembers;
		friend class DiscordCoreAPI::Guild;

		StickerManager(StickerManager* pointer);

		~StickerManager();
	};
};
#endif