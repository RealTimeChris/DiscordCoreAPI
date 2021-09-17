// StickerStuff.hpp - Header for the guild member related stuff.
// May 13, 2021
// Chris M.
// https://github.com/RealTimeChris

#pragma once

#ifndef _STICKER_STUFF_
#define _STICKER_STUFF_

#include "../pch.h"
#include "FoundationEntities.hpp"

namespace DiscordCoreAPI {

	class Sticker : public StickerData {
	protected:

		friend struct Concurrency::details::_ResultHolder<Sticker>;
		friend class DiscordCoreInternal::StickerManagerAgent;
		friend class DiscordCoreInternal::StickerManager;
		friend struct GetGuildMemberRolesData;
		friend struct OnGuildMemberUpdateData;
		friend struct OnGuildMemberAddData;
		friend class DiscordCoreClient;
		friend class Guild;

		Sticker() {};

		Sticker(StickerData dataNew) {
			this->discordCoreClient = dataNew.discordCoreClient;
			this->description = dataNew.description;
			this->formatType = dataNew.formatType;
			this->sortValue = dataNew.sortValue;
			this->available = dataNew.available;
			this->guildId = dataNew.guildId;
			this->packId = dataNew.packId;
			this->asset = dataNew.asset;
			this->type = dataNew.type;
			this->name = dataNew.name;
			this->tags = dataNew.tags;
			this->user = dataNew.user;
			this->id = dataNew.id;
		}
	};
};

namespace DiscordCoreInternal {

	class StickerManagerAgent : agent {
	protected:

		friend class DiscordCoreAPI::DiscordCoreClient;
		friend class DiscordCoreAPI::EventHandler;
		friend class StickerManager;

		static shared_ptr<DiscordCoreAPI::DiscordCoreClient> discordCoreClient;
		static overwrite_buffer<map<string, DiscordCoreAPI::Sticker>> cache;
		static shared_ptr<ThreadContext> threadContext;
		static HttpAgentResources agentResources;

		StickerManagerAgent()
			:agent(*StickerManagerAgent::threadContext->scheduler->scheduler) {}

		static void intialize(HttpAgentResources agentResourcesNew, shared_ptr<DiscordCoreAPI::DiscordCoreClient> discordCoreClientNew) {
			StickerManagerAgent::threadContext = ThreadManager::getThreadContext().get();
			StickerManagerAgent::discordCoreClient = discordCoreClientNew;
			StickerManagerAgent::agentResources = agentResourcesNew;
		}

		static void cleanup() {
			StickerManagerAgent::threadContext->releaseGroup();
		}

		void run() {
			try {

			}
			catch (...) {
				DiscordCoreAPI::rethrowException("StickerManagerAgent::run() Error: ");
			}
			this->done();
		}
	};

	class StickerManager : ThreadContext {
	public:

		template <class _Ty>
		friend _CONSTEXPR20_DYNALLOC void std::_Destroy_in_place(_Ty& _Obj) noexcept;
		friend class DiscordCoreAPI::DiscordCoreClientBase;
		friend class DiscordCoreAPI::DiscordCoreClient;
		friend class DiscordCoreAPI::GuildMembers;
		friend class DiscordCoreAPI::Guild;

		StickerManager(StickerManager* pointer)
			: ThreadContext(*ThreadManager::getThreadContext().get()) {
			if (pointer != nullptr) {
				*this = *pointer;
			}
		}

	protected:

		shared_ptr<DiscordCoreAPI::DiscordCoreClient> discordCoreClient{ nullptr };
		shared_ptr<ThreadContext> threadContext{ nullptr };
		HttpAgentResources agentResources{};

		void initialize(HttpAgentResources agentResourcesNew, shared_ptr<DiscordCoreAPI::DiscordCoreClient> discordCoreClientNew) {
			this->discordCoreClient = discordCoreClientNew;
			this->agentResources = agentResourcesNew;
		}

		~StickerManager() {}
	};
	shared_ptr<DiscordCoreAPI::DiscordCoreClient> StickerManagerAgent::discordCoreClient{ nullptr };
	overwrite_buffer<map<string, DiscordCoreAPI::Sticker>> StickerManagerAgent::cache{ nullptr };
	shared_ptr<ThreadContext> StickerManagerAgent::threadContext{ nullptr };
	HttpAgentResources StickerManagerAgent::agentResources{};
	};
#endif