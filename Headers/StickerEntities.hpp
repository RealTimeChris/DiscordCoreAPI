// StickerEntities.hpp - Header for the sticker related classes and structs.
// May 13, 2021
// Chris M.
// https://github.com/RealTimeChris

#pragma once

#ifndef _STICKER_ENTITIES_
#define _STICKER_ENTITIES_

#include "IndexInitial.hpp"
#include "FoundationEntities.hpp"
#include "CoRoutine.hpp"

namespace DiscordCoreAPI {

	class DiscordCoreAPI_Dll Sticker : public StickerData {
	protected:

		friend struct Concurrency::details::_ResultHolder<Sticker>;
		friend class DiscordCoreInternal::StickerManagerAgent;
		friend class DiscordCoreInternal::StickerManager;
		template<typename returnValueType>
		friend struct DiscordCoreAPI::CoRoutine;
		friend class Guild;

		Sticker();

		Sticker(StickerData dataNew);
	};
};

namespace DiscordCoreInternal {

	class DiscordCoreAPI_Dll StickerManagerAgent :agent {
	protected:

		friend class DiscordCoreAPI::DiscordCoreClient;
		friend class DiscordCoreAPI::EventHandler;
		friend class StickerManager;

		static overwrite_buffer<map<string, DiscordCoreAPI::Sticker>> cache;
		static shared_ptr<ThreadContext> threadContext;

		StickerManagerAgent();

		static void initialize();

		void run();
	};

	class DiscordCoreAPI_Dll StickerManager {
	public:

		friend class DiscordCoreAPI::DiscordCoreClient;
		friend class DiscordCoreAPI::GuildMembers;
		friend class DiscordCoreAPI::Guild;

		StickerManager(StickerManager* pointer);
	};
};
#endif