// StickerEntities.hpp - Header for the sticker related classes and structs.
// May 13, 2021
// Chris M.
// https://github.com/RealTimeChris

#pragma once

#include "IndexInitial.hpp"
#include "FoundationEntities.hpp"

namespace DiscordCoreAPI {

	class DiscordCoreAPI_Dll Sticker : public StickerData {
	public:
		friend struct Concurrency::details::_ResultHolder<Sticker>;
		friend class Guild;

		Sticker();

		virtual ~Sticker() {};

	protected:

		

		Sticker(StickerData dataNew);
	};

	class DiscordCoreAPI_Dll Stickers {
	public:

	};
};
