// StickerEntities.hpp - Header for the sticker related classes and structs.
// May 13, 2021
// Chris M.
// https://github.com/RealTimeChris

#pragma once

#include "IndexInitial.hpp"
#include "FoundationEntities.hpp"

namespace DiscordCoreAPI {

	class alignas(hardware_destructive_interference_size) DiscordCoreAPI_Dll Sticker : public StickerData {
	protected:

		friend struct Concurrency::details::_ResultHolder<Sticker>;
		friend class Guild;

		Sticker();

		Sticker(StickerData dataNew);
	};

	class alignas(hardware_destructive_interference_size) DiscordCoreAPI_Dll Stickers {
	public:

	};
};
