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
		friend class DiscordCoreAPI::CoRoutine;
		friend class Guild;

		Sticker();

		Sticker(StickerData dataNew);
	};
};

namespace DiscordCoreInternal {

	class DiscordCoreAPI_Dll StickerManager {
	public:

		friend class DiscordCoreAPI::DiscordCoreClient;
		friend class DiscordCoreAPI::GuildMembers;
		friend class DiscordCoreAPI::Guild;

		StickerManager();
	};
};
#endif