### **Getting a Guild Member:**
---
- Access the `GuildMemberS` class of the `DiscordCoreAPI` namespace.
- Select, from the `GuildMembers` class, the `getCachedGuildMemberAsync()` (which collects it from the cache), or `getGuildMember()` (which collects it from the Discord servers) function, while passing to it a completed data structure of the type `DiscordCoreAPI::GetGuildMemberData`.
- Call the function with `.get()` added to the end in order to wait for the results now.

```cpp
// Test.hpp - Header for the "test" command.
// https://github.com/RealTimeChris

#pragma once

#ifndef _TEST_
#define _TEST_

#include "Index.hpp"

namespace DiscordCoreAPI {

	class Test : public BaseFunction {
	public:
		Test() {
			this->commandName = "test";
			this->helpDescription = "Testing purposes!";
			EmbedData msgEmbed{};
			msgEmbed.setDescription("------\nSimply enter !test or /test!\n------");
			msgEmbed.setTitle("__**Test Usage:**__");
			msgEmbed.setTimeStamp(getTimeAndDate());
			msgEmbed.setColor("FeFeFe");
			this->helpEmbed = msgEmbed;
		}

		Test* create() {
			return new Test;
		}

		virtual CoRoutine<void> executeAsync(shared_ptr<DiscordCoreAPI::BaseFunctionArguments> args) {
			try {
				GetGuildMemberData dataPackage{};
				dataPackage.guildId = args->eventData.getGuildId();
				dataPackage.guildMemberId = args->eventData.getAuthorId();

				auto guildMember01 = GuildMembers::getCachedGuildMemberAsync(dataPackage).get();

				auto guildMember02 = GuildMembers::getGuildMemberAsync(dataPackage).get();

				co_return;
			}
			catch (...) {
				rethrowException("Test::executeAsync Error: ");
			}
		}
	};
}
#endif
```
