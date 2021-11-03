// StickerEntities.hpp - Header for the sticker related classes and structs.
// May 13, 2021
// Chris M.
// https://github.com/RealTimeChris

#pragma once

#include "IndexInitial.hpp"
#include "FoundationEntities.hpp"

namespace DiscordCoreAPI {

	class DiscordCoreAPI_Dll Sticker : public StickerData {
	protected:

		friend struct Concurrency::details::_ResultHolder<Sticker>;
		friend class DiscordCoreInternal::StickerManager;
		friend class Guild;

		Sticker();

		Sticker(StickerData dataNew);
	};
};

namespace DiscordCoreInternal {

	class DiscordCoreAPI_Dll StickerManager {
	public:

	};
};
