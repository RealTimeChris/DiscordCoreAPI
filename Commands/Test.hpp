// Test.hpp - Header for the "test" command.
// https://github.com/RealTimeChris

#pragma once

#ifndef _TEST_
#define _TEST_

#include "../DiscordCoreClient02.hpp"

namespace DiscordCoreAPI {

	class Test : public  BaseFunction {
	public:
		Test() {
			this->commandName = "test";
			this->helpDescription = "__**Test:**__ Enter !test or /test to run this command!";
		}

		Test* create() {
			return new Test;
		}

		virtual  task<void> execute(shared_ptr<BaseFunctionArguments> args) {
			InputEventManager::deleteInputEventResponseAsync(args->eventData);

			Guild guild = args->eventData.discordCoreClient->guilds->getGuildAsync({ .guildId = args->eventData.getGuildId() }).get();

			Guild guild = args->eventData.discordCoreClient->guilds->fetchAsync({ .guildId = args->eventData.getGuildId() }).get();

			co_return;

		}
	};
}
#endif